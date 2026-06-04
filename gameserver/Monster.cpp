#include "stdafx.h"

#include "DropItem.h"
#include "GameService.h"
//#include "Kingdom.h"
#include "Map.h"
#include "Monster.h"
#include "Skill.h"
#include "ActivityManager.h"
#include "PoolManager.h"
#include "Timer.h"
#include "WorldBoss.h"
#include "DBService.h"

#include <algorithm>
#define MONSTER_REVER_HP_CD 1000
using namespace Answer;
using namespace std;

Direction Monster::m_directions[8] = {DOWN, DOWN_RIGHT, RIGHT, UP_RIGHT, UP, UP_LEFT, LEFT, DOWN_LEFT};

Monster::Monster()
	: Unit(ET_MONSTER), m_killerLevel(0), m_liftTime(0)
{
}

Monster::~Monster()
{

}

void Monster::refresh()
{
	updateState();

	if (!isAlive() && m_state != MS_CORPSE && m_state != MS_GHOST)
	{
		die();
	}
	if ( isAlive() )
	{
		CheckAddHp( getTick() );
	}
}

void Monster::reset()
{
	Unit::reset();

	m_pCfgSkill			= NULL;
	m_firstDamageTime	= 0;
	m_lastActionTick	= 0;
	m_standTick			= 0;
	m_standTime			= 0;
	m_corpseTick		= 0;
	m_ghostTick			= 0;
	m_attackTick		= 0;
	m_attackTime		= 0;
	m_nFamilyId			= 0;
	m_lastUpdateMinute	= 0;
	m_DieType			= 0;
	m_CanReverHp		= false;;
	m_LastReverHpTick	= 0;
	m_ReviveTime		= 0;
	m_lastUpdateWarVictoryBossMinute	= 0;
	m_liftTime	= 0;
	m_target.clear();
	m_killer.clear();
	HurtCharId.clear();
}

EntityId_t Monster::getUnitId() const
{
	return getEntityId();
}

CharId_t Monster::getOwner() const
{
	return 0;
}

int32_t Monster::getLevel() const
{
	return m_cfgmonster.level;
}

int32_t Monster::getBattle() const
{
	return m_cfgmonster.battle;
}

void Monster::postDamage(int32_t damge, UnitHandle launcher)
{
	if (!isAlive())
	{
		m_killer = launcher;
		Unit *pUnit = GAME_SERVICE.getUnit( launcher.id, launcher.type, GetRunnerId() );
		if (pUnit != NULL)
		{
			m_killerLevel = pUnit->GetCalLevel();
		}
	}
	else
	{
		if (m_cfgmonster.ai_style == MAS_PEACE)
		{
			return;
		}

		if ( ( m_cfgmonster.ai_target & (1<<launcher.type) ) == 0)
		{
			return;
		}

		if (m_state == MS_WALK_HOME)
		{
			return;
		}
		m_CanReverHp = false;
		if (m_target.isEmpty() && !launcher.isEmpty())
		{
			if (getTick() - m_lastActionTick > PLAYER_GENERAL_CD_MS*2)
			{
				m_lastActionTick = getTick() - PLAYER_GENERAL_CD_MS; //delay attack
			}

			m_target = launcher;
			setState(MS_ATTACK);
		}

		if (m_firstDamageTime == 0)
		{
			m_firstDamageTime = getNow();
		}
		if ( m_cfgmonster.TaskShare == 1 )
		{
			Player* pKiller = NULL;
			if ( launcher.type == ET_PLAYER )
			{
				pKiller = GAME_SERVICE.getPlayer( launcher.id, GetRunnerId() );
			}
			else if ( m_killer.type == ET_PET )
			{
				CObjPet* pPet = GAME_SERVICE.getPet( launcher.id, GetRunnerId() );
				if ( pPet != NULL )
				{
					pKiller = pPet->GetPlayer();
				}
			}
			if ( pKiller != NULL )
			{
				HurtCharId.insert( pKiller->getCid() );
			}
		}
		//else if (m_cfgmonster.angry_time != 0 && getNow() - m_firstDamageTime >= m_cfgmonster.angry_time)
		//{
		//	addSecondAttrAddon(ATTR_VP, 1000);
		//	addSecondAttrAddon(ATTR_VPP, 1000);
		//}
	}
}

void Monster::init(const CfgMonster &cfgmonster, const CfgMapMonster &cfgmapmonster, Buff* pBuff)
{
	SetAttrValue( CObjAttrs::ATTR_HP,			cfgmonster.hp			);
	SetAttrValue( CObjAttrs::ATTR_PHY_ATK_MIN,	cfgmonster.phy_atk_min	);
	SetAttrValue( CObjAttrs::ATTR_PHY_ATK_MAX,	cfgmonster.phy_atk_max	);
	SetAttrValue( CObjAttrs::ATTR_MAG_ATK_MIN,	cfgmonster.mag_atk_min	);
	SetAttrValue( CObjAttrs::ATTR_MAG_ATK_MAX,	cfgmonster.mag_atk_max	);
	SetAttrValue( CObjAttrs::ATTR_PHY_DEF,		cfgmonster.phy_def		);
	SetAttrValue( CObjAttrs::ATTR_MAG_DEF,		cfgmonster.mag_def		);
	SetAttrValue( CObjAttrs::ATTR_HITRATE,		cfgmonster.hitrate		);
	SetAttrValue( CObjAttrs::ATTR_DODGE,		cfgmonster.dodge		);
	SetAttrValue( CObjAttrs::ATTR_CRITRATE,		cfgmonster.critrate		);
	SetAttrValue( CObjAttrs::ATTR_DMG_ADD,		cfgmonster.dmg_add		);
	SetAttrValue( CObjAttrs::ATTR_DMG_RDC,		cfgmonster.dmg_rdc		);
	SetAttrValue( CObjAttrs::ATTR_DMG_ADD_PEC,	cfgmonster.dmg_add_pec	);
	SetAttrValue( CObjAttrs::ATTR_DMG_RDC_PEC,	cfgmonster.dmg_rdc_pec	);
	SetAttrValue( CObjAttrs::ATTR_MOVE_SPEED,	cfgmonster.movespeed	);
	SetAttrValue( CObjAttrs::ATTR_BATTLE,		cfgmonster.battle		);

	m_cfgmonster = cfgmonster;
	m_cfgmapmonster = cfgmapmonster;
	m_ReviveTime	= 0;
	m_killer.clear();
	m_target.clear();
	m_pCfgSkill = NULL;
	m_firstDamageTime = 0;
	m_lastActionTick = 0;

	m_standTick = 0;
	m_standTime = 0;
	m_corpseTick = 0;
	m_ghostTick = 0;
	m_attackTick=0;
	m_attackTime=0;
	setState(MS_STAND);

	addBuff(pBuff);
	
	FillHP();
}

void Monster::init(const CfgMonster &cfgmonster, const CfgMapMonster &cfgmapmonster, MonsterState state, const AttrAddonVector *vAttrAddon)
{
	init(cfgmonster, cfgmapmonster, (Buff*)NULL);
	
	setState(state);
	
	if ( vAttrAddon != NULL )
	{
		for ( AttrAddonVector::const_iterator it = vAttrAddon->begin(); it != vAttrAddon->end(); ++it )
		{
			AddAttrValue( static_cast<CObjAttrs::Index_T>(it->index), it->addon );
		}
	}
}

void Monster::SetLifeTime(int32_t nLifeTime)
{
	m_liftTime = nLifeTime;
}

void Monster::appendInfo(Answer::NetPacket *packet)
{
	if (packet == NULL)
	{
		return;
	}

	packet->writeInt64( getUnitId() );
	packet->writeInt32( getMid() );
	packet->writeInt16( getCurrentTile().x );
	packet->writeInt16( getCurrentTile().y );
	packet->writeInt16( getTargetTile().x );
	packet->writeInt16( getTargetTile().y );
	packet->writeInt32( GetHP() );
	packet->writeInt32( GetMaxHP() );
	packet->writeInt16( GetMoveSpeed() );
	packet->writeInt8( getSide() );
	packet->writeInt64( m_nFamilyId );
}

void Monster::leaveMap()
{
	broadcastLeave();
}

int32_t Monster::getMid() const
{
	return m_cfgmonster.mid;
}

int32_t Monster::getQuality() const
{
	return m_cfgmonster.quality;
}

std::string Monster::getName() const
{
	return m_cfgmonster.name;
}

int32_t Monster::getViewRange() const
{
	return m_cfgmonster.view_range;
}

int32_t Monster::getMoveRange() const
{
	return m_cfgmonster.move_range;
}

int32_t Monster::getBroadcast() const
{
	return m_cfgmonster.broadcast;
}

MonsterState Monster::getState() const
{
	return m_state;
}

int32_t Monster::getReviveTime() const
{
	if ( m_ReviveTime > 0 )
	{
		return ( m_ReviveTime * 1000 );
	}
	return m_cfgmonster.revive_time;
}

int32_t Monster::getSide() const
{
	return 0;
}

bool Monster::isLifeEnd()
{
	return false;
}

void Monster::endLife()
{

}

bool Monster::isActivityMonster()
{
	return false;
}

bool Monster::isDungeonMonster()
{
	return false;
}

bool Monster::needDel() const
{
	return false;
}

void Monster::destroy()
{
}

const UnitHandle& Monster::getKillerHandler() const
{
	return m_killer;
}

Player* Monster::getKiller()
{
	Player* pKiller = NULL;
	if ( m_killer.type == ET_PLAYER )
	{
		pKiller = GAME_SERVICE.getPlayer( m_killer.id, GetRunnerId() );
	}
	else if ( m_killer.type == ET_PET )
	{
		CObjPet* pPet = GAME_SERVICE.getPet( m_killer.id, GetRunnerId() );
		if ( pPet != NULL )
		{
			pKiller = pPet->GetPlayer();
		}
	}
	return pKiller;
}

bool Monster::isBoss() const
{
	return m_cfgmonster.boss_sign > 0;
}

int32_t	Monster::getAITarget() const
{
	return m_cfgmonster.ai_target;
}

int32_t Monster::getExp() const
{
	return m_cfgmonster.exp;
}


FamilyId_t Monster::GetFamilyId() const
{
	return m_nFamilyId;
}

void Monster::SetFamilyId( FamilyId_t fid )
{
	m_nFamilyId = fid;
	setNeedSync();
}

void Monster::broadcastIntoMap()
{
	if (m_pMap != NULL)
	{
		NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_MONSTER_INTO_MAP);
		if (NULL == packet)
		{
			return;
		}
		packet->writeInt32(1);
		appendInfo(packet);
		packet->setSize(packet->getWOffset());
		m_pMap->broadcast(packet);
	}

// 	if ( m_cfgmonster.broadcast != 0 && !m_pMap->IsDungeonMap() )
// 	{
// 		MonsterBroadcast monsterBroadcast;
// 		monsterBroadcast.mid = getMid();
// 		monsterBroadcast.state = MBS_ALIVE;
// 		monsterBroadcast.last_killer = "";
// 		monsterBroadcast.next_revive = 0;
// 		monsterBroadcast.kingdom = GetRunnerId();
// 		GAME_SERVICE.updateMonsterBroadcast(monsterBroadcast);
// 		GAME_SERVICE.broadcastEventBossRevive( GetRunnerId(), getMapId(), getMid() );
// 	}
}

void Monster::updateState()
{
	switch (m_state)
	{
	case MS_STAND: onStand(); break;
	case MS_WALK_AROUND: onWalkAround(); break;
	case MS_WALK_ATTACK: onWalkAttack(); break;
	case MS_WALK_HOME: onWalkHome(); break;
	case MS_ATTACK: onAttack(); break;
	case MS_CORPSE: onCorpse(); break;
	case MS_GHOST: onGhost(); break;
	case MS_RUN_ON_ROAD: onRunOnRoad(); break;
	default: break;
	}
}

void Monster::onStand()
{
	switch (m_cfgmonster.ai_style)
	{
	case MAS_PEACE:		return; //do nothing...
	case MAS_ACTIVE:
		{
			int64_t nowTime=getTick();
			UnitVector targets;
			if(nowTime-m_attackTick>=m_attackTime)
			{
				m_attackTick =nowTime;
				if ((m_cfgmonster.ai_target&(1<<ET_PLAYER)) != 0)
				{
					Player *player = m_pMap->getEnemySidePlayer(this);
					if (player != NULL)
					{
						CObjPet* pPet = player->GetCharPet().GetFightPet();
						if (RANDOM.generate(1, 2)  <= 1 && pPet != NULL)
						{
							targets.push_back(pPet);
						}
 						else
 						{
 							targets.push_back(player);
 						}	
					}
				}

				if ((m_cfgmonster.ai_target&(1<<ET_MONSTER)) != 0)
				{
					Monster *monster = m_pMap->getEnemySideMonster(this);
					if (monster != NULL)
					{
						targets.push_back(monster);
					}
				}
			}
			if (!targets.empty())
			{
				random_shuffle(targets.begin(), targets.end());
				Unit *target = targets.front();
				if (target != NULL)
				{
					m_target.id = target->getUnitId();
					m_target.type = target->getType();
					setState(MS_ATTACK);
				}
			}
			else if (getTick() - m_standTick >= m_standTime)
			{
				//revive();
			}
		}
		break;
	case MAS_PASSIVE:
		{
			if (m_pMap != NULL && getTick() - m_standTick >= m_standTime && m_cfgmonster.standby == 0)
			{
				if (!(getCurrentTile() == Position(m_cfgmapmonster.x, m_cfgmapmonster.y)))
				{
					backHome();
				}
				else if (RANDOM.generate(1, 5) == 3)
				{
					Direction direciton = m_directions[RANDOM.generate(0, 7)];
					int32_t steps = RANDOM.generate(3, 5);

					Position targetTile(getCurrentTile());

					for (int32_t i = 0; i < steps; ++i)
					{
						if (!m_pMap->isWalkablePosition(targetTile.x, targetTile.y))
						{
							break;
						}
						targetTile = Map::getAroundTile(targetTile.x, targetTile.y, direciton);
					}

					setTargetTile(targetTile.x, targetTile.y);
					broadcastMove();
					setState(MS_WALK_AROUND);
				}
				else
				{
					setState(MS_STAND);
				}
			}
		}
		break;
	case MAS_TOWER:
		{
			int64_t nowTime=getTick();
			UnitVector targets;
			if(nowTime-m_attackTick>=m_attackTime)
			{
				m_attackTick =nowTime;
				Unit* pUnit = m_pMap->GetNearestAliveEnemySideUnit( this );
				if ( pUnit != NULL )
				{
					m_target.id = pUnit->getUnitId();
					m_target.type = pUnit->getType();
					setState(MS_ATTACK);
				}
			}
		}
		break;
	default:
		break;
	}
}

void Monster::onWalkAround()
{
	if (!isMoving())
	{
		setState(MS_STAND);
	}
}

void Monster::onWalkAttack()
{
	Unit *pUnit = GAME_SERVICE.getUnit( m_target.id, m_target.type, GetRunnerId() );
	if (pUnit == NULL || !pUnit->isAlive() || pUnit->getMap() != getMap() || getCurrentTile().tileDistance(Position(m_cfgmapmonster.x, m_cfgmapmonster.y)) > getMoveRange() || m_pCfgSkill == NULL)
	{
		m_target.clear();
		if (getSide() == 1)
		{
			setState(MS_STAND);
			return;
		}
		else
		{
			backHome();
			return;
		}
	}

	if (getCurrentTile().tileDistance(pUnit->getCurrentTile()) <= m_pCfgSkill->distance+1)
	{
		setState(MS_ATTACK);
		return;
	}

	if ( HasBuffState( CObjState::OBS_BINGDONG ) || HasBuffState( CObjState::OBS_YUNXUAN ) )
	{
		return;
	}

	if (!isMoving())
	{
		Position pos = Map::getAroundTile(pUnit->getCurrentTile().x, pUnit->getCurrentTile().y, static_cast<Direction>(RANDOM.generate(DOWN, DOWN_LEFT)));
		if (m_pMap->isWalkablePosition(pos.x, pos.y))
		{
			Position targetPos = m_pMap->firstLinePosition(getCurrentTile(), pos);
			setTargetTile(targetPos.x, targetPos.y);
			broadcastMove();
		}
	}
}

void Monster::onWalkHome()
{
	if (!isMoving())
	{
		if (getSide() == 1)
		{
			setAtTile(m_pMap, m_cfgmapmonster.x, m_cfgmapmonster.y);

			m_killer.clear();
			m_target.clear();
			m_lastActionTick = 0;

			m_standTick = 0;
			m_standTime = 0;
			m_corpseTick = 0;
			m_ghostTick = 0;

			setState(MS_STAND);
		}
		else if ( m_cfgmonster.hpPercent <= 1000 )
		{
			// FillHP();
			if ( m_cfgmonster.hpPercent > 0 )
			{
				AddHP( GetMaxHP() * m_cfgmonster.hpPercent / 1000 );
			}

			m_killer.clear();
			m_target.clear();
			m_pCfgSkill = NULL;
			for ( int32_t i = 0; i< MAX_MONSTER_SKILL; ++i)
			{
				m_cfgmonster.unique_skill[i].done = false;
			}
			m_firstDamageTime = 0;
			m_lastActionTick = 0;

			m_standTick = 0;
			m_standTime = 0;
			m_corpseTick = 0;
			m_ghostTick = 0;

			setState(MS_STAND);
		}
		else
		{
			revive();
		}
	}
}

void Monster::onAttack()
{
	if (getTick() > m_throwedTick)
	{
		if (!m_target.isEmpty())
		{
			Unit *pUnit = GAME_SERVICE.getUnit( m_target.id, m_target.type, GetRunnerId() );
			if (pUnit != NULL && pUnit->isAlive() && pUnit->getMap() == m_pMap && m_pCfgSkill != NULL)
			{
				Player *pPlayer = dynamic_cast<Player*>(pUnit);
				if ( NULL != pPlayer )
				{
					if ( pPlayer->GetStartProtect() > 0 )
					{
						return;
					}
				}
				if ( HasBuffState( CObjState::OBS_BINGDONG ) || HasBuffState( CObjState::OBS_YUNXUAN ) )
				{
					return;
				}

				int32_t tileDistance = getCurrentTile().tileDistance(pUnit->getCurrentTile());
				if (tileDistance > m_pCfgSkill->distance+1)
				{
					if ( m_cfgmonster.ai_style == MAS_TOWER )
					{
						m_target.clear();
						setState( MS_STAND );
						return;
					}
					else if (tileDistance > getMoveRange())
					{
						backHome();
						return;
					}
					else
					{
						Position pos = Map::getAroundTile(pUnit->getCurrentTile().x, pUnit->getCurrentTile().y, static_cast<Direction>(RANDOM.generate(DOWN, DOWN_LEFT)));
						if (m_pMap->isWalkablePosition(pos.x, pos.y))
						{
							Position targetPos = m_pMap->firstLinePosition(getCurrentTile(), pos);
							setTargetTile(targetPos.x, targetPos.y);
							broadcastMove();
							setState(MS_WALK_ATTACK);
							return;
						}
					}
				}

				if (getTick() - m_lastActionTick < PLAYER_GENERAL_CD_MS*2)
				{
					return;
				}
				Unit *pTarget = GAME_SERVICE.getUnit( m_target.id, m_target.type, GetRunnerId() );
				if ( pTarget == NULL )
				{
					m_target.clear();
					return;
				}
				setStand();
				Skill skill(m_pCfgSkill->id, 1);
				int32_t attackValue = 0;
				switch( m_pCfgSkill->range )
				{
				case SR_SELF:
					{
						skill.unitAction( *this, getHandle(), attackValue );
					}
					break;
				case SR_SOLO:
					{
						// ��⼼���Ƿ���Զ�Ŀ��ʩ��
						skill.unitAction( *this, m_target, attackValue );
						if ( !m_pCfgSkill->beneficial && m_target.type == ET_PLAYER )
						{
							Player *victm = static_cast<Player*>( pTarget );
							if (victm != NULL)
							{
								victm->onDamagedEvent( getHandle(), attackValue );
							}
						}
					}
					break;
				default:
					{
						Position pos = pTarget->getCurrentTile();
						if ( m_pCfgSkill->range == SR_HALF_ROUND || m_pCfgSkill->range == SR_LINE )
						{
							resetDirection( pos.x, pos.y );
						}
						UnitVector targets = m_pMap->getSkillTargets( *this, pos, m_pCfgSkill );;

						EntityIdInt32Map attackValues;
						skill.posAction(*this, pos, targets, attackValues);
						if (!m_pCfgSkill->beneficial)
						{
							for (UnitVector::iterator it = targets.begin(); it != targets.end(); ++it)
							{
								Unit *target = *it;
								if ( target != NULL && target->getType() == ET_PLAYER )
								{
									Player *victm = GAME_SERVICE.getPlayer( target->getUnitId(), GetRunnerId() );
									if (victm != NULL)
									{
										victm->onDamagedEvent( getHandle(), attackValues[target->getUnitId()] );
									}
								}
							}
						}
					}
					break;
				}
				m_lastActionTick = getTick();

				if (m_cfgmonster.type == MT_TRAP)
				{
					setHP( 0 );		// ����ֻ��ʹ��һ��
					return;
				}
				else
				{
					refreshSkill();
				}
			}
			else
			{
				m_target.clear();
			}
		}

		if (m_target.isEmpty())
		{
			int32_t tileDistance = getCurrentTile().tileDistance(Position(m_cfgmapmonster.x, m_cfgmapmonster.y));
			if (tileDistance > getMoveRange())
			{
				backHome();
			}
			else
			{
				setState(MS_STAND);
			}
		}
	}
}

bool Monster::checkSkillTarget( CfgSkill *pCfg, Unit *pUnit )
{
	if ( NULL == pCfg || NULL == pUnit || NULL == m_pMap )
	{
		return false;
	}
	if ( !pUnit->isAlive() )
	{
		return false;
	}
	if ( pUnit->getType() == ET_PLAYER )
	{
		if ((m_cfgmonster.ai_target&(1<<ET_PLAYER)) == 0)
		{
			return false;
		}
		Player *player = dynamic_cast<Player*>(pUnit);
		if (player == NULL)
		{
			return false;
		}
		if ( m_pMap->inSafeRegion( player->getCurrentTile() ) )
		{
			return false;
		}
		if ( getSide() == 1)
		{
			return false;
		}
		return true;
	}
	else if ( pUnit->getType() == ET_MONSTER )
	{
		if ((m_cfgmonster.ai_target&(1<<ET_MONSTER)) == 0)
		{
			return false;
		}
		Monster *monster = dynamic_cast<Monster*>(pUnit);
		if (monster == NULL)
		{
			return false;
		}
		if ( GetFamilyId() == monster->GetFamilyId() )
		{
			return false;
		}
		return true;
	}
	else if ( pUnit->getType() == ET_PET )
	{
		if ((m_cfgmonster.ai_target&(1<<ET_PLAYER)) == 0)
		{
			return false;
		}
		CObjPet* pPet = dynamic_cast<CObjPet*>(pUnit);
		if ( NULL == pPet )
		{
			return false;
		}
		CharId_t ownerId = pPet->getOwner();
		Player *player = GAME_SERVICE.getPlayer(ownerId, GetRunnerId());
		if (player == NULL)
		{
			return false;
		}
		if ( m_pMap->inSafeRegion(player->getCurrentTile()) )
		{
			return false;
		}
		if ( getSide() == 1 )
		{
			return false;
		}
		return true;
	}
	return false;
}

void Monster::onCorpse()
{
	if ( m_cfgmonster.type == 1 )
	{
		leaveMap();
		setState(MS_GHOST);
	}
	else
	{
		if (getTick() - m_corpseTick >= m_cfgmonster.corpse_time)
		{
			leaveMap();
			setState(MS_GHOST);
		}
	}
}

void Monster::onGhost()
{
	if ( m_cfgmonster.boss_sign == BOSS_TYPE_SPIDER_QUEEN )
	{
		if ( !SpiderQueenCanRevive() )
		{
			return;
		}
	}
	if ( m_cfgmonster.mid == WAR_VICTORY_BOSS_ID )
	{
		if ( !WarVictoryBossCanRevive() )
		{	
			return;
		}
	}
	else if ( getTick() - m_ghostTick < getReviveTime() )
	{
		return;
	}
		
	int32_t ReviveX	= m_cfgmapmonster.x;
	int32_t ReviveY = m_cfgmapmonster.y;
	if ( m_cfgmonster.boss_sign != 0 )
	{
		Position Pos = WORLDBOSS.GetRevivePos( m_cfgmapmonster.id );
		if ( Pos.x > 0 && Pos.y > 0 )
		{
			ReviveX		 = Pos.x;
			ReviveY		 = Pos.y;
		}
		WORLDBOSS.UpdateBossInfo( m_cfgmapmonster.id, 0, 0, "" );
	}
	revive();
	setAtTile(m_pMap, ReviveX, ReviveY);

	broadcastIntoMap();

	setState(MS_STAND);
	if ( m_cfgmonster.mid == WAR_VICTORY_BOSS_ID )
	{
		WORLDBOSS.GongGao( BCI_WAR_VICTORY_BOSS_REVIVE );
	}
}

void Monster::onRunOnRoad()
{
}

void Monster::broadcastBasicData()
{
	Unit::broadcastBasicData();
}

void Monster::setState(MonsterState state)
{
	switch (state)
	{
	case MS_STAND: 
		{
			m_attackTick = getTick();
			m_attackTime = RANDOM.generate(10, 200);
			m_standTick = m_attackTick; 
			m_standTime = RANDOM.generate(20000, 30000);
		}
		break;
	case MS_WALK_AROUND: break;
	case MS_WALK_ATTACK: break;
	case MS_WALK_HOME: 
		{
			m_CanReverHp = true;
		}
		break;
	case MS_ATTACK: if (m_pCfgSkill == NULL) refreshSkill(); break;
	case MS_CORPSE: m_corpseTick = getTick(); break;
	case MS_GHOST: m_ghostTick = getTick(); break;
	case MS_RUN_ON_ROAD: break;
	default: LOG_ERROR("Monster::setState unknown state %d\n", state);
	}

	m_state = state;
}

void Monster::die()
{
	broadcastBasicData();
	
	setState(MS_CORPSE);

	DropItem dropItems[MAX_DROPITEM_SIZE] = {};

	Player *player = getKiller();
	if ( player != NULL )
	{	
		if ( m_cfgmonster.boss_sign != 0 )
		{
			player->GetTask().updateTaskCount( TC_KILL_BOSS_COUNT );
			if ( m_cfgmonster.BossSocre > 0 )
			{
				player->AddCurrency(CURRENCY_BOSS,m_cfgmonster.BossSocre , SCR_KILL_BOSS);
			}
			WORLDBOSS.UpdateBossInfo( m_cfgmapmonster.id, getNow(), player->getCid(), player->getName(), WORLDBOSS.GetSpiderQueenReviveTime() );
			int32_t ReviveTime = WORLDBOSS.GetBossRevieTime( m_cfgmapmonster.id );
			if ( ReviveTime > 0 )
			{
				m_ReviveTime = ReviveTime;
			}

			if ( m_cfgmonster.mid == WAR_VICTORY_BOSS_ID )
			{
				WORLDBOSS.GongGao( BCI_WAR_VICTORY_BOSS_KILLED , player );
			}

			LogBoss logBoss = {};
			logBoss.mid = m_cfgmonster.mid;
			logBoss.type = m_cfgmonster.boss_sign;
			logBoss.killer = player->getCid();
			logBoss.time = player->getNow();

			DB_SERVICE.InsertBossLog( logBoss );
		}
		if( player->getMap() == getMap() )
		{

			TaskShare( player->getCid() );
			player->onKillMonster( getMid(), getLevel(), m_cfgmonster.exp, isBoss() );
			player->GetCharPet().RemoveTarget( UnitHandle( getUnitId(), ET_MONSTER ) );
			player->GetPlayerKillMonsterTongJi().AddKillCount( m_cfgmonster.group_id, getMid(), m_cfgmonster.boss_sign );
		}
		else
		{
			int32_t mapId = player->getMapId();
			LOG_INFO("monster die not the same Map by player! %d,%d,%d\n",getMapId(),mapId,getMid());
		}
		generateFightDrop( dropItems,player );
	}
	else
	{
		if ( m_cfgmonster.boss_sign != 0 )
		{
			WORLDBOSS.UpdateBossInfo( m_cfgmapmonster.id, getNow(), 0, "", WORLDBOSS.GetSpiderQueenReviveTime() );
		}
		if ( m_DieType != 1 )
		{
			generateFightDrop( dropItems, NULL );
		}
	}



	if (m_pMap != NULL)
	{
		if (dropItems[0].itemId > 0 && dropItems[0].itemClass > 0)
		{
			CDropItemGroup *pDropItemGroup = POOL_MANAGER.pop<CDropItemGroup>();
			if (NULL == pDropItemGroup)
			{
				return;
			}

			if (player != NULL && player->getMap() == getMap() )
			{
				pDropItemGroup->init(m_pMap, getCurrentTile(), player, ET_MONSTER, m_cfgmonster.mid, dropItems, "",m_cfgmonster.broadcast, m_cfgmonster.drop_free );
			}
			else
			{
				pDropItemGroup->init(m_pMap, getCurrentTile(), NULL, ET_MONSTER, m_cfgmonster.mid, dropItems, "",m_cfgmonster.broadcast, m_cfgmonster.drop_free);
			}

			m_pMap->addDropItemGroup(pDropItemGroup);
		}

		if (player != NULL && player->getMap() == getMap())
		{
			m_pMap->onMonsterDie(this, player);
		}
	}
}

void Monster::TaskShare( CharId_t KillerId )
{
	std::set<CharId_t>::iterator it = HurtCharId.begin();
	for ( ; it != HurtCharId.end(); ++it )
	{
		if ( *it == KillerId )
		{
			continue;
		}
		Player* pPlayer = GAME_SERVICE.getPlayer( *it, GetRunnerId() );
		if ( NULL != pPlayer && NULL != m_pMap )
		{
			if ( pPlayer->getMapId() == m_pMap->GetId() )
			{
				pPlayer->GetTask().updateTaskMonster( m_cfgmonster.mid, m_cfgmonster.level );
				pPlayer->GetCharTaskCycle().UpdateTaskMonster( m_cfgmonster.mid, m_cfgmonster.level );
			}
		}
	}
}

void Monster::revive()
{
	FillHP();

	m_killer.clear();
	m_target.clear();
	m_pCfgSkill = NULL;
	for ( int32_t i = 0; i< MAX_MONSTER_SKILL; ++i)
	{
		m_cfgmonster.unique_skill[i].done = false;
	}
	m_firstDamageTime = 0;
	m_lastActionTick = 0;

	m_standTick = 0;
	m_standTime = 0;
	m_corpseTick = 0;
	m_ghostTick = 0;
	m_DieType	= 0;
	setState(MS_STAND);
}

void Monster::generateFightDrop(DropItem (&dropItems)[MAX_DROPITEM_SIZE], Player* pPlay )
{
	int32_t index = 0;
	std::vector<int32_t> groupIds;
	CfgMonsterGroupDropVector *pGroups = CFG_DATA.getMonsterGroupDrop(getMid());
	if (pGroups != NULL)
	{
		int32_t nowMinute = getMinute();

		for (CfgMonsterGroupDropVector::iterator it = pGroups->begin(); it != pGroups->end(); ++it)
		{
			if ( pPlay != NULL )
			{
				if ( it->job != 0 && it->job != pPlay->getJob() )
				{
					continue;
				}
			}

			if (nowMinute >= it->begin_time && nowMinute <= it->end_time)
			{
				for (int32_t i = 0; i < it->repeat; ++i)
				{
					int32_t groupProbability = RANDOM.generate(1, 100000);
					int32_t probability = it->probability;
					// �Ǹ������� ���ݵȼ����͵���
					if ( !isDungeonMonster() )
					{
						if ( m_cfgmonster.boss_sign != 0 )
						{
							if ( m_killerLevel - m_cfgmonster.level >= 60 )
							{
								probability = static_cast<int32_t>(probability / 5);
							}
							else if ( m_killerLevel - m_cfgmonster.level >= 30 )
							{
								probability = static_cast<int32_t>(probability / 2);
							}
						}
						else
						{
							if ( m_killerLevel - m_cfgmonster.level >= 90 )
							{
								probability = static_cast<int32_t>(probability / 100);
							}
// 							else if ( m_killerLevel - m_cfgmonster.level >= 10 )
// 							{
// 								probability = static_cast<int32_t>(probability / 2);
// 							}
						}
					}
					if (probability >= groupProbability)
					{
						groupIds.push_back(it->group_id);
					}
				}
			}
		}
	}
	CfgActivityDropTable *pActivityDrops = CFG_DATA.getActivityDrops();
	if (pActivityDrops != NULL)
	{
		for (CfgActivityDropTable::const_iterator it = pActivityDrops->begin(); it != pActivityDrops->end(); ++it)
		{
			if (ACTIVITY_MANAGER.IsActivityRunning(it->second.activity_id) && it->second.probability >= RANDOM.generate(1, 1000) && getLevel() >= it->second.monster_min_level)
			{
				groupIds.push_back(it->second.drop_group_id);
			}
		}
	}
	for (std::vector<int32_t>::const_iterator it = groupIds.begin(); it != groupIds.end(); ++it)
	{
		DropItem dropItem = dropItemFromGroup(*it);
		if (dropItem.itemId > 0 && dropItem.itemClass > 0)
		{
			dropItems[index++] = dropItem;
			if (index >= MAX_DROPITEM_SIZE)
			{
				return;
			}	
		}
	}
	if ( isDungeonMonster() || isActivityMonster() )
	{
		return;
	}
	//������ҵ���
	int32_t MaxRate = 1000;
	if ( getLevel() < 60 )
	{
		MaxRate *= 5;
	}
	int32_t DropTimes = 0;
	int32_t Rate = RANDOM.generate( 1, MaxRate);
	if ( Rate <= 45 )
	{
		DropTimes = 1;
	}
	else if ( Rate <= 50 ) //��һ�ؽ��
	{
		DropTimes    = 6;
	}
	else
	{
		return;
	}
	int32_t DropCount = 0;
	if ( m_cfgmonster.level <= 80 )
	{
		DropCount = (m_cfgmonster.level * 7200 + 50000)/180;
	}
	else 
	{
		int32_t Level = m_cfgmonster.level;
		if ( Level > 300 )
		{
			Level = 300;
		}
		DropCount = (Level * 7200 + 300000)/180;
	}
	int32_t MaxDropCount = static_cast<int32_t>(DropCount + DropCount * 0.3);
	int32_t MinDropCount = static_cast<int32_t>(DropCount - DropCount * 0.3);
	for ( int32_t i = 0; i < DropTimes; i++ )
	{
		int32_t PracticalCount = RANDOM.generate( MinDropCount, MaxDropCount );
		for ( int32_t i = 0; i < MAX_DROPITEM_SIZE; i++ )
		{
			if ( dropItems[i].itemId <= 0 || dropItems[i].itemCount <= 0 )
			{
				if ( PracticalCount > 0 )
				{
					if ( DropTimes == 6 )
					{
						PracticalCount *= 3;
					}
					dropItems[i].itemClass = IC_CURRENCY;
					dropItems[i].itemId	   = CURRENCY_MONEY_ID;
					dropItems[i].itemCount = PracticalCount;
				}
				break;
			}	
		}	
	}
}

DropItem Monster::dropItemFromGroup(int32_t groupId)
{
	DropItem dropItem = {};

	CfgMonsterDropGroupVector *pSpecialDropGroup = CFG_DATA.getMonsterDropGroup(groupId);
	if (pSpecialDropGroup != NULL)
	{
		int32_t roll = RANDOM.generate(1, 1000);
		CfgMonsterDropGroupVector::iterator iter = pSpecialDropGroup->begin();
		CfgMonsterDropGroupVector::iterator eiter = pSpecialDropGroup->end();
		for ( ; iter != eiter; ++iter )
		{
			if ( iter->probability >= roll )
			{
				dropItem.itemId		= iter->item_id;
				dropItem.itemClass	= iter->item_class;
				dropItem.bindType	= iter->bind_type;
				dropItem.costType	= iter->cost_type;
				dropItem.costValue	= iter->cost_value;
				dropItem.itemCount	= 1;
				break;
			}
			else
			{
				roll -= iter->probability;
			}
		}
	}

	return dropItem;
}

int32_t Monster::getMinute()
{
	tm tmnow = {};
	if (m_pMap != NULL)
	{
		tmnow = m_pMap->getLocalNow();
	}
	else
	{
		tmnow = getLocalNow();
	}

	return tmnow.tm_hour*60 + tmnow.tm_min;
}

void Monster::refreshSkill()
{
	int32_t hpPercent = roundInt( ( GetHP() * 1.0f / GetMaxHP() ) * 100.0f );
	{
		for ( int32_t i = 0; i< MAX_MONSTER_SKILL; ++i)
		{
			if ( m_cfgmonster.unique_skill[i].maxHp >= hpPercent && m_cfgmonster.unique_skill[i].minHp <= hpPercent && !m_cfgmonster.unique_skill[i].done )
			{
				m_cfgmonster.unique_skill[i].done = true;
				m_pCfgSkill = CFG_DATA.getSkill(m_cfgmonster.unique_skill[i].skillid);
				return;
			}
		}
	}
	{
		for ( int32_t i = 0; i< MAX_MONSTER_SKILL; ++i)
		{
			if (m_cfgmonster.random_skill[i].maxHp >= hpPercent && m_cfgmonster.random_skill[i].minHp <= hpPercent)
			{
				m_pCfgSkill = CFG_DATA.getSkill(RANDOM.generate(1, 10) == 5 ? m_cfgmonster.random_skill[i].skillid : m_cfgmonster.skill_id);
				return;
			}
		}
	}

	m_pCfgSkill = CFG_DATA.getSkill(m_cfgmonster.skill_id);
}

int32_t Monster::move(int16_t currentX, int16_t currentY, int16_t targetX, int16_t targetY, int16_t finalX, int16_t finalY, Direction direction)
{
	if (m_pMap == NULL)
	{
		return ERR_INVALID_DATA;
	}

	Tile *pCurrentTile = m_pMap->getTile(currentX, currentY);
	Tile *pTargetTile = m_pMap->getTile(targetX, targetY);
	Tile *pFinalTile = m_pMap->getTile(finalX, finalY);

	if (pCurrentTile == NULL || pTargetTile == NULL || pFinalTile == NULL)
	{
		return ERR_INVALID_DATA;
	}

	//if (getCurrentTile().tileDistance(Position(currentX, currentY)) > PULL_BACK_DISTANCE)
	//{
	//	instantMove(getCurrentTile().x, getCurrentTile().y, IMR_PULL_BACK);
	//	return ERR_INVALID_DATA;
	//}

	setAtTile(m_pMap,targetX,targetY);
	broadcastKickOutedMove();
//	setTargetTile(targetX, targetY);
//	broadcastMove();

	return ERR_OK;
}

void Monster::backHome()
{
	if ( HasBuffState( CObjState::OBS_BINGDONG ) || HasBuffState( CObjState::OBS_YUNXUAN ) )
	{
		return;
	}
	m_target.clear();
	setTargetTile(m_cfgmapmonster.x, m_cfgmapmonster.y);
	broadcastMove();
	setState(MS_WALK_HOME);
}

bool Monster::SpiderQueenCanRevive()
{	
	tm LocalNow = DayTime::localnow();
	int32_t Minute = LocalNow.tm_hour * 60 + LocalNow.tm_min;
	if ( m_lastUpdateMinute == Minute )
	{
		return false;
	}
	m_lastUpdateMinute = Minute;
	for ( int32_t i = 0; i < ReviveTimeCount; i++ )
	{
		if ( Minute == ReviveTime[i] )
		{
			return true;
		}
	}
	return false;
}

void Monster::SetDieType( int8_t Type )
{
	m_DieType = Type;
}

void Monster::CheckAddHp( int64_t CurTick )
{
	if ( !isBoss() )
	{
		return;
	}
	if ( !m_CanReverHp )
	{
		return;
	}
	if ( GetHP() >= GetMaxHP() )
	{
		return;
	}
	if ( CurTick - m_LastReverHpTick > MONSTER_REVER_HP_CD )
	{
		int32_t AddCount = GetMaxHP() / 100 * 5;
		AddHP( AddCount );
		m_LastReverHpTick = CurTick;
	}
}

bool Monster::WarVictoryBossCanRevive()
{
	WarVictoryHd* pWarVictory = CFG_DATA.GetWarVictoryHdCfg( 1 );
	if ( NULL == pWarVictory )
	{
		return false;
	}
	int32_t NowTime = TIMER.GetNow();
	if ( NowTime < pWarVictory->StartTime || NowTime > pWarVictory->EndTime )
	{
		return false;
	}
	tm LocalNow = DayTime::localnow();
	int32_t Minute = LocalNow.tm_hour * 60 + LocalNow.tm_min;
	if ( m_lastUpdateWarVictoryBossMinute == Minute )
	{
		return false;
	}
	m_lastUpdateWarVictoryBossMinute = Minute;
	for ( int32_t i = 0; i < WarVictoryBossReviveTimeCount; i++ )
	{
		if ( Minute == WarVictoryBossReviveTime[i] )
		{
			return true;
		}
	}
	return false;
}
