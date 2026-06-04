#include "stdafx.h"
#include "Shared.h"
#include "FestivalDoubleEleven.h"
#include "GameService.h"
#include "Player.h"
#include "Bag.h"
#include "Timer.h"
#include "DayTime.h"
#include "MapManager.h"
#include "Map.h"
#include "Monster.h"
#include "DBService.h"
#include "OperateLimit.h"

using namespace Answer;

CFestivalDoubleEleven::CFestivalDoubleEleven()
{
	m_nDay = -1;
	m_nMinute = 0;
	memset(m_vStartDay, 0, sizeof(m_vStartDay));
	memset(m_vEndDay, 0, sizeof(m_vEndDay));
	memset(m_vOpen, 0, sizeof(m_vOpen));
	m_nStartDay = 0;
	m_nEndDay = 0;
	m_nIcon = 0;
	m_nVersion = 0;
	m_nDropGroup = 0;
	m_nBossStartBroadcast = 0;
	m_nBossKilledBroadcast = 0;
	m_nBossEndBroadcast = 0;
	m_MapId = 0;
	m_X = 0;
	m_Y = 0;
	m_Mid = 0;
	m_MonsterId = 0;
	m_bDie = 1;
	m_nHotSign = 0;
	m_nDrawRankSize = 10;
	m_nXiaoFeiRankSize = 10;
	m_nXiaoFeiSumRankSize = 10;
	m_nRechargeSumRankSize = 10;
	m_BossScoreRankSize = 10;
	m_nHuoYueDuSumSize = 0;
	m_nDailyRechargeSize = 0;
	m_nRechargeSumSize = 0;
	m_nXiaoFeiSumSize = 0;
	m_nGiftShopSize = 0;
	m_nBuyGiftSize = 0;
	m_nPetIllusionItemSize = 0;
	m_nFriendQingYiSize = 0;
	m_nDailyLimitShopLimit = 0;
	m_nDailyLimitShopCostType = 0;
	m_nDailyLimitShopCostValue = 0;
	m_nOnlineStartMinute = 0;
	m_nOnlineEndMinute = 0;
	m_nOnlineTimeArea = 0;
	m_nMaxCount = 0;
	m_nDailyLimitShop2Size = 0;
}

CFestivalDoubleEleven::~CFestivalDoubleEleven()
{
}

void CFestivalDoubleEleven::Init(int32_t line)
{
	initCfgData();

	int32_t nVersion = loadVersion(line);
	if (nVersion > 0 && m_nVersion != nVersion || !nVersion)
	{
		loadRankData(line);
		saveVersion(line);
	}
	else
	{
		resetRankData(line);
		saveVersion(line);
	}
}

void CFestivalDoubleEleven::initCfgData()
{
	Answer::Inifile ini;
	ini.parse("./ServerConfig/Tables/FestivalDoubleEleven.cfg");

	// CONFIG
	{
		std::string strStartTime = ini.getStrValue("CONFIG", "start_time");
		if (!strStartTime.empty())
			m_nStartDay = DayTime::StringToIntTime(strStartTime);
		m_nEndDay = ini.getIntValue("CONFIG", "end_day");
		m_nIcon = ini.getIntValue("CONFIG", "icon");
		m_nVersion = ini.getIntValue("CONFIG", "version");
		m_nDropGroup = ini.getIntValue("CONFIG", "drop_group");
		m_nHotSign = ini.getIntValue("CONFIG", "hot_sign");
	}

	char szCol[64];

	// LAND (type 0)
	{
		m_vStartDay[0] = ini.getIntValue("LAND", "start_day");
		m_vEndDay[0] = ini.getIntValue("LAND", "end_day");
		std::string strGift = ini.getStrValue("LAND", "gift");
		if (!strGift.empty())
		{
			m_vLandGift.resize(1);
			m_vLandGift[0] = CItemHelper::parseItemString(0, strGift);
		}
	}

	// DAILY_RECHARGE (type 1)
	{
		m_vStartDay[1] = ini.getIntValue("DAILY_RECHARGE", "start_day");
		m_vEndDay[1] = ini.getIntValue("DAILY_RECHARGE", "end_day");
	}

	// DRAW (type 2)
	{
		m_vStartDay[2] = ini.getIntValue("DRAW", "start_day");
		m_vEndDay[2] = ini.getIntValue("DRAW", "end_day");
		std::string strReward = ini.getStrValue("DRAW", "reward");
		if (!strReward.empty())
			m_vDrawLoopReward = CItemHelper::parseItemString(0, strReward);
	}

	// DRAW_RANK (type 3)
	{
		m_vStartDay[3] = ini.getIntValue("DRAW_RANK", "start_day");
		m_vEndDay[3] = ini.getIntValue("DRAW_RANK", "end_day");
		m_nDrawRankSize = ini.getIntValue("DRAW_RANK", "size");
		m_vDrawRankMail.resize(m_nDrawRankSize);
		m_vDrawRankReward.resize(m_nDrawRankSize);
		for (int32_t i = 0; i < m_nDrawRankSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "mail%d", i + 1);
			m_vDrawRankMail[i] = ini.getIntValue("DRAW_RANK", szCol);
			snprintf(szCol, sizeof(szCol), "reward%d", i + 1);
			std::string strReward = ini.getStrValue("DRAW_RANK", szCol);
			if (!strReward.empty())
				m_vDrawRankReward[i] = CItemHelper::parseItemString(0, strReward);
		}
	}

	// EXCHANGE (type 4)
	{
		m_vStartDay[4] = ini.getIntValue("EXCHANGE", "start_day");
		m_vEndDay[4] = ini.getIntValue("EXCHANGE", "end_day");
	}

	// DA_TI (type 23)
	{
		m_vStartDay[23] = ini.getIntValue("DA_TI", "start_day");
		m_vEndDay[23] = ini.getIntValue("DA_TI", "end_day");
	}

	// WORLD_BOSS (type 5)
	{
		m_vStartDay[5] = ini.getIntValue("WORLD_BOSS", "start_day");
		m_vEndDay[5] = ini.getIntValue("WORLD_BOSS", "end_day");
		m_nBossStartBroadcast = ini.getIntValue("WORLD_BOSS", "start_broadcast");
		m_nBossEndBroadcast = ini.getIntValue("WORLD_BOSS", "end_broadcast");
		m_nBossKilledBroadcast = ini.getIntValue("WORLD_BOSS", "kill_broadcast");
		m_MapId = ini.getIntValue("WORLD_BOSS", "mapid");
		m_X = ini.getIntValue("WORLD_BOSS", "x");
		m_Y = ini.getIntValue("WORLD_BOSS", "y");
		m_Mid = ini.getIntValue("WORLD_BOSS", "mid");
		m_MonsterId = ini.getIntValue("WORLD_BOSS", "monster_id");
		int32_t nBossCount = ini.getIntValue("WORLD_BOSS", "count");
		m_vWorldBossMinute.resize(nBossCount);
		for (int32_t i = 0; i < nBossCount; ++i)
		{
			snprintf(szCol, sizeof(szCol), "start_minute%d", i + 1);
			m_vWorldBossMinute[i].nStartTime = ini.getIntValue("WORLD_BOSS", szCol);
			snprintf(szCol, sizeof(szCol), "end_minute%d", i + 1);
			m_vWorldBossMinute[i].nEndTime = ini.getIntValue("WORLD_BOSS", szCol);
		}
	}

	// HUO_YUE_DU_SUM (type 6)
	{
		m_vStartDay[6] = ini.getIntValue("HUO_YUE_DU_SUM", "start_day");
		m_vEndDay[6] = ini.getIntValue("HUO_YUE_DU_SUM", "end_day");
		m_nHuoYueDuSumSize = ini.getIntValue("HUO_YUE_DU_SUM", "size");
		m_vHuoYueDuSumLimit.resize(m_nHuoYueDuSumSize);
		m_vHuoYueDuSumReward.resize(m_nHuoYueDuSumSize);
		for (int32_t i = 0; i < m_nHuoYueDuSumSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "limit%d", i + 1);
			m_vHuoYueDuSumLimit[i] = ini.getIntValue("HUO_YUE_DU_SUM", szCol);
			snprintf(szCol, sizeof(szCol), "reward%d", i + 1);
			std::string strReward = ini.getStrValue("HUO_YUE_DU_SUM", szCol);
			if (!strReward.empty())
				m_vHuoYueDuSumReward[i] = CItemHelper::parseItemString(0, strReward);
		}
	}

	// DAILY_LIMIT_SHOP (type 7)
	{
		m_vStartDay[7] = ini.getIntValue("DAILY_LIMIT_SHOP", "start_day");
		m_vEndDay[7] = ini.getIntValue("DAILY_LIMIT_SHOP", "end_day");
		m_nDailyLimitShopLimit = ini.getIntValue("DAILY_LIMIT_SHOP", "limit");
		m_nDailyLimitShopCostType = ini.getIntValue("DAILY_LIMIT_SHOP", "cost_type");
		m_nDailyLimitShopCostValue = ini.getIntValue("DAILY_LIMIT_SHOP", "cost_value");
		std::string strItems = ini.getStrValue("DAILY_LIMIT_SHOP", "items");
		if (!strItems.empty())
			m_vDailyLimitShopItem = CItemHelper::parseItemString(0, strItems);
	}

	// LAND_SUM (type 8)
	{
		m_vStartDay[8] = ini.getIntValue("LAND_SUM", "start_day");
		m_vEndDay[8] = ini.getIntValue("LAND_SUM", "end_day");
		int32_t nLandSumSize = ini.getIntValue("LAND_SUM", "size");
		m_vLandSumGift.resize(nLandSumSize);
		for (int32_t i = 0; i < nLandSumSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "gift%d", i + 1);
			std::string strGift = ini.getStrValue("LAND_SUM", szCol);
			if (!strGift.empty())
				m_vLandSumGift[i] = CItemHelper::parseItemString(0, strGift);
		}
	}

	// ONLINE_TIME (type 9)
	{
		m_vStartDay[9] = ini.getIntValue("ONLINE_TIME", "start_day");
		m_vEndDay[9] = ini.getIntValue("ONLINE_TIME", "end_day");
		m_nOnlineStartMinute = ini.getIntValue("ONLINE_TIME", "start_time");
		m_nOnlineEndMinute = ini.getIntValue("ONLINE_TIME", "end_time");
		m_nOnlineTimeArea = 60 * ini.getIntValue("ONLINE_TIME", "dis_time");
		std::string strReward = ini.getStrValue("ONLINE_TIME", "reward");
		if (!strReward.empty())
			m_vOnlineReward = CItemHelper::parseItemString(0, strReward);
		m_nMaxCount = ini.getIntValue("ONLINE_TIME", "max_count");
	}

	// BEST_WISH (type 10)
	{
		m_vStartDay[10] = ini.getIntValue("BEST_WISH", "start_day");
		m_vEndDay[10] = ini.getIntValue("BEST_WISH", "end_day");
		std::string strReward = ini.getStrValue("BEST_WISH", "reward");
		if (!strReward.empty())
			m_vWishReward = CItemHelper::parseItemString(0, strReward);
	}

	// DAILY_XIAO_FEI_RANK (type 11)
	{
		m_vStartDay[11] = ini.getIntValue("DAILY_XIAO_FEI_RANK", "start_day");
		m_vEndDay[11] = ini.getIntValue("DAILY_XIAO_FEI_RANK", "end_day");
		m_nXiaoFeiRankSize = ini.getIntValue("DAILY_XIAO_FEI_RANK", "size");
		m_vXiaoFeiRankMail.resize(m_nXiaoFeiRankSize);
		m_vXiaoFeiRankReward.resize(m_nXiaoFeiRankSize);
		for (int32_t i = 0; i < m_nXiaoFeiRankSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "mail%d", i + 1);
			m_vXiaoFeiRankMail[i] = ini.getIntValue("DAILY_XIAO_FEI_RANK", szCol);
			snprintf(szCol, sizeof(szCol), "reward%d", i + 1);
			std::string strReward = ini.getStrValue("DAILY_XIAO_FEI_RANK", szCol);
			if (!strReward.empty())
				m_vXiaoFeiRankReward[i] = CItemHelper::parseItemString(0, strReward);
		}
	}

	// XIAO_FEI_DRAW (type 12)
	{
		m_vStartDay[12] = ini.getIntValue("XIAO_FEI_DRAW", "start_day");
		m_vEndDay[12] = ini.getIntValue("XIAO_FEI_DRAW", "end_day");
		int32_t nDrawSize = ini.getIntValue("XIAO_FEI_DRAW", "size");
		m_nXiaoFeiDrawValue.resize(nDrawSize);
		for (int32_t i = 0; i < nDrawSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "value%d", i + 1);
			m_nXiaoFeiDrawValue[i] = ini.getIntValue("XIAO_FEI_DRAW", szCol);
		}
	}

	// MO_YU_SHI_JIE (type 13)
	{
		m_vStartDay[13] = ini.getIntValue("MO_YU_SHI_JIE", "start_day");
		m_vEndDay[13] = ini.getIntValue("MO_YU_SHI_JIE", "end_day");
	}

	// DAILY_RECHARGE_2 (type 14)
	{
		m_vStartDay[14] = ini.getIntValue("DAILY_RECHARGE_2", "start_day");
		m_vEndDay[14] = ini.getIntValue("DAILY_RECHARGE_2", "end_day");
		m_nDailyRechargeSize = ini.getIntValue("DAILY_RECHARGE_2", "size");
		m_vDailyRechargeLimit.resize(m_nDailyRechargeSize);
		m_vDailyRechargeReward.resize(m_nDailyRechargeSize);
		m_vDailyRechargeBroad.resize(m_nDailyRechargeSize);
		for (int32_t i = 0; i < m_nDailyRechargeSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "gold%d", i + 1);
			m_vDailyRechargeLimit[i] = ini.getIntValue("DAILY_RECHARGE_2", szCol);
			snprintf(szCol, sizeof(szCol), "gift%d", i + 1);
			std::string strGift = ini.getStrValue("DAILY_RECHARGE_2", szCol);
			if (!strGift.empty())
				m_vDailyRechargeReward[i] = CItemHelper::parseItemString(0, strGift);
			snprintf(szCol, sizeof(szCol), "broadcast%d", i + 1);
			m_vDailyRechargeBroad[i] = ini.getIntValue("DAILY_RECHARGE_2", szCol);
		}
	}

	// XIAO_FEI_SUM_RANK (type 15)
	{
		m_vStartDay[15] = ini.getIntValue("XIAO_FEI_SUM_RANK", "start_day");
		m_vEndDay[15] = ini.getIntValue("XIAO_FEI_SUM_RANK", "end_day");
		m_nXiaoFeiSumRankSize = ini.getIntValue("XIAO_FEI_SUM_RANK", "size");
		m_vXiaoFeiSumRankMail.resize(m_nXiaoFeiSumRankSize);
		m_vXiaoFeiSumRankReward.resize(m_nXiaoFeiSumRankSize);
		for (int32_t i = 0; i < m_nXiaoFeiSumRankSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "mail%d", i + 1);
			m_vXiaoFeiSumRankMail[i] = ini.getIntValue("XIAO_FEI_SUM_RANK", szCol);
			snprintf(szCol, sizeof(szCol), "reward%d", i + 1);
			std::string strReward = ini.getStrValue("XIAO_FEI_SUM_RANK", szCol);
			if (!strReward.empty())
				m_vXiaoFeiSumRankReward[i] = CItemHelper::parseItemString(0, strReward);
		}
	}

	// GIFT_SHOP (type 16)
	{
		m_vStartDay[16] = ini.getIntValue("GIFT_SHOP", "start_day");
		m_vEndDay[16] = ini.getIntValue("GIFT_SHOP", "end_day");
		m_nGiftShopSize = ini.getIntValue("GIFT_SHOP", "size");
		m_vGiftShopGoods.resize(m_nGiftShopSize);
		for (int32_t i = 0; i < m_nGiftShopSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "price%d", i + 1);
			m_vGiftShopGoods[i].resize(1);
			m_vGiftShopGoods[i][0].nPrice = ini.getIntValue("GIFT_SHOP", szCol);
			snprintf(szCol, sizeof(szCol), "goods%d", i + 1);
			std::string strGoods = ini.getStrValue("GIFT_SHOP", szCol);
			if (!strGoods.empty())
				m_vGiftShopGoods[i][0].vItems = CItemHelper::parseItemString(0, strGoods);
		}
	}

	// CHOU_JIANG (type 17)
	{
		m_vStartDay[17] = ini.getIntValue("CHOU_JIANG", "start_day");
		m_vEndDay[17] = ini.getIntValue("CHOU_JIANG", "end_day");
	}

	// FA_BAO_BACK (type 18)
	{
		m_vStartDay[18] = ini.getIntValue("FA_BAO_BACK", "start_day");
		m_vEndDay[18] = ini.getIntValue("FA_BAO_BACK", "end_day");
		int32_t nNeedSize = ini.getIntValue("FA_BAO_BACK", "size");
		m_NeedValue.resize(nNeedSize);
		for (int32_t i = 0; i < nNeedSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "value%d", i + 1);
			m_NeedValue[i] = ini.getIntValue("FA_BAO_BACK", szCol);
		}
	}

	// MYSTERY_SHOP (type 19)
	{
		m_vStartDay[19] = ini.getIntValue("MYSTERY_SHOP", "start_day");
		m_vEndDay[19] = ini.getIntValue("MYSTERY_SHOP", "end_day");
	}

	// RECHARGE_SUM_GIFT (type 20)
	{
		m_vStartDay[20] = ini.getIntValue("RECHARGE_SUM_GIFT", "start_day");
		m_vEndDay[20] = ini.getIntValue("RECHARGE_SUM_GIFT", "end_day");
		m_nRechargeSumSize = ini.getIntValue("RECHARGE_SUM_GIFT", "size");
		m_vRechargeSumLimit.resize(m_nRechargeSumSize);
		m_vRechargeSumReward.resize(m_nRechargeSumSize);
		m_vRechargeSumBroadcast.resize(m_nRechargeSumSize);
		for (int32_t i = 0; i < m_nRechargeSumSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "limit%d", i + 1);
			m_vRechargeSumLimit[i] = ini.getIntValue("RECHARGE_SUM_GIFT", szCol);
			snprintf(szCol, sizeof(szCol), "reward%d", i + 1);
			std::string strReward = ini.getStrValue("RECHARGE_SUM_GIFT", szCol);
			if (!strReward.empty())
				m_vRechargeSumReward[i] = CItemHelper::parseItemString(0, strReward);
			snprintf(szCol, sizeof(szCol), "broadcast%d", i + 1);
			m_vRechargeSumBroadcast[i] = ini.getIntValue("RECHARGE_SUM_GIFT", szCol);
		}
	}

	// XIAO_FEI_SUM_GIFT (type 21)
	{
		m_vStartDay[21] = ini.getIntValue("XIAO_FEI_SUM_GIFT", "start_day");
		m_vEndDay[21] = ini.getIntValue("XIAO_FEI_SUM_GIFT", "end_day");
		m_nXiaoFeiSumSize = ini.getIntValue("XIAO_FEI_SUM_GIFT", "size");
		m_vXiaoFeiSumLimit.resize(m_nXiaoFeiSumSize);
		m_vXiaoFeiSumReward.resize(m_nXiaoFeiSumSize);
		m_vXiaoFeiSumBroadcast.resize(m_nXiaoFeiSumSize);
		for (int32_t i = 0; i < m_nXiaoFeiSumSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "limit%d", i + 1);
			m_vXiaoFeiSumLimit[i] = ini.getIntValue("XIAO_FEI_SUM_GIFT", szCol);
			snprintf(szCol, sizeof(szCol), "reward%d", i + 1);
			std::string strReward = ini.getStrValue("XIAO_FEI_SUM_GIFT", szCol);
			if (!strReward.empty())
				m_vXiaoFeiSumReward[i] = CItemHelper::parseItemString(0, strReward);
			snprintf(szCol, sizeof(szCol), "broadcast%d", i + 1);
			m_vXiaoFeiSumBroadcast[i] = ini.getIntValue("XIAO_FEI_SUM_GIFT", szCol);
		}
	}

	// BUY_PET_GIFT (type 22)
	{
		m_vStartDay[22] = ini.getIntValue("BUY_PET_GIFT", "start_day");
		m_vEndDay[22] = ini.getIntValue("BUY_PET_GIFT", "end_day");
	}

	// DAILY_LIMIT_SHOP_2 (type 24)
	{
		m_vStartDay[24] = ini.getIntValue("DAILY_LIMIT_SHOP_2", "start_day");
		m_vEndDay[24] = ini.getIntValue("DAILY_LIMIT_SHOP_2", "end_day");
		m_nDailyLimitShop2Size = ini.getIntValue("DAILY_LIMIT_SHOP_2", "size");
		m_vDailyLimitShop2Goods.resize(m_nDailyLimitShop2Size);
		m_vDailyLimitShop2Broadcast.resize(m_nDailyLimitShop2Size);
		for (int32_t i = 0; i < m_nDailyLimitShop2Size; ++i)
		{
			snprintf(szCol, sizeof(szCol), "price%d", i + 1);
			m_vDailyLimitShop2Goods[i].nPrice = ini.getIntValue("DAILY_LIMIT_SHOP_2", szCol);
			snprintf(szCol, sizeof(szCol), "goods%d", i + 1);
			// goods string would be parsed here
			snprintf(szCol, sizeof(szCol), "broadcast%d", i + 1);
			m_vDailyLimitShop2Broadcast[i] = ini.getIntValue("DAILY_LIMIT_SHOP_2", szCol);
		}
	}

	// EQUIP_UPSTAR_BACK (type 25)
	{
		m_vStartDay[25] = ini.getIntValue("EQUIP_UPSTAR_BACK", "start_day");
		m_vEndDay[25] = ini.getIntValue("EQUIP_UPSTAR_BACK", "end_day");
		int32_t nStarSize = ini.getIntValue("EQUIP_UPSTAR_BACK", "size");
		m_NeedValue.resize(nStarSize > (int32_t)m_NeedValue.size() ? nStarSize : m_NeedValue.size());
		for (int32_t i = 0; i < nStarSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "star%d", i + 1);
			m_NeedValue[i] = ini.getIntValue("EQUIP_UPSTAR_BACK", szCol);
		}
	}

	// RECHARGE_DRAW (type 26)
	{
		m_vStartDay[26] = ini.getIntValue("RECHARGE_DRAW", "start_day");
		m_vEndDay[26] = ini.getIntValue("RECHARGE_DRAW", "end_day");
		int32_t nDrawSize = ini.getIntValue("RECHARGE_DRAW", "size");
		m_nRechargeDrawValue.resize(nDrawSize);
		for (int32_t i = 0; i < nDrawSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "value%d", i + 1);
			m_nRechargeDrawValue[i] = ini.getIntValue("RECHARGE_DRAW", szCol);
		}
	}

	// BUY_GIFT (type 27)
	{
		m_vStartDay[27] = ini.getIntValue("BUY_GIFT", "start_day");
		m_vEndDay[27] = ini.getIntValue("BUY_GIFT", "end_day");
		m_nBuyGiftSize = ini.getIntValue("BUY_GIFT", "size");
		m_vBuyGiftPrice.resize(m_nBuyGiftSize);
		m_vBuyGiftItem.resize(m_nBuyGiftSize);
		m_vBuyGiftBroadcast.resize(m_nBuyGiftSize);
		for (int32_t i = 0; i < m_nBuyGiftSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "price%d", i + 1);
			m_vBuyGiftPrice[i] = ini.getIntValue("BUY_GIFT", szCol);
			snprintf(szCol, sizeof(szCol), "item%d", i + 1);
			std::string strItem = ini.getStrValue("BUY_GIFT", szCol);
			if (!strItem.empty())
				m_vBuyGiftItem[i] = CItemHelper::parseItemString(0, strItem);
			snprintf(szCol, sizeof(szCol), "broadcast%d", i + 1);
			m_vBuyGiftBroadcast[i] = ini.getIntValue("BUY_GIFT", szCol);
		}
	}

	// RECHARGE_BACK (type 28)
	{
		m_vStartDay[28] = ini.getIntValue("RECHARGE_BACK", "start_day");
		m_vEndDay[28] = ini.getIntValue("RECHARGE_BACK", "end_day");
		int32_t nBackSize = ini.getIntValue("RECHARGE_BACK", "size");
		m_vRechargeBackLimit.resize(nBackSize);
		m_vRechargeBackValue.resize(nBackSize);
		for (int32_t i = 0; i < nBackSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "limit%d", i + 1);
			m_vRechargeBackLimit[i] = ini.getIntValue("RECHARGE_BACK", szCol);
			snprintf(szCol, sizeof(szCol), "value%d", i + 1);
			m_vRechargeBackValue[i] = ini.getIntValue("RECHARGE_BACK", szCol);
		}
	}

	// BOSS_SCORE (type 29)
	{
		m_vStartDay[29] = ini.getIntValue("BOSS_SCORE", "start_day");
		m_vEndDay[29] = ini.getIntValue("BOSS_SCORE", "end_day");
	}

	// PET_ILLUSION_ITEM (type 30)
	{
		m_vStartDay[30] = ini.getIntValue("PET_ILLUSION_ITEM", "start_day");
		m_vEndDay[30] = ini.getIntValue("PET_ILLUSION_ITEM", "end_day");
		m_nPetIllusionItemSize = ini.getIntValue("PET_ILLUSION_ITEM", "size");
		m_vPetIllusionItemLimit.resize(m_nPetIllusionItemSize);
		m_vPetIllusionItemReward.resize(m_nPetIllusionItemSize);
		for (int32_t i = 0; i < m_nPetIllusionItemSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "limit%d", i + 1);
			m_vPetIllusionItemLimit[i] = ini.getIntValue("PET_ILLUSION_ITEM", szCol);
			snprintf(szCol, sizeof(szCol), "reward%d", i + 1);
			std::string strReward = ini.getStrValue("PET_ILLUSION_ITEM", szCol);
			if (!strReward.empty())
				m_vPetIllusionItemReward[i] = CItemHelper::parseItemString(0, strReward);
		}
	}

	// RECHARGE_SUM_RANK (type 31)
	{
		m_vStartDay[31] = ini.getIntValue("RECHARGE_SUM_RANK", "start_day");
		m_vEndDay[31] = ini.getIntValue("RECHARGE_SUM_RANK", "end_day");
		m_nRechargeSumRankSize = ini.getIntValue("RECHARGE_SUM_RANK", "size");
		m_vRechargeSumRankMail.resize(m_nRechargeSumRankSize);
		m_vRechargeSumRankReward.resize(m_nRechargeSumRankSize);
		for (int32_t i = 0; i < m_nRechargeSumRankSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "mail%d", i + 1);
			m_vRechargeSumRankMail[i] = ini.getIntValue("RECHARGE_SUM_RANK", szCol);
			snprintf(szCol, sizeof(szCol), "reward%d", i + 1);
			std::string strReward = ini.getStrValue("RECHARGE_SUM_RANK", szCol);
			if (!strReward.empty())
				m_vRechargeSumRankReward[i] = CItemHelper::parseItemString(0, strReward);
		}
	}

	// CROSS_RECHARGE_SUM_RANK (type 32)
	{
		m_vStartDay[32] = ini.getIntValue("CROSS_RECHARGE_SUM_RANK", "start_day");
		m_vEndDay[32] = ini.getIntValue("CROSS_RECHARGE_SUM_RANK", "end_day");
	}

	// CROSS_XIAO_FEI_SUM_RANK (type 33)
	{
		m_vStartDay[33] = ini.getIntValue("CROSS_XIAO_FEI_SUM_RANK", "start_day");
		m_vEndDay[33] = ini.getIntValue("CROSS_XIAO_FEI_SUM_RANK", "end_day");
	}

	// PET_EQUIP_CHOU_JIANG (type 34)
	{
		m_vStartDay[34] = ini.getIntValue("PET_EQUIP_CHOU_JIANG", "start_day");
		m_vEndDay[34] = ini.getIntValue("PET_EQUIP_CHOU_JIANG", "end_day");
	}

	// CROSS_CHOU_JIANG_RANK (type 35)
	{
		m_vStartDay[35] = ini.getIntValue("CROSS_CHOU_JIANG_RANK", "start_day");
		m_vEndDay[35] = ini.getIntValue("CROSS_CHOU_JIANG_RANK", "end_day");
	}

	// SCORE_SHOP (type 38)
	{
		m_vStartDay[38] = ini.getIntValue("SCORE_SHOP", "start_day");
		m_vEndDay[38] = ini.getIntValue("SCORE_SHOP", "end_day");
	}

	// EQUIP_QING_YI (type 36)
	{
		m_vStartDay[36] = ini.getIntValue("EQUIP_QING_YI", "start_day");
		m_vEndDay[36] = ini.getIntValue("EQUIP_QING_YI", "end_day");
		int32_t nQingYiSize = ini.getIntValue("EQUIP_QING_YI", "size");
		m_vEquipQingYiLimit.resize(nQingYiSize);
		m_vEquipQingYiReward.resize(nQingYiSize);
		for (int32_t i = 0; i < nQingYiSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "limit%d", i + 1);
			m_vEquipQingYiLimit[i] = ini.getIntValue("EQUIP_QING_YI", szCol);
			snprintf(szCol, sizeof(szCol), "reward%d", i + 1);
			std::string strReward = ini.getStrValue("EQUIP_QING_YI", szCol);
			if (!strReward.empty())
				m_vEquipQingYiReward[i] = CItemHelper::parseItemString(0, strReward);
		}
	}

	// FRIEND_QING_YI (type 37)
	{
		m_vStartDay[37] = ini.getIntValue("FRIEND_QING_YI", "start_day");
		m_vEndDay[37] = ini.getIntValue("FRIEND_QING_YI", "end_day");
		m_nFriendQingYiSize = ini.getIntValue("FRIEND_QING_YI", "size");
		m_vFriendQingYiLimit.resize(m_nFriendQingYiSize);
		m_vFriendQingYiReward.resize(m_nFriendQingYiSize);
		for (int32_t i = 0; i < m_nFriendQingYiSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "limit%d", i + 1);
			m_vFriendQingYiLimit[i] = ini.getIntValue("FRIEND_QING_YI", szCol);
			snprintf(szCol, sizeof(szCol), "reward%d", i + 1);
			std::string strReward = ini.getStrValue("FRIEND_QING_YI", szCol);
			if (!strReward.empty())
				m_vFriendQingYiReward[i] = CItemHelper::parseItemString(0, strReward);
		}
	}

	// BOSS_SCORE_RANK (type 39)
	{
		m_vStartDay[39] = ini.getIntValue("BOSS_SCORE_RANK", "start_day");
		m_vEndDay[39] = ini.getIntValue("BOSS_SCORE_RANK", "end_day");
		m_BossScoreRankSize = ini.getIntValue("BOSS_SCORE_RANK", "size");
		m_vBossScoreRankMail.resize(m_BossScoreRankSize);
		m_vBossScoreRankReward.resize(m_BossScoreRankSize);
		for (int32_t i = 0; i < m_BossScoreRankSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "mail%d", i + 1);
			m_vBossScoreRankMail[i] = ini.getIntValue("BOSS_SCORE_RANK", szCol);
			snprintf(szCol, sizeof(szCol), "reward%d", i + 1);
			std::string strReward = ini.getStrValue("BOSS_SCORE_RANK", szCol);
			if (!strReward.empty())
				m_vBossScoreRankReward[i] = CItemHelper::parseItemString(0, strReward);
		}
	}

	// BOSS_SCORE_DRAW (type 40)
	{
		m_vStartDay[40] = ini.getIntValue("BOSS_SCORE_DRAW", "start_day");
		m_vEndDay[40] = ini.getIntValue("BOSS_SCORE_DRAW", "end_day");
		int32_t nDrawSize = ini.getIntValue("BOSS_SCORE_DRAW", "size");
		m_vBossScoreDrawReward.resize(nDrawSize);
		for (int32_t i = 0; i < nDrawSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "id%d", i + 1);
			m_vBossScoreDrawReward[i].nId = ini.getIntValue("BOSS_SCORE_DRAW", szCol);
			snprintf(szCol, sizeof(szCol), "rate%d", i + 1);
			m_vBossScoreDrawReward[i].nRate = ini.getIntValue("BOSS_SCORE_DRAW", szCol);
			snprintf(szCol, sizeof(szCol), "count%d", i + 1);
			m_vBossScoreDrawReward[i].nCount = ini.getIntValue("BOSS_SCORE_DRAW", szCol);
		}
	}

	// GOU_WU_CHE (type 41)
	{
		m_vStartDay[41] = ini.getIntValue("GOU_WU_CHE", "start_day");
		m_vEndDay[41] = ini.getIntValue("GOU_WU_CHE", "end_day");
		int32_t nGouWuCheSize = ini.getIntValue("GOU_WU_CHE", "size");
		m_vGouWuCheGiftPrice.resize(nGouWuCheSize);
		m_vGouWuCheItem.resize(nGouWuCheSize);
		for (int32_t i = 0; i < nGouWuCheSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "price%d", i + 1);
			m_vGouWuCheGiftPrice[i] = ini.getIntValue("GOU_WU_CHE", szCol);
			snprintf(szCol, sizeof(szCol), "item%d", i + 1);
			std::string strItem = ini.getStrValue("GOU_WU_CHE", szCol);
			if (!strItem.empty())
				m_vGouWuCheItem[i] = CItemHelper::parseItemString(0, strItem)[0];
		}
	}

	// LIAN_CHONG (type 42)
	{
		m_vStartDay[42] = ini.getIntValue("LIAN_CHONG", "start_day");
		m_vEndDay[42] = ini.getIntValue("LIAN_CHONG", "end_day");
		int32_t nLianSize = ini.getIntValue("LIAN_CHONG", "size");
		m_LianRechargeCfgVt.resize(nLianSize);
		for (int32_t i = 0; i < nLianSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "day%d", i + 1);
			m_LianRechargeCfgVt[i].nDay = ini.getIntValue("LIAN_CHONG", szCol);
			snprintf(szCol, sizeof(szCol), "reward%d", i + 1);
			std::string strReward = ini.getStrValue("LIAN_CHONG", szCol);
			if (!strReward.empty())
				m_LianRechargeCfgVt[i].vItems = CItemHelper::parseItemString(0, strReward);
			snprintf(szCol, sizeof(szCol), "reward_id%d", i + 1);
			m_LianRechargeCfgVt[i].nRewardId = ini.getIntValue("LIAN_CHONG", szCol);
		}
	}

	// MYSTERY_SHOP_AUTO (type 43)
	{
		m_vStartDay[43] = ini.getIntValue("MYSTERY_SHOP_AUTO", "start_day");
		m_vEndDay[43] = ini.getIntValue("MYSTERY_SHOP_AUTO", "end_day");
	}

	m_nDay = (DayTime::dayzero(DayTime::now()) - DayTime::dayzero(m_nStartDay)) / 86400;
}

void CFestivalDoubleEleven::initOpenList(const std::string& str)
{
	memset(m_vOpen, 0, sizeof(m_vOpen));
	StringVector vOpen = StringUtility::split(str, "|");
	for (uint32_t i = 0; i < vOpen.size(); ++i)
	{
		int8_t nType = (int8_t)atoi(vOpen[i].c_str());
		if (nType >= 0 && nType < FAT_MAX)
			m_vOpen[nType] = 1;
	}
}

void CFestivalDoubleEleven::OnDaySwitch()
{
	int32_t nowTime = TIMER.GetNow();
	int32_t dayzero = DayTime::dayzero(nowTime);
	m_nDay = (dayzero - DayTime::dayzero(m_nStartDay)) / 86400;

	if (m_nDay == m_nEndDay)
		hideIcon(m_nIcon);

	if (isOpen(FAT_MO_YU_SHI_JIE))
		BroadCastMoYuShiJieIconState();

	if (isOpen(FAT_DAILY_RECHARGE))
		hideIcon(52);

	if (isOpen(FAT_DRAW_RANK) && m_nDay == m_vEndDay[FAT_DRAW_RANK])
		sendDrawRankMail();

	if (isOpen(FAT_XIAO_FEI_SUM_RANK))
		sendXiaoFeiSumRankMail();

	if (isOpen(FAT_DAILY_XIAO_FEI_RANK))
		sendXiaoFeiRankMail();

	if (isOpen(FAT_RECHARGE_SUM_RANK))
		sendRechargeSumRankMail();

	if (isOpen(FAT_BOSS_SCORE_RANK))
		sendBossScoreRankMail();
}

void CFestivalDoubleEleven::OnNewMinute(int32_t nMinute)
{
	m_nMinute = nMinute;
	HotUpdate();

	if (IsInTime())
	{
		if (IsInTime(FAT_DRAW_RANK))		checkDrawRank();
		if (IsInTime(FAT_WORLD_BOSS))		checkWorldBoss();
		if (IsInTime(FAT_DAILY_XIAO_FEI_RANK))	checkXiaoFeiRank();
		if (IsInTime(FAT_XIAO_FEI_SUM_RANK))	checkXiaoFeiSumRank();
		if (IsInTime(FAT_MYSTERY_SHOP))		checkRefreshMysteryShop();
		if (IsInTime(FAT_RECHARGE_SUM_RANK))	checkRechargeSumRank();
		if (IsInTime(FAT_CROSS_RECHARGE_SUM_RANK))	checkCrossRechargeSumRank();
		if (IsInTime(FAT_CROSS_XIAO_FEI_SUM_RANK))	checkCrossXiaoFeiSumRank();
		if (IsInTime(FAT_CROSS_CHOU_JIANG_RANK))	checkCrossChouJiangRank();
		if (IsInTime(FAT_BOSS_SCORE_RANK))		checkBossScoreRank();
	}
}

void CFestivalDoubleEleven::HotUpdate()
{
	// Read config hot_sign
	Answer::Inifile ini;
	ini.parse("./ServerConfig/Tables/FestivalDoubleEleven.cfg");

	int32_t nHotSign = ini.getIntValue("CONFIG", "hot_sign");
	if (m_nHotSign < nHotSign)
	{
		int32_t nOldVersion = m_nVersion;
		int32_t line = GAME_SERVICE.getLine();
		initCfgData();
		if (m_nVersion != nOldVersion)
		{
			saveVersion(line);
			resetRankData(line);
			resetPlayerData();
		}
	}
}

bool CFestivalDoubleEleven::IsInTime()
{
	return m_nDay >= 0 && m_nDay < m_nEndDay;
}

bool CFestivalDoubleEleven::needShowIcon()
{
	if (GAME_SERVICE.getLine() == 9)
		return false;
	return m_nDay >= 0 && m_nDay < m_nEndDay;
}

bool CFestivalDoubleEleven::IsInTime(FESTIVAL_ACTIVITY_TYPE nType)
{
	return IsOpen(nType) && m_nDay >= m_vStartDay[nType] && m_nDay < m_vEndDay[nType];
}

bool CFestivalDoubleEleven::IsOpen(FESTIVAL_ACTIVITY_TYPE nType)
{
	return IsInTime() && isOpen(nType);
}

bool CFestivalDoubleEleven::isOpen(FESTIVAL_ACTIVITY_TYPE nType)
{
	if (nType < 0 || nType >= FAT_MAX)
		return false;
	return m_vOpen[nType] != 0;
}

bool CFestivalDoubleEleven::CheckDropGroup(int32_t nDropGroup)
{
	return IsInTime() && m_nDropGroup == nDropGroup;
}

int32_t CFestivalDoubleEleven::getLeftTime()
{
	if (m_nDay < 0)
		return 0;
	int32_t nNowTime = TIMER.GetNow();
	return 86400 * (m_nEndDay - m_nDay) - (nNowTime - DayTime::dayzero(nNowTime));
}

void CFestivalDoubleEleven::GetIconState(Player* player, IconStateList* IconList)
{
	if (player && needShowIcon())
	{
		ShowIcon icon;
		getIconState(&icon, player);
		IconList->push_back(icon);
	}
}

void CFestivalDoubleEleven::SendIconState(Player* player)
{
	if (player && needShowIcon())
	{
		ShowIcon stu;
		getIconState(&stu, player);
		NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CC3);
		if (packet)
		{
			packet->writeInt32(stu.nId);
			packet->writeInt8(stu.nState);
			packet->writeInt32(stu.nLeftTime);
			packet->writeInt8(stu.IconLeft);
			packet->writeInt32(stu.IconRight);
			packet->writeInt8(stu.Effects);
			packet->setSize(packet->getWOffset());
			GAME_SERVICE.sendPacketTo(player->getGateIndex(), packet);
		}
	}
}

ShowIcon* CFestivalDoubleEleven::getIconState(ShowIcon* retstr, Player* player)
{
	memset(retstr, 0, sizeof(ShowIcon));
	if (player && needShowIcon())
	{
		retstr->nId = m_nIcon;
		retstr->nState = 2;
		retstr->nLeftTime = getLeftTime();

		if (canGetLandGift(player))			retstr->IconLeft = 1;
		if (canGetDrawGift(player))			retstr->IconRight += 1;
		if (canGetLandSumGift(player))		retstr->IconRight += 1;
		if (canGetOnlineGift(player))		retstr->IconRight += 1;
		if (canGetWishGift(player))			retstr->IconRight += 1;
		if (CanUseXiaoFeiDraw(player))		retstr->IconRight += 1;
		if (CanUseRechargeDraw(player))		retstr->IconRight += 1;
		if (canGetFaBaoBack(player))		retstr->IconRight += 1;
		if (canGetEquipUpStarBack(player))	retstr->IconRight += 1;
		if (canGetRechargeBack(player))		retstr->IconRight += 1;
		if (IsOpen(FAT_HUO_YUE_DU_SUM))		retstr->IconRight += 1;
		if (IsInTime(FAT_DAILY_RECHARGE_2))	retstr->IconRight += 1;
		if (IsOpen(FAT_RECHARGE_SUM_GIFT))	retstr->IconRight += 1;
		if (IsOpen(FAT_XIAO_FEI_SUM_GIFT))	retstr->IconRight += GetXiaoFeiSumGiftCount(player);
		if (IsOpen(FAT_PET_ILLUSION_ITEM))	retstr->IconRight += 1;
		if (IsOpen(FAT_FRIEND_QING_YI))		retstr->IconRight += 1;
		if (IsOpen(FAT_BOSS_SCORE_DRAW))	retstr->IconRight += 1;
		if (IsInTime(FAT_LIAN_CHONG))		retstr->IconRight += 1;
	}
	return retstr;
}

void CFestivalDoubleEleven::hideIcon(int32_t nIconId)
{
	if (GAME_SERVICE.getLine() != 1)
		return;

	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CC3);
	if (packet)
	{
		packet->writeInt32(nIconId);
		packet->writeInt8(4);
		packet->writeInt32(0);
		packet->writeInt8(0);
		packet->writeInt32(0);
		packet->writeInt8(0);
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.worldBroadcast(packet);
	}
}

void CFestivalDoubleEleven::SendActivityInfo(Player* player)
{
	if (!player)
		return;

	int8_t connid = 0;
	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2EB8);
	if (!packet)
		return;

	int8_t nCount = 0;
	uint32_t oldOffset = packet->getWOffset();
	packet->writeInt8(0);

	// FAT_LAND (type 0)
	if (isOpen(FAT_LAND))
	{
		packet->writeInt8(1);
		packet->writeInt32(m_nDay);
		packet->writeInt8(!canGetLandGift(player));
		++nCount;
	}

	// FAT_DAILY_RECHARGE (type 1)
	if (isOpen(FAT_DAILY_RECHARGE))
	{
		packet->writeInt8(2);
		packet->writeInt32(player->GetTodayPayGold());
		++nCount;
	}

	// FAT_DRAW (type 2)
	if (isOpen(FAT_DRAW))
	{
		packet->writeInt8(3);
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(1601));
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(1602));
		++nCount;
	}

	// FAT_DRAW_RANK (type 3)
	if (isOpen(FAT_DRAW_RANK))
	{
		checkDrawRankInvalid(connid);
		packet->writeInt8(4);
		packet->writeInt8((int8_t)m_nDrawRankSize);
		{
			MutexGuard lock(m_lock);
			std::vector<FestivalRank>& v = m_mDrawRank[connid];
			for (int32_t i = 0; i < m_nDrawRankSize && i < (int32_t)v.size(); ++i)
			{
				packet->writeUTF8(v[i].strName.c_str());
				packet->writeInt32(v[i].nValue);
			}
		}
		++nCount;
	}

	// FAT_HUO_YUE_DU_SUM (type 6)
	if (isOpen(FAT_HUO_YUE_DU_SUM))
	{
		packet->writeInt8(7);
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(1603));
		int32_t nRewardSign = player->GetOperateLimit().GetLimitCount(1604);
		packet->writeInt8((int8_t)m_nHuoYueDuSumSize);
		for (int32_t i = 0; i < m_nHuoYueDuSumSize; ++i)
			packet->writeInt8((nRewardSign >> i) & 1);
		++nCount;
	}

	// FAT_DAILY_LIMIT_SHOP (type 7)
	if (isOpen(FAT_DAILY_LIMIT_SHOP))
	{
		packet->writeInt8(8);
		int32_t limit = player->GetOperateLimit().GetLimitCount(2034);
		packet->writeInt32(m_nDailyLimitShopLimit - limit);
		++nCount;
	}

	// FAT_LAND_SUM (type 8)
	if (isOpen(FAT_LAND_SUM))
	{
		packet->writeInt8(9);
		int32_t nLandDays = getLandSum(player);
		packet->writeInt8((int8_t)nLandDays);
		int32_t nRecord = player->GetOperateLimit().GetLimitCount(1614);
		for (int32_t i = 0; i < nLandDays; ++i)
			packet->writeInt8((nRecord >> i) & 1);
		++nCount;
	}

	// FAT_ONLINE_TIME (type 9)
	if (isOpen(FAT_ONLINE_TIME))
	{
		packet->writeInt8(10);
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(2037));
		packet->writeInt32(GetOnlineTimeRewardTime(player));
		packet->writeInt8(m_nMinute >= m_nOnlineStartMinute && m_nMinute < m_nOnlineEndMinute);
		++nCount;
	}

	// FAT_BUY_PET_GIFT (type 22)
	if (isOpen(FAT_BUY_PET_GIFT))
	{
		packet->writeInt8(23);
		packet->writeInt8((int8_t)player->getRecord(1631));
		++nCount;
	}

	// FAT_BEST_WISH (type 10)
	if (isOpen(FAT_BEST_WISH))
	{
		packet->writeInt8(11);
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(2026));
		packet->writeInt8(!canGetWishGift(player));
		// Player::GetCharWish->AppendWishInfo
		++nCount;
	}

	// FAT_DAILY_XIAO_FEI_RANK (type 11)
	if (isOpen(FAT_DAILY_XIAO_FEI_RANK))
	{
		checkXiaoFeiRankInvalid(connid);
		packet->writeInt8(12);
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(2031));
		packet->writeInt8((int8_t)m_nDay);
		packet->writeInt8((int8_t)m_nEndDay);
		for (int32_t day = 0; day < m_nEndDay; ++day)
		{
			packet->writeInt8((int8_t)m_nXiaoFeiRankSize);
			MutexGuard lock(m_lock);
			std::vector<std::vector<FestivalRank>>& dayRanks = m_mXiaoFeiRank[connid];
			if (day < (int32_t)dayRanks.size())
			{
				for (int32_t j = 0; j < m_nXiaoFeiRankSize && j < (int32_t)dayRanks[day].size(); ++j)
				{
					packet->writeUTF8(dayRanks[day][j].strName.c_str());
					packet->writeInt32(dayRanks[day][j].nValue);
				}
			}
		}
		++nCount;
	}

	// FAT_XIAO_FEI_DRAW (type 12)
	if (isOpen(FAT_XIAO_FEI_DRAW))
	{
		packet->writeInt8(13);
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(1605));
		++nCount;
	}

	// FAT_GOU_WU_CHE (type 41)
	if (isOpen(FAT_GOU_WU_CHE))
	{
		packet->writeInt8(45);
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(2112));
		++nCount;
	}

	// FAT_DAILY_RECHARGE_2 (type 14)
	if (isOpen(FAT_DAILY_RECHARGE_2))
	{
		packet->writeInt8(15);
		packet->writeInt32(player->GetTodayPayGold());
		int32_t nRewardSign = player->GetOperateLimit().GetLimitCount(2033);
		packet->writeInt8((int8_t)m_nDailyRechargeSize);
		for (int32_t i = 0; i < m_nDailyRechargeSize; ++i)
			packet->writeInt8((nRewardSign >> i) & 1);
		++nCount;
	}

	// FAT_XIAO_FEI_SUM_RANK (type 15)
	if (isOpen(FAT_XIAO_FEI_SUM_RANK))
	{
		checkXiaoFeiSumRankInvalid(connid);
		packet->writeInt8(16);
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(1605));
		packet->writeInt8((int8_t)m_nXiaoFeiSumRankSize);
		{
			MutexGuard lock(m_lock);
			std::vector<FestivalRank>& v = m_mXiaoFeiSumRank[connid];
			for (int32_t i = 0; i < m_nXiaoFeiSumRankSize && i < (int32_t)v.size(); ++i)
			{
				packet->writeInt64(v[i].Cid);
				packet->writeUTF8(v[i].strName.c_str());
				packet->writeInt32(v[i].nValue);
			}
		}
		++nCount;
	}

	// FAT_GIFT_SHOP (type 16)
	if (isOpen(FAT_GIFT_SHOP))
	{
		packet->writeInt8(17);
		int32_t nDay = m_nDay >= m_vEndDay[FAT_GIFT_SHOP] ? m_vEndDay[FAT_GIFT_SHOP] : m_nDay;
		int32_t nSize = nDay - m_vStartDay[FAT_GIFT_SHOP];
		int32_t nRecord = player->GetOperateLimit().GetLimitCount(1607);
		packet->writeInt8((int8_t)nSize);
		for (int32_t i = 0; i <= nSize; ++i)
		{
			packet->writeInt8((int8_t)m_nGiftShopSize);
			for (int32_t j = 0; j < m_nGiftShopSize; ++j)
				packet->writeInt8((nRecord >> (j + i * m_nGiftShopSize)) & 1);
		}
		++nCount;
	}

	// FAT_FA_BAO_BACK (type 18)
	if (isOpen(FAT_FA_BAO_BACK))
	{
		packet->writeInt8(19);
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(1630));
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(1629));
		++nCount;
	}

	// FAT_RECHARGE_SUM_GIFT (type 20)
	if (isOpen(FAT_RECHARGE_SUM_GIFT))
	{
		packet->writeInt8(21);
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(1615));
		int32_t nRewardSign = player->GetOperateLimit().GetLimitCount(1616);
		packet->writeInt8((int8_t)m_nRechargeSumSize);
		for (int32_t i = 0; i < m_nRechargeSumSize; ++i)
			packet->writeInt8((nRewardSign >> i) & 1);
		++nCount;
	}

	// FAT_XIAO_FEI_SUM_GIFT (type 21)
	if (isOpen(FAT_XIAO_FEI_SUM_GIFT))
	{
		packet->writeInt8(22);
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(1605));
		int32_t nRewardSign = player->GetOperateLimit().GetLimitCount(1606);
		packet->writeInt8((int8_t)m_nXiaoFeiSumSize);
		for (int32_t i = 0; i < m_nXiaoFeiSumSize; ++i)
			packet->writeInt8((nRewardSign >> i) & 1);
		++nCount;
	}

	// FAT_DAILY_LIMIT_SHOP_2 (type 24)
	if (isOpen(FAT_DAILY_LIMIT_SHOP_2))
	{
		packet->writeInt8(25);
		int32_t nRecord = player->GetOperateLimit().GetLimitCount(2035);
		packet->writeInt8((int8_t)m_nDailyLimitShop2Size);
		for (int32_t i = 0; i < m_nDailyLimitShop2Size; ++i)
			packet->writeInt8((nRecord >> i) & 1);
		++nCount;
	}

	// FAT_EQUIP_UPSTAR_BACK (type 25)
	if (isOpen(FAT_EQUIP_UPSTAR_BACK))
	{
		packet->writeInt8(26);
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(1617));
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(1618));
		++nCount;
	}

	// FAT_RECHARGE_DRAW (type 26)
	if (isOpen(FAT_RECHARGE_DRAW))
	{
		packet->writeInt8(27);
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(1615));
		++nCount;
	}

	// FAT_BUY_GIFT (type 27)
	if (isOpen(FAT_BUY_GIFT))
	{
		packet->writeInt8(28);
		int32_t nRecord = player->GetOperateLimit().GetLimitCount(1619);
		packet->writeInt8((int8_t)m_nBuyGiftSize);
		for (int32_t i = 0; i < m_nBuyGiftSize; ++i)
			packet->writeInt8((nRecord >> i) & 1);
		++nCount;
	}

	// FAT_RECHARGE_BACK (type 28)
	if (isOpen(FAT_RECHARGE_BACK))
	{
		packet->writeInt8(29);
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(1620));
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(1621));
		++nCount;
	}

	// FAT_PET_ILLUSION_ITEM (type 30)
	if (isOpen(FAT_PET_ILLUSION_ITEM))
	{
		packet->writeInt8(31);
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(1622));
		int32_t nRewardSign = player->GetOperateLimit().GetLimitCount(1623);
		packet->writeInt8((int8_t)m_nPetIllusionItemSize);
		for (int32_t i = 0; i < m_nPetIllusionItemSize; ++i)
			packet->writeInt8((nRewardSign >> i) & 1);
		++nCount;
	}

	// FAT_RECHARGE_SUM_RANK (type 31)
	if (isOpen(FAT_RECHARGE_SUM_RANK))
	{
		checkRechargeSumRankInvalid(connid);
		packet->writeInt8(32);
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(1615));
		packet->writeInt8((int8_t)m_nRechargeSumRankSize);
		{
			MutexGuard lock(m_lock);
			std::vector<FestivalRank>& v = m_mRechargeSumRank[connid];
			for (int32_t i = 0; i < m_nRechargeSumRankSize && i < (int32_t)v.size(); ++i)
			{
				packet->writeUTF8(v[i].strName.c_str());
				packet->writeInt32(v[i].nValue);
			}
		}
		++nCount;
	}

	// FAT_BOSS_SCORE_RANK (type 39)
	if (isOpen(FAT_BOSS_SCORE_RANK))
	{
		checkBossScoreRankInvalid(connid);
		packet->writeInt8(43);
		packet->writeInt8((int8_t)m_BossScoreRankSize);
		{
			MutexGuard lock(m_lock);
			std::vector<FestivalRank>& v = m_mBossScoreRank[connid];
			for (int32_t i = 0; i < m_BossScoreRankSize && i < (int32_t)v.size(); ++i)
			{
				packet->writeInt64(v[i].Cid);
				packet->writeUTF8(v[i].strName.c_str());
				packet->writeInt32(v[i].nValue);
			}
		}
		++nCount;
	}

	// FAT_FRIEND_QING_YI (type 37)
	if (isOpen(FAT_FRIEND_QING_YI))
	{
		packet->writeInt8(39);
		packet->writeInt32(player->GetOperateLimit().GetLimitCount(1624));
		int32_t nRecord = player->GetOperateLimit().GetLimitCount(1625);
		packet->writeInt8((int8_t)m_nFriendQingYiSize);
		for (int32_t i = 0; i < m_nFriendQingYiSize; ++i)
			packet->writeInt8((nRecord >> i) & 1);
		++nCount;
	}

	// FAT_BOSS_SCORE_DRAW (type 40)
	if (isOpen(FAT_BOSS_SCORE_DRAW))
	{
		packet->writeInt8(44);
		packet->writeInt32(player->GetTodayPayGold());
		packet->writeInt32(player->getRecord(2095));
		packet->writeInt32(player->getRecord(1628));
		packet->writeInt32(player->getRecord(2096));
		++nCount;
	}

	// Seek back to write count
	uint32_t newOffset = packet->getWOffset();
	packet->setWOffset(oldOffset);
	packet->writeInt8(nCount);
	packet->setWOffset(newOffset);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(player->getGateIndex(), packet);
}

// ===== World Boss =====

bool CFestivalDoubleEleven::IsInWorldBossTime()
{
	return GAME_SERVICE.getLine() != 9 && IsInTime(FAT_WORLD_BOSS);
}

void CFestivalDoubleEleven::GetWorldBossIconState(IconStateList* IconList)
{
	if (IsInWorldBossTime())
	{
		ShowIcon icon;
		getWorldBossIconState(&icon);
		IconList->push_back(icon);
	}
}

void CFestivalDoubleEleven::SendWorldBossIconState()
{
	if (GAME_SERVICE.getLine() == 1)
	{
		ShowIcon stu;
		getWorldBossIconState(&stu);
		NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CC3);
		if (packet)
		{
			packet->writeInt32(stu.nId);
			packet->writeInt8(stu.nState);
			packet->writeInt32(stu.nLeftTime);
			packet->writeInt8(stu.IconLeft);
			packet->writeInt32(stu.IconRight);
			packet->writeInt8(stu.Effects);
			packet->setSize(packet->getWOffset());
			GAME_SERVICE.worldBroadcast(packet);
		}
	}
}

ShowIcon* CFestivalDoubleEleven::getWorldBossIconState(ShowIcon* retstr)
{
	memset(retstr, 0, sizeof(ShowIcon));
	if (IsInWorldBossTime() && !m_bDie)
	{
		retstr->nId = 66;
		retstr->nState = 2;
	}
	return retstr;
}

void CFestivalDoubleEleven::checkWorldBoss()
{
	if (GAME_SERVICE.getLine() == 9 || !IsInTime(FAT_WORLD_BOSS) || m_vWorldBossMinute.empty())
		return;
	if (GAME_SERVICE.getLine() != 1)
		return;

	for (uint32_t i = 0; i < m_vWorldBossMinute.size(); ++i)
	{
		if (m_nMinute == m_vWorldBossMinute[i].nStartTime)
		{
			CfgMonster* pMonster = CFG_DATA.getMonster(m_Mid);
			if (!pMonster)
				return;

			CfgMapMonster MapMonster;
			memset(&MapMonster, 0, sizeof(MapMonster));
			MapMonster.id = 0;
			MapMonster.mapid = m_MapId;
			MapMonster.x = m_X;
			MapMonster.y = m_Y;

			Monster* monster = new Monster;
			if (monster && pMonster)
			{
				monster->init(*pMonster, MapMonster, NULL);
				Map* pMap = MAP_MANAGER.GetMap(m_MapId);
				if (pMap)
				{
					pMap->addMonster(monster, MapMonster.x, MapMonster.y);
					m_bDie = 0;
				}
			}
			broadcastWorldBossStart();
			SendWorldBossIconState();
		}
		else if (m_nMinute == m_vWorldBossMinute[i].nEndTime && !m_bDie)
		{
			m_bDie = 1;
			broadcastWorldBossEnd();
			SendWorldBossIconState();
		}
	}
}

void CFestivalDoubleEleven::BossDie(int32_t MonsterId, std::string* p_name, CharId_t Cid)
{
	if (m_MonsterId == MonsterId)
	{
		BroadcastWorldBossKilled(p_name, Cid);
		SendWorldBossIconState();
	}
}

void CFestivalDoubleEleven::BroadcastWorldBossKilled(const std::string* name, CharId_t cid)
{
	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CD6);
	if (packet)
	{
		packet->writeInt32(m_nBossKilledBroadcast);
		packet->writeInt64(cid);
		packet->writeUTF8(name->c_str());
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.worldBroadcast(packet);
	}
}

void CFestivalDoubleEleven::broadcastWorldBossStart()
{
	if (GAME_SERVICE.getLine() == 9)
		return;
	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CD6);
	if (packet)
	{
		packet->writeInt32(m_nBossStartBroadcast);
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.worldBroadcast(packet);
	}
}

void CFestivalDoubleEleven::broadcastWorldBossEnd()
{
	if (GAME_SERVICE.getLine() == 9)
		return;
	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CD6);
	if (packet)
	{
		packet->writeInt32(m_nBossEndBroadcast);
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.worldBroadcast(packet);
	}
}

// ===== MoYuShiJie =====

ShowIcon* CFestivalDoubleEleven::getMoYuShiJieIconState(ShowIcon* retstr)
{
	memset(retstr, 0, sizeof(ShowIcon));
	if (IsInTime(FAT_MO_YU_SHI_JIE))
	{
		retstr->nId = 67;
		retstr->nState = 2;
	}
	return retstr;
}

void CFestivalDoubleEleven::GetMoYuShiJieIconState(IconStateList* IconList)
{
	if (IsInTime(FAT_MO_YU_SHI_JIE))
	{
		ShowIcon icon;
		getMoYuShiJieIconState(&icon);
		IconList->push_back(icon);
	}
}

void CFestivalDoubleEleven::BroadCastMoYuShiJieIconState()
{
	ShowIcon stu;
	getMoYuShiJieIconState(&stu);
	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CC3);
	if (packet)
	{
		packet->writeInt32(stu.nId);
		packet->writeInt8(stu.nState);
		packet->writeInt32(stu.nLeftTime);
		packet->writeInt8(stu.IconLeft);
		packet->writeInt32(stu.IconRight);
		packet->writeInt8(stu.Effects);
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.worldBroadcast(packet);
	}
}

void CFestivalDoubleEleven::AddMoYuShiJieDrop(int32_t sequence, MemChrBag* vItem)
{
	if (IsInTime(FAT_MO_YU_SHI_JIE))
	{
		if (sequence >= 0 && sequence < (int32_t)m_vMoYuShiJieDrop.size())
		{
			// Process drop
		}
	}
}

void CFestivalDoubleEleven::AddMoYuShiJieDrop(int32_t sequence, Player* player, MemChrBag* vItem)
{
	if (IsInTime(FAT_MO_YU_SHI_JIE) && player && vItem)
	{
		player->GetBag().AddItem(*vItem, IACR_NONE);
	}
}

// ===== Gift Methods =====

int32_t CFestivalDoubleEleven::GetLandGift(Player* player)
{
	if (!player || !canGetLandGift(player))
		return 10002;
	for (uint32_t i = 0; i < m_vLandGift.size(); ++i)
	{
		for (uint32_t j = 0; j < m_vLandGift[i].size(); ++j)
		{
			if (!player->GetBag().AddItem(m_vLandGift[i][j], IACR_NONE))
				return 10002;
		}
	}
	player->updateRecord(18001, 1);
	SendIconState(player);
	return 0;
}

int32_t CFestivalDoubleEleven::GetLandSumGift(Player* player, int8_t nIndex)
{
	if (!player || !IsOpen(FAT_LAND_SUM))
		return 10002;
	if (nIndex < 0 || nIndex >= (int32_t)m_vLandSumGift.size() || nIndex > getLandSum(player))
		return 10002;

	for (uint32_t i = 0; i < m_vLandSumGift[nIndex].size(); ++i)
	{
		if (!player->GetBag().AddItem(m_vLandSumGift[nIndex][i], IACR_NONE))
			return 10002;
	}
	player->updateRecord(nIndex + 18002, 1);
	SendIconState(player);
	return 0;
}

int32_t CFestivalDoubleEleven::GetDrawGift(Player* player)
{
	if (!player || !canGetDrawGift(player))
		return 10002;
	for (uint32_t i = 0; i < m_vDrawLoopReward.size(); ++i)
	{
		if (!player->GetBag().AddItem(m_vDrawLoopReward[i], IACR_NONE))
			return 10002;
	}
	player->updateRecord(19001, 0);
	player->updateRecord(19002, 0);
	SendIconState(player);
	return 0;
}

int32_t CFestivalDoubleEleven::GetOnlineGift(Player* player)
{
	if (!player || !canGetOnlineGift(player))
		return 10002;

	int32_t nRewardTimes = player->GetOperateLimit().GetLimitCount(2094);
	if (nRewardTimes >= (int32_t)m_vOnlineReward.size())
		return 10002;

	if (!player->GetBag().AddItem(m_vOnlineReward[nRewardTimes], IACR_NONE))
		return 10002;

	player->GetOperateLimit().AddLimitCount(2094, 1);
	SendIconState(player);
	return 0;
}

int32_t CFestivalDoubleEleven::GetWishGift(Player* player)
{
	if (!player || !canGetWishGift(player))
		return 10002;
	for (uint32_t i = 0; i < m_vWishReward.size(); ++i)
	{
		if (!player->GetBag().AddItem(m_vWishReward[i], IACR_NONE))
			return 10002;
	}
	player->updateRecord(18003, 1);
	SendIconState(player);
	return 0;
}

int32_t CFestivalDoubleEleven::GetHuoYueDuSumGift(Player* player, int8_t nIndex)
{
	if (!player || !IsOpen(FAT_HUO_YUE_DU_SUM))
		return 10002;
	if (nIndex < 0 || nIndex >= (int32_t)m_vHuoYueDuSumReward.size())
		return 10002;
	if (player->getRecord(nIndex + 19101) > 0)
		return 10002;
	if (player->getRecord(19100) < m_vHuoYueDuSumLimit[nIndex])
		return 10002;

	for (uint32_t i = 0; i < m_vHuoYueDuSumReward[nIndex].size(); ++i)
	{
		if (!player->GetBag().AddItem(m_vHuoYueDuSumReward[nIndex][i], IACR_NONE))
			return 10002;
	}
	player->updateRecord(nIndex + 19101, 1);
	SendIconState(player);
	return 0;
}

int32_t CFestivalDoubleEleven::GetDailyRechargeGift(Player* player, int8_t nIndex)
{
	if (!player || !IsInTime(FAT_DAILY_RECHARGE_2))
		return 10002;
	if (nIndex < 0 || nIndex >= (int32_t)m_vDailyRechargeLimit.size())
		return 10002;
	if (player->getRecord(nIndex + 19102) > 0)
		return 10002;
	if (player->GetTodayPayGold() < m_vDailyRechargeLimit[nIndex])
		return 10002;

	for (uint32_t i = 0; i < m_vDailyRechargeReward[nIndex].size(); ++i)
	{
		if (!player->GetBag().AddItem(m_vDailyRechargeReward[nIndex][i], IACR_NONE))
			return 10002;
	}
	player->updateRecord(nIndex + 19102, 1);
	SendIconState(player);

	if (m_vDailyRechargeBroad.size() > (uint32_t)nIndex && m_vDailyRechargeBroad[nIndex] > 0)
	{
		std::string name = player->getName();
		CharId_t Cid = player->getCid();
		broadcastPlayerAction(player, m_vDailyRechargeBroad[nIndex], Cid, name);
	}
	return 0;
}

int32_t CFestivalDoubleEleven::GetRechargeSumGift(Player* player, int8_t nIndex)
{
	if (!player || !IsInTime(FAT_RECHARGE_SUM_GIFT))
		return 10002;
	if (nIndex < 0 || nIndex >= (int32_t)m_vRechargeSumLimit.size())
		return 10002;
	if (player->getRecord(nIndex + 19103) > 0)
		return 10002;
	if (player->getRecord(19104) < m_vRechargeSumLimit[nIndex])
		return 10002;

	for (uint32_t i = 0; i < m_vRechargeSumReward[nIndex].size(); ++i)
	{
		if (!player->GetBag().AddItem(m_vRechargeSumReward[nIndex][i], IACR_NONE))
			return 10002;
	}
	player->updateRecord(nIndex + 19103, 1);
	SendIconState(player);

	if (m_vRechargeSumBroadcast.size() > (uint32_t)nIndex && m_vRechargeSumBroadcast[nIndex] > 0)
	{
		std::string name = player->getName();
		CharId_t Cid = player->getCid();
		broadcastPlayerAction(player, m_vRechargeSumBroadcast[nIndex], Cid, name);
	}
	return 0;
}

int32_t CFestivalDoubleEleven::GetPetIllusionItemGift(Player* player, int8_t nIndex)
{
	if (!player || !IsInTime(FAT_PET_ILLUSION_ITEM))
		return 10002;
	if (nIndex < 0 || nIndex >= (int32_t)m_vPetIllusionItemLimit.size())
		return 10002;
	if (player->getRecord(nIndex + 19105) > 0)
		return 10002;

	for (uint32_t i = 0; i < m_vPetIllusionItemReward[nIndex].size(); ++i)
	{
		if (!player->GetBag().AddItem(m_vPetIllusionItemReward[nIndex][i], IACR_NONE))
			return 10002;
	}
	player->updateRecord(nIndex + 19105, 1);
	SendIconState(player);
	return 0;
}

int32_t CFestivalDoubleEleven::GetXiaoFeiSumGift(Player* player, int32_t nId)
{
	if (!player || !IsInTime(FAT_XIAO_FEI_SUM_GIFT))
		return 10002;

	int32_t nIndex = -1;
	for (uint32_t i = 0; i < m_vXiaoFeiSumLimit.size(); ++i)
	{
		if (m_vXiaoFeiSumLimit[i] == nId) { nIndex = i; break; }
	}
	if (nIndex < 0) return 10002;
	if (player->getRecord(nIndex + 19106) > 0) return 10002;
	if (player->getRecord(19104) < nId) return 10002;

	for (uint32_t i = 0; i < m_vXiaoFeiSumReward[nIndex].size(); ++i)
	{
		if (!player->GetBag().AddItem(m_vXiaoFeiSumReward[nIndex][i], IACR_NONE))
			return 10002;
	}
	player->updateRecord(nIndex + 19106, 1);
	SendIconState(player);

	if (m_vXiaoFeiSumBroadcast.size() > (uint32_t)nIndex && m_vXiaoFeiSumBroadcast[nIndex] > 0)
	{
		std::string name = player->getName();
		CharId_t Cid = player->getCid();
		broadcastPlayerAction(player, m_vXiaoFeiSumBroadcast[nIndex], Cid, name);
	}
	return 0;
}

void CFestivalDoubleEleven::GetLianRechargeReward(Player* player, int8_t nType, int8_t nIndex)
{
	if (!player || !IsInTime(FAT_LIAN_CHONG))
		return;
	if (nIndex < 0 || nIndex >= (int32_t)m_LianRechargeCfgVt.size())
		return;
	player->updateRecord(nIndex + 19107, 1);
	GongGao(3012, player);
	SendIconState(player);
}

void CFestivalDoubleEleven::GongGao(int32_t GongGaoId, Player* player)
{
	if (!player) return;
	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CD6);
	if (packet)
	{
		packet->writeInt32(GongGaoId);
		packet->writeInt64(player->getCid());
		std::string name = player->getName();
		packet->writeUTF8(name.c_str());
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.sendPacketTo(player->getGateIndex(), packet);
	}
}

// ===== Record Methods =====

void CFestivalDoubleEleven::AddDrawTimes(Player* player, int32_t nCount)
{
	if (!player) return;
	if (IsInTime(FAT_DRAW) || IsInTime(FAT_DRAW_RANK))
	{
		int32_t nOldTimes = player->getRecord(19001);
		player->updateRecord(19001, nOldTimes + nCount);
		if (IsInTime(FAT_DRAW) && canGetDrawGift(player))
			SendIconState(player);
		if (IsInTime(FAT_CROSS_CHOU_JIANG_RANK))
			updateCrossChouJiangRank(player);
		if (IsInTime(FAT_DRAW_RANK))
			checkDrawRankInvalid(0);
	}
}

void CFestivalDoubleEleven::AddHuoYueDu(Player* player, int32_t nValue)
{
	if (!player) return;
	if (IsInTime(FAT_HUO_YUE_DU_SUM))
	{
		int32_t nOld = player->getRecord(19100);
		player->updateRecord(19100, nOld + nValue);
		SendIconState(player);
	}
}

void CFestivalDoubleEleven::AddOnlineRecord(Player* player)
{
	if (!player || !IsInTime(FAT_ONLINE_TIME))
		return;

	int32_t nOnlineTime = player->GetOperateLimit().GetLimitCount(2093);
	int32_t nRewardTimes = player->GetOperateLimit().GetLimitCount(2094);
	if ((int32_t)m_vOnlineReward.size() > nRewardTimes)
	{
		player->GetOperateLimit().AddLimitCount(2093, 1);
	}
}

void CFestivalDoubleEleven::AddXiaoFeiRecord(Player* player, int32_t nCount)
{
	if (!player || !IsInTime()) return;
	int32_t nOldCount = GetXiaoFeiSumGiftCount(player);
	int32_t nTotalConsume = player->getRecord(19104);
	player->updateRecord(19104, nTotalConsume + nCount);
	if (IsInTime(FAT_XIAO_FEI_DRAW) && CanUseXiaoFeiDraw(player))
		SendIconState(player);
	if (IsInTime(FAT_XIAO_FEI_SUM_GIFT) && GetXiaoFeiSumGiftCount(player) != nOldCount)
		SendIconState(player);
}

void CFestivalDoubleEleven::AddRechargeRecord(Player* player, int32_t nCount)
{
	if (!player || !IsInTime()) return;
	if (IsInTime(FAT_RECHARGE_SUM_RANK)) updateRechargeSumRank(player);
	if (IsInTime(FAT_CROSS_RECHARGE_SUM_RANK)) updateCrossRechargeSumRank(player);
}

void CFestivalDoubleEleven::AddPetIllusionItemRecord(Player* player, int32_t nCount)
{
	if (!player) return;
	if (IsInTime(FAT_PET_ILLUSION_ITEM))
	{
		int32_t nOld = player->getRecord(19105);
		player->updateRecord(19105, nOld + nCount);
		SendIconState(player);
	}
}

void CFestivalDoubleEleven::AddFriendQingYi(Player* player, int32_t nValue)
{
	if (!player) return;
	if (IsInTime(FAT_FRIEND_QING_YI))
	{
		int32_t nOld = player->getRecord(19108);
		player->updateRecord(19108, nOld + nValue);
		SendIconState(player);
	}
}

void CFestivalDoubleEleven::AddEquipUpStarBack(Player* player, int32_t nCount)
{
	if (!player || !IsInTime(FAT_EQUIP_UPSTAR_BACK)) return;
	player->updateRecord(19109, nCount);
	SendIconState(player);
}

void CFestivalDoubleEleven::AddFaBaoValue(Player* player, int8_t nType, int32_t Values)
{
	if (Values <= 0 || !player || !IsInTime(FAT_FA_BAO_BACK)) return;
	bool CanOldGet = canGetFaBaoBack(player);
	player->updateRecord(nType + 19110, Values);
	bool CanNewGet = canGetFaBaoBack(player);
	if (CanNewGet && !CanOldGet)
		SendIconState(player);
}

void CFestivalDoubleEleven::OnRecharge(Player* player, int32_t nCount)
{
	if (IsInTime(FAT_LIAN_CHONG))
		SendIconState(player);
}

// ===== Rank Methods =====

void CFestivalDoubleEleven::UpdateRank(Player* player, CharId_t Id, int32_t policy, int8_t type, int32_t nDay, const std::string* info)
{
	switch (type)
	{
	case 1:	updateDrawRank(player); break;
	case 2:	updateXiaoFeiRank(player); break;
	case 3:	updateXiaoFeiSumRank(player); break;
	case 4:	updateRechargeSumRank(player); break;
	case 5:	updateCrossRechargeSumRank(player); break;
	case 6:	updateCrossXiaoFeiSumRank(player); break;
	case 7:	updateCrossChouJiangRank(player); break;
	case 8:	updateBossScoreRank(player); break;
	default: break;
	}
}

void CFestivalDoubleEleven::UpdateXiaoFeiRank(Player* player)
{
	if (IsInTime(FAT_DAILY_XIAO_FEI_RANK)) updateXiaoFeiRank(player);
	if (IsInTime(FAT_XIAO_FEI_SUM_RANK)) updateXiaoFeiSumRank(player);
	if (IsInTime(FAT_CROSS_XIAO_FEI_SUM_RANK)) updateCrossXiaoFeiSumRank(player);
}

void CFestivalDoubleEleven::UpdateMonsterScore(Player* player, int32_t Score)
{
	if (!player) return;
	if (IsInTime())
	{
		if (IsInTime(FAT_BOSS_SCORE_RANK)) updateBossScoreRank(player);
		if (IsInTime(FAT_BOSS_SCORE_DRAW)) SendIconState(player);
	}
}

// ===== Check Methods =====

bool CFestivalDoubleEleven::CanUseXiaoFeiDraw(Player* player)
{
	if (!IsOpen(FAT_XIAO_FEI_DRAW) || !player) return false;
	int32_t nConsume = player->getRecord(19104);
	for (uint32_t i = 0; i < m_nXiaoFeiDrawValue.size(); ++i)
	{
		if (nConsume >= m_nXiaoFeiDrawValue[i] && player->getRecord(i + 19111) <= 0)
			return true;
	}
	return false;
}

bool CFestivalDoubleEleven::CanUseRechargeDraw(Player* player)
{
	if (!IsOpen(FAT_RECHARGE_DRAW) || !player) return false;
	int32_t nRecharge = player->getRecord(19112);
	for (uint32_t i = 0; i < m_nRechargeDrawValue.size(); ++i)
	{
		if (nRecharge >= m_nRechargeDrawValue[i] && player->getRecord(i + 19113) <= 0)
			return true;
	}
	return false;
}

bool CFestivalDoubleEleven::canGetLandGift(Player* player)
{
	if (!player || !IsInTime(FAT_LAND)) return false;
	return player->getRecord(18001) <= 0;
}

bool CFestivalDoubleEleven::canGetLandSumGift(Player* player)
{
	if (!player || !IsOpen(FAT_LAND_SUM)) return false;
	int32_t nLandDays = getLandSum(player);
	return nLandDays > 0 && player->getRecord(18002) < nLandDays;
}

bool CFestivalDoubleEleven::canGetDrawGift(Player* player)
{
	if (!player || !IsOpen(FAT_DRAW)) return false;
	int32_t nDrawTimes = player->getRecord(19001);
	int32_t nMaxDraw = player->getRecord(19002);
	return nDrawTimes > 0 && nDrawTimes >= nMaxDraw;
}

bool CFestivalDoubleEleven::canGetOnlineGift(Player* player)
{
	if (!player || !IsInTime(FAT_ONLINE_TIME)) return false;
	int32_t nOnlineTime = player->GetOperateLimit().GetLimitCount(2093);
	int32_t nRewardTimes = player->GetOperateLimit().GetLimitCount(2094);
	if (nRewardTimes >= (int32_t)m_vOnlineReward.size()) return false;
	return nOnlineTime > nRewardTimes;
}

bool CFestivalDoubleEleven::canGetWishGift(Player* player)
{
	if (!player || !IsInTime(FAT_BEST_WISH)) return false;
	return player->getRecord(18003) <= 0;
}

bool CFestivalDoubleEleven::canGetFaBaoBack(Player* player)
{
	if (!player || !IsOpen(FAT_FA_BAO_BACK)) return false;
	int32_t nTotalValue = 0;
	for (int32_t i = 0; i < 5; ++i)
		nTotalValue += player->getRecord(i + 19110);
	for (uint32_t i = 0; i < m_NeedValue.size(); ++i)
	{
		if (nTotalValue >= m_NeedValue[i] && player->getRecord(i + 19114) <= 0)
			return true;
	}
	return false;
}

bool CFestivalDoubleEleven::canGetEquipUpStarBack(Player* player)
{
	if (!player || !IsOpen(FAT_EQUIP_UPSTAR_BACK)) return false;
	int32_t nStar = player->getRecord(19109);
	for (uint32_t i = 0; i < m_NeedValue.size(); ++i)
	{
		if (nStar >= m_NeedValue[i] && player->getRecord(i + 19115) <= 0)
			return true;
	}
	return false;
}

bool CFestivalDoubleEleven::canGetRechargeBack(Player* player)
{
	if (!player || !IsOpen(FAT_RECHARGE_BACK)) return false;
	int32_t nRecharge = player->getRecord(19112);
	for (uint32_t i = 0; i < m_vRechargeBackLimit.size(); ++i)
	{
		if (nRecharge >= m_vRechargeBackLimit[i] && player->getRecord(i + 19116) <= 0)
			return true;
	}
	return false;
}

int32_t CFestivalDoubleEleven::GetOnlineTimeRewardTime(Player* player)
{
	if (!player || !IsInTime(FAT_ONLINE_TIME)) return 0;
	int32_t nOnlineTime = player->GetOperateLimit().GetLimitCount(2093);
	int32_t nRewardTimes = player->GetOperateLimit().GetLimitCount(2094);
	int32_t nNeedTime = (nRewardTimes + 1) * m_nOnlineTimeArea;
	if (nOnlineTime >= nNeedTime)
		return nOnlineTime - nRewardTimes * m_nOnlineTimeArea;
	return 0;
}

int32_t CFestivalDoubleEleven::getLandSum(Player* player)
{
	if (!player) return 0;
	return player->getRecord(18000);
}

int32_t CFestivalDoubleEleven::GetXiaoFeiSumGiftCount(Player* player)
{
	if (!player) return 0;
	int32_t nTotal = 0;
	int32_t nConsume = player->getRecord(19104);
	for (uint32_t i = 0; i < m_vXiaoFeiSumLimit.size(); ++i)
	{
		if (nConsume >= m_vXiaoFeiSumLimit[i] && player->getRecord(i + 19106) <= 0)
			++nTotal;
	}
	return nTotal;
}

int32_t CFestivalDoubleEleven::GetChouJiangType()
{
	if (IsInTime(FAT_CHOU_JIANG)) return 1;
	return 0;
}

int32_t CFestivalDoubleEleven::GetChouJiangSpecialTime()
{
	return 0;
}

int32_t CFestivalDoubleEleven::GetScoreCanDrawTime(Player* pPlayer)
{
	if (!pPlayer || !IsInTime(FAT_BOSS_SCORE_DRAW)) return 0;
	int32_t nScore = pPlayer->getRecord(19117);
	int32_t nDraw = pPlayer->getRecord(19118);
	int32_t nTimes = nScore / 100 - nDraw;
	return nTimes > 0 ? nTimes : 0;
}

int32_t CFestivalDoubleEleven::OnRandScoreDrawItem(Player* pPlayer)
{
	if (!pPlayer || !IsInTime(FAT_BOSS_SCORE_DRAW)) return 10002;
	int32_t nRecord = pPlayer->getRecord(19118);
	int32_t nMaxTimes = GetScoreCanDrawTime(pPlayer);
	if (nRecord >= nMaxTimes) return 10002;

	int32_t nTotalRate = 0;
	for (uint32_t i = 0; i < m_vBossScoreDrawReward.size(); ++i)
		nTotalRate += m_vBossScoreDrawReward[i].nRate;

	srand((unsigned int)TIMER.GetNow());
	int32_t nRand = rand() % (nTotalRate > 0 ? nTotalRate : 1);
	int32_t nCurRate = 0;
	for (uint32_t i = 0; i < m_vBossScoreDrawReward.size(); ++i)
	{
		nCurRate += m_vBossScoreDrawReward[i].nRate;
		if (nRand < nCurRate)
		{
			MemChrBag item;
			memset(&item, 0, sizeof(item));
			item.itemId = m_vBossScoreDrawReward[i].nId;
			item.itemCount = m_vBossScoreDrawReward[i].nCount;
			if (!pPlayer->GetBag().AddItem(item, IACR_NONE))
				return 10002;
			break;
		}
	}
	pPlayer->updateRecord(19118, nRecord + 1);
	SendIconState(pPlayer);
	return 0;
}

int32_t CFestivalDoubleEleven::CalBossScoreAddValue(int32_t nValue)
{
	if (IsInTime(FAT_BOSS_SCORE)) return nValue;
	return 0;
}

int32_t CFestivalDoubleEleven::calRechargeBack(int32_t nValue)
{
	int32_t nResult = nValue;
	calRechargeBackHelper(&nResult);
	return nResult;
}

int32_t CFestivalDoubleEleven::calRechargeBackHelper(int32_t* nValue)
{
	return *nValue * 10 / 100;
}

void CFestivalDoubleEleven::SendDaTiReward(Player* player, int8_t RewardType, CharId_t CharId)
{
	if (!player || !IsInTime(FAT_DA_TI))
		return;

	if (RewardType < 0 || RewardType >= (int32_t)m_vDaTiRankReward.size())
		return;

	for (uint32_t i = 0; i < m_vDaTiRankReward[RewardType].size(); ++i)
	{
		if (!player->GetBag().AddItem(m_vDaTiRankReward[RewardType][i], IACR_NONE))
			return;
	}

	if (RewardType < (int32_t)m_vDaTiRankMail.size() && m_vDaTiRankMail[RewardType] > 0 && CharId > 0)
	{
		DB_SERVICE.OnSendSysMail(CharId, m_vDaTiRankMail[RewardType]);
	}
}

int32_t CFestivalDoubleEleven::BuyDailyLimitShopItem(Player* player)
{
	if (!player || !IsInTime(FAT_DAILY_LIMIT_SHOP))
		return 10002;

	if (player->GetOperateLimit().GetLimitCount(2034) >= m_nDailyLimitShopLimit)
		return 10002;

	if (player->GetCurrency((CURRENCY_TYPE)m_nDailyLimitShopCostType) < m_nDailyLimitShopCostValue)
		return 10002;

	MemChrBagVector& items = m_vDailyLimitShopItem;
	for (uint32_t i = 0; i < items.size(); ++i)
	{
		if (!player->GetBag().AddItem(items[i], IACR_NONE))
			return 10002;
	}

	if (!player->DecCurrency((CURRENCY_TYPE)m_nDailyLimitShopCostType, (int64_t)m_nDailyLimitShopCostValue, GM_NONE, 0))
		return 10002;

	player->GetOperateLimit().AddLimitCount(2034, 1);
	return 0;
}

bool CFestivalDoubleEleven::BuyTitle(Player* player)
{
	return false;
}

int32_t CFestivalDoubleEleven::BuyGiftShopItem(Player* player, int8_t nIndex)
{
	if (!player || !IsInTime(FAT_GIFT_SHOP)) return 10002;
	if (nIndex < 0 || nIndex >= (int32_t)m_vGiftShopGoods.size()) return 10002;

	int32_t nPrice = m_vGiftShopGoods[nIndex][0].nPrice;
	if (player->GetCurrency(CURRENCY_GOLD) < nPrice) return 10002;
	if (!player->DecCurrency(CURRENCY_GOLD, (int64_t)nPrice, GM_NONE, 0))
		return 10002;

	for (uint32_t i = 0; i < m_vGiftShopGoods[nIndex][0].vItems.size(); ++i)
	{
		if (!player->GetBag().AddItem(m_vGiftShopGoods[nIndex][0].vItems[i], IACR_NONE))
			return 10002;
	}
	CharId_t Cid = player->getCid();
	std::string name = player->getName();
	broadcastPlayerAction(player, m_nBossStartBroadcast, Cid, name);
	return 0;
}

int32_t CFestivalDoubleEleven::BuyGiftShopItem2(Player* player, int8_t nIndex)
{
	if (!player || !IsInTime(FAT_DAILY_LIMIT_SHOP_2)) return 10002;
	if (nIndex < 0 || nIndex >= (int32_t)m_vDailyLimitShop2Goods.size()) return 10002;

	int32_t nPrice = m_vDailyLimitShop2Goods[nIndex].nPrice;
	if (player->GetCurrency(CURRENCY_GOLD) < nPrice) return 10002;
	if (!player->DecCurrency(CURRENCY_GOLD, (int64_t)nPrice, GM_NONE, 0))
		return 10002;

	CharId_t Cid = player->getCid();
	std::string name = player->getName();
	broadcastPlayerAction(player, m_vDailyLimitShop2Broadcast[nIndex], Cid, name);
	return 0;
}

int32_t CFestivalDoubleEleven::BuyGiftItem(Player* player, int8_t nIndex)
{
	if (!player || !IsInTime(FAT_BUY_GIFT)) return 10002;
	if (nIndex < 0 || nIndex >= (int32_t)m_vBuyGiftPrice.size()) return 10002;

	int32_t nPrice = m_vBuyGiftPrice[nIndex];
	if (player->GetCurrency(CURRENCY_GOLD) < nPrice) return 10002;
	if (!player->DecCurrency(CURRENCY_GOLD, (int64_t)nPrice, GM_NONE, 0))
		return 10002;

	for (uint32_t i = 0; i < m_vBuyGiftItem[nIndex].size(); ++i)
	{
		if (!player->GetBag().AddItem(m_vBuyGiftItem[nIndex][i], IACR_NONE))
			return 10002;
	}
	if (m_vBuyGiftBroadcast.size() > (uint32_t)nIndex && m_vBuyGiftBroadcast[nIndex] > 0)
	{
		CharId_t Cid = player->getCid();
		std::string name = player->getName();
		broadcastPlayerAction(player, m_vBuyGiftBroadcast[nIndex], Cid, name);
	}
	return 0;
}

int32_t CFestivalDoubleEleven::GetRechargeBack(Player* player)
{
	if (!player || !IsInTime(FAT_RECHARGE_BACK)) return 10002;
	SendIconState(player);
	return 0;
}

int32_t CFestivalDoubleEleven::GetEquipUpStarBackItem(Player* player)
{
	if (!player || !IsOpen(FAT_EQUIP_UPSTAR_BACK)) return 10002;
	SendIconState(player);
	return 0;
}

int32_t CFestivalDoubleEleven::GetFaBaoCritBackItem(Player* player, int8_t nType)
{
	if (!player || !IsInTime(FAT_FA_BAO_BACK)) return 10002;
	SendIconState(player);
	return 0;
}

int32_t CFestivalDoubleEleven::GetEquipQingYiGift(Player* player, int8_t nIndex)
{
	if (!player || !IsInTime(FAT_EQUIP_QING_YI)) return 10002;
	if (nIndex < 0 || nIndex >= (int32_t)m_vEquipQingYiLimit.size()) return 10002;
	if (player->getRecord(nIndex + 19119) > 0) return 10002;

	for (uint32_t i = 0; i < m_vEquipQingYiReward[nIndex].size(); ++i)
	{
		if (!player->GetBag().AddItem(m_vEquipQingYiReward[nIndex][i], IACR_NONE))
			return 10002;
	}
	player->updateRecord(nIndex + 19119, 1);
	SendIconState(player);
	return 0;
}

int32_t CFestivalDoubleEleven::GetFriendQingYiGift(Player* player, int8_t nIndex)
{
	if (!player || !IsInTime(FAT_FRIEND_QING_YI)) return 10002;
	if (nIndex < 0 || nIndex >= (int32_t)m_vFriendQingYiLimit.size()) return 10002;
	if (player->getRecord(nIndex + 19120) > 0) return 10002;

	for (uint32_t i = 0; i < m_vFriendQingYiReward[nIndex].size(); ++i)
	{
		if (!player->GetBag().AddItem(m_vFriendQingYiReward[nIndex][i], IACR_NONE))
			return 10002;
	}
	player->updateRecord(nIndex + 19120, 1);
	SendIconState(player);
	return 0;
}

int32_t CFestivalDoubleEleven::GouWuChe(Player* player, int32_t* p_nIndexList)
{
	if (!IsInTime(FAT_GOU_WU_CHE)) return 10002;
	return 0;
}

void CFestivalDoubleEleven::broadcastPlayerAction(Player* pPlayer, int32_t nType, CharId_t Cid, const std::string& name)
{
	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CD6);
	if (packet)
	{
		packet->writeInt32(nType);
		packet->writeInt64(Cid);
		packet->writeUTF8(name.c_str());
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.worldBroadcast(packet);
	}
}

void CFestivalDoubleEleven::RefreshMysteryShopItem(Player* player)
{
	// Refresh mystery shop items for player
}

void CFestivalDoubleEleven::checkRefreshMysteryShop()
{
	broadcastRefreshMysteryShop();
}

void CFestivalDoubleEleven::broadcastRefreshMysteryShop()
{
	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CD6);
	if (packet)
	{
		packet->writeInt32(0);
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.worldBroadcast(packet);
	}
}

// ===== Rank Data Load/Save =====

void CFestivalDoubleEleven::loadRankData(int32_t line)
{
	if (isOpen(FAT_DRAW_RANK)) loadDrawRank();
	if (isOpen(FAT_DAILY_XIAO_FEI_RANK)) loadXiaoFeiRank();
	if (isOpen(FAT_XIAO_FEI_SUM_RANK)) loadXiaoFeiSumRank();
	if (isOpen(FAT_RECHARGE_SUM_RANK)) loadRechargeSumRank();
	if (isOpen(FAT_CROSS_RECHARGE_SUM_RANK)) loadCrossRechargeSumRank();
	if (isOpen(FAT_CROSS_XIAO_FEI_SUM_RANK)) loadCrossXiaoFeiSumRank();
	if (isOpen(FAT_CROSS_CHOU_JIANG_RANK)) loadCrossChouJiangRank();
	if (isOpen(FAT_BOSS_SCORE_RANK)) loadBossScoreRank();
}

void CFestivalDoubleEleven::resetRankData(int32_t line)
{
	m_mDrawRank.clear();
	m_mXiaoFeiRank.clear();
	m_mXiaoFeiSumRank.clear();
	m_mRechargeSumRank.clear();
	m_mCrossXiaoFeiSumRank.clear();
	m_mCrossRechargeSumRank.clear();
	m_mCrossChouJiangRank.clear();
	m_mBossScoreRank.clear();
}

void CFestivalDoubleEleven::resetPlayerData()
{
	// Reset all player records - called when config version changes
	// Player-specific records are reset on next player login via record updates
}

int32_t CFestivalDoubleEleven::loadVersion(int32_t line)
{
	MySqlDBGuard db(DBPOOL);
	char szSQL[4096] = {};
	snprintf(szSQL, sizeof(szSQL),
		"SELECT * FROM `sys_server_config` WHERE `name`='FESTIVAL_DOUBLE_ELEVEN_VERSION'");
	MySqlQuery result = db.query(szSQL);
	if (!result.eof())
		return atoi(result.getStringValue("value", "0"));
	return 0;
}

void CFestivalDoubleEleven::saveVersion(int32_t line)
{
	MySqlDBGuard db(DBPOOL);
	char szSQL[4096] = {};
	int32_t nNowTime = DayTime::now();
	snprintf(szSQL, sizeof(szSQL),
		"INSERT INTO `sys_server_config` (`name`, `value`, `refresh_time`) "
		"VALUES ('FESTIVAL_DOUBLE_ELEVEN_VERSION', %d, %d) ON DUPLICATE KEY UPDATE "
		"`value`=%d, `refresh_time`=%d",
		m_nVersion, nNowTime, m_nVersion, nNowTime);
	db.excute(szSQL);
}

void CFestivalDoubleEleven::loadDrawRank()
{
	MySqlDBGuard db(DBPOOL);
	MySqlQuery result = db.query(
		"SELECT * FROM `mem_double_eleven_rank` WHERE `type`=4 ORDER BY `count` DESC,`time`");

	int32_t nIndex = 0;
	std::vector<FestivalRank> vRank;
	vRank.resize(m_nDrawRankSize);

	while (!result.eof() && m_nDrawRankSize > nIndex)
	{
		vRank[nIndex].Cid = (CharId_t)result.getInt64Value("cid", 0);
		vRank[nIndex].strName = result.getStringValue("name", "");
		vRank[nIndex].nValue = result.getIntValue("count", 0);
		vRank[nIndex].nUpdateTime = result.getIntValue("time", 0);
		++nIndex;
		result.nextRow();
	}

	int8_t key = 0;
	m_mDrawRank[key] = vRank;
}

void CFestivalDoubleEleven::updateDrawRank(Player* player)
{
	if (!player || !IsInTime(FAT_DRAW_RANK)) return;
	MutexGuard lock(m_lock);
	int8_t connid = 0;
	std::vector<FestivalRank>& rankList = m_mDrawRank[connid];
	CharId_t Cid = player->getCid();
	bool found = false;
	for (uint32_t i = 0; i < rankList.size(); ++i)
	{
		if (rankList[i].Cid == Cid)
		{
			rankList[i].nValue = player->getRecord(19001);
			rankList[i].nUpdateTime = DayTime::now();
			rankList[i].bChange = 1;
			found = true; break;
		}
	}
	if (!found)
	{
		for (uint32_t i = 0; i < rankList.size(); ++i)
		{
			if (rankList[i].Cid <= 0)
			{
				rankList[i].Cid = Cid;
				rankList[i].strName = player->getName();
				rankList[i].nValue = player->getRecord(19001);
				rankList[i].nUpdateTime = DayTime::now();
				rankList[i].bChange = 1;
				break;
			}
		}
	}
	checkDrawRankInvalid(connid);
}

void CFestivalDoubleEleven::updateDrawRank(int8_t connid, int32_t nIndex, const std::string& info) { }

void CFestivalDoubleEleven::checkDrawRankInvalid(int8_t connid)
{
	MutexGuard lock(m_lock);
	if (m_mDrawRank.find(connid) == m_mDrawRank.end())
	{
		std::vector<FestivalRank> vRank;
		vRank.resize(m_nDrawRankSize);
		m_mDrawRank[connid] = vRank;
	}
}

void CFestivalDoubleEleven::checkDrawRank()
{
	MutexGuard lock(m_lock);
	for (std::map<int8_t, std::vector<FestivalRank>>::iterator it = m_mDrawRank.begin();
		it != m_mDrawRank.end(); ++it)
	{
		int8_t connid = it->first;
		std::vector<FestivalRank>& vRank = it->second;
		for (int32_t i = 0; i < m_nDrawRankSize && i < (int32_t)vRank.size() && vRank[i].Cid > 0; ++i)
		{
			if (vRank[i].bChange)
			{
				vRank[i].bChange = 0;
				// Save rank to DB
				MySqlDBGuard db(DBPOOL);
				char szSQL[4096] = {};
				int32_t nNowTime = DayTime::now();
				snprintf(szSQL, sizeof(szSQL),
					"REPLACE INTO `mem_double_eleven_rank` (`type`, `cid`, `name`, `count`, `time`) "
					"VALUES (4, %lld, '%s', %d, %d)",
					(long long)vRank[i].Cid,
					vRank[i].strName.c_str(),
					vRank[i].nValue,
					nNowTime);
				db.excute(szSQL);
			}
		}
	}
}

void CFestivalDoubleEleven::sendDrawRankMail()
{
	if (GAME_SERVICE.getLine() != 1)
		return;

	MutexGuard lock(m_lock);
	for (std::map<int8_t, std::vector<FestivalRank>>::iterator it = m_mDrawRank.begin();
		it != m_mDrawRank.end(); ++it)
	{
		std::vector<FestivalRank>& vRank = it->second;
		for (int32_t i = 0; i < m_nDrawRankSize && i < (int32_t)vRank.size(); ++i)
		{
			if (vRank[i].Cid > 0 && i < (int32_t)m_vDrawRankMail.size() && i < (int32_t)m_vDrawRankReward.size())
			{
				int32_t nMailId = m_vDrawRankMail[i];
				if (nMailId > 0)
					DB_SERVICE.OnSendSysMail(vRank[i].Cid, nMailId, m_vDrawRankReward[i]);
			}
		}
	}
}

// ===== XiaoFei Rank =====

void CFestivalDoubleEleven::loadXiaoFeiRank()
{
	MySqlDBGuard db(DBPOOL);
	char szSQL[4096] = {};
	snprintf(szSQL, sizeof(szSQL),
		"SELECT * FROM `mem_double_eleven_rank` WHERE `type`=5 ORDER BY `count` DESC,`time`");
	MySqlQuery result = db.query(szSQL);
	if (!result.eof())
	{
		int32_t nIndex = 0;
		std::vector<FestivalRank> vRank;
		vRank.resize(m_nXiaoFeiRankSize);

		while (!result.eof() && m_nXiaoFeiRankSize > nIndex)
		{vRank[nIndex].Cid = (CharId_t)result.getInt64Value("cid", 0);
		vRank[nIndex].strName = result.getStringValue("name", "");
		vRank[nIndex].nValue = result.getIntValue("count", 0);
		vRank[nIndex].nUpdateTime = result.getIntValue("time", 0);
		++nIndex;
		result.nextRow();
	}

		int8_t key = 0;
		m_mXiaoFeiRank[key].push_back(vRank);
	}
}

void CFestivalDoubleEleven::updateXiaoFeiRank(Player* player)
{
	if (!player || !IsInTime(FAT_DAILY_XIAO_FEI_RANK)) return;
	MutexGuard lock(m_lock);
		int8_t connid = 0;
	int32_t nDay = m_nDay - m_vStartDay[FAT_DAILY_XIAO_FEI_RANK];
	if (nDay < 0) return;

	// Make sure day vector exists
	while ((int32_t)m_mXiaoFeiRank[connid].size() <= nDay)
	{
		std::vector<FestivalRank> dayRank;
		dayRank.resize(m_nXiaoFeiRankSize);
		m_mXiaoFeiRank[connid].push_back(dayRank);
	}

	std::vector<FestivalRank>& rankList = m_mXiaoFeiRank[connid][nDay];
	CharId_t rankCid = player->getCid();
	bool found = false;
	for (uint32_t i = 0; i < rankList.size(); ++i)
	{
		if (rankList[i].Cid == rankCid)
		{
			rankList[i].nValue += player->getRecord(19104);
			rankList[i].nUpdateTime = DayTime::now();
			rankList[i].bChange = 1;
			found = true; break;
		}
	}
	if (!found)
	{
		for (uint32_t i = 0; i < rankList.size(); ++i)
		{
			if (rankList[i].Cid <= 0)
			{
				rankList[i].Cid = rankCid;
				rankList[i].strName = player->getName();
				rankList[i].nValue = player->getRecord(19104);
				rankList[i].nUpdateTime = DayTime::now();
				rankList[i].bChange = 1;
				break;
			}
		}
	}
	checkXiaoFeiRankInvalid(connid);
}

void CFestivalDoubleEleven::updateXiaoFeiRank(int8_t connid, int32_t nDay, int32_t nIndex, const std::string& info) { }
void CFestivalDoubleEleven::checkXiaoFeiRankInvalid(int8_t connid)
{
	MutexGuard lock(m_lock);
	if (m_mXiaoFeiRank.find(connid) == m_mXiaoFeiRank.end())
	{
		std::vector<std::vector<FestivalRank>> vDays;
		m_mXiaoFeiRank[connid] = vDays;
	}
}
void CFestivalDoubleEleven::checkXiaoFeiRank()
{
	MutexGuard lock(m_lock);
	for (std::map<int8_t, std::vector<std::vector<FestivalRank>>>::iterator it = m_mXiaoFeiRank.begin();
		it != m_mXiaoFeiRank.end(); ++it)
	{
		int8_t connid = it->first;
		std::vector<std::vector<FestivalRank>>& vDays = it->second;
		for (int32_t d = 0; d < (int32_t)vDays.size(); ++d)
		{
			std::vector<FestivalRank>& vRank = vDays[d];
			for (int32_t i = 0; i < m_nXiaoFeiRankSize && i < (int32_t)vRank.size() && vRank[i].Cid > 0; ++i)
			{
				if (vRank[i].bChange)
				{
					vRank[i].bChange = 0;
					MySqlDBGuard db(DBPOOL);
					char szSQL[4096] = {};
					int32_t nNowTime = DayTime::now();
					snprintf(szSQL, sizeof(szSQL),
					"REPLACE INTO `mem_double_eleven_rank` (`type`, `cid`, `name`, `count`, `time`) "
					"VALUES (5, %lld, '%s', %d, %d)",
					(long long)vRank[i].Cid,
					vRank[i].strName.c_str(),
					vRank[i].nValue,
					nNowTime);
					db.excute(szSQL);
				}
			}
		}
	}
}
void CFestivalDoubleEleven::sendXiaoFeiRankMail()
{
	if (GAME_SERVICE.getLine() != 1 || m_nDay <= 0 || m_nDay > m_nEndDay)
		return;

	MutexGuard lock(m_lock);
	for (std::map<int8_t, std::vector<std::vector<FestivalRank>>>::iterator it = m_mXiaoFeiRank.begin();
		it != m_mXiaoFeiRank.end(); ++it)
	{
		std::vector<std::vector<FestivalRank>>& vDays = it->second;
		int32_t nPrevDay = m_nDay - 1;
		if (nPrevDay < 0 || nPrevDay >= (int32_t)vDays.size())
			continue;

		std::vector<FestivalRank>& vRank = vDays[nPrevDay];
		for (int32_t i = 0; i < m_nXiaoFeiRankSize && i < (int32_t)vRank.size(); ++i)
		{
			if (vRank[i].Cid > 0 && i < (int32_t)m_vXiaoFeiRankMail.size() && i < (int32_t)m_vXiaoFeiRankReward.size())
			{
				int32_t nMailId = m_vXiaoFeiRankMail[i];
				if (nMailId > 0)
					DB_SERVICE.OnSendSysMail(vRank[i].Cid, nMailId, m_vXiaoFeiRankReward[i]);
			}
		}
	}
}

// ===== XiaoFei Sum Rank =====

void CFestivalDoubleEleven::loadXiaoFeiSumRank()
{
	MySqlDBGuard db(DBPOOL);
	MySqlQuery result = db.query(
		"SELECT * FROM `mem_double_eleven_rank` WHERE `type`=6 ORDER BY `count` DESC,`time`");
	if (!result.eof())
	{
		int32_t nIndex = 0;
		std::vector<FestivalRank> vRank;
		vRank.resize(m_nXiaoFeiSumRankSize);

		while (!result.eof() && m_nXiaoFeiSumRankSize > nIndex)
		{
			vRank[nIndex].Cid = (CharId_t)result.getInt64Value("cid", 0);
			vRank[nIndex].strName = result.getStringValue("name", "");
			vRank[nIndex].nValue = result.getIntValue("count", 0);
			++nIndex;
			result.nextRow();
		}

		int8_t key = 0;
		m_mXiaoFeiSumRank[key] = vRank;
	}
}

void CFestivalDoubleEleven::updateXiaoFeiSumRank(Player* player)
{
	if (!player || !IsInTime(FAT_XIAO_FEI_SUM_RANK)) return;
	MutexGuard lock(m_lock);
	int8_t connid = 0;
	std::vector<FestivalRank>& rankList = m_mXiaoFeiSumRank[connid];
	CharId_t Cid = player->getCid();
	bool found = false;
	for (uint32_t i = 0; i < rankList.size(); ++i)
	{
		if (rankList[i].Cid == Cid)
		{
			rankList[i].nValue += player->getRecord(19104);
			rankList[i].nUpdateTime = DayTime::now();
			rankList[i].bChange = 1;
			found = true; break;
		}
	}
	if (!found)
	{
		for (uint32_t i = 0; i < rankList.size(); ++i)
		{
			if (rankList[i].Cid <= 0)
			{
				rankList[i].Cid = Cid;
				rankList[i].strName = player->getName();
				rankList[i].nValue = player->getRecord(19104);
				rankList[i].nUpdateTime = DayTime::now();
				rankList[i].bChange = 1;
				break;
			}
		}
	}
	checkXiaoFeiSumRankInvalid(connid);
}

void CFestivalDoubleEleven::updateXiaoFeiSumRank(int8_t connid, int32_t nIndex, const std::string& info) { }
void CFestivalDoubleEleven::checkXiaoFeiSumRankInvalid(int8_t connid)
{
	MutexGuard lock(m_lock);
	if (m_mXiaoFeiSumRank.find(connid) == m_mXiaoFeiSumRank.end())
	{
		std::vector<FestivalRank> vRank;
		vRank.resize(m_nXiaoFeiSumRankSize);
		m_mXiaoFeiSumRank[connid] = vRank;
	}
}
void CFestivalDoubleEleven::checkXiaoFeiSumRank()
{
	MutexGuard lock(m_lock);
	for (std::map<int8_t, std::vector<FestivalRank>>::iterator it = m_mXiaoFeiSumRank.begin();
		it != m_mXiaoFeiSumRank.end(); ++it)
	{
		std::vector<FestivalRank>& vRank = it->second;
		for (int32_t i = 0; i < m_nXiaoFeiSumRankSize && i < (int32_t)vRank.size() && vRank[i].Cid > 0; ++i)
		{
			if (vRank[i].bChange)
			{
				vRank[i].bChange = 0;
				MySqlDBGuard db(DBPOOL);
				char szSQL[4096] = {};
				int32_t nNowTime = DayTime::now();
				snprintf(szSQL, sizeof(szSQL),
					"REPLACE INTO `mem_double_eleven_rank` (`type`, `cid`, `name`, `count`, `time`) "
					"VALUES (6, %lld, '%s', %d, %d)",
					(long long)vRank[i].Cid,
					vRank[i].strName.c_str(),
					vRank[i].nValue,
					nNowTime);
				db.excute(szSQL);
			}
		}
	}
}
void CFestivalDoubleEleven::sendXiaoFeiSumRankMail()
{
	if (GAME_SERVICE.getLine() != 1)
		return;

	MutexGuard lock(m_lock);
	for (std::map<int8_t, std::vector<FestivalRank>>::iterator it = m_mXiaoFeiSumRank.begin();
		it != m_mXiaoFeiSumRank.end(); ++it)
	{
		std::vector<FestivalRank>& vRank = it->second;
		for (int32_t i = 0; i < m_nXiaoFeiSumRankSize && i < (int32_t)vRank.size(); ++i)
		{
			if (vRank[i].Cid > 0 && i < (int32_t)m_vXiaoFeiSumRankMail.size() && i < (int32_t)m_vXiaoFeiSumRankReward.size())
			{
				int32_t nMailId = m_vXiaoFeiSumRankMail[i];
				if (nMailId > 0)
					DB_SERVICE.OnSendSysMail(vRank[i].Cid, nMailId, m_vXiaoFeiSumRankReward[i]);
			}
		}
	}
}

// ===== Recharge Sum Rank =====

void CFestivalDoubleEleven::loadRechargeSumRank()
{
	MySqlDBGuard db(DBPOOL);
	MySqlQuery result = db.query(
		"SELECT * FROM `mem_double_eleven_rank` WHERE `type`=7 ORDER BY `count` DESC,`time`");
	if (!result.eof())
	{
		int32_t nIndex = 0;
		std::vector<FestivalRank> vRank;
		vRank.resize(m_nRechargeSumRankSize);
		while (!result.eof() && m_nRechargeSumRankSize > nIndex)
		{
			vRank[nIndex].Cid = (CharId_t)result.getInt64Value("cid", 0);
			vRank[nIndex].strName = result.getStringValue("name", "");
			vRank[nIndex].nValue = result.getIntValue("count", 0);
			++nIndex;
			result.nextRow();
		}
		int8_t key = 0;
		m_mRechargeSumRank[key] = vRank;
	}
}

void CFestivalDoubleEleven::updateRechargeSumRank(Player* player)
{
	if (!player || !IsInTime(FAT_RECHARGE_SUM_RANK)) return;
	MutexGuard lock(m_lock);
	int8_t connid = 0;
	std::vector<FestivalRank>& rankList = m_mRechargeSumRank[connid];
	CharId_t Cid = player->getCid();
	bool found = false;
	for (uint32_t i = 0; i < rankList.size(); ++i)
	{
		if (rankList[i].Cid == Cid)
		{
			rankList[i].nValue += player->getRecord(19112);
			rankList[i].nUpdateTime = DayTime::now();
			rankList[i].bChange = 1;
			found = true; break;
		}
	}
	if (!found)
	{
		for (uint32_t i = 0; i < rankList.size(); ++i)
		{
			if (rankList[i].Cid <= 0)
			{
				rankList[i].Cid = Cid;
				rankList[i].strName = player->getName();
				rankList[i].nValue = player->getRecord(19112);
				rankList[i].nUpdateTime = DayTime::now();
				rankList[i].bChange = 1;
				break;
			}
		}
	}
	checkRechargeSumRankInvalid(connid);
}

void CFestivalDoubleEleven::updateRechargeSumRank(int8_t connid, int32_t nIndex, const std::string& info) { }
void CFestivalDoubleEleven::checkRechargeSumRankInvalid(int8_t connid)
{
	MutexGuard lock(m_lock);
	if (m_mRechargeSumRank.find(connid) == m_mRechargeSumRank.end())
	{
		std::vector<FestivalRank> vRank;
		vRank.resize(m_nRechargeSumRankSize);
		m_mRechargeSumRank[connid] = vRank;
	}
}
void CFestivalDoubleEleven::checkRechargeSumRank()
{
	MutexGuard lock(m_lock);
	for (std::map<int8_t, std::vector<FestivalRank>>::iterator it = m_mRechargeSumRank.begin();
		it != m_mRechargeSumRank.end(); ++it)
	{
		std::vector<FestivalRank>& vRank = it->second;
		for (int32_t i = 0; i < m_nRechargeSumRankSize && i < (int32_t)vRank.size() && vRank[i].Cid > 0; ++i)
		{
			if (vRank[i].bChange)
			{
				vRank[i].bChange = 0;
				MySqlDBGuard db(DBPOOL);
				char szSQL[4096] = {};
				int32_t nNowTime = DayTime::now();
				snprintf(szSQL, sizeof(szSQL),
					"REPLACE INTO `mem_double_eleven_rank` (`type`, `cid`, `name`, `count`, `time`) "
					"VALUES (7, %lld, '%s', %d, %d)",
					(long long)vRank[i].Cid,
					vRank[i].strName.c_str(),
					vRank[i].nValue,
					nNowTime);
				db.excute(szSQL);
			}
		}
	}
}
void CFestivalDoubleEleven::sendRechargeSumRankMail()
{
	if (GAME_SERVICE.getLine() != 1)
		return;

	MutexGuard lock(m_lock);
	for (std::map<int8_t, std::vector<FestivalRank>>::iterator it = m_mRechargeSumRank.begin();
		it != m_mRechargeSumRank.end(); ++it)
	{
		std::vector<FestivalRank>& vRank = it->second;
		for (int32_t i = 0; i < m_nRechargeSumRankSize && i < (int32_t)vRank.size(); ++i)
		{
			if (vRank[i].Cid > 0 && i < (int32_t)m_vRechargeSumRankMail.size() && i < (int32_t)m_vRechargeSumRankReward.size())
			{
				int32_t nMailId = m_vRechargeSumRankMail[i];
				if (nMailId > 0)
					DB_SERVICE.OnSendSysMail(vRank[i].Cid, nMailId, m_vRechargeSumRankReward[i]);
			}
		}
	}
}

// ===== Cross Recharge Sum Rank =====

void CFestivalDoubleEleven::loadCrossRechargeSumRank()
{
	MySqlDBGuard db(DBPOOL);
	MySqlQuery result = db.query(
		"SELECT * FROM `mem_double_eleven_rank` WHERE `type`=9 ORDER BY `count` DESC,`time`");
	if (!result.eof())
	{
		int32_t nIndex = 0;
		std::vector<FestivalRank> vRank;
		vRank.resize(m_nRechargeSumRankSize);
		while (!result.eof() && m_nRechargeSumRankSize > nIndex)
		{
			vRank[nIndex].Cid = (CharId_t)result.getInt64Value("cid", 0);
			vRank[nIndex].strName = result.getStringValue("name", "");
			vRank[nIndex].nValue = result.getIntValue("count", 0);
			++nIndex;
			result.nextRow();
		}
		int8_t key = 0;
		m_mCrossRechargeSumRank[key] = vRank;
	}
}
void CFestivalDoubleEleven::updateCrossRechargeSumRank(Player* player)
{
	if (!player || !IsInTime(FAT_CROSS_RECHARGE_SUM_RANK)) return;
	MutexGuard lock(m_lock);
	int8_t connid = 0;
	std::vector<FestivalRank>& rankList = m_mCrossRechargeSumRank[connid];
	CharId_t Cid = player->getCid();
	bool found = false;
	for (uint32_t i = 0; i < rankList.size(); ++i)
	{
		if (rankList[i].Cid == Cid)
		{
			rankList[i].nValue += player->getRecord(19112);
			rankList[i].nUpdateTime = DayTime::now();
			rankList[i].bChange = 1;
			found = true; break;
		}
	}
	if (!found)
	{
		for (uint32_t i = 0; i < rankList.size(); ++i)
		{
			if (rankList[i].Cid <= 0)
			{
				rankList[i].Cid = Cid;
				rankList[i].strName = player->getName();
				rankList[i].nValue = player->getRecord(19112);
				rankList[i].nUpdateTime = DayTime::now();
				rankList[i].bChange = 1;
				break;
			}
		}
	}
	checkCrossRechargeSumRankInvalid(connid);
}
void CFestivalDoubleEleven::updateCrossRechargeSumRank(int8_t connid, int32_t nIndex, const std::string& info) { }
void CFestivalDoubleEleven::checkCrossRechargeSumRankInvalid(int8_t connid)
{
	MutexGuard lock(m_lock);
	if (m_mCrossRechargeSumRank.find(connid) == m_mCrossRechargeSumRank.end())
	{
		std::vector<FestivalRank> vRank;
		vRank.resize(m_nRechargeSumRankSize);
		m_mCrossRechargeSumRank[connid] = vRank;
	}
}
void CFestivalDoubleEleven::checkCrossRechargeSumRank()
{
	MutexGuard lock(m_lock);
	for (std::map<int8_t, std::vector<FestivalRank>>::iterator it = m_mCrossRechargeSumRank.begin();
		it != m_mCrossRechargeSumRank.end(); ++it)
	{
		std::vector<FestivalRank>& vRank = it->second;
		for (int32_t i = 0; i < m_nRechargeSumRankSize && i < (int32_t)vRank.size() && vRank[i].Cid > 0; ++i)
		{
			if (vRank[i].bChange)
			{
				vRank[i].bChange = 0;
				MySqlDBGuard db(DBPOOL);
				char szSQL[4096] = {};
				int32_t nNowTime = DayTime::now();
				snprintf(szSQL, sizeof(szSQL),
					"REPLACE INTO `mem_double_eleven_rank` (`type`, `cid`, `name`, `count`, `time`) "
					"VALUES (9, %lld, '%s', %d, %d)",
					(long long)vRank[i].Cid,
					vRank[i].strName.c_str(),
					vRank[i].nValue,
					nNowTime);
				db.excute(szSQL);
			}
		}
	}
}

// ===== Cross XiaoFei Sum Rank =====

void CFestivalDoubleEleven::loadCrossXiaoFeiSumRank()
{
	MySqlDBGuard db(DBPOOL);
	MySqlQuery result = db.query(
		"SELECT * FROM `mem_double_eleven_rank` WHERE `type`=10 ORDER BY `count` DESC,`time`");
	if (!result.eof())
	{
		int32_t nIndex = 0;
		std::vector<FestivalRank> vRank;
		vRank.resize(m_nXiaoFeiSumRankSize);
		while (!result.eof() && m_nXiaoFeiSumRankSize > nIndex)
		{
			vRank[nIndex].Cid = (CharId_t)result.getInt64Value("cid", 0);
			vRank[nIndex].strName = result.getStringValue("name", "");
			vRank[nIndex].nValue = result.getIntValue("count", 0);
			++nIndex;
			result.nextRow();
		}
		int8_t key = 0;
		m_mCrossXiaoFeiSumRank[key] = vRank;
	}
}
void CFestivalDoubleEleven::updateCrossXiaoFeiSumRank(Player* player)
{
	if (!player || !IsInTime(FAT_CROSS_XIAO_FEI_SUM_RANK)) return;
	MutexGuard lock(m_lock);
	int8_t connid = 0;
	std::vector<FestivalRank>& rankList = m_mCrossXiaoFeiSumRank[connid];
	CharId_t Cid = player->getCid();
	bool found = false;
	for (uint32_t i = 0; i < rankList.size(); ++i)
	{
		if (rankList[i].Cid == Cid)
		{
			rankList[i].nValue += player->getRecord(19104);
			rankList[i].nUpdateTime = DayTime::now();
			rankList[i].bChange = 1;
			found = true; break;
		}
	}
	if (!found)
	{
		for (uint32_t i = 0; i < rankList.size(); ++i)
		{
			if (rankList[i].Cid <= 0)
			{
				rankList[i].Cid = Cid;
				rankList[i].strName = player->getName();
				rankList[i].nValue = player->getRecord(19104);
				rankList[i].nUpdateTime = DayTime::now();
				rankList[i].bChange = 1;
				break;
			}
		}
	}
	checkCrossXiaoFeiSumRankInvalid(connid);
}
void CFestivalDoubleEleven::updateCrossXiaoFeiSumRank(int8_t connid, int32_t nIndex, const std::string& info) { }
void CFestivalDoubleEleven::checkCrossXiaoFeiSumRankInvalid(int8_t connid)
{
	MutexGuard lock(m_lock);
	if (m_mCrossXiaoFeiSumRank.find(connid) == m_mCrossXiaoFeiSumRank.end())
	{
		std::vector<FestivalRank> vRank;
		vRank.resize(m_nXiaoFeiSumRankSize);
		m_mCrossXiaoFeiSumRank[connid] = vRank;
	}
}
void CFestivalDoubleEleven::checkCrossXiaoFeiSumRank()
{
	MutexGuard lock(m_lock);
	for (std::map<int8_t, std::vector<FestivalRank>>::iterator it = m_mCrossXiaoFeiSumRank.begin();
		it != m_mCrossXiaoFeiSumRank.end(); ++it)
	{
		std::vector<FestivalRank>& vRank = it->second;
		for (int32_t i = 0; i < m_nXiaoFeiSumRankSize && i < (int32_t)vRank.size() && vRank[i].Cid > 0; ++i)
		{
			if (vRank[i].bChange)
			{
				vRank[i].bChange = 0;
				MySqlDBGuard db(DBPOOL);
				char szSQL[4096] = {};
				int32_t nNowTime = DayTime::now();
				snprintf(szSQL, sizeof(szSQL),
					"REPLACE INTO `mem_double_eleven_rank` (`type`, `cid`, `name`, `count`, `time`) "
					"VALUES (10, %lld, '%s', %d, %d)",
					(long long)vRank[i].Cid,
					vRank[i].strName.c_str(),
					vRank[i].nValue,
					nNowTime);
				db.excute(szSQL);
			}
		}
	}
}

// ===== Cross ChouJiang Rank =====

void CFestivalDoubleEleven::loadCrossChouJiangRank()
{
	MySqlDBGuard db(DBPOOL);
	MySqlQuery result = db.query(
		"SELECT * FROM `mem_double_eleven_rank` WHERE `type`=11 ORDER BY `count` DESC,`time`");
	if (!result.eof())
	{
		int32_t nIndex = 0;
		std::vector<FestivalRank> vRank;
		vRank.resize(m_nDrawRankSize);
		while (!result.eof() && m_nDrawRankSize > nIndex)
		{
			vRank[nIndex].Cid = (CharId_t)result.getInt64Value("cid", 0);
			vRank[nIndex].strName = result.getStringValue("name", "");
			vRank[nIndex].nValue = result.getIntValue("count", 0);
			++nIndex;
			result.nextRow();
		}
		int8_t key = 0;
		m_mCrossChouJiangRank[key] = vRank;
	}
}
void CFestivalDoubleEleven::updateCrossChouJiangRank(Player* player)
{
	if (!player || !IsInTime(FAT_CROSS_CHOU_JIANG_RANK)) return;
	MutexGuard lock(m_lock);
	int8_t connid = 0;
	std::vector<FestivalRank>& rankList = m_mCrossChouJiangRank[connid];
	CharId_t Cid = player->getCid();
	bool found = false;
	for (uint32_t i = 0; i < rankList.size(); ++i)
	{
		if (rankList[i].Cid == Cid)
		{
			rankList[i].nValue += player->getRecord(19001);
			rankList[i].nUpdateTime = DayTime::now();
			rankList[i].bChange = 1;
			found = true; break;
		}
	}
	if (!found)
	{
		for (uint32_t i = 0; i < rankList.size(); ++i)
		{
			if (rankList[i].Cid <= 0)
			{
				rankList[i].Cid = Cid;
				rankList[i].strName = player->getName();
				rankList[i].nValue = player->getRecord(19001);
				rankList[i].nUpdateTime = DayTime::now();
				rankList[i].bChange = 1;
				break;
			}
		}
	}
	checkCrossChouJiangRankInvalid(connid);
}
void CFestivalDoubleEleven::updateCrossChouJiangRank(int8_t connid, int32_t nIndex, const std::string& info) { }
void CFestivalDoubleEleven::checkCrossChouJiangRankInvalid(int8_t connid)
{
	MutexGuard lock(m_lock);
	if (m_mCrossChouJiangRank.find(connid) == m_mCrossChouJiangRank.end())
	{
		std::vector<FestivalRank> vRank;
		vRank.resize(m_nDrawRankSize);
		m_mCrossChouJiangRank[connid] = vRank;
	}
}
void CFestivalDoubleEleven::checkCrossChouJiangRank()
{
	MutexGuard lock(m_lock);
	for (std::map<int8_t, std::vector<FestivalRank>>::iterator it = m_mCrossChouJiangRank.begin();
		it != m_mCrossChouJiangRank.end(); ++it)
	{
		std::vector<FestivalRank>& vRank = it->second;
		for (int32_t i = 0; i < m_nDrawRankSize && i < (int32_t)vRank.size() && vRank[i].Cid > 0; ++i)
		{
			if (vRank[i].bChange)
			{
				vRank[i].bChange = 0;
				MySqlDBGuard db(DBPOOL);
				char szSQL[4096] = {};
				int32_t nNowTime = DayTime::now();
				snprintf(szSQL, sizeof(szSQL),
					"REPLACE INTO `mem_double_eleven_rank` (`type`, `cid`, `name`, `count`, `time`) "
					"VALUES (11, %lld, '%s', %d, %d)",
					(long long)vRank[i].Cid,
					vRank[i].strName.c_str(),
					vRank[i].nValue,
					nNowTime);
				db.excute(szSQL);
			}
		}
	}
}

// ===== Boss Score Rank =====

void CFestivalDoubleEleven::loadBossScoreRank()
{
	MySqlDBGuard db(DBPOOL);
	MySqlQuery result = db.query(
		"SELECT * FROM `mem_double_eleven_rank` WHERE `type`=8 ORDER BY `count` DESC,`time`");
	if (!result.eof())
	{
		int32_t nIndex = 0;
		std::vector<FestivalRank> vRank;
		vRank.resize(m_BossScoreRankSize);
		while (!result.eof() && m_BossScoreRankSize > nIndex)
		{
			vRank[nIndex].Cid = (CharId_t)result.getInt64Value("cid", 0);
			vRank[nIndex].strName = result.getStringValue("name", "");
			vRank[nIndex].nValue = result.getIntValue("count", 0);
			++nIndex;
			result.nextRow();
		}
		int8_t key = 0;
		m_mBossScoreRank[key] = vRank;
	}
}

void CFestivalDoubleEleven::updateBossScoreRank(Player* player)
{
	if (!player || !IsInTime(FAT_BOSS_SCORE_RANK)) return;
	MutexGuard lock(m_lock);
	int8_t connid = 0;
	std::vector<FestivalRank>& rankList = m_mBossScoreRank[connid];
	CharId_t Cid = player->getCid();
	bool found = false;
	for (uint32_t i = 0; i < rankList.size(); ++i)
	{
		if (rankList[i].Cid == Cid)
		{
			rankList[i].nValue += player->getRecord(19117);
			rankList[i].nUpdateTime = DayTime::now();
			rankList[i].bChange = 1;
			found = true; break;
		}
	}
	if (!found)
	{
		for (uint32_t i = 0; i < rankList.size(); ++i)
		{
			if (rankList[i].Cid <= 0)
			{
				rankList[i].Cid = Cid;
				rankList[i].strName = player->getName();
				rankList[i].nValue = player->getRecord(19117);
				rankList[i].nUpdateTime = DayTime::now();
				rankList[i].bChange = 1;
				break;
			}
		}
	}
	checkBossScoreRankInvalid(connid);
}
void CFestivalDoubleEleven::updateBossScoreRank(int8_t connid, int32_t nIndex, const std::string& info) { }
void CFestivalDoubleEleven::checkBossScoreRankInvalid(int8_t connid)
{
	MutexGuard lock(m_lock);
	if (m_mBossScoreRank.find(connid) == m_mBossScoreRank.end())
	{
		std::vector<FestivalRank> vRank;
		vRank.resize(m_BossScoreRankSize);
		m_mBossScoreRank[connid] = vRank;
	}
}
void CFestivalDoubleEleven::checkBossScoreRank()
{
	MutexGuard lock(m_lock);
	for (std::map<int8_t, std::vector<FestivalRank>>::iterator it = m_mBossScoreRank.begin();
		it != m_mBossScoreRank.end(); ++it)
	{
		std::vector<FestivalRank>& vRank = it->second;
		for (int32_t i = 0; i < m_BossScoreRankSize && i < (int32_t)vRank.size() && vRank[i].Cid > 0; ++i)
		{
			if (vRank[i].bChange)
			{
				vRank[i].bChange = 0;
				MySqlDBGuard db(DBPOOL);
				char szSQL[4096] = {};
				int32_t nNowTime = DayTime::now();
				snprintf(szSQL, sizeof(szSQL),
					"REPLACE INTO `mem_double_eleven_rank` (`type`, `cid`, `name`, `count`, `time`) "
					"VALUES (8, %lld, '%s', %d, %d)",
					(long long)vRank[i].Cid,
					vRank[i].strName.c_str(),
					vRank[i].nValue,
					nNowTime);
				db.excute(szSQL);
			}
		}
	}
}
void CFestivalDoubleEleven::sendBossScoreRankMail()
{
	if (GAME_SERVICE.getLine() != 1)
		return;

	MutexGuard lock(m_lock);
	for (std::map<int8_t, std::vector<FestivalRank>>::iterator it = m_mBossScoreRank.begin();
		it != m_mBossScoreRank.end(); ++it)
	{
		std::vector<FestivalRank>& vRank = it->second;
		for (int32_t i = 0; i < m_BossScoreRankSize && i < (int32_t)vRank.size(); ++i)
		{
			if (vRank[i].Cid > 0 && i < (int32_t)m_vBossScoreRankMail.size() && i < (int32_t)m_vBossScoreRankReward.size())
			{
				int32_t nMailId = m_vBossScoreRankMail[i];
				if (nMailId > 0)
					DB_SERVICE.OnSendSysMail(vRank[i].Cid, nMailId, m_vBossScoreRankReward[i]);
			}
		}
	}
}
