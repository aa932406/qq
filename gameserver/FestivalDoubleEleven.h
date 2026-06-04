#ifndef __FESTIVAL_DOUBLE_ELEVEN_H__
#define __FESTIVAL_DOUBLE_ELEVEN_H__

#include "CfgData.h"
#include "DataStructs.h"
#include "Mutex.h"
#include "ItemHelper.h"
#include "Timer.h"

class Player;


// FESTIVAL_ACTIVITY_TYPE for CFestivalDoubleEleven
enum FESTIVAL_ACTIVITY_TYPE
{
	FAT_LAND					= 0,
	FAT_DAILY_RECHARGE			= 1,
	FAT_DRAW					= 2,
	FAT_DRAW_RANK				= 3,
	FAT_EXCHANGE				= 4,
	FAT_WORLD_BOSS				= 5,
	FAT_HUO_YUE_DU_SUM			= 6,
	FAT_DAILY_LIMIT_SHOP		= 7,
	FAT_LAND_SUM				= 8,
	FAT_ONLINE_TIME				= 9,
	FAT_BEST_WISH				= 10,
	FAT_DAILY_XIAO_FEI_RANK		= 11,
	FAT_XIAO_FEI_DRAW			= 12,
	FAT_MO_YU_SHI_JIE			= 13,
	FAT_DAILY_RECHARGE_2		= 14,
	FAT_XIAO_FEI_SUM_RANK		= 15,
	FAT_GIFT_SHOP				= 16,
	FAT_CHOU_JIANG				= 17,
	FAT_FA_BAO_BACK				= 18,
	FAT_MYSTERY_SHOP			= 19,
	FAT_RECHARGE_SUM_GIFT		= 20,
	FAT_XIAO_FEI_SUM_GIFT		= 21,
	FAT_BUY_PET_GIFT			= 22,
	FAT_DA_TI					= 23,
	FAT_DAILY_LIMIT_SHOP_2		= 24,
	FAT_EQUIP_UPSTAR_BACK		= 25,
	FAT_RECHARGE_DRAW			= 26,
	FAT_BUY_GIFT				= 27,
	FAT_RECHARGE_BACK			= 28,
	FAT_BOSS_SCORE				= 29,
	FAT_PET_ILLUSION_ITEM		= 30,
	FAT_RECHARGE_SUM_RANK		= 31,
	FAT_CROSS_RECHARGE_SUM_RANK	= 32,
	FAT_CROSS_XIAO_FEI_SUM_RANK	= 33,
	FAT_PET_EQUIP_CHOU_JIANG	= 34,
	FAT_CROSS_CHOU_JIANG_RANK	= 35,
	FAT_EQUIP_QING_YI			= 36,
	FAT_FRIEND_QING_YI			= 37,
	FAT_SCORE_SHOP				= 38,
	FAT_BOSS_SCORE_RANK			= 39,
	FAT_BOSS_SCORE_DRAW			= 40,
	FAT_GOU_WU_CHE				= 41,
	FAT_LIAN_CHONG				= 42,
	FAT_MYSTERY_SHOP_AUTO		= 43,
	FAT_MAX						= 44,
};

struct ShopGoods
{
	int32_t nId;
	int32_t nPrice;
	MemChrBagVector vItems;
};

struct TimeArea
{
	int32_t nStartTime;
	int32_t nEndTime;
};

struct RateItem
{
	int32_t nId;
	int32_t nRate;
	int32_t nCount;
};

struct LianRechargeCfg
{
	int32_t nDay;
	MemChrBagVector vItems;
	int32_t nRewardId;
};

class CFestivalDoubleEleven
{
public:
	CFestivalDoubleEleven();
	~CFestivalDoubleEleven();

public:
	void	Init(int32_t line);
	void	OnDaySwitch();
	void	OnNewMinute(int32_t nMinute);
	void	HotUpdate();
	void	initCfgData();
	void	initOpenList(const std::string& str);

	bool	IsInTime();
	bool	needShowIcon();
	bool	IsInTime(FESTIVAL_ACTIVITY_TYPE nType);
	bool	IsOpen(FESTIVAL_ACTIVITY_TYPE nType);
	bool	isOpen(FESTIVAL_ACTIVITY_TYPE nType);
	bool	CheckDropGroup(int32_t nDropGroup);

	int32_t	getLeftTime();

	void	GetIconState(Player* player, IconStateList* IconList);
	void	SendIconState(Player* player);
	ShowIcon*	getIconState(ShowIcon* retstr, Player* player);
	void	hideIcon(int32_t nIconId);
	void	SendActivityInfo(Player* player);

	bool	IsInWorldBossTime();

	// World Boss
	void	GetWorldBossIconState(IconStateList* IconList);
	void	SendWorldBossIconState();
	ShowIcon*	getWorldBossIconState(ShowIcon* retstr);
	ShowIcon*	getMoYuShiJieIconState(ShowIcon* retstr);
	void	checkWorldBoss();
	void	BossDie(int32_t MonsterId, std::string* p_name, CharId_t Cid);
	void	BroadcastWorldBossKilled(const std::string* name, CharId_t cid);
	void	broadcastWorldBossStart();
	void	broadcastWorldBossEnd();

	// MoYuShiJie
	void	GetMoYuShiJieIconState(IconStateList* IconList);
	void	BroadCastMoYuShiJieIconState();
	void	AddMoYuShiJieDrop(int32_t sequence, MemChrBag* vItem);

	// Gift methods
	int32_t	GetLandGift(Player* player);
	int32_t	GetLandSumGift(Player* player, int8_t nIndex);
	int32_t	GetDrawGift(Player* player);
	int32_t	GetOnlineGift(Player* player);
	int32_t	GetWishGift(Player* player);
	int32_t	GetHuoYueDuSumGift(Player* player, int8_t nIndex);
	int32_t	GetDailyRechargeGift(Player* player, int8_t nIndex);
	int32_t	GetRechargeSumGift(Player* player, int8_t nIndex);
	int32_t	GetPetIllusionItemGift(Player* player, int8_t nIndex);
	int32_t	GetXiaoFeiSumGift(Player* player, int32_t nId);
	int32_t	GetRechargeBack(Player* player);
	int32_t	GetEquipUpStarBackItem(Player* player);
	int32_t	GetFaBaoCritBackItem(Player* player, int8_t nType);
	int32_t	GetEquipQingYiGift(Player* player, int8_t nIndex);
	int32_t	GetFriendQingYiGift(Player* player, int8_t nIndex);

	// Shop/Buy methods
	int32_t	BuyDailyLimitShopItem(Player* player);
	bool	BuyTitle(Player* player);
	int32_t	BuyGiftShopItem(Player* player, int8_t nIndex);
	int32_t	BuyGiftShopItem2(Player* player, int8_t nIndex);
	int32_t	BuyGiftItem(Player* player, int8_t nIndex);
	int32_t	GouWuChe(Player* player, int32_t* p_nIndexList);

	// Record methods
	void	GetLianRechargeReward(Player* player, int8_t nType, int8_t nIndex);
	void	AddDrawTimes(Player* player, int32_t nCount);
	void	AddHuoYueDu(Player* player, int32_t nValue);
	void	AddOnlineRecord(Player* player);
	void	AddXiaoFeiRecord(Player* player, int32_t nCount);
	void	AddRechargeRecord(Player* player, int32_t nCount);
	void	AddPetIllusionItemRecord(Player* player, int32_t nCount);
	void	AddFriendQingYi(Player* player, int32_t nValue);
	void	AddEquipUpStarBack(Player* player, int32_t nCount);
	void	AddFaBaoValue(Player* player, int8_t nType, int32_t Values);
	void	AddMoYuShiJieDrop(int32_t sequence, Player* player, MemChrBag* vItem);
	void	OnRecharge(Player* player, int32_t nCount);

	// Rank methods
	void	UpdateRank(Player* player, CharId_t Id, int32_t policy, int8_t type, int32_t nDay, const std::string* info);
	void	UpdateXiaoFeiRank(Player* player);
	void	UpdateMonsterScore(Player* player, int32_t Score);

	bool	CanUseXiaoFeiDraw(Player* player);
	bool	CanUseRechargeDraw(Player* player);
	bool	canGetLandGift(Player* player);
	bool	canGetLandSumGift(Player* player);
	bool	canGetDrawGift(Player* player);
	bool	canGetOnlineGift(Player* player);
	bool	canGetWishGift(Player* player);
	bool	canGetFaBaoBack(Player* player);
	bool	canGetEquipUpStarBack(Player* player);
	bool	canGetRechargeBack(Player* player);

	int32_t	GetOnlineTimeRewardTime(Player* player);
	int32_t	getLandSum(Player* player);
	int32_t	GetXiaoFeiSumGiftCount(Player* player);
	int32_t	GetChouJiangType();
	int32_t	GetChouJiangSpecialTime();
	int32_t	GetScoreCanDrawTime(Player* pPlayer);
	int32_t	OnRandScoreDrawItem(Player* pPlayer);
	int32_t	CalBossScoreAddValue(int32_t nValue);
	int32_t	calRechargeBack(int32_t nValue);
	int32_t	calRechargeBackHelper(int32_t* nValue);

	void	SendDaTiReward(Player* player, int8_t RewardType, CharId_t CharId);
	void	GongGao(int32_t GongGaoId, Player* player);

	// Rank data
	void	loadRankData(int32_t line);
	void	resetRankData(int32_t line);
	void	resetPlayerData();
	int32_t	loadVersion(int32_t line);
	void	saveVersion(int32_t line);

	void	loadDrawRank();
	void	updateDrawRank(Player* player);
	void	updateDrawRank(int8_t connid, int32_t nIndex, const std::string& info);
	void	checkDrawRankInvalid(int8_t connid);
	void	checkDrawRank();
	void	sendDrawRankMail();

	void	loadXiaoFeiRank();
	void	updateXiaoFeiRank(Player* player);
	void	updateXiaoFeiRank(int8_t connid, int32_t nDay, int32_t nIndex, const std::string& info);
	void	checkXiaoFeiRankInvalid(int8_t connid);
	void	checkXiaoFeiRank();
	void	sendXiaoFeiRankMail();

	void	loadXiaoFeiSumRank();
	void	updateXiaoFeiSumRank(Player* player);
	void	updateXiaoFeiSumRank(int8_t connid, int32_t nIndex, const std::string& info);
	void	checkXiaoFeiSumRankInvalid(int8_t connid);
	void	checkXiaoFeiSumRank();
	void	sendXiaoFeiSumRankMail();

	void	loadRechargeSumRank();
	void	updateRechargeSumRank(Player* player);
	void	updateRechargeSumRank(int8_t connid, int32_t nIndex, const std::string& info);
	void	checkRechargeSumRankInvalid(int8_t connid);
	void	checkRechargeSumRank();
	void	sendRechargeSumRankMail();

	void	loadCrossRechargeSumRank();
	void	updateCrossRechargeSumRank(Player* player);
	void	updateCrossRechargeSumRank(int8_t connid, int32_t nIndex, const std::string& info);
	void	checkCrossRechargeSumRankInvalid(int8_t connid);
	void	checkCrossRechargeSumRank();

	void	loadCrossXiaoFeiSumRank();
	void	updateCrossXiaoFeiSumRank(Player* player);
	void	updateCrossXiaoFeiSumRank(int8_t connid, int32_t nIndex, const std::string& info);
	void	checkCrossXiaoFeiSumRankInvalid(int8_t connid);
	void	checkCrossXiaoFeiSumRank();

	void	loadCrossChouJiangRank();
	void	updateCrossChouJiangRank(Player* player);
	void	updateCrossChouJiangRank(int8_t connid, int32_t nIndex, const std::string& info);
	void	checkCrossChouJiangRankInvalid(int8_t connid);
	void	checkCrossChouJiangRank();

	void	loadBossScoreRank();
	void	updateBossScoreRank(Player* player);
	void	updateBossScoreRank(int8_t connid, int32_t nIndex, const std::string& info);
	void	checkBossScoreRankInvalid(int8_t connid);
	void	checkBossScoreRank();
	void	sendBossScoreRankMail();

	void	broadcastPlayerAction(Player* pPlayer, int32_t nType, CharId_t Cid, const std::string& name);
	void	RefreshMysteryShopItem(Player* player);
	void	checkRefreshMysteryShop();
	void	broadcastRefreshMysteryShop();

	// Rank struct
	struct FestivalRank
	{
		CharId_t	Cid;
		std::string	strName;
		int32_t		nValue;
		int32_t		nUpdateTime;
		int8_t		bChange;
	};

public:
	int32_t		m_nDay;
	int32_t		m_nMinute;
	int32_t		m_nStartDay;
	int32_t		m_nEndDay;
	int32_t		m_nIcon;
	int32_t		m_nVersion;
	int32_t		m_nDropGroup;
	int32_t		m_nBossStartBroadcast;
	int32_t		m_nBossKilledBroadcast;
	int32_t		m_nBossEndBroadcast;

	int32_t		m_vStartDay[FAT_MAX];
	int32_t		m_vEndDay[FAT_MAX];
	int8_t		m_vOpen[FAT_MAX];

	int32_t		m_nHotSign;

	int32_t		m_nDrawRankSize;
	int32_t		m_nXiaoFeiRankSize;
	int32_t		m_nXiaoFeiSumRankSize;
	int32_t		m_nRechargeSumRankSize;
	int32_t		m_BossScoreRankSize;
	int32_t		m_nHuoYueDuSumSize;
	int32_t		m_nDailyRechargeSize;
	int32_t		m_nRechargeSumSize;
	int32_t		m_nXiaoFeiSumSize;
	int32_t		m_nGiftShopSize;
	int32_t		m_nBuyGiftSize;
	int32_t		m_nPetIllusionItemSize;
	int32_t		m_nFriendQingYiSize;
	int32_t		m_nDailyLimitShopLimit;
	int32_t		m_nDailyLimitShopCostType;
	int32_t		m_nDailyLimitShopCostValue;
	int32_t		m_nOnlineStartMinute;
	int32_t		m_nOnlineEndMinute;
	int32_t		m_nOnlineTimeArea;
	int32_t		m_nMaxCount;
	int32_t		m_nDailyLimitShop2Size;

	int32_t		m_MapId;
	int32_t		m_X;
	int32_t		m_Y;
	int32_t		m_Mid;
	int32_t		m_MonsterId;
	int8_t		m_bDie;

	Answer::Mutex	m_lock;

	std::map<int8_t, std::vector<FestivalRank>>								m_mDrawRank;
	std::map<int8_t, std::vector<std::vector<FestivalRank>>>					m_mXiaoFeiRank;
	std::map<int8_t, std::vector<FestivalRank>>								m_mXiaoFeiSumRank;
	std::map<int8_t, std::vector<FestivalRank>>								m_mRechargeSumRank;
	std::map<int8_t, std::vector<FestivalRank>>								m_mCrossXiaoFeiSumRank;
	std::map<int8_t, std::vector<FestivalRank>>								m_mCrossRechargeSumRank;
	std::map<int8_t, std::vector<FestivalRank>>								m_mCrossChouJiangRank;
	std::map<int8_t, std::vector<FestivalRank>>								m_mBossScoreRank;

	std::vector<std::vector<MemChrBag>>										m_vLandGift;
	std::vector<MemChrBag>													m_vDrawLoopReward;
	std::vector<int32_t>													m_vDrawRankMail;
	std::vector<std::vector<MemChrBag>>										m_vDrawRankReward;
	std::vector<TimeArea>													m_vWorldBossMinute;
	std::vector<int32_t>													m_vHuoYueDuSumLimit;
	std::vector<std::vector<MemChrBag>>										m_vHuoYueDuSumReward;
	std::vector<MemChrBag>													m_vDailyLimitShopItem;
	std::vector<std::vector<MemChrBag>>										m_vLandSumGift;
	std::vector<MemChrBag>													m_vOnlineReward;
	std::vector<MemChrBag>													m_vWishReward;
	std::vector<int32_t>													m_vXiaoFeiRankLimit;
	std::vector<int32_t>													m_vXiaoFeiRankMail;
	std::vector<std::vector<MemChrBag>>										m_vXiaoFeiRankReward;
	std::vector<int32_t>													m_nXiaoFeiDrawValue;
	std::vector<std::vector<MemChrBag>>										m_vMoYuShiJieDrop;
	std::vector<int32_t>													m_vDailyRechargeLimit;
	std::vector<std::vector<MemChrBag>>										m_vDailyRechargeReward;
	std::vector<int32_t>													m_vDailyRechargeBroad;
	std::vector<int32_t>													m_vXiaoFeiSumRankMail;
	std::vector<std::vector<MemChrBag>>										m_vXiaoFeiSumRankReward;
	std::vector<std::vector<ShopGoods>>										m_vGiftShopGoods;
	std::vector<int32_t>													m_NeedValue;
	std::vector<MemChrBag>													m_BackItem;
	std::vector<int32_t>													m_vRechargeSumLimit;
	std::vector<int32_t>													m_vRechargeSumBroadcast;
	std::vector<std::vector<MemChrBag>>										m_vRechargeSumReward;
	std::vector<int32_t>													m_vXiaoFeiSumLimit;
	std::vector<int32_t>													m_vXiaoFeiSumBroadcast;
	std::vector<std::vector<MemChrBag>>										m_vXiaoFeiSumReward;
	std::vector<MemChrBag>													m_Items;
	std::vector<int32_t>													m_vDaTiRankMail;
	std::vector<std::vector<MemChrBag>>										m_vDaTiRankReward;
	std::vector<int32_t>													m_vDailyLimitShop2Broadcast;
	std::vector<ShopGoods>													m_vDailyLimitShop2Goods;
	std::map<int32_t, int32_t>												m_mEquipUpStarBackStarCount;
	std::vector<int32_t>													m_nRechargeDrawValue;
	std::vector<int32_t>													m_vBuyGiftPrice;
	std::vector<int32_t>													m_vBuyGiftBroadcast;
	std::vector<std::vector<MemChrBag>>										m_vBuyGiftItem;
	std::vector<int32_t>													m_vRechargeBackLimit;
	std::vector<int32_t>													m_vRechargeBackValue;
	std::vector<int32_t>													m_vPetIllusionItemLimit;
	std::vector<std::vector<MemChrBag>>										m_vPetIllusionItemReward;
	std::vector<int32_t>													m_vRechargeSumRankLimit;
	std::vector<int32_t>													m_vRechargeSumRankMail;
	std::vector<std::vector<MemChrBag>>										m_vRechargeSumRankReward;
	std::vector<int32_t>													m_vCrossRechargeSumRankLimit;
	std::vector<int32_t>													m_vCrossRechargeSumRankMail;
	std::vector<std::vector<MemChrBag>>										m_vCrossRechargeSumRankReward;
	std::vector<int32_t>													m_vCrossXiaoFeiSumRankLimit;
	std::vector<int32_t>													m_vCrossXiaoFeiSumRankMail;
	std::vector<std::vector<MemChrBag>>										m_vCrossXiaoFeiSumRankReward;
	std::vector<int32_t>													m_vCrossChouJiangRankLimit;
	std::vector<int32_t>													m_vCrossChouJiangRankMail;
	std::vector<std::vector<MemChrBag>>										m_vCrossChouJiangRankReward;
	std::vector<int32_t>													m_vEquipQingYiLimit;
	std::vector<std::vector<MemChrBag>>										m_vEquipQingYiReward;
	std::vector<int32_t>													m_vFriendQingYiLimit;
	std::vector<std::vector<MemChrBag>>										m_vFriendQingYiReward;
	std::vector<int32_t>													m_vBossScoreRankMail;
	std::vector<std::vector<MemChrBag>>										m_vBossScoreRankReward;
	std::vector<int32_t>													m_vBossScoreiLimit;
	std::vector<int32_t>													m_vGetBossScoreMail;
	std::vector<std::vector<MemChrBag>>										m_vBossScoreReward;
	std::vector<int32_t>													m_vScoreLimit;
	std::vector<int32_t>													m_nDailyRechargeDrawLimit;
	std::vector<RateItem>													m_vBossScoreDrawReward;
	std::vector<int32_t>													m_vGouWuCheGiftPrice;
	std::vector<MemChrBag>													m_vGouWuCheItem;
	std::vector<LianRechargeCfg>											m_LianRechargeCfgVt;
};

#endif // __FESTIVAL_DOUBLE_ELEVEN_H__
