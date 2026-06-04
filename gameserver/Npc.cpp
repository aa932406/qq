#include "stdafx.h"

#include "Npc.h"
#include "Player.h"

Npc::Npc()
	: Unit(ET_NPC)
{
	reset();
}

Npc::~Npc()
{

}

void Npc::refresh()
{

}

void Npc::reset()
{
	Unit::reset();
	m_nOwner = 0;
}

EntityId_t Npc::getUnitId() const
{
	return getEntityId();
}

CharId_t Npc::getOwner() const
{
	return m_nOwner;
}

int32_t Npc::getLevel() const
{
	return m_cfgNpc.level;
}

int32_t Npc::getBattle() const
{
	return 0;
}

void Npc::postDamage(int32_t damge, UnitHandle launcher)
{

}

bool Npc::isLifeEnd()
{
	if ( m_endTime <= 0 )
	{
		return false;
	}
	return getNow() >= m_endTime;
}

void Npc::onTalkByPlayer(Player *player)
{
	if (NULL == player)
	{
		return;
	}
	player->taskTalkWithNpc(m_cfgNpc.id);
//	player->taskKingdomTalkWithNpc(m_cfgNpc.id);
}

void Npc::init(const CfgNpc &cfgNpc, int32_t EndTime )
{
	m_cfgNpc	= cfgNpc;
	m_endTime	= EndTime;
}

bool Npc::appendInfo(Answer::NetPacket *packet)
{
	if ( NULL == packet )
	{
		return false;
	}
	packet->writeInt8( getType() );
	packet->writeInt64( getUnitId() );
	packet->writeInt32( m_cfgNpc.id );
	packet->writeInt32( GetPosX() );
	packet->writeInt32( GetPosY() );
	packet->writeInt32( 500000 );//TODO: use NPC real HP
	packet->writeInt32( 500000 );
	packet->writeInt32( 0 );
	packet->writeInt64( getOwner() );
	return true;
}

void Npc::leaveMap()
{
	broadcastLeave();
}

void Npc::SetOwner( CharId_t nOwner )
{
	m_nOwner = nOwner;
}

int32_t Npc::GetFunc() const
{
	return m_cfgNpc.func;
}

int32_t Npc::GetFuncExtra() const
{
	return m_cfgNpc.func_extra;
}

Position Npc::GetStallPos() const
{
	Position pos( -1, -1 );
	if ( m_cfgNpc.func_extra != NFE_STALL )
	{
		return pos;
	}

	if ( m_cfgNpc.params.size() == 2 )
	{
		pos.x = m_cfgNpc.params[0];
		pos.y = m_cfgNpc.params[1];
	}
	return pos;
}
