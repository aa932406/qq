#include "stdafx.h"
#include "Shared.h"
#include "NationalDayHd.h"
#include "GameService.h"
#include "Player.h"
#include "Bag.h"

#include "OpenBeta.h"

using namespace Answer;

CNationalDayHd::CNationalDayHd()
	: CExtSystemBase()
{
	m_Level = 0;
	m_Exp = 0;
	m_UnLockReward = 0;
	m_OrdinaryReward = 0;
	m_SeniorReward = 0;
}

CNationalDayHd::~CNationalDayHd()
{
}

void CNationalDayHd::OnCleanUp()
{
	m_NationalInfo.clear();
	m_Level = 0;
	m_Exp = 0;
	m_UnLockReward = 0;
	m_OrdinaryReward = 0;
	m_SeniorReward = 0;
}

void CNationalDayHd::Init(Player* pPlayer)
{
	CExtSystemBase::Init(pPlayer);
}

void CNationalDayHd::paraseNational(std::string* p_NationalInfo)
{
	if (!p_NationalInfo || p_NationalInfo->empty())
		return;

	StringVector items_receive = StringUtility::split(*p_NationalInfo, "|");
	for (uint32_t i = 0; i < items_receive.size(); ++i)
	{
		StringVector TaskInfo = StringUtility::split(items_receive[i], ":");
		if (TaskInfo.size() == 3)
		{
			int32_t key = atoi(TaskInfo[0].c_str());
			Param2 stu;
			stu.nParam1 = atoi(TaskInfo[1].c_str());
			stu.nParam2 = atoi(TaskInfo[2].c_str());
			m_NationalInfo[key] = stu;
		}
	}
}

std::string CNationalDayHd::SaveNationalInfo()
{
	std::string result;
	for (std::map<int32_t, Param2>::iterator it = m_NationalInfo.begin();
		it != m_NationalInfo.end(); ++it)
	{
		if (!result.empty())
			result += "|";
		char buf[64] = {};
		snprintf(buf, sizeof(buf), "%d:%d:%d", it->first, it->second.nParam1, it->second.nParam2);
		result += buf;
	}
	return result;
}

bool CNationalDayHd::GetNationalExp(int32_t nType, int32_t nId)
{
	if (!m_pPlayer)
		return false;

	std::map<int32_t, Param2>::iterator it = m_NationalInfo.find(nType);
	if (it == m_NationalInfo.end())
		return false;

	AddExp(1);
	return true;
}

bool CNationalDayHd::UnLockHighReward()
{
	if (!m_pPlayer)
		return false;
	if (m_UnLockReward > 0)
		return false;

	m_UnLockReward = 1;

	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CD6);
	if (packet)
	{
		packet->writeInt32(3023);
		packet->writeInt64(m_pPlayer->getCid());
		std::string name = m_pPlayer->getName();
		packet->writeUTF8(name.c_str());
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.worldBroadcast(packet);
	}
	return true;
}

bool CNationalDayHd::GetSeniorReward(int32_t nLevel)
{
	if (!m_pPlayer)
		return false;
	if (m_Level < nLevel)
		return false;

	int32_t nNewRecord = m_SeniorReward | (1 << nLevel);
	if (m_SeniorReward == nNewRecord)
		return false;

	m_SeniorReward = nNewRecord;
	return true;
}

bool CNationalDayHd::GetOrdinaryReward(int32_t nLevel)
{
	if (!m_pPlayer)
		return false;
	if (m_Level < nLevel)
		return false;

	int32_t nNewRecord = m_OrdinaryReward | (1 << nLevel);
	if (m_OrdinaryReward == nNewRecord)
		return false;

	m_OrdinaryReward = nNewRecord;
	return true;
}

bool CNationalDayHd::BuyLevel(int32_t BuyLv)
{
	if (!m_pPlayer)
		return false;
	if (m_Level >= BuyLv)
		return false;

	int32_t diffCost = BuyLv - m_Level;
	if (diffCost <= 0)
		return false;

	if (!m_pPlayer->DecCurrency(CURRENCY_GOLD, diffCost * 100, GM_NONE, 0))
		return false;

	m_Level = BuyLv;
	return true;
}

void CNationalDayHd::OnLoadFromDB(const PlayerDBData* dbData)
{
	if (!dbData)
		return;
	// Player data for NationalDayHd is loaded via paraseNational from Player
}

void CNationalDayHd::OnSaveToDB(PlayerDBData* dbData)
{
	if (!dbData)
		return;
	// Player data for NationalDayHd is saved via SaveNationalInfo from Player
}

void CNationalDayHd::AddNationalValue(NATIONAL_HD_TYPE nType, int32_t ParamValue1, int32_t ParamValue2)
{
	if (!m_pPlayer)
		return;

	int32_t OldCount = CanGetCount();

	switch (nType)
	{
	case HHT_LOGIN:
	case HHT_CBT:
	case HHT_CHOU_JIANG:
	case HHT_LUN_HUI_TA:
	case HHT_COST_GOLD:
		m_NationalInfo[nType].nParam1 += ParamValue1;
		break;
	case HHT_RECHARGE:
		if (ParamValue1 <= ParamValue2)
			++m_NationalInfo[nType].nParam1;
		break;
	case HHT_YJSK:
		m_NationalInfo[nType].nParam1 += ParamValue1;
		break;
	case HHT_KILL_BOSS:
		++m_NationalInfo[nType].nParam1;
		break;
	default:
		break;
	}

	int32_t NewCount = CanGetCount();
	if (NewCount != OldCount)
	{
		COpenBeta& openBeta = Answer::Singleton<COpenBeta>::instance();
		openBeta.SendIconState(m_pPlayer);
	}
}

void CNationalDayHd::AddExp(int32_t AddExp)
{
	m_Exp += AddExp;
	CheckLevelUp();
	SendLevelInfo();
}

void CNationalDayHd::CheckLevelUp()
{
	if (m_Exp >= 100)
	{
		m_Exp -= 100;
		++m_Level;
	}
}

void CNationalDayHd::SendLevelInfo()
{
	if (!m_pPlayer)
		return;

	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2844);
	if (packet)
	{
		packet->writeInt32(m_Level);
		packet->writeInt32(m_Exp);
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);
	}
}

void CNationalDayHd::PackHdInfo(Answer::NetPacket* packet)
{
	if (!packet)
		return;

	packet->writeInt32(m_Level);
	packet->writeInt32(m_Exp);
	packet->writeInt32(m_UnLockReward);
	packet->writeInt32(m_OrdinaryReward);
	packet->writeInt32(m_SeniorReward);

	packet->writeInt32((int32_t)m_NationalInfo.size());
	for (std::map<int32_t, Param2>::iterator it = m_NationalInfo.begin();
		it != m_NationalInfo.end(); ++it)
	{
		packet->writeInt32(it->first);
		packet->writeInt32(it->second.nParam1);
		packet->writeInt32(it->second.nParam2);
	}
}

int32_t CNationalDayHd::CanGetCount()
{
	int32_t nCount = 0;
	for (std::map<int32_t, Param2>::iterator it = m_NationalInfo.begin();
		it != m_NationalInfo.end(); ++it)
	{
		if (it->second.nParam1 > 0)
			++nCount;
	}
	return nCount;
}
