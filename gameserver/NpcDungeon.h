#pragma once

#include "Npc.h"

class Dungeon;

class NpcDungeon
	: public Npc
{
public:
	NpcDungeon();
	virtual ~NpcDungeon();

public:
	void	init(Dungeon *dungeon, const CfgNpc *cfgNpc);
	int32_t	onEndNpc(Player *player, int8_t choice);

protected:
	Dungeon*	m_dungeon;
};

