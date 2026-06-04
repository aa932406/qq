#include "stdafx.h"
#include "PDUDefine.h"
#include "Shared.h"
#include "CityWar.h"
#include "CfgData.h"
#include "ActivityMap.h"
#include "ActivityManager.h"
#include "GameService.h"
#include "Timer.h"
#include "DayTime.h"
#include "DBService.h"
#include "MapManager.h"
#include "FamilyManager.h"
#include "Player.h"

using namespace Answer;

// ==============================
// Constructor / Destructor
// ==============================

CCityWar::CCityWar(const CfgActivity& cfgActivity)
: CActivity(cfgActivity)
{
	m_pMonster = NULL;
	m_bUpdateFamilyScroe = false;
	m_nLastUpdateRankTick = 0;
	m_nFamilyId = 0;
	m_nLastFamilyId = 0;
	m_nWinTime = 0;
	m_nIndex = 0;
	m_Leader = 0;
	m_First = 0;
	m_Second = 0;
	m_Third = 0;
	m_FirstFamilyName.clear();
	m_SecondFamilyName.clear();
	m_ThirdFamilyName.clear();
}

CCityWar::~CCityWar()
{
}

// ==============================
// Init / reset
// ==============================

void CCityWar::Init()
{
	CActivity::Init();

	// Load city war results from database
	{
		MySqlDBGuard db(DBPOOL);
		char szSQL[MAX_SQL_LENGTH] = {};
		snprintf(szSQL, sizeof(szSQL) - 1,
			"SELECT `index`,`familyid`,`wintime`,`leader`,"
			"`first`,`second`,`third`,`firstfamily`,`secondfamily`,`thirdfamily` "
			"FROM `mem_city_war` WHERE `actid`=%d ORDER BY `index` DESC LIMIT 1",
			GetId());
		MySqlQuery result = db.query(szSQL);

		if (!result.eof())
		{
			m_nIndex = result.getIntValue("index");
			m_nFamilyId = (FamilyId_t)result.getInt64Value("familyid");
			m_nLastFamilyId = m_nFamilyId;
			m_nWinTime = result.getIntValue("wintime");
			m_Leader = (CharId_t)result.getInt64Value("leader");
			m_First = (CharId_t)result.getInt64Value("first");
			m_Second = (CharId_t)result.getInt64Value("second");
			m_Third = (CharId_t)result.getInt64Value("third");
			m_FirstFamilyName = result.getStringValue("firstfamily");
			m_SecondFamilyName = result.getStringValue("secondfamily");
			m_ThirdFamilyName = result.getStringValue("thirdfamily");
		}
	}

	// Load apply info from database
	LoadApplyInfo();
}

void CCityWar::LoadApplyInfo()
{
	m_Apply.clear();

	MySqlDBGuard db(DBPOOL);
	char szSQL[MAX_SQL_LENGTH] = {};
	snprintf(szSQL, sizeof(szSQL) - 1,
		"SELECT `applyinfo` FROM `mem_city_war_apply` WHERE `actid`=%d",
		GetId());
	MySqlQuery result = db.query(szSQL);

	if (!result.eof())
	{
		std::string applyInfo = result.getStringValue("applyinfo");
		// Parse "familyId:value|familyId:value|..." format
		std::string::size_type pos = 0;
		while (pos < applyInfo.length())
		{
			std::string::size_type colon = applyInfo.find(':', pos);
			if (colon == std::string::npos) break;
			std::string::size_type pipe = applyInfo.find('|', colon + 1);
			if (pipe == std::string::npos) pipe = applyInfo.length();

			FamilyId_t nFamilyId = (FamilyId_t)atol(applyInfo.substr(pos, colon - pos).c_str());
			int32_t nValue = atoi(applyInfo.substr(colon + 1, pipe - colon - 1).c_str());
			m_Apply[nFamilyId] = nValue;

			pos = pipe + 1;
		}
	}

	LOG_INFO("CCityWar::LoadApplyInfo: actId=%d loaded %d apply records",
		GetId(), (int32_t)m_Apply.size());
}

void CCityWar::reset()
{
	CActivity::reset();
	m_mPlayerScore.clear();
	m_mFamilyScore.clear();
	m_lFamilyScore.clear();
	m_Apply.clear();
	m_pMonster = NULL;
	m_bUpdateFamilyScroe = false;
	m_nLastUpdateRankTick = 0;
	m_FirstFamilyName.clear();
	m_SecondFamilyName.clear();
	m_ThirdFamilyName.clear();
	m_Leader = 0;
	m_First = 0;
	m_Second = 0;
	m_Third = 0;
}

// ==============================
// Date/week checks
// ==============================

bool CCityWar::isSpecialDay(int32_t nStartDays)
{
	return nStartDays == 2;
}

bool CCityWar::isInSpecialDay(int32_t nStartDays)
{
	return nStartDays <= 2;
}

bool CCityWar::checkData()
{
	int32_t startDays = CFG_DATA.getServerDiffTime();

	if (isSpecialDay(startDays))
		return true;
	if (isInSpecialDay(startDays))
		return false;
	return CActivity::checkData();
}

bool CCityWar::checkWeek()
{
	int32_t startDays = CFG_DATA.getServerDiffTime();

	if (isSpecialDay(startDays))
		return true;
	if (isInSpecialDay(startDays))
		return false;
	return CActivity::checkWeek();
}

// ==============================
// Player management
// ==============================

void CCityWar::addPlayer(Player* player)
{
	if (NULL == player)
	{
		return;
	}

	FamilyId_t nFamilyId = player->getFamilyId();
	int8_t nCamp = GetFamilyCamp(nFamilyId);

	// Set PK mode based on camp
	if (player->getFamilyId() > 0)
	{
		if (nCamp == 2)
			player->setPkMode(7, false);
		else
			player->setPkMode(3, false);
	}
	else
	{
		player->setPkMode(6, false);
	}

	// Find or create player score record
	PlayerScoreMap::iterator iter = m_mPlayerScore.find(player->getCid());
	if (iter != m_mPlayerScore.end())
	{
		iter->second.bInActivity = 1;
		iter->second.nTime = TIMER.GetNow();
	}
	else
	{
		// Create new player score
		PlayerScore score;
		memset(&score, 0, sizeof(score));
		score.nCharId = player->getCid();
		score.strName = player->getName();
		score.nFamilyId = player->getFamilyId();
		score.strFamilyName = player->getFamilyName();
		score.bInActivity = 1;
		score.nFamilyPosition = player->getFamilyPosition();
		score.nTime = TIMER.GetNow();

		std::pair<PlayerScoreMap::iterator, bool> ret =
			m_mPlayerScore.insert(std::make_pair(score.nCharId, score));
		iter = ret.first;

		PlayerScore* pScore = &iter->second;

		// Link to family score
		if (player->getFamilyId() > 0)
		{
			FamilyScoreMap::iterator famIter = m_mFamilyScore.find(player->getFamilyId());
			if (famIter != m_mFamilyScore.end())
			{
				famIter->second.lScoreRank.push_back(pScore);
			}
			else
			{
				// Create new family score
				FamilyScore family;
				memset(&family, 0, sizeof(family));
				family.nFamilyId = player->getFamilyId();
				family.strFamilyName = player->getFamilyName();
				family.nFamilyCamp = nCamp;
				family.lScoreRank.push_back(pScore);

				std::pair<FamilyScoreMap::iterator, bool> famRet =
					m_mFamilyScore.insert(std::make_pair(family.nFamilyId, family));
				famIter = famRet.first;

				m_lFamilyScore.push_back(&famIter->second);
			}
		}
	}

	// Set act state for owning family members
	if (player->getFamilyId() == m_nFamilyId)
	{
		player->SetActState(1);
	}

	CActivity::addPlayer(player);
}

void CCityWar::removePlayer(Player* player, bool islogout)
{
	if (NULL == player)
	{
		return;
	}

	PlayerScoreMap::iterator iter = m_mPlayerScore.find(player->getCid());
	if (iter != m_mPlayerScore.end())
	{
		iter->second.bInActivity = 0;
		int32_t nSeconds = iter->second.nSeconds;
		int32_t now = TIMER.GetNow();
		iter->second.nSeconds = nSeconds + now - iter->second.nTime;
	}

	CActivity::removePlayer(player, islogout);

	if (m_nState == AS_RUNNING)
	{
		checkChangeFamily(false);
	}

	// Update act state based on city war winner
	FamilyId_t winFamily = ACTIVITY_MANAGER.GetCityWarWinner();
	if (winFamily > 0 && player->getFamilyId() == winFamily)
	{
		player->SetActState(1);
	}
	else if (player->GetActState())
	{
		player->SetActState(0);
	}
}

int32_t CCityWar::canEnter(Player* player, CActivityMap* pTargetMap) const
{
	if (NULL == player || NULL == pTargetMap)
	{
		return ERR_INVALID_DATA;
	}

	int32_t nMapId = pTargetMap->GetId();

	// Map 50130 is the city war main map
	if (nMapId == 50130)
	{
		// Use non-const version of getLeftTime via const_cast since canEnter is const but we need access
		// Actually, getLeftTime() is non-const in the base, so we check a simpler condition
		if (m_nState != AS_RUNNING)
		{
			player->TiShiInfo(19, 0);
			return ERR_INVALID_DATA;
		}
	}

	// If the guardian monster is alive, only owning family can enter
	if (player->getFamilyId() != m_nFamilyId
		&& nMapId == 50130
		&& m_pMonster != NULL
		&& m_pMonster->isAlive())
	{
		player->TiShiInfo(18, 0);
		return ERR_INVALID_DATA;
	}

	return CActivity::canEnter(player, pTargetMap);
}

bool CCityWar::canGetOnlineReward(const PlayerScore* score)
{
	int32_t nOnlineTime = score->nSeconds;
	if (score->bInActivity)
	{
		nOnlineTime += TIMER.GetNow() - score->nTime;
	}
	return nOnlineTime > 299;
}

Position CCityWar::GetRandBornPos(Player* player)
{
	if (NULL == player)
	{
		Position pos(-1, -1);
		return pos;
	}

	if (player->getFamilyId() == m_nFamilyId)
	{
		return getBornRandPosB();
	}
	else
	{
		return getBornRandPosA();
	}
}

int32_t CCityWar::GetRevive(Player* player)
{
	if (NULL == player)
	{
		int32_t lenth = (int32_t)m_cfgActivity.target_regiona.size();
		if (lenth == 1)
			return m_cfgActivity.target_regiona[0];
		else if (lenth > 1)
			return m_cfgActivity.target_regiona[RANDOM.generate(0, lenth - 1)];
		return 0;
	}

	if (player->getFamilyId() == m_nFamilyId)
	{
		int32_t lenth = (int32_t)m_cfgActivity.target_regionb.size();
		if (lenth == 1)
			return m_cfgActivity.target_regionb[0];
		else if (lenth > 1)
			return m_cfgActivity.target_regionb[RANDOM.generate(0, lenth - 1)];
		return 0;
	}
	else
	{
		int32_t lenth = (int32_t)m_cfgActivity.target_regiona.size();
		if (lenth == 1)
			return m_cfgActivity.target_regiona[0];
		else if (lenth > 1)
			return m_cfgActivity.target_regiona[RANDOM.generate(0, lenth - 1)];
		return 0;
	}
}

bool CCityWar::OnChangeMap(Player* player, CActivityMap* pMap, int32_t nX, int32_t nY, int32_t nParam)
{
	if (NULL == player || NULL == pMap)
	{
		return false;
	}

	if (!pMap->isWalkablePosition(nX, nY))
	{
		return false;
	}

	// Check if this map belongs to the activity
	bool bFind = false;
	for (std::list<CActivityMap*>::iterator iter = m_activityMaps.begin(); iter != m_activityMaps.end(); ++iter)
	{
		CActivityMap* tp = *iter;
		if (tp != NULL && tp == pMap)
		{
			bFind = true;
			break;
		}
	}

	if (!bFind)
	{
		return false;
	}

	// Check if we need to check for family change (city war map transition)
	bool bCheckChangeWinFamily = false;
	if (player->getMapId() == 50130 || pMap->GetId() == 50130)
	{
		bCheckChangeWinFamily = true;
	}

	int32_t nErr = player->switchMap(pMap, nX, nY, 1);
	if (nErr == 0 && bCheckChangeWinFamily)
	{
		checkChangeFamily(false);
	}
	return nErr == 0;
}

// ==============================
// Battle
// ==============================

void CCityWar::onMonsterAdd(MonsterActivity* pMonster)
{
	m_pMonster = pMonster;
}

void CCityWar::onMonsterDie(MonsterActivity* pMonster, Player* pKiller)
{
	if (NULL == pMonster || NULL == pKiller || !IsRuning())
	{
		return;
	}

	// Broadcast monster killed announcement (0x2CD6, action=336)
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2CD6);
	if (NULL != packet)
	{
		packet->writeInt32(336);
		packet->writeInt64(pKiller->getCid());
		packet->writeUTF8(pKiller->getName());
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.worldBroadcast(packet);
	}
}

void CCityWar::onPlayerKilled(Player* pDier, Player* pAttacker)
{
	if (NULL == pDier || NULL == pAttacker)
	{
		return;
	}

	// Calculate score based on victim's family position
	int32_t nScore = 0;
	int32_t nFamilyPos = pDier->getFamilyPosition();
	if (nFamilyPos == 2 || nFamilyPos == 3)
	{
		nScore = 200;
	}
	else if (nFamilyPos == 1)
	{
		nScore = 150;
	}
	else
	{
		nScore = 100;
	}

	int32_t now = TIMER.GetNow();
	FamilyId_t nFamilyId = pAttacker->getFamilyId();

	addFamilyScore(nFamilyId, nScore, now);
	addPlayerScore(pAttacker, nScore);

	// Check for family change if on the city war main map
	if (pDier->getMapId() == 50130)
	{
		checkChangeFamily(false);
	}

	// Broadcast kill announcement (0x2CD6, action=341)
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2CD6);
	if (NULL != packet)
	{
		packet->writeInt32(341);
		packet->writeUTF8(pDier->getName());
		packet->writeInt64(pAttacker->getCid());
		packet->writeInt32(pAttacker->getMapId());
		packet->writeUTF8(pAttacker->getName());
		packet->writeInt64(pDier->getCid());
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.worldBroadcast(packet);
	}
}

// ==============================
// Scoring
// ==============================

void CCityWar::addPlayerScore(Player* player, int32_t nScore)
{
	if (NULL == player)
	{
		return;
	}

	PlayerScoreMap::iterator iter = m_mPlayerScore.find(player->getCid());
	if (iter != m_mPlayerScore.end())
	{
		iter->second.nScore += nScore;
		iter->second.nScoreTime = TIMER.GetNow();
		setUpdateScoreRank(player->getFamilyId(), nScore > 0);
	}
}

void CCityWar::setUpdateScoreRank(FamilyId_t nFamilyId, bool bScoreRank)
{
	FamilyScoreMap::iterator iter = m_mFamilyScore.find(nFamilyId);
	if (iter != m_mFamilyScore.end() && bScoreRank)
	{
		iter->second.bUpdateScroeRank = 1;
	}
}

void CCityWar::addFamilyScore(FamilyId_t nFamilyId, int32_t nScore, int32_t nNowTime)
{
	FamilyScoreMap::iterator iter = m_mFamilyScore.find(nFamilyId);
	if (iter != m_mFamilyScore.end())
	{
		iter->second.nScore += nScore;
		iter->second.nScoreTime = nNowTime;
		m_bUpdateFamilyScroe = true;
		setNeedBroadcastActivityScore();
	}
}

void CCityWar::getFamilyMembers(FamilyId_t nFamilyId, CharIdList* cids)
{
	if (NULL == cids)
	{
		return;
	}

	for (PlayerScoreMap::iterator iter = m_mPlayerScore.begin(); iter != m_mPlayerScore.end(); ++iter)
	{
		const PlayerScore& score = iter->second;
		if (score.nFamilyId == nFamilyId && score.bInActivity)
		{
			cids->push_back(score.nCharId);
		}
	}
}

// ==============================
// Family management
// ==============================

int8_t CCityWar::GetFamilyCamp(FamilyId_t nFamilyId)
{
	if (nFamilyId <= 0)
	{
		return 4;
	}

	if (nFamilyId == m_nLastFamilyId)
	{
		return 1;  // Defending family (previous owner)
	}

	ApplyMap::iterator iter = m_Apply.find(nFamilyId);
	if (iter == m_Apply.end())
	{
		return 4;  // Not a participant
	}

	if (iter->second == 1)
	{
		return 2;  // Challenger (manual apply)
	}

	return 3;  // Other participant
}

bool CCityWar::HaveOwner()
{
	return m_nFamilyId > 0;
}

void CCityWar::SetOwner()
{
	for (PlayerList::iterator iter = m_players.begin(); iter != m_players.end(); ++iter)
	{
		Player* player = *iter;
		if (NULL == player)
		{
			continue;
		}

		if (player->getFamilyId() == m_nFamilyId)
		{
			player->SetActState(1);
		}
		else if (player->GetActState())
		{
			player->SetActState(0);
		}
	}
}

FamilyId_t CCityWar::GetGuilDerFirstFamily()
{
	// Sort family scores to find the top challenger (camp 2)
	m_lFamilyScore.sort(FamilyScoreRankGreater());

	for (FamilyScoreList::iterator iter = m_lFamilyScore.begin(); iter != m_lFamilyScore.end(); ++iter)
	{
		FamilyScore* pScore = *iter;
		if (pScore != NULL && pScore->nFamilyCamp == 2)
		{
			return pScore->nFamilyId;
		}
	}

	return 0;
}

void CCityWar::checkChangeFamily(bool IsUpdateScore)
{
	if (IsUpdateScore)
	{
		// Called from OnUpdate timer - check if current owner's camp status changed
		FamilyScoreMap::iterator it = m_mFamilyScore.find(m_nFamilyId);
		if (it != m_mFamilyScore.end() && it->second.nFamilyCamp == 2)
		{
			FamilyId_t newFamilyId = GetGuilDerFirstFamily();
			if (newFamilyId > 0 && m_nFamilyId != newFamilyId)
			{
				m_nFamilyId = newFamilyId;
				GongGao(newFamilyId, 337);
				setNeedBroadcastActivityScore();
			}
		}
		return;
	}

	// Called from removePlayer/onPlayerKilled - check live players on map 50130
	int8_t WinCamp = 0;
	FamilyId_t winFamilyId = 0;

	for (PlayerList::iterator iter = m_players.begin(); iter != m_players.end(); ++iter)
	{
		Player* player = *iter;
		if (NULL == player || !player->isAlive() || player->getMapId() != 50130)
		{
			continue;
		}

		// Determine this player's camp based on their family
		int8_t camp = GetFamilyCamp(player->getFamilyId());
		if (camp <= 0 || camp == 4)
		{
			return;  // Neutral or invalid, no change
		}

		if (WinCamp != 0 && camp != WinCamp)
		{
			break;  // Different camps on the field, no change
		}

		if (WinCamp == 3)
		{
			if (player->getFamilyId() != winFamilyId)
			{
				if (!HaveOwner() && m_nFamilyId > 0)
				{
					continue;
				}
				return;
			}
		}

		WinCamp = camp;
		winFamilyId = player->getFamilyId();
	}

	if (WinCamp > 0)
	{
		if (WinCamp == 2)
		{
			FamilyId_t newFamilyId = GetGuilDerFirstFamily();
			if (newFamilyId > 0 && m_nFamilyId != newFamilyId)
			{
				m_nFamilyId = newFamilyId;
				GongGao(newFamilyId, 337);
				SetOwner();
				setNeedBroadcastActivityScore();
			}
		}
		else if (WinCamp == 3 || WinCamp == 1)
		{
			if (m_nFamilyId != winFamilyId)
			{
				m_nFamilyId = winFamilyId;
				GongGao(winFamilyId, 337);
				SetOwner();
				setNeedBroadcastActivityScore();
			}
		}
	}
}

// ==============================
// Updates
// ==============================

void CCityWar::OnUpdate(CActivityMap* pMap)
{
	CActivity::OnUpdate(pMap);

	if (NULL == pMap)
	{
		return;
	}

	if (m_nState == AS_RUNNING)
	{
		int64_t nCurTick = pMap->getTick();
		if (nCurTick - m_nLastUpdateRankTick > 999)
		{
			m_nLastUpdateRankTick = nCurTick;

			// Update family score rankings
			if (m_bUpdateFamilyScroe)
			{
				m_bUpdateFamilyScroe = false;
				m_lFamilyScore.sort(FamilyScoreRankGreater());
				checkChangeFamily(true);
				setNeedBroadcastActivityScore();
			}

			// Update player score rankings per family
			for (FamilyScoreMap::iterator iter = m_mFamilyScore.begin();
				iter != m_mFamilyScore.end();
				++iter)
			{
				FamilyScore& score = iter->second;

				CharIdList cids;
				if (score.bUpdateScroeRank)
				{
					getFamilyMembers(score.nFamilyId, &cids);
				}

				if (score.bUpdateScroeRank)
				{
					score.bUpdateScroeRank = 0;
					score.lScoreRank.sort(PlayerScoreRankGreater());

					Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2E2A);
					if (NULL == packet)
					{
						continue;
					}
					appendPlayerRankInfo(packet, &score.lScoreRank);
					packet->setSize(packet->getWOffset());
					GAME_SERVICE.broadcast(packet, cids);
				}
			}
		}
	}
}

// ==============================
// Activity lifecycle
// ==============================

void CCityWar::onActivityStart()
{
	++m_nIndex;

	// Update title: clear previous owner
	GAME_SERVICE.UpdateCityWarTitle(m_nLastFamilyId, 0);

	// Reset city war winner
	ACTIVITY_MANAGER.SetCityWarWinner(0);

	sendSocialUpdateActivityState(1);

	// Clear act state for the current owner family
	GAME_SERVICE.UpdateCityActState(m_nFamilyId, 0);

	m_nFamilyId = m_nLastFamilyId;

	// Auto apply top families
	AutoApplyCityWar();
}

void CCityWar::onTimeEnd()
{
	win();
	m_Apply.clear();
	CActivity::onTimeEnd();
}

int32_t CCityWar::getNextStartTime()
{
	if (IsRuning() || m_cfgActivity.start_hour.empty())
	{
		return 0;
	}

	int32_t nNowTime = TIMER.GetNow();
	const tm& localnow = TIMER.GetLocalNow();
	int32_t nowMinute = localnow.tm_hour * 60 + localnow.tm_min;

	int32_t startMinute = m_cfgActivity.start_hour[0];
	for (size_t i = 0; i < m_cfgActivity.start_hour.size(); ++i)
	{
		int32_t startTime = DayTime::dayzero(nNowTime) + m_cfgActivity.start_hour[i] * 60;
		if (startMinute > nNowTime)
		{
			startMinute = m_cfgActivity.start_hour[i];
			break;
		}
	}

	int32_t days = -1;
	int32_t startDays = CFG_DATA.getServerDiffTime();
	int32_t SpecialDay = 2;

	if (checkData() && checkWeek())
	{
		if (nowMinute < startMinute)
		{
			days = 0;
		}
	}
	else if (isInSpecialDay(startDays) && startDays < SpecialDay)
	{
		days = SpecialDay - startDays;
	}

	if (days < 0)
	{
		if (m_cfgActivity.weekday.empty())
		{
			return 0;
		}

		int32_t weekday = TIMER.GetWeekDay();
		int32_t nextweekday = 0;

		// Find next weekday from reversed list
		Int32Vector::const_reverse_iterator iter = m_cfgActivity.weekday.rbegin();
		for (; iter != m_cfgActivity.weekday.rend(); ++iter)
		{
			if (*iter > weekday)
			{
				break;
			}
		}

		if (iter == m_cfgActivity.weekday.rend())
		{
			nextweekday = m_cfgActivity.weekday.back();
		}
		else
		{
			nextweekday = *iter;
		}

		if (nextweekday >= weekday)
		{
			days = nextweekday - weekday;
		}
		else
		{
			days = nextweekday + 7 - weekday;
		}
	}

	return DayTime::dayzero(nNowTime) + days * 86400 + startMinute * 60;
}

// ==============================
// Win / Rewards / Save
// ==============================

void CCityWar::win()
{
	// Update city war title if family changed
	if (m_nFamilyId != m_nLastFamilyId)
	{
		m_nWinTime = DayTime::now();
		GAME_SERVICE.UpdateCityWarTitle(m_nLastFamilyId, m_nFamilyId);
		m_nLastFamilyId = m_nFamilyId;
	}

	// Set the winner
	ACTIVITY_MANAGER.SetCityWarWinner(m_nFamilyId);

	if (m_nFamilyId > 0)
	{
		// Update act state for owning family
		GAME_SERVICE.UpdateCityActState(m_nFamilyId, 1);

		// Get family info for notifications
		FamilyInfo info = FAMILY_MANAGER.GetFamilyInfo(m_nFamilyId);
		if (!info.IsEmpty())
		{
			// Send leader notification mail
			if (m_nIndex == 1)
			{
				DB_SERVICE.OnSendSysMail(info.nId, 6397);
			}

			// Broadcast family victory (0x2CD6, action=338)
			Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2CD6);
			if (NULL != packet)
			{
				packet->writeInt32(338);
				packet->writeUTF8(info.strName);
				packet->setSize(packet->getWOffset());
				GAME_SERVICE.worldBroadcast(packet);
			}
		}
	}
	else
	{
		// No winner - broadcast draw (0x2CD6, action=339)
		Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2CD6);
		if (NULL != packet)
		{
			packet->writeInt32(339);
			packet->setSize(packet->getWOffset());
			GAME_SERVICE.worldBroadcast(packet);
		}
	}

	m_nState = AS_END;
	addRewards();
	saveResult();
	delayKickAll(30);
}

void CCityWar::addRewards()
{
	// Sort family scores by rank
	m_lFamilyScore.sort(FamilyScoreRankGreater());

	// Participation rewards for all qualifying players
	for (PlayerScoreMap::iterator it = m_mPlayerScore.begin(); it != m_mPlayerScore.end(); ++it)
	{
		const PlayerScore& score = it->second;
		if (canGetOnlineReward(&score))
		{
			DB_SERVICE.OnSendSysMail(score.nCharId, MI_CITY_WAR_REWARD);
		}
	}

	// Family ranking rewards
	int32_t nFamilyIndex = 0;
	int32_t nOtherFamilyIndex = 2;

	for (FamilyScoreList::iterator it = m_lFamilyScore.begin(); it != m_lFamilyScore.end(); ++it)
	{
		FamilyScore* pScore = *it;
		if (NULL == pScore)
		{
			continue;
		}

		if (pScore->nFamilyId == m_nFamilyId)
		{
			nFamilyIndex = 1;
		}
		else
		{
			nFamilyIndex = nOtherFamilyIndex++;
		}

		if (nFamilyIndex > 3 || (pScore->nScore <= 0 && nFamilyIndex != 1))
		{
			continue;
		}

		// Record family names
		if (nFamilyIndex == 2)
		{
			m_FirstFamilyName = pScore->strFamilyName;
		}
		else if (nFamilyIndex == 3)
		{
			m_SecondFamilyName = pScore->strFamilyName;
		}

		// Award individual ranking rewards within the family
		int32_t nPlayerIndex = 0;
		int32_t nOtherPlayerIndex = 2;

		for (PlayerScoreRank::iterator pit = pScore->lScoreRank.begin();
			pit != pScore->lScoreRank.end();
			++pit)
		{
			PlayerScore* pPlayerScore = *pit;
			if (NULL == pPlayerScore)
			{
				continue;
			}

			if (pPlayerScore->nFamilyPosition == 3)
			{
				nPlayerIndex = 1;  // Family leader
			}
			else
			{
				nPlayerIndex = nOtherPlayerIndex++;
			}

			if (nPlayerIndex > 6)
			{
				continue;
			}

			if (pPlayerScore->nScore <= 0 && nPlayerIndex != 1)
			{
				continue;
			}

			// Determine reward index (1-4)
			int32_t nRewardIndex = 0;
			switch (nPlayerIndex)
			{
			case 1:
				nRewardIndex = 1;
				break;
			case 2:
				if (nFamilyIndex == 1)
					m_First = pPlayerScore->nCharId;
				nRewardIndex = 2;
				break;
			case 3:
				if (nFamilyIndex == 1)
					m_Second = pPlayerScore->nCharId;
				nRewardIndex = 3;
				break;
			case 4:
				if (nFamilyIndex == 1)
					m_Third = pPlayerScore->nCharId;
				nRewardIndex = 3;
				break;
			case 5:
			case 6:
				nRewardIndex = 4;
				break;
			}

			if (nRewardIndex > 0)
			{
				int32_t nMailId = MI_CITY_WAR_REWARD;
				if (nFamilyIndex == 1)
				{
					switch (nRewardIndex)
					{
					case 1: nMailId = MI_CITY_WAR_RANK1; break;
					case 2: nMailId = MI_CITY_WAR_RANK2; break;
					case 3: nMailId = MI_CITY_WAR_RANK3; break;
					default: nMailId = MI_CITY_WAR_REWARD; break;
					}
				}
				DB_SERVICE.OnSendSysMail(pPlayerScore->nCharId, nMailId);
			}
		}
	}
}

void CCityWar::saveResult()
{
	DB_SERVICE.SaveCityWarResult(GetId(), m_nIndex, m_nFamilyId, m_nWinTime,
		m_Leader, m_First, m_Second, m_Third,
		m_FirstFamilyName, m_SecondFamilyName, m_ThirdFamilyName);

	LOG_INFO("CCityWar::saveResult: actId=%d index=%d winnerFamily=%lld winTime=%d",
		GetId(), m_nIndex, m_nFamilyId, m_nWinTime);
}

// ==============================
// Network: Info / Score
// ==============================

void CCityWar::SendPlayerActivityInfo(Player* player)
{
	if (NULL == player)
	{
		return;
	}

	if (m_nLastFamilyId <= 0)
	{
		// No owner yet - send 0x2E22 with basic info
		Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2E22);
		if (NULL == packet)
		{
			return;
		}
		packet->writeInt32(m_cfgActivity.id);
		packet->writeInt32(getNextStartTime());
		packet->writeInt32(0);
		packet->writeInt64(m_nLastFamilyId);
		packet->writeUTF8(m_FirstFamilyName);
		packet->writeUTF8(m_SecondFamilyName);
		packet->writeUTF8(m_ThirdFamilyName);
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.sendPacketTo(player->getGateIndex(), packet);
	}
	else
	{
		// Has owner - send 0x4EAD with full detail
		Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x4EAD);
		if (NULL == packet)
		{
			return;
		}
		packet->writeInt32(player->getGateIndex());
		packet->writeInt32(m_cfgActivity.id);
		packet->writeInt32(getNextStartTime());
		packet->writeInt64(m_nLastFamilyId);
		packet->writeInt32(m_nWinTime);
		packet->writeInt64(m_First);
		packet->writeInt64(m_Second);
		packet->writeInt64(m_Third);
		packet->writeUTF8(m_FirstFamilyName);
		packet->writeUTF8(m_SecondFamilyName);
		packet->writeUTF8(m_ThirdFamilyName);
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.sendPacketTo(player->getGateIndex(), packet);
	}

	// Also send apply info
	SendAppyCityWarInfo(player);
}

void CCityWar::SendPlayerActivityScore(Player* player, int32_t nLeftTime)
{
	if (NULL == player)
	{
		return;
	}

	// Send family score packet via packetActivityScoreForPlayer
	Answer::NetPacket* familyPacket = packetActivityScoreForPlayer(player->getGateIndex());
	if (NULL != familyPacket)
	{
		GAME_SERVICE.sendPacketTo(player->getGateIndex(), familyPacket);
	}

	// Send player ranking within family via 0x2E2A
	FamilyScoreMap::iterator famIter = m_mFamilyScore.find(player->getFamilyId());
	if (famIter != m_mFamilyScore.end())
	{
		Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2E2A);
		if (NULL != packet)
		{
			appendPlayerRankInfo(packet, &famIter->second.lScoreRank);
			packet->setSize(packet->getWOffset());
			GAME_SERVICE.sendPacketTo(player->getGateIndex(), packet);
		}
	}
}

Answer::NetPacket* CCityWar::packetActivityScore()
{
	// No-param version - called from base class broadcast mechanism
	// Use connId=0 for world broadcast
	return packetActivityScoreForPlayer(0);
}

Answer::NetPacket* CCityWar::packetActivityScoreForPlayer(int32_t nConnId)
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2E26);
	if (NULL == packet)
	{
		return NULL;
	}

	packet->writeInt32(m_cfgActivity.id);
	packet->writeInt64(m_nFamilyId);

	// Write current owner family name
	if (m_nFamilyId <= 0)
	{
		packet->writeUTF8("");
	}
	else
	{
		FamilyInfo info = FAMILY_MANAGER.GetFamilyInfo(m_nFamilyId);
		packet->writeUTF8(info.IsEmpty() ? "" : info.strName);
	}

	packet->writeInt32(getLeftTime());

	// Write family scores with count fixup
	int32_t nCount = 0;
	uint32_t oldOffset = packet->getWOffset();
	packet->writeInt32(0);

	for (FamilyScoreList::iterator iter = m_lFamilyScore.begin();
		iter != m_lFamilyScore.end();
		++iter)
	{
		FamilyScore* pScore = *iter;
		if (NULL == pScore || pScore->nScore <= 0)
		{
			break;
		}
		packet->writeInt64(pScore->nFamilyId);
		packet->writeUTF8(pScore->strFamilyName);
		packet->writeInt32(pScore->nScore);
		++nCount;
	}

	uint32_t newOffset = packet->getWOffset();
	packet->setWOffset(oldOffset);
	packet->writeInt32(nCount);
	packet->setWOffset(newOffset);

	packet->setSize(packet->getWOffset());
	return packet;
}

void CCityWar::appendPlayerRankInfo(Answer::NetPacket* packet, const PlayerScoreRank* rank)
{
	if (NULL == packet || NULL == rank)
	{
		return;
	}

	packet->writeInt32(GetId());

	int32_t nCount = 0;
	uint32_t oldOffset = packet->getWOffset();
	packet->writeInt32(0);

	for (PlayerScoreRank::const_iterator iter = rank->begin(); iter != rank->end(); ++iter)
	{
		const PlayerScore* pScore = *iter;
		if (NULL == pScore)
		{
			continue;
		}
		packet->writeInt64(pScore->nCharId);
		packet->writeUTF8(pScore->strName);
		packet->writeInt32(pScore->nScore);
		++nCount;
	}

	uint32_t newOffset = packet->getWOffset();
	packet->setWOffset(oldOffset);
	packet->writeInt32(nCount);
	packet->setWOffset(newOffset);
}

// ==============================
// Network: Broadcasts
// ==============================

void CCityWar::broadcastReady()
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2CD6);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(332);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CCityWar::broadcastStart()
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2CD6);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(335);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);

	// Notify map 50133 to start the city war sequence
	Map* pMap = MAP_MANAGER.GetMap(50133);
	if (pMap != NULL)
	{
		int32_t nRunnerId = pMap->GetRunnerId();
		MAP_MANAGER.PostMsg(nRunnerId, GMC_ACTIVITY_START, pMap, this);
	}

	// Clear previous ranking data
	m_FirstFamilyName.clear();
	m_SecondFamilyName.clear();
	m_ThirdFamilyName.clear();
	m_First = 0;
	m_Second = 0;
	m_Third = 0;
}

void CCityWar::NotOccupyGongGao()
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2CD6);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(342);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CCityWar::GongGao(FamilyId_t nFamilyId, int32_t nGongGaoId)
{
	FamilyInfo info = FAMILY_MANAGER.GetFamilyInfo(nFamilyId);
	if (info.IsEmpty())
	{
		return;
	}

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2CD6);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(nGongGaoId);
	packet->writeUTF8(info.strName);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

// ==============================
// Apply System
// ==============================

void CCityWar::SendAppyCityWarInfo(Player* player)
{
	if (NULL == player)
	{
		return;
	}

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2E2E);
	if (NULL == packet)
	{
		return;
	}

	packet->writeInt32((int32_t)m_Apply.size());

	for (ApplyMap::iterator iter = m_Apply.begin(); iter != m_Apply.end(); ++iter)
	{
		packet->writeInt64(iter->first);

		FamilyInfo info = FAMILY_MANAGER.GetFamilyInfo(iter->first);
		packet->writeUTF8(info.IsEmpty() ? "" : info.strName);

		packet->writeInt32(iter->second);
	}

	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(player->getGateIndex(), packet);
}

void CCityWar::ApplyCityWar(Player* player, FamilyId_t nFamilyId, int32_t nType)
{
	if (NULL == player || nFamilyId == m_nFamilyId)
	{
		return;
	}

	ApplyMap::iterator iter = m_Apply.find(nFamilyId);
	if (iter != m_Apply.end())
	{
		if (nType == 2)
		{
			iter->second = 1;
			SendAppyCityWarInfo(player);
			SaveApplyInfo();
		}
		else if (nType == 3)
		{
			iter->second = TIMER.GetNow();
			SendAppyCityWarInfo(player);
			SaveApplyInfo();
		}
	}
	else if (nType == 1)
	{
		m_Apply[nFamilyId] = 0;
		SendAppyCityWarInfo(player);
		SaveApplyInfo();
		GongGao(nFamilyId, 340);
	}
}

void CCityWar::AutoApplyCityWar()
{
	MySqlDBGuard db(DBPOOL);
	char szSQL[MAX_SQL_LENGTH] = {};
	snprintf(szSQL, sizeof(szSQL) - 1,
		"SELECT `id` FROM `mem_family` WHERE `delflag`=0 ORDER BY `level` DESC LIMIT 10");
	MySqlQuery result = db.query(szSQL);

	while (!result.eof())
	{
		FamilyId_t nFamilyId = (FamilyId_t)result.getInt64Value("id");
		result.nextRow();

		if (nFamilyId == m_nFamilyId)
		{
			continue;
		}

		ApplyMap::iterator iter = m_Apply.find(nFamilyId);
		if (iter == m_Apply.end())
		{
			m_Apply[nFamilyId] = 0;
			SaveApplyInfo();
			GongGao(nFamilyId, 340);
		}
	}
}

void CCityWar::SaveApplyInfo()
{
	// Serialize apply map to string (familyId:value|familyId:value|...)
	std::stringstream ss;
	for (ApplyMap::iterator iter = m_Apply.begin(); iter != m_Apply.end(); ++iter)
	{
		if (iter != m_Apply.begin())
		{
			ss << "|";
		}
		ss << iter->first << ":" << iter->second;
	}

	MySqlDBGuard db(DBPOOL);
	char szSQL[MAX_SQL_LENGTH] = {};
	snprintf(szSQL, sizeof(szSQL) - 1,
		"INSERT INTO `mem_city_war_apply` (`index`,`applyinfo`) VALUES (%d,'%s') "
		"ON DUPLICATE KEY UPDATE `applyinfo`='%s'",
		m_nIndex + 1, ss.str().c_str(), ss.str().c_str());
	db.excute(szSQL);
}

// ==============================
// Result callback
// ==============================

void CCityWar::OnCityWarResult(int32_t nIndex, FamilyId_t nFamilyId, int32_t nWinTime,
	CharId_t nLeader, CharId_t nFirst, CharId_t nSecond, CharId_t nThird,
	const std::string& strFirstFamily, const std::string& strSecondFamily,
	const std::string& strThirdFamily)
{
	m_nState = AS_END;
	m_nIndex = nIndex;
	m_nFamilyId = nFamilyId;
	m_nLastFamilyId = nFamilyId;
	m_nWinTime = nWinTime;
	m_Leader = nLeader;
	m_First = nFirst;
	m_Second = nSecond;
	m_Third = nThird;
	m_FirstFamilyName = strFirstFamily;
	m_SecondFamilyName = strSecondFamily;
	m_ThirdFamilyName = strThirdFamily;

	GAME_SERVICE.broadFamilyWarIcon();
}
