#include "stdafx.h"
#include "PDUDefine.h"
#include "CampWar.h"
#include "ActivityMap.h"
#include "GameService.h"
#include "Timer.h"
#include "DBService.h"
#include "Player.h"

using namespace Answer;

CCampWar::CCampWar(const CfgActivity& cfgActivity)
: CActivity(cfgActivity)
{
	m_nLastTime = 0;
	m_Camp_1_battle = 0;
	m_Camp_2_battle = 0;
}

CCampWar::~CCampWar()
{
}

void CCampWar::OnUpdate(CActivityMap* pMap)
{
	CActivity::OnUpdate(pMap);

	if (m_nState != AS_RUNNING)
	{
		return;
	}

	// Periodically broadcast score update
	if (pMap != NULL)
	{
		int32_t nNow = pMap->getNow();
		if (nNow - m_nLastTime >= 300)
		{
			m_nLastTime = nNow;
			setNeedBroadcastActivityScore();
		}
	}

	// Update camp assignment based on rank
	for (ScoreRankSet::iterator iter = m_sScoreRank.begin(); iter != m_sScoreRank.end(); ++iter)
	{
		PlayerScore* pScore = *iter;
		if (pScore->bInActivity)
		{
			if (pScore->nRankIndex <= m_sScoreRank.size() / 2)
			{
				pScore->nCamp = 1;
			}
			else
			{
				pScore->nCamp = 2;
			}
		}
	}
}

void CCampWar::reset()
{
	CActivity::reset();
	m_nLastTime = 0;
	m_BuffIdMap.clear();
	m_mPlayerScore.clear();
	m_sScoreRank.clear();
	m_Camp_1_battle = 0;
	m_Camp_2_battle = 0;
}

void CCampWar::onPlayerKilled(Player* pDier, Player* pAttacker)
{
	if (NULL == pDier || NULL == pAttacker || m_nState == AS_END)
	{
		return;
	}

	PlayerScoreMap::iterator dierIter = m_mPlayerScore.find(pDier->getCid());
	if (dierIter == m_mPlayerScore.end())
	{
		return;
	}

	PlayerScore& dierScore = dierIter->second;

	// Remove from rank set, modify, then re-insert
	m_sScoreRank.erase(&dierScore);

	// Handle kill streak
	if (dierScore.nContLevel > 0)
	{
		// Reset streak
		dierScore.nTitle = 0;
		dierScore.nContKill = 0;
		dierScore.nContLevel = 0;
	}

	// Calculate score transfer (lose 20%)
	int32_t nLostScore = dierScore.nScore * 20 / 100;
	dierScore.nScore -= nLostScore;

	// Re-insert dier
	m_sScoreRank.insert(&dierScore);

	// Award points to attacker
	PlayerScoreMap::iterator killerIter = m_mPlayerScore.find(pAttacker->getCid());
	if (killerIter != m_mPlayerScore.end())
	{
		killerIter->second.nScore += nLostScore + 5;  // stolen + base 5
		killerIter->second.nKillCount++;
		killerIter->second.nContKill++;
		addKillReward(pAttacker);
	}

	refreshRankIndex();
}

void CCampWar::onMonsterDie(MonsterActivity* pMonster, Player* pKiller)
{
	if (NULL == pMonster || NULL == pKiller)
	{
		return;
	}

	addPlayerScore(pKiller, 5);
	notifyAddScore(pKiller, 5, 2);
}

void CCampWar::onPlayerRevive(Player* pPlayer)
{
	if (NULL == pPlayer || m_nState != AS_RUNNING)
	{
		return;
	}

	// Apply camp buff on revive
	for (BuffIdMap::iterator iter = m_BuffIdMap.begin(); iter != m_BuffIdMap.end(); ++iter)
	{
		int32_t nBuffId = iter->first;
		int8_t nCampId = (int8_t)iter->second;

		PlayerScoreMap::iterator scoreIter = m_mPlayerScore.find(pPlayer->getCid());
		if (scoreIter != m_mPlayerScore.end() && scoreIter->second.nCamp == nCampId)
		{
			addActivityBuff(pPlayer, nBuffId, true);
		}
	}
}

void CCampWar::onPlantGather(Plant* pPlant, Player* player)
{
	// Base implementation - derived classes can extend
}

int32_t CCampWar::onBeginGather(Plant* plant, Player* player)
{
	if (NULL == plant || NULL == player)
	{
		return ERR_INVALID_DATA;
	}

	// Check camp-based gather restriction
	int32_t nPlantCamp = plant->getOwner();
	if (nPlantCamp > 0)
	{
		PlayerScoreMap::iterator scoreIter = m_mPlayerScore.find(player->getCid());
		if (scoreIter != m_mPlayerScore.end() && scoreIter->second.nCamp == nPlantCamp)
		{
			return ERR_INVALID_DATA;  // Can't gather own camp's plant
		}
	}

	return ERR_OK;
}

void CCampWar::addPlayer(Player* player)
{
	if (NULL == player)
	{
		return;
	}

	// Check if player already has a score record
	PlayerScoreMap::iterator iter = m_mPlayerScore.find(player->getCid());
	if (iter != m_mPlayerScore.end())
	{
		// Restore existing player
		iter->second.bInActivity = 1;
		if (iter->second.nContLevel > 0)
		{
			iter->second.nTitle = getContTitle(iter->second.nContLevel);
		}
	}
	else
	{
		// New player
		PlayerScore score;
		score.nCharId = player->getCid();
		score.strName = player->getName();
		score.nConnId = player->getGateIndex();
		score.nStartTick = player->getTick();
		score.nCamp = selectCamp(player);
		score.bInActivity = 1;

		m_mPlayerScore[player->getCid()] = score;

		// Add to camp battle power
		if (score.nCamp == 1)
		{
			m_Camp_1_battle += player->getBattle();
		}
		else if (score.nCamp == 2)
		{
			m_Camp_2_battle += player->getBattle();
		}
	}

	// Apply camp buffs
	for (BuffIdMap::iterator buffIter = m_BuffIdMap.begin(); buffIter != m_BuffIdMap.end(); ++buffIter)
	{
		int8_t nCampId = (int8_t)buffIter->second;
		PlayerScoreMap::iterator scoreIter = m_mPlayerScore.find(player->getCid());
		if (scoreIter != m_mPlayerScore.end() && scoreIter->second.nCamp == nCampId)
		{
			addActivityBuff(player, buffIter->first, false);
		}
	}

	CActivity::addPlayer(player);
}

void CCampWar::removePlayer(Player* player, bool islogout)
{
	CActivity::removePlayer(player, islogout);

	if (NULL == player)
	{
		return;
	}

	// Reset player camp state
	// player->setCamp(0); // API not available in current codebase

	PlayerScoreMap::iterator iter = m_mPlayerScore.find(player->getCid());
	if (iter != m_mPlayerScore.end())
	{
		m_sScoreRank.erase(&iter->second);
		iter->second.bInActivity = 0;
		iter->second.nScore -= iter->second.nScore * 20 / 100;
		m_sScoreRank.insert(&iter->second);
		refreshRankIndex();
	}
}

void CCampWar::addPlayerScore(Player* player, int32_t nPoint)
{
	if (NULL == player)
	{
		return;
	}

	PlayerScoreMap::iterator iter = m_mPlayerScore.find(player->getCid());
	if (iter == m_mPlayerScore.end())
	{
		return;
	}

	// Remove from rank set before modifying
	m_sScoreRank.erase(&iter->second);

	iter->second.nScore += nPoint;
	iter->second.nKillCount++;
	iter->second.nContKill++;

	// Check cont kill level
	// Note: uses config-based level thresholds (stub without CfgCampWarContKill)
	if (iter->second.nContKill >= 10 && iter->second.nContLevel < 1)
	{
		iter->second.nContLevel = 1;
		iter->second.nTitle = getContTitle(1);
		broadcastContKill(player, 1);
	}
	else if (iter->second.nContKill >= 20 && iter->second.nContLevel < 2)
	{
		iter->second.nContLevel = 2;
		iter->second.nTitle = getContTitle(2);
		broadcastContKill(player, 2);
	}
	else if (iter->second.nContKill >= 30 && iter->second.nContLevel < 3)
	{
		iter->second.nContLevel = 3;
		iter->second.nTitle = getContTitle(3);
		broadcastContKill(player, 3);
	}

	// Re-insert
	m_sScoreRank.insert(&iter->second);
	refreshRankIndex();

	setNeedBroadcastActivityScore();

	// Check war end condition
	if (iter->second.nScore > 1999)
	{
		onWarEnd();
	}
}

void CCampWar::addKillReward(Player* player)
{
	if (NULL == player)
		return;

	PlayerScoreMap::iterator iter = m_mPlayerScore.find(player->getCid());
	if (iter == m_mPlayerScore.end())
		return;

	// Every 5 kills, check for reward eligibility
	int32_t nKillCount = iter->second.nKillCount;
	if (nKillCount > 0 && nKillCount % 5 == 0)
	{
		notifyAddScore(player, 0, 3);
	}
}

void CCampWar::addRewards()
{
	// Award rewards based on rank
	for (PlayerScoreMap::iterator iter = m_mPlayerScore.begin(); iter != m_mPlayerScore.end(); ++iter)
	{
		PlayerScore& score = iter->second;
		if (score.nRankIndex <= 0 || !score.bInActivity)
		{
			continue;
		}

		// Send reward mail by rank tier
		int32_t nMailId = 0;
		MemChrBagVector vRewardItems;

		if (score.nRankIndex <= 3)
		{
			nMailId = 6401;  // Top 3 reward
			MemChrBag item;
			memset(&item, 0, sizeof(item));
			item.itemId = 100;
			item.itemCount = 500 / score.nRankIndex;
			vRewardItems.push_back(item);
		}
		else if (score.nRankIndex <= 10)
		{
			nMailId = 6402;  // Top 10 reward
			MemChrBag item;
			memset(&item, 0, sizeof(item));
			item.itemId = 100;
			item.itemCount = 200;
			vRewardItems.push_back(item);
		}
		else
		{
			nMailId = 6403;  // Participation reward
			MemChrBag item;
			memset(&item, 0, sizeof(item));
			item.itemId = 100;
			item.itemCount = 50;
			vRewardItems.push_back(item);
		}

		if (nMailId > 0 && score.nCharId > 0)
		{
			DB_SERVICE.OnSendSysMail(score.nCharId, nMailId, vRewardItems);
		}
	}
}

void CCampWar::refreshRankIndex(Player* pExcept)
{
	int32_t nIndex = 0;
	for (ScoreRankSet::iterator iter = m_sScoreRank.begin(); iter != m_sScoreRank.end(); ++iter)
	{
		PlayerScore* pScore = *iter;
		++nIndex;

		if (pScore->nRankIndex != nIndex)
		{
			pScore->nRankIndex = nIndex;

			if (pScore->bInActivity)
			{
				// Notify player score update
			}
		}
	}
}

void CCampWar::sendPlayerScore(Player* player)
{
	if (NULL == player)
	{
		return;
	}

	PlayerScoreMap::iterator iter = m_mPlayerScore.find(player->getCid());
	if (iter == m_mPlayerScore.end())
	{
		return;
	}

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2E24);
	if (NULL == packet)
	{
		return;
	}

	packet->writeInt32(GetId());
	packet->writeInt32(iter->second.nCamp);
	packet->writeInt32(iter->second.nScore);
	packet->writeInt32(iter->second.nKillCount);
	packet->writeInt32(iter->second.nRankIndex);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(iter->second.nConnId, packet);
}

void CCampWar::SendPlayerRankInfo(Player* player)
{
	if (NULL == player)
	{
		return;
	}

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2E2A);
	if (NULL == packet)
	{
		return;
	}

	packet->writeInt32(GetId());
	for (ScoreRankSet::iterator iter = m_sScoreRank.begin(); iter != m_sScoreRank.end(); ++iter)
	{
		PlayerScore* pScore = *iter;
		if (pScore->nScore > 0)
		{
			packet->writeUTF8(pScore->strName);
			packet->writeInt32(pScore->nCamp);
			packet->writeInt32(pScore->nScore);
			packet->writeInt32(pScore->nKillCount);
		}
	}
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(player->getGateIndex(), packet);
}

void CCampWar::broadcastReady()
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_FAMILY_WAR_ICON);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(169);  // action: camp war ready
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CCampWar::broadcastStart()
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_FAMILY_WAR_ICON);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(170);  // action: camp war start
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CCampWar::broadcastContKill(Player* player, int32_t nLevel)
{
	if (NULL == player)
	{
		return;
	}

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_FAMILY_WAR_ICON);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(171 + nLevel);  // action codes per level
	packet->writeUTF8(player->getName());
	packet->writeInt64(player->getCid());
	int32_t nContKillCount = 0;
	PlayerScoreMap::iterator csIter = m_mPlayerScore.find(player->getCid());
	if (csIter != m_mPlayerScore.end())
	{
		nContKillCount = csIter->second.nContKill;
	}
	packet->writeInt32(nContKillCount);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CCampWar::broadcastBreakContKill(Player* player, Player* pKiller, int32_t nLevel)
{
	if (NULL == player || NULL == pKiller)
	{
		return;
	}

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_FAMILY_WAR_ICON);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(181 + nLevel);  // action codes for break
	packet->writeUTF8(pKiller->getName());
	packet->writeInt64(pKiller->getCid());
	packet->writeUTF8(player->getName());
	packet->writeInt64(player->getCid());
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CCampWar::onTimeEnd()
{
	addRewards();
	m_nState = AS_END;
	delayKickAll(30);
}

void CCampWar::onWarEnd()
{
	addRewards();
	m_nState = AS_END;
	delayKickAll(30);
}

int32_t CCampWar::canEnter(Player* player, CActivityMap* pTargetMap) const
{
	if (NULL == player || NULL == pTargetMap)
	{
		return ERR_INVALID_DATA;
	}
	return CActivity::canEnter(player, pTargetMap);
}

Answer::NetPacket* CCampWar::packetActivityScore()
{
	return CActivity::packetActivityScore();
}

int8_t CCampWar::selectCamp(Player* player)
{
	if (NULL == player)
	{
		return 0;
	}

	// Auto-assign to camp with lower battle power
	if (m_Camp_1_battle <= m_Camp_2_battle)
	{
		return 1;
	}
	return 2;
}

int32_t CCampWar::getContTitle(int32_t nContLevel)
{
	// Default title IDs for cont kill levels
	// These correspond to buff IDs applied to players during the activity
	switch (nContLevel)
	{
	case 1: return 1001;
	case 2: return 1002;
	case 3: return 1003;
	default: return 0;
	}
}

void CCampWar::notifyAddScore(Player* player, int32_t nScore, int32_t nReason)
{
	if (NULL == player)
	{
		return;
	}

	sendPlayerScore(player);
}

void CCampWar::RefreshBuff(int32_t nBuffId, int8_t nCampId)
{
	m_BuffIdMap[nBuffId] = nCampId;
}

void CCampWar::broadcastActivityResult()
{
	// Broadcast war end notification
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_FAMILY_WAR_ICON);
	if (NULL == packet)
		return;
	packet->writeInt32(180);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

bool CCampWar::isCrossActivity()
{
	return true;
}
