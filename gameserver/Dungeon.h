#pragma once

#include "Map.h"
#include "MapEvent.h"

#include <map>
#include <string>
#include "actStructs.h"
#include "MonsterBuff.h"

#define  DUNGEON_GIFT_COUNT  3

struct DungeonTower
{
	int32_t	id;
	int32_t mid;
	int32_t limit;
	int32_t	free;
	int32_t count;
	int8_t	costType;
	int32_t costValue;
};
typedef std::list<DungeonTower> DungeonTowerList;

struct DungeonReward 
{
	CharId_t cid;
	int32_t	exp;
	int32_t money;
	int32_t cash;
	int8_t	bGet;
};
typedef std::list<DungeonReward> DungeonRewardList;

struct DamageSum 
{
	CharId_t cid;
	std::string name;
	int32_t damage;
};
typedef std::list<DamageSum> DamageSumList;

class MonsterDungeon;
class NpcDungeon;
class PlantDungeon;
/*
* ������ͼ
*/
class Dungeon
	: public Map, public IMapEvent
{
	friend class Trap;
	friend class Plant;
public:
	Dungeon();
	virtual ~Dungeon();

public:
	virtual void update();
	virtual Player* getEnemySidePlayer(Monster *monster);
	virtual bool IsDungeonMap() const;
	virtual bool CanUseXP() const;
	virtual bool IsFullXP() const;
	virtual bool CanUsePet() const;

public:
	virtual void reset();
	virtual void init( const CfgDungeon &cfgDungeon, const CfgMap &cfgmap, int32_t nId =0 );
	virtual void addPlayer( Player *player, int32_t x, int32_t y );

	int32_t GetId() const;
	int32_t GetStartTime() const;
	int32_t getDungeonType() const;
	int32_t getDungeonId() const;
	std::string getDungeonName() const;
	int32_t getDungeonGroupId() const;
	bool empty() const;
	bool IsWin() const;
	DungeonState getState() const;
	const CfgDungeon& GetCfgDungeon() const;

	void start();
	
	void onPlayerLeave(Player *player);
	void onMonsterDie(MonsterDungeon *monster);
	void onNpcEnd(NpcDungeon *npc, int8_t choice);
	void onPlayerDie(Player* player);
	void onPlantGather(PlantDungeon *plant);
	void onMonsterArriveRoadEnd(MonsterDungeon *monster);

	void onMonsterHPEvent( MonsterDungeon *monster, int32_t id );
	void onPlayerHPEvent(Player* player);
	bool onEndDungeon( Player* player );

	//bool updatePlayerGiftInfo(int32_t cid);
	//MemChrBagVector* getGiftReward(int32_t index);
	//void clearGift();

	PlayerList getPlayers();
	Player*	GetLeader();
	const Player* GetLeader() const;

	bool BuildTower( Player* player, int32_t nId, int16_t nPosX, int16_t nPosY );
	bool BuyTower( Player* player, int32_t nId );
	bool GetReward( Player* player, int8_t nRatio = 1 );

	void InitTeamMember( const CharIdList& lst );
	void AddPlayerDamage( CharId_t cid, int32_t attackValue );
	void AddDungeonTime( int32_t nTime );

protected:
	virtual void checkEvents();
	virtual void checkEvent( CfgMapEvent &mapEvent );
	virtual void triggerEvent( CfgMapEvent &mapEvent, Unit* pUnit = NULL );

private:
	//void initGift();
	void initPlayerInfo( Player* player );
	void setState( DungeonState state, int32_t param = 0 );
	//void createGift(CfgDungeonDropGroupVector* pGroup);
	void clearMonster();
	int32_t	GetDurationTime() const;
	int32_t	GetLeftTime();

	void setCanSubmit();
	void signDungeonCompleted();
	void generateMonster( const std::string &str, bool rewardWave = false );
	void addMonsterHPEventInfo(MonsterDungeon* pMonster);

	void checkDropItem( MonsterDungeon* pMonster );
	void checkAddMonster( int64_t curTick );
	bool flashMonster( MonsterWait& waitAddMonster, int64_t curTick, bool rewardWave = false );
	void flashMonster( CfgDungeonMonster* pCfgDungeonMonster, CfgMonster *pCfgMonster, int32_t x, int32_t y, bool rewardWave = false, MonsterBuff* pMonsterBuff = NULL );

	void broadcastFinishDungon(int32_t param);
	void sendDungeonResult(Player *player, int32_t param);
	void broadcastDungeonEvent(int32_t evtId);
	void broadcastPlayMovie(int32_t movieID);
	void broadcastDungeonInfo();
	void broadcastDamageList();

	void addWaitTower( const DungeonTower& tower );
	void teamDungeonMemberEnter();
	void checkRevive( int64_t curTick );
	void checkBroadcastDamageList();
	void SendZSZZDungeonInfoToGlobal();
	void DungeonGongGao( CharId_t CharId, std::string Name );

private:
	CfgDungeon			m_cfgDungeon;			// ��������
	int32_t				m_nAddonTime;			// ���⸱��ʱ��
	int32_t				m_nId;					// ��������ID
	DungeonState		m_state;				// ����״̬
	int64_t				m_stateTick;			// ״̬ת��ʱ��
	int32_t				m_stateParam;			// ״̬����
	double				m_dRatio;				// ����ϵ��Ĭ��100

    DungeonFinshInfo	m_finshInfo;			// ���������Ϣ
	Int32MonsterWaveMap m_monsterWave;			// ���ﲨ��
	MonsterWaitList		m_waitMonster;			// �ȴ�ˢ�µĹ���
	DungeonTowerList	m_waitTower;			// ���������
	MonsterList			m_buildTower;			// �Խ������
	DungeonRewardList	m_playerReward;			// ��ҽ���

	CharIdList			m_memberList;

	DamageSumList		m_damages;				// �˺�ͳ��
	bool				m_needBroadcastDamage;	// �Ƿ���Ҫ�㲥�˺��б�

	int32_t				m_nStartTime;			// ��ʼʱ��
};

