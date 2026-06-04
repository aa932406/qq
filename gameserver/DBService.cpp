#include "stdafx.h"

#include "DBService.h"
#include "GameService.h"
#include "EquipManager.h"
#include "PetManager.h"
#include "EquipRansom.h"
#include "Timer.h"
#include "ActivityManager.h"
#include "GMBackstage.h"
#include <string>

using namespace Answer;
using namespace std;

DBService::DBService()
	: TcpService(CLIENT_RECV_BUFFER, CLIENT_RECV_BUFFER, 16384-NET_PACKET_HEAD_LEN, 0, 2000, "DBService")
{

}

DBService::~DBService()
{

}

void DBService::onNetPacket(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return;
	}
	uint16_t proc = inPacket->getProc();
	switch (proc)
	{
	case IM_DB_PLAYER_LOADED:				onDBPlayerLoaded (inPacket); break;
	case IM_DB_PLAYER_SAVED:				onDBPlayerSaved(inPacket); break;
	case IM_DB_EQUIP_UPDATED:				onDBEquipUpdated(inPacket); break;
	case IM_DB_USER_PAYED:					onDBUserPayed(inPacket); break;
	case IM_DB_UPDATE_RANSOM:				OnUpdateRansom(inPacket);break;
	case IM_DB_PET_UPDATED:					onDBPetUpdated(inPacket); break;
	case IM_DB_PET_DELETED:					onDBPetDeleted(inPacket); break;
	case IM_DB_UPDATE_NEW_MAIL:				OnUpdateMailInfo( inPacket ); break;
	case IM_DB_UPDATE_FAMILY_WAR_RESULT:	onUpdateFamilyWarResult( inPacket ); break;
	case IM_DB_UPDATE_TERRITORY_WAR_RESULT:	onUpdateTerritoryWarResult( inPacket ); break;
	case IM_DB_UPDATE_CITY_WAR_RESULT:		onUpdateCityWarResult( inPacket ); break;
	case IM_DB_UPDATE_GM_BROADCAST:			onUpdateGMBroadcast( inPacket ); break;
	case IM_DB_UPDATE_GM_BAN_CHAT:			onUpdateGMBanChat( inPacket ); break;
	case IM_DB_UPDATE_GM_SEAL:				onUpdateGMSeal( inPacket ); break;
	default: break;
	}

	inPacket->destroy();
}

void DBService::loadPlayer( int16_t cgindex, int32_t uid, int32_t sid )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt16(cgindex);
	packet->writeInt32(uid);
	packet->writeInt32(sid);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOAD_PLAYER);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::UpdateLoginTime( CharId_t cid, int32_t login_time )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64( cid );
	packet->writeInt32( login_time );
	packet->setType(PACK_PROC);
	packet->setProc( IM_DB_UPDATE_LOGIN_TIME );
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::insertMemEquip(const MemEquip &equip)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(equip.id);
	packet->writeInt32(equip.base);
	packet->writeInt64(equip.owner);
	packet->writeInt32(equip.star);
	packet->writeInt32(equip.starLucky);
	packet->writeInt32(equip.addAttr);
	packet->writeInt32(equip.UpGradeLucky);
	packet->writeInt32(equip.UpQuality);
	packet->writeUTF8( equip.GetEquipGemString() );
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_INSERT_EQUIP);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::updateMemEquip(const MemEquip &equip)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(equip.id);
	packet->writeInt32(equip.nFlag);
	packet->writeInt32(equip.base);
	packet->writeInt64(equip.owner);
	packet->writeInt32(equip.star);
	packet->writeInt32(equip.starLucky);
	packet->writeInt32(equip.addAttr);
	packet->writeInt32(equip.UpGradeLucky);
	packet->writeInt32(equip.UpQuality);
	packet->writeUTF8(equip.GetEquipGemString());
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_UPDATE_EQUIP);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::deleteMemEquip( EquipId_t eid )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64( eid );
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_DELETE_EQUIP);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}


void DBService::insertMemPet( const MemPetDBData &pet )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	
	pet.PackageData( packet );
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_INSERT_PET);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::updateMemPet( const MemPetDBData &pet )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}

	pet.PackageData( packet );
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_UPDATE_PET);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::deleteMemPet( PetId_t nPetId )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64( nPetId );
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_DELETE_PET);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::insertTaskInfo( const LogTask &task )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}

	packet->writeInt64( task.cid );
	packet->writeInt32( task.tid );
	packet->writeInt32( task.type );
	packet->writeInt32( task.time );
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_TASK_RECEIVE);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::updateTaskInfo( const LogTask &task )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64( task.cid );
	packet->writeInt32( task.tid );
	packet->writeInt32( task.type );
	packet->writeInt32( task.time );
	packet->writeInt32( task.state );
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_TASK_SUBMIT);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::insertEquipEnhance(const LogEquipEnhance & equip)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(equip.cid);
	packet->writeUTF8(equip.name);
	packet->writeInt64(equip.equip_id);
	packet->writeInt32(equip.base_id);
	packet->writeInt32(equip.equip_quality);
	packet->writeInt32(equip.old_level);
	packet->writeInt32(equip.new_level);
	packet->writeInt32(equip.money_cost);
	packet->writeInt32(equip.sys_gold_cost);
	packet->writeInt32(equip.gold_cost);
	packet->writeInt32(equip.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_EQUIP_ENHANCE);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::insertEquipAddgem(const LogEquipAddGem & equipAddGem)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(equipAddGem.cid);
	packet->writeUTF8(equipAddGem.name);
	packet->writeUTF8(equipAddGem.equip_name);
	packet->writeInt32(equipAddGem.equip_quality);
	packet->writeUTF8(equipAddGem.gem_name);
	packet->writeInt32(equipAddGem.gem_level);
	packet->writeInt32(equipAddGem.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_EQUIP_ADD_GEM);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::insertItemCombine(const LogItemCombine &itemCombine)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(itemCombine.cid);
	packet->writeUTF8(itemCombine.name);
	packet->writeInt32(itemCombine.old_item_id);
	packet->writeInt32(itemCombine.old_item_num);
	packet->writeInt32(itemCombine.new_item_id);
	packet->writeInt32(itemCombine.new_item_num);
	packet->writeInt32(itemCombine.money_cost);
	packet->writeInt32(itemCombine.sys_gold_cost);
	packet->writeInt32(itemCombine.gold_cost);
	packet->writeInt32(itemCombine.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_ITEM_COMBINE);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::insertEquipSmithing(const LogEquipSmithing &equipSmithing)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(equipSmithing.cid);
	packet->writeUTF8(equipSmithing.name);
	packet->writeInt64(equipSmithing.equip_id);
	packet->writeInt32(equipSmithing.old_base_id);
	packet->writeInt32(equipSmithing.new_base_id);
	packet->writeInt32(equipSmithing.equip_quality);
	packet->writeInt32(equipSmithing.blueprint_id);
	packet->writeInt32(equipSmithing.blueprint_quality);
	packet->writeInt32(equipSmithing.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_EQUIP_SMITHING);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::insertEquipDismantle(const LogEquipDismantle &equipDismantle)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(equipDismantle.cid);
	packet->writeUTF8(equipDismantle.name);
	packet->writeInt64(equipDismantle.equip_id);
	packet->writeInt32(equipDismantle.base);
	packet->writeInt32(equipDismantle.enhance);
	packet->writeInt32(equipDismantle.item1_id);
	packet->writeInt32(equipDismantle.item1_count);
	packet->writeInt32(equipDismantle.item2_id);
	packet->writeInt32(equipDismantle.item2_count);
	packet->writeInt32(equipDismantle.item3_id);
	packet->writeInt32(equipDismantle.item3_count);
	packet->writeInt32(equipDismantle.item4_id);
	packet->writeInt32(equipDismantle.item4_count);
	packet->writeInt32(equipDismantle.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_EQUIP_DISMANTLE);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::insertDungeon( const LogDungeon &dungeon )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}

	packet->writeInt64(dungeon.cid);
	packet->writeInt32(dungeon.did);
	packet->writeInt32(dungeon.type);
	packet->writeInt32(dungeon.level);
	packet->writeInt32(dungeon.start_time);

	packet->setType( PACK_PROC );
	packet->setProc( IM_DB_LOG_IN_DUNGEON );
	packet->setSize(packet->getWOffset());
	sendPacket( packet );
}


void DBService::updateDungeon( const LogDungeon &dungeon )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}

	packet->writeInt64(dungeon.cid);
	packet->writeInt32(dungeon.did);
	packet->writeInt32(dungeon.state);
	packet->writeInt32(dungeon.start_time);
	packet->writeInt32(dungeon.finish_time);

	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_OUT_DUNGEON);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::InsertBossLog( const LogBoss& boss )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}

	packet->writeInt32( boss.mid );
	packet->writeInt32( boss.type );
	packet->writeInt64( boss.killer );
	packet->writeInt32( boss.time );

	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_KILL_BOSS);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::InsertActivityLog( const LogActivity& activity )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}

	packet->writeInt64( activity.cid );
	packet->writeInt32( activity.actid );
	packet->writeInt32( activity.acttype );
	packet->writeInt32( activity.time );
	packet->writeInt64( activity.param );

	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_ACTIVITY_DATA);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::InsertDailyGiftLog( const LogDailyGift& gift )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}

	packet->writeInt64( gift.cid );
	packet->writeInt32( gift.type );
	packet->writeInt32( gift.giftid );
	packet->writeInt32( gift.time );

	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_DAILY_GIFT);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::insertFamilyCreate(const LogFamily &family)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(family.cid);
	packet->writeUTF8(family.name);
	packet->writeUTF8(family.family_name);
	packet->writeInt32(family.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_CREATE_FAMILY);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::insertFamilyJoin(const LogFamily &family)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(family.cid);
	packet->writeUTF8(family.name);
	packet->writeUTF8(family.family_name);
	packet->writeInt32(family.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_JOIN_FAMILY);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::updateFamilyOut(const LogFamily &family)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(family.cid);
	packet->writeUTF8(family.name);
	packet->writeUTF8(family.family_name);
	packet->writeInt32(family.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_OUT_FAMILY);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::insertAchievement(const LogAchievement &achievement)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(achievement.cid);
	packet->writeUTF8(achievement.name);
	packet->writeInt32(achievement.id);
	packet->writeInt32(achievement.type);
	packet->writeInt32(achievement.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_ACHIEVEMENT);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::insertItemChange(const LogItemChange &item)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(item.cid);
	packet->writeUTF8(item.name);
	packet->writeInt8(item.flag);
	packet->writeInt32(item.reason);
	packet->writeInt32(item.id);
	packet->writeInt8(item.itemClass);
	packet->writeInt32(item.count);
	packet->writeInt32(item.time);
	packet->writeInt64(item.srcId);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_ITEM_CHANGE);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::insertMemYelloStone(const MemYellowStone &mys,int16_t type)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt16(type);
	packet->writeInt64(mys.cid);
	packet->writeInt32(mys.id);
	packet->writeInt32(mys.get_time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_INSERT_MEMYELLOWSTONE);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::logArtifactAddExp(const LogArtifactAddExp &artifactAddExp)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(artifactAddExp.cid);
	packet->writeUTF8(artifactAddExp.name);
	packet->writeInt32(artifactAddExp.itemid);
	packet->writeInt32(artifactAddExp.baseid);
	packet->writeInt32(artifactAddExp.old_exp);
	packet->writeInt32(artifactAddExp.old_level);
	packet->writeInt32(artifactAddExp.new_exp);
	packet->writeInt32(artifactAddExp.new_level);
	packet->writeInt32(artifactAddExp.stone_id);
	packet->writeInt32(artifactAddExp.stone_count);
	packet->writeInt32(artifactAddExp.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_ARTIFACT_ADD_EXP);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::logArtifactAddMaxLevel(const LogArtifactAddMaxLevel &artifactAddMaxLevel)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(artifactAddMaxLevel.cid);
	packet->writeUTF8(artifactAddMaxLevel.name);
	packet->writeInt32(artifactAddMaxLevel.itemid);
	packet->writeInt32(artifactAddMaxLevel.baseid);
	packet->writeInt32(artifactAddMaxLevel.old_max_level);
	packet->writeInt32(artifactAddMaxLevel.new_max_level);
	packet->writeInt32(artifactAddMaxLevel.stone_id);
	packet->writeInt32(artifactAddMaxLevel.stone_count);
	packet->writeInt32(artifactAddMaxLevel.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_ARTIFACT_ADD_MAX_LEVEL);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::logArtifactAddQuality(const LogArtifactAddQuality &artifactAddQuality)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(artifactAddQuality.cid);
	packet->writeUTF8(artifactAddQuality.name);
	packet->writeInt32(artifactAddQuality.itemid);
	packet->writeInt32(artifactAddQuality.baseid);
	packet->writeInt32(artifactAddQuality.old_quality);
	packet->writeInt32(artifactAddQuality.new_quality);
	packet->writeInt32(artifactAddQuality.stone_id);
	packet->writeInt32(artifactAddQuality.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_ARTIFACT_ADD_QUALITY);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}


void DBService::logEquipAddGemSlotNum(const LogEquipAddGemSlot &equipAddGemSlot)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(equipAddGemSlot.cid);
	packet->writeUTF8(equipAddGemSlot.name);
	packet->writeInt64(equipAddGemSlot.eid);
	packet->writeInt32(equipAddGemSlot.baseid);
	packet->writeInt32(equipAddGemSlot.old_gem_slot_num);
	packet->writeInt32(equipAddGemSlot.new_gem_slot_num);
	packet->writeInt32(equipAddGemSlot.stone_id);
	packet->writeInt32(equipAddGemSlot.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_EQUIP_ADD_GEM_SLOT);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::logEquipAppendStar(const LogEquipAppendStar &equipAppendStar)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(equipAppendStar.cid);
	packet->writeUTF8(equipAppendStar.name);
	packet->writeInt64(equipAppendStar.eid);
	packet->writeInt32(equipAppendStar.baseid);
	packet->writeInt32(equipAppendStar.level);
	packet->writeInt32(equipAppendStar.fuxing_stone_id);
	packet->writeInt32(equipAppendStar.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_EQUIP_APPEND_STAR);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::logEquipCombine(const LogEquipCombine &equipCombine)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
 	packet->writeInt64(equipCombine.cid);
	packet->writeUTF8(equipCombine.name);
	packet->writeInt64(equipCombine.eid);
	packet->writeInt32(equipCombine.baseid);
	packet->writeInt32(equipCombine.new_baseid);
	packet->writeInt32(equipCombine.side_equip_1);
	packet->writeInt32(equipCombine.side_equip_2);
	packet->writeInt32(equipCombine.side_equip_3);
	packet->writeInt32(equipCombine.side_equip_4);
	packet->writeInt32(equipCombine.shouhu_stone_id);
	packet->writeInt32(equipCombine.money);
	packet->writeInt32(equipCombine.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_EQUIP_COMBINE);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::logEquipForge(const LogEquipForge &equipForge)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(equipForge.cid);
	packet->writeUTF8(equipForge.name);
	packet->writeInt64(equipForge.eid);
	packet->writeInt32(equipForge.baseid);
	packet->writeInt32(equipForge.item_id);
	packet->writeInt32(equipForge.money);
	packet->writeInt32(equipForge.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_EQUIP_FORGE);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::logEquipRefreshBind(const LogEquipRefreshBind &equipRefreshBind)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(equipRefreshBind.cid);
	packet->writeUTF8(equipRefreshBind.name);
	packet->writeInt64(equipRefreshBind.eid);
	packet->writeInt32(equipRefreshBind.baseid);
	packet->writeInt32(equipRefreshBind.old_bind);
	packet->writeInt32(equipRefreshBind.new_bind);
	packet->writeInt32(equipRefreshBind.stone_id);
	packet->writeInt32(equipRefreshBind.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_EQUIP_REFRESH_BIND);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::logEquipRefreshElement(const LogEquipRefreshElement &equipRefreshElement)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(equipRefreshElement.cid);
	packet->writeUTF8(equipRefreshElement.name);
	packet->writeInt64(equipRefreshElement.eid);
	packet->writeInt32(equipRefreshElement.baseid);
	packet->writeInt32(equipRefreshElement.old_element);
	packet->writeInt32(equipRefreshElement.new_element);
	packet->writeInt32(equipRefreshElement.stone_id);
	packet->writeInt32(equipRefreshElement.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_EQUIP_REFRESH_ELEMENT);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::logEquipRefreshFloatAttr(const LogEquipRefreshFloatAttr &equipRefreshFloatAttr)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(equipRefreshFloatAttr.cid);
	packet->writeUTF8(equipRefreshFloatAttr.name);
	packet->writeInt64(equipRefreshFloatAttr.eid);
	packet->writeInt32(equipRefreshFloatAttr.baseid);
	packet->writeInt32(equipRefreshFloatAttr.old_float_attr);
	packet->writeInt32(equipRefreshFloatAttr.new_float_attr);
	packet->writeInt32(equipRefreshFloatAttr.stone_id);
	packet->writeInt32(equipRefreshFloatAttr.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_EQUIP_REFRESH_FLOAT_ATTR);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::logEquipRemoveGem(const LogEquipRemoveGem &equipRemoveGem)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(equipRemoveGem.cid);
	packet->writeUTF8(equipRemoveGem.name);
	packet->writeInt64(equipRemoveGem.eid);
	packet->writeInt32(equipRemoveGem.baseid);
	packet->writeInt32(equipRemoveGem.slot_index);
	packet->writeInt32(equipRemoveGem.stone_id);
	packet->writeInt32(equipRemoveGem.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_EQUIP_REMOVE_GEM);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::logEquipRemoveStar(const LogEquipRemoveStar &equipRemoveStar)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(equipRemoveStar.cid);
	packet->writeUTF8(equipRemoveStar.name);
	packet->writeInt64(equipRemoveStar.eid);
	packet->writeInt32(equipRemoveStar.baseid);
	packet->writeInt32(equipRemoveStar.level);
	packet->writeInt32(equipRemoveStar.fuxing_stone_id);
	packet->writeInt32(equipRemoveStar.shouhu_stone_id);
	packet->writeInt32(equipRemoveStar.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_EQUIP_REMOVE_STAR);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::logEquipStrengthen(const LogEquipStrengthen &equipStrengthen)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(equipStrengthen.cid);
	packet->writeUTF8(equipStrengthen.name);
	packet->writeInt64(equipStrengthen.eid);
	packet->writeInt32(equipStrengthen.baseid);
	packet->writeInt32(equipStrengthen.old_strength);
	packet->writeInt32(equipStrengthen.new_strength);
	packet->writeInt32(equipStrengthen.stone_id);
	packet->writeInt32(equipStrengthen.stone_count);
	packet->writeInt32(equipStrengthen.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_EQUIP_STRENGTHEN);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::logEquipUpgrade(const LogEquipUpgrade &equipUpgrade)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(equipUpgrade.cid);
	packet->writeUTF8(equipUpgrade.name);
	packet->writeInt64(equipUpgrade.eid);
	packet->writeInt32(equipUpgrade.baseid);
	packet->writeInt32(equipUpgrade.new_baseid);
	packet->writeInt32(equipUpgrade.upgrade_stone_id);
	packet->writeInt32(equipUpgrade.upgrade_stone_count);
	packet->writeInt32(equipUpgrade.crystal_stone_id);
	packet->writeInt32(equipUpgrade.zhenxing_stone_id);
	packet->writeInt32(equipUpgrade.money);
	packet->writeInt32(equipUpgrade.time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_EQUIP_UPGRADE);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::logPlayerLogin(CharId_t cid, int32_t action, int32_t time)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(cid);
	packet->writeInt32(action);
	packet->writeInt32(time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_PLAYER_LOGIN);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::logPlayerDeal(CharId_t cid, std::string name, int64_t tcid, std::string tname, int32_t reason, int32_t money, std::string items, int32_t time)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(cid);
	packet->writeUTF8(name.c_str());
	packet->writeInt64(tcid);
	packet->writeUTF8(tname.c_str());
	packet->writeInt32(reason);
	packet->writeInt32(money);
	packet->writeUTF8(items.c_str());
	packet->writeInt32(time);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_LOG_PLAYER_DEAL);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

//void DBService::logPk(int32_t actid, PKUSERSLIST& datas,int32_t time)
//{
//	NetPacket *packet = popNetpacket();
//	if (NULL == packet)
//	{
//		return;
//	}
//	int32_t nsize =datas.size();
//	packet->writeInt16((int16_t)actid);
//	packet->writeInt32(time);
//	packet->writeInt32(nsize);
//	PKUSERSLIST::iterator item =datas.begin();
//	for (;item!=datas.end();item++)
//	{
//		packet->writeInt32(item->cid);
//		packet->writeUTF8(item->name);
//		packet->writeInt32(item->winCount);
//		packet->writeInt32(item->lostCount);
//	}
//	
//	packet->setType(PACK_PROC);
//	packet->setProc(IM_DB_LOG_PK);
//	packet->setSize(packet->getWOffset());
//	sendPacket(packet);
//}

void DBService::logCurrency( CharId_t cid, const std::string& name, int32_t nType, int32_t opWay, int32_t nVal, int64_t nParam, int32_t nTime )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64( cid );
	packet->writeUTF8( name );
	packet->writeInt32( nType );
	packet->writeInt32( opWay );
	packet->writeInt32( nVal );
	packet->writeInt64( nParam );
	packet->writeInt32( nTime );
	packet->setType( PACK_PROC );
	packet->setProc( IM_DB_LOG_CURRENCY );
	packet->setSize( packet->getWOffset() );
	sendPacket(packet);
}

void DBService::onDBPlayerLoaded(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return;
	}
	PlayerDBData dbData;
	dbData.UnPackageData( inPacket );

	GAME_SERVICE.onPlayerLoaded( dbData );
}
//void DBService::onDBPlayerColth(Answer::NetPacket *inPacket)
//{
//	if (NULL == inPacket)
//	{
//		return;
//	}
//	int32_t cid = inPacket->readUInt32();
//	GAME_SERVICE.onPlayerClothChange(cid);
//}
void DBService::onDBPlayerSaved(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return;
	}
	CharId_t cid = inPacket->readUInt64();
	string name = inPacket->readUTF8(true);
	int32_t ticks = inPacket->readInt32();

	int32_t uid = inPacket->readInt32();
	int32_t sid = inPacket->readInt32();
	int32_t reason = inPacket->readInt32();
	int32_t param = inPacket->readInt32();

	LOG_DEBUG("use %d ticks to save player cid = %lld, name = %s\n", ticks, cid, name.c_str());

	GAME_SERVICE.onPlayerSaved(uid, sid, reason, param, cid);
}

void DBService::onDBEquipUpdated(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return;
	}
	MemEquipVector equips;
	int32_t count = inPacket->readInt32();
	equips.reserve(count);
	for (int32_t i = 0; i < count; ++i)
	{
		MemEquip equip;
		equip.id				= inPacket->readInt64();
		equip.nFlag				= inPacket->readInt32();
		equip.base				= inPacket->readInt32();
		equip.owner				= inPacket->readInt64();
		equip.star				= inPacket->readInt32();
		equip.starLucky			= inPacket->readInt32();
		equip.addAttr			= inPacket->readInt32();
		equip.UpGradeLucky		= inPacket->readInt32();
		equip.UpQuality			= inPacket->readInt32();
		std::string EquipString = inPacket->readUTF8(true);		
		equip.ParesEquipString( EquipString );
		equips.push_back(equip);
	}
	EQUIP_MANAGER.OnEquipUpdated(equips);
	
}

void DBService::onDBPetUpdated(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return;
	}
	MemPetDBData pet;
	pet.UnPackageData( inPacket );
	PET_MANAGER.OnUpdated( pet );
}

void DBService::onDBPetDeleted(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return;
	}
	PetId_t nPetId = inPacket->readInt64();
	PET_MANAGER.OnDeleted( nPetId );
}

void DBService::onDBUserPayed(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return;
	}
	int32_t uid = inPacket->readInt32();
	int32_t sid = inPacket->readInt32();
	GAME_SERVICE.onUserPayed(uid, sid);
}

void DBService::OnUpdateRansom( Answer::NetPacket *inPacket )
{
	EQUIP_RANSOM.OnUpdatedRansom( inPacket );
}

void DBService::OnAddRansom( DropEquipInfo& DropEquip)
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64( DropEquip.CharId );	
	packet->writeUTF8( DropEquip.CharName	);
	packet->writeInt64( DropEquip.KillerId );
	packet->writeUTF8( DropEquip.KillerName );
	packet->writeInt32( DropEquip.CanGetLastTime );
	packet->writeInt32( DropEquip.itemId );
	packet->writeInt8( DropEquip.itemClass );
	packet->writeInt32( DropEquip.itemCount );
	packet->writeInt8( DropEquip.bind );		
	packet->writeInt32( DropEquip.endTime );
	packet->writeInt64( DropEquip.srcId	);
	packet->writeInt32( DropEquip.Mid );
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_ADD_RANSOM_INFO);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::OnDleRansom( EquipId_t EquipId )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64( EquipId	);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_DLE_RANSOM_INFO);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

//���͸�dbServer�洢�ʼ�
void DBService::OnSendMail( MailInfo& Mail, std::string Param )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32( Mail.MailId );
	packet->writeInt32( Mail.SysMailId );
	packet->writeInt64( Mail.SenderId );
	packet->writeUTF8( Mail.SenderName );
	packet->writeInt64( Mail.ReceiveId );
	packet->writeUTF8( Mail.ReceiveName );
	packet->writeInt32( Mail.SendTime );
	packet->writeInt8( Mail.HasRead );
	packet->writeInt8( Mail.Extract );
	packet->writeUTF8( Mail.MailTitle );
	packet->writeUTF8( Mail.MailContent );
	packet->writeUTF8( Mail.Param );
	if ( Mail.Extract == 1 )
	{ 
		int8_t Count = 0;
		int32_t Offset = packet->getWOffset();
		packet->writeInt8( Count );
		for ( int8_t i = 0; i < MAIL_MAX_FU_JIAN_COUNT; ++i )
		{
			if ( Mail.Item[i].itemId <= 0 || Mail.Item[i].itemCount <= 0 )
			{
				continue;
			}
			packet->writeInt8(i);
			packet->writeInt32( Mail.Item[i].itemId );
			packet->writeInt8( Mail.Item[i].itemClass );	
			packet->writeInt32( Mail.Item[i].itemCount );
			packet->writeInt8( Mail.Item[i].bind );	
			packet->writeInt32( Mail.Item[i].endTime );
			packet->writeInt64( Mail.Item[i].srcId );
			Count++;
		}
		*(int8_t*)( packet->getBuffer() + Offset ) =  Count;
	}

	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_SEND_MAIL_TO_DB);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::OnUpdateMailInfo(Answer::NetPacket *inPacket )
{
	GAME_SERVICE.OnUpdateMail( inPacket );
}

void DBService::OnSendSysMail( CharId_t ReceiverId, int32_t SysMailId, std::string Param )
{
	MailInfo Mail = {};
	CfgSysMail* pSysMail = CFG_DATA.GetSysMail( SysMailId );
	if ( NULL == pSysMail )
	{
		return;
	}

	if ( pSysMail->sender_name.empty() || StringUtility::utf8Strlen(pSysMail->sender_name.c_str()) > MAX_NAME_CCH_LENGTH )
	{
		return;
	}

	if ( pSysMail->title.empty() || StringUtility::utf8Strlen( pSysMail->title.c_str() ) > MAIL_TITLE_MAX_LENGTH )
	{
		return;
	}

	if ( pSysMail->content.empty() || StringUtility::utf8Strlen( pSysMail->content.c_str()) > MAIL_CONTENT_MAX_LENGTH )
	{
		return;
	}
//	snprintf( Mail.SenderName,MAX_NAME_CCH_LENGTH -1 ,pSysMail->sender_name.c_str() );
	Mail.SysMailId		= SysMailId;
	Mail.ReceiveId		= ReceiverId;
	Mail.SendTime		= DayTime::now();
	Mail.Param			= Param;
//	snprintf( Mail.MailTitle,MAIL_TITLE_MAX_LENGTH -1 , pSysMail->title.c_str() );
//	snprintf( Mail.MailContent,MAIL_CONTENT_MAX_LENGTH -1 , pSysMail->content.c_str() );
	int32_t FuJianCount = pSysMail->item.size();
	if (  FuJianCount >= 1 && FuJianCount < MAIL_MAX_FU_JIAN_COUNT )
	{
		Mail.Extract	= 1;
		for ( int32_t i = 0; i < FuJianCount; ++i )
		{
			Mail.Item[i] = pSysMail->item[i];
		}
	}
	OnSendMail( Mail );
}

void DBService::OnSendSysMail( CharId_t ReceiverId, int32_t SysMailId, MemChrBag& item, std::string Param )
{
	MemChrBagVector items;
	if ( item.itemId > 0 && item.itemCount > 0 )
	{
		items.push_back( item );
	}
	OnSendSysMail( ReceiverId, SysMailId, items, Param );
}

void DBService::OnSendSysMail( CharId_t ReceiverId, int32_t SysMailId, const MemChrBagVector& items, std::string Param )
{
	MailInfo Mail = {};
	CfgSysMail* pSysMail = CFG_DATA.GetSysMail( SysMailId );
	if ( NULL == pSysMail )
	{
		return;
	}

	if ( pSysMail->sender_name.empty() || StringUtility::utf8Strlen(pSysMail->sender_name.c_str()) > MAX_NAME_CCH_LENGTH )
	{
		return;
	}

	if ( pSysMail->title.empty() || StringUtility::utf8Strlen( pSysMail->title.c_str() ) > MAIL_TITLE_MAX_LENGTH )
	{
		return;
	}

	if ( pSysMail->content.empty() || StringUtility::utf8Strlen( pSysMail->content.c_str()) > MAIL_CONTENT_MAX_LENGTH )
	{
		return;
	}
//	snprintf( Mail.SenderName,MAX_NAME_CCH_LENGTH -1 ,pSysMail->sender_name.c_str() );
	Mail.SysMailId		= SysMailId;
	Mail.ReceiveId		= ReceiverId;
	Mail.SendTime		= DayTime::now();
//	snprintf( Mail.MailTitle,MAIL_TITLE_MAX_LENGTH -1 , pSysMail->title.c_str() );
//	snprintf( Mail.MailContent,MAIL_CONTENT_MAX_LENGTH -1 , pSysMail->content.c_str() );
	Mail.Param			= Param;
	int32_t FuJianCount = items.size();
	if (  FuJianCount >= 1 && FuJianCount < MAIL_MAX_FU_JIAN_COUNT )
	{
		Mail.Extract	= 1;
		for ( int32_t i = 0; i < FuJianCount; ++i )
		{
			Mail.Item[i] = items[i];
		}
	}
	OnSendMail( Mail );
}

void DBService::onNewMinuteCome()
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt8(0);
	packet->setType(PACK_PROC);
	packet->setProc(IM_DB_NEW_MINUTE);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void DBService::SaveFamilyWarResult( int32_t nActId, FamilyId_t nFamilyId, int16_t nWinTimes, int32_t nTime, string FamilyName, string LeaderName )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}

	packet->writeInt32( nActId );
	packet->writeInt64( nFamilyId );
	packet->writeInt16( nWinTimes );
	packet->writeInt32( nTime );
	packet->writeUTF8( FamilyName );
	packet->writeUTF8( LeaderName );
	packet->setType( PACK_PROC );
	packet->setProc( IM_DB_SAVE_FAMILY_WAR_RESULT );
	packet->setSize( packet->getWOffset() );
	sendPacket(packet);
}

void DBService::onUpdateFamilyWarResult( Answer::NetPacket* inPacket )
{
	if ( NULL == inPacket )
	{
		return;
	}
	int32_t		nActId				= inPacket->readInt32();
	FamilyId_t	familyWarWinner		= inPacket->readInt64();
	int16_t		familyWarWinTimes	= inPacket->readInt16();
	string		familyName			= inPacket->readUTF8(true);
	string		leadyerName			= inPacket->readUTF8(true);

	ACTIVITY_MANAGER.OnFamilyWarResult( nActId, familyWarWinner, familyWarWinTimes, familyName, leadyerName );
}

void DBService::SaveTerritoryWarResult( int32_t nActId, string winners, int32_t nTime )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}

	packet->writeInt32( nActId );
	packet->writeInt32( nTime );
	packet->writeUTF8( winners );
	packet->setType( PACK_PROC );
	packet->setProc( IM_DB_SAVE_TERRITORY_WAR_RESULT );
	packet->setSize( packet->getWOffset() );
	sendPacket(packet);
}

void DBService::onUpdateTerritoryWarResult( Answer::NetPacket* inPacket )
{
	if ( NULL == inPacket )
	{
		return;
	}
	int32_t		nActId		= inPacket->readInt32();
	std::string	winners		= inPacket->readUTF8(true);

	ACTIVITY_MANAGER.OnTerritoryWarResult( nActId, winners );
}

void DBService::SaveCityWarResult( int32_t nActId, int32_t nIndex, FamilyId_t nFamilyId, int32_t nWinTime,
	CharId_t nLeader, CharId_t nFirst, CharId_t nSecond, CharId_t nThird,
	const std::string& strFirstFamily, const std::string& strSecondFamily, const std::string& strThirdFamily )
{
	NetPacket *packet = popNetpacket();
	if (NULL == packet)
	{
		return;
	}

	packet->writeInt32( nActId );
	packet->writeInt32( nIndex );
	packet->writeInt64( nFamilyId );
	packet->writeInt32( nWinTime );
	packet->writeInt64( nLeader );
	packet->writeInt64( nFirst );
	packet->writeInt64( nSecond );
	packet->writeInt64( nThird );
	packet->writeUTF8( strFirstFamily );
	packet->writeUTF8( strSecondFamily );
	packet->writeUTF8( strThirdFamily );
	packet->setType( PACK_PROC );
	packet->setProc( IM_DB_SAVE_CITY_WAR_RESULT );
	packet->setSize( packet->getWOffset() );
	sendPacket(packet);
}

void DBService::onUpdateCityWarResult( Answer::NetPacket* inPacket )
{
	if ( NULL == inPacket )
	{
		return;
	}
	int32_t		nActId		= inPacket->readInt32();
	int32_t		nIndex		= inPacket->readInt32();
	FamilyId_t	nFamilyId	= inPacket->readInt64();
	int32_t		nWinTime	= inPacket->readInt32();
	CharId_t	nLeader		= inPacket->readInt64();
	CharId_t	nFirst		= inPacket->readInt64();
	CharId_t	nSecond		= inPacket->readInt64();
	CharId_t	nThird		= inPacket->readInt64();
	std::string strFirstFamily	= inPacket->readUTF8(true);
	std::string strSecondFamily	= inPacket->readUTF8(true);
	std::string strThirdFamily	= inPacket->readUTF8(true);

	ACTIVITY_MANAGER.OnCityWarResult( nActId, nIndex, nFamilyId, nWinTime, nLeader, nFirst, nSecond, nThird,
		strFirstFamily, strSecondFamily, strThirdFamily );
}

void DBService::onUpdateGMBroadcast( Answer::NetPacket *inPacket )
{
	GM_BACKSTAGE.onUpdateGMBroadcast( inPacket );
}

void DBService::onUpdateGMBanChat( Answer::NetPacket *inPacket )
{
	GM_BACKSTAGE.onUpdateGMBanChat( inPacket );
}

void DBService::onUpdateGMSeal( Answer::NetPacket *inPacket )
{
	GM_BACKSTAGE.onUpdateGMSeal( inPacket );
}


