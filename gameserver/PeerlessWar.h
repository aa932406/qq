#ifndef __PEERLESS_WAR_H__
#define __PEERLESS_WAR_H__

#include "stdafx.h"
#include "Activity.h"
#include "MonsterActivity.h"
#include <map>

class CPeerlessWar : public CActivity
{
public:
	struct PlayerScore
	{
		PlayerScore()
		{
			nCharId = 0;
			strName = "";
			nConnId = 0;
			nScore = 0;
			nKillCount = 0;
			nContKill = 0;
			nWinCount = 0;
			nLoseCount = 0;
			bInActivity = 0;
		}

		CharId_t	nCharId;
		std::string	strName;
		int32_t		nConnId;
		int32_t		nScore;
		int32_t		nKillCount;
		int32_t		nContKill;
		int32_t		nWinCount;
		int32_t		nLoseCount;
		int8_t		bInActivity;
	};

public:
	CPeerlessWar(const CfgActivity& cfgActivity);
	virtual ~CPeerlessWar();

public:
	virtual void Init();
	virtual void OnUpdate(CActivityMap* pMap);
	virtual void onPlayerKilled(Player* pDier, Player* pKiller);
	virtual void onPlayerRevive(Player* pPlayer);
	virtual void addPlayer(Player* player);
	virtual void removePlayer(Player* player, bool islogout);
	virtual Position GetRandBornPos(Player* player);
	virtual int32_t canEnter(Player* player, CActivityMap* pTargetMap) const;
	virtual void onTimeEnd();
	virtual Answer::NetPacket* packetActivityScore();
	virtual void broadcastReady();
	virtual void broadcastStart();

protected:
	virtual void reset();

private:
	void sendPlayerScore(Player* player);
	void sendActivityResult(Player* player);
	int32_t getLeftReviveTimes(Player* player);
	void win(Player* pWinner);
	void draw();
	void addWinnerReward(Player* player);
	void addJoinReward(Player* player);
	void addKillReward(Player* player);
	void addRewards();
	void saveWarResult();
	void clearOldTitle(Player* player);
	bool needBroadcastContKill();
	bool needBroadcastBreakCont();
	void addPlayerKillCount(Player* player);
	void broadcastKnockout(Player* pWinner, Player* pLoser);
	void broadcastWin(Player* pWinner);
	void broadcastConstKill(Player* player);
	void broadcastBreakConst(Player* player, Player* pKiller);
	void broadcastDraw();
	bool checkWin(Player* player);
	std::string GetWinnerName();
	void OnPeerlessWarResult(int32_t nWarState, CharId_t nWinnerId, const std::string& strWinnerName);

private:
	typedef std::map<CharId_t, PlayerScore> PlayerScoreMap;

	PlayerScoreMap	m_mPlayerScore;
	int64_t			m_nLastTick;
	std::string		m_strWinnerName;
	int32_t			m_nWarState;
};

#endif // __PEERLESS_WAR_H__
