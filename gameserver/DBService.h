#pragma once
//////////////////////////////////////////////////////////////////////////
// ��ȡ�����ݿ���������ӣ��ṩ���� ���������ݵ�dbserver��������
//
//////////////////////////////////////////////////////////////////////////
#include <map>
#include "actStructs.h"
#include "DataStruct.h"

class DBService
	: public Answer::TcpService
{
public:
	DBService();
	virtual ~DBService();

protected:
	//��������db���ݰ�
	virtual void onNetPacket(Answer::NetPacket *inPacket);

public:
	//��������dbserver��ȡplayer������Ϣ
	void loadPlayer(int16_t cgindex,  int32_t uid, int32_t sid);
	void UpdateLoginTime( CharId_t cid, int32_t login_time );

	void insertMemEquip(const MemEquip &equip);
	void updateMemEquip(const MemEquip &equip);
	void deleteMemEquip( EquipId_t eid );

	/* pet */
	void insertMemPet( const MemPetDBData &pet );
	void updateMemPet( const MemPetDBData &pet );
	void deleteMemPet( PetId_t nPetId );

	/* task */
	void insertTaskInfo( const LogTask &task );
	void updateTaskInfo( const LogTask &task );

	/*  equip */
	void insertEquipEnhance(const LogEquipEnhance &equip);
	void insertEquipAddgem(const LogEquipAddGem &equipAddGem);
	void insertItemCombine(const LogItemCombine &itemCombine);
	void insertEquipSmithing(const LogEquipSmithing &equipSmithing);
	void insertEquipDismantle(const LogEquipDismantle &equipDismantle);

	/* dungeon */
	void insertDungeon( const LogDungeon &dungeon );
	void updateDungeon( const LogDungeon &dungeon );

	/* family */
	void insertFamilyCreate(const LogFamily &family);
	void insertFamilyJoin(const LogFamily &family);
	void updateFamilyOut(const LogFamily &family);

	/*   achievement  */
	void insertAchievement(const LogAchievement &achievement);

	/* yellow_stone */
	void insertMemYelloStone(const MemYellowStone &mys,int16_t type);

	/* boss */
	void InsertBossLog( const LogBoss& boss );

	/* activity */
	void InsertActivityLog( const LogActivity& activity );

	/* gift */
	void InsertDailyGiftLog( const LogDailyGift& gift );

	// �
	void SaveFamilyWarResult( int32_t nActId, FamilyId_t nFamilyId, int16_t nWinTimes, int32_t nTime,string FamilyName, string LeaderName );
	void onUpdateFamilyWarResult( Answer::NetPacket* inPacket );
	void SaveTerritoryWarResult( int32_t nActId, string winners, int32_t nTime );
	void onUpdateTerritoryWarResult( Answer::NetPacket* inPacket );

	void SaveCityWarResult( int32_t nActId, int32_t nIndex, FamilyId_t nFamilyId, int32_t nWinTime,
		CharId_t nLeader, CharId_t nFirst, CharId_t nSecond, CharId_t nThird,
		const std::string& strFirstFamily, const std::string& strSecondFamily, const std::string& strThirdFamily );
	void onUpdateCityWarResult( Answer::NetPacket* inPacket );

	void insertItemChange(const LogItemChange &item);

	void logArtifactAddExp(const LogArtifactAddExp &artifactAddExp);
	void logArtifactAddMaxLevel(const LogArtifactAddMaxLevel &artifactAddMaxLevel);
	void logArtifactAddQuality(const LogArtifactAddQuality &artifactAddQuality);
	
	void logEquipAddGemSlotNum(const LogEquipAddGemSlot &equipAddGemSlotNum);
	void logEquipAppendStar(const LogEquipAppendStar &equipAppendStar);
	void logEquipCombine(const LogEquipCombine &equipCombine);

	void logEquipForge(const LogEquipForge &equipForge);
	void logEquipRefreshBind(const LogEquipRefreshBind &equipRefreshBind);
	void logEquipRefreshElement(const LogEquipRefreshElement &equipRefreshElement);
	void logEquipRefreshFloatAttr(const LogEquipRefreshFloatAttr &equipRefreshFloatAttr);
	void logEquipRemoveGem(const LogEquipRemoveGem &equipRemoveGem);
	void logEquipRemoveStar(const LogEquipRemoveStar &equipRemoveStar);
	void logEquipStrengthen(const LogEquipStrengthen &equipStrengthen);
	void logEquipUpgrade(const LogEquipUpgrade &equipUpgrade);

	void logPlayerLogin(CharId_t cid, int32_t action, int32_t time);
	void logPlayerDeal(CharId_t cid, std::string name, int64_t tcid, std::string tname, int32_t reason, int32_t money, std::string items, int32_t time);
	void logCurrency( CharId_t cid, const std::string& name, int32_t nType, int32_t opWay, int32_t nVal, int64_t nParam, int32_t nTime );

	void OnUpdateRansom( Answer::NetPacket *inPacket );
	void OnAddRansom( DropEquipInfo& DropEquip);
	void OnDleRansom( EquipId_t EquipId );

	void OnSendMail( MailInfo& Mail,std::string Param = "" );
	void OnSendSysMail( CharId_t ReceiverId, int32_t SysMailId, std::string Param = "" );							
	void OnSendSysMail( CharId_t ReceiverId, int32_t SysMailId, const MemChrBagVector& items, std::string Param = "" );
	void OnSendSysMail( CharId_t ReceiverId, int32_t SysMailId, MemChrBag& item, std::string Param = "");
	void OnUpdateMailInfo( Answer::NetPacket *inPacket );
	void onNewMinuteCome();

private:
	void onDBPlayerLoaded(Answer::NetPacket *inPacket);
	void onDBPlayerSaved(Answer::NetPacket *inPacket);
	void onDBEquipUpdated(Answer::NetPacket *inPacket);
	void onDBTitleUpdated(Answer::NetPacket *inPacket);
	void onDBUserPayed(Answer::NetPacket *inPacket);
	void onDBPetUpdated(Answer::NetPacket *inPacket);
	void onDBPetDeleted(Answer::NetPacket *inPacket);

	void onUpdateGMBroadcast( Answer::NetPacket *inPacket );
	void onUpdateGMBanChat( Answer::NetPacket *inPacket );
	void onUpdateGMSeal( Answer::NetPacket *inPacket ); 
};
#define DB_SERVICE Answer::Singleton<DBService>::instance()
