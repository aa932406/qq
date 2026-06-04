#include "stdafx.h"
#include "PDUDefine.h"
#include "ActivityWorldBoss.h"
#include "ActivityMap.h"
#include "GameService.h"
#include "Timer.h"
#include "DBService.h"
#include "MapManager.h"

using namespace Answer;

CActivityWorldBoss::CActivityWorldBoss(const CfgActivity& cfgActivity)
: CActivity(cfgActivity)
{
	m_IsBossDie = 0;
}

CActivityWorldBoss::~CActivityWorldBoss()
{
}

void CActivityWorldBoss::OnUpdate(CActivityMap* pMap)
{
	CActivity::OnUpdate(pMap);
}

void CActivityWorldBoss::reset()
{
	CActivity::reset();
	m_mPlayerScore.clear();
	m_sScoreRank.clear();
	m_ReviveTimes.clear();
	m_IsBossDie = 0;
}

int32_t CActivityWorldBoss::canEnter(Player* player, CActivityMap* pTargetMap) const
{
	return CActivity::canEnter(player, pTargetMap);
}

void CActivityWorldBoss::onMonsterDamaged(MonsterActivity* pMonster, int32_t nDamage, Player* pAttacker)
{
	if (NULL == pMonster || NULL == pAttacker)
	{
		return;
	}

	// Only count boss damage (all activity monsters)
	addPlayerScore(pAttacker, nDamage);
}

void CActivityWorldBoss::onPlayerKilled(Player* pDier, Player* pKiller)
{
	if (NULL == pDier || m_nState == AS_END)
	{
		return;
	}

	// Track revive count
	m_ReviveTimes[pDier->getCid()]++;

	if (NULL != pKiller)
	{
		// Steal 20% of dier's current boss damage
		PlayerScoreMap::iterator dierScore = m_mPlayerScore.find(pDier->getCid());
		int32_t stolenScore = 0;

		if (dierScore != m_mPlayerScore.end())
		{
			stolenScore = 20 * dierScore->second.nDamage / 100;
			if (stolenScore > 0)
			{
				dierScore->second.nDamage -= stolenScore;
			}
		}

		if (stolenScore > 0)
		{
			PlayerScoreMap::iterator killerScore = m_mPlayerScore.find(pKiller->getCid());
			if (killerScore != m_mPlayerScore.end())
			{
				killerScore->second.nDamage += stolenScore;
			}
		}

		refreshRank();
		setNeedBroadcastActivityScore();
	}
}

void CActivityWorldBoss::onMonsterDie(MonsterActivity* pMonster)
{
	if (NULL == pMonster)
	{
		return;
	}

	m_IsBossDie = 1;
}

void CActivityWorldBoss::onMonsterAdd(MonsterActivity* pMonster)
{
	if (NULL == pMonster)
	{
		return;
	}

	// Trigger score broadcast when boss spawns
	setNeedBroadcastActivityScore();
}

void CActivityWorldBoss::addPlayer(Player* player)
{
	if (NULL == player)
	{
		return;
	}

	PlayerScoreMap::iterator iter = m_mPlayerScore.find(player->getCid());
	if (iter != m_mPlayerScore.end())
	{
		// Existing player - mark as in activity
		iter->second.bInActivity = 1;
		iter->second.nStartTick = player->getTick();
	}
	else
	{
		// New player
		PlayerScore score;
		score.nCharId = player->getCid();
		score.strName = player->getName();
		score.nConnId = player->getGateIndex();
		score.nStartTick = player->getTick();
		score.nDamage = 0;
		score.nIndex = 0;
		score.bInActivity = 1;
		m_mPlayerScore[player->getCid()] = score;
	}

	CActivity::addPlayer(player);
}

void CActivityWorldBoss::removePlayer(Player* player, bool islogout)
{
	CActivity::removePlayer(player, islogout);

	if (NULL != player)
	{
		PlayerScoreMap::iterator iter = m_mPlayerScore.find(player->getCid());
		if (iter != m_mPlayerScore.end())
		{
			iter->second.bInActivity = 0;
		}
	}
}

bool CActivityWorldBoss::OnSitRevive(Player* player)
{
	return NULL != player;
}

int32_t CActivityWorldBoss::GetRevive(Player* player)
{
	return CActivity::GetRevive(player);
}

bool CActivityWorldBoss::shouldBroadcastScore(CActivityMap* pMap)
{
	if (NULL == pMap)
	{
		return false;
	}

	if (m_nBraodcastActivityScoreSign <= 0)
	{
		return false;
	}

	int64_t curTick = pMap->getTick();
	if (curTick - m_nBroadcastActivityScoreTick <= 2999)
	{
		return false;
	}

	m_nBroadcastActivityScoreTick = curTick;
	--m_nBraodcastActivityScoreSign;
	return true;
}

void CActivityWorldBoss::sendPlayerScore(Player* player)
{
	if (NULL == player)
	{
		return;
	}

	ReviveTimesMap::iterator reviveIter = m_ReviveTimes.find(player->getCid());
	int32_t nDieCount = (reviveIter != m_ReviveTimes.end()) ? reviveIter->second : 0;

	PlayerScoreMap::iterator scoreIter = m_mPlayerScore.find(player->getCid());
	if (scoreIter == m_mPlayerScore.end())
	{
		return;
	}

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2E24);
	if (NULL == packet)
	{
		return;
	}

	packet->writeInt32(GetId());
	packet->writeInt32(scoreIter->second.nIndex);
	packet->writeInt32(scoreIter->second.nDamage);
	packet->writeInt32(nDieCount);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(scoreIter->second.nConnId, packet);
}

void CActivityWorldBoss::addPlayerScore(Player* player, int32_t nDamage)
{
	if (m_nState == AS_END || NULL == player)
	{
		return;
	}

	PlayerScoreMap::iterator iter = m_mPlayerScore.find(player->getCid());
	if (iter == m_mPlayerScore.end())
	{
		return;
	}

	iter->second.nDamage += nDamage;
	iter->second.nStartTick = player->getTick();

	setNeedBroadcastActivityScore();
	refreshRank();
}

void CActivityWorldBoss::refreshRank()
{
	m_sScoreRank.clear();

	for (PlayerScoreMap::iterator iter = m_mPlayerScore.begin(); iter != m_mPlayerScore.end(); ++iter)
	{
		if (iter->second.nDamage > 0)
		{
			m_sScoreRank.insert(&iter->second);
		}
	}

	int32_t nIndex = 0;
	for (ScoreRankSet::iterator iter = m_sScoreRank.begin(); iter != m_sScoreRank.end(); ++iter)
	{
		(*iter)->nIndex = ++nIndex;
	}
}

void CActivityWorldBoss::addRewards()
{
	refreshRank();

	for (PlayerScoreMap::iterator iter = m_mPlayerScore.begin(); iter != m_mPlayerScore.end(); ++iter)
	{
		PlayerScore& score = iter->second;
		if (score.nIndex <= 0 && score.bInActivity == 0)
		{
			continue;
		}

		int32_t nGiftId = 0;
		int32_t nMailId = 0;

		if (score.nIndex == 1)
		{
			nGiftId = 1;  // 1st place reward
			nMailId = 6001;
		}
		else if (score.nIndex >= 2 && score.nIndex <= 3)
		{
			nGiftId = 2;  // 2nd-3rd place reward
			nMailId = 6002;
		}
		else if (score.nIndex >= 4 && score.nIndex <= 10)
		{
			nGiftId = 3;  // 4th-10th place reward
			nMailId = 6003;
		}
		else if (score.bInActivity)
		{
			nGiftId = 4;  // participation reward
			nMailId = 6004;
		}

		if (nGiftId > 0)
		{
			MemChrBagVector items;
			DB_SERVICE.OnSendSysMail(score.nCharId, nMailId, items);
		}
	}
}

void CActivityWorldBoss::onTimeEnd()
{
	addRewards();
	m_nState = AS_END;
	CActivity::delayKickAll(30);
}

Answer::NetPacket* CActivityWorldBoss::packetActivityScore()
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2E26);
	if (NULL == packet)
	{
		return NULL;
	}

	packet->writeInt32(GetId());
	packet->writeInt32(getLeftTime());

	// Top 20 players
	int32_t nCount = 0;
	for (ScoreRankSet::iterator iter = m_sScoreRank.begin(); iter != m_sScoreRank.end() && nCount < 20; ++iter, ++nCount)
	{
		PlayerScore* pScore = *iter;
		packet->writeInt32(pScore->nIndex);
		packet->writeUTF8(pScore->strName);
		packet->writeInt32(pScore->nDamage);
	}

	packet->writeInt8(m_IsBossDie);
	packet->setSize(packet->getWOffset());
	return packet;
}

void CActivityWorldBoss::broadcastStart()
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_FAMILY_WAR_ICON);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(323);  // action code: boss start
	packet->writeInt32(GetId());
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CActivityWorldBoss::broadcastBossKilled(const std::string& strKillerName, CharId_t nKillerId)
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_FAMILY_WAR_ICON);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(324);  // action code: boss killed
	packet->writeUTF8(strKillerName);
	packet->writeInt64(nKillerId);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CActivityWorldBoss::broadcastEnd()
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_FAMILY_WAR_ICON);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(325);  // action code: boss end
	packet->writeInt32(GetId());
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

int32_t CActivityWorldBoss::getAutoReviveTime(Player* player)
{
	if (NULL == player)
	{
		return 10000;
	}

	ReviveTimesMap::iterator iter = m_ReviveTimes.find(player->getCid());
	int32_t nTime = (iter != m_ReviveTimes.end()) ? iter->second : 0;

	int32_t nWaitTime = 5000 * nTime;
	if (nWaitTime > 30000)
	{
		nWaitTime = 30000;
	}
	return nWaitTime;
}

const CActivityWorldBoss::PlayerScore* CActivityWorldBoss::getRankFirst()
{
	ScoreRankSet::iterator iter = m_sScoreRank.begin();
	if (iter == m_sScoreRank.end())
	{
		return NULL;
	}
	return *iter;
}

void CActivityWorldBoss::broadcastActivityResult()
{
	// Broadcast results - base implementation
	// Derived classes can override for specific behavior
}
