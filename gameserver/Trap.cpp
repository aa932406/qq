#include "stdafx.h"

#include "Map.h"
#include "GameService.h"
#include "MapManager.h"
#include "Player.h"
#include "Trap.h"
#include "DBService.h"
#include "ActivityMap.h"
#include "Dungeon.h"

using namespace Answer;

Trap::Trap()
	: Entity(ET_TRAP), m_pMap(NULL), m_user(0), m_state(TS_STANDBY), m_stateTick(0), m_liftTime(0)
{

}

Trap::~Trap()
{

}

void Trap::init( Map *pMap, Position pos, const CfgTrap &cfgTrap )
{
	if ( pMap == NULL )
	{
		return;
	}
	m_pMap = pMap;
	m_pos = pos;
	m_cfgTrap = cfgTrap;

	m_user = 0;
	m_state = TS_STANDBY;
	m_stateTick = m_pMap->getTick();
	if ( m_cfgTrap.life > 0 )
	{
		m_liftTime = m_pMap->getNow() + m_cfgTrap.life;
	}
}

void Trap::reset()
{
	m_pMap = NULL;
	m_pos = Position(0, 0);

	m_user = 0;
	m_state = TS_STANDBY;
	m_stateTick = 0;
	m_liftTime = 0;
}

void Trap::checkState()
{
	if (m_pMap == NULL)
	{
		return;
	}

	if ( m_liftTime > 0 && m_state != TS_DIE && m_pMap->getNow() > m_liftTime )
	{
		setState(TS_DIE);
		return;
	}

	switch (m_state)
	{
	case TS_EFFECT:
		if (m_pMap->getTick() - m_stateTick >= m_cfgTrap.delay)
		{
			effect();
		}
		break;
	case TS_COOLDOWN:
		if (m_pMap->getTick() - m_stateTick >= m_cfgTrap.cd)
		{
			setState(TS_STANDBY);
		}
		break;
	default:
		break;
	}
}

int32_t Trap::onUse(Player *player)
{
	if (m_pMap == NULL || player == NULL)
	{
		return ERR_INVALID_DATA;
	}

	if (m_state != TS_STANDBY)
	{
		return ERR_GENERAL_CD;
	}

	if (m_cfgTrap.item_cost != 0 && !player->hasItem(m_cfgTrap.item_cost, IC_NORMAL, 1))
	{
		return ERR_INVALID_DATA;
	}

	if ( (m_cfgTrap.event_type == TT_TRANSFER_TEAM || m_cfgTrap.event_type == TT_CHANGE_DUNGEON) && player->IsTeamLeader() )
	{
		return ERR_INVALID_DATA;
	}

	setState(TS_EFFECT);
	m_user = player->getCid();

	return ERR_OK;
}

bool Trap::appendInfo(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return false;
	}
	inPacket->writeInt8(static_cast<int8_t>(getType()));
	inPacket->writeInt64(getEntityId());
	inPacket->writeInt32(m_cfgTrap.id);
	inPacket->writeInt16(static_cast<int16_t>(m_pos.x));
	inPacket->writeInt16(static_cast<int16_t>(m_pos.y));
	return true;
}

void Trap::leaveMap()
{
	broadcastLeaveMap();
}

void Trap::broadcastState()
{
	if (m_pMap == NULL)
	{
		return;
	}

	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_TRAP_STATE);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(getEntityId());
	packet->writeInt32(m_state);
	packet->setSize(packet->getWOffset());
	m_pMap->broadcast(packet);
}

void Trap::broadcastLeaveMap()
{
	if (m_pMap == NULL)
	{
		return;
	}

	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_REMOVE_TRAP);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(getEntityId());
	packet->setSize(packet->getWOffset());
	m_pMap->broadcast(packet);
}

void Trap::setState(TrapState state)
{
	if (m_pMap == NULL)
	{
		return;
	}

	m_state = state;
	m_stateTick = m_pMap->getTick();

	if (m_state == TS_DIE)
	{
		broadcastLeaveMap();
	}
	else
	{
		broadcastState();
	}
}

void Trap::effect()
{
	if (m_pMap == NULL)
	{
		return;
	}

	Player *player = GAME_SERVICE.getPlayer( m_user, m_pMap->GetRunnerId() );
	if ( player == NULL )
	{
		return;
	}

	switch ( m_cfgTrap.event_type )
	{
	case TT_BUFF: 
		break;
	case TT_MONSTER:
	case TT_DELAY_MONSTER:
		if ( m_pMap->IsActivityMap() )
		{
			CActivityMap* pAactivityMap = dynamic_cast<CActivityMap*>( m_pMap );
			if ( pAactivityMap != NULL )
			{
				pAactivityMap->generateMonster( m_cfgTrap.effect );
			}
		}
		else if ( m_pMap->IsDungeonMap() )
		{
			Dungeon* pDungeon = dynamic_cast<Dungeon*>( m_pMap );
			if ( pDungeon != NULL )
			{
				pDungeon->generateMonster( m_cfgTrap.effect );
			}
		}
		setState(TS_DIE);
		break;
	case TT_TRANSFER: 
		{
			StringVector strPos = StringUtility::split(m_cfgTrap.effect, ":");
			if (strPos.size() == 2)
			{
				int32_t x = atoi(strPos[0].c_str());
				int32_t y = atoi(strPos[1].c_str());
				player->instantMove(x, y, IMR_TRANSFER);
			
				setState(TS_COOLDOWN);
			}
		}
		break;
	case TT_TRANSFER_TEAM:
		{
			if ( m_pMap->IsDungeonMap() )
			{
				Dungeon* pDungeon = dynamic_cast<Dungeon*>( m_pMap );
				if ( pDungeon != NULL )
				{
					StringVector strPos = StringUtility::split(m_cfgTrap.effect, ":");
					if (strPos.size() == 2)
					{
						int32_t x = atoi(strPos[0].c_str());
						int32_t y = atoi(strPos[1].c_str());

						PlayerList players = pDungeon->getPlayers();
						for (PlayerList::iterator it = players.begin(); it != players.end(); ++it)
						{
							(*it)->instantMove(x, y, IMR_TRANSFER);
						}
						setState(TS_DIE);
					}
				}
			}
		}
		break;
	case TT_CHANGE_DUNGEON:
		{
			if ( m_pMap->IsDungeonMap() )
			{
				Dungeon* pDungeon = dynamic_cast<Dungeon*>( m_pMap );
				if ( pDungeon != NULL )
				{
					int32_t dungeonId = atoi(m_cfgTrap.effect.c_str());
					CfgDungeon *pCfgDungeon = CFG_DATA.getDungeon(dungeonId);
					if ( pCfgDungeon != NULL )
					{
						Dungeon* pNewDungeon = MAP_MANAGER.NewDungeon( dungeonId );
						pNewDungeon->start();
						if ( pNewDungeon != NULL )
						{
							PlayerList players = pDungeon->getPlayers();
							for (PlayerList::iterator it = players.begin(); it != players.end(); ++it)
							{
								Player *player = *it;
								if (player != NULL)
								{
									player->leaveDungeon();
									player->setOldPosition();
									player->broadcastLeave();
									player->getMap()->removePlayer(player, false);
									GAME_SERVICE.replySuccess( player->getGateIndex(), CM_ENTER_DUNGEON, pCfgDungeon->id );
									pNewDungeon->addPlayer(player, pCfgDungeon->x, pCfgDungeon->y);	
									MAP_MANAGER.PostMsg( pNewDungeon->GetRunnerId(), GMC_ADD_DUNGEON, pNewDungeon );

									LogDungeon logDungeon = {};
									logDungeon.cid = player->getCid();
									logDungeon.did = dungeonId;
									logDungeon.type = pNewDungeon->getDungeonType();
									logDungeon.level = player->getLevel();
									logDungeon.start_time = pNewDungeon->GetStartTime();
									DB_SERVICE.insertDungeon( logDungeon );
								}
							}
						}
					}
				}
			}
			setState(TS_DIE);
		}
		break;
	case TT_ADD_DUNGEON_TIME:
		{
			if ( m_pMap->IsDungeonMap() )
			{
				Dungeon* pDungeon = dynamic_cast<Dungeon*>( m_pMap );
				if ( pDungeon != NULL )
				{
					pDungeon->AddDungeonTime( atoi( m_cfgTrap.effect.c_str() ) );
				}
			}
			setState(TS_DIE);
		}
		break;
	default:
		break;
	}
}

