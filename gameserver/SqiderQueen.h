#ifndef __SQIDER_QUEEN_H__
#define __SQIDER_QUEEN_H__

#include "stdafx.h"
#include "Activity.h"

class CSqiderQueen : public CActivity
{
public:
	CSqiderQueen(const CfgActivity& cfgActivity);
	virtual ~CSqiderQueen();

public:
	virtual void broadcastStart();
	virtual void broadcastTimeEnd();

private:
	void StopActivityBySqiderQueenDie();
	void broadcastKillAllSqiderQueen();
};

#endif // __SQIDER_QUEEN_H__
