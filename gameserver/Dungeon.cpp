#include "stdafx.h"

#include "Dungeon.h"
#include "GameService.h"
#include "MapManager.h"
#include "MonsterDungeon.h"
#include "PathFinder.h"
#include "Trap.h"
#include <algorithm>
#include "pthread.h"
#include <iostream>
#include "DungeonBuff.h"
#include "PlantDungeon.h"
#include "DBService.h"
#include "MonsterBuff.h"
#include "PoolManager.h"

using namespace Answer;

#define FIRST_DUNGEON_ID 10002		//第一个副本
Dungeon::Dungeon()
	: m_state(DS_FREE), m_nId(0), m_nStartTime(0)
{
	bzero(&m_finshInfo,sizeof(m_finshInfo));
}

Dungeon::~Dungeon()
{

}

bool Dungeon::IsDungeonMap() const
{
	return true;
}

bool Dungeon::CanUseXP() const
{
	if ( getDungeonType() == DT_MONEY || m_cfgDungeon.id == FIRST_DUNGEON_ID )
	{
		return true;
	}
	return false;
}

bool Dungeon::IsFullXP() const
{
	return getDungeonType() == DT_MONEY || m_cfgDungeon.id == FIRST_DUNGEON_ID;
}

bool Dungeon::CanUsePet() const
{
	return getDungeonType() != DT_TD;
}

int32_t Dungeon::GetId() const
{
	return m_nId;
}

int32_t Dungeon::GetStartTime() const
{
	return m_nStartTime;
}

void Dungeon::init( const CfgDungeon &cfgDungeon, const CfgMap &cfgmap, int32_t nId )
{
	Map::init( cfgmap );
	IMapEvent::init( CFG_DATA.getDungeonEvent( cfgDungeon.id ) );

	m_cfgDungeon = cfgDungeon;
	m_state = DS_FREE;
	m_nId = nId;
}

void Dungeon::addPlayer( Player *player, int32_t x, int32_t y )
{
	if ( NULL == player )
	{
		return;
	}
	Map::addPlayer( player, x, y );
	initPlayerInfo( player );
	broadcastDungeonInfo();
	if ( m_cfgDungeon.type == DT_TEAM )
	{
		DamageSumList::iterator iter = m_damages.begin();
		DamageSumList::iterator eiter = m_damages.end();
		for ( ; iter != eiter; ++iter )
		{
			if ( iter->cid == player->getCid() )
			{
				break;
			}
		}
		if ( iter == eiter )
		{
			DamageSum dmg;
			dmg.cid = player->getCid();
			dmg.name = player->getName();
			dmg.damage = 0;
			m_damages.push_back( dmg );
		}
		m_needBroadcastDamage = true;
	}
	else if ( m_cfgDungeon.type == DT_MY_WORLD )
	{
		if ( m_cfgDungeon.star > 11 )
		{
			Answer::NetPacket *packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM);
			if (NULL == packet)
			{
				return;
			}
			packet->writeInt32( MO_YU_WORLD_DUNGEON_GONG_GAO );
			packet->writeUTF8(  player->getName() );
			packet->writeInt64( player->getCid());
			packet->writeInt32( m_cfgDungeon.star );
			packet->setSize(packet->getWOffset());
			GAME_SERVICE.worldBroadcast(packet);
		}
	}
	else if ( m_cfgDungeon.type == DT_MONEY || m_cfgDungeon.id == FIRST_DUNGEON_ID )
	{
		player->GetCharSkill().FillXP();
	}
	else if ( m_cfgDungeon.type == DT_TD )
	{
		
	}
// 	else if ( m_cfgDungeon.id == FIRST_DUNGEON_ID ) // 第一次副本引导xp
// 	{
// 		if ( player->GetCharSkill().IsLearnedXPSkill() )
// 		{
// 			if ( !player->GetCharSkill().IsInXP() )
// 			{
// 				player->GetCharSkill().sendSocialAddXP( MAX_XP_POINTS );
// 			}
// 		}
// 	}
	
}

//void Dungeon::initGift()
//{
//	CfgDungeonReward*pReward = CFG_DATA.getDungeonReward(m_cfgDungeon.id);
//	if (pReward != NULL)
//	{
//		CfgDungeonDropGroupVector*pGroup = CFG_DATA.getDungeonDropGroup(pReward->groupID);
//		if (pGroup != NULL)
//		{
//			createGift(pGroup);
//		}
//		else
//		{
//			LOG_DUNGEON("dungeon is not pReward id %d\n",m_cfgDungeon.id);
//		}
//	}
//}

void Dungeon::update()
{
	//if (m_state != DS_FREE && m_players.empty())
	//{
	//	reset();
	//}
	if(m_state == DS_FREE) 
	{
		Map::update();
		return;
	}

	int64_t curTick = getTick();

	switch (m_state)
	{
	case DS_FREE:
		break;
	case DS_WAIT:
		{
			if ( curTick - m_stateTick >= m_stateParam*1000 )
			{
				if ( m_cfgDungeon.type == DT_TEAM )
				{
					teamDungeonMemberEnter();
				}
				setState( DS_RUNNING );	// 准备时间结束
			}
		}
		break;
	case DS_RUNNING:
		{
			if ( curTick - m_stateTick >= GetDurationTime() * 1000 )
			{
				clearMonster();
				if ( getDungeonType() == DT_MONEY )
				{
					setCanSubmit();
					broadcastFinishDungon( 15 );
					signDungeonCompleted();
					setState( DS_WIN, 15 );
				}
				else
				{
					setState(DS_FAIL, m_stateParam);
				}
			}
			else
			{
				checkAddMonster( curTick );
				checkEvents();
			}
		}
		break;
	case DS_WIN:
		{
			if ( curTick - m_stateTick >= m_cfgDungeon.reward_time * 1000)
			{
				if ( !m_players.empty() )
				{
					PlayerList::iterator iter = m_players.begin();
					PlayerList::iterator eiter = m_players.end();
					for ( ; iter != eiter; ++iter )
					{
						GetReward( *iter );
					}
				}
				setState(DS_FAIL, m_stateParam);
			}
		}
		break;
	case DS_FAIL:
		{
			if ( curTick - m_stateTick >= 15 * 1000)
			{
				setState( DS_RESET );
			}
			//PlayerList players = m_players;
			//for (PlayerList::iterator it = players.begin(); it != players.end(); ++it)
			//{
			//	Player *player = *it;
			//	if (player != NULL)
			//	{
			//		player->leaveDungeon();
			//		sendDungeonResult(player, m_stateParam);
			//	}
			//}
		}
		break;
	case DS_RESET:
		{
			if ( !m_players.empty() )
			{
				PlayerList players = m_players;
				for (PlayerList::iterator it = players.begin(); it != players.end(); ++it)
				{
					Player *player = *it;
					if (player != NULL)
					{
						player->leaveDungeon();
						sendDungeonResult(player, m_stateParam);
					}
				}
			}
			reset();
			MAP_MANAGER.PostMsg( GetRunnerId(), GMC_DEL_DUNGEON, this );
		}
		break;
	default:
		break;
	}

	checkRevive( curTick );
	checkBroadcastDamageList();

	Map::update();
}

void Dungeon::reset()
{
	m_players.clear();

	{
		for (MonsterList::iterator it = m_monsters.begin(); it != m_monsters.end(); ++it)
		{
			MonsterDungeon *monster = dynamic_cast<MonsterDungeon*>(*it);
			if ( monster != NULL )
			{
				GAME_SERVICE.removeMonster(monster);
				POOL_MANAGER.push<MonsterDungeon>( monster );
			}
		}
		m_monsters.clear();
	}

	{
		for (PlantList::iterator it = m_plants.begin(); it != m_plants.end(); ++it)
		{
			PlantDungeon *plant = dynamic_cast<PlantDungeon*>(*it);
			if ( plant != NULL )
			{
				POOL_MANAGER.push<PlantDungeon>(plant);
			}
		}
		m_plants.clear();
	}

	{
		for (DropItemGroupList::iterator it = m_dropItems.begin(); it != m_dropItems.end(); ++it)
		{
			CDropItemGroup* pDropItemGroup = *it;
			if ( pDropItemGroup != NULL )
			{
				POOL_MANAGER.push<CDropItemGroup>( pDropItemGroup );
			}
		}
		m_dropItems.clear();
	}

	{
		for (TrapList::iterator it = m_traps.begin(); it != m_traps.end(); ++it)
		{
			POOL_MANAGER.push<Trap>( *it );
		}
		m_traps.clear();
	}

	IMapEvent::clear();

	bzero(&m_finshInfo,sizeof(m_finshInfo));
	m_monsterWave.clear();
	m_state = DS_FREE;
	m_stateTick = getTick();
	m_stateParam = 0;
	m_buildTower.clear();
	m_waitTower.clear();
	m_waitMonster.clear();
	m_playerReward.clear();
	m_memberList.clear();
	m_damages.clear();
	m_needBroadcastDamage = false;
	m_dRatio = 1;
	m_nAddonTime = 0;
	m_nStartTime = 0;
	m_lstStack.clear();
}

void Dungeon::checkEvents()
{
	for (CfgMapEventList::iterator it = m_events.begin(); it != m_events.end(); ++it)
	{
		if ( !it->bOpen || it->bDone)
		{
			continue;
		}
	
		if ( it->trigger_type == DETT_TIME_LINE 
			|| it->trigger_type == DETT_MONSTER_WAVE_TIMELINE 
			|| it->trigger_type == DETT_ARRIVE_AREA 
			|| it->trigger_type == DETT_MULTI_MONSTER_WAVE )
		{
			checkEvent( *it );
		}
	}
}

void Dungeon::checkEvent( CfgMapEvent &mapEvent )
{
	if ( !mapEvent.bOpen || mapEvent.bDone )
	{
		return;
	}

	switch ( mapEvent.trigger_type )
	{
	case DETT_TIME_LINE:
		{
			if ( mapEvent.trigger_param.size() == 1 )
			{
				if ( getTick() - m_stateTick >= mapEvent.trigger_param[0]*1000 )
				{
					triggerEvent( mapEvent );
				}
			}
		}		
		break;
	case DETT_MONSTER_WAVE:
		{
			if ( mapEvent.trigger_param.size() == 1 )
			{
				Int32MonsterWaveMap::iterator it = m_monsterWave.find( mapEvent.trigger_param[0] );
				if ( it != m_monsterWave.end() && !it->second.alive() )
				{
					triggerEvent( mapEvent );
				}
			}
		}
		break;
	case DETT_MULTI_MONSTER_WAVE:
		{
			if ( mapEvent.trigger_param.size() > 1 )
			{
				int32_t nsize = mapEvent.trigger_param.size();
				if ( nsize > 0 )
				{
					int32_t  count  = 0;
					for ( int32_t i = 0; i < count; ++i )
					{
						Int32MonsterWaveMap::iterator it = m_monsterWave.find(mapEvent.trigger_param[i]);
						if ( it != m_monsterWave.end() && !it->second.alive() )
						{
							++count;
						}
					}

					if ( count == nsize )
					{
						triggerEvent( mapEvent );
					}
				}
			}
		}
		break;
	case DETT_MONSTER_WAVE_TIMELINE:
		{
			if (mapEvent.trigger_param.size() == 2)
			{
				Int32MonsterWaveMap::iterator it = m_monsterWave.find(mapEvent.trigger_param[0]);
				if ( it != m_monsterWave.end() )
				{
					if ( !( it->second.alive() || getTick() - it->second.startTime > mapEvent.trigger_param[1]*1000 ) )
					{
						triggerEvent( mapEvent );
					}
				}
			}			
		}
		break;
	case  DETT_ARRIVE_AREA:
		{
			if( mapEvent.trigger_param.size() == 4 )
			{
				 Position ps1( mapEvent.trigger_param[0], mapEvent.trigger_param[1] );
				 Position ps2( mapEvent.trigger_param[2], mapEvent.trigger_param[3] );
				 for ( PlayerList::iterator it = m_players.begin();it != m_players.end();++it )
				 {
					 Player* pPlayer = *it;
					 if ( pPlayer != NULL && pPlayer->isInRectangle( ps1, ps2 ) )
					 {
						 triggerEvent( mapEvent, pPlayer );
					 }
				 }
			}
		}
		break;
	default:
		break;
	}
}

void Dungeon::SendZSZZDungeonInfoToGlobal()
{
	PlayerList::iterator it = m_players.begin();
	for ( ; it != m_players.end(); ++it )
	{
		NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_PROC, IM_SOCIAL_UPDATE_ZSZZ);
		if ( NULL == packet )
		{
			return;
		}
//		packet->writeInt32( (*it)->getGateIndex() );
		packet->writeInt64( (*it)->getCid() );
		packet->writeUTF8( (*it)->getName() );
		packet->writeInt32( getDungeonId() );
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.sendPacket(packet);

		(*it)->GetOperateLimit().UpdateLimitCount( PR_LAST_FINISH_UP_TOWER_DUNGEON, getDungeonId() );
	}
}

void Dungeon::triggerEvent( CfgMapEvent &mapEvent, Unit* pUnit )
{
	switch (mapEvent.event_type)
	{
	case DEET_WIN:
		{
			int32_t param = atoi(mapEvent.effect.c_str());
			setCanSubmit();
			clearMonster();
			broadcastFinishDungon(param);
			signDungeonCompleted();
			setState(DS_WIN, param);
			if ( getDungeonType() == DT_UP_TOWER )
			{
				SendZSZZDungeonInfoToGlobal();
				if ( m_cfgDungeon.star <= 30 )
				{
					if ( m_cfgDungeon.star % 5 == 0 )
					{
						PlayerList::iterator it = m_players.begin();
						for ( ; it != m_players.end(); ++it )
						{
							DungeonGongGao( (*it)->getCid(), (*it)->getName() );
						}
					}
				}
				else
				{
					PlayerList::iterator it = m_players.begin();
					for ( ; it != m_players.end(); ++it )
					{
						DungeonGongGao( (*it)->getCid(), (*it)->getName() );
					}
				}

				PlayerList::iterator it = m_players.begin();
				for ( ; it != m_players.end(); ++it )
				{
					(*it)->RecalcAttr();
				}
			}
		}
		break;
	case DEET_FAIL:
		{
			clearMonster();
			setState(DS_FAIL, atoi(mapEvent.effect.c_str()));
		}
		break;
	case DEET_ADD_MONSTER:
		{
			bool rewardWave = false;
			if ( mapEvent.trigger_type == DETT_MONSTER_WAVE_TIMELINE )
			{
				rewardWave = true;
			}

			generateMonster( mapEvent.effect, rewardWave );
		}
		break;
	case DEET_ADD_TRAP:
		{
			StringVector strIds = StringUtility::split(mapEvent.effect, ":");
			for (StringVector::iterator it = strIds.begin(); it != strIds.end(); ++it)
			{
				CfgDungeonTrap *pCfgDungeonTrap = CFG_DATA.getDungeonTrap(atoi(it->c_str()));
				if (pCfgDungeonTrap != NULL)
				{
					CfgTrap *pCfgTrap = CFG_DATA.getTrap(pCfgDungeonTrap->tid);
					if (pCfgTrap != NULL)
					{
						Position trapPos(pCfgDungeonTrap->x, pCfgDungeonTrap->y);
						if ( pUnit != NULL )
						{
							trapPos = pUnit->getCurrentTile();
						}
						if ( trapPos.x > 0 && trapPos.y > 0 )
						{
							Trap *pTrap = POOL_MANAGER.pop<Trap>();
							if (NULL == pTrap)
							{
								continue;
							}
							pTrap->init(this, trapPos, *pCfgTrap);
							addTrap(pTrap);
						}
					}
				}
			}
		}
		break;
	case DEET_ADD_PLANT:
		{
			StringVector strIds = StringUtility::split(mapEvent.effect, ":");
			for (StringVector::iterator it = strIds.begin(); it != strIds.end(); ++it)
			{
				CfgDungeonPlant *pCfgDungeonPlant = CFG_DATA.getDungeonPlant(atoi(it->c_str()));
				if (pCfgDungeonPlant != NULL)
				{
					CfgPlant *pCfgPlant = CFG_DATA.getPlant(pCfgDungeonPlant->pid);
					if (pCfgPlant != NULL)
					{
						Position plantPos(pCfgDungeonPlant->x, pCfgDungeonPlant->y);
						if ( pUnit != NULL )
						{
							plantPos = pUnit->getCurrentTile();
						}
						if ( plantPos.x > 0 && plantPos.y > 0 )
						{
							CfgMapPlant mapPlant;
							mapPlant.mapid = GetId();
							mapPlant.plantid = pCfgPlant->id;
							mapPlant.x = plantPos.x;
							mapPlant.y = plantPos.y;
							PlantDungeon *plant = POOL_MANAGER.pop<PlantDungeon>();
							if (NULL == plant)
							{
								continue;
							}
							plant->init(this, *pCfgPlant, mapPlant);
							addPlant(plant);
						}
					}
				}
			}
		}
		break;
	case  DEET_SHOW_MOVIE:
		{
			int32_t movieID = atoi(mapEvent.effect.c_str());
			if (CFG_DATA.getMovie(movieID))
			{
				broadcastPlayMovie(movieID);
			}
		}
		break;
	case  DEET_ACTIVE_EVENT:
		{
			Int32List events;
			StringVector strIds = StringUtility::split(mapEvent.effect, ":");
			for (StringVector::iterator it = strIds.begin();it != strIds.end();++it)
			{
				events.push_back( atoi( it->c_str() ) );
			}
			openEvents( events );
		}
		break;
	case DEET_REMOVE_MONSTER:
		{
			StringVector strIds = StringUtility::split(mapEvent.effect, ":");
			for (StringVector::iterator it = strIds.begin();it != strIds.end();++it)
			{
				for (MonsterList::iterator iter = m_monsters.begin(); iter != m_monsters.end();++iter)
				{
					if ((dynamic_cast<MonsterDungeon*>(*iter))->getDungeonMid() == atoi(it->c_str()))
					{
						GAME_SERVICE.removeMonster(*iter);
						(*iter)->leaveMap();
						break;
					}
				}
			}
		}
		break;
	case DEET_ADD_TOWER:
		{
			StringVector strIds = StringUtility::split(mapEvent.effect, ":");
			if ( strIds.size() == 5 )
			{
				DungeonTower tower = {};
				tower.id		= atoi( strIds[0].c_str() );
				tower.free		= atoi( strIds[1].c_str() );
				tower.limit		= atoi( strIds[2].c_str() );
				tower.costType	= atoi( strIds[3].c_str() );
				tower.costValue	= atoi( strIds[4].c_str() );
				CfgDungeonMonster *pCfgDungeonMonster = CFG_DATA.getDungeonMonster( tower.id );
				if ( pCfgDungeonMonster != NULL )
				{
					tower.mid	= pCfgDungeonMonster->mid;
				}
				addWaitTower( tower );
			}
		}
		break;
	case DEET_SET_WAIT_TIME:
		{
			StringVector strIds = StringUtility::split(mapEvent.effect, ":");
			if ( strIds.size() == 2 )
			{
				setState( DS_WAIT, atoi( strIds[0].c_str() ) );
				StringVector strEvents = StringUtility::split( strIds[1], "," );
				Int32List events;
				for ( uint32_t i = 0; i < strEvents.size(); ++i )
				{
					events.push_back( atoi( strEvents[i].c_str() ) );
				}
				openEvents( events );
			}
		}
		break;
	case DEET_REWARD_RATIO:
		{
			m_dRatio = static_cast<double>( atoi( mapEvent.effect.c_str() ) ) / 100;
		}
		break;
	case DEET_ADD_STACK:
		{
			PosList stacks;
			StringVector strstacks = StringUtility::split(mapEvent.effect, "|");
			for ( StringVector::iterator it = strstacks.begin();it != strstacks.end();++it )
			{
				StringVector vstack = StringUtility::split( *it, ":" );
				if ( vstack.size() == 2 )
				{
					stacks.push_back( Position( atoi( vstack[0].c_str() ), atoi( vstack[1].c_str() ) ) );
				}
			}
			AddStack( stacks );
		}
		break;
	case DEET_REMOVE_STACK:
		{
			PosList stacks;
			StringVector strstacks = StringUtility::split(mapEvent.effect, "|");
			for ( StringVector::iterator it = strstacks.begin();it != strstacks.end();++it )
			{
				StringVector vstack = StringUtility::split( *it, ":" );
				if ( vstack.size() == 2 )
				{
					stacks.push_back( Position( atoi( vstack[0].c_str() ), atoi( vstack[1].c_str() ) ) );
				}
			}
			RemoveStack( stacks );
		}
		break;
	default:
		break;
	}
	broadcastDungeonEvent(mapEvent.trigger_id);
	mapEvent.bDone = true;
}

void Dungeon::addMonsterHPEventInfo(MonsterDungeon* pMonster)
{
	if (NULL == pMonster)
	{
		return;
	}
	for (CfgMapEventList::iterator it = m_events.begin(); it != m_events.end(); ++it)
	{
		if (it->trigger_type == DETT_HP_PERCENT && it->trigger_param[0] ==HPET_MONSTER  && it->trigger_param.size() == 4)
		{
			if (it->trigger_param[1]== pMonster->getDungeonMid() &&  it->trigger_param[2] > 0 && it->trigger_param[3] > 0)
			{
				HPEvent hpEvent = {};
				hpEvent.id		= it->trigger_id;
				hpEvent.minhp	= it->trigger_param[2];
				hpEvent.maxhp	= it->trigger_param[3];

				pMonster->addEventHp( hpEvent );
			}	    
		}
	}
}

void Dungeon::signDungeonCompleted()
{
	int32_t nRecordId = PR_DUNGEON_COMPLETED_BEING + getDungeonId();
	for (PlayerList::iterator it =  m_players.begin();it != m_players.end();++it)
	{
		Player* player = *it;
		if ( player != NULL )
		{
			player->GetOperateLimit().AddLimitCount( nRecordId, 1 );
		}
	}
}

void Dungeon::setCanSubmit()
{
	for (PlayerList::iterator it =  m_players.begin();it != m_players.end();++it)
	{
		Player* player = *it;
		if ( player != NULL )
		{
			player->checkTaskCanSubmit(m_cfgDungeon.id);

			LogDungeon logDungeon = {};
			logDungeon.cid = player->getCid();
			logDungeon.did = m_cfgDungeon.id;
			logDungeon.start_time = GetStartTime();
			logDungeon.finish_time = getNow();
			logDungeon.state = 1;

			DB_SERVICE.updateDungeon( logDungeon );
		}
	}
}

Player* Dungeon::getEnemySidePlayer(Monster *monster)
{
	if (monster == NULL || m_players.empty())
	{
		return NULL;
	}

	PlayerList players;
	for (PlayerList::iterator it = m_players.begin(); it != m_players.end(); ++it)
	{
		int32_t ndis=monster->getCurrentTile().tileDistance((*it)->getCurrentTile());
		int32_t mdis =monster->getViewRange();
		//int8_t nkingdom=(*it)->getKingdom();
		if ( (*it)->isAlive() && ndis<= mdis)
		{
			players.push_back(*it);
		}
	}

	if (players.empty())
	{
		return NULL;
	}

	int32_t index = RANDOM.generate(0, players.size()-1);

	PlayerList::iterator it = players.begin();
	for (int32_t i = 0; i < index && it != players.end(); ++i)
	{
		it++;
	}

	if (it == players.end())
	{
		return NULL;
	}

	return *it;
}

void Dungeon::generateMonster( const std::string &str, bool rewardWave )
{
	StringVector strIds = StringUtility::split(str, ":");
	for (StringVector::iterator it = strIds.begin(); it != strIds.end(); ++it)
	{
		MonsterWait stu = {};
		stu.id = atoi( it->c_str() );
		stu.rewardWave = rewardWave;
		m_waitMonster.push_back( stu );
	}
}

void Dungeon::checkAddMonster( int64_t curTick )
{
	MonsterWaitList::iterator iter = m_waitMonster.begin();
	MonsterWaitList::iterator eiter = m_waitMonster.end();
	for ( ; iter != eiter; )
	{
		if ( flashMonster( *iter, curTick ) )
		{
			++iter;
		}
		else
		{
			m_waitMonster.erase( iter++ );
		}
	}
}

void Dungeon::checkDropItem( MonsterDungeon* pMonster )
{
	const CfgDungeonDrop* pDrop = CFG_DATA.randDungeonDrop( getDungeonId() );
	if ( NULL == pDrop )
	{
		return;
	}

	if ( pDrop->type == DDT_TRAP )
	{
		CfgDungeonTrap *pCfgDungeonTrap = CFG_DATA.getDungeonTrap( pDrop->item_id );
		if ( pCfgDungeonTrap != NULL )
		{
			CfgTrap *pCfgTrap = CFG_DATA.getTrap( pCfgDungeonTrap->tid );
			if ( pCfgTrap != NULL )
			{
				Trap *pTrap = POOL_MANAGER.pop<Trap>();
				if ( pTrap != NULL )
				{
					pTrap->init( this, pMonster->getCurrentTile(), *pCfgTrap );
					addTrap(pTrap);
				}
			}
		}
	}
	else if ( pDrop->type == DDT_ITEM )
	{
		CDropItemGroup *pDropItemGroup = POOL_MANAGER.pop<CDropItemGroup>();
		if (NULL == pDropItemGroup)
		{
			return;
		}

		DropItem dropItems[MAX_DROPITEM_SIZE] = {};
		dropItems[0].itemId		= pDrop->item_id;
		dropItems[0].itemClass	= pDrop->item_class;
		dropItems[0].bindType	= pDrop->bind_type;
		dropItems[0].itemCount	= 1;
		pDropItemGroup->init( this, pMonster->getCurrentTile(), NULL, ET_MONSTER, pMonster->getMid(), dropItems, "", 0, 1 );
		addDropItemGroup(pDropItemGroup);
	}
}

bool Dungeon::flashMonster( MonsterWait& waitAddMonster, int64_t curTick, bool rewardWave )
{
	CfgDungeonMonster *pCfgDungeonMonster = CFG_DATA.getDungeonMonster( waitAddMonster.id );
	if ( NULL == pCfgDungeonMonster )
	{
		return false;
	}

	if ( pCfgDungeonMonster->count < 0 )
	{
		return false;
	}

	CfgMonster *pCfgMonster = CFG_DATA.getMonster( pCfgDungeonMonster->mid );
	if ( NULL == pCfgMonster )
	{
		return false;
	}

	if ( curTick - waitAddMonster.lastTick < pCfgDungeonMonster->delay )
	{
		return true;
	}

	++waitAddMonster.times;
	waitAddMonster.lastTick = curTick;

	if ( pCfgDungeonMonster->count == 1 )
	{
		flashMonster( pCfgDungeonMonster, pCfgMonster, pCfgDungeonMonster->x, pCfgDungeonMonster->y, rewardWave );
	}
	else
	{
		std::vector<Position> positions;
		for (int32_t x = -2; x < 2; ++x)
		{
			for (int32_t y = -2; y < 2; ++y)
			{
				Position pos(pCfgDungeonMonster->x+x, pCfgDungeonMonster->y+y);
				if (isWalkablePosition(pos.x, pos.y))
				{
					positions.push_back(pos);
				}
			}
		}

		random_shuffle(positions.begin(), positions.end());

		for ( int32_t i = 0; i < pCfgDungeonMonster->count && i < static_cast<int32_t>(positions.size()); ++i )
		{
			flashMonster( pCfgDungeonMonster, pCfgMonster, positions[i].x, positions[i].y, rewardWave );
		}
	}

	if ( waitAddMonster.times >= pCfgDungeonMonster->times )
	{
		return false;	// 刷完从列表中删除
	}
	return true;
}

void Dungeon::flashMonster( CfgDungeonMonster* pCfgDungeonMonster, CfgMonster *pCfgMonster, int32_t x, int32_t y, bool rewardWave, MonsterBuff* pMonsterBuff )
{
	MonsterDungeon *pMonster = POOL_MANAGER.pop<MonsterDungeon>();
	if (NULL == pMonster)
	{
		return;
	}

	CfgMapMonster cfgmapmonster = {};
	cfgmapmonster.mapid = GetId();
	cfgmapmonster.monsterid = pCfgDungeonMonster->mid;
	cfgmapmonster.x = x;
	cfgmapmonster.y = y;

	pMonster->init( this, *pCfgDungeonMonster, *pCfgMonster, cfgmapmonster, pMonsterBuff );
	addMonster( pMonster, cfgmapmonster.x, cfgmapmonster.y );
	addMonsterHPEventInfo( pMonster );
	pMonster->runOnRoad();

	Int32MonsterWaveMap::iterator itMonsterWave = m_monsterWave.find(pCfgDungeonMonster->wave);
	if (itMonsterWave != m_monsterWave.end())
	{
		itMonsterWave->second.count++;
	}
	else
	{
		MonsterWave monsterWave = {};
		monsterWave.count = 1;
		monsterWave.startTime = getTick();
		monsterWave.rewardWave = rewardWave;
		m_monsterWave.insert( Int32MonsterWaveMap::value_type( pCfgDungeonMonster->wave, monsterWave ) );
	}
}

DungeonState Dungeon::getState() const
{
	return m_state;
}

const CfgDungeon& Dungeon::GetCfgDungeon() const
{
	return m_cfgDungeon;
}

bool Dungeon::empty() const
{
	return m_players.empty();
}

bool Dungeon::IsWin() const
{
	return m_state == DS_WIN;
}

void Dungeon::setState( DungeonState state, int32_t param )
{
	m_state = state;
	m_stateTick = getTick();
	m_stateParam = param;
}

void Dungeon::start()
{
	if( 0 == m_nStartTime )
	{
		m_nStartTime = getNow();
	}
	if ( m_cfgDungeon.type == DT_TEAM )
	{
		setState( DS_WAIT, 5 );
	}
	else
	{
		setState( DS_RUNNING );
	}
}

void Dungeon::initPlayerInfo( Player* player )
{
	if (NULL == player)
	{
		return;
	}
	//dungeon  buff
	CfgBuff *pCfgBuff = CFG_DATA.getBuff(m_cfgDungeon.player_buff);
	if (pCfgBuff != NULL)
	{
		for (PlayerList::iterator it = m_players.begin();it != m_players.end();++it)
		{
			DungeonBuff *pBuff = new DungeonBuff(*(*it), *pCfgBuff);
			if (NULL == pBuff)
			{
				continue;
			}
			pBuff->init();

			(*it)->addBuff(pBuff);

			//CfgBuff *pCfgVBuff = CFG_DATA.getBuff(m_cfgDungeon.vicegeneral_buff);
			//if (pCfgVBuff == NULL)
			//{
			//	continue;
			//}

			//int32_t vCount = (*it)->getViceGeneralCount();
			//for (int32_t i = 0; i < vCount;++i)
			//{
			//	ViceGeneral*pVicegeneral =  (*it)->getVicegeneralByIndex(i);
			//	if (pVicegeneral != NULL)
			//	{
			//		DungeonBuff *pVBuff = new DungeonBuff(*pVicegeneral, *pCfgVBuff);
			//		if (NULL == pVBuff)
			//		{
			//			continue;
			//		}
			//		pVBuff->init();

			//		pVicegeneral->addBuff(pVBuff);
			//	}
			//}
		}
	}

	//hp event
	for (CfgMapEventList::iterator it = m_events.begin(); it != m_events.end(); ++it)
	{
		if (it->trigger_type == DETT_HP_PERCENT && it->trigger_param[0] ==HPET_PLAYER  && it->trigger_param.size() ==3 )
		{
			if (  it->trigger_param[2] > 0)
			{
				player->addEventHp(it->trigger_param[2]);
			}	    
		}
	}

	// init reward
	{
		DungeonRewardList::iterator iter = m_playerReward.begin();
		DungeonRewardList::iterator eiter = m_playerReward.end();
		for ( ; iter != eiter; ++iter )
		{
			if ( iter->cid == player->getCid() )
			{
				break;
			}
		}
		if ( iter == eiter )
		{
			CfgDungeonReward* pReward = CFG_DATA.getDungeonReward( getDungeonId() );
			if ( pReward != NULL )
			{
				DungeonReward reward = {};
				reward.cid   = player->getCid();
				reward.exp   = pReward->exp;
				reward.cash  = pReward->cash;
				reward.money = pReward->money;
				m_playerReward.push_back( reward );
			}
		}
	}
}

void Dungeon::onPlayerLeave(Player *player)
{
	if (player == NULL)
	{
		return;
	}

	if (m_state == DS_RUNNING)
	{
		sendDungeonResult(player, 0);
	}
	else if (m_state == DS_WIN)
	{
		sendDungeonResult(player, m_stateParam);
		GetReward( player );
	}

	if ( m_cfgDungeon.type == DT_MONEY || m_cfgDungeon.id == FIRST_DUNGEON_ID )
	{
		player->GetCharSkill().ClearXP();
	}
}

bool Dungeon::onEndDungeon( Player* player )
{
	if ( player == NULL || GetLeader() != player )
	{
		return false;
	}

	if ( m_cfgDungeon.type != DT_TD )
	{
		return false;
	}
	
	if ( m_state != DS_RUNNING )
	{
		return false;
	}

	setCanSubmit();
	clearMonster();
	broadcastFinishDungon( 15 );
	signDungeonCompleted();
	setState( DS_WIN, 15 );	
	return true;
}


void Dungeon::onMonsterHPEvent( MonsterDungeon *monster, int32_t id )
{
	if (monster == NULL)
	{
		return;
	}

	for (CfgMapEventList::iterator it = m_events.begin(); it != m_events.end(); ++it)
	{
		if ( it->bOpen && !it->bDone && it->trigger_id == id )
		{
			triggerEvent( *it, monster );
		}
	}
}


void Dungeon::onPlayerHPEvent(Player* player)
{
	if (player == NULL)
	{
		return;
	}

	for (CfgMapEventList::iterator it = m_events.begin(); it != m_events.end(); ++it)
	{
		if ( it->bOpen && !it->bDone && it->trigger_type == DETT_HP_PERCENT && it->trigger_param.size() == 3)
		{
			triggerEvent( *it, player );
			player->addEventHp(0);
		}
	}
}

void Dungeon::onPlayerDie(Player* player)
{
	if (player == NULL)
	{
		return;
	}

	for (CfgMapEventList::iterator it = m_events.begin(); it != m_events.end(); ++it)
	{
		if ( it->bOpen && !it->bDone )
		{
			if (it->trigger_type == DETT_PLAYER_DIE)
			{
				triggerEvent( *it, player );
			}
		}
	}
}

void Dungeon::onMonsterDie(MonsterDungeon *monster)
{
	if (monster == NULL)
	{
		return;
	}

	checkDropItem( monster );


	Int32MonsterWaveMap::iterator it = m_monsterWave.find(monster->getWave());

	if (it != m_monsterWave.end())
	{
		it->second.killed++;
		if ( !it->second.alive() )
		{
			if ( it->second.rewardWave )
			{
				++m_finshInfo.reward_wave;
			}
			else
			{
				++m_finshInfo.normal_wave;
			}
		}
	}

	if ( monster->getSide() != 1 )
	{
		if ( getDungeonType() == DT_TD )
		{
			PlayerList players = getPlayers();
			PlayerList::iterator it = players.begin();
			for ( ; it != players.end(); ++it )
			{
				DungeonRewardList::iterator iter = m_playerReward.begin();
				DungeonRewardList::iterator eiter = m_playerReward.end();
				int32_t AddExp = 0;
				for ( ; iter != eiter; ++iter )
				{
					if ( (*it)->getCid() == iter->cid )
					{
						int32_t Level = (*it)->getLevel();
						if ( Level >= 130 )
						{
							Level = (*it)->GetSoulLevel() + 60;
						}
						else if ( Level > 70 )
						{
							Level = Level - 70;
						}
						else
						{
							Level = 0;
						}
						AddExp = static_cast<int32_t>( monster->getExp() * ( 1.0f + static_cast<float>(Level) / 50 ) * ( 1.0f + static_cast<float>(Level) / 50 ) );
						iter->exp += AddExp;
					}

				}
				m_finshInfo.get_exp += AddExp;
			}
		}
		else if ( getDungeonType() == DT_MONEY )
		{
			DungeonRewardList::iterator iter = m_playerReward.begin();
			DungeonRewardList::iterator eiter = m_playerReward.end();
			for ( ; iter != eiter; ++iter )
			{
				iter->money += monster->getMoney();
			}
			m_finshInfo.get_money += monster->getMoney();
		}
		if ( monster->isBoss() )
		{
			++m_finshInfo.boss_count;
		}
		else
		{
			++m_finshInfo.kill_count;
		}
	}
	for (CfgMapEventList::iterator it = m_events.begin(); it != m_events.end(); ++it)
	{
		if ( it->bOpen && !it->bDone )
		{
			if (it->trigger_type == DETT_MONSTER_WAVE)
			{
				checkEvent( *it );
			}
			else if (it->trigger_type == DETT_MONSTER_ID)
			{
				if (it->trigger_param[0] == monster->getDungeonMid())
				{
					triggerEvent( *it, monster );
				}
			}
		}
	}

	broadcastDungeonInfo();
}

void Dungeon::onNpcEnd(NpcDungeon *npc, int8_t choice)
{
	if ( NULL == npc )
	{
		return;
	}
}
void Dungeon::onPlantGather(PlantDungeon *plant)
{
	if (plant == NULL)
	{
		return;
	}

	for (CfgMapEventList::iterator it = m_events.begin(); it != m_events.end(); ++it)
	{
		if ( it->bOpen && !it->bDone && it->trigger_type == DETT_PLANT_GATHER && it->trigger_param[0] == plant->getPlantId())
		{
			triggerEvent( *it );
		}
	}
}

void Dungeon::onMonsterArriveRoadEnd(MonsterDungeon *monster)
{
	if (monster == NULL)
	{
		return;
	}

	Int32MonsterWaveMap::iterator it = m_monsterWave.find(monster->getWave());
	if (it != m_monsterWave.end())
	{
		it->second.escaped++;
	}

	m_finshInfo.escape_count++;

	PlayerList players = getPlayers();
	PlayerList::iterator itPlayer = players.begin();
	int32_t LostExp = 0;
	for ( ; itPlayer != players.end(); ++itPlayer )
	{
		int32_t Level = (*itPlayer)->getLevel();
		if ( Level >= 130 )
		{
			Level = (*itPlayer)->GetSoulLevel() + 60;
		}
		else if ( Level > 70 )
		{
			Level = Level - 70;
		}
		else
		{
			Level = 0;
		}
		LostExp = static_cast<int32_t>( monster->getExp() * ( 1.0f + static_cast<float>(Level) / 50 ) * ( 1.0f + static_cast<float>(Level) / 50 ) );
		break;//经验副本是单人副本
	}
	m_finshInfo.lost_exp += LostExp;
	monster->remove();

	for (CfgMapEventList::iterator it = m_events.begin(); it != m_events.end(); ++it)
	{
		if ( it->bOpen && !it->bDone )
		{
			if (it->trigger_type == DETT_MONSTER_WAVE)
			{
				checkEvent( *it );
			}
		}
	}
}

int32_t Dungeon::getDungeonId() const
{
	return m_cfgDungeon.id;
}

int32_t Dungeon::getDungeonType() const
{
	return m_cfgDungeon.type;
}

//bool Dungeon::updatePlayerGiftInfo(int32_t cid)
//{
//	Int32Int32Map::iterator it = m_playerGiftInfo.find(cid);
//	if (it != m_playerGiftInfo.end())
//	{
//		m_playerGiftInfo.erase(it);
//		return true;
//	}
//
//	return false;
//}

std::string Dungeon::getDungeonName() const
{
	return m_cfgDungeon.name;
}

//MemChrBagVector* Dungeon::getGiftReward(int32_t index)
//{
//	  if (index < 1 ||index > DUNGEON_GIFT_COUNT)
//	  {
//		  return NULL;
//	  }
//
//	 return  &m_dungeonGift[index-1];
//}

//void Dungeon::clearGift()
//{
//	 m_dungeonGift.clear();
//}

int32_t Dungeon::getDungeonGroupId() const
{
	return m_cfgDungeon.group_id;
}

PlayerList Dungeon::getPlayers()
{
	return m_players;
}

Player* Dungeon::GetLeader()
{
	if ( m_players.empty() )
	{
		return NULL;
	}

	return m_players.front();
}

const Player* Dungeon::GetLeader() const
{
	if ( m_players.empty() )
	{
		return NULL;
	}

	return m_players.front();
}

void Dungeon::clearMonster()
{
	for ( MonsterList::iterator it = m_monsters.begin();it != m_monsters.end();++it )
	{
		MonsterDungeon* pMonster = dynamic_cast<MonsterDungeon*>( *it );
		if ( pMonster != NULL )
		{
			pMonster->remove();
		}
	}
}

//void Dungeon::createGift(CfgDungeonDropGroupVector* pGroup)
//{
//	if (NULL == pGroup)
//	{
//		return;
//	}
//	for (int32_t i = 0; i < DUNGEON_GIFT_COUNT;++i)
//	{
//		int32_t index = 0;
//		int32_t weight = 1000;
//		int32_t wRatio = RANDOM.generate(1, 1000);
//		MemChrBag  mustBag = {};
//		for (CfgDungeonDropGroupVector::iterator it = pGroup->begin();it != pGroup->end();++it)
//		{
//			if (it->weight > 0)
//			{
//				if (it->weight >= wRatio &&it->weight<= weight )
//				{
//					weight = it->weight;
//					mustBag.itemId		= it->item_id;
//					mustBag.itemClass	= it->item_class;
//					mustBag.bind		= it->bind_type;
//					mustBag.itemCount	= it->count;
//				}
//			}
//			else
//			{
//				int32_t ratio = RANDOM.generate(1, 1000);
//				if (it->probability >= ratio && index <= 1)
//				{
//					MemChrBag  chrBag = {};
//					chrBag.itemId		= it->item_id;
//					chrBag.itemClass	= it->item_class;
//					chrBag.itemClass	= it->bind_type;
//					chrBag.itemCount	= it->count;
//
//					index++;
//					m_dungeonGift[i].push_back(chrBag);
//				}
//			}
//		}
//		if (mustBag.itemId > 0)
//		{
//			m_dungeonGift[i].push_back(mustBag);
//		}
//	}
//}

void Dungeon::broadcastFinishDungon(int32_t param)
{
	NetPacket *packet = GAME_SERVICE.popNetpacket( PACK_DISPATCH, SM_FINISH_DUNGEON );
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32( m_cfgDungeon.id );						// 副本ID
	if ( m_cfgDungeon.type == DT_TD )
	{
		packet->writeInt32( m_finshInfo.get_exp );				// 获得的经验值
		packet->writeInt32( m_finshInfo.lost_exp );				// 损失的经验值
		packet->writeInt32( m_finshInfo.kill_count );			// 击杀个数
		packet->writeInt32( m_finshInfo.escape_count );			// 逃跑个数
	}
	else if ( m_cfgDungeon.type == DT_MONEY )
	{
		packet->writeInt32( m_finshInfo.kill_count );			// 击杀怪物
		packet->writeInt32( m_finshInfo.normal_wave );			// 常规波数
		packet->writeInt32( m_finshInfo.reward_wave );			// 奖励波数
		packet->writeInt32( m_finshInfo.get_money );			// 获得金币
	}
	else
	{
		//empty
	}

	packet->setSize(packet->getWOffset());
	broadcast(packet);
}

void Dungeon::sendDungeonResult(Player *player, int32_t param)
{
	if (player == NULL)
	{
		return;
	}

	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_DUNGEON_RESULT);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32( m_cfgDungeon.id );
	packet->writeInt32(param);
	packet->writeInt32(1);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(player->getGateIndex(), packet);
}

void Dungeon::broadcastDungeonEvent(int32_t evtId)
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_DUNGEON_EVENT);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(getDungeonId());
	packet->writeInt32(evtId);
	packet->setSize(packet->getWOffset());
	broadcast(packet);
}

void Dungeon::broadcastPlayMovie(int32_t movieID)
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_DUNGEON_MOVIE);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(movieID);
	packet->setSize(packet->getWOffset());
	broadcast(packet);
}

void Dungeon::broadcastDungeonInfo()
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_DUNGEON_INFO_BOARD);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32( m_cfgDungeon.id );							// 副本ID
	if ( m_cfgDungeon.type == DT_TD )
	{
		packet->writeInt32( m_finshInfo.kill_count );				// 杀敌数量
		packet->writeInt32( m_finshInfo.get_exp );					// 获得的经验值
		packet->writeInt8( m_waitTower.size() );
		DungeonTowerList::iterator iter = m_waitTower.begin();
		DungeonTowerList::iterator eiter = m_waitTower.end();
		for ( ; iter != eiter; ++iter )
		{
			packet->writeInt32( iter->id );							// ID
			packet->writeInt32( iter->mid );						// Mid
			packet->writeInt32( iter->free - iter->count );			// 剩余数量
			packet->writeInt32( iter->limit - iter->free );			// 剩余购买数量
			packet->writeInt8( iter->costType );					// 价格
			packet->writeInt32( iter->costValue );
		}
	}
	else if ( m_cfgDungeon.type == DT_MONEY )
	{
		packet->writeInt32( m_finshInfo.kill_count );				// 杀敌数量
		packet->writeInt32( m_finshInfo.get_money );				// 获得的经验值
		packet->writeInt32( GetLeftTime() );
	}
	else
	{
		packet->writeInt32( m_finshInfo.kill_count );
		packet->writeInt32( m_finshInfo.boss_count );
	}
	packet->setSize(packet->getWOffset());
	broadcast(packet);
}

void Dungeon::broadcastDamageList()
{
	NetPacket *packet = GAME_SERVICE.popNetpacket( PACK_DISPATCH, SM_TEAM_DUNGEON_DAMAGE_LIST );
	if (NULL == packet)
	{
		return;
	}

	packet->writeInt8( m_damages.size() );							// 副本ID
	DamageSumList::iterator iter = m_damages.begin();
	DamageSumList::iterator eiter = m_damages.end();
	for ( ; iter != eiter; ++iter )
	{
		packet->writeUTF8( iter->name );
		packet->writeInt32( iter->damage );
	}
	packet->setSize(packet->getWOffset());
	broadcast(packet);
	m_needBroadcastDamage = false;
}

void Dungeon::checkBroadcastDamageList()
{
	if ( m_needBroadcastDamage )
	{
		broadcastDamageList();
	}
}

bool Dungeon::BuyTower( Player* player, int32_t nId )
{
	if ( NULL == player )
	{
		return false;
	}

	DungeonTowerList::iterator iter = m_waitTower.begin();
	DungeonTowerList::iterator eiter = m_waitTower.end();
	for ( ; iter != eiter; ++iter )
	{
		if ( iter->id != nId )
		{
			continue;
		}

		if ( iter->free >= iter->limit )
		{
			continue;
		}

		if ( iter->costValue > 0 )
		{
			if ( !player->DecCurrency( static_cast<CURRENCY_TYPE>( iter->costType ), iter->costValue, GCR_DUNGEON_BUILD_TOWER ) )
			{
				return false;
			}
		}

		iter->free++;
		break;
	}
	if ( iter == eiter )
	{
		return false;
	}
	broadcastDungeonInfo();
	return true;
}

bool Dungeon::BuildTower( Player* player, int32_t nId, int16_t nPosX, int16_t nPosY )
{
	if ( NULL == player )
	{
		return false;
	}

	if ( !isWalkablePosition( nPosX, nPosY ) )
	{
		return false;
	}

	CfgDungeonMonster *pCfgDungeonMonster = CFG_DATA.getDungeonMonster( nId );
	if ( NULL == pCfgDungeonMonster )
	{
		return false;
	}

	CfgMonster *pCfgMonster = CFG_DATA.getMonster( pCfgDungeonMonster->mid );
	if ( NULL == pCfgMonster )
	{
		return false;
	}

	DungeonTowerList::iterator iter = m_waitTower.begin();
	DungeonTowerList::iterator eiter = m_waitTower.end();
	for ( ; iter != eiter; ++iter )
	{
		if ( iter->id != nId )
		{
			continue;
		}

		if ( iter->count >= iter->free )
		{
			continue;
		}

		iter->count++;
		break;
	}

	if ( iter == eiter )
	{
		return false;
	}

	MonsterDungeon *pMonster = POOL_MANAGER.pop<MonsterDungeon>();
	if ( NULL == pMonster )
	{
		return false;
	}

	CfgMapMonster cfgmapmonster = {};
	cfgmapmonster.mapid = GetId();
	cfgmapmonster.monsterid = pCfgDungeonMonster->mid;
	cfgmapmonster.x = nPosX;
	cfgmapmonster.y = nPosY;

	MonsterBuff* pMonsterBuff = NULL;
	pMonster->init( this, *pCfgDungeonMonster, *pCfgMonster, cfgmapmonster, pMonsterBuff );
	addMonster( pMonster, cfgmapmonster.x, cfgmapmonster.y );

	m_buildTower.push_back( pMonster );
	broadcastDungeonInfo();
	return true;
}

void Dungeon::addWaitTower( const DungeonTower& tower )
{
	DungeonTowerList::iterator iter = m_waitTower.begin();
	DungeonTowerList::iterator eiter = m_waitTower.end();
	for ( ; iter != eiter; ++iter )
	{
		if ( iter->id == tower.id && iter->costType == tower.costType && iter->costValue == iter->costValue )
		{
			iter->free	+= tower.free;
			iter->limit += tower.limit;
			return;
		}
	}

	m_waitTower.push_back( tower );
	broadcastDungeonInfo();
}

bool Dungeon::GetReward( Player* player, int8_t nRatio )
{
	if ( NULL == player )
	{
		return false;
	}

	if ( player->getRecord( m_cfgDungeon.group_id ) > m_cfgDungeon.daily_count )
	{
		return false;
	}

	BenefitType bnfType = player->benefitType();
	double bnfRatio = player->benefitRatio();

	DungeonRewardList::iterator iter = m_playerReward.begin();
	DungeonRewardList::iterator eiter = m_playerReward.end();
	for ( ; iter != eiter; ++iter )
	{
		if ( iter->cid == player->getCid() )
		{
			if ( iter->bGet > 0 )
			{
				return false;
			}

			if ( nRatio == 2 )
			{
				if ( m_cfgDungeon.double_cost < 0 )
				{
					return false;
				}
				// 扣元宝, =0免费领取双倍
				if ( m_cfgDungeon.double_cost > 0 )
				{
					if ( !player->DecCurrency( CURRENCY_GOLD, m_cfgDungeon.double_cost, GCR_DUNGEON_DOUBLE_REWARD, getDungeonId() ) )
					{
						return false;
					}
				}
			}
			else if ( nRatio != 1 )
			{
				return false;
			}

			if ( iter->money > 0 )
			{
				player->AddCurrency( CURRENCY_BIND_MONEY, static_cast<int32_t>( iter->money * m_dRatio)*nRatio, MCR_DUNGEON_REWARD, m_cfgDungeon.id );
			}
			if ( iter->cash > 0 )
			{
				player->AddCurrency( CURRENCY_CASH, static_cast<int32_t>( iter->cash * bnfRatio * m_dRatio)*nRatio, GCR_DUNGEON_REWARD, m_cfgDungeon.id );
			}
			if ( iter->exp > 0 )
			{
				player->addExp( static_cast<int32_t>(iter->exp * m_dRatio)*nRatio );
			}
			if ( ( player->getRecord( PR_DUNGEON_COMPLETED_BEING + getDungeonId() ) == 1 ) && ( m_cfgDungeon.rewardOnce.itemCount > 0 ) )	// 首次通关
			{
				if ( m_cfgDungeon.rewardOnce.itemClass == IC_PET_EGG )
				{
					if (  player->GetCharPet().GetEggBagAddHatchBagFreeCount() <= 0  )
					{
						DB_SERVICE.OnSendSysMail( player->getCid(), DungeonRewards, m_cfgDungeon.rewardOnce );
					}
					else
					{
						std::list<int32_t> EggIdList;
						EggIdList.push_back( m_cfgDungeon.rewardOnce.itemId );
						player->GetCharPet().AddEggs( EggIdList );
					}
				}
				else
				{
					if( player->GetBag().GetbagFreeSize() <= 0 )
					{
						DB_SERVICE.OnSendSysMail( player->getCid(), DungeonRewards, m_cfgDungeon.rewardOnce );
					}
					else
					{					
						player->GetBag().AddItem( m_cfgDungeon.rewardOnce, IACR_DUNGEON_REWARD );
					}

				}
			}
			else if ( m_cfgDungeon.rewardItem.itemCount > 0 )
			{
				if ( m_cfgDungeon.rewardItem.itemClass == IC_PET_EGG )
				{
					if (  player->GetCharPet().GetEggBagAddHatchBagFreeCount() <= 0  )
					{
						DB_SERVICE.OnSendSysMail( player->getCid(), DungeonRewards, m_cfgDungeon.rewardItem );
					}
					else
					{
						std::list<int32_t> EggIdList;
						EggIdList.push_back( m_cfgDungeon.rewardItem.itemId );
						player->GetCharPet().AddEggs( EggIdList );
					}
				}
				else
				{
					if( player->GetBag().GetbagFreeSize() <= 0 )
					{
						DB_SERVICE.OnSendSysMail( player->getCid(), DungeonRewards, m_cfgDungeon.rewardItem );
					}
					else
					{					
						player->GetBag().AddItem( m_cfgDungeon.rewardItem, IACR_DUNGEON_REWARD );
					}
				}
			}
			iter->bGet = nRatio;
			if ( m_cfgDungeon.type == DT_TEAM || m_cfgDungeon.type == DT_BOSS )
			{
				player->updateRecord( m_cfgDungeon.group_id,player->getRecord( m_cfgDungeon.group_id ) + 1 );
				player->GetPlayerHuoYueDu().AddHuoYueDuRecord( HYDT_DUNGEON, m_cfgDungeon.group_id );
				player->GetAchievemnet().AddAchievement( AT_DUNGEON,m_cfgDungeon.group_id );
			}
			return true;
		}
	}
	return false;
}

void Dungeon::InitTeamMember( const CharIdList& lst )
{
	m_memberList = lst;
}

void Dungeon::AddPlayerDamage( CharId_t cid, int32_t attackValue )
{
	if ( m_cfgDungeon.type != DT_TEAM )
	{
		return;
	}
	DamageSumList::iterator iter = m_damages.begin();
	DamageSumList::iterator eiter = m_damages.end();
	for ( ; iter != eiter; ++iter )
	{
		if ( iter->cid == cid )
		{
			iter->damage += attackValue;
			m_needBroadcastDamage = true;
			break;
		}
	}
}

void Dungeon::AddDungeonTime( int32_t nTime )
{
	m_nAddonTime += nTime;
	broadcastDungeonInfo();
}

int32_t Dungeon::GetDurationTime() const
{
	return m_cfgDungeon.duration + m_nAddonTime;
}

int32_t Dungeon::GetLeftTime()
{
	switch ( m_state )
	{
	case DS_RUNNING:		return static_cast<int32_t>( ( m_stateTick + GetDurationTime() * 1000 - getTick() ) / 1000 );
	case DS_FAIL:
	case DS_WIN:			return static_cast<int32_t>( ( m_stateTick + 30000 - getTick() ) / 1000 );
	default:	break;
	}
	return GetDurationTime();
}


void Dungeon::teamDungeonMemberEnter()
{
	if ( m_players.empty() )
	{
		GAME_SERVICE.TeamDungeonEnterDungeon( this, m_memberList );
	}
}

void Dungeon::checkRevive( int64_t curTick )
{
	PlayerList::iterator iter = m_players.begin();
	PlayerList::iterator eiter = m_players.end();
	for ( ; iter != eiter; ++iter )
	{
		Player* player = *iter;
		if ( NULL == player )
		{
			continue;
		}

		if ( !player->IsDead() )
		{
			continue;
		}

		if ( curTick - player->GetDieTick() >= 10*1000 )
		{
			player->FillHP();
			player->FillMP();
			player->setSyncStatusFlag();
			player->instantMove( m_cfgDungeon.x, m_cfgDungeon.y, IMR_TRANSFER );
			if ( m_cfgDungeon.type == DT_MONEY || m_cfgDungeon.id == FIRST_DUNGEON_ID )
			{
				player->GetCharSkill().FillXP();
			}
		}
	}
}

void Dungeon::DungeonGongGao( CharId_t CharId, std::string Name )
{
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_ZSZZ_DUNGEON_GONG_GAO);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64( CharId );
	packet->writeUTF8(  Name );
	packet->writeInt32( m_cfgDungeon.Battle );
	packet->writeInt32( m_cfgDungeon.star );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}
