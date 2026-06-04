#ifndef __HORSE_RACING_H__
#define __HORSE_RACING_H__

#include "stdafx.h"
#include "Activity.h"
#include "MonsterActivity.h"

struct PlayerRank
{
	CharId_t	nCharId;
	std::string	strName;
};
typedef std::list<PlayerRank> PlayerRankList;

class CHorseRacing : public CActivity
{
public:
	CHorseRacing(const CfgActivity& cfgActivity);
	virtual ~CHorseRacing();

public:
	virtual void OnUpdate(CActivityMap* pMap);
	virtual bool CanUsePet(MapId_t mid) const;
	virtual int32_t canEnter(Player* player, CActivityMap* pTargetMap) const;
	virtual void onTimeEnd();
	virtual Answer::NetPacket* packetActivityScore(int8_t connid = 0);
	virtual void broadcastReady();
	virtual void broadcastStart();

protected:
	virtual void reset();
	virtual void onMonsterDie(MonsterActivity* pMonster, Player* pKiller);
	virtual void addPlayer(Player* player);
	virtual void removePlayer(Player* player, bool islogout);

private:
	void win(Player* player);
	void addReward(Player* player);
	void broadcastWin(Player* player);
	void sendActivityResult(Player* player);
	bool bArrived(CharId_t cid) const;

private:
	int32_t			m_nIndex;			// 排名序号
	PlayerRankList	m_rankList;			// 胜者列表
	PlayerList		m_racing;			// 参赛选手
	PlayerList		m_winner;			// 本轮胜者
	int8_t			m_BossDie;			// Boss是否被击杀
};

#endif // __HORSE_RACING_H__
