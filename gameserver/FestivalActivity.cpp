#include "stdafx.h"
#include "Shared.h"
#include "FestivalActivity.h"
#include "GameService.h"
#include "ActivityMap.h"
#include "Timer.h"
#include "DayTime.h"
#include "MapManager.h"
#include "Map.h"
#include "Plant.h"
#include "Npc.h"
#include "DBService.h"

using namespace Answer;

CFestivalActivity::CFestivalActivity(const CfgActivity& cfgActivity)
	: CActivity(cfgActivity)
{
	m_nDay = -1;
	m_nMinute = 0;
}

CFestivalActivity::~CFestivalActivity()
{
}

void CFestivalActivity::Init()
{
	initCfgData();

	int32_t nVersion = loadVersion();
	if (nVersion > 0 && m_nVersion != nVersion || !nVersion)
		saveVersion();

	const tm& localnow = TIMER.GetLocalNow();
	m_nMinute = localnow.tm_hour * 60 + localnow.tm_min;
}

void CFestivalActivity::initCfgData()
{
	memset(m_vStartDay, 0, sizeof(m_vStartDay));
	memset(m_vEndDay, 0, sizeof(m_vEndDay));

	Answer::Inifile ini;
	ini.parse("./ServerConfig/Tables/FestivalActivity.cfg");

	char szCol[64] = {};

	// [CONFIG]
	m_nVersion	= ini.getIntValue("CONFIG", "version");
	m_nStartDay	= DayTime::StringToIntTime(ini.getStrValue("CONFIG", "start_time"));
	m_nEndDay	= ini.getIntValue("CONFIG", "end_day");
	m_nIcon		= ini.getIntValue("CONFIG", "icon");

	initOpenList(ini.getStrValue("CONFIG", "open"));

	int32_t now = DayTime::now();
	int32_t dayzero = DayTime::dayzero(now);
	m_nDay = (dayzero - DayTime::dayzero(m_nStartDay)) / 86400;

	// [COLLECTION]
	if (isOpen(FAT2_COLLECTION))
	{
		m_vStartDay[1]	= ini.getIntValue("COLLECTION", "start_day");
		m_vEndDay[1]	= ini.getIntValue("COLLECTION", "end_day");
		m_PlantId		= ini.getIntValue("COLLECTION", "plant");
		m_PlantCount	= ini.getIntValue("COLLECTION", "count");
		m_Times			= ini.getIntValue("COLLECTION", "time");

		std::string mapStr = ini.getStrValue("COLLECTION", "map");
		if (!mapStr.empty() && mapStr != "-1")
		{
			StringVector splitStr = StringUtility::split(mapStr, "|");
			for (uint32_t i = 0; i < splitStr.size(); ++i)
			{
				StringVector vstr = StringUtility::split(splitStr[i], ":");
				if (vstr.size() == 3)
				{
					MapPos pos;
					pos.m_MapId		= atoi(vstr[0].c_str());
					pos.m_Pos.x		= atoi(vstr[1].c_str());
					pos.m_Pos.y		= atoi(vstr[2].c_str());
					m_PlantMapPosVt.push_back(pos);
				}
			}
		}

		int32_t nLeftTime = getLeftTime();
		if (nLeftTime > 0)
		{
			CfgPlant* pCfgPlant = CFG_DATA.getPlant(m_PlantId);
			if (pCfgPlant)
			{
				std::random_shuffle(m_PlantMapPosVt.begin(), m_PlantMapPosVt.end());
				int32_t nFreshCount = (int32_t)m_PlantMapPosVt.size() <= m_PlantCount
					? (int32_t)m_PlantMapPosVt.size() : m_PlantCount;
				for (int32_t i = 0; i < nFreshCount; ++i)
				{
					MapId_t mapId = m_PlantMapPosVt[i].m_MapId;
					Map* pMap = MAP_MANAGER.GetMap(mapId);
					Plant* plant = new Plant;
					if (plant && pMap)
					{
						CfgMapPlant stu;
						memset(&stu, 0, sizeof(stu));
						stu.mapid	= m_PlantMapPosVt[i].m_MapId;
						stu.plantid	= m_PlantId;
						stu.x		= m_PlantMapPosVt[i].m_Pos.x;
						stu.y		= m_PlantMapPosVt[i].m_Pos.y;
						plant->init(pMap, *pCfgPlant, stu);
						pMap->addPlant(plant);
					}
				}
			}
		}
	}

	// [DATI]
	if (isOpen(FAT2_DA_TI))
	{
		m_vStartDay[2]	= ini.getIntValue("DATI", "start_day");
		m_vEndDay[2]	= ini.getIntValue("DATI", "end_day");

		std::string giftDayStr = ini.getStrValue("DATI", "gift_day");
		MemChrBagVector dayGiftVec = CItemHelper::parseItemString(0, giftDayStr);
		if (!dayGiftVec.empty())
			m_DayGift = dayGiftVec[0];

		std::string giftActStr = ini.getStrValue("DATI", "gift_act");
		MemChrBagVector actGiftVec = CItemHelper::parseItemString(0, giftActStr);
		if (!actGiftVec.empty())
			m_ActGift = actGiftVec[0];

		int32_t nDay = m_vEndDay[2] - m_vStartDay[2];
		if (nDay > 0)
		{
			m_StringVtVector.resize(nDay);
			for (int32_t i = 0; i < nDay; ++i)
			{
				snprintf(szCol, sizeof(szCol), "da_an%d", i + 1);
				std::string daAnStr = ini.getStrValue("DATI", szCol);
				if (!daAnStr.empty() && daAnStr != "-1")
				{
					StringVector splitStr = StringUtility::split(daAnStr, "|");
					int32_t nSize = (int32_t)splitStr.size();
					if (nSize > 0)
					{
						m_StringVtVector[i].resize(nSize);
						for (int32_t j = 0; j < nSize; ++j)
							m_StringVtVector[i][j] = splitStr[j];
					}
				}
			}
		}
	}

	// [NPC]
	if (isOpen(FAT2_NPC))
	{
		m_vStartDay[3]	= ini.getIntValue("NPC", "start_day");
		m_vEndDay[3]	= ini.getIntValue("NPC", "end_day");
		m_NpcId			= ini.getIntValue("NPC", "npcid");
		m_NpcCount		= ini.getIntValue("NPC", "count");

		std::string mapStr = ini.getStrValue("NPC", "map");
		if (!mapStr.empty() && mapStr != "-1")
		{
			StringVector splitStr = StringUtility::split(mapStr, "|");
			for (uint32_t i = 0; i < splitStr.size(); ++i)
			{
				StringVector vstr = StringUtility::split(splitStr[i], ":");
				if (vstr.size() == 3)
				{
					MapPos pos;
					pos.m_MapId		= atoi(vstr[0].c_str());
					pos.m_Pos.x		= atoi(vstr[1].c_str());
					pos.m_Pos.y		= atoi(vstr[2].c_str());
					m_NpcMapPosVt.push_back(pos);
				}
			}
		}

		int32_t nLeftTime = getLeftTime();
		if (nLeftTime > 0)
		{
			CfgNpc* pCfgNpc = CFG_DATA.getNpc(m_NpcId);
			if (pCfgNpc)
			{
				std::random_shuffle(m_NpcMapPosVt.begin(), m_NpcMapPosVt.end());
				int32_t nFreshCount = (int32_t)m_NpcMapPosVt.size() <= m_NpcCount
					? (int32_t)m_NpcMapPosVt.size() : m_NpcCount;
				for (int32_t i = 0; i < nFreshCount; ++i)
				{
					MapId_t mapId = m_NpcMapPosVt[i].m_MapId;
					Map* pMap = MAP_MANAGER.GetMap(mapId);
					Npc* npc = new Npc;
					if (npc && pMap)
					{
						int32_t expireTime = DayTime::now() + nLeftTime;
						npc->init(*pCfgNpc, expireTime);
						pMap->addNpc(npc, m_NpcMapPosVt[i].m_Pos.x, m_NpcMapPosVt[i].m_Pos.y);
					}
				}
			}
		}
	}

	// [COLLECTION2]
	if (isOpen(FAT2_COLLECTION2))
	{
		m_vStartDay[4]	= ini.getIntValue("COLLECTION2", "start_day");
		m_vEndDay[4]	= ini.getIntValue("COLLECTION2", "end_day");
		m_PlantTime		= ini.getIntValue("COLLECTION2", "plant_time");
	}

	// [DUI_HUAN]
	if (isOpen(FAT2_DUI_HUAN))
	{
		m_vStartDay[5]	= ini.getIntValue("DUI_HUAN", "start_day");
		m_vEndDay[5]	= ini.getIntValue("DUI_HUAN", "end_day");
		m_DuiHuanSize	= ini.getIntValue("DUI_HUAN", "size");

		m_ExchangeCfgVt.clear();
		m_ExchangeCfgVt.resize(m_DuiHuanSize);
		for (int32_t i = 0; i < m_DuiHuanSize; ++i)
		{
			// need_item
			snprintf(szCol, sizeof(szCol), "need_item%d", i + 1);
			std::string costStr = ini.getStrValue("DUI_HUAN", szCol);
			// Parse item:count:bind format
			StringVector items = StringUtility::split(costStr, "|");
			for (uint32_t j = 0; j < items.size(); ++j)
			{
				StringVector item = StringUtility::split(items[j], ":");
				if (item.size() >= 2)
				{
					ItemData data;
					data.m_nId		= atoi(item[0].c_str());
					data.m_nClass	= 1;
					data.m_nCount	= atoi(item[1].c_str());
					m_ExchangeCfgVt[i].lCostItem.push_back(data);
				}
			}

			// need_gold
			snprintf(szCol, sizeof(szCol), "need_gold%d", i + 1);
			m_ExchangeCfgVt[i].nCostGold = ini.getIntValue("DUI_HUAN", szCol);

			// get_item
			snprintf(szCol, sizeof(szCol), "get_item%d", i + 1);
			m_ExchangeCfgVt[i].GetItems = CItemHelper::parseItemString(0, ini.getStrValue("DUI_HUAN", szCol));

			// time
			snprintf(szCol, sizeof(szCol), "time%d", i + 1);
			m_ExchangeCfgVt[i].Times = ini.getIntValue("DUI_HUAN", szCol);
		}
	}
}

void CFestivalActivity::initOpenList(const std::string& str)
{
	memset(m_vOpen, 0, sizeof(m_vOpen));
	StringVector vOpen = StringUtility::split(str, "|");
	for (uint32_t i = 0; i < vOpen.size(); ++i)
	{
		int8_t nType = (int8_t)atoi(vOpen[i].c_str());
		if (nType > 0 && nType <= 5)
			m_vOpen[nType] = 1;
	}
}

void CFestivalActivity::OnDaySwitch()
{
	int32_t nowTime = TIMER.GetNow();
	int32_t dayzero = DayTime::dayzero(nowTime);
	m_nDay = (dayzero - DayTime::dayzero(m_nStartDay)) / 86400;
	if (m_nDay == m_nEndDay)
		hideIcon(m_nIcon);
}

void CFestivalActivity::OnNewMinute(int32_t nMinute)
{
	m_nMinute = nMinute;
	int32_t nLeftTime = getLeftTime();
	if (nLeftTime > 0 && !m_nDay && !nMinute)
	{
		// Fresh plants
		CfgPlant* pCfgPlant = CFG_DATA.getPlant(m_PlantId);
		if (pCfgPlant)
		{
			std::random_shuffle(m_PlantMapPosVt.begin(), m_PlantMapPosVt.end());
			int32_t nFreshCount = (int32_t)m_PlantMapPosVt.size() <= m_PlantCount
				? (int32_t)m_PlantMapPosVt.size() : m_PlantCount;
			for (int32_t i = 0; i < nFreshCount; ++i)
			{
				MapId_t mapId = m_PlantMapPosVt[i].m_MapId;
				Map* pMap = MAP_MANAGER.GetMap(mapId);
				Plant* plant = new Plant;
				if (plant && pMap)
				{
					CfgMapPlant stu;
					memset(&stu, 0, sizeof(stu));
					stu.mapid	= m_PlantMapPosVt[i].m_MapId;
					stu.plantid	= m_PlantId;
					stu.x		= m_PlantMapPosVt[i].m_Pos.x;
					stu.y		= m_PlantMapPosVt[i].m_Pos.y;
					plant->init(pMap, *pCfgPlant, stu);
					pMap->addPlant(plant);
				}
			}
		}

		// Fresh Npcs
		CfgNpc* pCfgNpc = CFG_DATA.getNpc(m_NpcId);
		if (pCfgNpc)
		{
			std::random_shuffle(m_NpcMapPosVt.begin(), m_NpcMapPosVt.end());
			int32_t nFreshCount = (int32_t)m_NpcMapPosVt.size() <= m_NpcCount
				? (int32_t)m_NpcMapPosVt.size() : m_NpcCount;
			for (int32_t i = 0; i < nFreshCount; ++i)
			{
				MapId_t mapId = m_NpcMapPosVt[i].m_MapId;
				Map* pMap = MAP_MANAGER.GetMap(mapId);
				Npc* npc = new Npc;
				if (npc && pMap)
				{
					int32_t expireTime = DayTime::now() + nLeftTime;
					npc->init(*pCfgNpc, expireTime);
					pMap->addNpc(npc, m_NpcMapPosVt[i].m_Pos.x, m_NpcMapPosVt[i].m_Pos.y);
				}
			}
		}
	}
}

bool CFestivalActivity::IsInTime(FESTIVAL_ACTIVITY_TYPE_2 nType)
{
	return IsOpen(nType)
		&& m_nDay >= m_vStartDay[nType]
		&& m_nDay < m_vEndDay[nType];
}

bool CFestivalActivity::IsInTime()
{
	return m_nDay >= 0 && m_nDay < m_nEndDay;
}

bool CFestivalActivity::IsOpen(FESTIVAL_ACTIVITY_TYPE_2 nType)
{
	return IsInTime() && isOpen(nType);
}

bool CFestivalActivity::isOpen(FESTIVAL_ACTIVITY_TYPE_2 nType)
{
	return m_vOpen[nType] != 0;
}

int32_t CFestivalActivity::getLeftTime()
{
	if (m_nDay < 0)
		return 0;
	int32_t nNowTime = TIMER.GetNow();
	return 86400 * (m_nEndDay - m_nDay) - (nNowTime - DayTime::dayzero(nNowTime));
}

ShowIcon* CFestivalActivity::getIconState(ShowIcon* retstr, Player* player)
{
	memset(retstr, 0, sizeof(ShowIcon));
	if (player)
	{
		retstr->nId = m_nIcon;
		retstr->nState = needShowIcon() ? 2 : 4;
		retstr->nLeftTime = getLeftTime();
	}
	return retstr;
}

void CFestivalActivity::GetIconState(Player* player, IconStateList* IconList)
{
	if (player && needShowIcon())
	{
		ShowIcon icon;
		getIconState(&icon, player);
		IconList->push_back(icon);
	}
}

void CFestivalActivity::SendIconState(Player* player)
{
	if (player && needShowIcon())
	{
		ShowIcon stu;
		getIconState(&stu, player);
		NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_SEND_ONE_ICON);
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

bool CFestivalActivity::needShowIcon()
{
	if (GAME_SERVICE.getLine() == 9)
		return false;
	return m_nDay >= 0 && m_nDay < m_nEndDay;
}

void CFestivalActivity::hideIcon(int32_t nIconId)
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

int32_t CFestivalActivity::GetActDay(FESTIVAL_ACTIVITY_TYPE_2 nType)
{
	int32_t index = m_nDay - m_vStartDay[nType];
	if (index >= 0 && index <= m_vEndDay[nType] - m_vStartDay[nType] - 1)
		return index;
	return -1;
}

int32_t CFestivalActivity::GetPlantTimes()
{
	if (!IsInTime(FAT2_COLLECTION))
		return 0;
	return m_Times;
}

int32_t CFestivalActivity::GetPlantTimes2()
{
	if (!IsInTime(FAT2_COLLECTION2))
		return 0;
	return m_PlantTime;
}

int32_t CFestivalActivity::DaTi(Player* player, std::string* p_DaAn)
{
	if (!player || p_DaAn->empty())
		return 10002;

	if (!IsInTime(FAT2_DA_TI))
		return 0;

	int32_t DaTiTimes = player->getRecord(2120);
	int32_t index = GetActDay(FAT2_DA_TI);
	int32_t nSize = (int32_t)m_StringVtVector.size();
	if (index < 0 || index >= nSize)
		return 10002;

	int32_t DaAnCount = (int32_t)m_StringVtVector[index].size();
	if (DaTiTimes < 0 || DaTiTimes >= DaAnCount)
		return 10002;

	if (m_StringVtVector[index][DaTiTimes] != *p_DaAn)
		return 10002;

	player->updateRecord(2120, DaTiTimes + 1);
	return 0;
}

int32_t CFestivalActivity::GetDaTiReward(Player* player, int8_t RewardType)
{
	if (!player)
		return 10002;

	if (!IsInTime(FAT2_DA_TI))
		return 0;

	int32_t DaTiTimes = player->getRecord(2120);
	int32_t index = GetActDay(FAT2_DA_TI);
	int32_t nSize = (int32_t)m_StringVtVector.size();
	if (index < 0 || index >= nSize)
		return 10002;

	int32_t DaAnCount = (int32_t)m_StringVtVector[index].size();
	if (DaTiTimes < DaAnCount)
		return 10002;

	int32_t record = 0;
	MemChrBag item;
	memset(&item, 0, sizeof(item));
	if (RewardType == 1)
	{
		record = 2121;
		item = m_DayGift;
	}
	else
	{
		record = 37401;
		item = m_ActGift;
	}

	if (player->getRecord(record) > 0)
		return 10002;

	if (!player->GetBag().AddItem(item, IACR_NONE))
		return 10002;

	player->updateRecord(record, 1);
	return 0;
}

int32_t CFestivalActivity::DuiHuan(Player* player, int32_t nIndex)
{
	if (!player)
		return 10002;
	if (nIndex < 0 || m_DuiHuanSize <= nIndex)
		return 10002;
	if (!IsInTime(FAT2_DUI_HUAN))
		return 0;

	int32_t Record = player->getRecord(nIndex + 10101);
	FestivalExchangeCfg* pCfg = &m_ExchangeCfgVt[nIndex];
	if (Record >= pCfg->Times)
		return 10002;

	// Check gold
	if (pCfg->nCostGold > 0 && player->GetCurrency(CURRENCY_GOLD) < pCfg->nCostGold)
		return 10002;

	// Remove cost items
	if (!player->GetBag().RemoveItem(pCfg->lCostItem, IDCR_NONE))
		return 10002;

	// Deduct gold
	if (pCfg->nCostGold > 0)
		player->AddCurrency(CURRENCY_GOLD, -pCfg->nCostGold, GM_NONE);

	// Add reward items
	for (uint32_t i = 0; i < pCfg->GetItems.size(); ++i)
	{
		if (!player->GetBag().AddItem(pCfg->GetItems[i], IACR_NONE))
			return 10002;
	}

	player->updateRecord(nIndex + 10101, Record + 1);
	return 0;
}

void CFestivalActivity::saveVersion()
{
	MySqlDBGuard db(DBPOOL);
	char szSQL[4096] = {};
	int32_t nNowTime = DayTime::now();
	snprintf(szSQL, sizeof(szSQL),
		"INSERT INTO `sys_server_config` (`name`, `value`, `refresh_time`) "
		"VALUES ('FESTIVAL_2_VERSION', %d, %d) ON DUPLICATE KEY UPDATE "
		"`value`=%d, `refresh_time`=%d",
		m_nVersion, nNowTime, m_nVersion, nNowTime);
	db.excute(szSQL);
}

int32_t CFestivalActivity::loadVersion()
{
	MySqlDBGuard db(DBPOOL);
	char szSQL[4096] = {};
	snprintf(szSQL, sizeof(szSQL),
		"SELECT * FROM `sys_server_config` WHERE `name`='FESTIVAL_2_VERSION'");
	MySqlQuery result = db.query(szSQL);
	if (!result.eof())
		return atoi(result.getStringValue("value", ""));
	return 0;
}
