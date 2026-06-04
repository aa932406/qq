#include "FamilyWar.h"
#include "GameService.h"
#include "DBService.h"
#include "Timer.h"
#include "ActivityMap.h"
#include "FamilyManager.h"
#include "ActivityManager.h"
#include "KaiFuHuoDong.h"
#include "CfgData.h"
using namespace Answer;

CFamilyWar::CFamilyWar(const CfgActivity& cfgActivity)
	: CActivity(cfgActivity)
{
	m_nActiveState = 0;
	m_nActivePillarState = 0;
	m_pTitle = NULL;
	m_pBoss = NULL;
	m_pBuffPlayer = NULL;
	m_nBuffStartTick = 0;
	m_nWinFamily = 0;
}

CFamilyWar::~CFamilyWar()
{
}

void CFamilyWar::reset()
{
	CActivity::reset();
	m_nActiveState = 0;
	m_nActivePillarState = 0;
	m_pTitle = NULL;
	m_pBoss = NULL;
	m_pBuffPlayer = NULL;
	m_nBuffStartTick = 0;
	m_nWinFamily = 0;
	m_strWingFamily = "";
	m_lstFamilyScore.clear();
	m_lstPlayerScore.clear();
}

void CFamilyWar::OnUpdate(CActivityMap* pMap)
{
	CActivity::OnUpdate(pMap);
	if (pMap == NULL)
		return;

	int64_t curTick = pMap->getTick();

	// Check buff timer: ~5 minutes -> win
	if (m_pBuffPlayer && m_pBuffPlayer->getMap() == pMap)
	{
		if (curTick - m_nBuffStartTick > 299999)
		{
			FamilyId_t familyId = m_pBuffPlayer->getFamilyId();
			std::string strFamilyName = m_pBuffPlayer->getFamilyName();
			win(familyId, strFamilyName);
			removeBuff(m_pBuffPlayer);
			m_pBuffPlayer = NULL;
			setNeedBroadcastActivityScore();
		}
	}

	// Periodic join reward check
	for (PlayerScoreList::iterator iter = m_lstPlayerScore.begin(); iter != m_lstPlayerScore.end(); ++iter)
	{
		if (iter->bInActivity && curTick - iter->nStartTick > 9999)
		{
			Player* pPlayer = GAME_SERVICE.getPlayer(iter->nCharId, pMap->GetRunnerId(), true);
			if (pPlayer && pPlayer->getMap() == pMap)
			{
				iter->nStartTick = curTick;
				int32_t nLevel = pPlayer->getLevel();
				int32_t nExp = CFG_DATA.GetFamilyWarJoinRewardTable().GetReward(nLevel);
				pPlayer->addExp(nExp);
				iter->nExp += nExp;
			}
		}
	}
}

void CFamilyWar::addPlayer(Player* player)
{
	if (player == NULL)
		return;

	// Set PK mode
	if (player->getFamilyId() <= 0)
		player->setPkMode(PK_MODE_SERVER, false);
	else
		player->setPkMode(PK_MODE_LEGION, false);

	// Find existing or create new player score
	int32_t nTaskId = 0;
	bool bFound = false;
	for (PlayerScoreList::iterator iter = m_lstPlayerScore.begin(); iter != m_lstPlayerScore.end(); ++iter)
	{
		if (iter->nCharId == player->getCid())
		{
			iter->bInActivity = true;
			nTaskId = iter->nTaskId;
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
	PlayerScore score = {};
	score.nCharId = player->getCid();
	score.strName = player->getName();
		score.nFamilyId = player->getFamilyId();
		score.nStartTick = player->getTick();
		score.bInActivity = true;
		m_lstPlayerScore.push_back(score);
	}

	// Add buff
	if (m_nWinFamily > 0 && player->getFamilyId() == m_nWinFamily)
		addBuff(player);

	CActivity::addPlayer(player);
	KAI_FU_HUO_DONG.SendKaiFuHuoDongIcon(player);
}

void CFamilyWar::removePlayer(Player* player, bool islogout)
{
	CActivity::removePlayer(player, islogout);
	if (player == NULL)
		return;

	// Remove buff if held
	removeBuff(player);

	// If buff holder leaves, revive title
	if (m_pBuffPlayer == player)
	{
		m_pBuffPlayer = NULL;
		m_nBuffStartTick = 0;
		// Title monster buff holder left - reset title
		m_pTitle = NULL;
	}

	// Mark inactive
	for (PlayerScoreList::iterator iter = m_lstPlayerScore.begin(); iter != m_lstPlayerScore.end(); ++iter)
	{
		if (iter->nCharId == player->getCid())
		{
			iter->bInActivity = false;
			return;
		}
	}
}

int32_t CFamilyWar::canEnter(Player* player, CActivityMap* pTargetMap) const
{
	return CActivity::canEnter(player, pTargetMap);
}

void CFamilyWar::onMonsterAdd(MonsterActivity* pMonster)
{
	if (pMonster == NULL)
		return;

	// Track title (buff-giving) and boss monsters
	// First non-boss monster in the family war map is the title
	if (m_pTitle == NULL)
		m_pTitle = pMonster;
	else if (m_pBoss == NULL)
		m_pBoss = pMonster;

	if (m_nWinFamily > 0)
		pMonster->SetFamilyId(m_nWinFamily);
}

void CFamilyWar::onMonsterDie(MonsterActivity* pMonster, Player* pKiller)
{
	if (pMonster == NULL || pKiller == NULL || !IsRuning())
		return;

	int32_t nKillPoints = 20; // Default kill points
	addPlayerScore(pKiller, nKillPoints, 0);

	// Title monster killed -> give buff to killer
	if (m_pTitle == pMonster)
	{
		m_pBuffPlayer = pKiller;
		m_nBuffStartTick = pKiller->getTick();
		addBuff(m_pBuffPlayer);
		setNeedBroadcastActivityScore();
	}
}

void CFamilyWar::onPlayerKilled(Player* pDier, Player* pAttacker)
{
	if (pDier == NULL || pAttacker == NULL)
		return;

	addPlayerScore(pAttacker, 20, 1);

	// If buff holder dies, transfer to killer
	if (m_pBuffPlayer == pDier)
	{
		m_pBuffPlayer = pAttacker;
		m_nBuffStartTick = pAttacker->getTick();
		addBuff(m_pBuffPlayer);
		setNeedBroadcastActivityScore();
	}
}

PlayerScore* CFamilyWar::getPlayerScore(CharId_t nCharId)
{
	for (PlayerScoreList::iterator iter = m_lstPlayerScore.begin(); iter != m_lstPlayerScore.end(); ++iter)
	{
		if (iter->nCharId == nCharId)
			return &(*iter);
	}
	return NULL;
}

void CFamilyWar::addPlayerScore(Player* player, int32_t nScore, int32_t nKillCount)
{
	if (player == NULL)
		return;

	PlayerScore* pScore = getPlayerScore(player->getCid());
	if (pScore == NULL)
		return;

	// Limit kills
	if (nKillCount > 0)
	{
		if (pScore->nKillCount > 19)
			return;
		pScore->nKillCount += nKillCount;
	}

	pScore->nScore += nScore;
	addFamilyScore(player->getFamilyId(), player->getFamilyName(), nScore);

	m_lstPlayerScore.sort(std::greater<PlayerScore>());
	setNeedBroadcastActivityScore();
}

void CFamilyWar::addFamilyScore(FamilyId_t nFamilyId, const std::string& strFamilyName, int32_t nScore)
{
	for (FamilyScoreList::iterator iter = m_lstFamilyScore.begin(); iter != m_lstFamilyScore.end(); ++iter)
	{
		if (iter->nFamilyId == nFamilyId)
		{
			iter->nScore += nScore;
			m_lstFamilyScore.sort(std::greater<FamilyScore>());
			return;
		}
	}

	FamilyScore score;
	score.nFamilyId = nFamilyId;
	score.strFamilyName = strFamilyName;
	score.nScore = nScore;
	m_lstFamilyScore.push_back(score);
	m_lstFamilyScore.sort(std::greater<FamilyScore>());
}

void CFamilyWar::win(FamilyId_t nFamilyId, const std::string& strFamilyName)
{
	if (nFamilyId > 0)
	{
		m_nActiveState = 2;
		m_nWinFamily = nFamilyId;
		m_strWingFamily = strFamilyName;
		sendWinnerReward(nFamilyId);
	}

	sendPlayerScoreRankReward();
	sendFamilyScoreRankReward();
	setNeedBroadcastActivityScore();

	broadcastWin(nFamilyId, strFamilyName);
	m_nState = AS_END;
	delayKickAll(30);
	stopActivity();
	GAME_SERVICE.broadcastHuoDongDaTingIcon();
}

void CFamilyWar::sendPlayerScoreRankReward()
{
	m_lstPlayerScore.sort(std::greater<PlayerScore>());
	for (PlayerScoreList::iterator iter = m_lstPlayerScore.begin(); iter != m_lstPlayerScore.end(); ++iter)
	{
		if (!iter->bInActivity)
			continue;

		Player* player = GAME_SERVICE.getPlayer(iter->nCharId, 0, false);
		if (player == NULL)
			continue;

		// Send rank reward mail based on config
		if (m_cfgActivity.gift_id[0] > 0)
		{
			MemChrBag reward;
			reward.itemId = m_cfgActivity.gift_id[0];
			reward.itemClass = IC_NORMAL;
			reward.itemCount = 1;
			reward.bind = IBS_BIND;
			if (!player->GetBag().AddItem(reward, IACR_ACTIVITY))
				DB_SERVICE.OnSendSysMail(player->getCid(), MI_FAMILYWAR_REWARD, reward);
		}
	}
}

void CFamilyWar::sendFamilyScoreRankReward()
{
	m_lstFamilyScore.sort(std::greater<FamilyScore>());
	// Family score rank rewards handled by sendPlayerScoreRankReward
}

void CFamilyWar::sendWinnerReward(FamilyId_t nFamilyId)
{
	for (PlayerScoreList::iterator iter = m_lstPlayerScore.begin(); iter != m_lstPlayerScore.end(); ++iter)
	{
		if (iter->bInActivity && iter->nFamilyId == nFamilyId)
		{
			Player* player = GAME_SERVICE.getPlayer(iter->nCharId, 0, false);
			if (player && m_cfgActivity.daily_reward[0] > 0)
			{
				MemChrBag reward;
				reward.itemId = m_cfgActivity.daily_reward[0];
				reward.itemClass = IC_NORMAL;
				reward.itemCount = 1;
				reward.bind = IBS_BIND;
				if (!player->GetBag().AddItem(reward, IACR_ACTIVITY))
					DB_SERVICE.OnSendSysMail(player->getCid(), MI_FAMILYWAR_REWARD, reward);
			}
		}
	}
}

void CFamilyWar::onTimeEnd()
{
	CActivity::onTimeEnd();
}

void CFamilyWar::broadcastReady()
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM);
	if (packet == NULL) return;
	packet->writeInt32(BCI_WORLD_FAMILYWAR_READY);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CFamilyWar::broadcastStart()
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM);
	if (packet == NULL) return;
	packet->writeInt32(BCI_WORLD_FAMILYWAR_START);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CFamilyWar::broadcastWin(FamilyId_t nFamilyId, const std::string& strFamilyName)
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM);
	if (packet == NULL) return;
	packet->writeInt32(BCI_WORLD_FAMILYWAR_WIN);
	if (nFamilyId > 0)
		packet->writeUTF8(strFamilyName);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

FamilyId_t CFamilyWar::GetWinFamily()
{
	return m_nWinFamily;
}

std::string CFamilyWar::GetFamilyName()
{
	return m_strWingFamily;
}

std::string CFamilyWar::GetFamilyLeaderName()
{
	return "";
}

void CFamilyWar::OnFamilyWarResult(FamilyId_t nFamilyId, int16_t nTimes, std::string FamilyName, std::string LeadyerName)
{
	m_nWinFamily = nFamilyId;
	m_strWingFamily = FamilyName;
}

void CFamilyWar::addBuff(Player* player)
{
	if (player == NULL) return;

	CfgBuff* pCfgBuff = CFG_DATA.getBuff(ACTIVITY_FAMILY_WAR_BUFF_ID);
	if (pCfgBuff == NULL) return;

	SkillBuff* pBuff = new SkillBuff(*player, *pCfgBuff);
	if (pBuff == NULL) return;

	pBuff->init(pCfgBuff->id, 1, player->getHandle(), player->getHandle());
	player->addBuff(pBuff);
}

void CFamilyWar::removeBuff(Player* player)
{
	if (player)
		player->removeBuff(ACTIVITY_FAMILY_WAR_BUFF_ID);
}

Answer::NetPacket* CFamilyWar::packetActivityScore(int8_t connid)
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_NOTIFY_ACTIVITY_FAMILY_SCORE);
	if (packet == NULL) return NULL;

	packet->writeInt32(m_cfgActivity.id);
	packet->writeInt64(m_nWinFamily);
	packet->writeUTF8(m_strWingFamily);

	int8_t nCount = 0;
	uint32_t offset = packet->getWOffset();
	packet->writeInt8(nCount);

	m_lstFamilyScore.sort(std::greater<FamilyScore>());
	for (FamilyScoreList::iterator iter = m_lstFamilyScore.begin(); iter != m_lstFamilyScore.end(); ++iter)
	{
		packet->writeInt64(iter->nFamilyId);
		packet->writeUTF8(iter->strFamilyName);
		packet->writeInt32(iter->nScore);
		++nCount;
		if (nCount >= 4) break;
	}

	uint32_t oldOffset = packet->getWOffset();
	packet->setWOffset(offset);
	packet->writeInt8(nCount);
	packet->setWOffset(oldOffset);
	packet->setSize(packet->getWOffset());
	return packet;
}
