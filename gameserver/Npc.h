#pragma once

#include "CfgData.h"
#include "Unit.h"

class Player;

class Npc
	: public Unit
{
public:
	Npc();
	virtual ~Npc();

public:
	virtual void refresh();
	virtual void reset();
	virtual EntityId_t getUnitId() const;
	virtual CharId_t getOwner() const;
	virtual int32_t getLevel() const;
	virtual int32_t getBattle() const;
	virtual void postDamage(int32_t damge, UnitHandle launcher);

	virtual bool isLifeEnd();
	virtual void onTalkByPlayer(Player *player);

public:
	void init(const CfgNpc &cfgNpc, int32_t EndTime = 0 );
	bool appendInfo(Answer::NetPacket *packet);
	void leaveMap();

	void SetOwner( CharId_t nOwner );
	int32_t	GetFunc() const;
	int32_t GetFuncExtra() const;
	Position GetStallPos() const;

protected:
	CfgNpc m_cfgNpc;
	CharId_t m_nOwner;
	int32_t m_endTime;
};

