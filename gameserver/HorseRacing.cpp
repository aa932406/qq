#include "stdafx.h"
#include "HorseRacing.h"
#include "DBService.h"
#include "GameService.h"
#include "ActivityMap.h"
#include "CfgData.h"

CHorseRacing::CHorseRacing(const CfgActivity& cfgActivity)
	: CActivity(cfgActivity)
{
	m_nIndex = 0;
	m_BossDie = 0;
}

CHorseRacing::~CHorseRacing()
{
}

void CHorseRacing::reset()
{
	CActivity::reset();
	m_rankList.clear();
	m_racing.clear();
	m_winner.clear();
	m_nIndex = 0;
	m_BossDie = 0;
}

void CHorseRacing::OnUpdate(CActivityMap* pMap)
{
	CActivity::OnUpdate(pMap);
	if (pMap == NULL || pMap->GetId() != 10003)
		return;

	PlayerList::iterator iter = m_racing.begin();
	while (iter != m_racing.end())
	{
		Player* player = *iter;
		if (player == NULL)
		{
			iter = m_racing.erase(iter);
			continue;
		}

		if (player->getMapId() == pMap->GetId())
		{
			Position pos(20, 57);
			Position curPos = player->getCurrentTile();
			if (curPos.tileDistance(pos) <= 6)
			{
				m_winner.push_back(player);
				iter = m_racing.erase(iter);
				win(player);
				continue;
			}
		}
		++iter;
	}
}

void CHorseRacing::addPlayer(Player* player)
{
	CActivity::addPlayer(player);
	if (player)
	{
		m_racing.push_back(player);
		if (player->getMapId() != 90017)
			addActivityBuff(player, 136, false);
	}
}

void CHorseRacing::removePlayer(Player* player, bool islogout)
{
	CActivity::removePlayer(player, islogout);

	for (PlayerList::iterator iter = m_racing.begin(); iter != m_racing.end(); ++iter)
	{
		if (*iter == player)
		{
			m_racing.erase(iter);
			break;
		}
	}

	for (PlayerList::iterator iter = m_winner.begin(); iter != m_winner.end(); ++iter)
	{
		if (*iter == player)
		{
			m_winner.erase(iter);
			break;
		}
	}

	if (player)
		player->removeBuff(136);
}

int32_t CHorseRacing::canEnter(Player* player, CActivityMap* pTargetMap) const
{
	if (player == NULL || pTargetMap == NULL)
		return 10002;

	if (pTargetMap->GetId() == 90017)
	{
		if (!bArrived(player->getCid()))
			return 10002;
	}
	else
	{
		if (bArrived(player->getCid()))
		{
			player->TiShiInfo(4, 0);
			return 10002;
		}
	}
	return CActivity::canEnter(player, pTargetMap);
}

bool CHorseRacing::bArrived(CharId_t cid) const
{
	for (PlayerRankList::const_iterator iter = m_rankList.begin(); iter != m_rankList.end(); ++iter)
	{
		if (iter->nCharId == cid)
			return true;
	}
	return false;
}

void CHorseRacing::win(Player* player)
{
	if (player == NULL)
		return;

	++m_nIndex;
	addReward(player);
	sendActivityResult(player);
	broadcastWin(player);

	PlayerRank rank;
	rank.nCharId = player->getCid();
	rank.strName = player->getName();
	m_rankList.push_back(rank);

	if (m_nIndex <= 3)
		setNeedBroadcastActivityScore();

	LogActivity logActivity = {};
	logActivity.cid = player->getCid();
	logActivity.actid = 0;
	logActivity.acttype = GetType();
	logActivity.time = player->getNow();
	DB_SERVICE.InsertActivityLog(logActivity);
}

void CHorseRacing::addReward(Player* player)
{
	if (player == NULL)
		return;

	const CfgHorseRacingRewardTable& rewardTable = CFG_DATA.GetHorseRacingRewardTable();
	const CfgHorseRacingReward* pReward = rewardTable.GetReward(m_nIndex);
	if (pReward == NULL)
		return;

	if (pReward->nExp > 0)
		player->addExp(pReward->nExp, true);
	if (pReward->nMoney > 0)
		player->AddCurrency(CURRENCY_MONEY, pReward->nMoney, MCR_HORSE_RACING_REWARD);

	if (!pReward->Items.empty())
	{
		int32_t nMailIndex = 0;
		switch (m_nIndex)
		{
		case 1:  nMailIndex = 6212; break;
		case 2:  nMailIndex = 6213; break;
		case 3:  nMailIndex = 6214; break;
		case 4:  nMailIndex = 6240; break;
		case 5:  nMailIndex = 6241; break;
		case 6:  nMailIndex = 6242; break;
		case 7:  nMailIndex = 6243; break;
		case 8:  nMailIndex = 6244; break;
		case 9:  nMailIndex = 6245; break;
		case 10: nMailIndex = 6246; break;
		default: nMailIndex = 6247; break;
		}
		DB_SERVICE.OnSendSysMail(player->getCid(), nMailIndex, pReward->Items);
	}
}

void CHorseRacing::sendActivityResult(Player* player)
{
	if (player == NULL)
		return;

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2E27);
	if (packet == NULL)
		return;

	packet->writeInt32(m_cfgActivity.id);
	packet->writeInt32(m_nIndex);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(player->getGateIndex(), packet);
}

void CHorseRacing::broadcastWin(Player* player)
{
	if (player == NULL)
		return;

	int32_t nBroadCastId = 0;
	switch (m_nIndex)
	{
	case 1:  nBroadCastId = 13; break;
	case 2:  nBroadCastId = 14; break;
	case 3:  nBroadCastId = 15; break;
	case 4:  nBroadCastId = 16; break;
	case 5:  nBroadCastId = 17; break;
	default: nBroadCastId = 18; break;
	}

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2CD6);
	if (packet == NULL)
		return;

	packet->writeInt32(nBroadCastId);
	packet->writeUTF8(player->getName());
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CHorseRacing::broadcastReady()
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2CD6);
	if (packet == NULL)
		return;

	packet->writeInt32(19);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CHorseRacing::broadcastStart()
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2CD6);
	if (packet == NULL)
		return;

	packet->writeInt32(20);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CHorseRacing::onTimeEnd()
{
	m_nState = AS_NOT_START;
	delayKickAll(0);
}

bool CHorseRacing::CanUsePet(MapId_t mid) const
{
	return mid == 90017;
}

void CHorseRacing::onMonsterDie(MonsterActivity* pMonster, Player* pKiller)
{
	if (pMonster && pMonster->getMid() == 476078)
	{
		m_BossDie = 1;
		setNeedBroadcastActivityScore();
	}
}

Answer::NetPacket* CHorseRacing::packetActivityScore(int8_t connid)
{
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, 0x2E26);
	if (packet == NULL)
		return NULL;

	packet->writeInt32(m_cfgActivity.id);
	packet->writeInt32(getLeftTime());

	uint32_t offset = packet->getWOffset();
	packet->writeInt16(0);
	int16_t nCount = 0;
	for (PlayerRankList::iterator iter = m_rankList.begin(); iter != m_rankList.end(); ++iter)
	{
		packet->writeUTF8(iter->strName);
		if (++nCount > 2)
			break;
	}

	packet->writeInt8(m_BossDie ? 2 : 1);

	uint32_t oldOffset = packet->getWOffset();
	packet->setWOffset(offset);
	packet->writeInt16(nCount);
	packet->setWOffset(oldOffset);
	packet->setSize(packet->getWOffset());

	return packet;
}
