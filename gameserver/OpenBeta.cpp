#include "stdafx.h"
#include "Shared.h"
#include "OpenBeta.h"
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
#include "ItemHelper.h"

using namespace Answer;

COpenBeta::COpenBeta()
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
	m_nTitleShopSize = 0;
	m_nBossStartBroadcast = 0;
	m_nBossKillBroadcast = 0;
	m_nBossEndBroadcast = 0;
	m_nBossIcon = 0;
	m_nEndCollectCnt = 0;
	m_nEndCollectCntTime = 0;
	m_nEndCollectTime = 0;
	m_nDailyLimitShopSize = 0;
	m_nDailyLimitShop2Size = 0;
	m_nShiZhuangShopSize = 0;
	m_nRechargeDaysSize = 0;
	m_ChouJiangType = 0;
	m_nShowSize = 0;
	m_nXiaoFeiSumRankSize = 0;
	m_nXiaoFeiSumRankLimit = 0;
	m_FlopSize = 0;
	m_LiQuanSize = 0;
	m_QianDaoSize = 0;
	m_YanHuaSize = 0;
	m_nOnlineStartMinute = 0;
	m_nOnlineEndMinute = 0;
	m_nOnlineTimeArea = 0;
	m_nMaxCount = 0;
	m_MapId = 0;
	m_X = 0;
	m_Y = 0;
	m_Mid = 0;
	m_MonsterId = 0;
	m_bDie = 1;
	m_CurrYanHuaPoint = 0;
}

COpenBeta::~COpenBeta()
{
}

void COpenBeta::Init(int32_t line)
{
	initCfgData();

	MySqlDBGuard db(DBPOOL);
	char szSQL[4096] = {};
	snprintf(szSQL, sizeof(szSQL),
		"SELECT * FROM `sys_server_config` WHERE `name`='OPEN_BETA_VERSION'");
	MySqlQuery result = db.query(szSQL);
	int32_t nVersion = 0;
	if (!result.eof())
		nVersion = atoi(result.getStringValue("value", ""));

	if (nVersion > 0 && m_nVersion != nVersion || !nVersion)
	{
		int32_t nNowTime = DayTime::now();
		snprintf(szSQL, sizeof(szSQL),
			"INSERT INTO `sys_server_config` (`name`, `value`, `refresh_time`) "
			"VALUES ('OPEN_BETA_VERSION', %d, %d) ON DUPLICATE KEY UPDATE "
			"`value`=%d, `refresh_time`=%d",
			m_nVersion, nNowTime, m_nVersion, nNowTime);
		db.excute(szSQL);
	}

	const tm& localnow = TIMER.GetLocalNow();
	m_nMinute = localnow.tm_hour * 60 + localnow.tm_min;
}

void COpenBeta::initCfgData()
{
	Answer::Inifile ini;
	ini.parse("./ServerConfig/Tables/OpenBetaAct.cfg");

	// CONFIG
	{
		std::string strStartTime = ini.getStrValue("CONFIG", "start_time");
		if (!strStartTime.empty())
			m_nStartDay = DayTime::StringToIntTime(strStartTime);
		m_nEndDay = ini.getIntValue("CONFIG", "end_day");
		m_nIcon = ini.getIntValue("CONFIG", "icon");
	}

	// RED_PACK (type 1)
	{
		m_vStartDay[1] = ini.getIntValue("RED_PACK", "start_day");
		m_vEndDay[1] = ini.getIntValue("RED_PACK", "end_day");
		m_nOnlineStartMinute = ini.getIntValue("RED_PACK", "start_time");
		m_nOnlineEndMinute = ini.getIntValue("RED_PACK", "end_time");
		m_nOnlineTimeArea = 60 * ini.getIntValue("RED_PACK", "dis_time");
		std::string strReward = ini.getStrValue("RED_PACK", "reward");
		if (!strReward.empty())
			m_vOnlineReward = CItemHelper::parseItemString(0, strReward);
		m_nMaxCount = ini.getIntValue("RED_PACK", "max_count");
	}

	// RECHARGE (type 2)
	{
		m_vStartDay[2] = ini.getIntValue("RECHARGE", "start_day");
		m_vEndDay[2] = ini.getIntValue("RECHARGE", "end_day");
		m_nRechargeDaysSize = ini.getIntValue("RECHARGE", "size");
		m_vRechargeValue.resize(m_nRechargeDaysSize);
		m_vRechargeGift.resize(m_nRechargeDaysSize);
		char szCol[64] = {};
		for (int32_t i = 0; i < m_nRechargeDaysSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "gold%d", i + 1);
			m_vRechargeValue[i] = ini.getIntValue("RECHARGE", szCol);
			snprintf(szCol, sizeof(szCol), "gift%d", i + 1);
			std::string strGift = ini.getStrValue("RECHARGE", szCol);
			if (!strGift.empty())
				m_vRechargeGift[i] = CItemHelper::parseItemString(0, strGift);
		}
	}

	// DAILY_LIMIT_SHOP (type 3)
	{
		m_vStartDay[3] = ini.getIntValue("DAILY_LIMIT_SHOP", "start_day");
		m_vEndDay[3] = ini.getIntValue("DAILY_LIMIT_SHOP", "end_day");
		m_nDailyLimitShopSize = ini.getIntValue("DAILY_LIMIT_SHOP", "size");
		m_vDailyLimitShopGoods.resize(m_nDailyLimitShopSize);
		m_vDailyLimitShopBroadcast.resize(m_nDailyLimitShopSize);
		char szCol[64] = {};
		for (int32_t i = 0; i < m_nDailyLimitShopSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "price%d", i + 1);
			m_vDailyLimitShopGoods[i].nPrice = ini.getIntValue("DAILY_LIMIT_SHOP", szCol);
			snprintf(szCol, sizeof(szCol), "goods%d", i + 1);
			std::string strGoods = ini.getStrValue("DAILY_LIMIT_SHOP", szCol);
			if (!strGoods.empty())
				m_vDailyLimitShopGoods[i].vItems = CItemHelper::parseItemString(0, strGoods);
			snprintf(szCol, sizeof(szCol), "broadcast%d", i + 1);
			m_vDailyLimitShopBroadcast[i] = ini.getIntValue("DAILY_LIMIT_SHOP", szCol);
		}
	}

	// CHOU_JIANG (type 4)
	{
		m_vStartDay[4] = ini.getIntValue("CHOU_JIANG", "start_day");
		m_vEndDay[4] = ini.getIntValue("CHOU_JIANG", "end_day");
		m_ChouJiangType = ini.getIntValue("CHOU_JIANG", "Type");
	}

	// XIAO_FEI_SUM_RANK (type 5)
	{
		m_vStartDay[5] = ini.getIntValue("XIAO_FEI_SUM_RANK", "start_day");
		m_vEndDay[5] = ini.getIntValue("XIAO_FEI_SUM_RANK", "end_day");
		m_nShowSize = ini.getIntValue("XIAO_FEI_SUM_RANK", "show_size");
		m_nXiaoFeiSumRankSize = ini.getIntValue("XIAO_FEI_SUM_RANK", "size");
		m_nXiaoFeiSumRankLimit = ini.getIntValue("XIAO_FEI_SUM_RANK", "limit");
		m_vXiaoFeiSumRankMail.resize(m_nXiaoFeiSumRankSize);
		m_vXiaoFeiSumRankReward.resize(m_nXiaoFeiSumRankSize);
		char szCol[64] = {};
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

	// TITLE (type 6)
	{
		m_vStartDay[6] = ini.getIntValue("TITLE", "start_day");
		m_vEndDay[6] = ini.getIntValue("TITLE", "end_day");
		m_nTitleShopSize = ini.getIntValue("TITLE", "size");
		m_vTitleShopGoods.resize(m_nTitleShopSize);
		m_vTitleShopBroadcast.resize(m_nTitleShopSize);
		char szCol[64] = {};
		for (int32_t i = 0; i < m_nTitleShopSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "price%d", i + 1);
			m_vTitleShopGoods[i].nPrice = ini.getIntValue("TITLE", szCol);
			snprintf(szCol, sizeof(szCol), "goods%d", i + 1);
			std::string strGoods = ini.getStrValue("TITLE", szCol);
			if (!strGoods.empty())
				m_vTitleShopGoods[i].vItems = CItemHelper::parseItemString(0, strGoods);
			snprintf(szCol, sizeof(szCol), "broadcast%d", i + 1);
			m_vTitleShopBroadcast[i] = ini.getIntValue("TITLE", szCol);
		}
	}

	// SHI_ZHUANG (type 7)
	{
		m_vStartDay[7] = ini.getIntValue("SHI_ZHUANG", "start_day");
		m_vEndDay[7] = ini.getIntValue("SHI_ZHUANG", "end_day");
		m_nShiZhuangShopSize = ini.getIntValue("SHI_ZHUANG", "size");
		m_vShizhuangShopGoods.resize(m_nShiZhuangShopSize);
		m_vShiZhuangShopBroadcast.resize(m_nShiZhuangShopSize);
		char szCol[64] = {};
		for (int32_t i = 0; i < m_nShiZhuangShopSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "price%d", i + 1);
			m_vShizhuangShopGoods[i].nPrice = ini.getIntValue("SHI_ZHUANG", szCol);
			snprintf(szCol, sizeof(szCol), "goods%d", i + 1);
			std::string strGoods = ini.getStrValue("SHI_ZHUANG", szCol);
			if (!strGoods.empty())
				m_vShizhuangShopGoods[i].vItems = CItemHelper::parseItemString(0, strGoods);
			snprintf(szCol, sizeof(szCol), "broadcast%d", i + 1);
			m_vShiZhuangShopBroadcast[i] = ini.getIntValue("SHI_ZHUANG", szCol);
		}
	}

	// COLLECT_DROP (type 8)
	{
		m_vStartDay[8] = ini.getIntValue("COLLECT_DROP", "start_day");
		m_vEndDay[8] = ini.getIntValue("COLLECT_DROP", "end_day");
	}

	// BOSS (type 9)
	{
		m_vStartDay[9] = ini.getIntValue("BOSS", "start_day");
		m_vEndDay[9] = ini.getIntValue("BOSS", "end_day");
		m_nBossIcon = ini.getIntValue("BOSS", "icon");
		m_nBossStartBroadcast = ini.getIntValue("BOSS", "start_broadcast");
		m_nBossEndBroadcast = ini.getIntValue("BOSS", "end_broadcast");
		m_nBossKillBroadcast = ini.getIntValue("BOSS", "kill_broadcast");
		int32_t nBossCount = ini.getIntValue("BOSS", "count");
		m_vBossMinute.resize(nBossCount);
		char szCol[64] = {};
		for (int32_t i = 0; i < nBossCount; ++i)
		{
			snprintf(szCol, sizeof(szCol), "start_minute%d", i + 1);
			m_vBossMinute[i].nStartTime = ini.getIntValue("BOSS", szCol);
			snprintf(szCol, sizeof(szCol), "end_minute%d", i + 1);
			m_vBossMinute[i].nEndTime = ini.getIntValue("BOSS", szCol);
		}
		m_MapId = ini.getIntValue("BOSS", "mapid");
		m_X = ini.getIntValue("BOSS", "x");
		m_Y = ini.getIntValue("BOSS", "y");
		m_Mid = ini.getIntValue("BOSS", "mid");
	}

	// OBT_LI_QUAN (type 10)
	{
		m_vStartDay[10] = ini.getIntValue("OBT_LI_QUAN", "start_day");
		m_vEndDay[10] = ini.getIntValue("OBT_LI_QUAN", "end_day");
		m_LiQuanSize = ini.getIntValue("OBT_LI_QUAN", "size");
		m_LiQuanItem.resize(m_LiQuanSize);
		m_ItemDataVector.resize(m_LiQuanSize);
		m_LiQuanPriceVt.resize(m_LiQuanSize);
		m_LiQuanLimitCount.resize(m_LiQuanSize);
		char szCol[64] = {};
		for (int32_t i = 0; i < m_LiQuanSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "item%d", i + 1);
			std::string strItem = ini.getStrValue("OBT_LI_QUAN", szCol);
			MemChrBagVector itemVec = CItemHelper::parseItemString(0, strItem);
			if (!itemVec.empty())
				m_LiQuanItem[i] = itemVec[0];
			snprintf(szCol, sizeof(szCol), "price%d", i + 1);
			m_LiQuanPriceVt[i] = ini.getIntValue("OBT_LI_QUAN", szCol);
			snprintf(szCol, sizeof(szCol), "limit%d", i + 1);
			m_LiQuanLimitCount[i] = ini.getIntValue("OBT_LI_QUAN", szCol);
			snprintf(szCol, sizeof(szCol), "costitem%d", i + 1);
			// costitem not parsed - CItemHelper::parseItemDataString not available
		}
	}

	// OBT_FLOP (type 11)
	{
		m_vStartDay[11] = ini.getIntValue("OBT_FLOP", "start_day");
		m_vEndDay[11] = ini.getIntValue("OBT_FLOP", "end_day");
		m_FlopSize = ini.getIntValue("OBT_FLOP", "size");
		m_FlopType.resize(m_FlopSize);
		ItemData emptyData;
		memset(&emptyData, 0, sizeof(emptyData));
		m_FlopCost.resize(m_FlopSize, emptyData);
		char szCol[64] = {};
		for (int32_t i = 0; i < m_FlopSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "type%d", i + 1);
			m_FlopType[i] = ini.getIntValue("OBT_FLOP", szCol);
			snprintf(szCol, sizeof(szCol), "cost%d", i + 1);
			// Cost not parsed - would use CItemHelper::parseItemDataString
		}
	}	// OBT_QIAN_DAO (type 12)
	{
		m_vStartDay[12] = ini.getIntValue("OBT_QIAN_DAO", "start_day");
		m_vEndDay[12] = ini.getIntValue("OBT_QIAN_DAO", "end_day");
		m_QianDaoSize = ini.getIntValue("OBT_QIAN_DAO", "size");
		m_QianDaoReward.resize(m_QianDaoSize);
		m_BuQianPrice.resize(m_QianDaoSize);
		char szCol[64] = {};
		for (int32_t i = 0; i < m_QianDaoSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "item%d", i + 1);
			std::string strItem = ini.getStrValue("OBT_QIAN_DAO", szCol);
			MemChrBagVector itemVec = CItemHelper::parseItemString(0, strItem);
			if (!itemVec.empty())
				m_QianDaoReward[i] = itemVec[0];
			snprintf(szCol, sizeof(szCol), "price%d", i + 1);
			m_BuQianPrice[i] = ini.getIntValue("OBT_QIAN_DAO", szCol);
		}
	}

	// OBT_YAN_HUA (type 13)
	{
		m_vStartDay[13] = ini.getIntValue("OBT_YAN_HUA", "start_day");
		m_vEndDay[13] = ini.getIntValue("OBT_YAN_HUA", "end_day");
		m_YanHuaSize = ini.getIntValue("OBT_YAN_HUA", "size");
		m_YanHuaReward.resize(m_YanHuaSize);
		m_YanHuaPointVt.resize(m_YanHuaSize);
		char szCol[64] = {};
		for (int32_t i = 0; i < m_YanHuaSize; ++i)
		{
			snprintf(szCol, sizeof(szCol), "item%d", i + 1);
			std::string strItem = ini.getStrValue("OBT_YAN_HUA", szCol);
			MemChrBagVector itemVec = CItemHelper::parseItemString(0, strItem);
			if (!itemVec.empty())
				m_YanHuaReward[i] = itemVec[0];
			snprintf(szCol, sizeof(szCol), "point%d", i + 1);
			m_YanHuaPointVt[i] = ini.getIntValue("OBT_YAN_HUA", szCol);
		}
	}

	// OBT_GUO_QING_TASK (type 16)
	{
		m_vStartDay[16] = ini.getIntValue("OBT_GUO_QING_TASK", "start_day");
		m_vEndDay[16] = ini.getIntValue("OBT_GUO_QING_TASK", "end_day");
	}

	// OBT_GUO_QING_REWARD (type 17)
	{
		m_vStartDay[17] = ini.getIntValue("OBT_GUO_QING_REWARD", "start_day");
		m_vEndDay[17] = ini.getIntValue("OBT_GUO_QING_REWARD", "end_day");
	}

	m_nDay = (DayTime::dayzero(DayTime::now()) - DayTime::dayzero(m_nStartDay)) / 86400;

	loadXiaoFeiSumRank();
	LoadYanHuaValue();
}

void COpenBeta::OnDaySwitch()
{
	int32_t dayzero = DayTime::dayzero(DayTime::now());
	m_nDay = (dayzero - DayTime::dayzero(m_nStartDay)) / 86400;
}

void COpenBeta::OnNewMinute(int32_t nMinute)
{
	m_nMinute = nMinute;
	CheckBoss();
	checkXiaoFeiSumRank();
}

void COpenBeta::updateStartTime()
{
	m_nDay = (DayTime::dayzero(DayTime::now()) - DayTime::dayzero(m_nStartDay)) / 86400;
}

int32_t COpenBeta::GetFlopType(int32_t nFlopId)
{
	if (GAME_SERVICE.getLine() == 9)
		return 0;
	if (nFlopId >= 0 && nFlopId < (int32_t)m_FlopType.size())
		return m_FlopType[nFlopId];
	return 0;
}

bool COpenBeta::CanFlopDraw(Player* pPlayer, int32_t FlopId)
{
	if (GAME_SERVICE.getLine() == 9 || !pPlayer)
		return false;
	if (!IsInTime(11) || FlopId < 0 || FlopId >= (int32_t)m_FlopCost.size())
		return false;

	int32_t FlopTimes = pPlayer->getRecord(FlopId + 19003);
	if (FlopTimes >= GetMaxFlopTimes(pPlayer))
		return false;

	return true;
}

int32_t COpenBeta::GetMaxFlopTimes(Player* pPlayer)
{
	if (GAME_SERVICE.getLine() == 9 || !pPlayer || !IsInTime(11))
		return 0;
	return 1;
}

void COpenBeta::GetIconState(Player* player, IconStateList* IconList)
{
	if (player && GAME_SERVICE.getLine() != 9 && m_nDay >= 0 && m_nDay < m_nEndDay)
	{
		ShowIcon icon;
		getIconState(&icon, player);
		IconList->push_back(icon);
	}
}

void COpenBeta::SendIconState(Player* player)
{
	if (player && GAME_SERVICE.getLine() != 9 && IsOpenBetaTime())
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

ShowIcon* COpenBeta::getIconState(ShowIcon* retstr, Player* player)
{
	memset(retstr, 0, sizeof(ShowIcon));
	if (player)
	{
		retstr->nId = m_nIcon;
		retstr->nState = IsOpenBetaTime() ? 2 : 4;
		retstr->nLeftTime = getLeftTime();
		if (IsInTime(1) && canGetOnlineGift(player))
			retstr->IconLeft = 1;
		if (IsInTime(12))
			retstr->IconRight += 1;
		if (IsInTime(13))
			retstr->IconRight += 1;
		if (IsInTime(11) && CanFlopDraw(player, 0))
			retstr->IconRight += 1;
		if (IsInTime(16))
			retstr->IconRight += 1;
	}
	return retstr;
}

void COpenBeta::SendActivityInfo(Player* player)
{
	if (!player || GAME_SERVICE.getLine() == 9)
		return;

	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2EBD);
	if (packet)
	{
		packet->writeInt8((int8_t)m_nTitleShopSize);
		int32_t nRecord = player->GetOperateLimit().GetLimitCount(1917);
		for (int32_t i = 0; i < m_nTitleShopSize; ++i)
			packet->writeInt8((nRecord >> i) & 1);
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.sendPacketTo(player->getGateIndex(), packet);
	}
}

void COpenBeta::GetBossIconState(IconStateList* IconList)
{
	if (GAME_SERVICE.getLine() != 9 && IsInBossTime())
	{
		ShowIcon icon;
		getBossIconState(&icon);
		IconList->push_back(icon);
	}
}

void COpenBeta::SendBossIconState()
{
	if (GAME_SERVICE.getLine() == 1)
	{
		ShowIcon stu;
		getBossIconState(&stu);
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

ShowIcon* COpenBeta::getBossIconState(ShowIcon* retstr)
{
	memset(retstr, 0, sizeof(ShowIcon));
	if (IsInBossTime() && !m_bDie)
	{
		retstr->nId = 65;
		retstr->nState = 2;
	}
	return retstr;
}

bool COpenBeta::IsInBossTime()
{
	return GAME_SERVICE.getLine() != 9 && IsInTime(9);
}

void COpenBeta::BossDie(int32_t MonsterId, std::string* p_name, CharId_t Cid)
{
	if (m_MonsterId == MonsterId)
	{
		m_bDie = 1;
		BroadcastBossKilled(p_name, Cid);
		SendBossIconState();
	}
}

void COpenBeta::BroadcastBossKilled(const std::string* name, CharId_t cid)
{
	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CD6);
	if (packet)
	{
		packet->writeInt32(0);
		packet->writeInt64(cid);
		packet->writeUTF8(name->c_str());
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.worldBroadcast(packet);
	}
}

void COpenBeta::CheckBoss()
{
	if (GAME_SERVICE.getLine() == 9 || !IsInTime(9) || m_vBossMinute.empty())
		return;
	if (GAME_SERVICE.getLine() != 1)
		return;

	for (uint32_t i = 0; i < m_vBossMinute.size(); ++i)
	{
		if (m_nMinute == m_vBossMinute[i].nStartTime)
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
			m_MonsterId = MapMonster.id;

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
			broadcastBossStart();
			SendBossIconState();
		}
		else if (m_nMinute == m_vBossMinute[i].nEndTime && !m_bDie)
		{
			m_bDie = 1;
			broadcastBossEnd();
			SendBossIconState();
		}
	}
}

void COpenBeta::broadcastBossStart()
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

void COpenBeta::broadcastBossEnd()
{
	if (GAME_SERVICE.getLine() == 9)
		return;

	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CD6);
	if (packet)
	{
		packet->writeInt32(0);
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.worldBroadcast(packet);
	}
}

int32_t COpenBeta::GetRechargeGift(Player* player, int32_t nIndex)
{
	if (!player || GAME_SERVICE.getLine() == 9)
		return 10002;
	if (!IsInTime(2) || nIndex < 0 || nIndex >= (int32_t)m_vRechargeValue.size())
		return 10002;

	int32_t nRewardRecord = player->GetOperateLimit().GetLimitCount(2091);
	int32_t TodayPayGold = player->GetTodayPayGold();
	if (TodayPayGold < m_vRechargeValue[nIndex])
		return 10002;

	int32_t nNewRecord = nRewardRecord | (1 << nIndex);
	if (nRewardRecord == nNewRecord)
		return 10002;

	for (uint32_t i = 0; i < m_vRechargeGift[nIndex].size(); ++i)
	{
		if (!player->GetBag().AddItem(m_vRechargeGift[nIndex][i], IACR_NONE))
			return 10002;
	}

	player->GetOperateLimit().UpdateLimitCount(2091, nNewRecord);
	SendIconState(player);
	SendResult(player, 2, nIndex, 0);
	return 0;
}

int32_t COpenBeta::BuyGiftShopItem(Player* player, int8_t nIndex)
{
	if (!player || GAME_SERVICE.getLine() == 9)
		return 10002;
	if (!IsInTime(3) || nIndex < 0 || nIndex >= m_nDailyLimitShopSize)
		return 10002;

	int32_t nOldRecord = player->GetOperateLimit().GetLimitCount(2092);
	int32_t nNewRecord = nOldRecord | (1 << nIndex);
	if (nOldRecord == nNewRecord)
		return 10002;

	int32_t nPrice = m_vDailyLimitShopGoods[nIndex].nPrice;
	if (player->GetCurrency(CURRENCY_GOLD) < nPrice)
		return 10002;

	for (uint32_t i = 0; i < m_vDailyLimitShopGoods[nIndex].vItems.size(); ++i)
	{
		if (!player->GetBag().AddItem(m_vDailyLimitShopGoods[nIndex].vItems[i], IACR_NONE))
			return 10002;
	}

	if (!player->DecCurrency(CURRENCY_GOLD, (int64_t)nPrice, GM_NONE, 0))
		return 10002;

	player->GetOperateLimit().UpdateLimitCount(2092, nNewRecord);

	if (m_vDailyLimitShopBroadcast[nIndex] > 0)
	{
		std::string name = player->getName();
		CharId_t Cid = player->getCid();
		NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CD6);
		if (packet)
		{
			packet->writeInt32(m_vDailyLimitShopBroadcast[nIndex]);
			packet->writeInt64(Cid);
			packet->writeUTF8(name.c_str());
			packet->setSize(packet->getWOffset());
			GAME_SERVICE.worldBroadcast(packet);
		}
	}

	SendResult(player, 3, nIndex, 0);
	return 0;
}

int32_t COpenBeta::BuyTitleShopItem(Player* player, int8_t nIndex)
{
	if (!player || GAME_SERVICE.getLine() == 9)
		return 10002;
	if (!IsInTime(6) || nIndex < 0 || nIndex >= (int32_t)m_vTitleShopGoods.size())
		return 10002;

	int32_t nOldRecord = player->GetOperateLimit().GetLimitCount(1917);
	int32_t nNewRecord = nOldRecord | (1 << nIndex);
	if (nOldRecord == nNewRecord)
		return 10002;

	int32_t nPrice = m_vTitleShopGoods[nIndex].nPrice;
	if (player->GetCurrency(CURRENCY_GOLD) < nPrice)
		return 10002;

	if (!player->DecCurrency(CURRENCY_GOLD, (int64_t)nPrice, GM_NONE, 0))
		return 10002;

	player->GetOperateLimit().UpdateLimitCount(1917, nNewRecord);

	if (m_vTitleShopBroadcast[nIndex] > 0)
	{
		std::string name = player->getName();
		CharId_t Cid = player->getCid();
		NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CD6);
		if (packet)
		{
			packet->writeInt32(m_vTitleShopBroadcast[nIndex]);
			packet->writeInt64(Cid);
			packet->writeUTF8(name.c_str());
			packet->setSize(packet->getWOffset());
			GAME_SERVICE.worldBroadcast(packet);
		}
	}

	SendResult(player, 6, nIndex, 0);
	return 0;
}

int32_t COpenBeta::BuyShiZhuangShopItem(Player* player, int8_t nIndex)
{
	if (!player || GAME_SERVICE.getLine() == 9)
		return 10002;
	if (!IsInTime(7) || nIndex < 0 || nIndex >= (int32_t)m_vShizhuangShopGoods.size())
		return 10002;

	int32_t nOldRecord = player->GetOperateLimit().GetLimitCount(1918);
	int32_t nNewRecord = nOldRecord | (1 << nIndex);
	if (nOldRecord == nNewRecord)
		return 10002;

	int32_t nPrice = m_vShizhuangShopGoods[nIndex].nPrice;
	if (player->GetCurrency(CURRENCY_GOLD) < nPrice)
		return 10002;

	if (!player->DecCurrency(CURRENCY_GOLD, (int64_t)nPrice, GM_NONE, 0))
		return 10002;

	player->GetOperateLimit().UpdateLimitCount(1918, nNewRecord);

	if (m_vShiZhuangShopBroadcast[nIndex] > 0)
	{
		std::string name = player->getName();
		CharId_t Cid = player->getCid();
		NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CD6);
		if (packet)
		{
			packet->writeInt32(m_vShiZhuangShopBroadcast[nIndex]);
			packet->writeInt64(Cid);
			packet->writeUTF8(name.c_str());
			packet->setSize(packet->getWOffset());
			GAME_SERVICE.worldBroadcast(packet);
		}
	}

	SendResult(player, 7, nIndex, 0);
	return 0;
}

int32_t COpenBeta::BuyLiQuan(Player* player, int8_t nIndex)
{
	if (GAME_SERVICE.getLine() == 9 || !player)
		return 10002;
	if (!IsInTime(10) || nIndex < 0 || nIndex >= (int32_t)m_LiQuanItem.size())
		return 10002;

	if (m_LiQuanLimitCount[nIndex] <= player->getRecord(nIndex + 10000))
		return 10002;

	if (m_LiQuanPriceVt[nIndex] > 0 && player->GetCurrency(CURRENCY_GOLD) < m_LiQuanPriceVt[nIndex])
		return 10002;

	if (player->GetBag().GetFreeSlotCount() <= 0)
		return 10002;

	if (!player->GetBag().RemoveItem(m_ItemDataVector[nIndex], IDCR_NONE))
		return 10002;

	if (m_LiQuanPriceVt[nIndex] > 0)
	{
		if (!player->DecCurrency(CURRENCY_GOLD, (int64_t)m_LiQuanPriceVt[nIndex], GM_NONE, 0))
			return 10002;
	}

	player->updateRecord(nIndex + 10000, player->getRecord(nIndex + 10000) + 1);
	player->GetBag().AddItem(m_LiQuanItem[nIndex], IACR_NONE);
	return 0;
}

void COpenBeta::AddOnlineRecord(Player* player)
{
	if (GAME_SERVICE.getLine() == 9 || !IsInTime(1) || !player)
		return;

	int32_t nOnlineTime = player->GetOperateLimit().GetLimitCount(2093);
	int32_t nRewardTimes = player->GetOperateLimit().GetLimitCount(2094);

	if (m_vOnlineReward.size() > (uint32_t)nRewardTimes)
	{
		player->GetOperateLimit().AddLimitCount(2093, 1);
	}
}

bool COpenBeta::canGetOnlineGift(Player* player)
{
	if (!player || !IsInTime(1))
		return false;

	int32_t nOnlineTime = player->GetOperateLimit().GetLimitCount(2093);
	int32_t nRewardTimes = player->GetOperateLimit().GetLimitCount(2094);

	if (nRewardTimes >= (int32_t)m_vOnlineReward.size())
		return false;

	return nOnlineTime > nRewardTimes;
}

int32_t COpenBeta::GetOnlineTimeRewardTime(Player* player)
{
	if (!player || !IsInTime(1))
		return 0;
	int32_t nOnlineTime = player->GetOperateLimit().GetLimitCount(2093);
	int32_t nRewardTimes = player->GetOperateLimit().GetLimitCount(2094);
	int32_t nNeedTime = (nRewardTimes + 1) * m_nOnlineTimeArea;
	if (nOnlineTime >= nNeedTime)
		return nOnlineTime - nRewardTimes * m_nOnlineTimeArea;
	return 0;
}

int32_t COpenBeta::GetOnlineGift(Player* player)
{
	if (!player || GAME_SERVICE.getLine() == 9)
		return 10002;
	if (!canGetOnlineGift(player))
		return 10002;

	int32_t nRewardTimes = player->GetOperateLimit().GetLimitCount(2094);
	if (nRewardTimes >= (int32_t)m_vOnlineReward.size())
		return 10002;

	if (!player->GetBag().AddItem(m_vOnlineReward[nRewardTimes], IACR_NONE))
		return 10002;

	player->GetOperateLimit().AddLimitCount(2094, 1);
	SendIconState(player);
	SendResult(player, 1, 0, 0);
	return 0;
}

bool COpenBeta::IsInTime(int8_t Type)
{
	if (GAME_SERVICE.getLine() == 9)
		return false;
	if (Type <= 0 || Type > 17)
		return false;
	return m_nDay >= m_vStartDay[Type] && m_nDay < m_vEndDay[Type];
}

int32_t COpenBeta::getLeftTime()
{
	if (GAME_SERVICE.getLine() == 9)
		return 0;
	return 86400 * (m_nEndDay - m_nDay) - 60 * m_nMinute;
}

bool COpenBeta::IsOpenBetaTime()
{
	if (GAME_SERVICE.getLine() == 9)
		return false;
	return m_nDay >= 0 && m_nDay < m_nEndDay;
}

int32_t COpenBeta::GetEndCollectDropTime()
{
	if (!IsInTime(8))
		return 0;
	return m_nEndCollectTime;
}

int32_t COpenBeta::GetChouJiangType()
{
	if (IsInTime(4))
		return 1;
	if (IsInTime(5))
		return 2;
	return 0;
}

void COpenBeta::LoadYanHuaValue()
{
	MySqlDBGuard db(DBPOOL);
	char szSQL[4096] = {};
	snprintf(szSQL, sizeof(szSQL),
		"SELECT * FROM `sys_server_config` WHERE `name`='OPEN_BETA_YAN_HUA'");
	MySqlQuery result = db.query(szSQL);
	if (!result.eof())
		m_CurrYanHuaPoint = atoi(result.getStringValue("value", "0"));
}

void COpenBeta::UpdateYanHuaValue()
{
	MySqlDBGuard db(DBPOOL);
	char szSQL[4096] = {};
	int32_t nNowTime = DayTime::now();
	snprintf(szSQL, sizeof(szSQL),
		"INSERT INTO `sys_server_config` (`name`, `value`, `refresh_time`) "
		"VALUES ('OPEN_BETA_YAN_HUA', %d, %d) ON DUPLICATE KEY UPDATE "
		"`value`=%d, `refresh_time`=%d",
		m_CurrYanHuaPoint, nNowTime, m_CurrYanHuaPoint, nNowTime);
	db.excute(szSQL);
}

void COpenBeta::AddYanHuaPoint(int32_t Values)
{
	if (GAME_SERVICE.getLine() != 9 && IsInTime(13))
	{
		m_CurrYanHuaPoint += Values;
		UpdateYanHuaValue();
	}
}

int32_t COpenBeta::GetYanHuaPointReward(Player* player, int8_t nIndex)
{
	if (!player || !IsInTime(13))
		return 10002;
	if (nIndex < 0 || nIndex >= (int32_t)m_YanHuaPointVt.size())
		return 10002;
	if (m_CurrYanHuaPoint < m_YanHuaPointVt[nIndex])
		return 10002;

	int32_t nRecord = player->getRecord(nIndex + 19100);
	if (nRecord > 0)
		return 10002;

	if (!player->GetBag().AddItem(m_YanHuaReward[nIndex], IACR_NONE))
		return 10002;

	player->updateRecord(nIndex + 19100, 1);
	SendResult(player, 13, nIndex, 0);
	SendIconState(player);
	return 0;
}

int32_t COpenBeta::GetQianDaoReward(Player* player, int8_t nIndex, int8_t nType)
{
	if (!player || !IsInTime(12))
		return 10002;
	if (nIndex < 0 || nIndex >= (int32_t)m_QianDaoReward.size())
		return 10002;

	int32_t nRecord = player->getRecord(nIndex + 21001);
	if (nRecord > 0)
		return 10002;

	if (nType == 1)
	{
		int32_t nBuQian = m_BuQianPrice[nIndex];
		if (player->GetCurrency(CURRENCY_GOLD) < nBuQian)
			return 10002;
		if (!player->DecCurrency(CURRENCY_GOLD, (int64_t)nBuQian, GM_NONE, 0))
			return 10002;
	}

	if (!player->GetBag().AddItem(m_QianDaoReward[nIndex], IACR_NONE))
		return 10002;

	player->updateRecord(nIndex + 21001, 1);
	SendResult(player, 12, nIndex, 0);
	SendIconState(player);
	return 0;
}

void COpenBeta::loadXiaoFeiSumRank()
{
	MySqlDBGuard db(DBPOOL);
	char szSQL[4096] = {};
	snprintf(szSQL, sizeof(szSQL),
		"SELECT * FROM `sys_server_config` WHERE `name`='OPEN_BETA_XIAO_FEI_RANK'");
	MySqlQuery result = db.query(szSQL);
	if (!result.eof())
	{
		std::string data = result.getStringValue("value", "");
		if (!data.empty())
		{
			StringVector rankList = StringUtility::split(data, ";");
			for (uint32_t i = 0; i < rankList.size(); ++i)
			{
				StringVector rankInfo = StringUtility::split(rankList[i], ":");
				if (rankInfo.size() == 3)
				{
					FestivalRank rank;
					rank.Cid = atoll(rankInfo[0].c_str());
					rank.nValue = atoi(rankInfo[1].c_str());
					rank.nUpdateTime = atoi(rankInfo[2].c_str());
					m_mXiaoFeiSumRank[0].push_back(rank);
				}
			}
		}
	}
}

void COpenBeta::updateXiaoFeiSumRank(Player* player, int32_t AddValues)
{
	if (!player)
		return;
	if (GAME_SERVICE.getLine() != 9 && IsInTime(5))
	{
		MutexGuard lock(m_lock);
		CharId_t Cid = player->getCid();
		std::vector<FestivalRank>& rankList = m_mXiaoFeiSumRank[0];
		bool found = false;
		for (uint32_t i = 0; i < rankList.size(); ++i)
		{
			if (rankList[i].Cid == Cid)
			{
				rankList[i].nValue += AddValues;
				rankList[i].nUpdateTime = DayTime::now();
				found = true;
				break;
			}
		}
		if (!found)
		{
			FestivalRank rank;
			rank.Cid = Cid;
			rank.nValue = AddValues;
			rank.nUpdateTime = DayTime::now();
			rankList.push_back(rank);
		}
		checkXiaoFeiSumRankInvalid(0);
	}
}

void COpenBeta::checkXiaoFeiSumRankInvalid(int8_t connid)
{
	MySqlDBGuard db(DBPOOL);
	std::string data;
	for (uint32_t i = 0; i < m_mXiaoFeiSumRank[0].size(); ++i)
	{
		if (!data.empty())
			data += ";";
		char buf[256] = {};
		snprintf(buf, sizeof(buf), "%lld:%d:%d",
			(long long)m_mXiaoFeiSumRank[0][i].Cid,
			m_mXiaoFeiSumRank[0][i].nValue,
			m_mXiaoFeiSumRank[0][i].nUpdateTime);
		data += buf;
	}

	char szSQL[8192] = {};
	int32_t nNowTime = DayTime::now();
	snprintf(szSQL, sizeof(szSQL),
		"INSERT INTO `sys_server_config` (`name`, `value`, `refresh_time`) "
		"VALUES ('OPEN_BETA_XIAO_FEI_RANK', '%s', %d) ON DUPLICATE KEY UPDATE "
		"`value`='%s', `refresh_time`=%d",
		data.c_str(), nNowTime, data.c_str(), nNowTime);
	db.excute(szSQL);
}

void COpenBeta::checkXiaoFeiSumRank()
{
	if (GAME_SERVICE.getLine() != 9 && IsInTime(5))
	{
		sendXiaoFeiSumRankMail();
	}
}

void COpenBeta::sendXiaoFeiSumRankMail()
{
	if (GAME_SERVICE.getLine() != 1)
		return;

	MutexGuard lock(m_lock);
	std::vector<FestivalRank>& vRank = m_mXiaoFeiSumRank[0];
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

void COpenBeta::sendBroadcast(Player* pPlayer, int32_t nType, CharId_t Cid, const std::string& name)
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

void COpenBeta::SendResult(Player* pPlayer, int8_t Type, int32_t Index, int32_t param)
{
	if (!pPlayer)
		return;

	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2EBE);
	if (packet)
	{
		packet->writeInt32(Type);
		packet->writeInt32(Index);
		packet->writeInt32(param);
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.sendPacketTo(pPlayer->getGateIndex(), packet);
	}
}
