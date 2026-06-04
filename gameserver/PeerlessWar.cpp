#include "stdafx.h"
#include "PDUDefine.h"
#include "PeerlessWar.h"
#include "ActivityMap.h"
#include "GameService.h"
#include "Timer.h"
#include "DayTime.h"
#include "DBService.h"
#include "Player.h"

using namespace Answer;

CPeerlessWar::CPeerlessWar(const CfgActivity& cfgActivity)
: CActivity(cfgActivity)
{
	m_nLastTick = 0;
	m_nWarState = 0;
	m_strWinnerName = "";
}

CPeerlessWar::~CPeerlessWar()
{
}

void CPeerlessWar::Init()
{
	CActivity::Init();
}

void CPeerlessWar::OnUpdate(CActivityMap* pMap)
{
	CActivity::OnUpdate(pMap);
}

void CPeerlessWar::reset()
{
	CActivity::reset();
	m_mPlayerScore.clear();
	m_nLastTick = 0;
	m_nWarState = 0;
	m_strWinnerName = "";
}

void CPeerlessWar::onPlayerKilled(Player* pDier, Player* pKiller)
{
	if (NULL == pDier || NULL == pKiller || m_nState != AS_RUNNING)
	{
		return;
	}

	PlayerScoreMap::iterator dierIter = m_mPlayerScore.find(pDier->getCid());
	PlayerScoreMap::iterator killerIter = m_mPlayerScore.find(pKiller->getCid());

	if (killerIter != m_mPlayerScore.end())
	{
		killerIter->second.nKillCount++;
		killerIter->second.nContKill++;
		killerIter->second.nScore += 10;

		if (needBroadcastContKill())
		{
			broadcastConstKill(pKiller);
		}
	}

	if (dierIter != m_mPlayerScore.end())
	{
		dierIter->second.nContKill = 0;

		if (needBroadcastBreakCont())
		{
			broadcastBreakConst(pDier, pKiller);
		}
	}
}

void CPeerlessWar::onPlayerRevive(Player* pPlayer)
{
	if (NULL == pPlayer || m_nState != AS_RUNNING)
	{
		return;
	}
}

void CPeerlessWar::addPlayer(Player* player)
{
	if (NULL == player)
	{
		return;
	}

	PlayerScoreMap::iterator iter = m_mPlayerScore.find(player->getCid());
	if (iter == m_mPlayerScore.end())
	{
		PlayerScore score;
		score.nCharId = player->getCid();
		score.strName = player->getName();
		score.nConnId = player->getGateIndex();
		score.bInActivity = 1;
		m_mPlayerScore[player->getCid()] = score;
	}
	else
	{
		iter->second.bInActivity = 1;
	}

	CActivity::addPlayer(player);
}

void CPeerlessWar::removePlayer(Player* player, bool islogout)
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

Position CPeerlessWar::GetRandBornPos(Player* player)
{
	return getBornRandPosB();
}

int32_t CPeerlessWar::canEnter(Player* player, CActivityMap* pTargetMap) const
{
	if (NULL == player || NULL == pTargetMap)
	{
		return ERR_INVALID_DATA;
	}
	return CActivity::canEnter(player, pTargetMap);
}

void CPeerlessWar::onTimeEnd()
{
	addRewards();
	saveWarResult();
	m_nState = AS_END;
	delayKickAll(30);
}

Answer::NetPacket* CPeerlessWar::packetActivityScore()
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2E26);
	if (NULL == packet)
	{
		return NULL;
	}

	packet->writeInt32(GetId());
	packet->writeInt32(getLeftTime());

	for (PlayerScoreMap::iterator iter = m_mPlayerScore.begin(); iter != m_mPlayerScore.end(); ++iter)
	{
		packet->writeUTF8(iter->second.strName);
		packet->writeInt32(iter->second.nScore);
		packet->writeInt32(iter->second.nKillCount);
	}

	packet->writeUTF8(m_strWinnerName);
	packet->writeInt32(m_nWarState);
	packet->setSize(packet->getWOffset());
	return packet;
}

void CPeerlessWar::sendPlayerScore(Player* player)
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
	packet->writeInt32(iter->second.nScore);
	packet->writeInt32(iter->second.nKillCount);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(iter->second.nConnId, packet);
}

void CPeerlessWar::sendActivityResult(Player* player)
{
	if (NULL == player)
		return;

	PlayerScoreMap::iterator iter = m_mPlayerScore.find(player->getCid());
	if (iter == m_mPlayerScore.end())
		return;

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2E26);
	if (NULL == packet)
		return;

	packet->writeInt32(GetId());
	packet->writeInt32(iter->second.nScore);
	packet->writeInt32(iter->second.nKillCount);
	packet->writeInt32(m_nWarState);
	packet->writeUTF8(m_strWinnerName);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(player->getGateIndex(), packet);
}

int32_t CPeerlessWar::getLeftReviveTimes(Player* player)
{
	if (NULL == player)
	{
		return 0;
	}
	return 3;  // Default 3 revive chances
}

void CPeerlessWar::win(Player* pWinner)
{
	if (NULL == pWinner)
	{
		return;
	}

	m_nWarState = 1;
	m_strWinnerName = pWinner->getName();

	PlayerScoreMap::iterator iter = m_mPlayerScore.find(pWinner->getCid());
	if (iter != m_mPlayerScore.end())
	{
		iter->second.nWinCount++;
	}

	addWinnerReward(pWinner);
	broadcastWin(pWinner);
}

void CPeerlessWar::draw()
{
	m_nWarState = 2;
	m_strWinnerName = "";
	broadcastDraw();

	// Give draw rewards to all participants
	for (PlayerScoreMap::iterator iter = m_mPlayerScore.begin(); iter != m_mPlayerScore.end(); ++iter)
	{
		if (iter->second.bInActivity)
		{
			// TODO: find player by charId and give reward
			// addJoinReward(player);
		}
	}
}

void CPeerlessWar::addWinnerReward(Player* player)
{
	if (NULL == player)
		return;

	MemChrBagVector reward;
	MemChrBag item;
	memset(&item, 0, sizeof(item));
	item.itemId = 100;
	item.itemCount = 1000;
	reward.push_back(item);

	DB_SERVICE.OnSendSysMail(player->getCid(), 6404, reward);
}

void CPeerlessWar::addJoinReward(Player* player)
{
	if (NULL == player)
		return;

	MemChrBagVector reward;
	MemChrBag item;
	memset(&item, 0, sizeof(item));
	item.itemId = 100;
	item.itemCount = 100;
	reward.push_back(item);

	DB_SERVICE.OnSendSysMail(player->getCid(), 6405, reward);
}

void CPeerlessWar::addKillReward(Player* player)
{
	if (NULL == player)
		return;

	PlayerScoreMap::iterator iter = m_mPlayerScore.find(player->getCid());
	if (iter == m_mPlayerScore.end())
		return;

	if (iter->second.nKillCount > 0 && iter->second.nKillCount % 5 == 0)
	{
		// Every 5 kills, send notification
		sendPlayerScore(player);
	}
}

void CPeerlessWar::addRewards()
{
	// Send reward mail to all participants by rank
	std::vector<std::pair<CharId_t, int32_t>> rewards;
	for (PlayerScoreMap::iterator iter = m_mPlayerScore.begin(); iter != m_mPlayerScore.end(); ++iter)
	{
		PlayerScore& score = iter->second;
		if (score.bInActivity)
		{
			int32_t nMailId = 0;
			int32_t nRewardGold = 0;
			if (score.nScore >= 100)
			{
				nMailId = 6404;
				nRewardGold = 500;
			}
			else if (score.nScore >= 50)
			{
				nMailId = 6405;
				nRewardGold = 200;
			}
			else
			{
				nMailId = 6406;
				nRewardGold = 50;
			}

			MemChrBagVector reward;
			MemChrBag item;
			memset(&item, 0, sizeof(item));
			item.itemId = 100;
			item.itemCount = nRewardGold;
			reward.push_back(item);

			if (score.nCharId > 0)
				DB_SERVICE.OnSendSysMail(score.nCharId, nMailId, reward);
		}
	}
}

void CPeerlessWar::saveWarResult()
{
	MySqlDBGuard db(DBPOOL);
	char szSQL[4096] = {};
	int32_t nNowTime = DayTime::now();

	// Save the war result summary
	int32_t nWinnerCid = 0;
	for (PlayerScoreMap::iterator iter = m_mPlayerScore.begin(); iter != m_mPlayerScore.end(); ++iter)
	{
		if (iter->second.nWinCount > 0)
		{
			nWinnerCid = iter->first;
			break;
		}
	}

	snprintf(szSQL, sizeof(szSQL),
		"REPLACE INTO `sys_server_config` (`name`, `value`, `refresh_time`) "
		"VALUES ('PEERLESS_WAR_RESULT', '%lld,%s,%d', %d)",
		(long long)nWinnerCid, m_strWinnerName.c_str(), m_nWarState, nNowTime);
	db.excute(szSQL);
}

void CPeerlessWar::clearOldTitle(Player* player)
{
	if (NULL == player)
		return;
	// Title clearing is handled by the activity end logic
}

bool CPeerlessWar::needBroadcastContKill()
{
	return true;  // Always broadcast cont kill for PeerlessWar
}

bool CPeerlessWar::needBroadcastBreakCont()
{
	return true;  // Always broadcast break cont for PeerlessWar
}

void CPeerlessWar::addPlayerKillCount(Player* player)
{
	if (NULL == player)
	{
		return;
	}

	PlayerScoreMap::iterator iter = m_mPlayerScore.find(player->getCid());
	if (iter != m_mPlayerScore.end())
	{
		iter->second.nKillCount++;
		if (checkWin(player))
		{
			win(player);
		}
	}
}

void CPeerlessWar::broadcastReady()
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_FAMILY_WAR_ICON);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(200);  // action: peerless war ready
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CPeerlessWar::broadcastStart()
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_FAMILY_WAR_ICON);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(201);  // action: peerless war start
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CPeerlessWar::broadcastKnockout(Player* pWinner, Player* pLoser)
{
	if (NULL == pWinner || NULL == pLoser)
	{
		return;
	}

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_FAMILY_WAR_ICON);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(202);
	packet->writeUTF8(pWinner->getName());
	packet->writeInt64(pWinner->getCid());
	packet->writeUTF8(pLoser->getName());
	packet->writeInt64(pLoser->getCid());
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CPeerlessWar::broadcastWin(Player* pWinner)
{
	if (NULL == pWinner)
	{
		return;
	}

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_FAMILY_WAR_ICON);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(203);
	packet->writeUTF8(pWinner->getName());
	packet->writeInt64(pWinner->getCid());
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CPeerlessWar::broadcastConstKill(Player* player)
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
	packet->writeInt32(204);
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

void CPeerlessWar::broadcastBreakConst(Player* player, Player* pKiller)
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
	packet->writeInt32(205);
	packet->writeUTF8(pKiller->getName());
	packet->writeInt64(pKiller->getCid());
	packet->writeUTF8(player->getName());
	packet->writeInt64(player->getCid());
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CPeerlessWar::broadcastDraw()
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_FAMILY_WAR_ICON);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(206);  // action: draw
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

bool CPeerlessWar::checkWin(Player* player)
{
	if (NULL == player)
	{
		return false;
	}

	PlayerScoreMap::iterator iter = m_mPlayerScore.find(player->getCid());
	if (iter == m_mPlayerScore.end())
	{
		return false;
	}

	return iter->second.nKillCount >= 10;  // Win condition: 10 kills
}

void CPeerlessWar::OnPeerlessWarResult(int32_t nWarState, CharId_t nWinnerId, const std::string& strWinnerName)
{
	m_nWarState = nWarState;
	m_strWinnerName = strWinnerName;
}

std::string CPeerlessWar::GetWinnerName()
{
	return m_strWinnerName;
}
