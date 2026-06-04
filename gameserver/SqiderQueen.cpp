#include "stdafx.h"
#include "SqiderQueen.h"
#include "GameService.h"
#include "ActivityMap.h"
using namespace Answer;

CSqiderQueen::CSqiderQueen(const CfgActivity& cfgActivity)
	: CActivity(cfgActivity)
{
}

CSqiderQueen::~CSqiderQueen()
{
}

void CSqiderQueen::broadcastStart()
{
	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CD6);
	if (packet == NULL)
		return;
	packet->writeInt32(35);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CSqiderQueen::broadcastTimeEnd()
{
	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CD6);
	if (packet == NULL)
		return;
	packet->writeInt32(37);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CSqiderQueen::StopActivityBySqiderQueenDie()
{
	broadcastKillAllSqiderQueen();
	stopActivity();
}

void CSqiderQueen::broadcastKillAllSqiderQueen()
{
	NetPacket* packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, 0x2CD6);
	if (packet == NULL)
		return;
	packet->writeInt32(36);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}
