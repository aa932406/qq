#include "stdafx.h"

#include "ActivityManager.h"
#include "CfgData.h"
#include "DBService.h"
#include "GameService.h"
//#include "Kingdom.h"
#include "Map.h"
#include "MapManager.h"
#include "Monster.h"
#include "MonsterActivity.h"
#include "NpcActivity.h"
#include "PlantActivity.h"
#include "Player.h"
#include "PlayerRobot.h"
#include "Tile.h"
#include "Trailer.h"
#include "EquipManager.h"
#include "PoolManager.h"
#include "PetManager.h"
#include "FamilyManager.h"
#include "Timer.h"
#include "DaTiHD.h"
#include "KaiFuHuoDong.h"
#include "GMBackstage.h"
#include <string>
using namespace Answer;
using namespace std;
GameService::GameService()
	: TcpService(CLIENT_RECV_BUFFER, CLIENT_RECV_BUFFER, 2048-NET_PACKET_HEAD_LEN, 0, 2000, "GameService"), m_line(0)
{
	bzero(m_users, sizeof(m_users));
	bzero(m_onLineBroadcast,sizeof(m_onLineBroadcast));
}

GameService::~GameService()
{

}

void GameService::onNetPacket(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return;
	}
	uint16_t proc = inPacket->getProc();
	if (proc > IM_MIN && proc < IM_MAX)
	{
		if (proc > IM_SOCIAL_GAME_MIN && proc < IM_SOCIAL_GAME_MAX)
		{
			//�����Զ���������//�ڲ�
			onSocialNetpacket(inPacket);
		}
// 		else if (proc == CM_GATE_PHP_PROP)
// 		{
// 			onBuyGameShopItem(inPacket);
// 		}
		else
		{
			switch (proc)
			{
			case IM_GATE_ADD_USER:							onAddUser(inPacket); break;
			case IM_GATE_REMOVE_USER:						onRemoveUser(inPacket); break;
			case IM_GATE_UPDATE_BAN_CHAT:					onUpdateBanChat(inPacket); break;
			case IM_GATE_RELOAD_ITEM:						onGMReloadItem(inPacket); break;
			case IM_GATE_QQ_TO_PLAYER:						qqToPlayer(inPacket);break;
			case IM_SOCIAL_ALL_GAME_UPDATE_PET_RANK:		onSocialUpdatePetRank( inPacket );		break;
			case IM_SOCIAL_ALL_GAME_UPDATE_FAMILY:			onSocialUpdateFamilyInfo( inPacket );	break;
			case IM_SOCIAL_SINGLE_GAME_INIT_PET_RANK:		onSocialInitPetRank( inPacket );		break;
			case IM_SOCIAL_SINGLE_GAME_TEAM_DUNGEON_CREATE:	onSocialCreateTeamDungeon( inPacket );	break;
			case IM_SOCIAL_SINGLE_GAME_DA_TI_RANK:			OnReceiveDaTiRank(inPacket);			break;
			case IM_SOCIAL_SINGLE_GAME_SEND_SYSTEM_MAIL:	onSocialSendSystemMail(inPacket);		break;
			case IM_SOCIAL_SINGLE_GAME_SEND_SYSTEM_MAIL2:	onSocialSendSystemMail2(inPacket);		break;
			case IM_SOCIAL_ALL_GAME_CHANGE_OWNER:			onSocialChangeOwner(inPacket);			break;
			case IM_SOCIAL_ALL_GAME_KAI_FU_HUO_DAO_RANK:	KAI_FU_HUO_DONG.RecvRank( inPacket );	break;
			default: break;
			}
		}
	}
	else
	{
		int16_t cgindex = static_cast<int16_t>(inPacket->readInt32());
		if (cgindex > 0 && cgindex > 0 && cgindex < MAX_CONNECTION)
		{
			switch (proc)
			{
			case CM_ENTER_GAME: onEnterGame(cgindex, inPacket); break;
			case CM_ENTER_GAME_ROBOT: onEnterGameRobot(cgindex, inPacket); break;
			//��������Ӵ��� //flex�ͻ��˹���������
			default: onGameNetpacket(cgindex, inPacket); break;
			}
		}
	}

	inPacket->destroy();
}
//���ص�ͼ�ϵ����� ��ȡ˫�������� ������·
void GameService::startGame(int32_t line)
{
	m_line = line;
	//���������Ӧ����
	Player::initNetPacketHandlers();

	// ��ͼ��ʼ��
	MAP_MANAGER.Init();

	// ��ʼ����ͼ����Ϣ
	TILE_MANAGER.Init();

	// ������ͼ�߳�
	MAP_MANAGER.StartAll();

	// ��ʼ���
	ACTIVITY_MANAGER.Init();

	// ��ʼ��GM��̨
	GM_BACKSTAGE.Init();

	// ����������а���Ϣ
	requestPetRankInfo();
	SendServerDiffToGlobal();
	requestFamilyInfo();
	PET_MANAGER.UpdataChangedPet();
}

void GameService::stopGame()
{
	// ֹͣ��ͼ�߳�
	MAP_MANAGER.StopAll();

	saveAllPlayerToDB();
}

int32_t GameService::getLine()
{
	return m_line;
}
void GameService::onNewMinuteCome(int32_t minute)
{
	if (m_line == GAME_SERVICE_LINE_SOCIAL)
	{
		DB_SERVICE.onNewMinuteCome();		//���µ�DBServer

		NetPacket *packet = popNetpacket(PACK_PROC, IM_SOCIAL_NEW_MINUTE_COME);
		if (NULL == packet)
		{
			return;
		}
		packet->writeInt32(minute);
		packet->setSize(packet->getWOffset());
		sendPacket(packet);
	}
}

//Kingdom* GameService::getKingdom(int32_t id)
//{
//	for (std::vector<Kingdom*>::iterator it = m_kingdoms.begin(); it != m_kingdoms.end(); ++it)
//	{
//		if ((*it)->getId() == id)
//		{
//			return *it;
//		}
//	}
//	return NULL;
//}

void GameService::onPlayerLoaded( PlayerDBData& dbData, bool isRobot )
{
	// ��ʱ��ɫ��δ���ӽ���ͼ�У����������̴߳���,����Ҫ������·���
	Player *player = getPlayer( dbData.chr.data.cid, 0, false );
	if (player != NULL)
	{
		Map *pMap = player->getMap();
		int32_t mapid = 0;
		if (pMap != NULL)
		{
			mapid = pMap->GetId();
		}

		LOG_INFO("GameService::onPlayerLoaded player cid = %lld, map = %p, mapid = %d already in game\n", dbData.chr.data.cid, pMap, mapid);

		if (pMap != NULL)
		{
			return;
		}
	}

	User *pUser = m_users[dbData.cgindex];
	if (pUser != NULL && pUser->getPlayer() == NULL)
	{
		Map	*pMap = NULL;
		pMap = MAP_MANAGER.GetMap( dbData.chr.data.mapid );
		if (pMap == NULL)
		{
			// ��ͼ����
			//LOG_INFO("GameService::onPlayerLoaded player cid=%d can't find map with kingdom = %d, mapid = %d, change kingdom\n", dbData.chr.data.cid, dbData.chr.data.in_kingdom, dbData.chr.data.mapid);

			//CfgMap *pCfgMap = CFG_DATA.getMap(dbData.chr.data.mapid);
			//if (pCfgMap != NULL && pCfgMap->type == MT_KINGDOM_MUTEX)
			//{
			//	Map	*pTempMap = MAP_MANAGER.GetMap(pCfgMap->param, dbData.chr.data.mapid);
			//	if (pTempMap != NULL)
			//	{
			//		pMap = pTempMap;
			//		dbData.chr.data.in_kingdom = pCfgMap->param;
			//	}
			//}
			//else
			//{
				CfgMapRegion *pCfgRegion = CFG_DATA.getMapRegion(PBA_REGION);
				if (pCfgRegion != NULL)
				{
					Map *pTempMap = MAP_MANAGER.GetMap( pCfgRegion->mapid );
					if (pTempMap != NULL)
					{
						Position pos = pTempMap->getRandomWalkablePositionInRegion(*pCfgRegion);
						if (pos.x >= 0 && pos.y >= 0)
						{
							pMap = pTempMap;
							dbData.chr.data.mapid = pMap->GetId();
							dbData.chr.data.x = pos.x;
							dbData.chr.data.y = pos.y;
						}
					}
				}
			//}
		}

		if (pMap != NULL)
		{
			//Kingdom *pKingdom = pMap->getKingdom();
			//if (pKingdom != NULL)
			//{
				Player *player = NULL;
				if (isRobot)
				{
					player = new PlayerRobot;
					player->reset();
				}
				else
				{
					player = POOL_MANAGER.pop<Player>();
				}
				if (player != NULL)
				{
					LOG_DEBUG("GameService::onPlayerLoaded pop player %p\n", player);

					player->init( dbData );

					std::string openId = pUser->getOpenId();
					std::string openKey = pUser->getOpenKey();
					std::string pf = pUser->getPf();
					std::string pfKey = pUser->getPfKey();
					if (!openId.empty() && !openKey.empty() && !pf.empty() && !pfKey.empty())
					{
						player->setOpenId(openId);
						player->setOpenKey(openKey);
						player->setPf(pf);
						player->setPfKey(pfKey);
					}

					player->setIsYellowVip(pUser->getIsYellowVip());
					player->setIsYellowYearVip(pUser->getIsYellowYearVip());
					player->setYellowVipLevel(pUser->getYellowVipLevel());
					player->setIsYellowHighVip(pUser->getIsYellowHighVip());

					MAP_MANAGER.PostMsg( pMap->GetRunnerId(), GMC_PLAYER_ENTER_MAP, player, pMap, dbData.chr.data.x, dbData.chr.data.y);
					pUser->setPlayer(player);
					AddPlayer(player);		

				
				}
			//}
			//else
			//{
			//	LOG_INFO("GameService::onPlayerLoaded player cid=%d can't find kingdom with kingdom = %d, mapid = %d\n", dbData.chr.data.cid, dbData.chr.data.in_kingdom, dbData.chr.data.mapid);
			//}
		}
		else
		{
			LOG_INFO("GameService::onPlayerLoaded player cid=%lld can't find map with mapid = %d\n", dbData.chr.data.cid, dbData.chr.data.mapid);
		}
	}
}

void GameService::onPlayerLogout(Player *player, int16_t cgindex)
{
	if (player != NULL)
	{
		LOG_DEBUG("GameService::onPlayerLogout push player %p, cgindex = %d\n", player, static_cast<int32_t>(cgindex));
		removePlayer(player);
		if (player->getGateIndex() > 0)
		{
			if (player->isRobot())
			{
				delete player;
			}
			else
			{
				POOL_MANAGER.push<Player>( player );
			}
		}
	}

	if (cgindex >= 0 && cgindex < MAX_CONNECTION)
	{
		User *user = m_users[cgindex];
		if (user != NULL)
		{
			user->setPlayer(NULL);
		}
	}
}

void GameService::onPlayerSaved(int32_t uid, int32_t sid, int32_t reason, int32_t param, CharId_t cid)
{
	NetPacket *packet = popNetpacket(PACK_PROC, IM_GAME_GATE_USER_SAVED);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(uid);
	packet->writeInt32(sid);
	packet->writeInt32(reason);
	packet->writeInt64(cid);
	packet->writeInt32(param);
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

Answer::NetPacket* GameService::popNetpacket(Answer::PackType packType, uint16_t proc)
{
	NetPacket *packet = TcpService::popNetpacket();
	if (packet != NULL)
	{
		packet->setType(packType);
		packet->setProc(proc);
	}
	return packet;
}


Answer::NetPacket* GameService::popNetpacket(Answer::PackType packType, uint16_t proc, unsigned int buffersize)
{
	NetPacket *packet = TcpService::popNetpacket(buffersize);
	if (packet != NULL)
	{
		packet->setType(packType);
		packet->setProc(proc);
	}
	return packet;
}

void GameService::broadcast( Answer::NetPacket *inPacket, const CharIdList& cids )
{
	if ( NULL == inPacket || cids.empty() )
	{
		return;
	}

	MutexGuard lock(m_playerLock);

	uint32_t oldSize = inPacket->getSize();
	int32_t nsize=0;
	Int16List indexList;
	for ( CharIdList::const_iterator iter = cids.begin(); iter != cids.end(); ++iter )
	{
		CharIdPlayerMap::iterator findIter = m_players.find( *iter );
		if ( findIter != m_players.end() )
		{
			indexList.push_back( findIter->second->getGateIndex() );
			++nsize;
		}
	}

	if ( 0 == nsize )
	{
		inPacket->destroy();
		return;
	}

	uint32_t addonSize = (nsize+1)*(sizeof(int16_t));
	if (inPacket->rightShift(addonSize))
	{
		inPacket->writeInt16(static_cast<int16_t>(nsize));
		for ( Int16List::iterator it = indexList.begin(); it != indexList.end(); ++it)
		{
			inPacket->writeInt16( *it );
		}
		inPacket->setSize(oldSize+addonSize);
		sendPacket(inPacket);
	}
	else
	{
		NetPacket *packet = popNetpacket(inPacket->getType(), inPacket->getProc(), (nsize+1)*sizeof(int16_t) + inPacket->getSize());
		if (NULL == packet)
		{
			return;
		}
		packet->writeInt16(static_cast<int16_t>(nsize));
		for ( Int16List::iterator it = indexList.begin(); it != indexList.end(); ++it)
		{
			inPacket->writeInt16( *it );
		}
		packet->write(inPacket->getBuffer(), inPacket->getSize());
		packet->setSize(packet->getWOffset());
		sendPacket(packet);
		inPacket->destroy();
	}
}

void GameService::broadcastHuoDongDaTingIcon()
{
	MutexGuard lock(m_playerLock);

	if (m_players.empty())
	{
		return;
	}
	for (CharIdPlayerMap::iterator it = m_players.begin(); it != m_players.end(); ++it)
	{
		it->second->GetPlayerDailyActivity().SendHuoDongDaTingIcon();
	}
}

void GameService::broadcast(Answer::NetPacket *inPacket)
{
	if (inPacket == NULL)
	{
		return;
	}

	MutexGuard lock(m_playerLock);

	if (m_players.empty())
	{
		inPacket->destroy();
		return;
	}

	uint32_t oldSize = inPacket->getSize();
	uint32_t addonSize = (m_players.size()+1)*(sizeof(int16_t));
	if (inPacket->rightShift(addonSize))
	{
		inPacket->writeInt16(static_cast<int16_t>(m_players.size()));
		for (CharIdPlayerMap::iterator it = m_players.begin(); it != m_players.end(); ++it)
		{
			inPacket->writeInt16(it->second->getGateIndex());
		}
		inPacket->setSize(oldSize+addonSize);
		sendPacket(inPacket);
	}
	else
	{
		NetPacket *packet = popNetpacket(inPacket->getType(), inPacket->getProc(), (m_players.size()+1)*sizeof(int16_t) + inPacket->getSize());
		if (NULL == packet)
		{
			return;
		}
		packet->writeInt16(static_cast<int16_t>(m_players.size()));
		for (CharIdPlayerMap::iterator it = m_players.begin(); it != m_players.end(); ++it)
		{
			packet->writeInt16(it->second->getGateIndex());
		}
		packet->write(inPacket->getBuffer(), inPacket->getSize());
		packet->setSize(packet->getWOffset());
		sendPacket(packet);
		inPacket->destroy();
	}
}

void GameService::worldBroadcast(Answer::NetPacket *inPacket)
{
	if (inPacket == NULL)
	{
		return;
	}

	if (m_players.empty())
	{
		inPacket->destroy();
		return;
	}

	uint32_t oldSize = inPacket->getSize();
	uint32_t addonSize = sizeof(int16_t);
	
	if (inPacket->rightShift(addonSize))
	{
		inPacket->writeInt16(-1);
		inPacket->setSize(oldSize+addonSize);
		sendPacket(inPacket);
	}
	else
	{
		NetPacket *packet = popNetpacket(inPacket->getType(), inPacket->getProc(), sizeof(int16_t)+inPacket->getSize());
		if (NULL == packet)
		{
			return;
		}
		packet->writeInt16(-1);
		packet->write(inPacket->getBuffer(), inPacket->getSize());
		packet->setSize(packet->getWOffset());
		sendPacket(packet);
		inPacket->destroy();
	}
}

void GameService::AddPlayer(Player *player)
{
	if (player != NULL)
	{
		{
			Answer::MutexGuard lock(m_playerLock);
			m_players[player->getCid()] = player;
		}

		//sendSocialAddPlayerInfo(player);
		sendSocialAddPlayer(player);
		player->sendBasicInfo();
		DB_SERVICE.UpdateLoginTime( player->getCid(), player->getLastLoginTime() );
	}
}

void GameService::removePlayer(Player *player)
{
	if (player != NULL)
	{
		Answer::MutexGuard lock(m_playerLock);
		CharIdPlayerMap::iterator it = m_players.find(player->getCid());
		if (it == m_players.end())
		{
			LOG_INFO("GameService::removePlayer player = %p, cid = %lld not in m_players\n", player, player->getCid());
		}
		else if (it->second->getEntityId() != player->getEntityId())
		{
			LOG_INFO("GameService::removePlayer player entityid = %lld, cid = %lld not equal with it->seconde entity id = %lld, cid =%lld\n", player->getEntityId(), player->getCid(), it->second->getEntityId(), it->second->getCid());
		}
		else
		{
			m_players.erase(it);
			sendSocialRemovePlayer(player);
		}
	}
}

//Player* GameService::getPlayer(int32_t id,int32_t type)
//{
//	//if (type == ET_PET)
//	//{
//	//	ViceGeneral*pVicegeneral = getViceGeneral(id);
//	//	if (pVicegeneral == NULL)
//	//	{
//	//		return NULL;
//	//	}
//
//	//	return getPlayer(pVicegeneral->getOwner());
//	//}
//	//else if (type == ET_PLAYER)
//	//{
//		return getPlayer(id);
//	//}
//	//else
//	//{
//	//	return NULL;
//	//}
//}

Player* GameService::getPlayer( CharId_t cid, int32_t nRunnerId, bool bCheck )
{
	Answer::MutexGuard lock( m_playerLock );
	CharIdPlayerMap::iterator iter = m_players.find(cid);
	if ( iter != m_players.end() )
	{
		Player* pPlayer = iter->second;
		if ( pPlayer != NULL )
		{
			if( bCheck )
			{ 
				if( pPlayer->GetRunnerId() == nRunnerId )
				{
					return pPlayer;
				}
			}
			else
			{
				return pPlayer;
			}
		}
	}

	return NULL;
}

void GameService::addPet( CObjPet *pPet )
{
	if ( pPet != NULL )
	{
		Answer::MutexGuard lock( m_petsLock );
		m_pets[pPet->GetPetId()] = pPet;
	}
}

void GameService::removePet( CObjPet *pPet )
{
	if ( pPet != NULL )
	{
		Answer::MutexGuard lock( m_petsLock );
		m_pets.erase( pPet->GetPetId() );
	}
}

CObjPet* GameService::getPet( PetId_t pid, int32_t nRunnerId )
{
	Answer::MutexGuard lock(m_petsLock);
	PetIdObjPetMap::iterator iter = m_pets.find( pid );
	if ( iter != m_pets.end() )
	{
		CObjPet* pPet = iter->second;
		if ( pPet != NULL && pPet->GetRunnerId() == nRunnerId )
		{
			return pPet;
		}
	}

	return NULL;
}

void GameService::addMonster(Monster *monster)
{
	if (monster != NULL)
	{
		Answer::MutexGuard lock(m_monstersLock);
		m_monsters[monster->getUnitId()] = monster;
	}
}

void GameService::removeMonster(Monster *monster)
{
	if (monster != NULL)
	{
		Answer::MutexGuard lock(m_monstersLock);
		m_monsters.erase(monster->getUnitId());
	}
}

Monster* GameService::getMonster( EntityId_t unitid, int32_t nRunnerId )
{
	Answer::MutexGuard lock(m_monstersLock);
	EntityIdMonsterMap::iterator iter = m_monsters.find(unitid);
	if ( iter != m_monsters.end() )
	{
		Monster* pMonster = iter->second;
		if ( pMonster != NULL && pMonster->GetRunnerId() == nRunnerId )
		{
			return pMonster;
		}
	}

	return NULL;
}

void GameService::addTrailer(Trailer *trailer)
{
	if (trailer != NULL)
	{
		Answer::MutexGuard lock(m_trailersLock);
		m_trailers[trailer->getUnitId()] = trailer;
	}
}


void GameService::removeTrailer(Trailer *trailer)
{
	if (trailer != NULL)
	{
		Answer::MutexGuard lock(m_trailersLock);
		m_trailers.erase(trailer->getUnitId());
	}
}

Trailer* GameService::getTrailer( EntityId_t unitid, int32_t nRunnerId )
{
	Answer::MutexGuard lock(m_trailersLock);
	EntityIdTrailerMap::iterator iter = m_trailers.find(unitid);
	if ( iter != m_trailers.end() )
	{
		Trailer* pTrailer = iter->second;
		if ( pTrailer != NULL && pTrailer->GetRunnerId() == nRunnerId )
		{
			return pTrailer;
		}
	}

	return NULL;
}

// void GameService::onlineBroadCast(int32_t cid,const std::string &name,int32_t kingdom,int32_t post,int32_t time)
// {
// 	if (time >= m_onLineBroadcast[post])
// 	{
// 		broadcastOnline(cid,name,kingdom,post);
// 		m_onLineBroadcast[post] = time+30*60;
// 	}
// }

Unit* GameService::getUnit(EntityId_t unitid, int32_t unittype, int32_t nRunnerId)
{
	switch (unittype)
	{
	case ET_PLAYER:		return getPlayer( unitid, nRunnerId );
	case ET_MONSTER:	return getMonster( unitid, nRunnerId );
	case ET_PET:		return getPet( unitid, nRunnerId );
	case ET_TRAILER:	return getTrailer( unitid, nRunnerId );
	default: return NULL;
	}
}

void GameService::saveAllPlayerToDB()
{
	Answer::MutexGuard lock(m_playerLock);
	for (CharIdPlayerMap::iterator it = m_players.begin(); it != m_players.end(); ++it)
	{
		Player *player = it->second;
		if (player != NULL)
		{
			DB_SERVICE.logPlayerLogin(player->getCid(), 0, TIMER.GetNow());
			player->saveToDB(LR_DISCONNECT, 0);
		}
	}
}

void GameService::onUserPayed(int32_t uid, int32_t sid)
{
	Answer::MutexGuard lock(m_playerLock);
	for (CharIdPlayerMap::iterator it = m_players.begin(); it != m_players.end(); ++it)
	{
		Player *player = it->second;
		if (player != NULL && player->getUid() == uid && player->getSid() == sid)
		{
			player->syncGold();
			break;
		}
	}
}

// void GameService::updateMonsterBroadcast(const MonsterBroadcast &monsterBroadcast)
// {
// 	if (!CFG_DATA.isMonsterBroadcast(monsterBroadcast.mid))
// 	{
// 		return;
// 	}
// 
// 	Answer::RwLockWrGuard lock(m_monsterBroadcastLock);
// 	for (MonsterBroadcastVector::iterator it = m_monsterBroadcast.begin(); it != m_monsterBroadcast.end(); ++it)
// 	{
// 		if (it->mid == monsterBroadcast.mid && it->kingdom == monsterBroadcast.kingdom)
// 		{
// 			std::string old_killer = it->last_killer;
// 			*it = monsterBroadcast;
// 			if (monsterBroadcast.state == MBS_INACTIVE)
// 			{
// 				it->last_killer = old_killer;
// 			}
// 			return;
// 		}
// 	}
// 	m_monsterBroadcast.push_back(monsterBroadcast);
// }

// void GameService::sendMonsterBroadcast(Player *player)
// {
// 	if (player == NULL)
// 	{
// 		return;
// 	}
// 
// 	Answer::RwLockRdGuard lock(m_monsterBroadcastLock);
// 	NetPacket *packet = popNetpacket(PACK_DISPATCH, SM_MONSTER_BROADCAST);
// 	if (NULL == packet)
// 	{
// 		return;
// 	}
// 	int32_t count = 0;
// 	packet->writeInt32(count);
// 	for (MonsterBroadcastVector::iterator it = m_monsterBroadcast.begin(); it != m_monsterBroadcast.end(); ++it)
// 	{
// 		//if (it->kingdom == player->getKingdom() || it->kingdom == KI_NEUTRAL)
// 		//{
// 			packet->writeInt32(it->mid);
// 			packet->writeInt32(it->state);
// 			packet->writeUTF8(it->last_killer);
// 			packet->writeInt32(it->next_revive);
// 			++count;
// 		//}
// 	}
// 	int32_t oldoffset = packet->getWOffset();
// 	packet->setWOffset(0);
// 	packet->writeInt32(count);
// 	packet->setWOffset(oldoffset);
// 	packet->setSize(packet->getWOffset());
// 	sendPacketTo(player->getGateIndex(), packet);
// }


// void GameService::broadcastEventBossDie(int32_t killer_cid, std::string killer_name,int32_t mapid, int32_t bossid,int32_t item)
// {
// 	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_GAME_EVENT);
// 	if (NULL == packet)
// 	{
// 		return;
// 	}
// 	packet->writeInt32(GET_BOSS_DIE);
// 	packet->writeInt32(killer_cid);
// 	packet->writeUTF8(killer_name);
// 	//packet->writeInt32(kingdom);
// 	packet->writeInt32(mapid);
// 	packet->writeInt32(bossid);
// 	packet->writeInt32(item);
// 	packet->setSize(packet->getWOffset());
// 
// 	GAME_SERVICE.worldBroadcast(packet);
// }

// void GameService::broadcastOnline(int32_t cid, std::string name,int32_t kingdom,int32_t post)
// {
// 	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_GAME_EVENT);
// 	if (NULL == packet)
// 	{
// 		return;
// 	}
// 	packet->writeInt32(GET_ONLINE);
// 	packet->writeInt32(cid);
// 	packet->writeUTF8(name);
// 	packet->writeUInt16(kingdom);
// 	packet->writeInt32(post);
// 	packet->setSize(packet->getWOffset());
// 
// 	GAME_SERVICE.worldBroadcast(packet);
// }

//void GameService::broadcastGamble(int32_t cid,const std::string name,int32_t kingdom,const GambleRecordVector & record,int32_t type)
//{
//	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_GAMBLE);
//	if (NULL == packet)
//	{
//		return;
//	}
//	packet->writeInt32(cid);
//	packet->writeUTF8(name);
//	packet->writeInt32(kingdom);
//	packet->writeInt32(record.size());
//	packet->writeInt32(type);
//
//	for (GambleRecordVector::const_iterator it = record.begin();it != record.end();++it)
//	{
//		packet->writeInt32(it->item);
//		packet->writeInt32(it->item_type);
//		packet->writeInt32(it->count);
//	}
//
//	packet->setSize(packet->getWOffset());
//
//	GAME_SERVICE.worldBroadcast(packet);
//}

//void GameService::broadcastGainVicegeneral(int32_t cid,const std::string name,int32_t vid)
//{
//	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_GAIN_VICEGENERAL_BROADCAST);
//	if (NULL == packet)
//	{
//		return;
//	}
//	packet->writeInt32(cid);
//	packet->writeUTF8(name);
//	packet->writeInt32(vid);
//
//	packet->setSize(packet->getWOffset());
//
//	GAME_SERVICE.worldBroadcast(packet);
//}

//void GameService::broadcastVicegeneralUpgrade(int32_t cid,const std::string name,int32_t vid)
//{
//	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_VICEGENERAL_UPGRADE_BROADCAST);
//	if (NULL == packet)
//	{
//		return;
//	}
//	packet->writeInt32(cid);
//	packet->writeUTF8(name);
//	packet->writeInt32(vid);
//
//	packet->setSize(packet->getWOffset());
//
//	GAME_SERVICE.worldBroadcast(packet);
//}

//void GameService::broadcastVicegeneralGengu(int32_t cid,const std::string &name,int32_t vid,int32_t level)
//{
//	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_VICEGENERAL_GENGU_BROADCAST);
//	if (NULL == packet)
//	{
//		return;
//	}
//	packet->writeInt32(cid);
//	packet->writeUTF8(name);
//	packet->writeInt32(vid);
//	packet->writeInt32(level);
//
//	packet->setSize(packet->getWOffset());
//
//	GAME_SERVICE.worldBroadcast(packet);
//}

//void GameService::broadcastVicegeneralWuyi(int32_t cid,const std::string &name,int32_t vid,int32_t level)
//{
//	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_VICEGENERAL_WUYI_BROADCAST);
//	if (NULL == packet)
//	{
//		return;
//	}
//	packet->writeInt32(cid);
//	packet->writeUTF8(name);
//	packet->writeUInt32(vid);
//	packet->writeInt32(level);
//
//	packet->setSize(packet->getWOffset());
//
//	GAME_SERVICE.worldBroadcast(packet);
//}

// void GameService::broadcastEventBossRevive(int32_t kingdom, int32_t mapid, int32_t bossid)
// {
// 	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_GAME_EVENT);
// 	if (NULL == packet)
// 	{
// 		return;
// 	}
// 	packet->writeInt32(GET_BOSS_REVIVE);
// 	packet->writeInt32(kingdom);
// 	packet->writeInt32(mapid);
// 	packet->writeInt32(bossid);
// 	packet->writeInt32(getLine());
// 	packet->setSize(packet->getWOffset());
// 
// 	GAME_SERVICE.worldBroadcast(packet);
// }

// void GameService::broadcastEventCompleteDungeon(int32_t leader_cid, std::string leader_name, int32_t kingdom, int32_t dungeon_id)
// {
// 	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_GAME_EVENT);
// 	if (NULL == packet)
// 	{
// 		return;
// 	}
// 	packet->writeInt32(GET_COMPLETE_DUNGEON);
// 	packet->writeInt32(leader_cid);
// 	packet->writeUTF8(leader_name);
// 	packet->writeInt32(kingdom);
// 	packet->writeInt32(dungeon_id);
// 	packet->setSize(packet->getWOffset());
// 
// 	GAME_SERVICE.worldBroadcast(packet);
// }

// void GameService::broadcastEventArriveLevel(int32_t cid, std::string name, int32_t kingdom, int32_t level)
// {
// 	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_GAME_EVENT);
// 	if (NULL == packet)
// 	{
// 		return;
// 	}
// 	packet->writeInt32(GET_ARRIVE_LEVEL);
// 	packet->writeInt32(cid);
// 	packet->writeUTF8(name);
// 	packet->writeInt32(kingdom);
// 	packet->writeInt32(level);
// 	packet->setSize(packet->getWOffset());
// 
// 	GAME_SERVICE.worldBroadcast(packet);
// }

// void GameService::broadcastEventArtifactLevel(int32_t cid, std::string name, int32_t kingdom, int32_t artifact_id, int32_t level)
// {
// 	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_GAME_EVENT);
// 	if (NULL == packet)
// 	{
// 		return;
// 	}
// 	packet->writeInt32(GET_ARTIFACT_LEVEL);
// 	packet->writeInt32(cid);
// 	packet->writeUTF8(name);
// 	packet->writeInt32(kingdom);
// 	packet->writeInt32(artifact_id);
// 	packet->writeInt32(level);
// 	packet->setSize(packet->getWOffset());
// 
// 	GAME_SERVICE.worldBroadcast(packet);
// }

// void GameService::broadcastEventEnhanceEquip(int32_t cid, std::string name, int32_t kingdom, int32_t equip_id, int32_t level)
// {
// 	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_GAME_EVENT);
// 	if (NULL == packet)
// 	{
// 		return;
// 	}
// 	packet->writeInt32(GET_ENHANCE_EQUIP);
// 	packet->writeInt32(cid);
// 	packet->writeUTF8(name);
// 	packet->writeInt32(kingdom);
// 	packet->writeInt32(equip_id);
// 	packet->writeInt32(level);
// 	packet->setSize(packet->getWOffset());
// 
// 	GAME_SERVICE.worldBroadcast(packet);
// }

// void GameService::broadcastEventGetPetEgg(int32_t cid, std::string name, int32_t kingdom, int32_t egg_id)
// {
// 	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_GAME_EVENT);
// 	if (NULL == packet)
// 	{
// 		return;
// 	}
// 	packet->writeInt32(GET_GET_EGG);
// 	packet->writeInt32(cid);
// 	packet->writeUTF8(name);
// 	packet->writeInt32(kingdom);
// 	packet->writeInt32(egg_id);
// 	packet->setSize(packet->getWOffset());
// 
// 	GAME_SERVICE.worldBroadcast(packet);
// }

// void GameService::broadcastEventCreateFamily(int32_t cid, std::string name, int32_t kingdom, std::string fname)
// {
// 	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_GAME_EVENT);
// 	if (NULL == packet)
// 	{
// 		return;
// 	}
// 	packet->writeInt32(GET_CREATE_FAMILY);
// 	packet->writeInt32(cid);
// 	packet->writeUTF8(name);
// 	packet->writeInt32(kingdom);
// 	packet->writeUTF8(fname);
// 	packet->setSize(packet->getWOffset());
// 
// 	GAME_SERVICE.worldBroadcast(packet);
// }

// void GameService::broadcastEventKillPlayerReputation(int32_t dierKingdom, int32_t dierCid, const std::string &dierName, int32_t killerKingdom, int32_t killerCid, const std::string &killerName, int32_t mapid)
// {
// 	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_GAME_EVENT);
// 	if (NULL == packet)
// 	{
// 		return;
// 	}
// 	packet->writeInt32(GET_KILL_PLAYER_REPUTATION);
// 	packet->writeInt32(dierKingdom);
// 	packet->writeInt32(dierCid);
// 	packet->writeUTF8(dierName);
// 	packet->writeInt32(killerKingdom);
// 	packet->writeInt32(killerCid);
// 	packet->writeUTF8(killerName);
// 	packet->writeInt32(mapid);
// 	packet->setSize(packet->getWOffset());
// 
// 	GAME_SERVICE.worldBroadcast(packet);
// }

// void GameService::broadcastEventTreasure(int32_t cid, std::string name, int32_t kingdom, const MemEquipVector &equips, const MemChrBagVector &items, int32_t treasure)
// {
// 	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_GAME_EVENT);
// 	if (NULL == packet)
// 	{
// 		return;
// 	}
// 	packet->writeInt32(GET_TREASURE);
// 	packet->writeInt32(cid);
// 	packet->writeUTF8(name);
// 	packet->writeInt32(kingdom);
// 	EQUIP_MANAGER.PacketEquipInfo( packet, equips );
// 	packet->writeInt32(static_cast<int32_t>(items.size()));
// 	for (MemChrBagVector::const_iterator it = items.begin(); it != items.end(); ++it)
// 	{
// 		packet->writeInt32(it->itemId);
// 		packet->writeInt32(it->itemClass);
// 	}
// 	packet->writeInt32(treasure);
// 	packet->setSize(packet->getWOffset());
// 
// 	GAME_SERVICE.worldBroadcast(packet);
// }

// void GameService::broadcastEventTrailer(int32_t cid, std::string name, int32_t kingdom, int32_t quality)
// {
// 	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_GAME_EVENT);
// 	if (NULL == packet)
// 	{
// 		return;
// 	}
// 	packet->writeInt32(GET_TRAILER);
// 	packet->writeInt32(cid);
// 	packet->writeUTF8(name);
// 	packet->writeInt32(kingdom);
// 	packet->writeInt32(quality);
// 	packet->setSize(packet->getWOffset());
// 
// 	GAME_SERVICE.worldBroadcast(packet);
// }

// void GameService::broadcastEventKillPlayerPkValue(int32_t dierKingdom, int32_t dierCid, const std::string &dierName, int32_t killerKingdom, int32_t killerCid, const std::string &killerName, int32_t mapid)
// {
// 	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_GAME_EVENT);
// 	if (NULL == packet)
// 	{
// 		return;
// 	}
// 	packet->writeInt32(GET_KILL_PLAYER_PKVALUE);
// 	packet->writeInt32(dierKingdom);
// 	packet->writeInt32(dierCid);
// 	packet->writeUTF8(dierName);
// 	packet->writeInt32(killerKingdom);
// 	packet->writeInt32(killerCid);
// 	packet->writeUTF8(killerName);
// 	packet->writeInt32(mapid);
// 	packet->setSize(packet->getWOffset());
// 
// 	GAME_SERVICE.worldBroadcast(packet);
// }


// void GameService::broadcastEventTrailerKilled(int32_t killer_cid, std::string killer_name, int32_t killer_kingdom, int32_t dier_cid, std::string dier_name, int32_t dier_kingdom)
// {
// 	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_GAME_EVENT);
// 	if (NULL == packet)
// 	{
// 		return;
// 	}
// 	packet->writeInt32(GET_TRAILER_KILLER);
// 	packet->writeInt32(killer_cid);
// 	packet->writeUTF8(killer_name);
// 	packet->writeInt32(killer_kingdom);
// 	packet->writeInt32(dier_cid);
// 	packet->writeUTF8(dier_name);
// 	packet->writeInt32(dier_kingdom);
// 	packet->setSize(packet->getWOffset());
// 
// 	GAME_SERVICE.worldBroadcast(packet);
// }

// �㲥������ҵĻ����
// void GameService::broadcastActivityState()
// {
// 	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_ACTIVITY_STATE);
// 	if (NULL == packet)
// 	{
// 		return;
// 	}
// 	ACTIVITY_MANAGER.AppendActivityState(packet);
// 	packet->setSize(packet->getWOffset());
// 	GAME_SERVICE.broadcast(packet);
// }

void GameService::onAddUser(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return;
	}
	int16_t cgindex = inPacket->readInt16();
	int32_t uid = inPacket->readInt32();
	int32_t sid = inPacket->readInt32();

	if (cgindex > 0 && cgindex < MAX_CONNECTION && uid > 0 && sid > 0)
	{
		User *user = POOL_MANAGER.pop<User>();
		if (NULL == user)
		{
			return;
		}
		user->init(uid, sid);
		m_users[cgindex] = user;
#ifdef _DEBUG
		printf( "GameService::onAddUser() index=%d,\tuid=%d\n", cgindex, user->getUid() );
#endif
	}
}

void GameService::qqToPlayer(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return;
	}
	int16_t cgindex = inPacket->readInt16();
	std::string openId = inPacket->readUTF8(true);
	std::string openKey = inPacket->readUTF8(true);
	std::string pf = inPacket->readUTF8(true);
	std::string pfKey = inPacket->readUTF8(true);
	int32_t is_yellow_vip = inPacket->readInt32();
	int32_t is_yellow_year_vip = inPacket->readInt32();
	int32_t yellow_vip_level = inPacket->readInt32();
	int32_t is_yellow_high_vip = inPacket->readInt32();

	User *user = m_users[cgindex];
	if (NULL == user)
	{
		return;
	}
	user->setOpenId(openId);
	user->setOpenKey(openKey);
	user->setPf(pf);
	user->setPfKey(pfKey);

	user->setIsYellowVip(is_yellow_vip);
	user->setIsYellowYearVip(is_yellow_year_vip);
	user->setYellowVipLevel(yellow_vip_level);
	user->setIsYellowHighVip(is_yellow_high_vip);

}

void GameService::onRemoveUser(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return;
	}
	int16_t index = inPacket->readInt16();
	if (index > 0 && index < MAX_CONNECTION)
	{
		User *user = m_users[index];
		if (user != NULL)
		{

			m_users[index] = NULL;

			Player *player = user->getPlayer();
			if (player != NULL)
			{
				inPacket->readInt32(); //uid
				int32_t reason = inPacket->readInt32();
				int32_t param = inPacket->readInt32();
				player->addLogoutPacket(reason, param);				
			}

			user->reset();
			POOL_MANAGER.push<User>( user );
		}
	}
}

void GameService::onUpdateBanChat(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return;
	}
	int32_t action = inPacket->readInt32();
	int32_t uid = inPacket->readInt32();
	int32_t expire_time = inPacket->readInt32();
	CFG_DATA.onBanChatUpdated(action, uid, expire_time);
}

void GameService::onGMReloadItem(Answer::NetPacket *inPacket)
{
	CFG_DATA.reload();
}

void GameService::onAvgLevel(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return;
	}
	CFG_DATA.onAvgLevelUpdated(0);
}

void GameService::onSocialNetpacket(Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return;
	}
	int16_t cgindex = static_cast<int32_t>(inPacket->readInt32());
	if (cgindex > 0 && cgindex < MAX_CONNECTION)
	{
		onGameNetpacket(cgindex, inPacket);
	}
}

void GameService::onEnterGame(int16_t cgindex, Answer::NetPacket *inPacket)
{
	User *user = m_users[cgindex];
	if(user!=NULL)
	{
		DB_SERVICE.loadPlayer(cgindex, user->getUid(),user->getSid());
	}
}

void GameService::onEnterGameRobot(int16_t cgindex, Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return;
	}
	User *user = m_users[cgindex];
	if (user != NULL && user->getCid() == 0)
	{
		PlayerDBData dbData;

		CharId_t cid = inPacket->readInt32();
		if (cid > 0x1234567)
		{
			inPacket->readInt32();//line
			user->setCid(cid);
			dbData.chr.data.cid = cid;
			snprintf(dbData.chr.data.name, MAX_NAME_CCH_LENGTH, inPacket->readUTF8(true).c_str());
			dbData.chr.data.sex = inPacket->readInt32();
			dbData.chr.data.job = inPacket->readInt32();
			inPacket->readInt32();
			inPacket->readInt32();
			dbData.chr.data.level = 60;
			dbData.chr.data.mapid = inPacket->readInt32();
			dbData.chr.data.x = inPacket->readInt32();
			dbData.chr.data.y = inPacket->readInt32();
			dbData.chr.data.hp = inPacket->readInt32();
			dbData.chr.data.mp = inPacket->readInt32();
			dbData.chr.data.head = inPacket->readInt32();

			int32_t id[]={10001,10006,10011};

			int16_t tempId =id[RANDOM.generate(0,0)];
			onPlayerLoaded( dbData );
		}
	}
}
//��������Ӷ���
void GameService::onGameNetpacket(int16_t cgindex, Answer::NetPacket *inPacket)
{
	if (NULL == inPacket)
	{
		return;
	}
	User *user = m_users[cgindex];
	if (user != NULL)
	{
		Player *player = user->getPlayer();
		if (player != NULL)
		{
			player->addNetPacket( inPacket, sizeof(int32_t) );
		}
	}
}

//void GameService::sendSocialAddPlayerInfo(Player *player)
//{
//	if (player == NULL)
//	{
//		return;
//	}
//
//	NetPacket *packet = popNetpacket(PACK_PROC, IM_SOCIAL_ADD_PLAYER_INFO);
//	if (NULL == packet)
//	{
//		return;
//	}
//	packet->writeInt32(player->getCid());
//	packet->writeUTF8(player->getName());
//	packet->writeInt8(player->getSex());
//	packet->writeInt8(player->getJob());
//	packet->writeInt32(0);
//	packet->writeInt32(player->getLevel()); 
//	packet->writeInt32(player->getVip().type);
//	packet->writeInt32(player->getFamilyId());
//	packet->writeInt32(0);
//	packet->writeInt32(0);
//	packet->writeInt32(player->getFightPower());
//	packet->writeInt8( player->GetCharSkill().IsLearnedXPSkill() ? 1 : 0 );
//	packet->setSize(packet->getWOffset());
//	sendPacket(packet);
//}

//void GameService::sendSocialUpdateGamble(const GambleRecordVector & record)
//{
//	NetPacket *packet = popNetpacket(PACK_PROC, IM_SOCIAL_UPDATE_GAMBLE);
//	if (NULL == packet)
//	{
//		return;
//	}
//	packet->writeInt32(record.size());
//
//	for (GambleRecordVector::const_iterator it = record.begin();it != record.end();++it)
//	{
//		packet->writeInt32(it->cid);
//		packet->writeUTF8(it->name);
//		packet->writeInt32(it->type);
//		packet->writeInt32(it->item);
//		packet->writeInt32(it->item_type);
//		packet->writeInt32(it->count);
//	}
//	packet->setSize(packet->getWOffset());
//	sendPacket(packet);
//}

void GameService::sendSocialAddPlayer(Player *player)
{
	if (player == NULL)
	{
		return;
	}

	NetPacket *packet = popNetpacket(PACK_PROC, IM_SOCIAL_ADD_PLAYER);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32(player->getGateIndex());
	packet->writeInt64(player->getCid());
	packet->writeInt32(player->getUid());
	packet->writeInt32(player->getSid());
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

//void GameService::sendSocialFamilyActivityPlayer(const ActivityDamageStatisticsTable & rank,const Int32Vector &rewards,int32_t familyID)
//{
//	NetPacket *packet = popNetpacket(PACK_PROC, IM_SOCIAL_FAMILY_ACTIVITY_PLAYER);
//	if (NULL == packet)
//	{
//		return;
//	}
//	packet->writeInt32(familyID);
//	packet->writeInt16(rewards.size());
//	for (Int32Vector::const_iterator it = rewards.begin();it != rewards.end();++it)
//	{
//		packet->writeInt32(*it);
//	}
//
//	packet->writeInt16(rank.size());
//	for (ActivityDamageStatisticsTable::const_reverse_iterator it = rank.rbegin();it != rank.rend();++it)
//	{
//		packet->writeInt32(it->second.cid);
//		packet->writeUTF8(it->second.name);
//	}
//
//	packet->setSize(packet->getWOffset());
//	sendPacket(packet);
//}

void GameService::sendSocialRemovePlayer(Player *player)
{
	if (player == NULL)
	{
		return;
	}

	NetPacket *packet = popNetpacket(PACK_PROC, IM_SOCIAL_REMOVE_PLAYER);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt64(player->getCid());
	packet->setSize(packet->getWOffset());
	sendPacket(packet);
}

void GameService::onSocialUpdatePetRank( Answer::NetPacket* inPacket )
{
	PET_MANAGER.OnUpdatePetRank( inPacket );
}

void GameService::onSocialUpdateFamilyInfo( Answer::NetPacket* inPacket )
{
	FAMILY_MANAGER.OnUpdateFamilyInfo( inPacket );
}

void GameService::onSocialInitPetRank( Answer::NetPacket* inPacket )
{
	PET_MANAGER.OnInitPetRank( inPacket );
}

void GameService::onSocialCreateTeamDungeon( Answer::NetPacket* inPacket )
{
	int32_t nDungeonId = inPacket->readInt32();
	Dungeon* pDungeon = MAP_MANAGER.NewDungeon( nDungeonId );
	if ( pDungeon == NULL )
	{
		return;
	}

	CharIdList memberList;
	for ( int32_t i = 0; i < MAX_TEAM_MEMBER_COUNT; ++i )
	{
		CharId_t memberId = inPacket->readInt64();
		if ( memberId > 0 )
		{
			memberList.push_back( memberId );
		}
	}
	// ��ҵĽ����ڸ�������
	pDungeon->InitTeamMember( memberList );
	pDungeon->start();
	MAP_MANAGER.PostMsg( pDungeon->GetRunnerId(), GMC_ADD_DUNGEON, pDungeon );
}

void GameService::onSocialSendSystemMail( Answer::NetPacket* inPacket )
{
	if ( NULL == inPacket )
	{
		return;
	}

	CharId_t nReceiver	= inPacket->readInt64();
	int32_t	nMailId		= inPacket->readInt32();

	MemChrBag item = {};
	item.itemId		= inPacket->readInt32();
	item.itemClass	= inPacket->readInt8();
	item.itemCount	= inPacket->readInt32();
	item.srcId		= inPacket->readInt64();
	item.bind		= inPacket->readInt8();
	item.endTime	= inPacket->readInt32();

	if ( item.itemId > 0 && item.itemCount > 0 )
	{
		DB_SERVICE.OnSendSysMail( nReceiver, nMailId, item );
	}
	else
	{
		DB_SERVICE.OnSendSysMail( nReceiver, nMailId );
	}
}

void GameService::onSocialSendSystemMail2( Answer::NetPacket* inPacket )
{
	if ( NULL == inPacket )
	{
		return;
	}

	CharId_t nReceiver	= inPacket->readInt64();
	int32_t	nMailId		= inPacket->readInt32();
	int32_t nSize		= inPacket->readInt32();
	if ( nSize > 0 )
	{
		MemChrBagVector vItem;
		vItem.resize( nSize );
		for ( int32_t i = 0; i < nSize; ++i )
		{
			vItem[i].itemClass	= inPacket->readInt8();
			vItem[i].itemId		= inPacket->readInt32();
			vItem[i].itemCount	= inPacket->readInt32();
			vItem[i].srcId		= 0;
			vItem[i].bind		= 0;
			vItem[i].endTime	= 0;
		}
		DB_SERVICE.OnSendSysMail( nReceiver, nMailId, vItem );
	}
	else
	{
		DB_SERVICE.OnSendSysMail( nReceiver, nMailId );
	}
}

void GameService::onSocialChangeOwner( Answer::NetPacket* inPacket )
{
	if ( NULL == inPacket )
	{
		return;
	}

	int8_t nClass	= inPacket->readInt8();
	int64_t	nSrcId	= inPacket->readInt64();
	CharId_t nOwner	= inPacket->readInt64();

	if ( nClass == IC_EQUIP )
	{
		EQUIP_MANAGER.ChangeOwner( nSrcId, nOwner );
	}
	else if ( nClass == IC_PET )
	{
		PET_MANAGER.ChangeOwner( nSrcId, nOwner );
	}
}

void GameService::TeamDungeonEnterDungeon( Dungeon* pDungeon, const CharIdList& lst )
{
	if ( NULL == pDungeon )
	{
		return;
	}
	CharIdList::const_iterator iter = lst.begin();
	CharIdList::const_iterator eiter = lst.end();
	for ( ; iter != eiter; ++iter )
	{
		Answer::MutexGuard lock(m_playerLock);
		CharIdPlayerMap::iterator it = m_players.find( *iter );
		if ( it != m_players.end() )
		{
			Player* pPlayer = it->second;
			if ( pPlayer != NULL )
			{
				pPlayer->setOldPosition();
				Map* pMap = pPlayer->getMap();
				if ( pMap != NULL )
				{
					pPlayer->broadcastLeave();
					pMap->removePlayer( pPlayer, false);
				}

				GAME_SERVICE.replySuccess( pPlayer->getGateIndex(), CM_ENTER_DUNGEON, pDungeon->getDungeonId() );	// ��Ҫ���ڸ�����Ϣ�·����ͻ���
				pDungeon->addPlayer( pPlayer, pDungeon->GetCfgDungeon().x, pDungeon->GetCfgDungeon().y );
			}
		}
	}
}

void GameService::OnUpdateMail( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return;
	}
	CharId_t Cid = inPacket->readInt64();
	Answer::MutexGuard lock(m_playerLock);
	CharIdPlayerMap::iterator it = m_players.find( Cid );
	if ( it != m_players.end() )
	{
		if ( NULL != it->second )
		{
			it->second->addNetPacket( inPacket, sizeof(int64_t) );
		}
	}
}

void GameService::requestPetRankInfo()
{
	NetPacket *packet = GAME_SERVICE.popNetpacket( PACK_DISPATCH, IM_SOCIAL_REQUEST_PET_RANK_DATA );
	if (NULL == packet)
	{
		return;
	}

	packet->writeInt32( m_line );
	packet->setSize( packet->getWOffset() ); 
	GAME_SERVICE.sendPacket( packet );
}

void GameService::OnReceiveDaTiRank( Answer::NetPacket* inPacket )
{
	if ( NULL == inPacket )
	{
		return;
	}
	int32_t nSize = inPacket->readInt32();
	for ( int32_t i = 0; i < nSize; ++i )
	{
		CharId_t CharId		= inPacket->readInt64();
		std::string Name	= inPacket->readUTF8(true);
		int32_t Index		= inPacket->readInt32();
		if ( Index == 1 )
		{
			DB_SERVICE.OnSendSysMail( CharId, DaTiHDFirst );
			Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM );
			if (NULL == packet)
			{
				return;
			}
			packet->writeInt32( BCI_DA_TI_FIRST );
			packet->writeUTF8( Name );
			packet->writeInt64( CharId );
			packet->setSize(packet->getWOffset());
			worldBroadcast(packet);
		}
		else if ( Index == 2 )
		{
			DB_SERVICE.OnSendSysMail( CharId, DaTiHDSecond );
		}
		else if ( Index == 3 )
		{
			DB_SERVICE.OnSendSysMail( CharId, DaTiHDThird );
		}
		else
		{
			DB_SERVICE.OnSendSysMail( CharId, DaTiHDChanYu );
		}

		LogActivity logActivity = {};
		logActivity.cid = CharId;
		logActivity.acttype = ATI_DA_TI_HUO_DONG;
		logActivity.time = TIMER.GetNow();
		logActivity.param = Index;

		DB_SERVICE.InsertActivityLog( logActivity );
	}
}

void GameService::SendServerDiffToGlobal()
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_PROC, IM_SOCIAL_SYN_SERVER_DAY);
	if ( NULL == packet )
	{
		return;
	}
	int32_t DiffDay = DayTime::daydiff( CFG_DATA.getServerStartTime() );
	packet->writeInt32(DiffDay);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacket(packet);
}

void GameService::requestFamilyInfo()
{
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_PROC, IM_SOCIAL_REQUEST_FAMILY_INFO);
	if ( NULL == packet )
	{
		return;
	}
	packet->writeInt8(0);
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacket(packet);
}

void GameService::KickUser( CharId_t cid )
{
	Answer::MutexGuard lock( m_playerLock );
	CharIdPlayerMap::iterator iter = m_players.find( cid );
	if ( iter == m_players.end() )
	{
		return;
	}

	Player* player = iter->second;
	if ( NULL == player )
	{
		return;
	}

	NetPacket *packet = popNetpacket( PACK_PROC, IM_GAME_GATE_KICK_USER );
	if (NULL == packet)
	{
		return;
	}

	packet->writeInt32( player->getUid() );
	packet->writeInt32( player->getSid() );
	packet->setSize( packet->getWOffset() );
	sendPacket( packet );
}

// City war / cross-server functions
void GameService::broadFamilyWarIcon()
{
	Answer::MutexGuard lock(m_playerLock);
	for (CharIdPlayerMap::iterator it = m_players.begin(); it != m_players.end(); ++it)
	{
		Player* player = it->second;
		if (NULL != player)
		{
			player->SendFamilyWarIcon();
		}
	}
}

void GameService::UpdateCityWarTitle(FamilyId_t OldFamilyId, FamilyId_t NewFamilyId)
{
	Answer::MutexGuard lock(m_playerLock);
	for (CharIdPlayerMap::iterator it = m_players.begin(); it != m_players.end(); ++it)
	{
		Player* player = it->second;
		if (NULL == player)
		{
			continue;
		}
		if (player->getFamilyId() == OldFamilyId)
		{
			player->GetCharTitle().RemoveTitle(1, 0);
		}
		if (player->getFamilyId() == NewFamilyId)
		{
			int32_t position = player->getFamilyPosition();
			player->GetCharTitle().CheckAddTitle(1, position);
		}
	}
}

void GameService::UpdateCityActState(FamilyId_t FamilyId, int8_t ActState)
{
	Answer::MutexGuard lock(m_playerLock);
	for (CharIdPlayerMap::iterator it = m_players.begin(); it != m_players.end(); ++it)
	{
		Player* player = it->second;
		if (NULL != player && player->getFamilyId() == FamilyId)
		{
			player->SetActState(ActState);
		}
	}
}
