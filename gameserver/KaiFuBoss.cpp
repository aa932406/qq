#include "stdafx.h"
#include "KaiFuBoss.h"
#include "GameService.h"
#include "ActivityMap.h"
#include "CfgData.h"
#include "Timer.h"
#include "DayTime.h"
using namespace Answer;

CKaiFuBoss::CKaiFuBoss(const CfgActivity& cfgActivity)
	: CActivity(cfgActivity)
{
	m_Boss = NULL;
}

CKaiFuBoss::~CKaiFuBoss()
{
}

void CKaiFuBoss::reset()
{
	m_Boss = NULL;
	CActivity::reset();
}

bool CKaiFuBoss::checkData()
{
	int32_t startDays = TIMER.GetDaysFromStart();
	if (isSpecialDay(startDays))
		return true;
	if (isInSpecialDay(startDays))
		return false;
	return CActivity::checkData();
}

bool CKaiFuBoss::checkWeek()
{
	int32_t startDays = TIMER.GetDaysFromStart();
	if (isSpecialDay(startDays))
		return true;
	if (isInSpecialDay(startDays))
		return false;
	return CActivity::checkWeek();
}

bool CKaiFuBoss::isSpecialDay(int32_t nStartDays)
{
	return nStartDays >= 0 && nStartDays <= 6;
}

bool CKaiFuBoss::isInSpecialDay(int32_t nStartDays)
{
	// In special day range but not the first few days
	return nStartDays >= 0 && nStartDays <= 6;
}

int32_t CKaiFuBoss::getNextStartTime()
{
	if (IsRuning() || m_cfgActivity.start_hour.empty())
		return 0;

	int32_t nNowTime = TIMER.GetNow();
	const tm& localnow = TIMER.GetLocalNow();
	int32_t nowMinute = localnow.tm_hour * 60 + localnow.tm_min;
	int32_t startMinute = m_cfgActivity.start_hour[0];

	for (uint32_t i = 0; i < m_cfgActivity.start_hour.size(); ++i)
	{
		int32_t startTime = DayTime::dayzero(nNowTime) + m_cfgActivity.start_hour[i] * 60;
		if (startMinute > nNowTime)
		{
			startMinute = m_cfgActivity.start_hour[i];
			break;
		}
	}

	int32_t days = -1;
	int32_t startDays = TIMER.GetDaysFromStart();

	if (checkData() && checkWeek())
	{
		if (nowMinute < startMinute)
			days = 0;
	}
	else if (isInSpecialDay(startDays) && startDays <= 6)
	{
		days = 7 - startDays;
	}

	if (days < 0)
	{
		if (m_cfgActivity.weekday.empty())
			return 0;

		int32_t weekday = TIMER.GetWeekDay();
		int32_t nextweekday = 0;

		Int32Vector::const_reverse_iterator iter = m_cfgActivity.weekday.rbegin();
		for (; iter != m_cfgActivity.weekday.rend(); ++iter)
		{
			if (*iter > weekday)
				break;
		}

		if (iter == m_cfgActivity.weekday.rend())
			nextweekday = m_cfgActivity.weekday.back();
		else
			nextweekday = *iter;

		if (nextweekday >= weekday)
			days = nextweekday - weekday;
		else
			days = nextweekday + 7 - weekday;
	}

	return DayTime::dayzero(nNowTime) + days * 86400 + startMinute * 60;
}

void CKaiFuBoss::onMonsterAdd(MonsterActivity* pMonster)
{
	if (pMonster)
		m_Boss = pMonster;
}

void CKaiFuBoss::sendPlayerScore(Player* player)
{
	if (player == NULL)
		return;

	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2E26);
	if (packet == NULL)
		return;

	packet->writeInt32(m_cfgActivity.id);
	packet->writeInt32(getLeftTime());
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(player->getGateIndex(), packet);
}

void CKaiFuBoss::broadcastReady()
{
	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CD6);
	if (packet == NULL)
		return;
	packet->writeInt32(470);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CKaiFuBoss::broadcastStart()
{
	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CD6);
	if (packet == NULL)
		return;
	packet->writeInt32(471);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CKaiFuBoss::broadcastTimeEnd()
{
	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CD6);
	if (packet == NULL)
		return;

	if (m_Boss && m_Boss->isAlive())
		packet->writeInt32(473);
	else
		packet->writeInt32(472);

	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CKaiFuBoss::onTimeEnd()
{
	broadcastTimeEnd();
	delayKickAll(0);
}
