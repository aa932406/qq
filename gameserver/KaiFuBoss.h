#ifndef __KAI_FU_BOSS_H__
#define __KAI_FU_BOSS_H__

#include "stdafx.h"
#include "Activity.h"
#include "MonsterActivity.h"

class CKaiFuBoss : public CActivity
{
public:
	CKaiFuBoss(const CfgActivity& cfgActivity);
	virtual ~CKaiFuBoss();

public:
	virtual void onMonsterAdd(MonsterActivity* pMonster);
	virtual void broadcastReady();
	virtual void broadcastStart();
	virtual void onTimeEnd();

protected:
	virtual void reset();
	virtual bool checkData();
	virtual bool checkWeek();
	virtual int32_t getNextStartTime();

private:
	bool isSpecialDay(int32_t nStartDays);
	bool isInSpecialDay(int32_t nStartDays);
	void sendPlayerScore(Player* player);
	void broadcastTimeEnd();

private:
	MonsterActivity*	m_Boss;
};

#endif // __KAI_FU_BOSS_H__
