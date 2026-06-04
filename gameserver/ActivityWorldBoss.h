#ifndef __ACTIVITY_WORLD_BOSS_H__
#define __ACTIVITY_WORLD_BOSS_H__

#include "stdafx.h"
#include "Activity.h"
#include "MonsterActivity.h"
#include <map>
#include <set>

class CActivityWorldBoss : public CActivity
{
public:
	struct PlayerScore
	{
		PlayerScore()
		{
			nCharId = 0;
			strName = "";
			nConnId = 0;
			nDamage = 0;
			nIndex = 0;
			bInActivity = 0;
			nStartTick = 0;
		}

		CharId_t	nCharId;
		std::string	strName;
		int32_t		nConnId;
		int32_t		nDamage;
		int32_t		nIndex;
		int8_t		bInActivity;
		int64_t		nStartTick;
	};

	struct PlayerScoreRankGreater
	{
		bool operator()(const PlayerScore* a, const PlayerScore* b) const
		{
			if (a->nDamage != b->nDamage)
				return a->nDamage > b->nDamage;
			return a->nCharId < b->nCharId;
		}
	};

public:
	CActivityWorldBoss(const CfgActivity& cfgActivity);
	virtual ~CActivityWorldBoss();

public:
	virtual void OnUpdate(CActivityMap* pMap);
	virtual void onMonsterDamaged(MonsterActivity* pMonster, int32_t nDamage, Player* pAttacker);
	virtual void onPlayerKilled(Player* pDier, Player* pKiller);
	virtual void onMonsterDie(MonsterActivity* pMonster);
	virtual void onMonsterAdd(MonsterActivity* pMonster);
	virtual void addPlayer(Player* player);
	virtual void removePlayer(Player* player, bool islogout);
	virtual bool OnSitRevive(Player* player);
	virtual int32_t GetRevive(Player* player);
	virtual int32_t canEnter(Player* player, CActivityMap* pTargetMap) const;
	virtual void onTimeEnd();
	virtual Answer::NetPacket* packetActivityScore();
	virtual void broadcastStart();
	virtual void broadcastActivityResult();
	bool shouldBroadcastScore(CActivityMap* pMap);

protected:
	virtual void reset();

private:
	void addPlayerScore(Player* player, int32_t nDamage);
	void addRewards();
	void sendPlayerScore(Player* player);
	void refreshRank();
	void broadcastBossKilled(const std::string& strKillerName, CharId_t nKillerId);
	void broadcastEnd();
	int32_t getAutoReviveTime(Player* player);
	const PlayerScore* getRankFirst();

private:
	typedef std::map<CharId_t, PlayerScore> PlayerScoreMap;
	typedef std::multiset<PlayerScore*, PlayerScoreRankGreater> ScoreRankSet;
	typedef std::map<CharId_t, int32_t> ReviveTimesMap;

	PlayerScoreMap	m_mPlayerScore;
	ScoreRankSet	m_sScoreRank;
	ReviveTimesMap	m_ReviveTimes;
	int8_t			m_IsBossDie;
};

#endif // __ACTIVITY_WORLD_BOSS_H__
