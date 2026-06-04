#ifndef __FAMILY_WAR_H__
#define __FAMILY_WAR_H__

#include "stdafx.h"
#include "Activity.h"
#include "MonsterActivity.h"

struct FamilyScore 
{
	FamilyId_t	nFamilyId;
	std::string	strFamilyName;
	int32_t		nScore;

	bool operator>( const FamilyScore& rhs ) const
	{
		return nScore > rhs.nScore;
	}
};
typedef std::list<FamilyScore> FamilyScoreList;

struct PlayerScore 
{
	CharId_t	nCharId;
	std::string	strName;
	FamilyId_t	nFamilyId;
	int32_t		nScore;
	int32_t		nKillCount;
	int32_t		nExp;
	int32_t		nTaskId;
	int64_t		nStartTick;
	int8_t		bInActivity;

	bool operator>( const PlayerScore& rhs ) const
	{
		if (nScore != rhs.nScore)
			return nScore > rhs.nScore;
		return nCharId < rhs.nCharId;
	}
};
typedef std::list<PlayerScore> PlayerScoreList;

class CFamilyWar : public CActivity
{
public:
	CFamilyWar(const CfgActivity& cfgActivity);
	virtual ~CFamilyWar();

public:
	virtual void OnUpdate(CActivityMap* pMap);
	virtual void onMonsterDie(MonsterActivity* pMonster, Player* pKiller);
	virtual void onPlayerKilled(Player* pDier, Player* pAttacker);
	virtual void onMonsterAdd(MonsterActivity* pMonster);
	virtual void addPlayer(Player* player);
	virtual void removePlayer(Player* player, bool islogout);
	virtual int32_t canEnter(Player* player, CActivityMap* pTargetMap) const;
	virtual void onTimeEnd();
	virtual Answer::NetPacket* packetActivityScore(int8_t connid = 0);
	virtual void broadcastReady();
	virtual void broadcastStart();

protected:
	virtual void reset();

private:
	PlayerScore* getPlayerScore(CharId_t nCharId);
	void addPlayerScore(Player* player, int32_t nScore, int32_t nKillCount);
	void addFamilyScore(FamilyId_t nFamilyId, const std::string& strFamilyName, int32_t nScore);
	void win(FamilyId_t nFamilyId, const std::string& strFamilyName);
	void sendPlayerScoreRankReward();
	void sendFamilyScoreRankReward();
	void sendWinnerReward(FamilyId_t nFamilyId);
	void addBuff(Player* player);
	void removeBuff(Player* player);
	void broadcastWin(FamilyId_t nFamilyId, const std::string& strFamilyName);

public:
	FamilyId_t		GetWinFamily();
	std::string		GetFamilyName();
	std::string		GetFamilyLeaderName();
	void			OnFamilyWarResult(FamilyId_t nFamilyId, int16_t nTimes, std::string FamilyName, std::string LeadyerName);

private:
	MonsterActivity*	m_pTitle;			// 称号怪物
	MonsterActivity*	m_pBoss;			// Boss怪物
	Player*				m_pBuffPlayer;		// 持有Buff的玩家
	int64_t				m_nBuffStartTick;	// Buff开始时间
	FamilyId_t			m_nWinFamily;		// 获胜家族
	std::string			m_strWingFamily;	// 获胜家族名称
	int8_t				m_nActiveState;		// 活动状态
	int8_t				m_nActivePillarState;// 柱子状态

	FamilyScoreList		m_lstFamilyScore;	// 家族分数列表
	PlayerScoreList		m_lstPlayerScore;	// 玩家分数列表
};

#endif // __FAMILY_WAR_H__
