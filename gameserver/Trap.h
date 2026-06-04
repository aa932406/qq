#pragma once

#include "CfgData.h"
#include "Entity.h"

enum TrapState
{
	TS_STANDBY,
	TS_EFFECT,
	TS_COOLDOWN,
	TS_DIE,
};

enum TrapType
{
	TT_BUFF				= 1,	// BUFF
	TT_MONSTER			= 2,	// ˢ��
	TT_TRANSFER			= 3,	// ����
	TT_TRANSFER_TEAM	= 4,	// ���鴫��
	TT_DELAY_MONSTER	= 5,	// ��ʱˢ��
	TT_CHANGE_DUNGEON	= 6,	// �л�����
	TT_ADD_DUNGEON_TIME	= 7,	// �ӳ�����ʱ��
};

class Map;
class Dungeon;
class CActivityMap;
/*
* ����
*/
class Trap
	: public Entity
{
public:
	Trap();
	virtual ~Trap();

public:
	void init( Map *pMap, Position pos, const CfgTrap &cfgTrap);
	void reset();

	void checkState();

	int32_t onUse(Player *player);

	bool appendInfo(Answer::NetPacket *inPacket);
	void leaveMap();

private:
	void broadcastState();
	void broadcastLeaveMap();

	void setState(TrapState state);
	void effect();

private:
	Map*		m_pMap;
	CfgTrap		m_cfgTrap;
	Position	m_pos;
	CharId_t	m_user;
	TrapState	m_state;
	int64_t		m_stateTick;
	int64_t		m_liftTime;		// ��������(ms)
};


