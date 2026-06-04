#pragma once

#include "CfgData.h"
#include "Unit.h"

enum MonsterState
{
	MS_STAND,
	MS_WALK_AROUND,
	MS_WALK_ATTACK,
	MS_WALK_HOME,
	MS_ATTACK,
	MS_CORPSE,
	MS_GHOST,
	MS_RUN_ON_ROAD,				// ����Ԥ��·���ƶ�
};

enum MonsterAIStype
{
	MAS_PEACE	= 0,			// ��ƽ
	MAS_PASSIVE	= 1,			// ����
	MAS_ACTIVE	= 2,			// ����
	MAS_TOWER	= 3,			// ��
};

class Monster
	: public Unit
{
public:
	Monster();
	virtual ~Monster();

public:
	virtual void	refresh();
	virtual void	reset();
	virtual EntityId_t getUnitId() const;
	virtual CharId_t getOwner() const;
	virtual int32_t getLevel() const;
	virtual int32_t	getBattle() const;
	virtual void	postDamage(int32_t damge, UnitHandle launcher);

public:
	virtual int32_t	getSide() const;
	virtual bool	isLifeEnd();
	virtual void	endLife();
	virtual bool	isActivityMonster();
	virtual bool	isDungeonMonster();
	virtual bool	needDel() const;
	virtual void	destroy();
	virtual bool	isBoss() const;

public:
	void init(const CfgMonster &cfgmonster, const CfgMapMonster &cfgmapmonster, Buff* pBuff);
	void init(const CfgMonster &cfgmonster, const CfgMapMonster &cfgmapmonster, MonsterState state, const AttrAddonVector *vAttrAddon);
	void appendInfo(Answer::NetPacket *packet);
	void leaveMap();
	void SetDieType( int8_t Type );

	int32_t getMid() const;
	int32_t getQuality() const;
	std::string getName() const;
	int32_t getViewRange() const;
	int32_t getMoveRange() const;
	int32_t getBroadcast() const;
	MonsterState getState() const;
	int32_t getReviveTime() const;
	int32_t getKillerType() const;
	Player* getKiller();
	const UnitHandle& getKillerHandler() const;
	int32_t	getAITarget() const;
	int32_t	getExp() const;
	FamilyId_t	GetFamilyId() const;
	void		SetFamilyId( FamilyId_t fid );

	void SetLifeTime(int32_t nLifeTime);
	void broadcastIntoMap();
	int32_t move(int16_t currentX, int16_t currentY, int16_t targetX, int16_t targetY, int16_t finalX, int16_t finalY, Direction direction);
	bool	SpiderQueenCanRevive();
	bool	checkSkillTarget( CfgSkill *pCfg, Unit *pUnit );
	bool	WarVictoryBossCanRevive();
protected:
	void updateState();

	virtual void onStand();
	void onWalkAround();
	void onWalkAttack();
	void onWalkHome();
	void onAttack();
	virtual void onCorpse();
	virtual void onGhost();
	virtual void onRunOnRoad();
	virtual void broadcastBasicData();

	void setState(MonsterState state);
	
	virtual void die();
	virtual void revive();

	void generateFightDrop(DropItem	(&dropItems)[MAX_DROPITEM_SIZE], Player* pPlay );
	DropItem dropItemFromGroup(int32_t groupId);
	int32_t getMinute();

	void refreshSkill();

	void backHome();
	void TaskShare( CharId_t KillerId );
	void CheckAddHp( int64_t CurTick );

protected:
	static Direction m_directions[8];

	CfgMonster m_cfgmonster;
	CfgMapMonster m_cfgmapmonster;

	int32_t m_killerLevel;  // ɱ���ߵĵȼ�,�����ǳ���ĵȼ�
	UnitHandle m_killer;
	UnitHandle m_target;
	CfgSkill *m_pCfgSkill;
	int32_t m_firstDamageTime;
	int64_t m_lastActionTick;

	MonsterState m_state;
	int64_t m_standTick;
	int64_t m_standTime;
	int64_t m_corpseTick;
	int64_t m_ghostTick;
	int64_t m_attackTick;
	int64_t m_attackTime;
	int8_t	m_DieType;
	FamilyId_t	m_nFamilyId;
	int32_t			m_lastUpdateMinute;
	std::set<CharId_t> HurtCharId;
	bool	m_CanReverHp;
	int64_t m_LastReverHpTick;
	int32_t m_ReviveTime;
	int32_t	m_lastUpdateWarVictoryBossMinute;
	int64_t	m_liftTime;	// ������ʱ(ms)
};

