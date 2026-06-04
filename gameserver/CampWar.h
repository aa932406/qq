#ifndef __CAMP_WAR_H__
#define __CAMP_WAR_H__

#include "stdafx.h"
#include "Activity.h"
#include "MonsterActivity.h"
#include <map>
#include <set>

class CCampWar : public CActivity
{
public:
	struct PlayerScore
	{
		PlayerScore()
		{
			nCharId = 0;
			strName = "";
			nConnId = 0;
			nCamp = 0;
			nScore = 0;
			nKillCount = 0;
			nContKill = 0;
			nContLevel = 0;
			nRankIndex = 0;
			bInActivity = 0;
			nStartTick = 0;
			nTitle = 0;
		}

		CharId_t	nCharId;
		std::string	strName;
		int32_t		nConnId;
		int8_t		nCamp;
		int32_t		nScore;
		int32_t		nKillCount;
		int32_t		nContKill;
		int32_t		nContLevel;
		int32_t		nRankIndex;
		int8_t		bInActivity;
		int64_t		nStartTick;
		int32_t		nTitle;
	};

	struct PlayerScoreGreater
	{
		bool operator()(const PlayerScore* a, const PlayerScore* b) const
		{
			if (a->nScore != b->nScore)
				return a->nScore > b->nScore;
			return a->nCharId < b->nCharId;
		}
	};

public:
	CCampWar(const CfgActivity& cfgActivity);
	virtual ~CCampWar();

public:
	virtual void OnUpdate(CActivityMap* pMap);
	virtual void onPlayerKilled(Player* pDier, Player* pKiller);
	virtual void onMonsterDie(MonsterActivity* pMonster, Player* pKiller);
	virtual void onPlayerRevive(Player* pPlayer);
	virtual void onPlantGather(Plant* pPlant, Player* player);
	virtual int32_t onBeginGather(Plant* plant, Player* player);
	virtual void addPlayer(Player* player);
	virtual void removePlayer(Player* player, bool islogout);
	virtual int32_t canEnter(Player* player, CActivityMap* pTargetMap) const;
	virtual void onTimeEnd();
	virtual Answer::NetPacket* packetActivityScore();
	virtual void broadcastReady();
	virtual void broadcastStart();
	virtual void broadcastActivityResult();
	virtual bool isCrossActivity();

protected:
	virtual void reset();

private:
	void addPlayerScore(Player* player, int32_t nPoint);
	void addKillReward(Player* player);
	void addRewards();
	void refreshRankIndex(Player* pExcept = NULL);
	void sendPlayerScore(Player* player);
	void SendPlayerRankInfo(Player* player);
	void broadcastContKill(Player* player, int32_t nLevel);
	void broadcastBreakContKill(Player* player, Player* pKiller, int32_t nLevel);
	void onWarEnd();
	int8_t selectCamp(Player* player);
	int32_t getContTitle(int32_t nContLevel);
	void notifyAddScore(Player* player, int32_t nScore, int32_t nReason);
	void RefreshBuff(int32_t nBuffId, int8_t nCampId);

private:
	typedef std::map<CharId_t, PlayerScore> PlayerScoreMap;
	typedef std::set<PlayerScore*, PlayerScoreGreater> ScoreRankSet;
	typedef std::map<int32_t, int32_t> BuffIdMap;

	PlayerScoreMap	m_mPlayerScore;
	ScoreRankSet	m_sScoreRank;
	BuffIdMap		m_BuffIdMap;
	int32_t			m_nLastTime;
	int32_t			m_Camp_1_battle;
	int32_t			m_Camp_2_battle;
};

#endif // __CAMP_WAR_H__
