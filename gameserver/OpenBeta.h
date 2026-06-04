#ifndef __OPEN_BETA_H__
#define __OPEN_BETA_H__

#include "CfgData.h"
#include "DataStructs.h"
#include "Mutex.h"

class Player;


class COpenBeta
{
public:
	COpenBeta();
	~COpenBeta();

public:
	void	Init(int32_t line);

	void	OnDaySwitch();
	void	OnNewMinute(int32_t nMinute);
	void	updateStartTime();

	void	GetIconState(Player* player, IconStateList* IconList);
	void	SendIconState(Player* player);
	ShowIcon*	getIconState(ShowIcon* retstr, Player* player);
	void	SendActivityInfo(Player* player);

	void	GetBossIconState(IconStateList* IconList);
	void	SendBossIconState();
	ShowIcon*	getBossIconState(ShowIcon* retstr);
	bool	IsInBossTime();
	void	BossDie(int32_t MonsterId, std::string* p_name, CharId_t Cid);
	void	BroadcastBossKilled(const std::string* name, CharId_t cid);
	void	CheckBoss();
	void	broadcastBossStart();
	void	broadcastBossEnd();

	int32_t	GetRechargeGift(Player* player, int32_t nIndex);
	int32_t	BuyGiftShopItem(Player* player, int8_t nIndex);
	int32_t	BuyTitleShopItem(Player* player, int8_t nIndex);
	int32_t	BuyShiZhuangShopItem(Player* player, int8_t nIndex);
	int32_t	BuyLiQuan(Player* player, int8_t nIndex);

	void	AddOnlineRecord(Player* player);
	bool	canGetOnlineGift(Player* player);
	int32_t	GetOnlineTimeRewardTime(Player* player);
	int32_t	GetOnlineGift(Player* player);

	bool	IsInTime(int8_t Type);
	int32_t	getLeftTime();
	bool	IsOpenBetaTime();
	int32_t	GetEndCollectDropTime();
	int32_t	GetChouJiangType();

	int32_t	GetFlopType(int32_t nFlopId);
	bool	CanFlopDraw(Player* pPlayer, int32_t FlopId);
	int32_t	GetMaxFlopTimes(Player* pPlayer);

	void	LoadYanHuaValue();
	void	UpdateYanHuaValue();
	void	AddYanHuaPoint(int32_t Values);
	int32_t	GetYanHuaPointReward(Player* player, int8_t nIndex);

	int32_t	GetQianDaoReward(Player* player, int8_t nIndex, int8_t nType);

	void	loadXiaoFeiSumRank();
	void	updateXiaoFeiSumRank(Player* player, int32_t AddValues);
	void	checkXiaoFeiSumRankInvalid(int8_t connid);
	void	checkXiaoFeiSumRank();
	void	sendXiaoFeiSumRankMail();

	void	sendBroadcast(Player* pPlayer, int32_t nType, CharId_t Cid, const std::string& name);
	void	SendResult(Player* pPlayer, int8_t Type, int32_t Index, int32_t param);

	// Rank struct
	struct FestivalRank
	{
		CharId_t	Cid;
		int32_t		nValue;
		int32_t		nUpdateTime;
	};

	struct TimeArea
	{
		int32_t nStartTime;
		int32_t nEndTime;
	};

	struct ShopGoods
	{
		int32_t nId;
		int32_t nPrice;
		MemChrBagVector vItems;
	};

private:
	void	initCfgData();

public:
	int32_t		m_nDay;
	int32_t		m_nMinute;
	int32_t		m_nStartDay;
	int32_t		m_nEndDay;
	int32_t		m_nIcon;
	int32_t		m_nVersion;
	int32_t		m_nTitleShopSize;
	int32_t		m_nBossStartBroadcast;
	int32_t		m_nBossKillBroadcast;
	int32_t		m_nBossEndBroadcast;
	int32_t		m_nBossIcon;
	int32_t		m_nEndCollectCnt;
	int32_t		m_nEndCollectCntTime;
	int32_t		m_nEndCollectTime;
	int32_t		m_nDailyLimitShopSize;
	int32_t		m_nDailyLimitShop2Size;
	int32_t		m_nShiZhuangShopSize;
	int32_t		m_nRechargeDaysSize;
	int32_t		m_ChouJiangType;
	int32_t		m_nShowSize;
	int32_t		m_nXiaoFeiSumRankSize;
	int32_t		m_nXiaoFeiSumRankLimit;
	int32_t		m_FlopSize;
	int32_t		m_LiQuanSize;
	int32_t		m_QianDaoSize;
	int32_t		m_YanHuaSize;
	int32_t		m_nOnlineStartMinute;
	int32_t		m_nOnlineEndMinute;
	int32_t		m_nOnlineTimeArea;
	int32_t		m_nMaxCount;

	int32_t		m_vStartDay[18];
	int32_t		m_vEndDay[18];
	int8_t		m_vOpen[18];

	int32_t		m_MapId;
	int32_t		m_X;
	int32_t		m_Y;
	int32_t		m_Mid;
	int32_t		m_MonsterId;
	int8_t		m_bDie;

	int32_t		m_CurrYanHuaPoint;

	Answer::Mutex	m_lock;

	std::map<int8_t, std::vector<FestivalRank>>				m_mXiaoFeiSumRank;
	std::vector<TimeArea>									m_vBossMinute;
	std::vector<int32_t>									m_vRechargeValue;
	std::vector<std::vector<MemChrBag>>						m_vRechargeGift;
	std::vector<int32_t>									m_vDailyLimitShopBroadcast;
	std::vector<ShopGoods>									m_vDailyLimitShopGoods;
	std::vector<int32_t>									m_vTitleShopBroadcast;
	std::vector<ShopGoods>									m_vTitleShopGoods;
	std::vector<int32_t>									m_vShiZhuangShopBroadcast;
	std::vector<ShopGoods>									m_vShizhuangShopGoods;
	std::vector<MemChrBag>									m_vOnlineReward;
	std::vector<int32_t>									m_vXiaoFeiSumRankMail;
	std::vector<std::vector<MemChrBag>>						m_vXiaoFeiSumRankReward;
	std::vector<ItemData>									m_FlopCost;
	std::vector<int32_t>									m_FlopType;
	std::vector<MemChrBag>									m_LiQuanItem;
	std::vector<int32_t>									m_LiQuanPriceVt;
	std::vector<int32_t>									m_LiQuanLimitCount;
	std::vector<ItemData>									m_ItemDataVector;
	std::vector<MemChrBag>									m_QianDaoReward;
	std::vector<int32_t>									m_BuQianPrice;
	std::vector<int32_t>									m_YanHuaPointVt;
	std::vector<MemChrBag>									m_YanHuaReward;
};

#endif // __OPEN_BETA_H__
