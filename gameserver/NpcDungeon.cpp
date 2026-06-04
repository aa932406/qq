#include "stdafx.h"

#include "NpcDungeon.h"
#include "Dungeon.h"
#include "Player.h"

NpcDungeon::NpcDungeon()
	: m_dungeon(NULL)
{
}

NpcDungeon::~NpcDungeon()
{
}

void NpcDungeon::init(Dungeon *dungeon, const CfgNpc *cfgNpc)
{
	if ( dungeon == NULL )
	{
		return;
	}
	m_dungeon = dungeon;
	Npc::init(*cfgNpc, 0);
}

int32_t NpcDungeon::onEndNpc(Player *player, int8_t choice)
{
	if ( NULL == player )
	{
		return ERR_SYETEM_ERR;
	}
	m_dungeon->onNpcEnd(this, choice);
	return ERR_OK;
}
