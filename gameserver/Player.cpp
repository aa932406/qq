#include "stdafx.h"
#include "PDUDefine.h"
#include "ActivityManager.h"
#include "DBService.h"
//#include "ActPKMap.h"
#include "ActivityMap.h"
#include "DropItem.h"
#include "GameService.h"
#include "ItemEffect.h"
//#include "Kingdom.h"
#include "Map.h"
#include "MapManager.h"
#include "Monster.h"
#include "Npc.h"
#include "PersistBuff.h"
#include "Plant.h"
#include "Player.h"
#include "Skill.h"
#include "Trap.h"
#include "Trailer.h"
#include "EquipManager.h"
#include "PoolManager.h"
#include "EquipRansom.h"
#include "WorldBoss.h"
#include "DaTiHD.h"
#include "FamilyWar.h"
#include "HorseRacing.h"
#include "VipGuaJi.h"
#include "KaiFuHuoDong.h"
#include "PetManager.h"
#include "GMBackstage.h"
#include <algorithm>
#include <string>
#include <iostream>

using namespace Answer;
using namespace std;
Player::NetPacketHandler Player::m_netPacketHandlers[CM_GAME_MAX];
Player::Player()
	: Unit(ET_PLAYER)
{
	m_saveDataTimeCount =10 * 60 * 1000;

	InitExtSystems();
}
Player::~Player()
{

}
void Player::refresh()
{
	checkSaveToDB();	//每隔15分钟保存数据
	checkNetPackets();	//处理事件
	checkTrailer();		//镖车
	checkPool();
	minuteCheck(true);
	checkSyncStatus();
	checkDie();

	if ( checkUseSkill() )
	{
		m_useSkill.CleanUp();
	}

	// 扩展系统心跳
	m_ExtSysMgr.OnUpdate( this->getTick() );
}

EntityId_t Player::getUnitId() const
{
	return getCid();
}

CharId_t Player::getOwner() const
{
	return getCid();
}

int32_t Player::getLevel() const
{
	return m_chr.level;
}

int32_t Player::GetCalLevel() const
{
	return getLevel() > 130 ? ( 130 + GetSoulLevel() ) : getLevel();
}

void Player::SetLevel(int32_t Level)
{
	m_chr.level = Level;
}

int32_t Player::getDeadTime() const
{
	return getRecord(PR_DEAD_TIME);
}

bool Player::IsDead() const
{
	return m_bDie;
}

int64_t Player::GetDieTick() const
{
	return m_nDieTick;
}

void Player::checkDie()
{
	if( !m_bDie && !isAlive() )
	{
		m_bDie = true;
		m_nDieTick = getTick();
		updateRecord( PR_DEAD_TIME, getRecord( PR_DEAD_TIME ) + 1 );
		if( m_pMap != NULL )
		{
			m_pMap->onPlayerDie( this );
		}
		m_extCharPet.FreeAllFightPet( true );
	}
}

void Player::postDamage(int32_t damge, UnitHandle launcher)
{
	BreakGather();

	// 玩家死亡
	if (!isAlive())
	{
		if (launcher.type == ET_PLAYER ||launcher.type == ET_PET )
		{
			Player *killer = NULL;
			if ( launcher.type == ET_PLAYER )
			{
				killer = GAME_SERVICE.getPlayer( launcher.id, GetRunnerId() );
			} 
			else if ( launcher.type == ET_PET )
			{
				CPet* pPet = PET_MANAGER.GetPet( launcher.id );
				if ( NULL == pPet )
				{
					return;
				}
				int64_t OwnerId = pPet->GetOwnerId();
				killer = GAME_SERVICE.getPlayer( OwnerId, GetRunnerId() );
			}	
			if ( killer != NULL )
			{
				m_pMap->onPlayerKilled( this, killer );
				if ( !InActivity() ||   m_pMap->GetMapParam() == FAMILY_LIGHT_MAP_PARA )
				{
					onDropItem( killer );
					killer->addKillPlayerCount();
					killer->updateRecord(RP_DAILY_PK_MARK,killer->getRecord(RP_DAILY_PK_MARK)+5);
					UpdateKilledByPlayer( killer->getCid() );
				}
				updatePkValue( killer, m_pMap );
				DieResetXp();
				m_extCharSkill.DieResetXp();
				//sendKilledByPlayer(killer->getCid(), killer->getName());
			}
		}
		else if (launcher.type == ET_MONSTER)
		{
			// 处理掉落
			if (!InActivity() )
			{
				Monster* pMonster = GAME_SERVICE.getMonster( launcher.id, GetRunnerId() );
				if ( NULL != pMonster )
				{
					onDropItem( NULL, pMonster->getMid() );
				}
			}
			DieResetXp();
			m_extCharSkill.DieResetXp();
// 			if (InDungeon())
// 			{
// 				dynamic_cast<Dungeon*>(m_pMap)->onPlayerDie(this);
// 			}
		}
	}
}

bool Player::isFriendSide(Unit *pUnit)
{
	if (pUnit == NULL)
	{
		return false;
	}

	if (pUnit->getType() == ET_PLAYER || pUnit->getType() == ET_PET )
	{
		Player *player = NULL;
		if (pUnit->getType() == ET_PET)
		{
			CObjPet* pPet = static_cast<CObjPet*>(pUnit);
			if (pPet == NULL)
			{
				return false;
			}
			else
			{
				player = GAME_SERVICE.getPlayer(pPet->getOwner(), GetRunnerId());
			}
		}
		else
		{
			player = dynamic_cast<Player*>(pUnit);
		}
		
		if (player == NULL)
		{
			return false;
		}
		
		if (player == this)
		{
			return true;
		}

		if (  player->GetStartProtect() > 0 )
		{
			return true;
		}
		if ( player->getPkProtectTime() > 0 )
		{
			return true;
		}
		switch (getPkMode())
		{
		case PK_MODE_PEACE:
			{
				return true;
			}
		case PK_MODE_LEGION:
			{
				return getFamilyId() > 0 && getFamilyId() == player->getFamilyId();
			}
		case PK_MODE_TEAM:
			{
				if ( IsInTeam() && GetTeamId() == player->GetTeamId() )
				{
					return true;
				}
				return false;
			}
		case PK_MODE_GOOD_EVIL:
			{
				if  ( player->getPkValue() > PK_VALUE_RED || player->GetFightChecker().IsBuleName())
				{
					return false;
				}
				return true;
			}
		case PK_MODE_SERVER:
			{
				if ( getSid() == player->getSid() )
				{
					return true;
				}
				return false;
			}
		case PK_MODE_ALL:
			{
				return false;
			}
		default:
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool Player::checkSkillTarget(CfgSkill *pCfg, Unit *pUnit)
{
	if (pCfg == NULL || pUnit == NULL || m_pMap == NULL || pUnit->getMap() != m_pMap)
	{
		return false;
	}

	if (!pUnit->isAlive() && pCfg->special != SS_FUHUO)
	{
		return false;
	}

	if (pCfg->beneficial)
	{
		return isFriendSide(pUnit);
	}
	else
	{
		if (pUnit->getType() == ET_PLAYER)
		{
			if (getLevel() < PK_PROTECT_LEVEL)
			{
				return false;
			}

			Player *player = dynamic_cast<Player*>(pUnit);
			if (player == NULL)
			{
				return false;
			}

			if (player == this)
			{
				return false;
			}

			if (player->getLevel() < PK_PROTECT_LEVEL)
			{
				return false;
			}

			//safeRegon
			if (m_pMap->inSafeRegion(player->getCurrentTile()) || m_pMap->inSafeRegion(getCurrentTile()))
			{
				return false;
			}

			return !isFriendSide(player);
		}
		else if (pUnit->getType() == ET_PET)
		{
			if ( getLevel() < PK_PROTECT_LEVEL )
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

			if (player == this)
			{
				return false;
			}

			if (player->getLevel() < 30)
			{
				return false;
			}

			if (m_pMap->inSafeRegion(player->getCurrentTile()) || m_pMap->inSafeRegion(getCurrentTile()))
			{
				return false;
			}

			return !isFriendSide(player);
		}
		else if (pUnit->getType() == ET_MONSTER)
		{
			Monster *monster = dynamic_cast<Monster*>(pUnit);

			if (monster == NULL)
			{
				return false;
			}

			if ( monster->getSide() == 1)
			{
				return false;
			}

			if ( monster->GetFamilyId() > 0 && monster->GetFamilyId() == getFamilyId() )
			{
				return false;
			}

			return true;
		}
		else if (pUnit->getType() == ET_TRAILER)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

Position Player::getCurrentTile()
{
	return Unit::getCurrentTile();
}

void Player::broadcastBasicData()
{
	Unit::broadcastBasicData();
}

bool Player::isRobot() 
{
	return false;
}

void Player::SaveDBData( PlayerDBData& dbData )
{
	int32_t mapid = m_chr.mapid;
	int32_t x = m_chr.x;
	int32_t y = m_chr.y;

	if (m_pMap != NULL)
	{
		mapid = m_pMap->GetId();
		x = getCurrentTile().x;
		y = getCurrentTile().y;

		if ( InDungeon() || InActivity() || CFG_DATA.IsBossHomeMap( mapid ) || m_pMap->IsVipGuaJiMap())
		{
			mapid = m_oldPosition.mapid;
			x = m_oldPosition.x;
			y = m_oldPosition.y;
		}
	}

	int32_t hp = GetHP();
	//int32_t mp = GetAttrValue( CObjAttrs::ATTR_MP );
	if (!isAlive() && !InActivity() && !InDungeon())
	{
		if (m_pMap != NULL)
		{
			CfgMapRegion *pCfgMapRegion = CFG_DATA.getMapRegion(m_pMap->getReive( this ));
			if (pCfgMapRegion != NULL)
			{
				CfgMap* pMapCfgInfo = CFG_DATA.getMap(pCfgMapRegion->mapid);
				if(pMapCfgInfo !=NULL)
				{
					Map *pTargetMap = MAP_MANAGER.GetMap( pCfgMapRegion->mapid );
					if (pTargetMap != NULL)
					{
						Position pos = pTargetMap->getRandomWalkablePositionInRegion(*pCfgMapRegion);
						if (pos.x >= 0 || pos.y >= 0)
						{
							mapid = pTargetMap->GetId();
							x = pos.x;
							y = pos.y;

							if( pTargetMap->GetRunnerId() ==0 ) { LOG_INFO("mapID:%d",mapid);};
						}
					}
				}
				else
				{
					LOG_INFO("Map is NULL %d",mapid);
				}
			}
		}

		hp = GetMaxHP();
	}

	//if (mapid == PBA_FAMILY_MAP)
	//{
	//	Map *pTargetMap = MAP_MANAGER.getMap(KI_NEUTRAL, PBA_CENTER_CITY,0);
	//	if (pTargetMap != NULL)
	//	{
	//		CfgMapRegion *pCfgRegion = CFG_DATA.getMapRegion(pTargetMap->getReive());
	//		if (pCfgRegion != NULL)
	//		{
	//			Position  oldPosition = pTargetMap->getRandomWalkablePositionInRegion(*pCfgRegion);
	//			if (oldPosition.x != -1 && oldPosition.y != -1)
	//			{
	//				in_kingdom = KI_NEUTRAL;
	//				mapid = pTargetMap->getId();
	//				x = oldPosition.x;
	//				y = oldPosition.y;
	//			}
	//		}
	//	}
	//}

	ChrBagInfo bagInfo = m_extCharBag.GetChrBagInfo();

	// character
	dbData.chr.data.cid						= m_chr.cid;
	snprintf( dbData.chr.data.name, MAX_NAME_CCH_LENGTH, m_chr.name );
	snprintf( dbData.chr.data.familyName, MAX_NAME_CCH_LENGTH, m_chr.familyName );
	dbData.chr.data.sex						= m_chr.sex;
	dbData.chr.data.job						= m_chr.job;
	dbData.chr.data.level					= m_chr.level;
	//dbData.chr.data.family_id				= m_chr.family_id;
	//dbData.chr.data.family_position			= m_chr.family_position;
	//dbData.chr.data.family_level			= m_chr.family_level;
	dbData.chr.data.exp						= m_chr.exp;
	dbData.chr.data.exp_minus				= m_chr.exp_minus;

	dbData.chr.data.mapid					= mapid;
	dbData.chr.data.x						= x;
	dbData.chr.data.y						= y;
	dbData.chr.data.hp						= hp;
	dbData.chr.data.mp						= GetMP();
	dbData.chr.data.pp						= GetPP();
	dbData.chr.data.xp						= GetXP();

	dbData.chr.data.level_gift				= m_chr.level_gift;
	dbData.chr.data.level_gold				= m_chr.level_gold;
	dbData.chr.data.seven_login				= m_chr.seven_login;
	dbData.chr.data.head					= m_chr.head;

	dbData.chr.data.pk_mode					= m_pkMode_bk;
	dbData.chr.data.pk_value				= m_chr.pk_value;
	dbData.chr.data.pk_killed_count			= m_chr.pk_killed_count;
	dbData.chr.data.pk_time					= ( m_chr.pk_time > getNow() ) ? ( m_chr.pk_time ) : 0;
	dbData.chr.data.kill_count				= m_chr.kill_count;

	dbData.chr.data.last_task_id			= m_chr.last_task_id;
	dbData.chr.data.logout_count			= m_chr.logout_count;
	dbData.chr.data.pay_click_count			= m_chr.pay_click_count;

	m_chr.level_stay_time  += ( getNow() - m_levelStartTime );
	dbData.chr.data.level_stay_time			= m_chr.level_stay_time;
	dbData.chr.data.last_login_time			= m_chr.last_login_time;
	dbData.chr.data.last_logout_time		= m_chr.last_logout_time;
	dbData.chr.data.create_time				= m_chr.create_time;
	dbData.chr.data.leader					= m_chr.leader;

	//attr TODO:此处用于查看离线玩家需要加载这部分数据
	dbData.attrData.data.hp				= GetAttrValue( CObjAttrs::ATTR_HP );
	dbData.attrData.data.mp				= GetAttrValue( CObjAttrs::ATTR_MP );
	dbData.attrData.data.pp				= GetAttrValue( CObjAttrs::ATTR_PP );
	dbData.attrData.data.xp				= GetAttrValue( CObjAttrs::ATTR_XP );
	dbData.attrData.data.phy_atk_min	= GetAttrValue( CObjAttrs::ATTR_PHY_ATK_MIN );
	dbData.attrData.data.phy_atk_max	= GetAttrValue( CObjAttrs::ATTR_PHY_ATK_MAX );
	dbData.attrData.data.mag_atk_min	= GetAttrValue( CObjAttrs::ATTR_MAG_ATK_MIN );
	dbData.attrData.data.mag_atk_max	= GetAttrValue( CObjAttrs::ATTR_MAG_ATK_MAX );
	dbData.attrData.data.phy_def		= GetAttrValue( CObjAttrs::ATTR_PHY_DEF );
	dbData.attrData.data.mag_def		= GetAttrValue( CObjAttrs::ATTR_MAG_DEF );
	dbData.attrData.data.hitrate		= GetAttrValue( CObjAttrs::ATTR_HITRATE );
	dbData.attrData.data.dodge			= GetAttrValue( CObjAttrs::ATTR_DODGE );
	dbData.attrData.data.critrate		= GetAttrValue( CObjAttrs::ATTR_CRITRATE );
	dbData.attrData.data.dmg_add		= GetAttrValue( CObjAttrs::ATTR_DMG_ADD );
	dbData.attrData.data.dmg_rdc		= GetAttrValue( CObjAttrs::ATTR_DMG_RDC );
	dbData.attrData.data.dmg_add_pec	= GetAttrValue( CObjAttrs::ATTR_DMG_ADD_PEC );
	dbData.attrData.data.dmg_rdc_pec	= GetAttrValue( CObjAttrs::ATTR_DMG_RDC_PEC );
	dbData.attrData.data.movespeed		= GetAttrValue( CObjAttrs::ATTR_MOVE_SPEED );
	dbData.attrData.data.battle			= getBattle();
	dbData.attrData.data.lucky			= GetAttrValue( CObjAttrs::ATTR_LUCKY );

	// sysuser
	dbData.sysUser.data.uid						= m_sysUser.uid;
	dbData.sysUser.data.sid						= m_sysUser.sid;
	dbData.sysUser.data.gold_cost_total			= m_sysUser.gold_cost_total;
	dbData.sysUser.data.map_enter_time			= m_sysUser.map_enter_time;
	dbData.sysUser.data.last_login_time			= m_sysUser.last_login_time;
	dbData.sysUser.data.last_logout_time		= m_sysUser.last_logout_time;
	dbData.sysUser.data.total_login_count		= getRecord(RP_LOGIN_COUNT);
	dbData.sysUser.data.continue_login_count	= m_sysUser.continue_login_count;
	dbData.sysUser.data.total_online_time		= m_sysUser.total_online_time;
	dbData.sysUser.data.total_offline_time		= m_sysUser.total_offline_time;

	// sysUserPreventWallow
	dbData.sysUserPreventWallow.data			= m_sysUserPreventWallow;
	dbData.skillData.skillVt					= m_skills;
	//buff保存到数据库
	dbData.buffData.buffVt						= getPersistBuffs();
	m_task.SaveDBData( dbData );
	//m_taskFamily.SaveDBData( dbData );
	//m_taskKingdom.SaveDBData( dbData );

	// actionData
	memcpy( dbData.actionData.actionArry, m_actions, sizeof( m_actions ) );

	// autoFight
	dbData.autoFight.data						= m_autoFight;
	
	// systemSetting
	dbData.systemSetting.data					= GetSysSetting();
	updateRecord(RP_IN_BOSS_HOME_TIME, m_InBossHomeTime);
	// 扩展系统存储
	m_ExtSysMgr.OnSaveToDB( dbData );
}

//将玩家信息保存到数据库中
void Player::saveToDB( int32_t reason, int32_t param )
{
	PlayerDBData dbData;
	SaveDBData( dbData );

	NetPacket *packet = DB_SERVICE.popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(reason);
	packet->writeInt32(param);
	dbData.PackageData( packet );
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_SAVE_PLAYER);
	packet->setSize(packet->getWOffset());
	DB_SERVICE.sendPacket(packet);
}

//添加玩家响应事件
void Player::initNetPacketHandlers()
{
	setNetPacketHandler(CM_LOGOUT, &Player::onLogout);
	setNetPacketHandler(CM_SYNC_TIME, &Player::onSyncTime);
	setNetPacketHandler(CM_MOVE, &Player::onMove);
	setNetPacketHandler(CM_JUMP, &Player::onJump);
	setNetPacketHandler(CM_TRAILER_MOVE,&Player::onTrailerMove);//跟踪
	setNetPacketHandler(CM_HIT, &Player::onHit);
	setNetPacketHandler(CM_UNIT_THROWED,&Player::onUnitThrowed);
	setNetPacketHandler(CM_KICK_OUT_MOVE, &Player::onKickOutMove);//踢中
	setNetPacketHandler(CM_ARRIVE, &Player::onArrive);//到达
	setNetPacketHandler(CM_SWITCH_MAP, &Player::onSwitchMap);//选择地图
	//setNetPacketHandler(CM_GETMATCHINFO,&Player::onGetMatchMsg);//
	setNetPacketHandler(CM_ENTER_DUNGEON, &Player::onEnterDungeon);//进入副本
	setNetPacketHandler(CM_LEAVE_DUNGEON, &Player::onLeaveDungeon);//离开副本
	setNetPacketHandler(CM_DUNGEON_BUILD_TOWER,&Player::onDungeonBuildTower);		//副本造塔
	setNetPacketHandler(CM_DUNGOEN_BUY_TOWER,&Player::onDungeonBuyTower);		//副本造塔
	setNetPacketHandler(CM_DUNGEON_START,&Player::onDungeonStart);					//副本开始
	setNetPacketHandler(CM_DUNGEON_SELECT_REWARD,&Player::onDungeonSelectReward);	//选择副本奖励
	setNetPacketHandler(CM_ENTER_ACTIVIY, &Player::onEnterActivity);
	setNetPacketHandler(CM_LEAVE_ACTIVIY, &Player::onLeaveActivity);
	setNetPacketHandler(CM_GETAWARD_ACTIVIY,&Player::onGetAwardActivity);
	setNetPacketHandler(CM_SELECT_UNIT, &Player::onSelectUnit);//1
	setNetPacketHandler(CM_DO_UNIT_SKILL, &Player::onDoUnitSkill);
	setNetPacketHandler(CM_PICK_DROPITEM, &Player::onPickDropItem);
	setNetPacketHandler(CM_BEGIN_GATHER, &Player::onBeginGather);//开始采集
	setNetPacketHandler(CM_END_GATHER, &Player::onEndGather);//结束采集
	setNetPacketHandler(CM_USE_TRAP, &Player::onUseTrap);
	setNetPacketHandler(CM_SWITCH_PK_MODE, &Player::onSwitchPkMode);
	setNetPacketHandler(CM_SAFE_REVIVE, &Player::onSafeRevive);
	setNetPacketHandler(CM_ON_SITE_REVIVE, &Player::onSiteRevive);////立即复活
	setNetPacketHandler(CM_STRONG_REVIVE, &Player::onStrongRevive);
	setNetPacketHandler(CM_QUERY_CHR_INFO, &Player::onQueryChrInfo);
	setNetPacketHandler(CM_UPGRADE_LEVEL, &Player::onUpgradeLevel);
	setNetPacketHandler(CM_PREVENT_WALLOW , &Player::checkPreventWallow);//验证防沉迷
	setNetPacketHandler(CM_USE_ITEM, &Player::onUseItem);
	setNetPacketHandler(CM_PATCH_USE_ITEM,&Player::onPatchUseItem);
	setNetPacketHandler(CM_QUERY_SKILL_LIST, &Player::onQuerySkillList);
	setNetPacketHandler(CM_UPGRADE_SKILL_LEVEL, &Player::onUpgradeSkillLevel);
	setNetPacketHandler(CM_QUERY_TASK_LIST, &Player::onQueryTaskList);
	setNetPacketHandler(CM_RECEIVE_TASK, &Player::onReceiveTask);
	setNetPacketHandler(CM_SUBMIT_TASK, &Player::onSubmitTask);
	setNetPacketHandler(CM_GIVEUP_TASK, &Player::onGiveUpTask);
	setNetPacketHandler(CM_SET_TASK_CAN_SUBMIT, &Player::onSetTaskCanSubmit);
	setNetPacketHandler(CM_TALK_WITH_NPC, &Player::onTalkWithNpc);
	setNetPacketHandler(CM_QUICK_DONE, &Player::onQuickDone);	
	setNetPacketHandler(CM_TELEPORT, &Player::onTeleport);//npc传送
	setNetPacketHandler(CM_TELEPORT_ACTIVITY, &Player::onTeleportActivity);//活动npc传送
	setNetPacketHandler(CM_ADD_ACTION, &Player::onAddAction);
	setNetPacketHandler(CM_REMOVE_ACTION, &Player::onRemoveAction);
	setNetPacketHandler(CM_EXCHANGE_ACTION, &Player::onExchangeAction);
	setNetPacketHandler(CM_SET_AUTOFIGHT, &Player::onSetAutoFight);
	setNetPacketHandler(CM_SET_SYSTEM_SETTING, &Player::onSetSystemSetting);
	setNetPacketHandler(CM_BUY_GAME_CHR_ITEM, &Player::onBuyChrShopItem);
	setNetPacketHandler(CM_BUY_BACK_CHR_ITEM,&Player::onBuyBackChrShopItem);
	setNetPacketHandler(CM_BUY_RESOURCE,&Player::onBuyResource);
	setNetPacketHandler(CM_BUY_TASK_COUNT,&Player::onBuyTaskCount);
//	setNetPacketHandler(CM_QUERY_MONSTER_BROADCAST, &Player::onQueryMonsterBroadcast);
	setNetPacketHandler(CM_TELEPORT_BY_ITEM, &Player::onTeleportByItem);
	setNetPacketHandler(CM_CLICK_PAY_BUTTON, &Player::onClickPayButton);
	setNetPacketHandler(CM_MAP_ENTERED, &Player::onMapEntered);
	setNetPacketHandler(CM_DEBUG_CMD, &Player::onDebugCmd);
	setNetPacketHandler(CM_QUERY_PLAYER_INFO, &Player::onQueryPlayerInfo);
	setNetPacketHandler(CM_BUY_PVP_STATE,&Player::onBuyPvpState);
	setNetPacketHandler(CM_QUERY_KILLER_RANK_SELF,&Player::onQueryKillerRankSelf);
	setNetPacketHandler(CM_QUERY_EVERY_GOLD,&Player::onQueryEveryDayGoldInfo);
	setNetPacketHandler(CM_GET_EVERY_GOLD,&Player::onGetEveryDayGold);
	setNetPacketHandler(CM_GAME_PUBLIC_CHAT,&Player::onGamePublicChat);
	setNetPacketHandler(CM_UPDATE_FLY_ICON_INT, &Player::onUpdateFlyIconInt);
	setNetPacketHandler(CM_YELLOW_STONE , &Player::initYellowStone);
	setNetPacketHandler(CM_GET_YELLOW_AWARD , &Player::getYellowAward);
	setNetPacketHandler(CM_ASK_BOSS_INFO , &Player::OnAskBossInfo);
	setNetPacketHandler(CM_REQUEST_ACTIVITY_INFO , &Player::OnRequestActivityInfo);
	setNetPacketHandler(CM_ACTIVITY_GET_DAILY_REWARD , &Player::OnActivityGetDailyReward);
	setNetPacketHandler(CM_ACTIVITY_REQUEST_TERRITORY_INFO , &Player::OnRequestTerritoryWarInfo);
	setNetPacketHandler(CM_BUY_ENTER_BOSS_HOME, &Player::OnEnterBossHome);
	setNetPacketHandler(CM_LEAVE_BOSS_HOME, &Player::OnLevelBossHome);
	setNetPacketHandler(CM_ENTER_VIP_GUA_JI_MAP, &Player::OnEnterVipGuaJiMap);
	setNetPacketHandler(CM_ENTER_MO_LING_RU_QIN, &Player::OnEnterMoLingRuQin);
	setNetPacketHandler(CM_CLICK_GAME,&Player::OnClickGame);
	setNetPacketHandler(CM_USE_MMULTI_ITEM,&Player::OnUsemMultiItem);
	setNetPacketHandler(CM_ASK_KAI_FU_HUO_DAO_INFO,&Player::OnKaiFuHuoDongOperator);
	setNetPacketHandler(CM_GET_KAI_FU_HUO_DAO_WARD,&Player::OnKaiFuHuoDongOperator);
	setNetPacketHandler(CM_ASK_KAI_FU_HUO_DONG_STATE,&Player::OnKaiFuHuoDongOperator);
}

void Player::init( PlayerDBData& dbData )
{
	int32_t nowTime = getNow();

	m_cgindex = dbData.cgindex;

	m_chr = dbData.chr.data;

	setHP( dbData.chr.data.hp );
	setMP( dbData.chr.data.mp );
	setPP( dbData.chr.data.pp );
	if (m_chr.pk_time > getNow() )
	{
		m_chr.pk_time = m_chr.pk_time;//getNow()+ m_chr.pk_time;
	}
	else
	{
		m_chr.pk_time = 0;
	}
	if ( m_chr.pk_mode == PK_MODE_FREE )
	{
		m_chr.pk_mode = PK_MODE_PEACE;
	}
	m_pkMode_bk = m_chr.pk_mode;
	m_levelStartTime = nowTime;
	m_lastPkValueTick = getTick();

	m_chr.last_login_time = nowTime;
	//m_chr.point_level  = m_chr.point_level ==0?CFG_DATA.getFirstAchievement():m_chr.point_level;
	m_sysUser = dbData.sysUser.data;
	m_sysUser.last_login_time = nowTime;
	m_sysUser.total_offline_time += nowTime - m_sysUser.last_logout_time;
	if (m_sysUser.total_offline_time >= 5*3600)
	{
		m_sysUser.total_online_time = 0;
		m_sysUser.total_offline_time = 0;
	}

	m_sysUserPreventWallow = dbData.sysUserPreventWallow.data;


	m_skills.assign(dbData.skillData.skillVt.begin(), dbData.skillData.skillVt.end());
	m_mysVector.assign(dbData.mysVector.stonVt.begin(),dbData.mysVector.stonVt.end());
	m_task.init(this, dbData.taskData.taskVt);

	memcpy(m_actions, dbData.actionData.actionArry, sizeof(m_actions));

	m_autoFight = dbData.autoFight.data;
	m_systemSetting = dbData.systemSetting.data;
	m_lastLocalNow = getLocalNow();
	m_lastSaveTick = getTick();
	//初始化buff
	MemChrBuffVector::iterator it = dbData.buffData.buffVt.begin();
	for ( ; it != dbData.buffData.buffVt.end(); ++it )
	{
		CfgItem *pCfgItem = CFG_DATA.getItem(it->id);
		if (pCfgItem != NULL)
		{
			CfgBuff *pCfgBuff = CFG_DATA.getBuff(atoi(pCfgItem->effect.c_str()));
			if (pCfgBuff != NULL)
			{
				PersistBuff *pBuff = new PersistBuff(*this, *pCfgBuff);
				pBuff->init(it->id, it->time);
				addBuff(pBuff);
			}
		}
	}
	// 扩展系统初始化数据
	m_ExtSysMgr.OnLoadFromDB( dbData );
	
	//这个要在扩系统之后
	if ( dbData.sysUser.data.gold > 0 )
	{
		AddCurrency(CURRENCY_GOLD,dbData.sysUser.data.gold, GCR_RECHARGE );
		PayedDispose( dbData.sysUser.data.gold );
	}
	//这个要在扩系统之后
	if ( m_sysUser.last_logout_time > 0 )
	{
		int32_t offlineTime = nowTime - m_sysUser.last_logout_time;
		GetOperateLimit().AddLimitCount( PR_ACCUMULATIVE_OFFLINE_TIME, offlineTime );
	}
	else//第一次登陆
	{
		updateRecord( RP_WEEK_ONLINE_TIME, 0 );
		updateRecord( RP_DAILY_ONLIN_TIME, 0 );
	}
	refreshDailyCheck(false);

	if (DayTime::daydiff(dbData.loadTime) == 0)
	{
		m_todayGoldCharge = dbData.todayGoldCharge;
		if (DayTime::daydiff(CFG_DATA.getServerStartTime()) == 0)
		{
			m_todayGoldCharge += m_sysUser.prepay_gold;
		}
	}
	m_firstFiveDayGoldCharge = dbData.firstFiveDayGoldCharge + m_sysUser.prepay_gold;
	//if (!dbData.equips.equipVt.empty())
	//{
	//	sendEquipInfo(dbData.equips.equipVt);
	//}
	m_InBossHomeTime = getRecord(RP_IN_BOSS_HOME_TIME);
	sendPlayerEquipInfo();
	InitSysSetting();
	setSyncStatusFlag();
	minuteCheck(false);
	//初始化扩展系统要在recalcAttr前面
	recalcAttr();
	EQUIP_RANSOM.OnPlayerLogin( getCid() );
	DB_SERVICE.logPlayerLogin(getCid(), 1, getNow());
	m_PlayerFunctionOpen.InitFunctionOpen( 0, getLevel() );
}

void Player::reset()
{
	generateEntityId();

	Unit::reset();

	NetPacket *packet = NULL;
	while ((packet = m_netPackets.pop()) != NULL)
	{
		packet->destroy();
	}
	m_cgindex = 0;

	bzero(&m_chr, sizeof(m_chr));
	bzero(&m_sysUser, sizeof(m_sysUser));
	bzero(&m_sysUserPreventWallow, sizeof(m_sysUserPreventWallow));


	m_pkMode_bk = 0;
	m_lastPkValueTick = 0;

	m_levelStartTime = 0;
	m_eventHP = 0;

	m_jumpTime = 0;
	m_skills.clear();
	m_mysVector.clear();
	m_task.reset();

	bzero(m_actions, sizeof(m_actions));
	bzero(&m_autoFight, sizeof(m_autoFight));
	m_systemSetting = "";
	bzero(&m_lastLocalNow, sizeof(m_lastLocalNow));
	m_lastActionTick = 0;
	m_lastSaveTick = 0;
	m_lastPoolTick = 0;
	m_lastExpPoolTick = 0;
	m_lastMagicWeaponBuffTick = 0;

	bzero(&m_oldPosition, sizeof(m_oldPosition));
	bzero(&m_ActFrontPosition,sizeof(m_ActFrontPosition));

	m_plantId = 0;
	m_startGatherTick = 0;
	m_trailer  = NULL;
	m_todayGoldCharge = 0;
	m_firstFiveDayGoldCharge = 0;

	m_needSyncStatus = false;
	m_needSyncToTeam = false;

	m_StartProtect  = 0;
m_actState = 0;
	m_nDieTick = 0;
	m_ExpRate  = 0;	
	m_InBossHomeTime	= 0;
	m_useSkill.CleanUp();
	m_SystemSetting.clear();
//	m_SystemSetting[SS_PETEGG] = 1;
	m_LastAddPPTick		= 0;
	// 扩展系统清理数据
	m_ExtSysMgr.OnCleanUp();
}

int16_t Player::getGateIndex() const
{
	return m_cgindex;
}

void Player::setGateIndex(int16_t index)
{
	m_cgindex = index;
}

void Player::appendInfo(Answer::NetPacket *packet)
{
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(getCid());
	packet->writeUTF8(getName());
	packet->writeInt64(getFamilyId());
	packet->writeInt8(getFamilyPosition());
	packet->writeUTF8(getFamilyName());
	packet->writeInt8(getSex());
	packet->writeInt8(getJob());
	packet->writeInt8(0);
	packet->writeInt16(getLevel());
	packet->writeInt8(m_chr.pk_mode);
	packet->writeInt32(m_chr.pk_value);
	packet->writeInt32(( getPkProtectTime() - getNow()) > 0 ? ( getPkProtectTime() - getNow()) : 0 );					// PK保护时间
	packet->writeInt16(getCurrentTile().x);
	packet->writeInt16(getCurrentTile().y);
	packet->writeInt16(getTargetTile().x);
	packet->writeInt16(getTargetTile().y);
	packet->writeInt32( GetHP() );
	packet->writeInt32( GetMaxHP() );
	packet->writeInt32( GetMP() );
	packet->writeInt32( GetMaxMP() );
	packet->writeInt32( GetPP() );
	packet->writeInt32( GetMaxPP() );
	packet->writeInt16( GetMoveSpeed() );
	packet->writeInt8(getHead());
	packet->writeInt8( GetTeamStatus() );
	packet->writeInt32( m_extCharPet.GetRidePet() );
	packet->writeInt32( m_extCharPet.GetRidePetPoints() );
	packet->writeInt8(getAction());
	packet->writeInt32(getWeaponId());
	packet->writeInt32(getClothesId());
	packet->writeInt8(GetPlayerVip().GetVipLevel());// vip等级
	packet->writeInt32(0);			// vip到期时间
	packet->writeInt32(m_yellow_vip_level);
	packet->writeInt32(m_is_yellow_year_vip);
	packet->writeInt32(m_is_yellow_high_vip);
	packet->writeInt8( GetPlayerJueWei().GetJueWei() );
	packet->writeInt8( (int8_t)getRecord( RP_CUR_GUAN_WEI ) );
}

//地图间切换
int32_t Player::switchMap(Map *pMap, int32_t x, int32_t y, bool isFly)
{
	if ( pMap == NULL || pMap == m_pMap )
	{
		return ERR_INVALID_DATA;
	}

	//判断是否可以进入这个地图
	int32_t err = pMap->canEnter(this);
	if (err != ERR_OK)
	{
		return GAME_SERVICE.replyfailure(m_cgindex, CM_SWITCH_MAP, err, pMap->GetId());
	}
	//在副本
	if (InDungeon())
	{
		return ERR_INVALID_DATA; //leaveDungeon();
	}
	else 
	{
		Position playerPos = getCurrentTile();
		int32_t nRunnerId = GetRunnerId();
		if ( !m_pMap->IsVipGuaJiMap() && pMap->IsVipGuaJiMap() )
		{
			setOldPosition();
		}
		BreakGather();			// 打断采集
		//sendChrRecord();
		broadcastLeave();		// 发送离开消息
		m_pMap->removePlayer(this, false);
		if ( pMap->GetRunnerId() == nRunnerId )
		{	
			GAME_SERVICE.replySuccess(m_cgindex, CM_SWITCH_MAP, pMap->GetId());
			pMap->addPlayer(this, x, y);
			if (!isFly && m_trailer != NULL && m_trailer->getCurrentTile().tileDistance(playerPos) < 20)
			{
				m_trailer->checkSwitchMap();
			}
			return ERR_OK;
		}
		else
		{
			MAP_MANAGER.PostMsg( nRunnerId, GMC_PLAYER_LEAVE_MAP, this, pMap, x, y );
			GAME_SERVICE.replySuccess(m_cgindex, CM_SWITCH_MAP, pMap->GetId());
			return ERR_LEAVE_KINGDOM;
		}
	}
}

int32_t Player::OnEnterVipGuaJiMap( Answer::NetPacket *inPacket )
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	if (m_pMap == NULL)
	{
		return ERR_INVALID_DATA;
	}

	if ( IsInStall() )
	{
		return ERR_INVALID_DATA;
	}

	int32_t mapid	= inPacket->readInt32();
	int32_t x		= inPacket->readInt32();
	int32_t y		= inPacket->readInt32();
	Map *pTargetMap = MAP_MANAGER.GetMap( mapid );
	if (pTargetMap == NULL || pTargetMap == m_pMap)
	{
		return ERR_INVALID_DATA;
	}

	int32_t ret = switchMap(pTargetMap, x, y, false);
	if ( ret != ERR_OK )
	{
		return ret;
	}

	LogActivity logActivity = {};
	logActivity.cid = getCid();
	logActivity.acttype = ATI_SAFE_GUAJI;
	logActivity.time = getNow();
	logActivity.param = mapid;

	DB_SERVICE.InsertActivityLog( logActivity );

	return ERR_OK;
}

void Player::LeaveVipGuaJiMap()
{
	if( NULL == m_pMap )
	{
		return;
	}

	if ( !isAlive() )
	{
		FillHP();
		FillMP();
	}

	Map	*pTargetMap = MAP_MANAGER.GetMap( m_oldPosition.mapid );
	if ( pTargetMap == NULL || pTargetMap == m_pMap )
	{
		LOG_INFO( " pTargetMap == pAct(%d,%d)", m_oldPosition.runnerId, m_oldPosition.mapid );
		return;
	}

	broadcastLeave();
	if ( m_pMap->GetRunnerId() == pTargetMap->GetRunnerId() )
	{
		m_pMap->removePlayer( this, false );
		pTargetMap->addPlayer( this, m_oldPosition.x, m_oldPosition.y );
	}
	else
	{
		m_pMap->removePlayer( this, false );
		MAP_MANAGER.PostMsg( pTargetMap->GetRunnerId(), GMC_PLAYER_LEAVE_MAP, this, pTargetMap, m_oldPosition.x, m_oldPosition.y );
	}
}

void Player::moveToReviveRegion(bool bInAct)
{
	if (m_pMap == NULL)
	{
		return;
	}

	int32_t region = m_pMap->getReive( this );
	CfgMapRegion *pCfgMapRegion = CFG_DATA.getMapRegion( region );
	if ( pCfgMapRegion == NULL )
	{
		return;
	}

	CfgMap*pRegionMap = CFG_DATA.getMap( pCfgMapRegion->mapid );
	if ( pRegionMap == NULL )
	{
		return;
	}

	Map *pTargetMap = MAP_MANAGER.GetMap( pCfgMapRegion->mapid );
	if ( pTargetMap == NULL )
	{
		return;
	}

	Position pos = pTargetMap->getRandomWalkablePositionInRegion( *pCfgMapRegion );
	if (pos.x < 0 || pos.y < 0)
	{
		return;
	}
	
	if ( pTargetMap == m_pMap )
	{
		instantMove(pos.x, pos.y, IMR_TRANSFER);
	}
	else
	{
		switchMap(pTargetMap, pos.x, pos.y, true);
	}
}

int32_t Player::getActivityBirthRegion(Int32Vector* pRegsions) const
{
	if(pRegsions ==NULL)
	{
		return 0;
	}
		
	int32_t lenth =pRegsions->size();
	if(pRegsions->size() == 1)
	{
		return pRegsions->at(0);
	}

	return pRegsions->at(RANDOM.generate(0,lenth-1));
}

int32_t Player::leaveDungeon()
{
	Dungeon *pDungeon = dynamic_cast<Dungeon*>(m_pMap);
	if (pDungeon == NULL)
	{
		LOG_INFO("pDungeon NULL");
		return ERR_INVALID_DATA;
	}

	Position oldPosition(m_oldPosition.x,m_oldPosition.y);

	Map	*pTargetMap = MAP_MANAGER.GetMap( m_oldPosition.mapid );
	if (pTargetMap == NULL )
	{
		LOG_DUNGEON("leave dungeon is  null faile  oldmap id %d,kingdom id %d\n",m_oldPosition.mapid, GetRunnerId() );
		return ERR_INVALID_DATA;
	}

	if (pTargetMap == pDungeon)
	{
		LOG_DUNGEON("====  id %d,kingdom id %d\n",m_oldPosition.mapid, GetRunnerId());
		return ERR_INVALID_DATA;
	}

	broadcastLeave();
	pDungeon->removePlayer(this, false);

	if (!isAlive())
	{
		safeRevive();
	}

	if (pDungeon->getDungeonType() == DT_TEAM)
	{
		m_extCharTeamDungeon.LeaveTeamDungeon();
	}

	pTargetMap->addPlayer(this, oldPosition.x, oldPosition.y);

	pDungeon->onPlayerLeave(this);
	return ERR_OK;
}

int32_t Player::leaveActivity()
{
	if( NULL == m_pMap )
	{
		return ERR_INVALID_DATA;
	}

	if ( !isAlive() )
	{
		FillHP();
		FillMP();
	}

	Map	*pTargetMap = MAP_MANAGER.GetMap( m_oldPosition.mapid );
	if ( pTargetMap == NULL || pTargetMap == m_pMap )
	{
		LOG_INFO( " pTargetMap == pAct(%d,%d)", m_oldPosition.runnerId, m_oldPosition.mapid );
		return ERR_INVALID_DATA;
	}

	broadcastLeave();
	if ( m_pMap->GetRunnerId() == pTargetMap->GetRunnerId() )
	{
		m_pMap->removePlayer( this, false );
		pTargetMap->addPlayer( this, m_oldPosition.x, m_oldPosition.y );
	}
	else
	{
		m_pMap->removePlayer( this, false );
		MAP_MANAGER.PostMsg( pTargetMap->GetRunnerId(), GMC_PLAYER_LEAVE_MAP, this, pTargetMap, m_oldPosition.x, m_oldPosition.y );
	}
	recalcAttr();
	NetPacket *outPacket = GAME_SERVICE.popNetpacket( PACK_DISPATCH, IM_GAME_SOCIAL_UPDATE_ACTIVITY_STATE );
	if (NULL == outPacket)
	{
		return ERR_INVALID_DATA;
	}
	outPacket->writeInt32(m_cgindex);
	outPacket->writeInt32(0);
	outPacket->setSize(outPacket->getWOffset());
	GAME_SERVICE.sendPacket(outPacket);

	return ERR_OK;
}

//bool Player::inMatch() const
//{
//	bool bInAct=false;
//	ActPKMap *pDungeon = dynamic_cast<ActPKMap*>(m_pMap);
//	if(pDungeon!=NULL)
//	{
//		bInAct =true;
//	}
//	return bInAct;
//}

bool Player::isInRectangle(Position x,Position y) const
{
      return Unit::isInRectangle(x,y);
}

CharId_t Player::getCid() const
{
	return m_chr.cid;
}

std::string Player::getName() const
{
	return m_chr.name;
}

Sex_t Player::getSex() const
{
	return m_chr.sex;
}

Job_t Player::getJob() const
{
	return m_chr.job;
}

int64_t Player::getExp() const
{
	return m_chr.exp;
}

int32_t Player::getCreateTime() const
{
	return m_chr.create_time;
}

int32_t Player::getHead() const
{
	return m_chr.head;
}

int32_t Player::getLastLogoutTime() const
{
	return m_chr.last_logout_time;
}

int32_t Player::getLastLoginTime() const
{
	return m_chr.last_login_time;
}

FamilyId_t Player::getFamilyId() const
{
	return m_extCharFamily.GetFamilyId();
}

string Player::getFamilyName() const
{
	return m_extCharFamily.GetFamilyName();
}

int8_t Player::getFamilyPosition() const
{
	return m_extCharFamily.GetPosition();
}

Answer::NetPacket *Player::getOtherQueryInfo()
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_QUERY_PLAYER_INFO);
	if (NULL == packet)
	{
		return NULL;
	}
	packet->writeInt64( getCid() );												// cid
	packet->writeUTF8( getName() );												// 名字
	packet->writeInt64( getFamilyId() );										// 帮派ID
	packet->writeUTF8( getFamilyName() );										// 帮派名字
	packet->writeInt8( getSex() );												// 性别
	packet->writeInt8( getJob() );												// 职业
	packet->writeInt16( getLevel() );											// 等级
	packet->writeInt64( getExp() );												// 当前经验
	packet->writeInt64( GetLevelExp() );										// 升级需求经验值
	packet->writeInt32( GetHP() );												// 当前血量
	packet->writeInt32( GetMaxHP() );											// 最大生命值
	packet->writeInt32( GetMP() );												// 当前法力值
	packet->writeInt32( GetMaxMP() );											// 最大法力值
	packet->writeInt32( GetPP() );												// 体力值
	packet->writeInt32( GetMaxPP() );											// 最大体力值
	packet->writeInt32( GetAttrValue( CObjAttrs::ATTR_PHY_ATK_MIN ) );			// 最小物理攻击
	packet->writeInt32( GetAttrValue( CObjAttrs::ATTR_PHY_ATK_MAX ) );			// 最大物理攻击
	packet->writeInt32( GetAttrValue( CObjAttrs::ATTR_MAG_ATK_MIN ) );			// 最小魔法攻击
	packet->writeInt32( GetAttrValue( CObjAttrs::ATTR_MAG_ATK_MAX ) );			// 最大魔法攻击
	packet->writeInt32( GetAttrValue( CObjAttrs::ATTR_PHY_DEF ) );				// 物理防御
	packet->writeInt32( GetAttrValue( CObjAttrs::ATTR_MAG_DEF ) );				// 魔法防御
	packet->writeInt32( GetAttrValue( CObjAttrs::ATTR_HITRATE ) );				// 命中
	packet->writeInt32( GetAttrValue( CObjAttrs::ATTR_DODGE ) );				// 闪避
	packet->writeInt32( GetAttrValue( CObjAttrs::ATTR_CRITRATE ) );				// 暴击
	packet->writeInt32( getBattle() );											// 战斗力

	m_extEquip.PackageEquipInfo( packet );										// 打包装备信息
//	m_extEquip.PackageGemInfo( packet );										// 打包宝石信息
	m_PlayerFaBao.PackFaBaoInfo( packet );
	packet->setSize(packet->getWOffset());

	return packet;
}

void Player::addKillPlayerCount()
{
   m_chr.kill_count++;
}

void Player::delExp( int64_t nValue )
{
	m_chr.exp -= nValue;
	if ( m_chr.exp < 0 )
	{
		m_chr.exp = 0;
	}

	setNeedSyncSelf();
}

void Player::addExp(int64_t addon, bool Iswallow )
{
	if (addon != 0)
	{
		if ( addon > 0 )
		{
			if ( Iswallow )
			{
				double  bnfRatio = benefitRatio();
				addon = static_cast<int32_t>( addon * bnfRatio );	
			}
			sendGainInfo( GT_EXP, static_cast<int32_t>(addon),benefitType() );
		}
		if (m_chr.exp < 0)
		{
			m_chr.exp = 0;
		}

		m_chr.exp += addon - m_chr.exp_minus;//经验减去值，主要用于pk惩罚
		if (m_chr.exp < 0)
		{
			int64_t levelNeedExp = GetLevelExp();
			m_chr.exp_minus = - m_chr.exp;
			m_chr.exp = 0;
			if (m_chr.exp_minus > levelNeedExp * 5)
			{
				m_chr.exp_minus = levelNeedExp * 5;
			}
		}
		else
		{
			m_chr.exp_minus = 0;
		}

		if (m_chr.exp)
		{
			autoUpgradeLevel();

			int64_t levelMaxExp = GetLevelMaxExp();
			if (m_chr.exp >= levelMaxExp)
			{
				m_chr.exp = levelMaxExp;
			}
		}

		if (m_chr.exp < 0)
		{
			m_chr.exp = 0;
		}

		if (m_chr.exp < 0)
		{
			m_chr.exp = 0;
		}

		setNeedSyncSelf();
	}
}

int64_t Player::GetLevelExp()
{
	return CFG_DATA.getNeedLevelExp( getLevel() );
}

int64_t Player::GetLevelMaxExp()
{
	return CFG_DATA.getMaxExp( getLevel() );
}

void Player::addContribution(int32_t addon, int32_t rate)
{
	NetPacket *outPacket = GAME_SERVICE.popNetpacket(PACK_DISPATCH, IM_GAME_SOCIAL_ADD_CONTRIBUTION);
	if (NULL == outPacket)
	{
		return;
	}
	outPacket->writeInt32(m_cgindex);
	outPacket->writeInt32(addon);
	outPacket->writeInt32(rate);
	outPacket->setSize(outPacket->getWOffset());
	GAME_SERVICE.sendPacket(outPacket);
}

void Player::addEventHp(int32_t hpValue)
{
	m_eventHP =  hpValue;
}

int32_t Player::getUid() const
{
	return m_sysUser.uid;
}

int32_t Player::getSid() const
{
	return m_sysUser.sid;
}

void Player::syncGold()
{
	MySqlDBGuard db(DBPOOL);

	char szSQL[MAX_SQL_LENGTH] = {};
	snprintf(szSQL, sizeof(szSQL)-1, "SELECT `gold_pay` FROM `sys_user` WHERE `uid`=%d AND `sid`=%d", getUid(), m_sysUser.sid);

	MySqlQuery result = db.query(szSQL);
	if (!result.eof())
	{
		int32_t gold_pay = result.getIntValue("gold_pay");
		if (gold_pay > 0)
		{
			AddCurrency( CURRENCY_GOLD, gold_pay, GCR_RECHARGE );

			m_sysUser.gold_pay_total += gold_pay;
			setNeedSyncSelf();

			bzero(szSQL, sizeof(szSQL));
			snprintf(szSQL, sizeof(szSQL)-1, "UPDATE `sys_user` SET `gold`=`gold`+%d, `gold_pay`=`gold_pay`-%d, `gold_pay_total`=`gold_pay_total`+%d WHERE `uid`=%d AND `sid`=%d", gold_pay, gold_pay, gold_pay, getUid(), m_sysUser.sid);

			db.excute(szSQL);

			int32_t startDayZero = DayTime::dayzero(CFG_DATA.getServerStartTime());
			if (getNow() <= startDayZero + 5 * 24 * 60 * 60)
			{
				bzero(szSQL, sizeof(szSQL));
				snprintf(szSQL, sizeof(szSQL)-1, "SELECT sum(`amount`) FROM `pay_log` WHERE `uid`=%d AND `sid`=%d AND `time`<=%d", m_sysUser.uid, m_sysUser.sid, startDayZero + 5 * 24 * 60 * 60);

				result = db.query(szSQL);
				if (!result.eof())
				{
					m_firstFiveDayGoldCharge = result.getIntValue(0) + m_sysUser.prepay_gold;
				}
			}
			syncTodayGoldPay();
			m_PlayerYunYingHD.SendShouChongInfo();
			PayedDispose( gold_pay );
		}
	}
}

void Player::PayedDispose( const int32_t AddGold )
{
	//充值相关的活动可以从这里接
	m_PlayerYunYingHD.AddTotalChongZhiCount( AddGold );
	m_WarVictory.AddGold( AddGold );
};

void Player::syncTodayGoldPay()
{
	MySqlDBGuard db(DBPOOL);
	int32_t dayZero = DayTime::dayzero(getNow());
	char szSQL[MAX_SQL_LENGTH] = {};
	bzero(szSQL, sizeof(szSQL));
	snprintf(szSQL, sizeof(szSQL)-1, "SELECT sum(`amount`) FROM `pay_log` WHERE `uid`=%d AND `sid`=%d AND `time`>=%d AND `time`<=%d", m_sysUser.uid, m_sysUser.sid, dayZero, dayZero + 24 * 60 * 60);

	m_todayGoldCharge = 0;
	MySqlQuery result = db.query(szSQL);
	if (!result.eof())
	{
		m_todayGoldCharge = result.getIntValue(0);
		if (DayTime::daydiff(CFG_DATA.getServerStartTime()) == 0)
		{
			m_todayGoldCharge += m_sysUser.prepay_gold;
		}
	}
	m_PlayerYunYingHD.SendEveryDayChongZhiInfo();
}

void Player::updatePayGold()
{
// 	MySqlDBGuard db(DBPOOL);
// 
// 	char szSQL[MAX_SQL_LENGTH] = {};
// 	snprintf(szSQL, sizeof(szSQL)-1, "UPDATE `sys_user` SET `gold_pay_total`= %d WHERE `uid`=%d AND `sid`=%d",m_sysUser.gold_pay_total,getUid(),m_sysUser.sid);
// 	db.excute(szSQL);
}

std::string Player::getIP()
{
	return m_sysUser.IP;
}

int32_t Player::getClothesId()
{
	return m_extEquip.GetEquipSlot( EQUIP_SLOT_CLOTH ).itemId;
}

void Player::setTrailer(Trailer* pTrailer)
{
	  m_trailer = pTrailer;
}

void Player::checkTrailer()
{
	if (m_trailer != NULL)
	{
		m_trailer->update();
	}
}

int32_t Player::getWeaponId()
{
	return m_extEquip.GetEquipSlot( EQUIP_SLOT_WEAPON ).itemId;
}

double Player::benefitRatio()
{
	if (m_sysUser.adult)
	{
		return 1.0;
	}
	else
	{
		int32_t onlineTime = m_sysUser.total_online_time + getNow() - m_sysUser.last_login_time;
		if (onlineTime >= 5*3600)
		{
			return 0.0;
		}
		else if (onlineTime >= 3*3600)
		{
			return 0.5;
		}
		else
		{
			return 1.0;
		}
	}
}

BenefitType Player::benefitType()
{
	if (m_sysUser.adult)
	{
		return BT_NORMAL;
	}
	else
	{
		int32_t onlineTime = m_sysUser.total_online_time + getNow() - m_sysUser.last_login_time;
		if (onlineTime >= 5*3600)
		{
			return BT_HURT;
		}
		else if (onlineTime >= 3*3600)
		{
			return BT_TIRED;
		}
		else
		{
			return BT_NORMAL;
		}
	}
}

int32_t Player::GetFreeSlotCount()
{
	return m_extCharBag.GetFreeSlotCount();
}

MemChrBag Player::getBagSlotData( int32_t slot )
{
	return m_extCharBag.GetSlotData( slot );
}

void Player::setBagSlotData( int32_t slot, const MemChrBag &slotData, int32_t reason, int32_t count )
{
	m_extCharBag.SetSlotData( slot, slotData, reason, count );
}

bool Player::hasItem(int32_t baseid, int32_t type, int32_t count)
{
	return m_extCharBag.HasItem(baseid, type, count);
}

void Player::autoUseItem(MemChrBagVector &items)
{
	MemChrBagVector temp;
	temp.assign(items.begin(),items.end());

	items.clear();

	for (uint32_t i = 0; i < temp.size();++i)
	{
		if ( !autoUseItem( temp[i] ) )
		{
			items.push_back(temp[i]);
		}
	}
}

bool Player::autoUseItem(const MemChrBag &item)
{
	if (item.itemClass == IC_NORMAL && item.itemCount == 1)
	{
		CfgItem *pCfgItem = CFG_DATA.getItem(item.itemId);
		if (pCfgItem != NULL)
		{
			 if (pCfgItem->type == IET_MONEY_GAIN)
			{
				ITEM_EFFECT.effect(item.itemId, *this, *this);
				return true;
			}
			else if( pCfgItem->type == IET_YUANBAO )
			{
				ITEM_EFFECT.effect(item.itemId, *this, *this);
				return true;
			}
			else if ( pCfgItem->type == IET_AUTO_PET_GIFT )
			{
				if ( ITEM_EFFECT.effect(item.itemId, *this, *this) != ERR_OK )
				{
					return false;
				}
				return true;
			}
		}
	}

	return false;
}

int32_t Player::getFirstFreeSlot()
{
	return m_extCharBag.GetFirstFreeSlot();
}

int32_t Player::getBattle() const
{
	return m_Fighting.GetBattle();
	//return GetAttrValue( CObjAttrs::ATTR_BATTLE );
}

//int32_t Player::getJiangXing()
//{
//	return m_chr.jiangxing;
//}
//int32_t Player::getChangeFlag()
//{
//	return m_chr.change_flag;
//}

//void Player::updateTotalFightPower()
//{
//	m_chr.total_fight_power = m_chr.fight_power;// + m_chrViceGeneral.getVicegeneralTotalFight();
//}

//void Player::setChangeFlag(int32_t flag)
//{
//	m_chr.change_flag = flag;
//}

//void  Player::setFamilyPosition(int32_t value)
//{
//	m_chr.family_position = value;
//}

//void Player::addJungong(int32_t addon,int32_t reason, BenefitType bnfType)
//{
//	// TODO：change
//}
//
//void Player::addWuhuen(int32_t addon,int32_t reason, BenefitType bnfType)
//{
//	// TODO：change
//}


//void Player::addKingdomContribute(int32_t addon)
//{
//	if (addon < 0 && std::abs(addon) > m_chr.kingdom_contribute)return;
//	if (addon == 0) return;
//
//	int32_t oldKingdomContribute = m_chr.kingdom_contribute;
//	m_chr.kingdom_contribute += addon;
//
//	sendChrKingdomInfo();
//
//	NetPacket *outPacket = GAME_SERVICE.popNetpacket(PACK_DISPATCH, IM_GAME_SOCIAL_UPDATE_KINGDOM_CONTRIBUTE);
//	if (NULL == outPacket)
//	{
//		return;
//	}
//	outPacket->writeInt32(m_cgindex);
//	outPacket->writeInt32(m_chr.kingdom_contribute);
//	outPacket->setSize(outPacket->getWOffset());
//	GAME_SERVICE.sendPacket(outPacket);
//
//	sendKingdomChangeInfo(addon);
//}

//void Player::addFamilyMoney(int32_t moneyAddon,int32_t contributeAddon)
//{
//	if (contributeAddon <= 0)return;
//	//m_chr.family_contribute += contributeAddon;
//
//	setNeedSyncSelf();
//
//	NetPacket *outPacket = GAME_SERVICE.popNetpacket(PACK_DISPATCH, IM_GAME_SOCIAL_UPDATE_FAMILY_MONEY);
//	if (NULL == outPacket)
//	{
//		return;
//	}
//	outPacket->writeInt32(m_cgindex);
//	outPacket->writeInt32(moneyAddon);
//	outPacket->writeInt32(contributeAddon);
//	outPacket->setSize(outPacket->getWOffset());
//	GAME_SERVICE.sendPacket(outPacket);
//}

//void Player::addFamilyContribute(int32_t addon)
//{
//	if (addon < 0 && std::abs(addon) > m_chr.family_contribute)return;
//	if (addon == 0) return;
//
//	m_chr.family_contribute += addon;
//	sendChrInfo();
//
//	NetPacket *outPacket = GAME_SERVICE.popNetpacket(PACK_DISPATCH, IM_GAME_SOCIAL_UPDATE_FAMILY_CONTRIBUTE);
//	if (NULL == outPacket)
//	{
//		return;
//	}
//	outPacket->writeInt32(m_cgindex);
//	outPacket->writeInt32(m_chr.family_contribute);
//	outPacket->setSize(outPacket->getWOffset());
//	GAME_SERVICE.sendPacket(outPacket);
//}

//void Player::addJiangxing(int32_t addon,int32_t reason, BenefitType bnfType)
//{
//	// TODO：change
//}

//bool Player::getDungeonReward( CfgDungeonReward*pReward, int32_t nRatio )
//{
//	if (NULL == pReward)
//	{
//		return false;
//	}
//	double bnfRatio = benefitRatio();
//	BenefitType bnfType = benefitType();
//	if ( pReward->money > 0 )
//	{
//		AddCurrency( CURRENCY_MONEY, static_cast<int32_t>(pReward->money * bnfRatio)*nRatio, MCR_DUNGEON_REWARD, pReward->dungeonID );
//	}
//	if ( pReward->gold > 0 )
//	{
//		AddCurrency( CURRENCY_CASH, static_cast<int32_t>(pReward->gold * bnfRatio)*nRatio, GCR_DUNGEON_REWARD, bnfType);
//	}
//	if ( pReward->exp > 0 )
//	{
//		addExp(static_cast<int64_t>(pReward->exp * bnfType)*nRatio);
//	}
//	return true;
//}

int32_t Player::getPkMode()
{
	return m_chr.pk_mode;
}

void Player::setPkMode(int32_t mode, bool backUp)
{
	m_chr.pk_mode = mode;
	if (backUp)
	{
		m_pkMode_bk = m_chr.pk_mode;
	}

	if ( mode != PK_MODE_PEACE )
	{
		SetPkProtectTime( 0 );
	}

	setNeedSyncSelf();
	setSyncStatusFlag();
}

void Player::resetPkModeOnEnterMap(const Map *pMap)
{
	if (pMap == NULL)
	{
		return;
	}
	if ( pMap->getPkMode() == PK_MODE_FREE )
	{
		if ( m_chr.pk_mode != m_pkMode_bk )
		{
			m_chr.pk_mode = m_pkMode_bk;
		}
	}
	else if ( pMap->getPkMode() == PK_MODE_PEACE )
	{
		m_pkMode_bk		= m_chr.pk_mode;
		m_chr.pk_mode	= PK_MODE_PEACE;
	}
	else if ( pMap->getPkMode() > PK_MODE_PEACE )
	{
		m_pkMode_bk		= m_chr.pk_mode;
		m_chr.pk_mode	= pMap->getPkMode();
		if ( m_chr.pk_time > 0 )
		{
			m_chr.pk_time	= 0;
		}
	}
	setNeedSyncSelf();
	setSyncStatusFlag();
}

void Player::onDropItem( Player *pKiller, int32_t Mid ) //pKiller 是怪物的时候为NULL,
{
	PkDropRate* pPkDropRate = CFG_DATA.GetPkDropRate( getPkValue() );
	if ( NULL == pPkDropRate )
	{
		return;
	}
	int64_t HaveMoney = GetCurrency( CURRENCY_MONEY );
	int64_t DropMoneyCount = 0;
	if ( HaveMoney > 0 )
	{
		DropMoneyCount = HaveMoney / 100 * pPkDropRate->MoneyRate;
		if ( DropMoneyCount <= 0 )
		{
			DropMoneyCount = HaveMoney;
		}
		DecCurrency( CURRENCY_MONEY, DropMoneyCount, MCR_PK_DROP );
	}
	

	//获取装备个数
	MemChrBagVector DropItemVt;
	DropItemVt.clear();
	m_extEquip.GetDropEquip( pKiller, pPkDropRate->EquipCount, pPkDropRate->EquipRate, DropItemVt, Mid );
	m_extCharBag.GetDropItem( pKiller, pPkDropRate, DropItemVt, Mid );


	DropItem dropItems[MAX_DROPITEM_SIZE] = {};
	
	int32_t i = 0;

	if ( DropMoneyCount > 0 )
	{
		dropItems[i].itemId		= CURRENCY_MONEY_ID;
		dropItems[i].itemClass	= IC_CURRENCY;
		dropItems[i].bindType	= IBS_UNBIND;
		dropItems[i].costType	= CCT_NONE;
		dropItems[i].costValue	= 0;
		dropItems[i].itemCount	= static_cast<int32_t>(DropMoneyCount);
		dropItems[i].endTime	= 0;
		dropItems[i].srcId		= 0;
		++i;
	}
	//掉落小菊花
	if ( pKiller != NULL )
	{
		int32_t nRand = RANDOM.generate(0,100);
		if ( nRand < 10 )
		{
			dropItems[i].itemId		= ISI_XIAO_JV_HUA;
			dropItems[i].itemClass	= IC_NORMAL;
			dropItems[i].bindType	= IBS_UNBIND;
			dropItems[i].costType	= 0;
			dropItems[i].costValue	= 0;
			dropItems[i].itemCount	= 1;
			dropItems[i].endTime	= 0;
			dropItems[i].srcId		= 0;
			++i;
			//小菊花公告
			Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM );
			if (NULL == packet)
			{
				return;
			}
			packet->writeInt32( BCI_XIAO_JV_HUA_GONG_GAO );
			packet->writeUTF8( pKiller->getName() );
			packet->writeInt64( pKiller->getCid() );
			packet->writeUTF8( getName() );
			packet->writeInt64( getCid() );
			packet->setSize(packet->getWOffset());
			GAME_SERVICE.worldBroadcast(packet);
		}
	}
	
	MemChrBagVector::iterator it = DropItemVt.begin();
	for ( ; it != DropItemVt.end() && i < MAX_DROPITEM_SIZE; ++i, ++it )
	{
		dropItems[i].itemId		= it->itemId;
		dropItems[i].itemClass	= it->itemClass;
		dropItems[i].bindType	= it->bind;
		dropItems[i].costType	= CCT_NONE;
		dropItems[i].costValue	= 0;
		dropItems[i].itemCount	= it->itemCount;
		dropItems[i].endTime	= it->endTime;
		dropItems[i].srcId		= it->srcId;
	}
	CDropItemGroup *pDropItemGroup = NULL;
	if (m_pMap != NULL)
	{
		pDropItemGroup = POOL_MANAGER.pop<CDropItemGroup>();
		if ( pDropItemGroup != NULL )
		{
			pDropItemGroup->init(m_pMap, getCurrentTile(), pKiller, ET_PLAYER, 0, dropItems, getName(), 0, 0);
			m_pMap->addDropItemGroup(pDropItemGroup);
		}
	}

}

void  Player::updatePkValue( Player *pKiller, Map* pMap )
{
	if ( NULL == pKiller || NULL == pMap )
	{
		return;
	}
	
	if ( InDungeon() || ( InActivity() && pMap->GetMapParam() != FAMILY_LIGHT_MAP_PARA ) ) //4军团之光地图
	{
		return;
	}
	
	if ( m_extFightChecker.IsBuleName() )
	{
		return;
	}

	if ( getPkValue() <= PK_VALUE_WHITE && getPkMode() == PK_MODE_PEACE )
	{
		m_chr.pk_time = getNow() + PK_PROTECT_TIME;
		setSyncStatusFlag();
		setNeedSyncSelf();
	}

	if (getPkValue() > PK_VALUE_RED )	//黑名被杀
	{
		addPkValue( -1* BLACK_KILLED_SUB_PK_VALUES );
		setSyncStatusFlag();
	}
	else if ( getPkValue() <= PK_VALUE_WHITE )	//白名被杀
	{
		pKiller->addPkValue(KILLED_WHITE_PLAYER_PK_VALUES);
		pKiller->setSyncStatusFlag();
		pKiller->setNeedSyncSelf();
	}
	else //红名被杀
	{
		if ( pKiller->getPkValue() > PK_VALUE_WHITE )
		{
			pKiller->addPkValue(KILLED_RED_PLAYER_PK_VALUES);
			pKiller->setSyncStatusFlag();
			pKiller->setNeedSyncSelf();
		}
	}
}

void Player::addPkValue(int32_t addon)
{

	m_chr.pk_value += addon;
	if (m_chr.pk_value < 0)
	{
		m_chr.pk_value = 0;
	}
	if ( m_chr.pk_value > PK_VALUE_RED )
	{
		m_extFightChecker.ChangeBuleName(false);
	}
}

bool Player::SubPkValues( int32_t Addon )
{
	if ( getPkValue() <= 0 )
	{
		return false;
	}
	if ( Addon <= 0 )
	{
		return false;
	}
	addPkValue( -1 * Addon );
	setSyncStatusFlag();
	setNeedSyncSelf();
	return true;
}

int32_t Player::getPkValue()
{
	return m_chr.pk_value;
}

int32_t Player::getPkProtectTime()
{
	return m_chr.pk_time;
}

void Player::SetPkProtectTime( int32_t Time )
{
	m_chr.pk_time = Time;
	setSyncStatusFlag();
	setNeedSyncSelf();
}

void Player::updateLastTaskId(int32_t tid)
{
	m_chr.last_task_id = tid;
}

void Player::setTaskCanSubmit(int32_t tid)
{
	m_task.setTaskCanSubmit(tid);
}

void Player::checkTaskCanSubmit(int32_t dungeonID)
{
	  m_task.checkTaskCanSubmit(dungeonID);
}

void Player::taskTalkWithNpc(int32_t npcid)
{
	
}

void Player::updateRecord(int32_t id, int32_t param)
{
	m_extOperateLimit.UpdateLimitCount( id, param );
}

int32_t Player::getRecord(int32_t id) const
{
	return m_extOperateLimit.GetLimitCount( id );
}

void Player::clearRecordRange(int32_t start, int32_t end, int32_t diffDay)
{
	m_extOperateLimit.ResetRange( start, end, diffDay );
}

void Player::onKillMonster(int32_t mid, int32_t level, int32_t exp,bool isBoss )
{
	if ( isBoss )
	{
		GetPlayerHuoYueDu().AddHuoYueDuRecord( HYDT_KILL_BOSS );
		GetAchievemnet().AddAchievement( AT_KILL_BOSS );
	}
	GetPlayerHuoYueDu().AddHuoYueDuRecord( HYDT_KILL_MONSTER, mid );
	GetAchievemnet().AddAchievement( AT_KILL_MONSTER_BY_MID , mid );
	GetAchievemnet().AddAchievement( AT_KILL_MONSSTER );
	int32_t ExpRate = 0;
	PlayerList players;
	if ( IsInTeam() )
	{
		CharIdList teamMembers;
		m_extCharTeam.GetTeamMembers( teamMembers );
		if ( teamMembers.empty() )
		{
			players.push_back( this );
		}
		else
		{
			CharIdList::const_iterator iter = teamMembers.begin();
			CharIdList::const_iterator eiter = teamMembers.end();
			for ( ; iter != eiter; ++iter )
			{
				Player *player = GAME_SERVICE.getPlayer( *iter, GetRunnerId() );
				if ( player != NULL && player->getMap() == getMap() )
				{
					if ( player->getCurrentTile().tileDistance(getCurrentTile()) <= 30 )
					{
						players.push_back( player );
						continue;
					}
				}
			}
		}

		int32_t nMemberCount = teamMembers.size();
		if ( nMemberCount >= 0 && nMemberCount <= MAX_TEAM_MEMBER_COUNT )
		{
			ExpRate += CFG_DATA.GetTeamTable().GetTeamExpRatio( nMemberCount );
		}
	}
	else
	{
		players.push_back( this );
	}
	ExpRate	 += GetExpRate();
	ExpRate	 += m_extEquip.GetGemAddEffectTypeRate( GEM_TYPE_EXP );
	ExpRate  += GetPlayerVip().GetExpRate();;
	if ( getFamilyId() > 0 && ACTIVITY_MANAGER.GetFamilyWarWinner() == getFamilyId() )
	{
		ExpRate += 20;
	}

	int32_t nCurLevel = GetCalLevel();
	int32_t tLevel = nCurLevel - level;
	float LevelRatio = 1.0f;
	if ( !isBoss )
	{
		if ( nCurLevel - level <= 10 )
		{
			LevelRatio = 1.0f;
		}
		else if ( nCurLevel - level <= 30 )
		{
			LevelRatio = 0.8f;
		}
		else if ( nCurLevel - level <= 60 )
		{
			LevelRatio = 0.3f;
		}
		else
		{
			LevelRatio = 0.1f;
		}
	}
	// 经验值
	ExpRate += m_extCharPet.GetKnightExpRatio();

	int32_t nFinalExp = roundInt( exp * LevelRatio * ( 1.0f +static_cast<float>(ExpRate)/100 ) );
	if ( m_extCharSkill.IsInXP() )
	{
		m_extCharSkill.AddMonsterExp( nFinalExp );
	}
	else
	{
		addExp( nFinalExp );
		m_PlayerExpBall.AddBallExp( nFinalExp );
		m_extCharPet.OnAddExp( nFinalExp );
	}
	
	// 灵力
	float levelRatio = 1.0f;
	if ( nCurLevel - level <= 30 )
	{
		levelRatio = 1.0f;
	}
	else if ( nCurLevel - level <= 60 )
	{
		levelRatio = 0.5f;
	}
	else if ( nCurLevel - level <= 90 )
	{
		levelRatio = 0.1f;
	}
	else
	{
		levelRatio = 0;
	}
	int32_t nVigour = roundInt( 50 * levelRatio * ( 1.0f +static_cast<float>(ExpRate)/100 )  * this->benefitRatio() );
	if ( nVigour > 0 )
	{
		AddCurrency( CURRENCY_VIGOUR, nVigour, VCR_KILL_MONSTER );
	}
	if ( players.empty() )
	{
		return;
	}

	// 杀怪掉落分享
	CfgMonsterTaskDropVector *pCfgMonsterTaskDrop = CFG_DATA.getMonsterTaskDrop(mid);
	if (pCfgMonsterTaskDrop != NULL)
	{
		for (CfgMonsterTaskDropVector::iterator it = pCfgMonsterTaskDrop->begin(); it != pCfgMonsterTaskDrop->end(); ++it)
		{
			if (RANDOM.generate(1, 1000) <= it->probability)
			{
				for (PlayerList::iterator itPlayer = players.begin(); itPlayer != players.end(); ++itPlayer)
				{
					Player *player = *itPlayer;
					if (player != NULL)
					{
						player->m_task.updateTaskDrop(it->tid, it->item);
					}
				}
			}
		}
	}

	// 杀怪数分享
	for (PlayerList::iterator it = players.begin(); it != players.end(); ++it)
	{
		Player *player = *it;
		if ( NULL == player )
		{
			continue;
		}

		player->m_task.updateTaskMonster(mid, level);
		player->m_extTaskCycle.UpdateTaskMonster(mid, level);
		
		player->m_extCharPet.OnKillMonster( level );
	}
}

Int32Vector Player::getStrategicsReadState(int32_t *pIndexArry)
{
	Int32Vector stateVector;
	int32_t b[5] = {};
	for (int32_t i = 0; i < 5;++i)
	{
		int32_t a = *(pIndexArry+i);
		b[a-1] += 1;
	}

	int32_t count = 0;
	int32_t sum = 0;
	for (int32_t i = 0; i < 5;++i)
	{
		if(b[i] != 1 && b[i] != 0)
		{
			if(sum+b[i]==5 && b[i]!=5){
				sum=3;
			}else
			{
				sum=b[i];

			}
			count++;
		}
	}
	stateVector.push_back(count);
	stateVector.push_back(sum);
	return stateVector;
}

PlayerAction Player::getAction()
{
	if (m_plantId != 0)
	{
		return PA_COLLECT;
	}
	else
	{
		return PA_STAND;
	}
}

void Player::kingdomTaskFaile()
{
	//if (m_trailer != NULL)
	//{
	//	m_taskKingdom.taskFaile(); 
	//}
}

void Player::setKingdomTaskState(int32_t tid,int32_t state)
{
	//if (m_trailer != NULL)
	//{
	//	m_taskKingdom.setTaskState(tid,state);
	//}
}

PlayerTeamStatus Player::GetTeamStatus() const
{
	if ( IsInTeam() )
	{
		if ( IsTeamLeader() )
		{
			return PTS_TEAM_LEADER;
		}
		return PTS_TEAM_MEMBER;
	}
	return PTS_FREE;
}

void Player::onNewDayCome()
{
	refreshDailyCheck(true);
}

void Player::refeshKillerRecord()
{
	m_extOperateLimit.Reset( RP_PK_RANK_COUNT );
}

void Player::refreshDailyCheck(bool first)
{
	int32_t daydiff = DayTime::daydiff(getRecord(PR_DAILY_CHECK_LAST_UPDATE));
	//连续登陆
	if (daydiff == 1)
	{
		++m_sysUser.continue_login_count;
	}
	else if(daydiff > 1)
	{
		m_sysUser.continue_login_count = 1;
		GetAchievemnet().AddAchievement( AT_LOGIN );
	}

	//跨天处理
	if (daydiff >= 1)
	{  
		clearRecordRange(PR_DAILY_CLEAR_BEGIN, PR_DAILY_CLEAR_END, daydiff );
		syncTodayGoldPay();
		//登陆天数
		int32_t loginCount = getRecord(RP_LOGIN_COUNT) + 1;
		updateRecord(RP_LOGIN_COUNT, loginCount);

		m_ExtSysMgr.OnDaySwitch( daydiff );
	}
	updateRecord(PR_DAILY_CHECK_LAST_UPDATE, getNow());
}


void Player::sendActivityGain(int32_t activityId, int32_t exp, int32_t money, int32_t isEnd)
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_CHR_ACTIVITY_GAIN);
	if(NULL == packet)
	{
		return;
	}
	packet->writeInt32(activityId);
	packet->writeInt32(exp);
	packet->writeInt32(money);
	packet->writeInt32(isEnd);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}


void Player::setSyncStatusFlag()
{
	m_needSyncStatus = true;
}

void Player::setSyncToTeamFlag()
{
	m_needSyncToTeam = true;
}

//添加处理事情
void Player::addNetPacket( Answer::NetPacket *inPacket, uint32_t rsize )
{
	if ( inPacket != NULL && inPacket->getSize() > rsize )
	{
		NetPacket *packet = GAME_SERVICE.popNetpacket(inPacket->getType(), inPacket->getProc(), inPacket->getSize());
		if(NULL == packet)
		{
			return;
		}
		packet->write(inPacket->getBuffer()+rsize, inPacket->getSize()-rsize);
		packet->setSize(inPacket->getSize()-rsize);
		m_netPackets.push(packet);
	}
}

void Player::broadcastLeave()
{
	Unit::broadcastLeave();
}

void Player::setActFrontPosition()
{
	m_ActFrontPosition.runnerId = GetRunnerId();
	m_ActFrontPosition.mapid = getMapId();
	m_ActFrontPosition.x = getCurrentTile().x;
	m_ActFrontPosition.y = getCurrentTile().y;
	//ActivityMap* pAct=dynamic_cast<ActivityMap*>(m_pMap);
	//if(pAct!=NULL)
	//	m_ActFrontPosition.actId =pAct->getActivityId();
}

void Player::setOldPosition()
{
	if ( NULL == m_pMap )
	{
		return;
	}
	if ( m_pMap->IsVipGuaJiMap() || CFG_DATA.IsBossHomeMap( m_pMap->GetId() ) )
	{
		return;
	}
	m_oldPosition.runnerId = GetRunnerId();
	m_oldPosition.mapid = getMapId();
	m_oldPosition.x = getCurrentTile().x;
	m_oldPosition.y = getCurrentTile().y;
}

PlayerPosition Player::getOldPosition()
{
	return m_oldPosition;
}

void Player::addLogoutPacket(int32_t reason, int32_t param)
{
	if (m_pMap != NULL)
	{
		NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, CM_LOGOUT);
		if(NULL == packet)
		{
			return;
		}
		packet->writeInt32(reason);
		packet->writeInt32(param);
		packet->setSize(packet->getWOffset());
		m_netPackets.push(packet);
	}
	else
	{
		LOG_INFO("Player::addLogoutPacket with m_pMap == NULL, cid = %lld\n", m_chr.cid);
		setGateIndex(-1);
		saveToDB(reason, param);
		GAME_SERVICE.onPlayerLogout(this, m_cgindex);
	}
}

void Player::sendBasicInfo()
{
	sendChrInfo();
	m_extCurrency.SendCurrencyInfo();
	m_extCharBag.SendBagInfo();
	m_extCharBag.SendBagItem();
	m_extCharBag.SendBagSellItem();
	m_extCharBag.SendLimitCount();
	m_extEquip.SendEquipInfo();
	m_extEquip.SendGemInfo();
	m_PlayerDepot.SendDepotCurrency();
	m_PlayerDepot.SendDepotInfo();
	m_PlayerDepot.SendDepotItem();
	m_PlayerVip.SendVipInfo();
	sendSkillList();
	m_task.sendTaskList();
	sendActionList();
	sendAutoFight();
	sendSystemSetting();
	sendChrRecord();
	sendActivityState();
	sendLoginInfo();
//	sendPvpInfo();
	sendChrLoginInInfo();
	m_extCharSoul.SendSoulInfo();
	m_extTaskCycle.SendTaskCycle();
	m_KillMonsterTongJi.SendKillMonsterInfo();
	m_PlayerDailyActivity.SendDailyActivityInfo();
	m_extCharPet.SendPetInfoList();
	m_extCharPet.SendPetEggPoolInfo();
	m_extCharWorship.SendWorshipInfo();
	m_PlayerChouJiang.SendChouJiangItem();
	m_PlayerQiFu.SendQiFuInfo();
	m_PlayerExpBall.SendExpBallInfo();
	m_PlayerHuoYueDu.SendHuoYueDuInfo();
	m_PlayerGuanWei.SendGuanWeiInfo();
	m_PlayerShangCheng.SendLimitInfo();
	m_Achievement.SendAchievementInfo();
	m_extCharInsidePet.sendBaseInfo();
	SendHDIcon();
	ACTIVITY_MANAGER.NotifyActivityInfo( this);
	m_extCharTeam.UpdateTeamSetting();	//同步设置到global
	m_PlayerJueWei.RequestDonateRank();	//登陆成功到global请求爵位排行榜
	m_extCharFamily.OnSynPetContribution();
}


void Player::sendPlayerEquipInfo()
{
	EQUIP_MANAGER.SendPlayerEquipInfo( *this );
}

void Player::sendEquipInfo(const MemEquip &memEquip)
{
	MemEquipVector vEquip;
	vEquip.reserve( 1 );
	vEquip.push_back( memEquip );
	sendEquipInfo( vEquip );
}

void Player::sendEquipInfo(const MemEquipVector &memEquips)
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_EQUIP_INFO);
	if(NULL == packet)
	{
		return;
	}
	EQUIP_MANAGER.PacketEquipInfo( packet, memEquips );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendItemEffect(const std::string &effect)
{
	if (m_pMap != NULL)
	{
		NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_ITEM_EFFECT);
		if(NULL == packet)
		{
			return;
		}
		packet->writeInt32(getCurrentTile().x);
		packet->writeInt32(getCurrentTile().y);
		packet->writeUTF8(effect);
		packet->setSize(packet->getWOffset());
		m_pMap->broadcastAreaAround(packet, this);
	}
}

void Player::sendGainInfo(int32_t type, int64_t value, BenefitType benefitType)
{
	if (benefitType == BT_NORMAL && value == 0)
	{
		return;
	}
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_GAIN_INFO);
	if(NULL == packet)
	{
		return;
	}
	packet->writeInt32(type);
	packet->writeInt64(value);
	packet->writeInt32(benefitType);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendBuyItemInfo(int32_t item_id, int32_t item_type, int32_t count, int32_t money, int32_t index )
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_BUY_ITEM_INFO);
	if(NULL == packet)
	{
		return;
	}
	packet->writeInt32(item_id);
	packet->writeInt32(item_type);
	packet->writeInt32(count);
	packet->writeInt32(money);
	packet->writeInt32(index);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendSellItemInfo(int32_t item_id, int32_t item_type, int32_t count, int32_t money)
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_SELL_ITEM_INFO);
	if(NULL == packet)
	{
		return;
	}
	packet->writeInt32(item_id);
	packet->writeInt32(item_type);
	packet->writeInt32(count);
	packet->writeInt32(money);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}


void Player::sendKillerRankSelf()
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_KILLER_RANK_SELF);
	if(NULL == packet)
	{
		return;
	}
	packet->writeInt32(getRecord(RP_PK_RANK_COUNT));

	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendGetGoldInfo()
{
// 	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_EVERY_DAY_GET_GOLD);
// 	if(NULL == packet)
// 	{
// 		return;
// 	}
// 	packet->writeInt16((getRecord(RP_EVERY_DAY_GET_GOLD) & (1<<1)) != 0?1:0);
// 	packet->writeInt16((getRecord(RP_EVERY_DAY_GET_GOLD) & (1<<2)) != 0?1:0);
// 
// 	packet->setSize(packet->getWOffset());
// 	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

//void Player::broadcastMagicSmith(int32_t equipId)
//{
//	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_EQUIP_MAGIC_SMITH);
//	if(NULL == packet)
//	{
//		return;
//	}
//	packet->writeInt64(getCid());
//	packet->writeUTF8(getName());
//	packet->writeInt32(equipId);
//	packet->setSize(packet->getWOffset());
//	GAME_SERVICE.worldBroadcast(packet);
//}

void Player::sendYellowStone()
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_YELLOW_STONE);
	if(NULL == packet)
	{
		return;
	}
	packet->writeInt32(m_yellow_vip_level);//黄钻等级
	packet->writeInt32(m_chr.level);
	packet->writeInt16(m_is_yellow_year_vip);//是否是年费黄钻
	packet->writeInt16(getRecord(PR_YEllOW_DAILY));
	packet->writeInt16(getRecord(PR_YEAR_YEllOW_DAILY));
	packet->writeInt16(getYellowGrowId());
	packet->writeInt16(getMemYellowStone(m_chr.cid,18));//黄钻新手礼包
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendPreventWallow()
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_PREVENT_WALLOW); 
	if(NULL == packet)
	{
		return;
	}
	packet->writeInt32(m_sysUserPreventWallow.isGrowUp); //是否已认证

	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendPvpInfo()
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_PVP_INFO);
	if(NULL == packet)
	{
		return;
	}
	packet->writeInt16(m_chr.pk_mode);
	
	if (m_chr.pk_time -getNow() >0)
	{
		packet->writeInt32(m_chr.pk_time -getNow());
	}
	else
	{
		packet->writeInt32(0);
	}

	packet->writeInt16(m_chr.pk_killed_count);

	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendChrLoginInInfo()
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_CHR_LOGIN_IN_INFO);
	if(NULL == packet)
	{
		return;
	}
	packet->writeInt32(m_sysUser.uid);
	packet->writeInt64(m_chr.cid);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendGambel(const MemChrBagVector& item)
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_GAMBLE_SINGLE_INFO);
	if(NULL == packet)
	{
		return;
	}
	packet->writeInt32(item.size());
	for (MemChrBagVector::const_iterator it = item.begin();it != item.end();++it)
	{
		packet->writeInt32(it->itemId);
		packet->writeInt16(it->itemClass);
		packet->writeInt16(it->itemCount);
	}

	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendPublicChat(int32_t channel, Answer::NetPacket *inPacket)
{
	NetPacket *chatPacket = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_PUBLIC_CHAT);
	if(NULL == chatPacket)
	{
		return;
	}
	chatPacket->writeInt32(channel);
	chatPacket->writeInt64(getCid());
	chatPacket->writeInt8(m_PlayerVip.GetVipLevel());				//vip等级
	chatPacket->writeInt8(m_chr.sex);
	chatPacket->writeInt8(0);
	chatPacket->writeUTF8(getName());
	chatPacket->writeInt8( GetPlayerJueWei().GetJueWei() );
	chatPacket->writeInt32(m_yellow_vip_level);
	chatPacket->writeInt32(m_is_yellow_year_vip);
	chatPacket->writeInt32(m_is_yellow_high_vip);
	chatPacket->write(inPacket->getBuffer(), inPacket->getSize());
	chatPacket->setSize(chatPacket->getWOffset());

	if (channel == CC_WORLD)
	{
		GAME_SERVICE.worldBroadcast(chatPacket);
		updateRecord(PR_WORLD_CHAT_LAST_TIME, getNow());
	}
	else if ( channel == CC_TEAM && IsInTeam() )
	{
		m_extCharTeam.Broadcast( chatPacket );
	}
	else if (channel == CC_AREA && m_pMap != NULL)
	{
		m_pMap->broadcast(chatPacket);
	}
	else if (channel == CC_COLOR_WORLD)
	{
		GAME_SERVICE.worldBroadcast(chatPacket);
	}
	else
	{
		chatPacket->destroy();
	}
}

void Player::setNetPacketHandler(int32_t proc, NetPacketHandler handler)
{
	m_netPacketHandlers[proc] = handler;
}

Player::NetPacketHandler Player::getNetPacketHandler(int32_t proc)
{
	if (proc > CM_GAME_MIN && proc < CM_GAME_MAX)
	{
		return m_netPacketHandlers[proc];
	}
	else if (proc > IM_SOCIAL_GAME_MIN && proc < IM_SOCIAL_GAME_MAX)
	{
		switch (proc)
		{
		//case IM_SOCIAL_GAME_SYN_FAMILY: return &Player::onSocialSynFamily;
		//case IM_SOCIAL_GAME_SYN_FAMILY_LEVEL: return &Player::onSocialSynFamilyLevel;
		//case IM_SOCIAL_GAME_CREATE_FAMILY: return &Player::onSocialCreateFamily;
		//case IM_SOCIAL_GAME_UPDATE_FAMILY_CONTRIBUTE:return &Player::onSocialUpdateFamilyContribute;
		//case IM_SOCIAL_GAME_LEADER_DOWN:return &Player::onSocialFamilyLeaderDown;
		//case IM_SOCIAL_GAME_UPDATE_LEADER_CID:return &Player::onSocialUpdateLeaderCid;
		//case IM_SOCIAL_GAME_GOTO_ENEMY:return &Player::onSocialGotoEnemy;
		//case IM_SOCIAL_GAME_CHANGE_FAMILY_INFO:return &Player::onSocialChangeFamilyInfo;
		case IM_SOCIAL_GAME_USE_EXCHANGE_CODE: return &Player::onSocialUseExchangeCode;
		//case IM_SOCIAL_GAME_CHANGE_FAMILY_NAME: return &Player::onSocialChangeFamilyName;
		default: return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

void Player::setBaseAttr()
{
	CfgBornAttr *bornattr  = CFG_DATA.getBornAttr(getJob());
	if (bornattr != NULL)
	{
		for (AttrAddonVector::iterator it = bornattr->bornAttr.begin(); it != bornattr->bornAttr.end();++it)
		{
			AddAttrValue( static_cast<CObjAttrs::Index_T>( it->index ),it->addon );
		}
	}

	CfgLevelAttr levelAttr = CFG_DATA.getLevelAttr(getJob(),getLevel());
	if (levelAttr.job > 0 && levelAttr.level >0)
	{
		for (AttrAddonVector::iterator it = levelAttr.addonattr.begin(); it != levelAttr.addonattr.end();++it)
		{
			AddAttrValue( static_cast<CObjAttrs::Index_T>( it->index ),it->addon );
		}
	}

	//if (m_chr.change_flag)
	//{
	//	 CfgChangeJobAttr*pChangeAttr = CFG_DATA.getChangeJobAttr(m_chr.change_flag);//转职标示
	//	 if (pChangeAttr != NULL)
	//	 {
	//		 for (AttrAddonVector::iterator it = pChangeAttr->changeAttr.begin(); it != pChangeAttr->changeAttr.end();++it)
	//		 {
	//			 AddAttrValue( static_cast<CObjAttrs::Index_T>( it->index ), it->addon );
	//		 }
	//	 }
	//}

}

void Player::recalcAttr()
{
	ResetAttrs();
    setBaseAttr();//设置基础攻击

	for (MemChrSkillVector::iterator it = m_skills.begin(); it != m_skills.end(); ++it)
	{
		CfgSkill *pCfgSkill = CFG_DATA.getSkill(it->id);
		if (pCfgSkill != NULL && pCfgSkill->type == SK_PASSIVE && pCfgSkill->buff > 0)
		{
			CfgBuff *pCfgBuff = CFG_DATA.getBuff(pCfgSkill->buff);
			if (pCfgBuff != NULL)
			{
				for (int32_t i = 0; i < 2; ++i)
				{
					if (pCfgBuff->buffAttr[i].attr > 0)
					{
						AddAttrValue( static_cast<CObjAttrs::Index_T>( pCfgBuff->buffAttr[i].attr ), pCfgBuff->buffAttr[i].getAddon(it->level));
					}
				}
			}
		}
	}

	m_extEquip.AddCharAttr();
	m_extCharBag.AddCharAttr();
	m_PlayerDepot.AddCharAttr();
	m_extCharPet.AddZoariumPetAttr();
	m_extCharPet.AddKnightPetAttr();
	m_extCharPet.AddQiShiAttr();
	m_extCharPet.AddMoveSpeed();
	m_PlayerFaBao.AddPlayerAttr();
	m_Fighting.AddFighting();
	m_extCharInsidePet.AddAttrToTarget( this );
	m_extCharSoul.AddCharAttr();
	m_PlayerVip.AddVipAttr();
	m_PlayerGuanWei.AddGuanWeiAttr();
	AddAppendAttr();

	adjustUnitAttr();

	setNeedSync();
	setNeedSyncSelf();
	KAI_FU_HUO_DONG.UpdateHDData( this,KFHDC_BATTLE_POINT,getBattle() );
}

int32_t Player::verifyBagInfoForGroup(Answer::NetPacket *inPacket,MemChrBagVector &slotDataVector,int32_t item_type,int32_t count)
{
	if(NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t slotCount = inPacket->readInt32();
	BagSlotVector slotVector;
	if (slotCount >100 || slotCount < 0)
	{
		return ERR_INVALID_DATA;
	}

	slotVector.reserve(slotCount);
	for (int32_t i = 0; i < slotCount;++i)
	{
		BagSlot slot = {};
		slot.bag = inPacket->readInt32();
		slot.slot = inPacket->readInt32();

		slotVector.push_back(slot);
	}

	if (count < 0)
	{
		return ERR_INVALID_DATA;
	}

	int32_t item_count = 0;
	int32_t item_id = 0;
	for (BagSlotVector::iterator it = slotVector.begin();it != slotVector.end();++it)
	{
		MemChrBag slot = getBagSlotData( it->slot );
		if (slot.itemId == 0)
		{
			return ERR_INVALID_DATA;
		}

		CfgItem*pItem = CFG_DATA.getItem(slot.itemId);
		if (pItem == NULL || pItem->type != item_type)
		{
			return  ERR_INVALID_DATA;
		}

		item_id = pItem->id;
		item_count += slot.itemCount;
	}

	int32_t newItemCount = item_count%count == 0?(item_count/count+1):item_count/count;
	
	MemChrBag bag = {};
	bag.itemId = item_id;
	bag.itemClass = IC_NORMAL;
	bag.itemCount = newItemCount;

	slotDataVector.push_back(bag);

	return ERR_OK;

}

int32_t Player::verifyBagInfo( const Int32Vector& vSlot, int32_t nItemId, int32_t nCount )
{
	if (nCount <= 0)
	{
		return ERR_OK;
	}

	int32_t slotCount = vSlot.size();
	for ( int32_t i = 0; i < slotCount; ++i )
	{
		MemChrBag slot = getBagSlotData( vSlot[i] );
		if (slot.itemId == 0)
		{
			return ERR_INVALID_DATA;
		}

		if ( slot.itemId != nItemId )
		{
			return  ERR_INVALID_DATA;
		}

		if (slot.itemCount >= nCount)
		{
			nCount = 0;
			break;
		}
		else
		{
			nCount -= slot.itemCount;   
		}
	}

	if (nCount > 0)
	{
		return ERR_INVALID_DATA;
	}

	return ERR_OK;
}

bool Player::queryBagInfo( Answer::NetPacket* inPacket, Int32Vector& vSlot )
{
	if ( NULL == inPacket )
	{
		return false;
	}

	if(NULL == inPacket)
	{
		return false;
	}
	int32_t slotCount = inPacket->readInt32();
	if (slotCount > 150 || slotCount < 0)
	{
		return false;
	}

	vSlot.resize( slotCount );
	for (int32_t i = 0; i < slotCount;++i)
	{
		vSlot[i] = inPacket->readInt32();
	}
	return false;
}

int32_t Player::queryAutoBuyBagInfo( const Int32Vector& vSlot,int32_t nItemId,int32_t& count,int32_t nShopId )
{
	if (count <= 0)
	{
		return ERR_OK;
	}

	int32_t nCount = count;
	int32_t slotCount = vSlot.size();
	for ( int32_t i = 0; i < slotCount; ++i )
	{
		MemChrBag bagSlot = getBagSlotData( vSlot[i] );
		if ( bagSlot.itemId == 0 )
		{
			return ERR_INVALID_DATA;
		}

		if ( bagSlot.itemId != nItemId )//
		{
			return  ERR_INVALID_DATA;
		}

		if ( bagSlot.itemCount >= nCount)
		{
			nCount = 0;
			break;
		}
		else
		{
			nCount -= bagSlot.itemCount;
		}
	}

	if ( nCount > 0 )
	{
		//购买数量
		//
		//int32_t nCount = count;

		CfgGameShop *pCfgGameShop = CFG_DATA.GetGameShop(nShopId);
		if (pCfgGameShop == NULL)
		{
			return ERR_INVALID_DATA;
		}

		int32_t totalCost = pCfgGameShop->Price * nCount;
		int64_t nTotalGold = GetCurrency( CURRENCY_GOLD );
		if (nTotalGold < totalCost)
		{
			return ERR_INVALID_DATA;
		}

		DecCurrency( CURRENCY_GOLD, totalCost, GCR_AUTOBUYBAG, nShopId );

	}

	// 返回需要扣除的道具数量
	count -= nCount;

	return ERR_OK;
}

bool Player::isDeadProc( ProcId_t nProc ) const
{
	if ( nProc > IM_MIN && nProc < IM_MAX )
	{
		return true;
	}
	switch ( nProc )
		{
		case CM_SAFE_REVIVE:
		case CM_ON_SITE_REVIVE:
		case CM_STRONG_REVIVE:
		case CM_MOVE:
		case CM_LOGOUT:
		case CM_ASK_RANSOM_INFO:
		case CM_ASK_SPOILS_INFO:
		case CM_EQUIP_REQUEST_INFO:
		case CM_PET_MUTI_HATCHING:
		case CM_PET_REHATCHING:
		case CM_ASK_FA_BAO_INFO:
		case CM_GAME_PUBLIC_CHAT:
		case CM_BUY_SHANG_CHENG_ITEM:	// 军团战购买复活道具
		case CM_BUY_VIP:
			return true;
		default:break;
		}
	return false;
}

//自动处理一些事情
void Player::checkNetPackets()
{
	NetPacket *packet = NULL;
	while ((packet = m_netPackets.pop()) != NULL)
	{
		ProcId_t nProc = packet->getProc();

#ifdef _DEBUG
		//printf ( "Player::checkNetPackets() on net packet procId = %d\n", nProc );
#endif
		if ( !isAlive() && !isDeadProc( nProc ) )
		{
			packet->destroy();
			continue;
		}

		NetPacketHandler handler = getNetPacketHandler(packet->getProc());
		if (handler != NULL && (this->*handler)(packet) == ERR_LEAVE_KINGDOM)
		{
			packet->destroy();
			break;
		}

		int32_t nErr = m_ExtSysMgr.DispatchNetDatas( packet->getProc(), packet );
		packet->destroy();
	}
}

int32_t Player::onLogout(Answer::NetPacket *inPacket)
{
	if(NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t reason = inPacket->readInt32();
	int32_t param = inPacket->readInt32();
	if (reason == LR_DISCONNECT || reason == LR_KICK_SELF || reason == LR_KICK_BY_GM)
	{
		++m_chr.logout_count;
		DB_SERVICE.logPlayerLogin(getCid(), 0, getNow());
	}

	if ( !m_extOperateLimit.CheckIsLimitedForever( PR_FIRST_ENTER_GAME ) )
	{
		m_extOperateLimit.AddLimitCount( PR_FIRST_ENTER_GAME, 1 );
	}
	if (m_pMap != NULL)
	{
		int32_t nRunnerId = m_pMap->GetRunnerId();

		m_extCharTeam.LeaveTeam();

		if (m_trailer != NULL)
		{
			m_trailer->onLogout();
			m_trailer = NULL;
		}

		if (InDungeon())
		{
			Dungeon* pDungeon = dynamic_cast<Dungeon*>(m_pMap);
			if (pDungeon != NULL)
			{
				if ( pDungeon->IsWin() )
				{
					pDungeon->GetReward( this );
				}
				else
				{
					LogDungeon logDungeon = {};
					logDungeon.cid = getCid();
					logDungeon.did = pDungeon->getDungeonId();
					logDungeon.start_time = pDungeon->GetStartTime();
					logDungeon.finish_time = getNow();
					logDungeon.state = 2;

					DB_SERVICE.updateDungeon( logDungeon );
				}
			}
		}
		else if ( InActivity() )
		{
			leaveActivity();
		}

		m_extCharBag.OnLogout();
		m_extCharTeamDungeon.LeaveTeamDungeon();
		m_extCharAuction.OnLogOut();

		//Kingdom *pKingdom = m_pMap->getKingdom();
		//if (pKingdom != NULL)
		//{
		//	pKingdom->postMessage(GMC_KINGDOM_PLAYER_LOGOUT, this, NULL, m_cgindex, 0);
		//}

		m_chr.last_logout_time = getNow();
		sendUpdateSocialPlayerInfo(PII_LAST_LOGOUT_TIME, m_chr.last_logout_time);

		m_sysUser.last_logout_time = getNow();
		m_sysUser.total_online_time += getNow() - m_sysUser.last_login_time;

		saveToDB(reason, param);

		broadcastLeave();
		m_pMap->removePlayer(this, true);

		//GAME_SERVICE.onPlayerLogout( this, m_cgindex );
		// 需要整个Update做完后在清理内存，所以在此投递
		MAP_MANAGER.PostMsg( nRunnerId, GMC_PLAYER_LOGOUT, this, NULL, m_cgindex );
	}
	else
	{
		LOG_INFO("Player %d logout with Map = NULL\n", getCid());
	}

	return ERR_LEAVE_KINGDOM;
}

int32_t Player::onSyncTime(Answer::NetPacket *inPacket)
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_SYNC_TIME);
	if(NULL == packet)
	{
		return ERR_INVALID_DATA;
	}
	int32_t nNew = getNow();

	packet->writeInt32(nNew);

	packet->writeInt32(DayTime::daydiff(CFG_DATA.getServerStartTime())+1);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);

	return ERR_OK;
}

int32_t Player::onMove(Answer::NetPacket *inPacket)
{
	if(NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	if (m_pMap == NULL)
	{
		return ERR_INVALID_DATA;
	}
	if ( IsInStall() )
	{
		return ERR_INVALID_DATA;
	}
	if ( HasBuffState( CObjState::OBS_BINGDONG ) || HasBuffState( CObjState::OBS_YUNXUAN ) )
	{
		return ERR_INVALID_DATA;
	}
	if ( getTick() < m_jumpTime + 800)
	{
		return ERR_INVALID_DATA;
	}
	BreakGather();

	int16_t currentX = inPacket->readInt16();
	int16_t currentY = inPacket->readInt16();
	int16_t targetX = inPacket->readInt16();
	int16_t targetY = inPacket->readInt16();
	int16_t finalX = inPacket->readInt16();
	int16_t finalY = inPacket->readInt16();
	inPacket->readInt8();//direction no usage now

	Tile *pCurrentTile = m_pMap->getTile(currentX, currentY);
	Tile *pTargetTile = m_pMap->getTile(targetX, targetY);
	Tile *pFinalTile = m_pMap->getTile(finalX, finalY);
	if ( GetStartProtect() > 0 )
	{
		SetStartProtect( 0 );
	}
	if (pCurrentTile == NULL || pTargetTile == NULL || pFinalTile == NULL)
	{
		return ERR_INVALID_DATA;
	}

	int32_t ndistan=getCurrentTile().tileDistance(Position(currentX, currentY));
	//if(ndistan ==0) return ERR_INVALID_DATA;
	if (ndistan > PULL_BACK_DISTANCE)
	{
		instantMove(getCurrentTile().x, getCurrentTile().y, IMR_PULL_BACK);
		return ERR_INVALID_DATA;
	}
	
	setAtTile(m_pMap, currentX, currentY);

	setTargetTile(targetX, targetY);
	m_extCharPet.SetTargetTile( targetX, targetY, getDirection() );

	broadcastMove();

	return ERR_OK;
}

int32_t Player::onTrailerMove(Answer::NetPacket *inPacket)
{
	if (m_pMap == NULL || m_trailer == NULL || NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}

	int32_t mapId = inPacket->readInt32();
	int16_t currentX = inPacket->readInt16();
	int16_t currentY = inPacket->readInt16();
	int16_t targetX = inPacket->readInt16();
	int16_t targetY = inPacket->readInt16();
	int16_t finalX = inPacket->readInt16();
	int16_t finalY = inPacket->readInt16();
	int8_t direction = inPacket->readInt8(); //direction, no usage now

	return m_trailer->move(mapId,currentX, currentY, targetX, targetY, finalX, finalY, (Direction)direction);
}

int32_t Player::onJump(Answer::NetPacket *inPacket)
{
	if(NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	if (m_pMap == NULL)
	{
		return ERR_INVALID_DATA;
	}
	if ( !m_pMap->CanJump() )
	{
		return ERR_INVALID_DATA;
	}
	int32_t	EffEctValue =	GetCharPet().GetKnightEffectValue( PK_LEITING );
	if ( EffEctValue <= 0 )
	{
		return ERR_INVALID_DATA;
	}
	if ( EffEctValue > GetPP() )
	{
		return ERR_INVALID_DATA;
	}
	if ( HasBuffState( CObjState::OBS_BINGDONG ) || HasBuffState( CObjState::OBS_YUNXUAN ) )
	{
		return ERR_INVALID_DATA;
	}

// 	if (getTick() < m_jumpTime + 1000)
// 	{
// 		return ERR_INVALID_DATA;
// 	}

	BreakGather();

	int16_t currentX = inPacket->readInt16();
	int16_t currentY = inPacket->readInt16();
	int16_t targetX = inPacket->readInt16();
	int16_t targetY = inPacket->readInt16();

	if ( !m_pMap->isWalkablePosition( targetX, targetY ) )
	{
		return ERR_INVALID_DATA;
	}
	AddPP( -1 * EffEctValue );
	int32_t nDistan=getCurrentTile().tileDistance(Position(targetX, targetY));
	if ( nDistan> 10)
	{
		int32_t ndisx=getCurrentTile().tileDistance(Position(currentX, currentY));
		int32_t ndisy=Position(currentX, currentY).tileDistance(Position(targetX, targetY));
		if ( ndisx> 6 ||  ndisy> 10)
		{
			instantMove(getCurrentTile().x, getCurrentTile().y, IMR_PULL_BACK);
			return ERR_INVALID_DATA;
		}
	}

	instantMove(targetX, targetY, IMR_JUMP);

	m_jumpTime = getTick();
	return ERR_OK;
}

int32_t Player::onHit(Answer::NetPacket *inPacket)
{
	if(NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	if (m_pMap == NULL)
	{
		return ERR_INVALID_DATA;
	}

	if ( HasBuffState( CObjState::OBS_BINGDONG ) || HasBuffState( CObjState::OBS_YUNXUAN ) )
	{
		return ERR_INVALID_DATA;
	}

	BreakGather();

	int16_t currentX = inPacket->readInt16();
	int16_t currentY = inPacket->readInt16();
	int16_t targetX = inPacket->readInt16();
	int16_t targetY = inPacket->readInt16();

	if ( !m_pMap->isWalkablePosition( targetX, targetY ) )
	{
		return ERR_INVALID_DATA;
	}

	if (getCurrentTile().tileDistance(Position(targetX, targetY)) > 16)
	{
		if (getCurrentTile().tileDistance(Position(currentX, currentY)) > 6 || Position(currentX, currentY).tileDistance(Position(targetX, targetY)) > 10)
		{
			instantMove(getCurrentTile().x, getCurrentTile().y, IMR_PULL_BACK);
			return ERR_INVALID_DATA;
		}
	}

	instantMove(targetX, targetY, IMR_HIT);
	return ERR_OK;
}

int32_t Player::onUnitThrowed(Answer::NetPacket *inPacket)
{
	if (m_pMap == NULL || NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	EntityId_t id = inPacket->readInt64();
	int32_t type = inPacket->readInt32();
	Unit* pUnit = GAME_SERVICE.getUnit( id, type, GetRunnerId() );
	if (pUnit != NULL)
	{
		pUnit->onThrowed();
		return ERR_OK;
	}
	return ERR_INVALID_DATA;
}

int32_t Player::onKickOutMove(Answer::NetPacket *inPacket)
{
	if (m_pMap == NULL || NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}

	if ( HasBuffState( CObjState::OBS_BINGDONG ) || HasBuffState( CObjState::OBS_YUNXUAN ) )
	{
		return ERR_INVALID_DATA;
	}

	EntityId_t id = inPacket->readInt64();
	int32_t type = inPacket->readInt32();
	Unit* pUnit = GAME_SERVICE.getUnit( id, type, GetRunnerId() );
	if (pUnit == NULL)
	{
		return ERR_INVALID_DATA;
	}

	int16_t currentX = inPacket->readInt16();
	int16_t currentY = inPacket->readInt16();
	int16_t targetX = inPacket->readInt16();
	int16_t targetY = inPacket->readInt16();
	int16_t finalX = inPacket->readInt16();
	int16_t finalY = inPacket->readInt16();
	int8_t dir = inPacket->readInt8();//direction no usage now

	pUnit->onKickOutMove(currentX,currentY,targetX,targetY,finalX,finalY,static_cast<Direction>(dir));
	return ERR_OK;
}


int32_t	Player::onArrive(Answer::NetPacket *inPacket)
{
	return ERR_OK;
}

//int32_t Player::onGetMatchMsg(Answer::NetPacket *inPacket)
//{
//	if(inPacket ==NULL) return ERR_INVALID_DATA;
//	int16_t actid =inPacket->readInt16();
//	ActivityMap* pAct =MAP_MANAGER.getActivityMap(actid);
//	if (pAct == NULL)
//	{
//		return  ERR_INVALID_DATA;
//	}
//	bool flag=pAct->getMatchMsg(this);
//	if(!flag)
//	{
//		return GAME_SERVICE.replySuccess(m_cgindex, inPacket->getProc(),actid);
//	}
//	return ERR_OK;
//}

int32_t Player::initYellowStone(Answer::NetPacket *inPacket)
{
	LOG_INFO("initYellowStone");
	sendYellowStone();

	return ERR_OK;
	
}


int Player::BirthdayIsRight(std::string cardId)
{
	string year,month,day;

	year=cardId.substr(6,4);   //取18位身份证中的年
	month=cardId.substr(10,2); //月
	day=cardId.substr(12,2);   //日

	int nYear = atoi(year.c_str());
	int nMonth = atoi(month.c_str());
	int nDay = atoi(day.c_str());

	if((nYear == 0) || (nMonth == 0) || (nDay == 0))
	{
		return 0;
	}

	if(GetDay(nYear, nMonth) >= nDay)
	{
		//验证是否成年
		tm tmNow = getLocalNow();
		tmNow.tm_mon += 1;
		tmNow.tm_year += 1900;

		if (tmNow.tm_year - nYear < MAX_IDENTITYCARD_LENGTH)
		{
			return 0;
		}
		else if (tmNow.tm_year - nYear > MAX_IDENTITYCARD_LENGTH)
		{
			return 1;
		}
		else
		{
			if (tmNow.tm_mon > nMonth)
			{
				return 1;
			}
			else if (tmNow.tm_mon  < nMonth)
			{
				return 0;
			}
			else
			{
				if (tmNow.tm_mday >= nDay)
				{
					return 1;
				}

				return 0;
			}
		}

	}

	return 0;	
}

int Player::GetDay(int year,int month)
{
	int day=0;

	switch (month)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		{
			day=31;
			break;
		}
	case 4:
	case 6:
	case 9:
	case 11:
		{
			day=30;
			break;
		}
	case 2:
		{
			if(((year%4==0)&&(year%100!=0))||(year%400==0))
			{
				day=29;
			}
			else
			{
				day=28;
			}
			break;
		}
	default: 
		{
			day=-1;
			break;
		}
	}

	return day;
}

//防沉迷验证
int32_t Player::checkPreventWallow(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	std::string name = inPacket->readUTF8(true);
	std::string identitycard = inPacket->readUTF8(true);

	bool isUseFull = isPreventWallow(identitycard);
	if (!isUseFull)
	{
		//验证成功
		m_sysUserPreventWallow.isGrowUp = 1;
		snprintf(m_sysUserPreventWallow.name, sizeof(m_sysUserPreventWallow.name)-1, name.c_str());
		snprintf(m_sysUserPreventWallow.identitycard, sizeof(m_sysUserPreventWallow.identitycard)-1, identitycard.c_str());
	}

	sendPreventWallow();

	return ERR_OK;
}


bool Player::CheckString(std::string card)
{
	char szCardBuf[MAX_IDENTITYCARD_LENGTH+1] = {0};

	strcpy(szCardBuf, card.c_str());

	char szStr[] = "1234567890";
	int nFlag = 0;

	int nLen1 = (int)strlen(szCardBuf);
	int nLen2 = (int)strlen(szStr);

	for (int i = 0; i < nLen1; i++)
	{
		nFlag = 0;
		for (int j = 0; j < nLen2; j++)
		{
			if (szCardBuf[i] == szStr[j])
			{
				nFlag = 1;
				break;
			}
		}

		if (nFlag == 1)
		{
			continue;
		}
		else
		{
			if (i == 17)
			{
				if (szCardBuf[i] == 'x' || szCardBuf[i] == 'X')
				{
					nFlag = 1;
					break;
				}
			}

			nFlag = 2;
			break;
		}
	}

	if (nFlag == 1)
	{
		return true;
	}

	return false;
}

//是否沉迷
bool Player::isPreventWallow(std::string card)
{
	bool bResult = false;
	int  nLen = (int)card.length();

	if (nLen != MAX_IDENTITYCARD_LENGTH)
	{
		return true;
	}

	bool bFlag = CheckString(card);
	if(!bFlag)
	{
		return true;
	}

	if(!BirthdayIsRight(card))
	{
		return true;
	}

	int no[]={7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2}; //权值
	char id[]={'1','0','X','9','8','7','6','5','4','3','2'};//校验码常数

	const char *p=card.c_str();
	int i=0,wi=0,sum=0;

	for(; i<nLen-1; i++)
	{
		wi=(*(p+i)-'0')*no[i];
		sum+=wi;
	}

	if(*(p+i)<'0'||*(p+i)>'9')
	{
		if (*(p+i)!='X'&&*(p+i)!='x')
		{
			bResult = true;			  //身份证最后一位输入错误
		}
	}

	wi=sum%11;
	if(*(p+17)=='x'||*(p+17)=='X')   //最后一位为'x'或'X';
	{
		if(id[wi]!='x'&&id[wi]!='X')
			bResult = true;
	}
	else if(id[wi]!=*(p+17))        //判断计算出的身份证校验码与输入是否相符
	{
		bResult = true;
	}	

	return bResult;
}

int32_t Player::getYellowAward(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t id = inPacket->readInt32(); //黄钻礼包的id

	int32_t yellow_level = m_yellow_vip_level;//黄钻等级

	CfgYellowStone* yellowStone = CFG_DATA.getYellowStone(id);

	if (yellow_level <= 0 || m_is_yellow_vip <= 0)
	{
		return ERR_INVALID_DATA;
	}

	if(yellowStone == NULL)
	{
		return ERR_INVALID_DATA;
	}

	MemChrBagVector awards;
	awards.assign(yellowStone->awards.begin(),yellowStone->awards.end());

	if(yellowStone->yellow_type == YELLOW_DAILY)
	{
		if(getRecord(PR_YEllOW_DAILY) != 0)
		{
			return ERR_INVALID_DATA;
		}

		if(yellowStone->level != yellow_level)
		{
			return ERR_INVALID_DATA;
		}

		if (!m_extCharBag.AddItem( awards,IACR_YELLOW_STONE_REWARD ))
		{
			return GAME_SERVICE.replyfailure(m_cgindex,inPacket->getProc(),ERR_BAG_IS_FULL);
		}
		BenefitType bnfType = benefitType();
		updateRecord(PR_YEllOW_DAILY,1);
		
	}
	else if (yellowStone->yellow_type == YELLOW_YEAR)
	{
		if(m_is_yellow_year_vip <= 0)
		{
			return ERR_INVALID_DATA;
		}

		if(getRecord(PR_YEAR_YEllOW_DAILY) != 0)
		{
			return ERR_INVALID_DATA;
		}

		if (!m_extCharBag.AddItem( awards, IACR_YEAR_YELLOW_STONE_REWARD ))
		{
			return GAME_SERVICE.replyfailure(m_cgindex,inPacket->getProc(),ERR_BAG_IS_FULL);
		}
		BenefitType bnfType = benefitType();
		updateRecord(PR_YEAR_YEllOW_DAILY,1);

	}
	else if (yellowStone->yellow_type == YELLOW_GROW_UP)
	{
		if (getMemYellowStone(m_chr.cid,id) == 1)
		{
			return ERR_INVALID_DATA;
		}

		if (yellowStone->level > m_chr.level)
		{
			return ERR_INVALID_DATA;
		}

		if (!m_extCharBag.AddItem( awards, IACR_YELLOW_STONE_GROW_UP ))
		{
			return GAME_SERVICE.replyfailure(m_cgindex,inPacket->getProc(),ERR_BAG_IS_FULL);
		}
		BenefitType bnfType = benefitType();

		MemYellowStone mys = {};
		mys.cid = m_chr.cid;
		mys.id = id;
		mys.get_time = getNow();
		m_mysVector.push_back(mys);
		DB_SERVICE.insertMemYelloStone(mys,1);//增加mem表

	}
	else if (yellowStone->yellow_type == YELLOW_NEW)
	{
		if (getMemYellowStone(m_chr.cid,id) == 1)
		{
			return ERR_INVALID_DATA;
		}

		if (!m_extCharBag.AddItem( awards, IACR_YELLOW_STONE_GROW_UP ))
		{
			return GAME_SERVICE.replyfailure(m_cgindex,inPacket->getProc(),ERR_BAG_IS_FULL);
		}
		BenefitType bnfType = benefitType();

		MemYellowStone mys = {};
		mys.cid = m_chr.cid;
		mys.id = id;
		mys.get_time = getNow();
		m_mysVector.push_back(mys);
		DB_SERVICE.insertMemYelloStone(mys,1);//增加mem表

	}	

	MemYellowStone mys = {};
	mys.cid = m_chr.cid;
	mys.id = id;
	mys.get_time = getNow();
	DB_SERVICE.insertMemYelloStone(mys,2);//增加log表

	return ERR_OK;
	
}

int32_t Player::onSwitchMap(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	if (m_pMap == NULL)
	{
		return ERR_INVALID_DATA;
	}

	if ( IsInStall() )
	{
		return ERR_INVALID_DATA;
	}

	int32_t kingdomid = inPacket->readInt32();
	int32_t mapid = inPacket->readInt32();
	int32_t x = inPacket->readInt32();
	int32_t y = inPacket->readInt32();
	Map *pTargetMap = MAP_MANAGER.GetMap( mapid );
	if (pTargetMap == NULL || pTargetMap == m_pMap)
	{
		return ERR_INVALID_DATA;
	}
	return switchMap(pTargetMap, x, y, false);
}
////进入活动副本
//int32_t Player::onEnterActDungeon(Kingdom* pKingdom,ActPKMap* pMap)
//{
//	if(!InActivity()) return ERR_INVALID_DATA;
//	if(pMap ==NULL) return ERR_INVALID_DATA;
//	if(pMap->isPK()) return ERR_INVALID_DATA;
//	ActivityMap *pCurMap= dynamic_cast<ActivityMap*>(m_pMap);
//	if(pCurMap ==NULL)  
//	{
//		return ERR_INVALID_DATA;
//	}
//	CfgMapRegion *pCfgRegion = CFG_DATA.getMapRegion(900502);
//	if(pCfgRegion ==NULL) return ERR_INVALID_DATA;
//	Position pos = pMap->getRandomWalkablePositionInRegion(*pCfgRegion);
//	if(pos.x ==-1 && pos.y ==-1) return ERR_INVALID_DATA;
//	setActFrontPosition();//保存活动地图
//	broadcastLeave();
//	pCurMap->removePlayer(this, false,1);
//	pMap->addPlayer(this, pos.x, pos.y);
//	return ERR_OK;
//}
void Player::onActAddHL()
{
	safeRevive();
	m_bDie=false;
}
//int32_t Player::onLeaveActDungeon(Answer::NetPacket *inPacket)
//{
//	onLeaveActDungeon(0,1);
//	return ERR_OK;
//}
//
////离开活动副本
//int32_t Player::onLeaveActDungeon(int32_t mapid,int16_t groupId)
//{	
//	if(m_pMap ==NULL) return ERR_INVALID_DATA;
//	ActPKMap *pAct= dynamic_cast<ActPKMap*>(m_pMap);
//	if (pAct == NULL)
//	{
//		return ERR_INVALID_DATA;
//	}
//	ActivityMap	*pTargetMap =dynamic_cast<ActivityMap*>(MAP_MANAGER.GetMap( m_ActFrontPosition.mapid ));
//	if (pTargetMap == NULL || pTargetMap == dynamic_cast<Map*>(pAct))
//	{
//		return ERR_INVALID_DATA;
//	}
//
//	broadcastLeave();
//	pAct->removePlayer(this,false);
//	pTargetMap->addPlayer(this, m_ActFrontPosition.x, m_ActFrontPosition.y);
//	return ERR_OK;
//}

int32_t Player::onGetAwardActivity(Answer::NetPacket *inPacket)
{
	//if (NULL == inPacket)
	//{
	//	return ERR_INVALID_DATA;
	//}
	//int32_t actid =inPacket->readInt32();
	//ActivityMap *pAct =NULL;
	//if(InActivity())
	//{
	//	pAct =dynamic_cast<ActivityMap*>(m_pMap);
	//	if (pAct==NULL) return ERR_INVALID_DATA;
	//	{
	//		pAct->getWealByCid(this);
	//		leaveActivity();
	//	}
	//}
	//else
	//{
	//	pAct =MAP_MANAGER.getActivityMap(actid);
	//	if (pAct==NULL) return ERR_INVALID_DATA;
	//	pAct->getWealByCid(this);
	//}
	return ERR_OK;
}

int32_t	Player::OnEnterBossHome( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket || NULL == m_pMap )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t MapId  = inPacket->readInt32();
	if ( IsInStall() )
	{
		return ERR_INVALID_DATA;
	}
	if( InActivity() ) 
	{
		return ERR_SYETEM_ERR;
	}
	if ( InDungeon() )//副本
	{
		return ERR_SYETEM_ERR;
	}
	CfgBossHome* pBossHome = CFG_DATA.GetBossHomeCfg( MapId );
	if ( NULL == pBossHome )
	{
		return ERR_SYETEM_ERR;
	}
	switch( pBossHome->Grade )
	{
	case 1:
	case 2:
	case 3:
		{
			if ( pBossHome->NeedVipLevel > GetPlayerVip().GetVipLevel() )
			{
				if ( !DecCurrency( CURRENCY_GOLD, pBossHome->NeedGold, GCR_ENTER_BOSS_HOME, pBossHome->Grade ) )
				{
					return ERR_SYETEM_ERR;
				}
			}
			else
			{
				DB_SERVICE.logCurrency( getCid(), getName(), CURRENCY_GOLD, GCR_ENTER_BOSS_HOME, 0, pBossHome->Grade, getNow() );	// 插一条0消耗日志
			}
		}
		break;
	case 4:
		{
			if ( !GetBag().RemoveItem( pBossHome->CostItem, IDCR_ENTER_BOSS_HOME ) )
			{
				return ERR_SYETEM_ERR;
			}
		}
		break;
	case 999:
		{
			if ( !DecCurrency( CURRENCY_CASH, pBossHome->NeedCash, GCR_ENTER_BOSS_HOME, pBossHome->Grade ) )
			{
				return ERR_SYETEM_ERR;
			}
		}
		break;
	default:
		return ERR_SYETEM_ERR;
	}

	Map *pTargetMap = MAP_MANAGER.GetMap( MapId );
	if( pTargetMap == m_pMap )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t err = pTargetMap->canEnter( this );
	if (err != ERR_OK)
	{
		return ERR_SYETEM_ERR;
	}
	std::vector<Position> PosVector = pBossHome->EnterPosVector;
	std::random_shuffle( PosVector.begin(), PosVector.end());
	if ( PosVector.begin() == PosVector.end() )
	{
		return ERR_SYETEM_ERR;
	}
	Position pos = *(PosVector.begin());  //随机获取坐标
	if( pos.x ==-1 && pos.y ==-1 ) 
	{
		return ERR_SYETEM_ERR;
	}

	if ( !CFG_DATA.IsBossHomeMap( m_pMap->GetId() ) )
	{
		setOldPosition();
	}
	broadcastLeave();
	if ( pTargetMap->GetRunnerId() == m_pMap->GetRunnerId() )
	{
		m_pMap->removePlayer(this, false);
		pTargetMap->addPlayer(this, pos.x, pos.y);
	}
	else
	{
		m_pMap->removePlayer(this, false);
		MAP_MANAGER.PostMsg( pTargetMap->GetRunnerId(), GMC_PLAYER_LEAVE_MAP, this, pTargetMap, pos.x, pos.y );
	}
	GAME_SERVICE.replySuccess(m_cgindex, inPacket->getProc(), MapId);
	return ERR_OK;
}

void Player::SendBossHomeInfo( int32_t MapId )
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_SEND_BOSS_HOME_INFO);
	if (NULL == packet)
	{
		return;
	}
	WORLDBOSS.PacketBossHomeInfo( packet, MapId );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo( m_cgindex, packet);
}

int32_t Player::OnLevelBossHome( Answer::NetPacket *inPacket )
{
	LevelBossHome();
	return ERR_OK;
}

int32_t Player::LevelBossHome()
{
	if( NULL == m_pMap )
	{
		return ERR_INVALID_DATA;
	}

	if ( !isAlive() )
	{
		FillHP();
		FillMP();
	}

	Map	*pTargetMap = MAP_MANAGER.GetMap( m_oldPosition.mapid );
	if ( pTargetMap == NULL || pTargetMap == m_pMap )
	{
		LOG_INFO( " pTargetMap == pAct(%d,%d)", m_oldPosition.runnerId, m_oldPosition.mapid );
		return ERR_INVALID_DATA;
	}

	broadcastLeave();
	if ( m_pMap->GetRunnerId() == pTargetMap->GetRunnerId() )
	{
		m_pMap->removePlayer( this, false );
		pTargetMap->addPlayer( this, m_oldPosition.x, m_oldPosition.y );
	}
	else
	{
		m_pMap->removePlayer( this, false );
		MAP_MANAGER.PostMsg( pTargetMap->GetRunnerId(), GMC_PLAYER_LEAVE_MAP, this, pTargetMap, m_oldPosition.x, m_oldPosition.y );
	}
	return ERR_OK;
}

int32_t	Player::onEnterActivity(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	if ( IsInStall() )
	{
		return ERR_INVALID_DATA;
	}

	if( InActivity() ) 
	{
		return GAME_SERVICE.replyfailure(m_cgindex, CM_SWITCH_MAP, ERR_MAP_ACTIVITY_INPLAY);
	}

	if ( InDungeon() )//副本
	{
		return GAME_SERVICE.replyfailure(m_cgindex, CM_SWITCH_MAP, ERR_MAP_ACTIVITY_INDUM);
	}

	int32_t actID = inPacket->readInt32();
	CActivity* pActivity = ACTIVITY_MANAGER.GetActivity( actID );
	if ( NULL == pActivity || !pActivity->IsRuning() )
	{
		return ERR_INVALID_DATA;
	}

	CfgActivity* pCfgActInfo = CFG_DATA.getActivity(actID);
	if (pCfgActInfo == NULL) 
	{
		return ERR_INVALID_DATA;
	}

	Map *pTargetMap = MAP_MANAGER.GetMap( pCfgActInfo->target_mapid );
	if( pTargetMap == m_pMap )
	{
		return ERR_INVALID_DATA;
	}

	CActivityMap *pAct = dynamic_cast<CActivityMap*>( pTargetMap );
	if (pAct == NULL)
	{
		return GAME_SERVICE.replyfailure(m_cgindex, CM_SWITCH_MAP, ERR_MAP_ACTIVITY_NOTMAP);
	}

	int32_t err = pAct->canEnter( this );
	if (err != ERR_OK)
	{
		return GAME_SERVICE.replyfailure( m_cgindex, CM_SWITCH_MAP, err, pAct->GetId() );
	}
	
	Position pos = pAct->GetBornPos( this );
	if( pos.x ==-1 && pos.y ==-1 ) 
	{
		return GAME_SERVICE.replyfailure( m_cgindex, CM_SWITCH_MAP, ERR_MAP_ACTIVITY_NOTREG );
	}

	setOldPosition();
	broadcastLeave();
	if ( pTargetMap->GetRunnerId() == m_pMap->GetRunnerId() )
	{
		m_pMap->removePlayer(this, false);
		pTargetMap->addPlayer(this, pos.x, pos.y);
	}
	else
	{
		m_pMap->removePlayer(this, false);
		MAP_MANAGER.PostMsg( pTargetMap->GetRunnerId(), GMC_PLAYER_LEAVE_MAP, this, pTargetMap, pos.x, pos.y );
	}
	
	GAME_SERVICE.replySuccess(m_cgindex, inPacket->getProc(), actID);

	NetPacket *outPacket = GAME_SERVICE.popNetpacket(PACK_DISPATCH, IM_GAME_SOCIAL_UPDATE_ACTIVITY_STATE);
	if ( NULL == outPacket )
	{
		return ERR_INVALID_DATA;
	}

	outPacket->writeInt32(m_cgindex);
	outPacket->writeInt32(1);
	outPacket->setSize(outPacket->getWOffset());
	GAME_SERVICE.sendPacket(outPacket);

	LogActivity logActivity = {};
	logActivity.cid = getCid();
	logActivity.actid = actID;
	logActivity.acttype = pActivity->GetType();
	logActivity.time = getNow();
	if ( pActivity->GetType() == ATI_FAMILY_WAR )
	{
		logActivity.param = getFamilyId();
	}

	DB_SERVICE.InsertActivityLog( logActivity );

	return ERR_OK;
}

int32_t	Player::onLeaveActivity(Answer::NetPacket *inPacket)
{
	if ( InActivity() )
	{
		leaveActivity();
	}
	return ERR_OK;
}


bool Player::isInWorldBossActivity()
{
	return false;
}

bool Player::isFirstDay()
{
	return getNow() - CFG_DATA.getServerStartTime() < 24 * 60 * 60;
}

bool Player::isFirstSevenDay()
{
	return getNow() - CFG_DATA.getServerStartTime() < 7 * 24 * 60 * 60;
}

int32_t Player::OnEnterMoLingRuQin(Answer::NetPacket *inPacket)
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	EntityId_t npcid = inPacket->readInt64();
	Npc *npc = m_pMap->getNpc(npcid);

	if (npc == NULL || npc->getCurrentTile().tileDistance(getCurrentTile()) > 3)
	{
		return ERR_INVALID_DATA;
	}
	m_pMap->removeNpc(npcid);
	inPacket->setProc( CM_ENTER_DUNGEON );	//进入副本原来的协议,有成功包
	if ( ERR_OK != onEnterDungeon(inPacket) )
	{
		return ERR_SYETEM_ERR;
	}
	return ERR_OK;
}

int32_t Player::onEnterDungeon(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t dungeonID = inPacket->readInt32();
	CfgDungeon* pCfgDungeon = CFG_DATA.getDungeon(dungeonID);
	if (pCfgDungeon == NULL)
	{
		return ERR_INVALID_DATA;
	}

	if ( IsInStall() )
	{
		return ERR_INVALID_DATA;
	}

	// 副本内切换地图
	Dungeon *pCurDungeon = dynamic_cast<Dungeon*>(m_pMap);
	if ( pCurDungeon != NULL )
	{
		switch ( pCurDungeon->getDungeonType() )
		{
		case DT_UP_TOWER:
			{
				if ( pCfgDungeon->type != DT_UP_TOWER )
				{
					return ERR_INVALID_DATA;
				}
				break;
			}
		case DT_MY_WORLD:
			{
				if ( pCfgDungeon->type != DT_MY_WORLD )
				{
					return ERR_INVALID_DATA;
				}
				break;
			}
		default: return ERR_INVALID_DATA;
		}
		leaveDungeon();
	}
	else if ( InDungeon() || InActivity() )
	{
		return ERR_INVALID_DATA;
	}

	// TODO:组队副本判定
	if ( m_extCharTeamDungeon.IsInTeamDungeon() )
	{
		return ERR_INVALID_DATA;
	}

	if ( pCfgDungeon->type == DT_TEAM )
	{

	}
	else
	{
		if (m_trailer != NULL)
		{
			return GAME_SERVICE.replyfailure(m_cgindex, inPacket->getProc(), ERR_DUNGEON_TRAILER, getCid());
		}

		tm localNow = getLocalNow();
		// 日期判定
		if ( ( pCfgDungeon->weekday & ( 1 << GetWeekDay( localNow.tm_wday ) ) ) == 0 )
		{
			return ERR_INVALID_DATA;
		}

		// 时间判定
		if (localNow.tm_hour < pCfgDungeon->start_hour || localNow.tm_hour > pCfgDungeon->end_hour)
		{
			return ERR_INVALID_DATA;
		}

		// 等级判定
		if ( getLevel() < pCfgDungeon->level || getLevel() > pCfgDungeon->maxLevel )
		{
			GAME_SERVICE.replyfailure(m_cgindex, inPacket->getProc(), ERR_DUNGEON_PLAYER_LEVEL, getCid());
			return ERR_INVALID_DATA;
		}
	
		// 前置副本判定
		if ( pCfgDungeon->last_id > 0 )
		{
			if ( !m_extOperateLimit.CheckIsLimitedForever( PR_DUNGEON_COMPLETED_BEING + pCfgDungeon->last_id ) )
			{
				return ERR_INVALID_DATA;
			}
		}

		if ( pCfgDungeon->type == DT_UP_TOWER ) //爬塔副本只能往前
		{
			if ( m_extOperateLimit.CheckIsLimitedForever( PR_DUNGEON_COMPLETED_BEING + pCfgDungeon->id ) )
			{
				return ERR_INVALID_DATA;
			}
		}

		// 每日进入次数限制
		if (getRecord(pCfgDungeon->group_id) >= pCfgDungeon->daily_count)
		{
			GAME_SERVICE.replyfailure(m_cgindex, inPacket->getProc(), ERR_DUNGEON_DAILY_COUNT, getCid());
			return  ERR_INVALID_DATA;
		}
		if ( pCfgDungeon->type == DT_BOSS ) //boss副本每天从头开始
		{
			if ( pCfgDungeon->last_id > 0 )
			{
				CfgDungeon* pLastDungeon = CFG_DATA.getDungeon(pCfgDungeon->last_id);
				if (pLastDungeon == NULL)
				{
					return ERR_INVALID_DATA;
				}
				if ( getRecord(pLastDungeon->group_id) <= 0 )
				{
					return ERR_INVALID_DATA;
				}
			}
		}
		if ( pCfgDungeon->vip > GetPlayerVip().GetVipLevel() )
		{
			return ERR_SYETEM_ERR;
		}
		// 检测进入道具
		if ( pCfgDungeon->costItem.m_nCount > 0 )
		{
			if ( !m_extCharBag.HasItem( pCfgDungeon->costItem ) )
			{
				return ERR_INVALID_DATA;
			}
		}
		//铜钱
		if ( pCfgDungeon->costMoney > 0 )
		{
			if ( GetCurrency().GetMoneyBindAndNoBind() < pCfgDungeon->costMoney )
			{
				return ERR_SYETEM_ERR;
			}
		}
		//元宝
		if ( pCfgDungeon->costGold > 0 )
		{
			if ( GetCurrency( CURRENCY_GOLD ) < pCfgDungeon->costGold )
			{
				return ERR_SYETEM_ERR;
			}
		}

		Dungeon* pDungeon = MAP_MANAGER.NewDungeon( dungeonID );
		if (pDungeon == NULL)
		{
			GAME_SERVICE.replyfailure(m_cgindex, inPacket->getProc(), 50004,0);
			return  ERR_INVALID_DATA;
		}

		// 扣除铜钱
		if ( pCfgDungeon->costMoney > 0 )
		{
			if ( !GetCurrency().DecMoneyAndNoBind( pCfgDungeon->costMoney, MCR_ENTER_DUNGEON ) )
			{
				return ERR_SYETEM_ERR;
			}
		}
		// 扣除元宝
		if ( pCfgDungeon->costGold > 0 )
		{
			if ( !DecCurrency( CURRENCY_GOLD, pCfgDungeon->costGold, GCR_ENTER_DUNGEON, dungeonID ) )
			{
				return ERR_SYETEM_ERR;
			}
		}
		// 扣除进入道具
		if ( pCfgDungeon->costItem.m_nCount > 0 )
		{
			m_extCharBag.RemoveItem( pCfgDungeon->costItem, IDCR_DUNGEON );
		}

		pDungeon->start();
		setOldPosition();
		broadcastLeave();
		m_pMap->removePlayer(this, false);
		GAME_SERVICE.replySuccess(m_cgindex, inPacket->getProc(), dungeonID);	// 需要早于副本信息下发给客户端
		if ( pCfgDungeon->type != DT_BOSS )
		{
			updateRecord( pCfgDungeon->group_id,getRecord( pCfgDungeon->group_id ) + 1 );
			GetPlayerHuoYueDu().AddHuoYueDuRecord( HYDT_DUNGEON, pCfgDungeon->group_id );
			GetAchievemnet().AddAchievement( AT_DUNGEON, pCfgDungeon->group_id );
		}
		pDungeon->addPlayer( this, pCfgDungeon->x, pCfgDungeon->y );
		MAP_MANAGER.PostMsg( pDungeon->GetRunnerId(), GMC_ADD_DUNGEON, pDungeon );

		LogDungeon logDungeon = {};
		logDungeon.cid = getCid();
		logDungeon.did = dungeonID;
		logDungeon.type = pDungeon->getDungeonType();
		logDungeon.level = getLevel();
		logDungeon.start_time = pDungeon->GetStartTime();

		DB_SERVICE.insertDungeon( logDungeon );
	}	
	return ERR_OK;
}

int32_t Player::onLeaveDungeon(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}

	if ( !InDungeon() )
	{
		return ERR_INVALID_DATA;
	}

	Dungeon* pDungeon = dynamic_cast<Dungeon*>(m_pMap);
	if ( NULL == pDungeon )
	{
		return ERR_INVALID_DATA;
	}

	if ( pDungeon->getDungeonType() == DT_TD && !pDungeon->IsWin() )
	{
		pDungeon->onEndDungeon( this );
	}
	else
	{
		if ( !pDungeon->IsWin() )
		{
			LogDungeon logDungeon = {};
			logDungeon.cid = getCid();
			logDungeon.did = pDungeon->getDungeonId();
			logDungeon.start_time = pDungeon->GetStartTime();
			logDungeon.finish_time = getNow();
			logDungeon.state = 2;

			DB_SERVICE.updateDungeon( logDungeon );
		}

		leaveDungeon();	
	}
	return ERR_OK;
}

int32_t Player::onDungeonBuildTower(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}

	int32_t	nId		= inPacket->readInt32();
	int16_t	nPosX	= inPacket->readInt16();
	int16_t nPosY	= inPacket->readInt16();

	if ( m_pMap == NULL || !m_pMap->IsDungeonMap() )
	{
		return ERR_INVALID_DATA;
	}

	Dungeon *pDungeon = dynamic_cast<Dungeon*>(m_pMap);
	if ( pDungeon == NULL )
	{
		return ERR_INVALID_DATA;
	}

	if ( pDungeon->getDungeonType() != DT_TD )
	{
		return ERR_INVALID_DATA;
	}

	if ( pDungeon->getState() != DS_RUNNING && pDungeon->getState() != DS_WAIT )
	{
		return ERR_INVALID_DATA;
	}

	if ( !pDungeon->BuildTower( this, nId, nPosX, nPosY ) )
	{
		return ERR_INVALID_DATA;
	}

	GAME_SERVICE.replySuccess( m_cgindex, inPacket->getProc(), nId );
	return ERR_OK;
}

int32_t Player::onDungeonBuyTower(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}

	int32_t	nId		= inPacket->readInt32();
	if ( m_pMap == NULL || !m_pMap->IsDungeonMap() )
	{
		return ERR_INVALID_DATA;
	}

	Dungeon *pDungeon = dynamic_cast<Dungeon*>(m_pMap);
	if ( pDungeon == NULL )
	{
		return ERR_INVALID_DATA;
	}

	if ( pDungeon->getDungeonType() != DT_TD )
	{
		return ERR_INVALID_DATA;
	}

	if ( pDungeon->getState() != DS_RUNNING && pDungeon->getState() != DS_WAIT )
	{
		return ERR_INVALID_DATA;
	}

	if ( !pDungeon->BuyTower( this, nId ) )
	{
		return ERR_INVALID_DATA;
	}

	GAME_SERVICE.replySuccess( m_cgindex, inPacket->getProc(), nId );
	return ERR_OK;
}

int32_t Player::onDungeonStart(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}

	if ( m_pMap == NULL || !m_pMap->IsDungeonMap() )
	{
		return ERR_INVALID_DATA;
	}

	Dungeon *pDungeon = dynamic_cast<Dungeon*>(m_pMap);
	if ( pDungeon == NULL )
	{
		return ERR_INVALID_DATA;
	}

	if ( pDungeon->getState() != DS_WAIT )
	{
		return ERR_INVALID_DATA;
	}

	pDungeon->start();
	GAME_SERVICE.replySuccess( m_cgindex,inPacket->getProc() );
	return ERR_OK;
}

int32_t Player::onDungeonSelectReward(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t index = inPacket->readInt32();
	if (m_pMap == NULL || !m_pMap->IsDungeonMap() )
	{
		return ERR_INVALID_DATA;
	}

	Dungeon *pDungeon = dynamic_cast<Dungeon*>(m_pMap);
	if ( pDungeon == NULL || !pDungeon->IsWin() )
	{
		return ERR_INVALID_DATA;
	}

	//CfgDungeonReward* pReward = CFG_DATA.getDungeonReward(pDungeon->getDungeonId());
	//if (pReward == NULL)
	//{
	//	return ERR_INVALID_DATA;
	//}

	//if ( index == 2 )
	//{
	//	// 扣元宝
	//	if ( !DecCurrency( CURRENCY_GOLD, DUNGEON_DOUBLE_REWARD_COST, GCR_DUNGEON_DOUBLE_REWARD, pDungeon->getDungeonId() ) )
	//	{
	//		return ERR_INVALID_DATA;
	//	}
	//}
	//else if ( index != 1 )
	//{
	//	return ERR_INVALID_DATA;
	//}

	//MemChrBagVector*pItems = pDungeon->getGiftReward(index);
	//if (pItems == NULL|| pItems->empty())
	//{
	//	return ERR_INVALID_DATA;
	//}

	//if (pDungeon->getDungeonType() == DT_TEAM)
	//{
	//	if (!pDungeon->updatePlayerGiftInfo(m_chr.cid))
	//	{
	//		return ERR_INVALID_DATA;
	//	}
	//}

	//BenefitType bnfType = benefitType();
	//double bnfRatio = benefitRatio();
	//MemChrBagVector items;
	//if (bnfType == BT_NORMAL)
	//{
	//	items.assign(pItems->begin(), pItems->end());
	//}
	//else
	//{
	//	for (MemChrBagVector::iterator it = pItems->begin(); it != pItems->end(); ++it)
	//	{
	//		MemChrBag item = *it;
	//		item.itemCount = static_cast<int32_t>(item.itemCount * bnfRatio);
	//		items.push_back(item);
	//	}
	//}
	//if (m_extCharBag.GetFreeSlotCount() < static_cast<int32_t>(items.size()))
	//{
	//	return GAME_SERVICE.replyfailure(m_cgindex, inPacket->getProc(), ERR_BAG_IS_FULL);
	//}
	//autoUseItem(items);
	//if (!m_extCharBag.AddItem( items, IACR_DUNGEON_REWARD ))
	//{
	//	return ERR_INVALID_DATA;
	//}

	//if ( !getDungeonReward( pReward, index ) )
	//{
	//	return ERR_INVALID_DATA;
	//}

	//if (pDungeon->getDungeonType() != DT_TEAM)
	//{
	//	pDungeon->clearGift();
	//}

	if ( !pDungeon->GetReward( this, index ) )
	{
		return ERR_INVALID_DATA;
	}

	return GAME_SERVICE.replySuccess(m_cgindex,inPacket->getProc());
}

int32_t Player::onSelectUnit(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	clearSelectedUnit();

	EntityId_t id = inPacket->readInt64();
	int32_t type = inPacket->readInt32();

	Unit *pUnit = GAME_SERVICE.getUnit( id, type, GetRunnerId() );
	if (pUnit != NULL && pUnit != this && pUnit->getMap() == getMap())
	{
		pUnit->addFans(m_cgindex);
		m_selectedUnit.id = id;
		m_selectedUnit.type = type;
	}

	return ERR_OK;
}

int32_t	Player::onDoUnitSkill(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	if (!isAlive())
	{
		return ERR_INVALID_DATA;
	}

	if ( IsInStall() )
	{
		return ERR_INVALID_DATA;
	}

	//if (m_throwedTick > getTick())
	//{
	//	return ERR_INVALID_DATA;
	//	
	//}
	int32_t skillid = inPacket->readInt32();
	int32_t Flag	= inPacket->readInt32();

	// 塔防副本不能攻击
	if ( m_pMap != NULL && m_pMap->IsDungeonMap() )
	{
		Dungeon* pDungeon = dynamic_cast<Dungeon*>( m_pMap );
		if ( pDungeon != NULL && pDungeon->getDungeonType() == DT_TD )
		{
			if ( skillid != DUNGEON_SKILL_ID_1 && skillid != DUNGEON_SKILL_ID_2 && skillid != DUNGEON_SKILL_ID_3)
			{
				return ERR_INVALID_DATA;
			}
			MemChrSkillVector::iterator itSkill = findSkill(skillid);
			if (itSkill == m_skills.end())
			{
				MemChrSkill charSkill;
				charSkill.id = skillid;
				charSkill.level = 1;
				charSkill.last_action_tick = 0;
				m_skills.push_back( charSkill );
			}
		}
	}

	if ( skillid != RACING_HD_SKILL_ID )	//赛马场技能
	{
		if ( HasBuffState( CObjState::OBS_BINGDONG ) || HasBuffState( CObjState::OBS_YUNXUAN ) )
		{
			return ERR_INVALID_DATA;
		}
	}


	CfgSkill *pCfgSkill = CFG_DATA.getSkill(skillid);
	if (pCfgSkill == NULL || pCfgSkill->type == SK_PASSIVE)
	{
		return ERR_INVALID_DATA;
	}

	if ( pCfgSkill->kind == SK_HORSERACING )
	{
		MemChrSkillVector::iterator itSkill = findSkill(skillid);
		if (itSkill == m_skills.end())
		{
			MemChrSkill charSkill;
			charSkill.id = skillid;
			charSkill.level = 1;
			charSkill.last_action_tick = 0;
			m_skills.push_back( charSkill );
		}
		CActivityMap* pMap = dynamic_cast<CActivityMap*>( m_pMap );
		if ( NULL == pMap )
		{
			return ERR_INVALID_DATA;
		}
		CHorseRacing* pActivity = dynamic_cast<CHorseRacing*>( pMap->GetActivity() );
		if ( NULL == pActivity || !pActivity->IsRuning() || pActivity->CanUsePet( pMap->GetId() ) )
		{
			return ERR_INVALID_DATA;
		}
	}
	else
	{
		CActivityMap* pMap = dynamic_cast<CActivityMap*>( m_pMap );
		if ( pMap != NULL )
		{
			CHorseRacing* pActivity = dynamic_cast<CHorseRacing*>( pMap->GetActivity() );
			if ( pActivity != NULL && pActivity->IsRuning() && !pActivity->CanUsePet( pMap->GetId() ) )
			{
				// 跑马活动不能施放跑马技能以外的技能
				return ERR_INVALID_DATA;
			}
		}
	}

	MemChrSkillVector::iterator itSkill = findSkill(skillid);
	if (itSkill == m_skills.end())
	{
		return ERR_INVALID_DATA;
	}

	if ( pCfgSkill->kind == SK_XP )
	{
		if ( m_extCharSkill.GetSelectedXP() != skillid )
		{
			return ERR_INVALID_DATA;
		}
		if ( !m_pMap->CanUseXP() )
		{
			return ERR_INVALID_DATA;
		}
	}

	//// 检测法力值消耗
	//if ( GetMP() < pCfgSkill->getMpCost(itSkill->level))
	//{
	//	return GAME_SERVICE.replyfailure(m_cgindex, inPacket->getProc(), ERR_NOT_ENOUGH_MP);
	//}

	//// 检测体力消耗
	//if ( GetPP() < pCfgSkill->angry_cost)
	//{
	//	return GAME_SERVICE.replyfailure(m_cgindex, inPacket->getProc(), ERR_NOT_ENOUGH_ANGRY);
	//}

	//// 检测CD
	//if (getTick() - itSkill->last_action_tick < pCfgSkill->cd)
	//{
	//	return GAME_SERVICE.replyfailure(m_cgindex, inPacket->getProc(), ERR_SKILL_CD);
	//}

	m_useSkill.CleanUp();
	m_useSkill.skillId = skillid;
	
	switch (pCfgSkill->range)
	{
	case SR_SELF:
		{
			//doSkillCost(pCfgSkill, itSkill);
			//Skill skill(itSkill->id, itSkill->level);
			//int32_t attackValue = 0;
			//skill.unitAction(*this, UnitHandle(getUnitId(), getType()), attackValue);
		}
		break;
	case SR_SOLO:
		{
			UnitHandle target;
			target.id = inPacket->readInt64();
			target.type = inPacket->readInt32();

			m_useSkill.targets.push_back( target );

			//Unit *pTarget = GAME_SERVICE.getUnit( target.id, target.type, GetRunnerId() );
			//if (pTarget == NULL || pTarget->getMap() != m_pMap)
			//{
			//	return GAME_SERVICE.replyfailure(m_cgindex, inPacket->getProc(), ERR_TARGET_NOT_EXIST);
			//}

			//// 检测技能是否可以对目标施放
			//if (!checkSkillTarget(pCfgSkill, pTarget))
			//{
			//	return ERR_INVALID_DATA;
			//}

			//Position srcPos;	// 人物最终位置
			//Position tarPos;	// 目标最终位置
			if ( pCfgSkill->kind == SK_PUSH )
			{
				//srcPos.x = inPacket->readInt16();
				//srcPos.y = inPacket->readInt16();
				//tarPos.x = inPacket->readInt16();
				//tarPos.y = inPacket->readInt16();
				m_useSkill.srcPos.x = inPacket->readInt16();
				m_useSkill.srcPos.y = inPacket->readInt16();
				m_useSkill.tarPos.x = inPacket->readInt16();
				m_useSkill.tarPos.y = inPacket->readInt16();
			}

			//doSkillCost(pCfgSkill, itSkill);
			//Skill skill(itSkill->id, itSkill->level);
			//int32_t attackValue = 0;
			//int32_t hpChange = skill.unitAction(*this, target, attackValue,srcPos,tarPos);

			//// 检测是否是伤害技能
			//if (!pCfgSkill->beneficial)
			//{
			//	onDamageEvent(target, attackValue);
			//	if (pTarget->getType() == ET_PLAYER)
			//	{
			//		Player *victm = GAME_SERVICE.getPlayer( pTarget->getUnitId(), GetRunnerId() );
			//		if (victm != NULL)
			//		{
			//			if ( !victm->GetFightChecker().IsBuleName() && victm->getPkValue() <= PK_VALUE_RED && getPkValue() <= PK_VALUE_RED )
			//			{
			//				m_extFightChecker.ChangeBuleName( true );
			//			}
			//			victm->onDamagedEvent(UnitHandle(getUnitId(), getType()), attackValue);
			//		}
			//	}
			//}
		}
		break;
	case SR_TEAM:
		{
			//// 小队
			//UnitVector targets;
			//targets.push_back(this);

			//CharIdList members;
			//m_extCharTeam.GetTeamMembers( members );
			//CharIdList::const_iterator iter = members.begin();
			//CharIdList::const_iterator eiter = members.end();
			//for ( ; iter != eiter; ++iter )
			//{
			//	CharId_t nCid = *iter;
			//	if ( nCid == getCid() )
			//	{
			//		continue;
			//	}
			//	Player *pMember = GAME_SERVICE.getPlayer( nCid, GetRunnerId() );
			//	if ( pMember != NULL && pMember->getMap() == getMap() && pMember->getCurrentTile().tileDistance( getCurrentTile() ) < 20 )
			//	{
			//		targets.push_back( pMember );
			//	}
			//}

			//doSkillCost(pCfgSkill, itSkill);
			//Skill skill(itSkill->id, itSkill->level);
			//Int32Int32Map attackValues;
			//skill.posAction(*this, getCurrentTile(), targets, attackValues);
		}
		break;
	default:
		{
			// 群体伤害
			//Position pos;
			//UnitVector targets;
			//UnitVector playerTargets;

			//pos.x = inPacket->readInt32();
			//pos.y = inPacket->readInt32();
			m_useSkill.srcPos.x = inPacket->readInt32();
			m_useSkill.srcPos.y = inPacket->readInt32();

			int32_t count = inPacket->readInt32();
			if (count >= 0 && count <= 100)
			{
				for (int32_t i = 0; i < count && i < pCfgSkill->target_num; ++i)
				{
					UnitHandle target;
					target.id = inPacket->readInt64();
					target.type = inPacket->readInt32();
					m_useSkill.targets.push_back( target );
					//Unit *pTarget = GAME_SERVICE.getUnit( target.id, target.type, GetRunnerId() );
					//if (pTarget != NULL && checkSkillTarget(pCfgSkill, pTarget) && pTarget->getMap() == getMap())
					//{
					//	targets.push_back(pTarget);
					//	if (pTarget->getType() == ET_PLAYER)
					//	{
					//		playerTargets.push_back(pTarget);
					//	}
					//}
				}
			}

			//doSkillCost(pCfgSkill, itSkill);//缺少对生命，法力消耗的支持
			//Skill skill(itSkill->id, itSkill->level);
			//Int32Int32Map attackValues;
			//skill.posAction(*this, pos, targets, attackValues);

			//if (!pCfgSkill->beneficial)
			//{
			//	for (UnitVector::iterator it = targets.begin(); it != targets.end(); ++it)
			//	{
			//		Unit *target = *it;
			//		onDamageEvent(UnitHandle(target->getUnitId(), target->getType()), attackValues[target->getUnitId()]);
			//	}
			//	if (playerTargets.size() > 0)
			//	{
			//		for (UnitVector::iterator it = playerTargets.begin(); it != playerTargets.end(); ++it)
			//		{
			//			Unit *target = *it;
			//			Player *victm = GAME_SERVICE.getPlayer( target->getUnitId(), GetRunnerId() );
			//			if (victm != NULL)
			//			{
			//				if ( !victm->GetFightChecker().IsBuleName() && victm->getPkValue() <= PK_VALUE_RED && getPkValue() <= PK_VALUE_RED )
			//				{
			//					m_extFightChecker.ChangeBuleName( true );
			//				}
			//				victm->onDamagedEvent(UnitHandle(getUnitId(), getType()), attackValues[target->getUnitId()]);
			//			}
			//		}
			//	}
			//}
		}
		break;
	}

	//SetStartProtect( 0 );
	//SetInFight();
	//m_task.updateTaskSkill(pCfgSkill->id);
	//setSyncStatusFlag();
	SetSkillFlag( Flag );
	return ERR_OK;
}

bool Player::checkUseSkill()
{
	int32_t skillid = m_useSkill.skillId;
	if ( skillid <= 0 )
	{
		return false;
	}

	if ( !isAlive() )
	{
		return true;
	}

	// 塔防副本不能攻击
	if ( m_pMap != NULL && m_pMap->IsDungeonMap() )
	{
		Dungeon* pDungeon = dynamic_cast<Dungeon*>( m_pMap );
		if ( pDungeon != NULL && pDungeon->getDungeonType() == DT_TD )
		{
			if ( skillid != DUNGEON_SKILL_ID_1 && skillid != DUNGEON_SKILL_ID_2 && skillid != DUNGEON_SKILL_ID_3 )
			{
				return true;
			}
		}
	}
	if ( skillid != RACING_HD_SKILL_ID )
	{
		if ( HasBuffState( CObjState::OBS_BINGDONG ) || HasBuffState( CObjState::OBS_YUNXUAN ) )
		{
			return true;
		}
	}

	if ( m_throwedTick > getTick() )
	{
		return true;
	}

	CfgSkill *pCfgSkill = CFG_DATA.getSkill(skillid);
	if (pCfgSkill == NULL || pCfgSkill->type == SK_PASSIVE)
	{
		return true;
	}

	MemChrSkillVector::iterator itSkill = findSkill(skillid);
	if (itSkill == m_skills.end())
	{
		return true;
	}

	//if ( pCfgSkill->kind == SK_XP )
	//{
	//	if ( m_extCharSkill.GetSelectedXP() != skillid )
	//	{
	//		return true;
	//	}
	//}

	// 检测CD
	if ( getTick() - itSkill->last_action_tick < pCfgSkill->cd )
	{
		return false;
	}

	// 检测法力值消耗
	if ( GetMP() < pCfgSkill->getMpCost(itSkill->level))
	{
		GAME_SERVICE.replyfailure( m_cgindex, CM_DO_UNIT_SKILL, ERR_NOT_ENOUGH_MP );
		return true;
	}

	// 检测体力消耗
	if ( GetPP() < pCfgSkill->angry_cost)
	{
		GAME_SERVICE.replyfailure(m_cgindex, CM_DO_UNIT_SKILL, ERR_NOT_ENOUGH_ANGRY);
		return true;
	}


	switch ( pCfgSkill->range )
	{
	case SR_SELF:
		{
			Skill skill(itSkill->id, itSkill->level);
			int32_t attackValue = 0;
			if ( skill.unitAction(*this, UnitHandle(getUnitId(), getType()), attackValue) < 0 )
			{
				return true;
			}
			doSkillCost(pCfgSkill, itSkill);
		}
		break;
	case SR_SOLO:
		{
			UnitHandle target = m_useSkill.targets.front();
			Unit *pTarget = GAME_SERVICE.getUnit( target.id, target.type, GetRunnerId() );
			if (pTarget == NULL || pTarget->getMap() != m_pMap)
			{
				GAME_SERVICE.replyfailure(m_cgindex, CM_DO_UNIT_SKILL, ERR_TARGET_NOT_EXIST);
				return true;
			}

			// 检测技能是否可以对目标施放
			if (!checkSkillTarget(pCfgSkill, pTarget))
			{
				return true;
			}

			Position srcPos	= m_useSkill.srcPos;	// 人物最终位置
			Position tarPos	= m_useSkill.tarPos;	// 目标最终位置

			Skill skill(itSkill->id, itSkill->level);
			int32_t attackValue = 0;
			if ( skill.unitAction(*this, target, attackValue,srcPos,tarPos) < 0 )
			{
				return true;
			}
			doSkillCost(pCfgSkill, itSkill);

			// 检测是否是伤害技能
			if (!pCfgSkill->beneficial)
			{
				onDamageEvent(target, attackValue);
				if (pTarget->getType() == ET_PLAYER)
				{
					Player *victm = dynamic_cast<Player*>( pTarget );
					if (victm != NULL)
					{
						if ( !victm->GetFightChecker().IsBuleName() && victm->getPkValue() <= PK_VALUE_RED && getPkValue() <= PK_VALUE_RED )
						{
							m_extFightChecker.ChangeBuleName( true );
						}
						victm->onDamagedEvent( getHandle(), attackValue);
					}
				}
			}
		}
		break;
	case SR_TEAM:
		{
			// 小队
			UnitVector targets;
			targets.push_back(this);

			CharIdList members;
			m_extCharTeam.GetTeamMembers( members );
			CharIdList::const_iterator iter = members.begin();
			CharIdList::const_iterator eiter = members.end();
			for ( ; iter != eiter; ++iter )
			{
				CharId_t nCid = *iter;
				if ( nCid == getCid() )
				{
					continue;
				}
				Player *pMember = GAME_SERVICE.getPlayer( nCid, GetRunnerId() );
				if ( pMember != NULL && pMember->getMap() == getMap() && pMember->getCurrentTile().tileDistance( getCurrentTile() ) < 20 )
				{
					targets.push_back( pMember );
				}
			}

			Skill skill(itSkill->id, itSkill->level);
			EntityIdInt32Map attackValues;
			skill.posAction(*this, getCurrentTile(), targets, attackValues);
			doSkillCost(pCfgSkill, itSkill);
		}
		break;
	default:
		{
			// 群体伤害
			Position pos = m_useSkill.srcPos;
			UnitVector targets;
			UnitVector playerTargets;

			UnitHandlerList::iterator iter = m_useSkill.targets.begin();
			UnitHandlerList::iterator eiter = m_useSkill.targets.end();
			for ( ; iter != eiter; ++iter )
			{
				UnitHandle target = *iter;
				Unit *pTarget = GAME_SERVICE.getUnit( target.id, target.type, GetRunnerId() );
				if (pTarget != NULL && checkSkillTarget(pCfgSkill, pTarget) && pTarget->getMap() == getMap())
				{
					targets.push_back(pTarget);
					if (pTarget->getType() == ET_PLAYER)
					{
						playerTargets.push_back(pTarget);
					}
				}
			}

			Skill skill(itSkill->id, itSkill->level);
			EntityIdInt32Map attackValues;
			skill.posAction(*this, pos, targets, attackValues);
			doSkillCost(pCfgSkill, itSkill);

			if (!pCfgSkill->beneficial)
			{
				for (UnitVector::iterator it = targets.begin(); it != targets.end(); ++it)
				{
					Unit *target = *it;
					onDamageEvent(UnitHandle(target->getUnitId(), target->getType()), attackValues[target->getUnitId()]);
				}
				if (playerTargets.size() > 0)
				{
					for (UnitVector::iterator it = playerTargets.begin(); it != playerTargets.end(); ++it)
					{
						Unit *target = *it;
						Player *victm = GAME_SERVICE.getPlayer( target->getUnitId(), GetRunnerId() );
						if (victm != NULL)
						{
							if ( !victm->GetFightChecker().IsBuleName() && victm->getPkValue() <= PK_VALUE_RED && getPkValue() <= PK_VALUE_RED )
							{
								m_extFightChecker.ChangeBuleName( true );
							}
							victm->onDamagedEvent(UnitHandle(getUnitId(), getType()), attackValues[target->getUnitId()]);
						}
					}
				}
			}
		}
		break;
	}
	AddMP( -1 * pCfgSkill->getMpCost(itSkill->level) );
	AddPP( -1 * pCfgSkill->angry_cost );
	SetStartProtect( 0 );
	SetInFight();
	m_task.updateTaskSkill(pCfgSkill->id);
	return true;
}

int32_t Player::onPickDropItem(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	if (m_pMap == NULL || !isAlive())
	{
		return ERR_INVALID_DATA;
	}

	EntityId_t dropid = inPacket->readInt64();

	CDropItemGroup *dropItemGroup = m_pMap->getDropItemGroup(dropid);
	if (dropItemGroup == NULL)
	{
		return ERR_INVALID_DATA;
	}

	int32_t err = dropItemGroup->pick(*this, dropid);
	if (err == ERR_OK)
	{
		GAME_SERVICE.replySuccess(m_cgindex,inPacket->getProc(),dropid);
	}

	return err;
}

int32_t Player::onUseTrap(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	if (m_pMap == NULL)
	{
		return ERR_INVALID_DATA;
	}

	EntityId_t id = inPacket->readInt64();

	Trap *trap = m_pMap->getTrap( id );
	if (trap == NULL)
	{
		return ERR_INVALID_DATA;
	}

	return trap->onUse(this);
}

int32_t Player::onSwitchPkMode(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t pkMode = inPacket->readInt32();
	if (getLevel() < PK_PROTECT_LEVEL)
	{
		return ERR_INVALID_DATA;
	}

	if ( pkMode == PK_MODE_TEAM )
	{
		if ( !IsInTeam() )
		{
			return ERR_INVALID_DATA;
		}
	}

	if ( pkMode == PK_MODE_LEGION )
	{
		if ( getFamilyId() <= 0 )
		{
			return ERR_INVALID_DATA;
		}
	}

	if ( m_pMap->getPkMode() == PK_MODE_FREE )
	{
		if ( pkMode > 0 && pkMode < PK_MODE_COUNT )
		{
			if ( pkMode != PK_MODE_PEACE )
			{
				m_chr.pk_time = 0;
			}
			setPkMode(pkMode, true);
			GAME_SERVICE.replySuccess( getGateIndex(), inPacket->getProc(), pkMode );
		}
	}
	return ERR_OK;
}

int32_t Player::onSafeRevive(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}

	if ( InDungeon() )
	{
		return ERR_INVALID_DATA;
	}

	safeRevive();

	return GAME_SERVICE.replySuccess(m_cgindex, inPacket->getProc(), 0);
}

void Player::SafeToPrison()
{

	Map *pTargetMap = MAP_MANAGER.GetMap( PRISON_MAP_ID );
	if (pTargetMap == m_pMap)
	{
		instantMove(PRISON_X, PRISON_X, IMR_TRANSFER);
	}
	else
	{
		switchMap(pTargetMap, PRISON_X, PRISON_X, true);
	}
}

int32_t Player::onSiteRevive(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	if (isAlive())
	{
		return ERR_INVALID_DATA;
	}

	if (!m_pMap->canRevive())
	{
		return GAME_SERVICE.replyfailure(m_cgindex, inPacket->getProc(), ERR_MAP_CANNOT_REVIVE);
	}

	if ( InActivity() )
	{
		if ( !m_pMap->OnSitRevive( this ) )
		{
			return ERR_INVALID_DATA;
		}
		m_extCharPet.ReviveAllPet();
	}
	else
	{
		if ( GetPlayerVip().CanSiteRevive() )
		{
			GetOperateLimit().AddLimitCount( RP_VIP_SITEREVIVE_TIMES, 1 );
		}
		else
		{
			Int32Vector vSlot;
			ItemData data = {};
			int32_t Slot = inPacket->readInt32();
			vSlot.push_back( Slot );
			data.m_nId		= ISI_FU_HUO_DAN;
			data.m_nClass	= IC_NORMAL;
			data.m_nCount	= 1;

			if ( !m_extCharBag.RemoveItem( vSlot, data, IDCR_SITE_REVIVE ) )
			{
				return ERR_INVALID_DATA;
			}
		}
	}
	SetStartProtect( getNow() );
	FillHP();
	FillMP();
	broadcastBasicData();
	//sendChrRecord();
	setNeedSyncSelf();

	setSyncStatusFlag();

	return GAME_SERVICE.replySuccess( m_cgindex, inPacket->getProc() );
}

int32_t Player::onStrongRevive(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	if (isAlive())
	{
		return ERR_INVALID_DATA;
	}

	FillHP();
	FillMP();
	//sendChrRecord();
	setNeedSyncSelf();
	setSyncStatusFlag();

	return ERR_OK;
}

int32_t Player::onQueryChrInfo(Answer::NetPacket *inPacket)
{
	setNeedSyncSelf();
	return ERR_OK;
}

int32_t Player::onUpgradeLevel(Answer::NetPacket *inPacket)
{
	if (upgradeLevel(false))
	{
		recalcAttr();

		FillHP();
		FillMP();

		sendSkillList();
		setNeedSyncSelf();
		setSyncStatusFlag();
		sendUpdateSocialPlayerInfo( PII_LEVEL, m_chr.level);
	}

	return ERR_OK;
}

int32_t Player::onPatchUseItem(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	if (!isAlive())
	{
		return ERR_INVALID_DATA;
	}

	int32_t slot = inPacket->readInt32();
	int32_t count = inPacket->readInt32();

	return m_extCharBag.patchUseItem( slot, count );
}

int32_t Player::OnUsemMultiItem( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}

	ItemData CostItem = {};
	CostItem.m_nClass = IC_NORMAL;
	CostItem.m_nId	  = inPacket->readInt32();
	CostItem.m_nCount = inPacket->readInt32();
	Int32Vector vSlot;
	queryBagInfo( inPacket,vSlot );
	if ( !GetBag().RemoveItem( vSlot, CostItem, IDCR_USE_MULTI_ITEM ) )
	{
		return ERR_SYETEM_ERR;
	}
	ITEM_EFFECT.effect( CostItem.m_nId, *this, *this,CostItem.m_nCount );
	GAME_SERVICE.replySuccess(getGateIndex(), CM_USE_MMULTI_ITEM, CostItem.m_nId);
	return ERR_OK;
}

int32_t Player::onUseItem( Answer::NetPacket *inPacket )
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	if (!isAlive())
	{
		return ERR_INVALID_DATA;
	}

	int32_t slot = inPacket->readInt32();
	int32_t type =  inPacket->readInt32();
	//PetId_t petId = inPacket->readInt64();

	MemChrBag slotData = m_extCharBag.GetSlotData( slot );
	if (slotData.itemCount <= 0 || slotData.itemClass != IC_NORMAL)
	{
		return ERR_INVALID_DATA;
	}
	
	CfgItem *cfgItem = CFG_DATA.getItem(slotData.itemId);
	if (cfgItem == NULL || cfgItem->level > getLevel())
	{
		return ERR_INVALID_DATA;
	}

	if (cfgItem->job != 0 && cfgItem->job != getJob())
	{
		return ERR_INVALID_DATA;
	}

	if (cfgItem->cd_group < 0 || cfgItem->cd_group >= ITEM_CD_GROUP_COUNT)
	{
		return ERR_INVALID_DATA;
	}
	// 特殊处理随机卷轴,和回城卷轴
	if ( slotData.itemId == ISI_BACK_CITY_JUAN )
	{
		return OnBackCity( slot );
	}

	if ( slotData.itemId == ISI_SUI_JI_JUAN )
	{
		return OnRandPos( slot );
	}
	int32_t nType = cfgItem->type;
	return m_extCharBag.useItem( slot, type );
}

int32_t Player::onQuerySkillList(Answer::NetPacket *inPacket)
{
	sendSkillList();

	return ERR_OK;
}

int32_t Player::onUpgradeSkillLevel(Answer::NetPacket *inPacket)
{
	int32_t skillid = inPacket->readInt32();
	int32_t BagSlot = inPacket->readInt32();
	return upgradeSkill( skillid, BagSlot );
}

int32_t Player::onQueryTaskList(Answer::NetPacket *inPacket)
{
	m_task.sendTaskList();
	return ERR_OK;
}

int32_t Player::onReceiveTask(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t tid = inPacket->readInt32();
	if (ERR_OK != m_task.receive( tid, TT_TRUNK ))
	{
		return  GAME_SERVICE.replyfailure(m_cgindex,inPacket->getProc(),ERR_TASK_FAIL,tid);
	}
	return ERR_OK;
}

int32_t Player::onSubmitTask(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t tid = inPacket->readInt32();
	if (ERR_OK != m_task.submit(tid))
	{
		return  GAME_SERVICE.replyfailure(m_cgindex,inPacket->getProc(),ERR_TASK_FAIL,tid);
	}
	return ERR_OK;
}

int32_t Player::onGiveUpTask(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t tid = inPacket->readInt32();

	return m_task.giveUp(tid);
}

int32_t Player::onSetTaskCanSubmit(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t tid = inPacket->readInt32();

	CfgTask *pCfgTask = CFG_DATA.getTask(tid);
	if (pCfgTask == NULL || pCfgTask->condition != TC_PROTECT_BY_CLIENT)
	{
		return ERR_INVALID_DATA;
	}

	setTaskCanSubmit(tid);

	return ERR_OK;
}

int32_t Player::onTalkWithNpc(Answer::NetPacket *inPacket)
{
	if (m_pMap == NULL || NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}

	EntityId_t npcid = inPacket->readInt64();

	Npc *npc = m_pMap->getNpc(npcid);
	if (npc == NULL || npc->getCurrentTile().tileDistance(getCurrentTile()) > 3)
	{
		return ERR_INVALID_DATA;
	}

	npc->onTalkByPlayer(this);

	return ERR_OK;
}

int32_t Player::onQuickDone(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t tid = inPacket->readInt32();
	int32_t type = inPacket->readInt32();

	if (type == TT_TRUNK )
	{
		return ERR_INVALID_DATA;
	}
	return ERR_INVALID_DATA;
}


int32_t Player::onTeleport(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t aid = inPacket->readInt32();

	return doTeleport(aid);
}

int32_t Player::onTeleportActivity(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t aid = inPacket->readInt32();

	return doTeleportActivity(aid);
}

int32_t Player::onAddAction(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t slot = inPacket->readInt32();
	int32_t id = inPacket->readInt32();
	int32_t type = inPacket->readInt32();

	if (slot < 1 || slot > ACTION_NUMBER-3)
	{
		return ERR_INVALID_DATA;
	}

	if (type == AT_ITEM)
	{
		if (!m_extCharBag.HasItem(id, IC_NORMAL, 1))
		{
			return ERR_INVALID_DATA;
		}
	}
	else if (type == AT_SKILL)
	{
		if (findSkill(id) == m_skills.end())
		{
			return ERR_INVALID_DATA;
		}
		if ( slot != 6 )
		{
			for ( int32_t i = 1; i < 6; i++ )
			{
				if ( m_actions[i].id == id &&  m_actions[i].type == type )
				{
					m_actions[i].id		= 0;
					m_actions[i].type	= AT_NONE;
				}
			}
		}
	}

	m_actions[slot].id = id;
	m_actions[slot].type = type;

	sendActionList();

	return ERR_OK;
}

int32_t Player::onRemoveAction(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t slot = inPacket->readInt32();

	if (slot < 1 || slot > ACTION_NUMBER-3)
	{
		return ERR_INVALID_DATA;
	}

	MemChrAction action = {};
	m_actions[slot] = action;

	sendActionList();

	return ERR_OK;
}

int32_t Player::onExchangeAction(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t oldslot = inPacket->readInt32();
	int32_t newslot = inPacket->readInt32();

	if (oldslot < 1 || oldslot > ACTION_NUMBER-3 || newslot < 1 || newslot > ACTION_NUMBER-3)
	{
		return ERR_INVALID_DATA;
	}

	std::swap(m_actions[oldslot], m_actions[newslot]);

	sendActionList();

	return ERR_OK;
}

int32_t Player::onSetAutoFight(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	string fight = inPacket->readUTF8(true);
	string pick = inPacket->readUTF8(true);

	snprintf(m_autoFight.fight, MAX_AUTO_FIGHT_CCH_LENGTH, fight.c_str());
	snprintf(m_autoFight.pick, MAX_AUTO_PICK_CCH_LENGTH, pick.c_str());

	sendAutoFight();

	return ERR_OK;
}

int32_t Player::onSetSystemSetting(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t  Key		= inPacket->readInt32();
	int32_t  Values		= inPacket->readInt32();
	m_SystemSetting[Key] = Values;
	if ( Key == SS_TEAM || Key == SS_FRIEND )
	{
		m_extCharTeam.UpdateTeamSetting();
	}
	return ERR_OK;
}

int32_t Player::onBuyBackChrShopItem(Answer::NetPacket *inPacket)
{
	int32_t index = inPacket->readInt32();
	int32_t itemID = inPacket->readInt32();
	int32_t type  = inPacket->readInt32();

	return   m_extCharBag.buyBackChrShopItem(index,itemID,type);
}

int32_t Player::onBuyTaskCount(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	//int32_t type = inPacket->readInt32();
	//int32_t goldCost = 0;
	//switch (type)
	//{
	//case  TT_CYCLE:
	//	if ( GetCurrency( CURRENCY_GOLD ) < 250)
	//	{
	//		return ERR_INVALID_DATA;
	//	}

	//	goldCost = 250;
	//	updateRecord(PR_BUY_CYCLE_TASK_COUNT,getRecord(PR_BUY_CYCLE_TASK_COUNT)+10);
	//	m_extTaskCycle.sendTaskCycle();
	//	DecCurrency( CURRENCY_GOLD, goldCost, GCR_BUY_CYCLE_TASK_COUNT );
	//	break;
	//case  TT_KINGDOM:
	//	if ( GetCurrency( CURRENCY_GOLD ) < 80)
	//	{
	//		return ERR_INVALID_DATA;
	//	}

	//	goldCost = 80;
	//	updateRecord(PR_BUY_KINGDOM_TASK_COUNT,getRecord(PR_BUY_KINGDOM_TASK_COUNT)+10);
	//	m_taskKingdom.sendTaskKingdom();
	//	DecCurrency( CURRENCY_GOLD, goldCost, GCR_BUY_FAMILY_TASK_COUNT );
	//	break;
	//case  TT_FACTION:
	//	if ( GetCurrency( CURRENCY_GOLD ) < 80)
	//	{
	//		return ERR_INVALID_DATA;
	//	}

	//	goldCost = 80;
	//	updateRecord(PR_BUY_FAMILY_TASK_COUNT,getRecord(PR_BUY_FAMILY_TASK_COUNT)+10);
	//	m_taskFamily.sendTaskFamily();
	//	DecCurrency( CURRENCY_GOLD, goldCost, GCR_BUY_KINGDOM_TASK_COUNT );
	//	break;
	//default:
	//	return ERR_INVALID_DATA;
	//	break;
	//}

	return ERR_OK;
}

int32_t Player::onBuyResource(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t countType = inPacket->readInt32();
	int32_t attrType = inPacket->readInt32();

// 	int32_t value = CFG_DATA.getDailyReward(attrType,getLevel());
// 
// 	int32_t cost = 0;
// 	if (value <= 0)
// 	{
// 		return ERR_INVALID_DATA;
// 	}
// 
// 	MemChrBagVector slotDataVector;
// 	if (countType == 1)
// 	{
// 		cost = 1;
// 	}
// 	else if (countType == 2)
// 	{
// 		cost = 20;
// 		value = value*20;
// 	}
// 	else
// 	{
// 		return ERR_INVALID_DATA;
// 	}
// 
// 	Int32Vector vSlot;
// 	queryBagInfo( inPacket, vSlot );
// 	ItemCostData data = {};
// 	data.m_nClass	= IC_NORMAL;
// 	data.m_nCount	= cost;
// 	switch(attrType)
// 	{
// 	case 1:
// 		data.m_nType	= IET_JIANGXINGFU;
// 		if ( !m_extCharBag.RemoveItem( vSlot, data, IDCR_BUY_JIANGXING ) )
// 		{
// 			return ERR_INVALID_DATA;
// 		}
// 
// 		AddCurrency( CURRENCY_HONOR, value, MCR_BUY );
// 		break;
// 	case 2:
// 		data.m_nType	= IET_JUNGONGFU;
// 		if ( !m_extCharBag.RemoveItem( vSlot, data, IDCR_BUY_JUNGONG ) )
// 		{
// 			return ERR_INVALID_DATA;
// 		}
// 
// 		AddCurrency( CURRENCY_VIGOUR, value, MCR_BUY );
// 		break;
// 	case 3:
// 		data.m_nType	= IET_WUHUENFU;
// 		if ( !m_extCharBag.RemoveItem( vSlot, data, IDCR_BUY_WUHUEN ) )
// 		{
// 			return ERR_INVALID_DATA;
// 		}
// 
// 		AddCurrency( CURRENCY_KARMA, value, MCR_BUY );
// 		break;
// 	case 4:
// 		data.m_nType	= IET_TONGQIANFU;
// 		if ( !m_extCharBag.RemoveItem( vSlot, data, IDCR_BUY_MONEY ) )
// 		{
// 			return ERR_INVALID_DATA;
// 		}
// 		
// 		AddCurrency( CURRENCY_MONEY, value, MCR_BUY );
// 		break;
// 	default:
// 		return ERR_INVALID_DATA;
// 	}

	return ERR_OK;
}

int32_t Player::onBuyChrShopItem(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t id = inPacket->readInt32();
	int32_t count = inPacket->readInt32();
	CfgChrShop* pCharShop = CFG_DATA.getChrShop( id );
	if ( NULL == pCharShop )
	{
		return ERR_INVALID_DATA;
	}

	if (count <= 0)
	{
		return ERR_INVALID_DATA;
	}

	 int32_t err = m_extCharBag.buyChrShopItem( pCharShop,id,count );
	 if (err != ERR_OK)
	 {
		 	GAME_SERVICE.replyfailure(m_cgindex, inPacket->getProc(), err,id);
	 }
	 else
	 {
		 	GAME_SERVICE.replySuccess(m_cgindex, inPacket->getProc(), id);
	 }

	return err;
}

void Player::sendChrRecord()
{
	m_extOperateLimit.NotifyLimitInfo();
}

// int32_t Player::onQueryMonsterBroadcast(Answer::NetPacket *inPacket)
// {
// 	GAME_SERVICE.sendMonsterBroadcast(this);
// 	return ERR_OK;
// }

int32_t Player::onGamePublicChat(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}

	if (CFG_DATA.isBanChat(getUid(), getNow()))
 	{
 		return GAME_SERVICE.replyfailure(m_cgindex, inPacket->getProc(), ERR_GM_BAN_CHAT);
	}
	
	int32_t nBanChatTime = GM_BACKSTAGE.GetBanChatLeftTime( getCid() );
	if ( nBanChatTime > 0 )
	{
		return GAME_SERVICE.replyfailure(m_cgindex, inPacket->getProc(), ERR_GM_BAN_CHAT, nBanChatTime );
	}

	int32_t channel = inPacket->readInt32();

	if (channel == CC_WORLD)
	{
		if (getLevel() < 30)
		{
			return ERR_INVALID_DATA;
		}

		if (getNow() - getRecord(PR_WORLD_CHAT_LAST_TIME) < 3)
		{
			return ERR_INVALID_DATA;
		}
	}
	if ( channel == CC_COLOR_WORLD )
	{
		ItemData Item	= {};
		Item.m_nClass	= IC_NORMAL;
		Item.m_nCount	= 1;
		Item.m_nId		= ISI_XIAO_LA_BA;
		if ( !GetBag().RemoveItem( Item, IDCR_BAG_USE ) )
		{
			return ERR_INVALID_DATA;
		}
	}
	sendPublicChat(channel, inPacket);

	return ERR_OK;
}

int32_t Player::onUpdateFlyIconInt(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	updateRecord(PR_FLY_ICON_INT, inPacket->readInt32());
	return ERR_OK;
}

int32_t Player::onQueryKillerRankSelf(Answer::NetPacket *inPacket)
{
	sendKillerRankSelf();

	return ERR_OK;
}

int32_t Player::onBuyPvpState(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t count = inPacket->readInt32();
	if (count < 0)
	{
		return ERR_INVALID_DATA;
	}

	//Int32Vector vSlot;
	//queryBagInfo( inPacket, vSlot );
	//ItemCostData data = {};
	//data.m_nType	= IET_HUFU;
	//data.m_nCount	= count;
	//data.m_nClass	= IC_NORMAL;
	//if ( !m_extCharBag.RemoveItem( vSlot, data, IDCR_CLEAR_PK_VALUE ) )
	//{
	//	return ERR_INVALID_DATA;
	//}
	//addPkValue(-count * CLEAR_PK_VALUE_PER_ITEM);
	//setSyncStatusFlag();
	//setNeedSyncSelf();
	return ERR_OK;
}

int32_t Player::onTeleportByItem(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t targetkingdom = inPacket->readInt32();
	int32_t mapid = inPacket->readInt32();
	int32_t mapx = inPacket->readInt32();
	int32_t mapy = inPacket->readInt32();
	int32_t taskid = inPacket->readInt32();
	inPacket->readInt32();//无效的变量

	if ( IsInStall() )
	{
		return ERR_INVALID_DATA;
	}

	if ( InActivity())
	{
		return GAME_SERVICE.replyfailure(m_cgindex, CM_SWITCH_MAP, ERR_IS_BEING_ACTIVE);
	}
	Map *pTargetMap = pTargetMap = MAP_MANAGER.GetMap( mapid );
	if (pTargetMap == NULL)
	{
		return ERR_INVALID_DATA;
	}

	Position finalPos = pTargetMap->getRandomWalkableAroundPosition(mapx, mapy);
	if (finalPos.x < 0 || finalPos.y < 0)
	{
		return ERR_INVALID_DATA;
	}
	int32_t err = pTargetMap->canEnter(this);
	if ( ERR_OK != err )
	{
		return GAME_SERVICE.replyfailure(m_cgindex, CM_SWITCH_MAP, err, pTargetMap->GetId());;
	}
	
	if ( CFG_DATA.IsBossHomeMap( pTargetMap->GetId() ) || pTargetMap->IsActivityMap() || \
		 pTargetMap->IsDungeonMap() ||CFG_DATA.IsVipGuaJiMap( pTargetMap->GetId() ) )
	{
		TiShiInfo( NO_ENTER_ACTIVITY );
		return ERR_INVALID_DATA;
	}

	if ( !GetPlayerVip().CanFreeFlying() )
	{
		Int32Vector vSlot;
		ItemData data = {};
		queryBagInfo( inPacket, vSlot );
		if ( ERR_OK != verifyBagInfo( vSlot, ISI_TELEPORT, 1 ) )
		{
			return ERR_INVALID_DATA;
		}
		data.m_nId		= ISI_TELEPORT;
		data.m_nClass	= IC_NORMAL;
		data.m_nCount	= 1;

		if ( !m_extCharBag.RemoveItem( vSlot, data, IDCR_FLY ) )
		{
			return ERR_INVALID_DATA;
		}
	}

	if (pTargetMap == m_pMap)
	{
		instantMove(finalPos.x, finalPos.y, IMR_TRANSFER);
		
	}
	else
	{
		if (ERR_OK != switchMap(pTargetMap, finalPos.x, finalPos.y, true))
		{
			return ERR_INVALID_DATA;
		}
	}
	return GAME_SERVICE.replySuccess(m_cgindex,inPacket->getProc(),taskid);
}

int32_t Player::onClickPayButton(Answer::NetPacket *inPacket)
{
	++m_chr.pay_click_count;
	return ERR_OK;
}

int32_t Player::onMapEntered(Answer::NetPacket *inPacket)
{
	m_sysUser.map_enter_time = getNow();
	return ERR_OK;
}

int32_t Player::onDebugCmd(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	if (!CFG_DATA.getDebug())
	{
		return ERR_INVALID_DATA;
	}
	int32_t opcode = inPacket->readInt32();
	switch (opcode)
	{
	case 1: //add item
		{
			int32_t id = inPacket->readInt32();
			int32_t count = inPacket->readInt32();

			CfgItem *cfgItem = CFG_DATA.getItem(id);
			if (cfgItem != NULL && count > 0)
			{
				MemChrBag slotData = {};
				slotData.itemId = id;
				slotData.itemClass = IC_NORMAL;
				slotData.itemCount = count;
				if ( !autoUseItem(slotData) )
				{
					m_extCharBag.AddItem( slotData, IACR_NONE );
				}
			}
		}
		break;
	case 2: //add equip
		{
			int32_t id = inPacket->readInt32();
			int32_t count = inPacket->readInt32();

			MemChrBag slotData = {};
			slotData.itemId = id;
			slotData.itemClass = IC_EQUIP;
			slotData.itemCount = count;
			m_extCharBag.AddItem( slotData, IACR_NONE );
		}
		break;
	case 3: //add money
		{
			int32_t money = inPacket->readInt32();
			if (money > 0)
			{
				AddCurrency( CURRENCY_MONEY, money, GM_NONE, 9527 );
				AddCurrency( CURRENCY_GOLD, money, GM_NONE );
				AddCurrency( CURRENCY_CASH, money, GM_NONE );
				AddCurrency( CURRENCY_VIGOUR, money, GM_NONE );
				AddCurrency( CURRENCY_CHOUJINAG, money, GM_NONE );
				AddCurrency( CURRENCY_BOSS, money, GM_NONE );
				AddCurrency( CURRENCY_HONOR, money,GM_NONE );
				AddCurrency( CURRENCY_BIND_MONEY, money, GM_NONE );
			}
		}
		break;
	case 4: //move
		{
			if (m_pMap != NULL)
			{
				int32_t mapid = inPacket->readInt32();
				int32_t x = inPacket->readInt32();
				int32_t y = inPacket->readInt32();
				if (mapid != m_pMap->GetId())
				{
					switchMap( MAP_MANAGER.GetMap( mapid ), x, y, true );
				}
				else
				{
					instantMove(x, y, IMR_TRANSFER);
				}
			}
		}
		break;
	case 5: //exp
		{
			int64_t exp = inPacket->readInt64();
			if (exp > 0)
			{
				addExp( exp,false );
			}
		}
		break;
	case 6: //buff attr
		{
			int32_t index = inPacket->readInt32();
			int32_t addon = inPacket->readInt32();
			AddBuffAttrValue( static_cast<CObjAttrs::Index_T>( index ), addon);
			setNeedSyncSelf();
		}
		break;
	case 8: //task
		{
			int32_t trunkTaskId = inPacket->readInt32();
			m_task.cmdResetTrunkTask(trunkTaskId);
		}
		break;
	case 9:	// GM class id count
		{
			int32_t nClass	= inPacket->readInt32();
			int32_t nId		= inPacket->readInt32();
			int32_t nCount	= inPacket->readInt32();

			MemChrBag slotData	= {};
			slotData.itemClass	= nClass;
			slotData.itemId		= nId;
			slotData.itemCount	= nCount;
			m_extCharBag.AddItem( slotData, IACR_NONE );
		}
		break;
	case 10:// PetEgg
		{
			int32_t nEggId = inPacket->readInt32();
			std::list<int32_t> Eggs;
			Eggs.push_back( nEggId );
			m_extCharPet.AddEggs( Eggs );
		}
		break;
	case 11:// mail 给自己发邮件
		{	
			int32_t MailId = inPacket->readInt32();
			DB_SERVICE.OnSendSysMail( getCid(), MailId );
		}
		break;
	case 12://XP瞬间满
		{
			m_extCharSkill.FillXP();
		}
		break;
	case 13: //fabao法宝资源
		{
			int8_t	FaBaoResType = inPacket->readInt8();
			int32_t nCount		 = inPacket->readInt32();
			m_PlayerFaBao.AddFaBaoRes( static_cast<FaBaoResourceType>(FaBaoResType), nCount );
			break;
		}
	case 14: //qian钱
		{
			int8_t	Type		= inPacket->readInt8();
			int32_t	nCount		= inPacket->readInt32();
			if ( nCount > 0 )
			{
				AddCurrency( static_cast<CURRENCY_TYPE>(Type), nCount, GM_DEBUG_CMD, 9527 );
			}
			else
			{
				DecCurrency( static_cast<CURRENCY_TYPE>(Type), -1*nCount, GM_DEBUG_CMD, 9527 );
			}
			break;
		}
	case 15: //pp体力
		{
			AddPP( GetMaxPP() );
			break;
		}
	case 16:	// 学技能
		{
			for ( int32_t i = 0; i < 300; ++i )
			{
				CfgSkill *pCfgSkill = CFG_DATA.getSkill( i );
				if ( pCfgSkill != NULL && pCfgSkill->job == getJob() )
				{
					MemChrSkillVector::iterator itSkill = findSkill( i );
					if ( itSkill == m_skills.end() )
					{
						MemChrSkill sk = {};
						sk.id = i;
						sk.level = 1;
						m_skills.push_back( sk );
					}
				}
			}
			sendSkillList();
			break;
		}
	case 17:	// 强制完成任务
		{
			int32_t nTaskId = inPacket->readInt32();
			m_task.ForceSubmit( nTaskId );
			break;
		}
	default:
		break;
	}

	return ERR_OK;
}

int32_t Player::onQueryPlayerInfo(Answer::NetPacket *inPacket)
{	  
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}

	CharId_t tcid = inPacket->readInt64();
	if (tcid < 0)
	{
		return ERR_INVALID_DATA;
	}
	Player *pOtherplayer = GAME_SERVICE.getPlayer( tcid, GetRunnerId() );
	if (pOtherplayer == NULL)
	{
		NetPacket *outPacket = GAME_SERVICE.popNetpacket(PACK_DISPATCH, IM_GAME_SOCIAL_QUERY_PLAYER_INFO);
		if (NULL == outPacket)
		{
			return ERR_INVALID_DATA;
		}
		outPacket->writeInt32(m_cgindex);
		outPacket->writeInt64(tcid);
		outPacket->setSize(outPacket->getWOffset());
		GAME_SERVICE.sendPacket(outPacket);
		return ERR_OK;
	}

	Answer::NetPacket *packet = pOtherplayer->getOtherQueryInfo();
	if (NULL == packet)
	{
		return ERR_INVALID_DATA;
	}
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
	return ERR_OK;	
}


bool Player::isLeader() const
{
	return (m_chr.leader ==getCid());
}

int32_t Player::getLeaderCid() const
{
	return m_chr.leader;
}

int32_t Player::onSocialUseExchangeCode(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t item_id = inPacket->readInt32();
	std::string code = inPacket->readUTF8(true);
	
	MemChrBag item = {};
	item.itemId = item_id;
	item.itemClass = IC_NORMAL;
	item.itemCount = 1;
	item.bind	   = IBS_BIND;

	if (!m_extCharBag.AddItem( item, IACR_EXCHANGE_CODE ))
	{
		NetPacket *outPacket = GAME_SERVICE.popNetpacket(PACK_DISPATCH, IM_GAME_SOCIAL_USE_EXCHANGE_CODE);
		if (NULL == outPacket)
		{
			return ERR_INVALID_DATA;
		}
		outPacket->writeInt32(m_cgindex);
		outPacket->writeInt32(0);
		outPacket->writeUTF8("");
		outPacket->setSize(outPacket->getWOffset());
		GAME_SERVICE.sendPacket(outPacket);

		return GAME_SERVICE.replyfailure(m_cgindex,CM_USE_EXCHANGE_CODE,ERR_BAG_IS_FULL);
	}	

	NetPacket *outPacket = GAME_SERVICE.popNetpacket(PACK_DISPATCH, IM_GAME_SOCIAL_USE_EXCHANGE_CODE);
	if (NULL == outPacket)
	{
		return ERR_INVALID_DATA;
	}
	outPacket->writeInt32(m_cgindex);
	outPacket->writeInt32(1);
	outPacket->writeUTF8(code);
	outPacket->setSize(outPacket->getWOffset());
	GAME_SERVICE.sendPacket(outPacket);
	return ERR_OK;
}


//int32_t Player::onSocialChangeFamilyName(Answer::NetPacket *inPacket)
//{
//	// 帮派改名
//	return ERR_OK;
//}

void Player::sendLoginInfo()
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_LOGIN_INFO);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(m_chr.last_task_id);
	packet->writeInt32(m_sysUser.adult);
	packet->writeInt32(m_sysUser.total_online_time);
	packet->writeInt32(getRecord(PR_WORLD_CHAT_LAST_TIME));
	packet->writeInt32(CFG_DATA.getServerStartTime());
	packet->writeInt32(CFG_DATA.getServerStartDayTime());

	int32_t diff = CFG_DATA.getServerDiffTime();

	packet->writeInt32(diff); //距离开服几天

	packet->writeInt32(m_chr.create_time);

	packet->writeInt32(m_sysUserPreventWallow.isGrowUp);

	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendFightExpMoney(int32_t exp,int32_t kingdom_contribute)
{
	BenefitType bnfType = benefitType();
	if (bnfType == BT_NORMAL && exp <= 0 && kingdom_contribute <= 0)
	{
		return;
	}
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_FIGHT_EXP_MONEY);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(exp);
	//packet->writeInt32(money);
	packet->writeInt32(kingdom_contribute);
	//packet->writeInt32(skill_point);
	packet->writeInt32(bnfType);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

//void Player::sendChrKingdomInfo()
//{
//	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_CHR_KINGDOM_INFO);
//	if (NULL == packet)
//	{
//		return;
//	}
//	packet->writeInt32(0);
////	packet->writeInt32(m_chr.zhenlongzhiqi);
//	packet->writeInt16(0);					// 废弃//神兵
//	packet->writeInt32(0);					// 废弃//神兵
//	packet->writeInt16(0);					// 废弃//官职
//
//	packet->setSize(packet->getWOffset());
//	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
//}

void Player::sendChrInfo()
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_CHR_INFO);
	if ( NULL == packet )
	{
		return;
	}

	packet->writeInt64( getCid() );										// cid
	packet->writeUTF8( getName() );										// 名字
	packet->writeInt64( getFamilyId() );								// 帮派ID
	packet->writeUTF8( getFamilyName() );								// 帮派名字
	packet->writeInt8( getSex() );										// 性别
	packet->writeInt8( getJob() );										// 职业
	packet->writeInt16( getLevel() );									// 等级
	packet->writeInt64( getExp() );										// 当前经验
	packet->writeInt64( GetLevelExp() );								// 升级需求经验值
	packet->writeInt32( GetHP() );										// 当前血量
	packet->writeInt32( GetMP() );										//  当前法力值
	packet->writeInt32( GetPP() );										// 当前体力值
	packet->writeInt32( GetXP() );										// 当前XP值
	packet->writeInt32( GetMaxHP() );									// 最大血量
	packet->writeInt32( GetMaxMP() );									// 最大法力值
	packet->writeInt32( GetMaxPP() );									// 最大体力值
	packet->writeInt32( GetMaxXP() );									// 最大XP值
	packet->writeInt32( GetAttrValue( CObjAttrs::ATTR_PHY_ATK_MIN ) );	// 最小物理攻击		
	packet->writeInt32( GetAttrValue( CObjAttrs::ATTR_PHY_ATK_MAX ) );	// 最大物理攻击
	packet->writeInt32( GetAttrValue( CObjAttrs::ATTR_MAG_ATK_MIN ) );	// 最小魔法攻击
	packet->writeInt32( GetAttrValue( CObjAttrs::ATTR_MAG_ATK_MAX ) );	// 最大魔法攻击
	packet->writeInt32( GetAttrValue( CObjAttrs::ATTR_PHY_DEF ) );		// 物理防御
	packet->writeInt32( GetAttrValue( CObjAttrs::ATTR_MAG_DEF ) );		// 魔法防御
	packet->writeInt32( GetAttrValue( CObjAttrs::ATTR_HITRATE ) );		// 命中
	packet->writeInt32( GetAttrValue( CObjAttrs::ATTR_DODGE ) );		// 闪避
	packet->writeInt32( GetAttrValue( CObjAttrs::ATTR_CRITRATE ) );		// 暴击
	packet->writeInt32( getBattle() );									// 战斗力
	packet->writeInt32( getPkValue() );									// pk值
	packet->writeInt8( getPkMode() );									// pk模式
	packet->writeInt32( ( getPkProtectTime() - getNow()) > 0 ? ( getPkProtectTime() - getNow()) : 0  );				// pk保护时间
	packet->writeInt8( m_extFightChecker.IsBuleName()?1:0 );			// 蓝名
	packet->writeInt8( m_extOperateLimit.CheckIsLimitedForever( PR_FIRST_ENTER_GAME ) );	// 是否是第一次进入游戏
	packet->writeInt8( IsInStall() ? 1 : 0 );							// 摆摊状态
	//packet->writeInt8( 0 );												// head
	//packet->writeInt32( m_sysUser.gold_pay_total );						// 充值数
	//packet->writeInt32( m_sysUser.prepay_gold );						// 充值数
	//packet->writeInt8( getRecord(PR_FAV_GETTED_FLAG) );					// 收藏和保存桌面快捷方式奖励标记

	//packet->writeInt32( m_yellow_vip_level );							// 黄钻
	//packet->writeInt32( m_is_yellow_year_vip );							// 黄钻
	//packet->writeInt32( m_is_yellow_high_vip );							// 黄钻

	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendViewStarFinished(int32_t sid, int32_t level)
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_VIEW_STAR_FINISHED);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(sid);
	packet->writeInt32(level);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendSkillList()
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_SKILL_LIST);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(0);
	packet->writeInt32(static_cast<int32_t>(m_skills.size()));
	for (MemChrSkillVector::iterator it = m_skills.begin(); it != m_skills.end(); ++it)
	{
		packet->writeInt32(it->id);
		packet->writeInt32(it->level);
	}
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendNewSkill(int32_t skillid)
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_NEW_SKILL);
	if (NULL == packet)
	{
		return;
	}

	packet->writeInt32(skillid);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendActionList()
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_ACTION_LIST);
	if (NULL == packet)
	{
		return;
	}
	//packet->writeInt32(m_actions[ACTION_NUMBER-2].id);
	//packet->writeInt32(m_actions[ACTION_NUMBER-2].type);
	packet->writeInt32(m_actions[ACTION_NUMBER-1].id);
	packet->writeInt32(m_actions[ACTION_NUMBER-1].type);

	for (int32_t i = 1; i < ACTION_NUMBER-2; ++i)
	{
		packet->writeInt32(m_actions[i].id);
		packet->writeInt32(m_actions[i].type);
	}

	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendAutoFight()
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_AUTO_FIGHT);
	if (NULL == packet)
	{
		return;
	}
	packet->writeUTF8(m_autoFight.fight);
	packet->writeUTF8(m_autoFight.pick);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendSystemSetting()
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_SYSTEM_SETTING);
	if (NULL == packet)
	{
		return;
	}
	int32_t nSize = m_SystemSetting.size();
	packet->writeInt32( nSize );
	std::map<int32_t, int32_t>::iterator it = m_SystemSetting.begin();
	for ( ; it != m_SystemSetting.end(); ++it )
	{
		packet->writeInt32( it->first );
		packet->writeInt32( it->second );
	}
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::UpdateKilledByPlayer( CharId_t KillerId )
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_PROC, IM_GAME_SOCIAL_ADD_ENEMY);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32( getGateIndex() );
	packet->writeInt64( KillerId );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacket(packet);
}

void Player::DieResetXp()
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_PROC, IM_GAME_SOCIAL_RESET_XP);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32( getGateIndex() );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacket(packet);
}

void Player::sendKilledByPlayer(CharId_t cid, const std::string &name)
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_KILLED_BY_PLAYER);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(cid);
	packet->writeUTF8(name);
	packet->setSize(packet->getWOffset()); 
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}


void Player::sendToastInfo(CharId_t cid,CharId_t beCid)
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_BANQUET_TOAST);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(cid);
	packet->writeInt64(beCid);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendActivityState()
{
	//NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_ACTIVITY_STATE);
	//if (NULL == packet)
	//{
	//	return;
	//}
	//ACTIVITY_MANAGER.appendActivityState(packet);
	//packet->setSize(packet->getWOffset());
	//GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}


void Player::sendJungongChangeInfo(int32_t addon, BenefitType bnfType)
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_JUNGONG_CHANGE);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(addon);
	packet->writeInt32(bnfType);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendKingdomChangeInfo(int32_t addon)
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_KINGDOM_CONTRIBUTE_CHANGE);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(addon);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendjiangxingChangeInfo(int32_t addon, BenefitType bnfType)
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_JIANGCING_CHANGE);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(addon);
	packet->writeInt32(bnfType);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::sendWuhuenChangeInfo(int32_t addon, BenefitType bnfType)
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_WUHUEN_CHANGE);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(addon);
	packet->writeInt32(bnfType);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

//void Player::sendActivityData(int8_t kingdom,int32_t acitd,int8_t ntype,int32_t values,int32_t id,const char* fmName)
//{
//	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_ACTVIVTY_DATA);
//	if (NULL == packet)
//	{
//		return;
//	}
//	packet->writeInt64(getCid());
//	packet->writeInt64(getFamilyId());
//	packet->writeInt8(kingdom);
//	packet->writeInt32(acitd);
//	packet->writeInt8(ntype);
//	packet->writeInt32(values);
//	packet->writeInt32(id);
//	packet->writeUTF8(getName());
//	packet->writeUTF8(getFamilyName());
//	packet->writeUTF8(fmName);
//	packet->setSize(packet->getWOffset());
//	GAME_SERVICE.broadcast(packet);
//}

void Player::sendUpdateSocialPlayerInfo(PlayerInfoIndex index, int32_t value)
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_PROC, IM_SOCIAL_UPDATE_PLAYER_INFO);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(getCid());
	packet->writeInt32(index);
	packet->writeInt32(value);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacket(packet);
}

void Player::checkSaveToDB()
{
	if (getTick() - m_lastSaveTick >= m_saveDataTimeCount)//10 minutes
	{
		saveToDB(LR_SAVE_INTERVAL, 0);

		m_lastSaveTick = getTick();
	}
}

//void Player::sendViceGeneralDie(int32_t vid)
//{
//	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH,SM_VICE_GENERAL_DIE);
//	if (NULL == packet)
//	{
//		return;
//	}
//	packet->writeInt32(vid);
//	packet->setSize(packet->getWOffset());
//	GAME_SERVICE.sendPacketTo(m_cgindex,packet);
//}

//void Player::sendSelectKingdom()
//{
//	m_task.sendSelectKingdom();
//}

void Player::checkSyncStatus()
{
	if ( m_needSync || m_needSyncToTeam )
	{
		m_needSyncToTeam = false;
		m_extCharTeam.BroadcastBasicData();
	}

	if ( m_needSyncStatus && m_pMap != NULL )
	{
		NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_PLAYER_STATUS);
		if (NULL == packet)
		{
			return;
		}
		packet->writeInt64( getCid() );							// cid
		packet->writeInt8( getJob() );							// 职业
		packet->writeInt16( getLevel() );						// 等级
		packet->writeInt8( GetTeamStatus() );					// 组队状态
		packet->writeInt32( m_extCharPet.GetRidePet() );		// 坐骑ID
		packet->writeInt32( m_extCharPet.GetRidePetPoints() );  // 坐骑积分
		packet->writeInt16( GetMoveSpeed() );					// 移动速度
		packet->writeInt8( getAction() );						// 采集状态
		packet->writeInt32( getWeaponId() );					// 武器
		packet->writeInt32( getClothesId() );					// 衣服
		packet->writeInt8( m_PlayerVip.GetVipLevel() );			// VIP状态
		packet->writeInt32(0);									// VIP时间
		packet->writeInt16(0);									// TODO:称号
		packet->writeInt64(getFamilyId());						// 帮派ID
		packet->writeInt8(getFamilyPosition());					// 帮派职位
		packet->writeUTF8(getFamilyName());						// 帮派名称
		packet->writeInt8(m_chr.pk_mode);						// PK模式
		packet->writeInt32(m_chr.pk_value);						// PK值
		packet->writeInt32(( getPkProtectTime() - getNow()) > 0 ? ( getPkProtectTime() - getNow()) : 0 );					// PK保护时间
		packet->writeInt8( m_extFightChecker.IsBuleName()?1:0 );// 蓝名
		packet->writeInt32( GetProtectTime() );					// 复活保护时间
		packet->writeInt8( GetPlayerJueWei().GetJueWei() );		// 爵位
		packet->writeInt8( (int8_t)getRecord( RP_CUR_GUAN_WEI ) );//官位
		packet->writeInt8( m_extCharSkill.IsInXP() ? 1 : 0 );
		packet->setSize( packet->getWOffset() );
		m_pMap->broadcast(packet);

		m_needSyncStatus = false;
	}
	if ( m_needSyncSelf )
	{
		sendChrInfo();
		m_needSyncSelf = false;
	}
}

void Player::checkPool()
{
	if (m_pMap == NULL)
	{
		return;
	}
	int64_t CurTick = getTick();
	if (isAlive() && m_eventHP > 0 && m_pMap->IsDungeonMap() && GetHP() <= GetMaxHP()*m_eventHP*0.01f)
	{
		  Dungeon *pDungeon = dynamic_cast<Dungeon*>(m_pMap);
		  if (NULL == pDungeon)
		  {
			  return;
		  }
		  pDungeon->onPlayerHPEvent(this);
	}

	if ( m_chr.pk_time > 0 )
	{
		if ( getNow() - m_chr.pk_time >= 0)
		{
			m_chr.pk_time = 0;
			setSyncStatusFlag();
			setNeedSyncSelf();
		}
	}
	
	if (getPkValue() > 0)
	{
		if ( CurTick - m_lastPkValueTick >= SUB_PK_TIME)
		{
			addPkValue(-1);
			m_lastPkValueTick = getTick();
			setSyncStatusFlag();
			setNeedSyncSelf();
		}
	}
	if ( CurTick - m_lastPoolTick >= 1000 )
	{
		m_lastPoolTick = CurTick;
	}
	if (isAlive())
	{	
		if ( GetStartProtect() > 0 && getNow() - GetStartProtect() > 10   )
		{
			SetStartProtect( 0 );
		}

		if ( CurTick - m_LastAddPPTick > 5000 )
		{
			SetPPTick( CurTick );
			if ( !IsInFight() )
			{
				if ( isMoving() )
				{
					float Rate = static_cast<float>( GetPP() ) / static_cast<float>( GetMaxPP());
					if ( Rate <= 0.3f )
					{
						AddPP( ADD_PP_VALUES1 );
					}
					else if ( Rate <= 0.6f )
					{
						AddPP( ADD_PP_VALUES2 );
					}
					else
					{
						AddPP( ADD_PP_VALUES3 );
					}
				}
				else
				{
					float Rate = static_cast<float>( GetPP() ) / static_cast<float>( GetMaxPP() );
					if ( Rate <= 0.3f )
					{
						AddPP(  2 * ADD_PP_VALUES1 );
					}
					else if ( Rate <= 0.6f )
					{
						AddPP( 2 * ADD_PP_VALUES2 );
					}
					else
					{
						AddPP( 2 * ADD_PP_VALUES3 );
					}
				}
			}
		}
		if (CurTick - m_lastExpPoolTick > 5000)
		{
			m_lastExpPoolTick = CurTick;
			bool isAddExp = false;
			if (isInWorldBossActivity())
			{
				isAddExp = true;
			}
			else if ( InActivity() )
			{
				//ActivityMap* pActivityMap = static_cast<ActivityMap*>(getMap());
				//if (pActivityMap != NULL)
				//{
				//	CfgActivity* pActivity = pActivityMap->getCfgactivity();
				//	if (pActivity != NULL && pActivity->exp_add_interval > 0 && ACTIVITY_MANAGER.IsActivityRunning(pActivity->id))
				//	{
				//		isAddExp = true;
				//	}
				//}
			}
			if (isAddExp)
			{
				int32_t expAddon = static_cast<int32_t>(getLevel() * getLevel() * getLevel() / 100 );
				addExp((int64_t)expAddon);
				sendFightExpMoney(expAddon, 0);
			}
		}
	}
}

void Player::minuteCheck(bool bSend)
{
	int32_t nowTime = getNow();

	int32_t secondsPassed = nowTime - getRecord(PR_MINUTE_CHECK_TIME);
	if (secondsPassed > 60 )
	{
		updateRecord(PR_MINUTE_CHECK_TIME, nowTime);
	}

	tm localNow = getLocalNow();
	if (m_lastLocalNow.tm_mday != localNow.tm_mday)
	{
		onNewDayCome();
		KAI_FU_HUO_DONG.SendKaiFuHuoDongIcon( this );
	}

	if (m_lastLocalNow.tm_hour != 18 && localNow.tm_hour == 18 )
	{
		refeshKillerRecord();
	}
	m_lastLocalNow = localNow;
}

void Player::clearSelectedUnit()
{
	if (!m_selectedUnit.isEmpty())
	{
		Unit *pUnit = GAME_SERVICE.getUnit( m_selectedUnit.id, m_selectedUnit.type, GetRunnerId() );
		if (pUnit != NULL)
		{
			pUnit->removeFans(m_cgindex);
		}
	}
}

bool Player::autoUpgradeLevel()
{
	bool upgrade = false;

	while (upgradeLevel(true))
	{
		upgrade = true;
	}

	if (upgrade)
	{
		LevelUped();
	}

	return upgrade;
}

void Player::LevelUped()
{
	m_extCharInsidePet.OnLevelUp();	// 需要加在重新计算属性之前
	GetPlayerDailyActivity().OnLevelUp();
	m_PlayerFunctionOpen.CheckFunctionOpne( 0, getLevel() );
	m_PlayerTouZi.SendTouZiIcon();
	recalcAttr();
	KAI_FU_HUO_DONG.UpdateHDData( this,KFHDC_LEVEL_UP, getLevel() );
	FillHP();
	FillMP();

	sendSkillList();
	GetAchievemnet().AddAchievement( AT_LEVEL, m_chr.level );
	//m_extTaskCycle.LevelUp();
	//m_taskFamily.levelChange();
	//m_taskKingdom.levelChange();

	setSyncStatusFlag();

	sendUpdateSocialPlayerInfo(PII_LEVEL, m_chr.level);
}

bool Player::upgradeLevel(bool bAuto)
{
	CfgLevelExp* upgradeExp = CFG_DATA.getUpgradeExp(m_chr.level);
	if (upgradeExp == NULL)
	{
		return false;
	}

	int32_t maxLevel = GetMaxLevel();
	if (m_chr.exp >= upgradeExp->upgrade_exp && m_chr.level < maxLevel)
	{
		m_chr.level++;
		SetLvelStartTime();
		m_chr.exp -= upgradeExp->upgrade_exp;
		return true;
	}
	else
	{
		m_chr.exp =  m_chr.exp >= upgradeExp->max_exp?(upgradeExp->max_exp):(m_chr.exp);
		return false;
	}
}

void Player::SetLvelStartTime()
{
	m_chr.level_stay_time = 0;
	m_levelStartTime = getNow();
}

int32_t Player::doTeleport(int32_t aid)
{
	if (m_pMap == NULL)
	{
		return ERR_INVALID_DATA;
	}

	CfgNpcAirport* pCfgAirport = CFG_DATA.getNpcAirport(aid);
	if (pCfgAirport == NULL)
	{
		return ERR_INVALID_DATA;
	}

	//if (pCfgAirport->player_kingdom != 0 && pCfgAirport->player_kingdom != getKingdom())
	//{
	//	return ERR_INVALID_DATA;
	//}

	if ( GetCurrency().GetMoneyBindAndNoBind() < pCfgAirport->cost )
	{
		return ERR_INVALID_DATA;
	}

	//int32_t targetKingdomId = pCfgAirport->kingdom_id;
	//if (targetKingdomId == 0)
	//{
	//	targetKingdomId = GetRunnerId();
	//}
	//else if (targetKingdomId == KI_HOME)
	//{
	//	targetKingdomId = getKingdom();
	//}

	Map *pTargetMap = MAP_MANAGER.GetMap( pCfgAirport->map_id );
	if (pTargetMap == NULL)
	{
		return ERR_INVALID_DATA;
	}

	if (pTargetMap == m_pMap)
	{
		instantMove(pCfgAirport->map_x, pCfgAirport->map_y, IMR_TRANSFER);
		return ERR_OK;
	}
	else
	{
		return switchMap(pTargetMap, pCfgAirport->map_x, pCfgAirport->map_y, true);
	}
}

//通知进入活动地图
int32_t Player::doTeleportActivity(int32_t aid)
{
	//当前地图
	if (m_pMap == NULL)
	{
		return ERR_INVALID_DATA;
	}

	CfgActivity* activityInfo = CFG_DATA.getActivity(aid);
	if (activityInfo == NULL || activityInfo->target_mapid == 0)
	{
		return ERR_INVALID_DATA;
	}

	if ( !ACTIVITY_MANAGER.IsActivityRunning( activityInfo->id ) )
	{
		return ERR_INVALID_DATA;
	}

	int32_t  nReginId=-1;
	if(activityInfo->target_regiona.size() ==0) return ERR_INVALID_DATA;
	nReginId = getActivityBirthRegion(&activityInfo->target_regiona); //地图区域

	CfgMapRegion *pCfgRegion = CFG_DATA.getMapRegion( nReginId );
	if(pCfgRegion ==NULL)
	{
		return ERR_INVALID_DATA;
	}
		
	//目标地图
	Map *pTargetMap = MAP_MANAGER.GetMap( activityInfo->target_mapid );
	if ( pTargetMap == NULL )
	{
		return ERR_INVALID_DATA;
	}

	Position pos = pTargetMap->getRandomWalkablePositionInRegion( *pCfgRegion );
	if(pos.x ==-1 && pos.y ==-1) 
	{
		return ERR_INVALID_DATA;
	}

	if ( pTargetMap == m_pMap )
	{
		instantMove( pos.x, pos.y, IMR_ACTIVITY );
		return ERR_OK;
	}
	//查找地图
	else
	{
		return switchMap(pTargetMap, pos.x, pos.y, true);
	}
}

void Player::safeRevive()
{
	if (!isAlive())
	{
		FillHP();
		FillMP();
		SetStartProtect( getNow() );	
		broadcastBasicData();
		setNeedSyncSelf();
		setSyncStatusFlag();

		if ( InActivity() )
		{
			moveToReviveRegion( true );
			m_extCharPet.ReviveAllPet();
		}
		else
		{
			m_extCharPet.FightLastFightPet();
			if ( getPkValue() > PK_VALUE_RED )
			{
				SafeToPrison();
			}
			else 
			{
				moveToReviveRegion(false);
			}
		}		
	}
}

MemChrSkillVector::iterator Player::findSkill(int32_t skillid)
{
	for (MemChrSkillVector::iterator it = m_skills.begin(); it != m_skills.end(); ++it)
	{
		if (it->id == skillid)
		{
			return it;
		}
	}

	return m_skills.end();
}

bool Player::HasSkill( int32_t SkillId )
{
	for (MemChrSkillVector::iterator it = m_skills.begin(); it != m_skills.end(); ++it)
	{
		if (it->id == SkillId)
		{
			return true;
		}
	}

	return false;
}
void Player::addSkill(int32_t skillid, int32_t level)
{
	MemChrSkillVector::iterator it = findSkill(skillid);
	if (it != m_skills.end())
	{
		it->level = level;
	}
	else
	{
		MemChrSkill skill = {};
		skill.id = skillid;
		skill.level = level;
		m_skills.push_back(skill);
	}

}

void Player::removeSkill(int32_t skillid)
{
	MemChrSkillVector::iterator it = findSkill(skillid);
	if (it != m_skills.end())
	{
		m_skills.erase(it);
	}
}

int32_t Player::onGetEveryDayGold(Answer::NetPacket *inPacket)
{
// 	if (NULL == inPacket)
// 	{
// 		return ERR_INVALID_DATA;
// 	}
// 	tm tmNow = getLocalNow();
// 	if ((tmNow.tm_hour >= 12 && tmNow.tm_hour <= 13) && ((getRecord(RP_EVERY_DAY_GET_GOLD) & (1<<1)) ==0))
// 	{
// 		updateRecord(RP_EVERY_DAY_GET_GOLD,getRecord(RP_EVERY_DAY_GET_GOLD)+(1<<1));
// 	}
// 	else if ((tmNow.tm_hour >= 18 && tmNow.tm_hour <= 19) && ((getRecord(RP_EVERY_DAY_GET_GOLD) & (1<<2)) ==0))
// 	{
// 		updateRecord(RP_EVERY_DAY_GET_GOLD,getRecord(RP_EVERY_DAY_GET_GOLD)+(1<<2));
// 
// 	}
// 	else
// 	{
// 		return ERR_INVALID_DATA;
// 	}
// 
// 	AddCurrency( CURRENCY_CASH, static_cast<int32_t>(2000 * benefitRatio()), GCR_RECHARGE, benefitType());
// 	updatePayGold();
// 	sendGetGoldInfo();
// 	return GAME_SERVICE.replySuccess(m_cgindex,inPacket->getProc());
	return ERR_OK;
}

int32_t Player::onQueryEveryDayGoldInfo(Answer::NetPacket *inPacket)
{
	//sendGetGoldInfo();

	return ERR_OK;
}

int32_t Player::upgradeSkill( int32_t skillid, int32_t BagSlot )
{
	CfgSkill*pCfgSkill = CFG_DATA.getSkill(skillid);
	if (pCfgSkill == NULL ||  pCfgSkill->job != getJob() )
	{
		return ERR_INVALID_DATA; 
	}

	int32_t level  = 0;
	MemChrSkillVector::iterator it = findSkill(skillid);
	bool AddSkill = false;
	int32_t nBoolId = 0;
	if (it == m_skills.end())
	{  
		if (pCfgSkill->study_level > getLevel())
		{
			return GAME_SERVICE.replyfailure(m_cgindex,CM_UPGRADE_SKILL_LEVEL,ERR_LEVEL_TOO_LOW,skillid);
		}

		nBoolId  = CFG_DATA.GetSkillLevelUpTable().GetBook( skillid, level );
		AddSkill = true; 
	}
	else
	{
		level = it->level+1;
		if (pCfgSkill->maxLevel < level)
		{
			return GAME_SERVICE.replyfailure(m_cgindex,CM_UPGRADE_SKILL_LEVEL,ERR_SKILL_MAXLEVEL,skillid);
		}
		nBoolId = CFG_DATA.GetSkillLevelUpTable().GetBook( skillid, it->level );
	}
	if ( nBoolId <= 0 )
	{
		return ERR_INVALID_DATA;
	}
	const MemChrBag& bagItem = GetBag().GetSlotData( BagSlot );
	if ( GetBag().IsEmptySlot( bagItem ) )
	{
		return ERR_INVALID_DATA;
	}

	if ( bagItem.itemClass != IC_NORMAL || bagItem.itemId != nBoolId )
	{
		return ERR_INVALID_DATA;
	}
	CfgItem* pItem = CFG_DATA.getItem( bagItem.itemId );
	if ( NULL == pItem )
	{
		return ERR_INVALID_DATA;
	}

	if ( getLevel() < pItem->level )
	{
		return ERR_INVALID_DATA;
	}
	
	Int32Vector vSlot;
	vSlot.push_back( BagSlot );
	ItemData item = {};
	item.m_nClass	= IC_NORMAL;
	item.m_nId		= nBoolId;
	item.m_nCount   = 1;

	if ( !GetBag().RemoveItem( vSlot, item, IDCR_SKILL_LEVEL_UP ) )
	{
		return ERR_INVALID_DATA;
	}

	if (  AddSkill )
	{
		MemChrSkill memSkill = {};
		memSkill.id = skillid;
		memSkill.level = 1;

		if ( pCfgSkill->kind == SK_XP )
		{
			m_extCharSkill.LearnXPSkill();
		}
		m_skills.push_back(memSkill);
	}
	else
	{
		it->level++;
	}
	setNeedSyncSelf();
	setSyncStatusFlag();
	sendSkillList();
	return GAME_SERVICE.replySuccess(m_cgindex,CM_UPGRADE_SKILL_LEVEL,skillid);
}

void Player::doSkillCost(CfgSkill *pCfgSkill, MemChrSkillVector::iterator &itSkill)
{
	if (pCfgSkill == NULL || itSkill == m_skills.end())
	{
		return;
	}

	setStand();
	itSkill->last_action_tick = getTick();
	m_lastActionTick = getTick();
	GAME_SERVICE.replySuccess(m_cgindex, CM_DO_UNIT_SKILL, itSkill->id);		
}

void Player::onDamageEvent(UnitHandle target, int32_t attackValue)
{
	m_extCharPet.AddTarget( target );
	// 造成伤害
	if ( m_pMap != NULL && m_pMap->IsDungeonMap() )
	{
		Dungeon* pDungeon = dynamic_cast<Dungeon*>( m_pMap );
		if ( pDungeon != NULL )
		{
			pDungeon->AddPlayerDamage( getCid(), attackValue );
		}
	}
}

void Player::onDamagedEvent(UnitHandle launcher, int32_t attackValue)
{
	m_extCharPet.AddTarget( launcher );
	// 受到伤害
	SetInFight();
};

void Player::doSkillLevel(int32_t sid, int32_t slevel, UnitHandle target)
{
	CfgSkill* skillCfg = CFG_DATA.getSkill(sid);
	if (skillCfg == NULL)
	{
		return;
	}

	Skill skill(sid, slevel);
	switch(skillCfg->range)
	{
	case SR_SELF:
		{
			int32_t attackValue = 0;
			skill.unitAction(*this, UnitHandle(getUnitId(), getType()), attackValue);
		}
		break;
	case SR_SOLO:
		{
			int32_t attackValue = 0;
			skill.unitAction(*this, target, attackValue);
		}
	default:
		{
			Unit* pUnit = GAME_SERVICE.getUnit( target.id, target.type, GetRunnerId() );
			Position pos;
			if ( pUnit != NULL )
			{
				pos = pUnit->getCurrentTile();
			}
			if ( skillCfg->self == STP_SELF || skillCfg->self == STP_POS )
			{
				pos = getCurrentTile();
			}
			UnitVector targets = getMap()->getSkillTargets(*this, pos, skillCfg);

			EntityIdInt32Map attackValues;
			skill.posAction(*this, getCurrentTile(), targets, attackValues);
		}
	}
}

Trailer* Player::getTrailer()
{
	return m_trailer;
}


int32_t Player::getMemYellowStone(CharId_t cid , int32_t id)
{
	for (MemYellowStoneVector::iterator it = m_mysVector.begin();it != m_mysVector.end();++it)
	{
		if (it->cid == cid && it->id == id)
		{
			return 1;
		}
	}

	return 0;
}

int32_t Player::getYellowGrowId()
{
	int32_t id = 0;
	for (int i = 10;i < 18; i++)
	{
		if(getMemYellowStone(m_chr.cid,i) == 0)
		{
			id = i;
			break;
		}
		
	}
	
	return id;

}


void Player::setOpenId(std::string openId)
{
	m_openId = openId;
}

std::string Player::getOpenId()
{
	return m_openId;
}

void Player::setOpenKey(std::string openKey)
{
	m_openKey = openKey;
}

std::string Player::getOpenKey()
{
	return m_openKey;
}

void Player::setPf(std::string pf)
{
	m_pf = pf;
}

std::string Player::getPf()
{
	return m_pf;
}

void Player::setPfKey(std::string pfKey)
{
	m_pfKey = pfKey;
}

std::string Player::getPfKey()
{
	return m_pfKey;
}

int32_t Player::getIsYellowVip()
{
	return m_is_yellow_vip;
}

void Player::setIsYellowVip(int32_t isYellowVip)
{
	m_is_yellow_vip = isYellowVip;
}

int32_t Player::getIsYellowYearVip()
{
	return m_is_yellow_year_vip;
}

void Player::setIsYellowYearVip(int32_t isYellowYearVip)
{
	m_is_yellow_year_vip = isYellowYearVip;
}

int32_t Player::getYellowVipLevel()
{
	return m_yellow_vip_level;
}

void Player::setYellowVipLevel(int32_t yellowVipLevel)
{
	m_yellow_vip_level = yellowVipLevel;
}

int32_t Player::getIsYellowHighVip()
{
	return m_is_yellow_high_vip;
}

void Player::setIsYellowHighVip(int32_t isYellowHighVip)
{
	m_is_yellow_high_vip = isYellowHighVip;
}

int32_t Player::OnAskBossInfo( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	int8_t BossType = inPacket->readInt8();

	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_SEND_BOSS_INFO);
	if (NULL == packet)
	{
		return ERR_SYETEM_ERR;
	}
	WORLDBOSS.PacketBossInfo( packet, BossType );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo( m_cgindex, packet);
	return ERR_OK;
}

int32_t Player::OnRequestActivityInfo( Answer::NetPacket* inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_INVALID_DATA;
	}
	int32_t nActivityId = inPacket->readInt32();
	ACTIVITY_MANAGER.SendActivityInfo( this, nActivityId );
	return ERR_OK;
}

int32_t Player::OnRequestTerritoryWarInfo( Answer::NetPacket* inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_INVALID_DATA;
	}
	//int32_t nActivityId = inPacket->readInt32();
	ACTIVITY_MANAGER.OnTerritoryWarInfo( this );
	return ERR_OK;
}

int32_t Player::OnActivityGetDailyReward( Answer::NetPacket* inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_INVALID_DATA;
	}
	int32_t nActivityId = inPacket->readInt32();
	if ( ACTIVITY_MANAGER.GiveDailyReward( this, nActivityId ) != ERR_OK )
	{
		return ERR_INVALID_DATA;
	}

	GAME_SERVICE.replySuccess( m_cgindex, inPacket->getProc() );
	return ERR_OK;
}

void Player::TiShiInfo( int32_t TiShiId, int32_t Pos/* = 0*/ )
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_NOTIFY_TI_SHI_INFO);
	if ( NULL != packet )
	{
		packet->writeInt32( Pos );
		packet->writeInt32(TiShiId);
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.sendPacketTo(m_cgindex, packet);
	}
}

int64_t Player::GetCurrency( CURRENCY_TYPE const nType ) const
{
	return m_extCurrency.GetCurrency( nType );
}

bool Player::AddCurrency( CURRENCY_TYPE const nType, int64_t nVal, int32_t opWay, int64_t nParam )
{
	return m_extCurrency.AddCurrency( nType, nVal, opWay, nParam );
}

bool Player::DecCurrency( CURRENCY_TYPE const nType, int64_t nVal, int32_t opWay, int64_t nParam )
{
	return m_extCurrency.DecCurrency( nType, nVal, opWay, nParam );
}

bool Player::DecGoldAndCash( int64_t nVal, int32_t opWay, int32_t nParam )
{
	if ( !DecCurrency( CURRENCY_CASH, nVal * 5, opWay, nParam ) )
	{
		return DecCurrency( CURRENCY_GOLD, nVal, opWay, nParam );
	}
	return true;
}

int32_t	Player::GetProtectTime()
{
	int32_t Protecttime = m_StartProtect == 0  ? 0 : ( 10 - ( getNow() - m_StartProtect ) );
	return Protecttime;
}

int32_t	Player::GetStartProtect()
{
	return m_StartProtect;
}

void Player::SetStartProtect( int32_t ProtectTime )
{	
	if ( m_StartProtect == ProtectTime )
	{
		return;
	}
	m_StartProtect = ProtectTime;
	setSyncStatusFlag();
}

int32_t	Player::OnRandPos( int32_t Slot )
{
	if ( InDungeon() || InActivity() || IsInStall() )
	{
		return ERR_SYETEM_ERR;
	}
	Map * pMap = getMap();
	if ( NULL == pMap )
	{
		return ERR_SYETEM_ERR;
	}

	int32_t MaxX = pMap->GetWidthTileCount();
	int32_t MaxY = pMap->GetHeightTileCount();
	int32_t X	 = 0;
	int32_t Y	 = 0;
	for ( int32_t i = 0 ; i < 10; i++ )
	{
		int32_t RandX = RANDOM.generate( 1, MaxX);
		int32_t RandY = RANDOM.generate( 1, MaxY);
		if ( pMap->isWalkablePosition( RandX, RandY ) )
		{
			X	= RandX;
			Y	= RandY;
			break;
		}
	}
	if ( X == 0 || Y == 0 )
	{
		return ERR_SYETEM_ERR;
	}
	Int32Vector vSlot;
	vSlot.push_back(Slot);
	ItemData data = {};
	data.m_nId		= ISI_SUI_JI_JUAN;
	data.m_nClass	= IC_NORMAL;
	data.m_nCount	= 1;
	if ( !m_extCharBag.RemoveItem( vSlot, data, IDCR_SUI_JI ) )
	{
		return ERR_SYETEM_ERR;
	}
	instantMove( X, Y, IMR_TRANSFER );
	GetTask().updateTaskUseItem( ISI_SUI_JI_JUAN, 1 );
	return ERR_OK;
}

int32_t Player::OnBackCity( int32_t Slot )
{
	if ( InDungeon() || InActivity() || IsInStall() )
	{
		return ERR_SYETEM_ERR;
	}

	CfgMapRegion* pCfgMapRegion = CFG_DATA.getMapRegion(m_pMap->getReive(this));
	if (pCfgMapRegion == NULL)
	{
		return ERR_SYETEM_ERR;
	}
	CfgMap* pRegionMap = CFG_DATA.getMap(pCfgMapRegion->mapid);
	if (pRegionMap == NULL)
	{
		return ERR_SYETEM_ERR;
	}
	Map* pTargetMap = MAP_MANAGER.GetMap( pCfgMapRegion->mapid );
	if (pTargetMap == NULL)
	{
		return ERR_SYETEM_ERR;
	}
	Position pos = pTargetMap->getRandomWalkablePositionInRegion(*pCfgMapRegion);
	if (pos.x < 0 || pos.y < 0)
	{
		return ERR_SYETEM_ERR;
	}
	Int32Vector vSlot;
	vSlot.push_back( Slot );
	ItemData data = {};
	data.m_nId		= ISI_BACK_CITY_JUAN;
	data.m_nClass	= IC_NORMAL;
	data.m_nCount	= 1;
	if ( !m_extCharBag.RemoveItem( vSlot, data, IDCR_BACK_CITY ) )
	{
		return ERR_SYETEM_ERR;
	}
	if (pTargetMap == m_pMap)
	{
		instantMove(pos.x, pos.y, IMR_TRANSFER);
	}
	else
	{
		switchMap(pTargetMap, pos.x, pos.y, true);
	}
	GetTask().updateTaskUseItem( ISI_BACK_CITY_JUAN, 1 );
	return ERR_OK;
}

void Player::SendHDIcon()
{
	IconStateList IconList;
	tm localnow = getLocalNow();
	m_PlayerDailyActivity.GetDaWeiWangIcon( IconList );
	ACTIVITY_MANAGER.GetActicityIconState( IconList );
	m_PlayerDailyActivity.GetHuoDongDaTingIcon( IconList );
	m_PlayerDailyActivity.GetJiangLiDaTingIcon( IconList );
	m_PlayerYunYingHD.GetTeHuiIconState( IconList );
	m_PlayerYunYingHD.GetShouChongIconState( IconList );
	m_PlayerYunYingHD.GetEVeryDayChongZhiIconState( IconList );
	m_PlayerYunYingHD.GetThreePetGiftIconState( IconList );
	m_PlayerYunYingHD.GetTotalChongZhiIconState( IconList );
	m_PlayerTouZi.GetTouZiIconState( IconList );
	m_PlayerVip.GetVipGiftIconState( IconList );
	m_PlayerChouJiang.GetChouJiangStu( IconList );
	m_PlayerQiFu.GetChouJiangStu( IconList );
	m_WarVictory.GetWarVictoryIconState( IconList );
	KAI_FU_HUO_DONG.GetKaiFuHuoDongIcon( this, IconList );
	if ( m_PlayerFunctionOpen.IsOpened( FT_BOSS ) )
	{
		WORLDBOSS.GetWorldBossIcon( IconList );
	}
	VIP_GUA_JI_SINGLETON.GetIconState( IconList );
	m_extCharHallOfFame.GetIconState( IconList );
	m_ScoreShop.GetIconState( IconList );
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_ALL_ICON );
	if (NULL == packet)
	{
		return;
	}
	IconStateList::iterator it = IconList.begin();
	int32_t nSize = static_cast<int32_t>(IconList.size());
	packet->writeInt32( nSize );
	for ( ; it != IconList.end(); ++it )
	{
		packet->writeInt32( it->nId );
		packet->writeInt8( it->nState );
		packet->writeInt32( it->nLeftTime );
		packet->writeInt8( it->IconLeft );
		packet->writeInt32( it->IconRight );
		packet->writeInt8( it->Effects );
	}
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo( getGateIndex(), packet);
}

void Player::AddExpRate( int32_t AddValue )
{
	m_ExpRate += AddValue;
}

int32_t Player::GetExpRate()
{
	return m_ExpRate;
}

int32_t Player::onBeginGather(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	if (m_pMap == NULL )
	{
		BreakGather();
		return ERR_INVALID_DATA;
	}
	EntityId_t plantid = inPacket->readInt64();
	Plant *plant = m_pMap->getPlant(plantid);
	if (plant == NULL)
	{
		return ERR_INVALID_DATA;
	}
	if ( m_plantId != 0 || m_plantId != plantid ) //重新采集
	{
		BreakGather( false );
	}
	int32_t err = plant->onBeginGather(this);
	if (err == ERR_OK)
	{
		SetPlantId( plant->getEntityId() );
		setSyncStatusFlag();
	}
	return err;
}

int32_t Player::onEndGather(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	if ( m_pMap == NULL || m_plantId == 0 )
	{
		return ERR_INVALID_DATA;
	}
	EntityId_t plantid = inPacket->readInt64();
	if (plantid != m_plantId)
	{
		BreakGather( false );
		return ERR_INVALID_DATA;
	}

	Plant *plant = m_pMap->getPlant(plantid);
	if (plant == NULL)
	{
		BreakGather( false );
		return ERR_INVALID_DATA;
	}

	int32_t Err = plant->onEndGather(this);
	SetStartGather( 0 );
	SetPlantId( 0 );
	setSyncStatusFlag();
	return Err;
}

int32_t	Player::OnClickGame( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_INVALID_DATA;
	}
	char szSQL[MAX_SQL_LENGTH] = {};
	snprintf(szSQL, sizeof(szSQL)-1, "UPDATE `mem_character` SET `start_game`=4 WHERE `cid`=%lld", getCid());
	MySqlDBGuard db(DBPOOL);
	db.excute(szSQL);
	return ERR_OK;
}

void Player::sendEndGather(int32_t err)
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_END_GATHER);
	if(NULL == packet)
	{
		return;
	}
	packet->writeInt32(err);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_cgindex, packet);
}

void Player::SetPlantId(  EntityId_t PlantId )
{
	m_plantId  = PlantId;
}

void Player::SetStartGather( int64_t CurTick )
{
	m_startGatherTick = CurTick;
}

EntityId_t Player::GetPlantId()
{
	return m_plantId;
}

int64_t	Player::GetStartGather()
{
	return m_startGatherTick;
}

void Player::BreakGather( bool IsNotify )
{
	if ( GetPlantId() > 0 )
	{
		int32_t Err = 0;
		Plant *plant = m_pMap->getPlant( GetPlantId() );
		if (plant != NULL)
		{
			Err = plant->onBreakGather( this );
		}
		SetPlantId(0);
		SetStartGather(0);
		if ( IsNotify )
		{
			sendEndGather( Err );
		}
		setSyncStatusFlag();
	}
}

int32_t	Player::OnKaiFuHuoDongOperator( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	return KAI_FU_HUO_DONG.KaiFuHuoDongOperator( this, inPacket );
}

void Player::InitSysSetting()
{
	if ( m_systemSetting == "" )
	{
		return;
	}
	StringVector mineString = Answer::StringUtility::split( m_systemSetting, "|");
	StringVector::iterator it = mineString.begin();
	for ( ; it != mineString.end(); ++it )
	{
		StringVector StringVt = Answer::StringUtility::split( *it, ":");
		if ( StringVt.size() == 2 )
		{
			int32_t Key			= atoi( StringVt[0].c_str());
			int32_t Values		= atoi( StringVt[1].c_str());

			m_SystemSetting[Key] = Values;
		}
	}
}

std::string	Player::GetSysSetting()
{
	std::stringstream ss;
	std::map<int32_t, int32_t>::iterator  it = m_SystemSetting.begin();
	for ( ; it != m_SystemSetting.end(); ++it )
	{
		ss	<< it->first
			<< ":" 
			<< it->second
			<< "|" ;
	}
	return ss.str();
}

bool Player::GetSysSettingInfo( int32_t Index )
{
	return m_SystemSetting[Index] > 0;
}

void Player::SetPPTick( int64_t CurTick )
{
	m_LastAddPPTick = CurTick;
}

void Player::AddAppendAttr()
{
	if ( getFamilyId() > 0 && getFamilyId() == ACTIVITY_MANAGER.GetFamilyWarWinner() && getFamilyPosition() == FP_LEADER )
	{
		AddAttrList AddAttr = CFG_DATA.GetAppendAttr( FAMILY_WAR_WIN, getJob() );
		AddAttrList::iterator it = AddAttr.begin();
		for ( ; it != AddAttr.end(); ++it )
		{
			AddAttrValue(static_cast<CObjAttrs::Index_T>( it->m_nAddAttrType ), it->m_nAddAttrValue );
		}
	}
}

void Player::RecalcAttr()
{
	recalcAttr();
}

void Player::InitExtSystems()
{
	//成就系统要优先加载
	m_Achievement.Init( this );
	m_ExtSysMgr.Register( &m_Achievement );

	/*
	* EXT_INIT_WMF
	* 汪敏锋的新系统注册加在这之后
	*/
	m_extOperateLimit.Init( this );
	m_ExtSysMgr.Register( &m_extOperateLimit );

	m_extCurrency.Init( this );
	m_ExtSysMgr.Register( &m_extCurrency );

	m_extCharBag.Init( this );
	m_ExtSysMgr.Register( &m_extCharBag );

	m_extEquip.Init( this );
	m_ExtSysMgr.Register( &m_extEquip );

	m_extFightChecker.Init( this );
	m_ExtSysMgr.Register( &m_extFightChecker );

	m_extCharTeam.Init( this );
	m_ExtSysMgr.Register( &m_extCharTeam );

	m_extCharPet.Init( this );
	m_ExtSysMgr.Register( &m_extCharPet );

	m_extCharSkill.Init( this );
	m_ExtSysMgr.Register( &m_extCharSkill );

	m_extTaskCycle.Init( this );
	m_ExtSysMgr.Register( &m_extTaskCycle );

	m_extCharFamily.Init( this );
	m_ExtSysMgr.Register( &m_extCharFamily );

	m_extCharTeamDungeon.Init( this );
	m_ExtSysMgr.Register( &m_extCharTeamDungeon );

	m_extCharInsidePet.Init( this );
	m_ExtSysMgr.Register( &m_extCharInsidePet );

	m_extCharWorship.Init( this );
	m_ExtSysMgr.Register( &m_extCharWorship );

	m_extCharAuction.Init( this );
	m_ExtSysMgr.Register( &m_extCharAuction );

	m_extCharSoul.Init( this );
	m_ExtSysMgr.Register( &m_extCharSoul );

	m_extCharHallOfFame.Init( this );
	m_ExtSysMgr.Register( &m_extCharHallOfFame );

	/*
	* EXT_INIT_ZK
	* 朱开的新系统注册加在这之后
	*/

	m_KillMonsterTongJi.Init( this );
	m_ExtSysMgr.Register( &m_KillMonsterTongJi );

	m_PlayerDepot.Init( this );
	m_extCharTitle.Init( this );
	m_ExtSysMgr.Register( &m_PlayerDepot );
	m_ExtSysMgr.Register( &m_extCharTitle );

	m_PlayerTrade.Init( this );
	m_ExtSysMgr.Register( &m_PlayerTrade );

	m_PlayerMail.Init( this );
	m_ExtSysMgr.Register( &m_PlayerMail );

	m_FRiendExp.Init( this );
	m_ExtSysMgr.Register( &m_FRiendExp );

	m_PlayerFaBao.Init( this );
	m_ExtSysMgr.Register( &m_PlayerFaBao );

	m_Fighting.Init( this );
	m_ExtSysMgr.Register( &m_Fighting );

	m_PlayerJueWei.Init( this );
	m_ExtSysMgr.Register( &m_PlayerJueWei );

	m_PlayerShangCheng.Init( this );
	m_ExtSysMgr.Register( &m_PlayerShangCheng );

	m_PlayerChouJiang.Init( this );
	m_ExtSysMgr.Register( &m_PlayerChouJiang );

	m_PlayerQiFu.Init( this );
	m_ExtSysMgr.Register( &m_PlayerQiFu );

	m_PlayerDailyActivity.Init( this );
	m_ExtSysMgr.Register( &m_PlayerDailyActivity );

	m_PlayerVip.Init( this );
	m_ExtSysMgr.Register( &m_PlayerVip );

	m_ScoreShop.Init( this );
	m_ExtSysMgr.Register( &m_ScoreShop );

	m_PlayerFunctionOpen.Init( this );
	m_ExtSysMgr.Register( &m_PlayerFunctionOpen );

	m_PlayerExpBall.Init( this );
	m_ExtSysMgr.Register( &m_PlayerExpBall );

	m_PlayerYunYingHD.Init( this );
	m_ExtSysMgr.Register( &m_PlayerYunYingHD );

	m_PlayerTouZi.Init( this );
	m_ExtSysMgr.Register( &m_PlayerTouZi );

	m_levelRefining.Init( this );
	m_ExtSysMgr.Register( &m_levelRefining );

	m_PlayerHuoYueDu.Init( this );
	m_ExtSysMgr.Register( &m_PlayerHuoYueDu );
	
	m_PlayerGuanWei.Init( this );
	m_ExtSysMgr.Register( &m_PlayerGuanWei );

	m_WarVictory.Init( this );
	m_ExtSysMgr.Register( &m_WarVictory );
	/*
	* EXT_INIT_WXJ
	* 王鑫剑的新系统注册加在这之后
	*/
}

// City war functions
void Player::SendFamilyWarIcon()
{
	// TODO: Send family war icon packet to this player
	// This should notify the client about city war / family war state
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_FAMILY_WAR_ICON);
	if (NULL != packet)
	{
		packet->writeInt32(300); // action: family war icon
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.sendPacketTo(m_cgindex, packet);
	}
}

void Player::SetActState(int8_t ActState)
{
	// TODO: Set player activity state for city war
	// This tracks which families are participating in city war
	// For now, update the player's PK mode or activity flag
	// TODO: Store activity state for city war (would notify client)
	m_actState = ActState;
	LOG_INFO("Player::SetActState: cid=%lld familyId=%lld state=%d", getCid(), getFamilyId(), ActState);
}

int8_t Player::GetActState() const
{
	return m_actState;
}
