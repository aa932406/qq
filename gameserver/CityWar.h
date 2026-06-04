#ifndef __CITY_WAR_H__
#define __CITY_WAR_H__

#include "stdafx.h"
#include "Activity.h"
#include "MonsterActivity.h"
#include <map>
#include <list>
#include <string>

class CCityWar : public CActivity
{
public:
	struct PlayerScore
	{
		CharId_t	nCharId;
		std::string	strName;
		std::string	strFamilyName;
		FamilyId_t	nFamilyId;
		int32_t		nScore;
		int32_t		nScoreTime;		// last score timestamp
		int32_t		nTime;			// join/re-enter time
		int32_t		nSeconds;		// accumulated online seconds
		int8_t		bInActivity;
		int8_t		nFamilyPosition;	// family position (1=elder, 2=member, 3=leader)
		std::list<PlayerScore*> lScoreRank;	// family-internal rank list (pointers)
		int8_t		bUpdateScroeRank;
	};

	struct FamilyScore
	{
		FamilyId_t	nFamilyId;
		std::string	strFamilyName;
		int32_t		nScore;
		int32_t		nScoreTime;
		int32_t		nFamilyCamp;	// camp (1=defender, 2=challenger, 3=other, 4=none)
		std::list<PlayerScore*> lScoreRank;	// family-internal rank list (pointers)
		int8_t		bUpdateScroeRank;
	};

	// Typedef for rank list used by appendPlayerRankInfo
	typedef std::list<PlayerScore*> PlayerScoreRank;

	struct PlayerScoreRankGreater
	{
		bool operator()(const PlayerScore* a, const PlayerScore* b) const
		{
			if (a->nScore != b->nScore)
				return a->nScore > b->nScore;
			return a->nCharId < b->nCharId;
		}
	};

	struct FamilyScoreRankGreater
	{
		bool operator()(const FamilyScore* a, const FamilyScore* b) const
		{
			if (a->nScore != b->nScore)
				return a->nScore > b->nScore;
			if (a->nFamilyCamp != b->nFamilyCamp)
				return a->nFamilyCamp > b->nFamilyCamp;
			return a->nFamilyId < b->nFamilyId;
		}
	};

public:
	CCityWar(const CfgActivity& cfgActivity);
	virtual ~CCityWar();

public:
	virtual void Init();
	virtual void OnUpdate(CActivityMap* pMap);
	virtual void SendPlayerActivityInfo(Player* player);
	virtual void SendPlayerActivityScore(Player* player, int32_t nLeftTime);
	virtual Position GetRandBornPos(Player* player);
	virtual int32_t GetRevive(Player* player);
	virtual void onMonsterDie(MonsterActivity* pMonster, Player* pKiller);
	virtual void onMonsterAdd(MonsterActivity* pMonster);
	virtual void onPlayerKilled(Player* pDier, Player* pAttacker);
	virtual void addPlayer(Player* player);
	virtual void removePlayer(Player* player, bool islogout);
	virtual int32_t canEnter(Player* player, CActivityMap* pTargetMap) const;
	virtual void onActivityStart();
	virtual void onTimeEnd();
	virtual int32_t getNextStartTime();
	virtual Answer::NetPacket* packetActivityScore();
	virtual Answer::NetPacket* packetActivityScoreForPlayer(int32_t nConnId);
	virtual void broadcastReady();
	virtual void broadcastStart();

	void LoadApplyInfo();

	// CityWar specific (public)
	void OnCityWarResult(int32_t nIndex, FamilyId_t nFamilyId, int32_t nWinTime,
		CharId_t nLeader, CharId_t nFirst, CharId_t nSecond, CharId_t nThird,
		const std::string& strFirstFamily, const std::string& strSecondFamily,
		const std::string& strThirdFamily);
	void SendAppyCityWarInfo(Player* player);
	void ApplyCityWar(Player* player, FamilyId_t nFamilyId, int32_t nType);
	void AutoApplyCityWar();
	void GongGao(FamilyId_t nFamilyId, int32_t nGongGaoId);
	void SaveApplyInfo();
	bool OnChangeMap(Player* player, CActivityMap* pMap, int32_t nX, int32_t nY, int32_t nParam);
	void appendPlayerRankInfo(Answer::NetPacket* packet, const PlayerScoreRank* rank);

	// v3 virtual overrides
	virtual bool checkData();
	virtual bool checkWeek();

protected:
	virtual void reset();

private:
	bool isSpecialDay(int32_t nStartDays);
	bool isInSpecialDay(int32_t nStartDays);
	void addPlayerScore(Player* player, int32_t nScore);
	void setUpdateScoreRank(FamilyId_t nFamilyId, bool bScoreRank);
	void addFamilyScore(FamilyId_t nFamilyId, int32_t nScore, int32_t nNowTime);
	void getFamilyMembers(FamilyId_t nFamilyId, CharIdList* cids);
	int8_t GetFamilyCamp(FamilyId_t nFamilyId);
	bool HaveOwner();
	void SetOwner();
	FamilyId_t GetGuilDerFirstFamily();
	void checkChangeFamily(bool IsUpdateScore);
	bool canGetOnlineReward(const PlayerScore* score);
	void NotOccupyGongGao();
	void win();
	void addRewards();
	void saveResult();

private:
	typedef std::map<CharId_t, PlayerScore> PlayerScoreMap;
	typedef std::map<FamilyId_t, FamilyScore> FamilyScoreMap;
	typedef std::list<FamilyScore*> FamilyScoreList;
	typedef std::map<FamilyId_t, int32_t> ApplyMap;

	PlayerScoreMap		m_mPlayerScore;
	FamilyScoreMap		m_mFamilyScore;
	FamilyScoreList		m_lFamilyScore;
	ApplyMap			m_Apply;

	MonsterActivity*	m_pMonster;
	int8_t				m_bUpdateFamilyScroe;
	int64_t				m_nLastUpdateRankTick;
	FamilyId_t			m_nFamilyId;
	FamilyId_t			m_nLastFamilyId;
	int32_t				m_nWinTime;
	int32_t				m_nIndex;

	std::string			m_FirstFamilyName;
	std::string			m_SecondFamilyName;
	std::string			m_ThirdFamilyName;
	CharId_t			m_Leader;
	CharId_t			m_First;
	CharId_t			m_Second;
	CharId_t			m_Third;
};

#endif // __CITY_WAR_H__
