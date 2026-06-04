#pragma once
//////////////////////////////////////////////////////////////////////////
//author:zxj			modify Time: 2012 -7-7
//description:��Ϸ�̴߳�������
//////////////////////////////////////////////////////////////////////////
#include <list>
#include <vector>

#include "Player.h"
#include "User.h"

#define MAX_TREASURE_RECORD_SIZE  10
#define MAX_ONLINE_BROADCAST_COUNT 3



class MonsterActivity;
class NpcActivity;
class PlantActivity;
class Trailer;
class GameService
	: public Answer::TcpService
{
public:
	GameService();
	virtual ~GameService();

protected:
	virtual void onNetPacket(Answer::NetPacket *inPacket);

public:
	void startGame(int32_t line);
	void stopGame();

	int32_t getLine();

	void onPlayerLoaded( PlayerDBData& dbData, bool isRobot = false );
	void onPlayerLogout(Player *player, int16_t cgindex);
	void onPlayerSaved(int32_t uid, int32_t sid, int32_t reason, int32_t param, CharId_t cid);
	Answer::NetPacket* popNetpacket(Answer::PackType packType, uint16_t proc);
	Answer::NetPacket* popNetpacket(Answer::PackType packType, uint16_t proc, unsigned int buffersize);

	void broadcast(Answer::NetPacket *inPacket);
	void worldBroadcast(Answer::NetPacket *inPacket);
	void broadcast( Answer::NetPacket *inPacket, const CharIdList& cids );

	void AddPlayer(Player *player);
	void removePlayer(Player *player);
	Player* getPlayer( CharId_t cid, int32_t nRunnerId, bool bCheck = true );	// ֻ��ͬһ�߳���ֱ��ȡ�ã���ͬ�߳���Ͷ����Ϣ bCheck �Ƿ����ͬ�߼��

	void addPet( CObjPet *pPet );
	void removePet( CObjPet *pPet );
	CObjPet* getPet( PetId_t pid, int32_t nRunnerId );

	void addMonster(Monster *monster);
	void removeMonster(Monster *monster);
	Monster* getMonster( EntityId_t unitid, int32_t nRunnerId );

	void addTrailer(Trailer *trailer);
	void removeTrailer(Trailer *trailer);
	Trailer* getTrailer( EntityId_t unitid, int32_t nRunnerId );

	Unit* getUnit( EntityId_t unitid, int32_t unittype, int32_t nRunnerId );

	void saveAllPlayerToDB();

	void onUserPayed(int32_t uid, int32_t sid);
	void onNewMinuteCome(int32_t minute);

	void OnUpdateMail( Answer::NetPacket *inPacket );
	void TeamDungeonEnterDungeon( Dungeon* pDungeon, const CharIdList& lst );
	void broadcastHuoDongDaTingIcon();
	void SendServerDiffToGlobal();
	void requestFamilyInfo();
	void KickUser( CharId_t cid );

	// City war functions
	void broadFamilyWarIcon();
	void UpdateCityWarTitle(FamilyId_t OldFamilyId, FamilyId_t NewFamilyId);
	void UpdateCityActState(FamilyId_t FamilyId, int8_t ActState);

private:
	void onAddUser(Answer::NetPacket *inPacket);
	void onRemoveUser(Answer::NetPacket *inPacket);
	void onUpdateBanChat(Answer::NetPacket *inPacket);
	void onGMReloadItem(Answer::NetPacket *inPacket);
	void onAvgLevel(Answer::NetPacket *inPacket);
	void onSocialNetpacket(Answer::NetPacket *inPacket);
	void onEnterGame(int16_t cgindex, Answer::NetPacket *inPacket);
	void onEnterGameRobot(int16_t cgindex, Answer::NetPacket *inPacket);
	void onGameNetpacket(int16_t cgindex, Answer::NetPacket *inPacket); //�������social �� flash Client�����ݰ�

	void sendSocialAddPlayer(Player *player);
	void sendSocialRemovePlayer(Player *player);
	void qqToPlayer(Answer::NetPacket *inPacket);

	void onSocialUpdatePetRank( Answer::NetPacket* inPacket );
	void onSocialUpdateFamilyInfo( Answer::NetPacket* inPacket );
	void onSocialInitPetRank( Answer::NetPacket* inPacket );
	void onSocialCreateTeamDungeon( Answer::NetPacket* inPacket );
	void onSocialSendSystemMail( Answer::NetPacket* inPacket );
	void onSocialSendSystemMail2( Answer::NetPacket* inPacket );
	void onSocialChangeOwner( Answer::NetPacket* inPacket );

	void requestPetRankInfo();

	void OnReceiveDaTiRank( Answer::NetPacket* inPacket );
private:
	int32_t m_line;

	User * m_users[MAX_CONNECTION];

	CharIdPlayerMap m_players;
	Answer::Mutex m_playerLock;

	PetIdObjPetMap m_pets;
	Answer::Mutex m_petsLock;

	EntityIdMonsterMap m_monsters;
	Answer::Mutex m_monstersLock;

	EntityIdTrailerMap m_trailers;
	Answer::Mutex   m_trailersLock;

	int32_t m_onLineBroadcast[MAX_ONLINE_BROADCAST_COUNT];
};
#define GAME_SERVICE Answer::Singleton<GameService>::instance()
