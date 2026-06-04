#pragma once

#include "Bag.h"
#include "CfgData.h"
#include "Task.h"
#include "TaskCycle.h"
#include "Achievement.h"
#include "Unit.h"
#include "actStructs.h"
#include "Dungeon.h"

#include "ExtSystemMgr.h"
#include "OperateLimit.h"
#include "Equip.h"
#include "FightChecker.h"
#include "Currency.h"
#include "CharTeam.h"
#include "KillMonsterTongJi.h"
#include "ChrDepot.h"
#include "CharPet.h"
#include "CharSkill.h"
#include "CharFamily.h"
#include "CharTitle.h"
#include "CharTeamDungeon.h"
#include "CharInsidePet.h"
#include "CharWorship.h"
#include "CharAuction.h"
#include "CharSoul.h"
#include "CharHallOfFame.h"

#include "Trade.h"
#include "PlayerMail.h"
#include "FriendExpReward.h"
#include "FaBao.h"
#include "Fighting.h"
#include "JueWei.h"
#include "ShangCheng.h"
#include "ChouJiang.h"
#include "QiFu.h"
#include "DailyActivity.h"
#include "Vip.h"
#include "ScoreShop.h"
#include "FunctionOpen.h"
#include "ExpBall.h"
#include "YunYingHD.h"
#include "TouZi.h"
#include "HuoYueDu.h"
#include "GuanWei.h"
#include "Achievement.h"
#include "WarVictory.h"
#define MAX_GOLD_SHARE_COUNT 40
#define MAX_CHALLENGE_MAP_MINUTES 120
#define MAX_STRATEGICS_READ_COUNT 5
#define BUY_POOL_COST 10
#define MAX_POOL_COUNT 100
#define REFUGEE_CHANGE_RATIO  100
#define REFUGEE_CHANGE_COUNT  5
#define DAY_FLY_COUNT  20
// #define DAILY_ITEM_STRATEGICS  5004
// #define DAILY_ITEM_EQUIP_ENHANCE 5009
// #define DAILY_ITEM_KINGDOM_REWAR  5018
// #define DAILY_ITEM_FAMILY_REWAR  5017
// #define DAILY_ITEM_WUSHEN_REWAR  5019
// #define DAILY_ITEM_LEVEL_REWAR  5020
// #define DAILY_ITEM_KILL_MONSTER  5016
// #define DAILY_ITEM_ONLINE_REWARD 5001
// #define DAILY_ITEM_LOGIN_REWARD 5002


struct PlayerPosition 
{
	int16_t actId;
	int32_t runnerId;
	int32_t mapid;
	int32_t x;
	int32_t y;
};

class Deal;
class ActMatchkingDom;
class ActPKMap;
class Player
	: public Unit
{
public:
	Player();
	virtual ~Player();

public:
	virtual void reset();
	virtual void refresh();
	virtual EntityId_t getUnitId() const;
	virtual CharId_t getOwner() const;
	virtual int32_t getLevel() const;
	virtual void	SetLevel(int32_t Level);
	virtual int32_t getDeadTime() const;
	virtual void postDamage(int32_t damge, UnitHandle launcher);
	virtual bool isFriendSide(Unit *pUnit);
	virtual bool checkSkillTarget(CfgSkill *pCfg, Unit *pUnit);			// ��⼼���Ƿ���Զ�Ŀ��ʩ��
	virtual Position getCurrentTile();
	virtual int32_t	GetCalLevel() const;

protected:
	virtual void broadcastBasicData();

public:
	virtual bool isRobot();
	virtual void saveToDB(int32_t reason, int32_t param);

public:
	static void initNetPacketHandlers();
	void init( PlayerDBData& dbData );

	int16_t getGateIndex() const;
	void setGateIndex(int16_t index);
	void appendInfo(Answer::NetPacket *packet);
	//int32_t getFamilyContribute() const;
	//void updateFamilyID(int32_t familyID);
	int32_t switchMap(Map *pMap, int32_t x, int32_t y, bool isFly);
	void moveToReviveRegion(bool bInAct);
	int32_t getActivityBirthRegion(Int32Vector* pRegsions) const;//ȡ�ù��ҵĳ�����
	int32_t leaveDungeon();
	bool isInRectangle(Position x,Position y) const;
	//bool inMatch() const;//�ڱ�����
	bool isLeader() const; //�Ƿ��ǰ���
	int32_t getLeaderCid() const;
	int32_t leaveActivity();
	CharId_t getCid() const;
	std::string getName() const;
	Sex_t getSex() const;
	Job_t getJob() const;
	//int32_t getKingdom() const;
	int64_t getExp() const;
	int32_t getCreateTime() const;
	int32_t getHead() const;

	int32_t getLastLogoutTime() const;
	int32_t getLastLoginTime() const;
	FamilyId_t getFamilyId() const;
	std::string getFamilyName() const;
	int8_t getFamilyPosition() const;
	Answer::NetPacket *getOtherQueryInfo();

	void delExp( int64_t nValue );
	void addExp(int64_t addon , bool Iswallow = true ); //Iswallow �Ƿ��ܷ�����ϵͳӰ�� 
	int64_t GetLevelExp();
	int64_t GetLevelMaxExp();

	void addKillPlayerCount();
	void addContribution(int32_t addon, int32_t rate);
	void addEventHp(int32_t hpValue);
	
	int32_t getUid() const;
	int32_t getSid() const;
	void syncGold();
	void syncTodayGoldPay();
	void updatePayGold();
  
	
	std::string getIP();

	void setTrailer(Trailer* pTrailer);
	void checkTrailer();
	Trailer* getTrailer();
	
	int32_t getClothesId();
	int32_t getWeaponId();

	double benefitRatio();
	BenefitType benefitType();//�������͡��𺦡�ƣ�͵�

	int32_t GetFreeSlotCount();
	MemChrBag getBagSlotData( int32_t slot );
	void setBagSlotData( int32_t slot, const MemChrBag &slotData, int32_t reason, int32_t count );

	bool hasItem(int32_t baseid, int32_t type, int32_t count);
	bool autoUseItem(const MemChrBag &item);
	void autoUseItem(MemChrBagVector &items);
	int32_t getFirstFreeSlot();

	virtual int32_t getBattle() const;
	//int32_t getJiangXing();
	//int32_t getChangeFlag();

	//void  updateTotalFightPower();
	//void  setChangeFlag(int32_t flag);
	//void  setFamilyPosition(int32_t value);
	//void  addJungong(int32_t addon,int32_t reason, BenefitType bnfType);//����
	//void  addWuhuen(int32_t addon,int32_t reason, BenefitType bnfType);//xinfa
	//void  addJiangxing(int32_t addon,int32_t reason, BenefitType bnfType);//wuyi
	//void  addKingdomContribute(int32_t addon);
	//void  addFamilyContribute(int32_t addon);
	//void  addFamilyMoney(int32_t moneyAddon,int32_t contributeAddon);

	//pvp
	int32_t getPkMode();
	void setPkMode(int32_t mode, bool backUp);
	void resetPkModeOnEnterMap(const Map *pMap);
	void updatePkValue( Player *pKiller, Map* pMap );
	void addPkValue(int32_t addon);
	int32_t getPkValue();
	int32_t getPkProtectTime();
	void	SetPkProtectTime( int32_t Time );
	void	SafeToPrison();

	void updateLastTaskId(int32_t tid);
	void setTaskCanSubmit(int32_t tid);
    void checkTaskCanSubmit(int32_t dungeonID);
	void taskTalkWithNpc(int32_t npcid);

	void updateRecord(int32_t id, int32_t param);
	int32_t getRecord(int32_t id) const;
	void clearRecordRange(int32_t start, int32_t end, int32_t diffDay);

	void onKillMonster(int32_t mid, int32_t level, int32_t exp, bool isBoss );
	PlayerAction getAction();
	void kingdomTaskFaile();
	void setKingdomTaskState(int32_t tid,int32_t state);

	void onNewDayCome();
	void refreshDailyCheck(bool first);
	void refeshKillerRecord();
	void sendActivityGain(int32_t activityId, int32_t exp, int32_t money, int32_t isEnd);

	// City war / family war functions
	void SendFamilyWarIcon();
	void SetActState(int8_t ActState);
	int8_t GetActState() const;

private:
	int8_t m_actState;

	void setSyncStatusFlag();
	void setSyncToTeamFlag();
	//void sendActivityData(int8_t kingdom,int32_t acitd,int8_t ntype,int32_t values ,int32_t id,const char* fmName);
	void addNetPacket( Answer::NetPacket *inPacket, uint32_t rsize );
	void addLogoutPacket(int32_t reason, int32_t param);
	void broadcastLeave();

	void setOldPosition();
	PlayerPosition getOldPosition();

	void setActFrontPosition();
	void sendBasicInfo();

	void sendPlayerEquipInfo();
	void sendEquipInfo(const MemEquip &memEquip);
	void sendEquipInfo(const MemEquipVector &memEquips);

	void sendItemEffect(const std::string &effect);

	void sendGainInfo(int32_t type, int64_t value, BenefitType benefitType);
	void sendChrRecord();
	void sendBuyItemInfo(int32_t item_id, int32_t item_type, int32_t count, int32_t money, int32_t index = 0 );
	void sendSellItemInfo(int32_t item_id, int32_t item_type, int32_t count, int32_t money);
	void sendGambel(const MemChrBagVector& item);
	void sendPublicChat(int32_t channel, Answer::NetPacket *inPacket);

	//int32_t onEnterActDungeon(Kingdom* pKingdom,ActPKMap* pMap);
	//int32_t onLeaveActDungeon(int32_t mapid,int16_t bRemove);

	void onActAddHL();

	int32_t verifyBagInfo( const Int32Vector& vSlot,int32_t nItemId,int32_t nCount);//��ʵ����Ϣ
	int32_t queryAutoBuyBagInfo( const Int32Vector& vSlot,int32_t nItemId,int32_t& count,int32_t nShopId);//�Զ������ʵ����Ϣ
	bool queryBagInfo( Answer::NetPacket* inPacket, Int32Vector& vSlot );

	int32_t verifyBagInfoForGroup(Answer::NetPacket *inPacket,MemChrBagVector &slotDataVector,int32_t item_type,int32_t count);

	bool isFirstSevenDay();

	void setOpenId(std::string openId);
	std::string getOpenId();
	void setOpenKey(std::string openKey);
	std::string getOpenKey();
	void setPf(std::string pf);
	std::string getPf();
	void setPfKey(std::string pfKey);
	std::string getPfKey();

	int32_t checkPreventWallow(Answer::NetPacket *inPacket);
	int     BirthdayIsRight(std::string cardId);
	int	    GetDay(int year,int month);
	bool    CheckString(std::string card); 

	int32_t getIsYellowVip();
	void setIsYellowVip(int32_t isYellowVip);
	int32_t getIsYellowYearVip();
	void setIsYellowYearVip(int32_t isYellowYearVip);
	int32_t getYellowVipLevel();
	void setYellowVipLevel(int32_t yellowVipLevel);
	int32_t getIsYellowHighVip();
	void setIsYellowHighVip(int32_t isYellowHighVip);
	int32_t getMemYellowStone(CharId_t cid,int32_t id);
	int32_t getYellowGrowId();

	void onDamageEvent(UnitHandle target, int32_t attackValue);									// �˺���Ϣ
	void onDamagedEvent(UnitHandle launcher, int32_t attackValue);								// �˺���Ϣ

	bool	IsDead() const;
	int64_t	GetDieTick() const;
	void	RecalcAttr();
private:
	typedef int32_t (Player::*NetPacketHandler)(Answer::NetPacket*);
	static void setNetPacketHandler(int32_t proc, NetPacketHandler handler);
	static Player::NetPacketHandler getNetPacketHandler(int32_t proc);

	bool isPreventWallow(std::string card);

	void recalcAttr();
	void setBaseAttr();

	void checkNetPackets();

	bool isDeadProc( ProcId_t nProc ) const;

	// start NetPacket
	int32_t onLogout(Answer::NetPacket *inPacket);
	int32_t onSyncTime(Answer::NetPacket *inPacket);
	int32_t	onMove(Answer::NetPacket *inPacket);
	int32_t	onJump(Answer::NetPacket *inPacket);
	int32_t onTrailerMove(Answer::NetPacket *inPacket);
	int32_t	onHit(Answer::NetPacket *inPacket);
	int32_t	onKickOutMove(Answer::NetPacket *inPacket);
	int32_t onUnitThrowed(Answer::NetPacket *inPacket);
	int32_t onArrive(Answer::NetPacket *inPacket);
	int32_t onSwitchMap(Answer::NetPacket *inPacket);
	int32_t onEnterDungeon(Answer::NetPacket *inPacket);
	int32_t onLeaveDungeon(Answer::NetPacket *inPacket);
	//int32_t onLeaveActDungeon(Answer::NetPacket *inPacket);
	int32_t onDungeonBuildTower(Answer::NetPacket *inPacket);
	int32_t onDungeonBuyTower(Answer::NetPacket *inPacket);
	int32_t onDungeonStart(Answer::NetPacket *inPacket);
	int32_t onDungeonSelectReward(Answer::NetPacket *inPacket);
	int32_t onGetAwardActivity(Answer::NetPacket *inPacket);
	int32_t	onEnterActivity(Answer::NetPacket *inPacket);
	int32_t	onLeaveActivity(Answer::NetPacket *inPacket);
	int32_t onSelectUnit(Answer::NetPacket *inPacket);
	int32_t onDoUnitSkill(Answer::NetPacket *inPacket);
	int32_t onPickDropItem(Answer::NetPacket *inPacket);
	int32_t onUseTrap(Answer::NetPacket *inPacket);
	int32_t onSwitchPkMode(Answer::NetPacket *inPacket);
	int32_t onSafeRevive(Answer::NetPacket *inPacket);
	int32_t onSiteRevive(Answer::NetPacket *inPacket);
	int32_t onStrongRevive(Answer::NetPacket *inPacket);
	int32_t onQueryChrInfo(Answer::NetPacket *inPacket);
	int32_t onUpgradeLevel(Answer::NetPacket *inPacket);



	int32_t onAddBagSlot(Answer::NetPacket *inPacket);
	int32_t onUseItem(Answer::NetPacket *inPacket);
	int32_t onQuerySkillList(Answer::NetPacket *inPacket);
	int32_t onUpgradeSkillLevel(Answer::NetPacket *inPacket);
	int32_t onQueryTaskList(Answer::NetPacket *inPacket);
	int32_t onReceiveTask(Answer::NetPacket *inPacket);
	int32_t onSubmitTask(Answer::NetPacket *inPacket);
	int32_t onGiveUpTask(Answer::NetPacket *inPacket);
	int32_t onSetTaskCanSubmit(Answer::NetPacket *inPacket);
	int32_t onTalkWithNpc(Answer::NetPacket *inPacket);
	int32_t onQuickDone(Answer::NetPacket *inPacket);
	int32_t onTeleport(Answer::NetPacket *inPacket);
	int32_t onTeleportActivity(Answer::NetPacket *inPacket);
	int32_t onPetDoSkill(Answer::NetPacket *inPacket);
	int32_t onAddAction(Answer::NetPacket *inPacket);
	int32_t onRemoveAction(Answer::NetPacket *inPacket);
	int32_t onExchangeAction(Answer::NetPacket *inPacket);
	int32_t onSetAutoFight(Answer::NetPacket *inPacket);
	int32_t onSetSystemSetting(Answer::NetPacket *inPacket);
	int32_t onBuyChrShopItem(Answer::NetPacket *inPacket);
	int32_t onBuyResource(Answer::NetPacket *inPacket);
	int32_t onBuyTaskCount(Answer::NetPacket *inPacket);
	int32_t onBuyBackChrShopItem(Answer::NetPacket *inPacket);
	int32_t onPatchUseItem(Answer::NetPacket *inPacket);
//	int32_t onQueryMonsterBroadcast(Answer::NetPacket *inPacket);
	int32_t onTeleportByItem(Answer::NetPacket *inPacket);
	int32_t onClickPayButton(Answer::NetPacket *inPacket);
	int32_t onMapEntered(Answer::NetPacket *inPacket);
	int32_t onDebugCmd(Answer::NetPacket *inPacket);
	int32_t onQueryPlayerInfo(Answer::NetPacket *inPacket);
	int32_t onGetEveryDayGold(Answer::NetPacket *inPacket);
	int32_t onQueryEveryDayGoldInfo(Answer::NetPacket *inPacket);

	int32_t onGamePublicChat(Answer::NetPacket *inPacket);
	int32_t onBuyPvpState(Answer::NetPacket *inPacket);
	int32_t onQueryKillerRankSelf(Answer::NetPacket *inPacket);
	int32_t onUpdateFlyIconInt(Answer::NetPacket *inPacket);
	int32_t onSocialUseExchangeCode(Answer::NetPacket *inPacket);
	int32_t initYellowStone(Answer::NetPacket *inPacket);
	int32_t getYellowAward(Answer::NetPacket *inPacket);
	// end NetPacket
	int32_t OnAskBossInfo( Answer::NetPacket *inPacket );
	int32_t OnRequestActivityInfo( Answer::NetPacket* inPacket );
	int32_t OnActivityGetDailyReward( Answer::NetPacket* inPacket );
	int32_t OnRequestTerritoryWarInfo( Answer::NetPacket* inPacket );

	Int32Vector getStrategicsReadState(int32_t *pIndexArry);
	void sendLoginInfo();
	void sendFightExpMoney(int32_t exp,int32_t kingdom_contribute);
	void sendChrInfo();
	void sendViewStarFinished(int32_t sid, int32_t level);
	void sendSkillList();
	void sendNewSkill(int32_t skillid);
	void sendActionList();
	void sendAutoFight();
	void sendSystemSetting();
	void sendKilledByPlayer(CharId_t cid, const std::string &name);
	void sendToastInfo(CharId_t cid,CharId_t beCid);
	void sendActivityState();
	void UpdateKilledByPlayer( CharId_t KillerId );

	void sendUpdateSocialPlayerInfo(PlayerInfoIndex index, int32_t value);
	void sendJungongChangeInfo(int32_t addon, BenefitType bnfType);
	void sendWuhuenChangeInfo(int32_t addon, BenefitType bnfType);
	void sendjiangxingChangeInfo(int32_t addon, BenefitType bnfType);
	void sendKingdomChangeInfo(int32_t addon);
	void sendPvpInfo();
	void sendChrLoginInInfo();
	void sendKillerRankSelf();
	void sendGetGoldInfo();
	//void broadcastMagicSmith(int32_t equipId);

	void sendYellowStone();
	void sendPreventWallow();
	void checkSaveToDB();
	void checkSyncStatus();
	void checkDie();
	void checkPool();
	void minuteCheck(bool bSend);
	void clearSelectedUnit();
	bool autoUpgradeLevel();
	bool upgradeLevel(bool bAuto);

	int32_t doTeleport(int32_t aid);
	int32_t doTeleportActivity(int32_t aid);
	void safeRevive();
	MemChrSkillVector::iterator findSkill(int32_t skillid);
	void addSkill(int32_t skillid, int32_t level);
	void removeSkill(int32_t skillid);

	int32_t upgradeSkill( int32_t skillid, int32_t BagSlot );
	void doSkillCost(CfgSkill *pCfgSkill, MemChrSkillVector::iterator &itSkill);				// ��������
	void doSkillLevel(int32_t sid, int32_t slevel, UnitHandle target);
	//bool isEnterDungeon(int32_t id ,int8_t ntype);
	bool isInWorldBossActivity();
	bool isFirstDay();
	bool checkUseSkill();

private:
	static NetPacketHandler m_netPacketHandlers[CM_GAME_MAX];
	Answer::NetPacketList m_netPackets;
	int16_t m_cgindex;

	MemCharacter m_chr;

	SysUser m_sysUser;
	SysUserPreventWallow m_sysUserPreventWallow;

	int32_t m_pkMode_bk;
	int64_t m_lastPkValueTick;

	int64_t	m_nDieTick;

	UseSkill	m_useSkill;

public:
	ChrTask& GetTask() { return m_task;}
private:
	ChrTask m_task;

	MemChrAction m_actions[ACTION_NUMBER];
	MemChrAutoFight m_autoFight;
	std::string m_systemSetting;

	MemChrSkillVector m_skills;
	MemYellowStoneVector m_mysVector;
	
	int64_t m_jumpTime;
	UnitHandle m_selectedUnit; 

	tm m_lastLocalNow;
	int64_t m_lastActionTick;
	int64_t m_lastSaveTick;
	int64_t m_lastPoolTick;
	int64_t m_lastExpPoolTick;
	int64_t m_lastMagicWeaponBuffTick;

	PlayerPosition m_ActFrontPosition; //����ǰ��λ��
	PlayerPosition m_oldPosition;

	int32_t m_eventHP;
	
	Trailer *m_trailer;
	bool m_needSyncStatus;
	bool m_needSyncToTeam;
	int32_t m_saveDataTimeCount;
	int32_t m_levelStartTime;
	int32_t m_todayGoldCharge;//���ճ�ֵԪ��
	int32_t m_firstFiveDayGoldCharge;//����10���ֵԪ��

	std::string m_openId;
	std::string m_openKey;
	std::string m_pf;
	std::string m_pfKey;

	int32_t m_is_yellow_vip;
	int32_t m_is_yellow_year_vip;
	int32_t m_yellow_vip_level;
	int32_t m_is_yellow_high_vip;



// ��չϵͳ
public:
	void			SaveDBData( PlayerDBData& dbData );
	void			InitExtSystems();
protected:
private:
	CExtSystemMgr	m_ExtSysMgr;

/*
* EXT_SYS_WMF
* ���������ϵͳ������֮��
*/
	friend class CExtOperateLimit;
public:				CExtOperateLimit&		GetOperateLimit() { return m_extOperateLimit; }
private:			CExtOperateLimit		m_extOperateLimit;

	friend class CExtEquip;
public:				CExtEquip&				GetEquip() { return m_extEquip; }
private:			CExtEquip				m_extEquip;

	friend class CExtFightChecker;
public:				CExtFightChecker&		GetFightChecker() { return m_extFightChecker; }
					bool					IsInFight() const { return m_extFightChecker.IsInFight(); }
					void					SetInFight() { m_extFightChecker.SetInFight( getTick() ); }
private:			CExtFightChecker		m_extFightChecker;

	friend class CExtCharBag;
public:				CExtCharBag&			GetBag() { return m_extCharBag; }
private:			CExtCharBag				m_extCharBag;

	friend class CExtCurrency;
public:				CExtCurrency&			GetCurrency() { return m_extCurrency; }
					int64_t					GetCurrency( CURRENCY_TYPE const nType ) const;
					bool					AddCurrency( CURRENCY_TYPE const nType, int64_t nVal, int32_t opWay, int64_t nParam = 0 );
					bool					DecCurrency( CURRENCY_TYPE const nType, int64_t nVal, int32_t opWay, int64_t nParam = 0 );
					bool					DecGoldAndCash( int64_t nVal, int32_t opWay, int32_t nParam = 0 );
private:			CExtCurrency			m_extCurrency;

	friend class CExtCharTeam;
public:				CExtCharTeam&			GetCharTeam() { return m_extCharTeam; }
					bool					IsInTeam() const { return m_extCharTeam.IsInTeam(); }
					bool					IsTeamLeader() const { return m_extCharTeam.IsLeader(); }
					PlayerTeamStatus		GetTeamStatus() const;
					int32_t					GetTeamId() const { return m_extCharTeam.GetTeamId(); }
					CharId_t				GetTeamLeaderId() const { return m_extCharTeam.GetLeaderId(); }
					std::string				GetTeamLeaderName() const { return m_extCharTeam.GetLeaderName(); }
private:			CExtCharTeam			m_extCharTeam;

	friend class CExtCharPet;
public:				CExtCharPet&			GetCharPet() { return m_extCharPet; }
private:			CExtCharPet				m_extCharPet;

	friend class CExtCharSkill;
public:				CExtCharSkill&			GetCharSkill() { return m_extCharSkill; }
private:			CExtCharSkill			m_extCharSkill;

	friend class CExtChrTaskCycle;
public:				CExtChrTaskCycle&		GetCharTaskCycle() { return m_extTaskCycle; }
private:			CExtChrTaskCycle		m_extTaskCycle;

	friend class CExtCharFamily;
public:				CExtCharFamily&			GetCharFamily() { return m_extCharFamily; }
private:			CExtCharFamily			m_extCharFamily;

	friend class CExtCharTitle;
public:				CExtCharTitle&			GetCharTitle() { return m_extCharTitle; }
private:			CExtCharTitle			m_extCharTitle;

	friend class CExtCharTeamDungeon;
public:				CExtCharTeamDungeon&	GetCharTeamDungeon() { return m_extCharTeamDungeon; }
private:			CExtCharTeamDungeon		m_extCharTeamDungeon;

	friend class CExtCharInsidePet;
public:				CExtCharInsidePet&		GetCharInsidePet() { return m_extCharInsidePet; }
private:			CExtCharInsidePet		m_extCharInsidePet;

	friend class CExtCharWorship;
public:				CExtCharWorship&		GetCharWorship() { return m_extCharWorship; }
private:			CExtCharWorship			m_extCharWorship;

	friend class CExtCharAuction;
public:				CExtCharAuction&		GetCharAuction() { return m_extCharAuction; }
					bool					IsInStall() const { return m_extCharAuction.IsInStall(); }
private:			CExtCharAuction			m_extCharAuction;

	friend class CExtCharSoul;
public:				CExtCharSoul&			GetCharSoul() { return m_extCharSoul; }
					int32_t					GetSoulLevel() const { return m_extCharSoul.GetLevel(); }
					int32_t					GetMaxLevel() const { return m_extCharSoul.GetPlayerMaxLevel(); }
private:			CExtCharSoul			m_extCharSoul;

	friend class CExtCharSoul;
public:				CExtCharHallOfFame&		GetCharHallOfFame() { return m_extCharHallOfFame; }
private:			CExtCharHallOfFame		m_extCharHallOfFame;

/*
* EXT_SYS_ZK
* �쿪����ϵͳ������֮��
*/

public: 
	CChrDepot&			GetCharDepot() { return m_PlayerDepot; }
private:
	CChrDepot			m_PlayerDepot;
	friend				class CChrDepot;

public:
	CKillMonsterTongJi&	GetPlayerKillMonsterTongJi(){ return m_KillMonsterTongJi;}
private:
	CKillMonsterTongJi	m_KillMonsterTongJi;

public:
	CTrade&				GetPlayerTrade(){ return m_PlayerTrade;}
private:
	CTrade				m_PlayerTrade;

public:
	CPlayerMail&		GetPlayerMail(){ return m_PlayerMail; }
private:
	CPlayerMail			m_PlayerMail;

public:
	CFriendExpReward&   GetFriendExp() { return m_FRiendExp; } 
private:
	CFriendExpReward	m_FRiendExp;

public:
	CFaBao&				GetPlayerFaBao(){ return m_PlayerFaBao;}
private:
	CFaBao				m_PlayerFaBao;
	friend				class CFaBao;

public: 
	CFighting&			GetPlayerFighting(){ return m_Fighting; }
private:
	CFighting			m_Fighting;
	friend				class CFighting;

public:
	CJueWei&			GetPlayerJueWei(){ return m_PlayerJueWei; }
private:
	CJueWei				m_PlayerJueWei;
	friend				class CJueWei;

public:
	CShangCheng&		GetPlayerShangCheng(){ return m_PlayerShangCheng;}
private:
	CShangCheng			m_PlayerShangCheng;

public:	
	CQiFu&				GetPlayerQiFu(){ return	m_PlayerQiFu;}
private:
	CQiFu				m_PlayerQiFu;

public:
	ChouJiang&			GetPlayerChouJiang(){ return m_PlayerChouJiang; }
private:
	ChouJiang			m_PlayerChouJiang;

public:
	DailyActivity&		GetPlayerDailyActivity(){ return m_PlayerDailyActivity; }
private:
	DailyActivity		m_PlayerDailyActivity;

public:
	CVip&				GetPlayerVip(){ return m_PlayerVip;}
private:
	CVip				m_PlayerVip;
	friend				class CVip;

public:
	CExpBall&			GetExpBall(){ return m_PlayerExpBall;}
private:
	CExpBall			m_PlayerExpBall;

private:
	ScoreShop			m_ScoreShop;
public:
	CFunctionOpen&		GetPlayerFunctionOpen(){ return m_PlayerFunctionOpen;}
private:
	CFunctionOpen		m_PlayerFunctionOpen;
public:
	CYunYingHD&			GetPlayerYunYingHd(){ return m_PlayerYunYingHD; }
private:				
	CYunYingHD			m_PlayerYunYingHD;
public:
	CTouZi&				GetPlayerGetTouZi(){ return m_PlayerTouZi; }
private:
	CTouZi				m_PlayerTouZi;

public:
	CHuoYueDu&			GetPlayerHuoYueDu(){ return m_PlayerHuoYueDu; }
private:
	CHuoYueDu			m_PlayerHuoYueDu;
public:
	CGuanWei&			GetPlayerGuanWei(){ return m_PlayerGuanWei; }
private:
	CGuanWei			m_PlayerGuanWei;
	friend				class CGuanWei;
public: 
	CAchievement&		GetAchievemnet(){ return m_Achievement; }
private:
	CAchievement		m_Achievement;	
public:
	CWarVictory&		GetWarVictory(){ return  m_WarVictory; }
private:
	CWarVictory			m_WarVictory;
public:
	void			SetPlantId( EntityId_t PlantId );
	void			SetStartGather( int64_t CurTick );
	EntityId_t		GetPlantId();
	int64_t			GetStartGather();
	void			sendEndGather(int32_t err);
	void			BreakGather( bool IsNotify = true );
private:
	int32_t			onBeginGather(Answer::NetPacket *inPacket);
	int32_t			onEndGather(Answer::NetPacket *inPacket);
	int32_t			OnKaiFuHuoDongOperator( Answer::NetPacket *inPacket );


	EntityId_t m_plantId;				//���ڲɼ��Ĳɼ���
	int64_t	   m_startGatherTick;	
public:
	int32_t				GetStartProtect();
	void				TiShiInfo( int32_t TiShiId, int32_t Pos = 0 );
	void				onDropItem( Player *pKiller, int32_t Mid = 0 );
	void				SendBossHomeInfo( int32_t MapId );
private:
//�����
	void				SetStartProtect( int32_t ProtectTime );
	int32_t				m_StartProtect;								//��ʼ������ʱ��
	int32_t				GetProtectTime();							//��ȡ����ʱ��
	void				DieResetXp();
	void				SendHDIcon();
	int32_t				OnRandPos( int32_t BagSlot );				//ʹ���������
	int32_t				OnBackCity( int32_t BagSlot );				//ʹ�ûسǾ�
	void				PayedDispose( int32_t AddGold );			//��ֵ�Ժ�Ĵ���

	int32_t				OnEnterBossHome( Answer::NetPacket *inPacket );
	int32_t				OnLevelBossHome( Answer::NetPacket *inPacket );
	int32_t				LevelBossHome();
	int32_t				OnEnterVipGuaJiMap( Answer::NetPacket *packet );

	int32_t				OnEnterMoLingRuQin( Answer::NetPacket *inPacket );
	int32_t				OnClickGame( Answer::NetPacket *inPacket );
	int32_t				OnUsemMultiItem( Answer::NetPacket *inPacket );
public:
	void				LevelUped();
	void				SetLvelStartTime();
	void				LeaveVipGuaJiMap();
	bool				SubPkValues( int32_t Addon );
	int32_t				GetTotalPayGold(){ return m_sysUser.gold_pay_total; }
	int32_t				GetTodayPayGold(){ return m_todayGoldCharge; }
	bool				HasSkill( int32_t SkillId );
	void				AddAppendAttr();
private:
	int32_t				m_InBossHomeTime;
public:
	void				AddExpRate( int32_t AddValue );
	int32_t				GetExpRate();
private:
	int32_t				m_ExpRate;
	//����ֵ�ظ�
public:
	void				SetPPTick( int64_t CurTick );
private:
	int64_t				m_LastAddPPTick;
public:
	bool				GetSysSettingInfo( int32_t Index );
private:
	void				InitSysSetting();
	std::string			GetSysSetting();
	std::map<int32_t, int32_t> m_SystemSetting;
};


