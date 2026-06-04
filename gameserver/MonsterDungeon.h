#pragma once

#include "Monster.h"

class Dungeon;

class MonsterDungeon
	: public Monster
{
public:
	MonsterDungeon();
	virtual ~MonsterDungeon();

public:
	virtual void	refresh();
	virtual void	onArriveTarget();
	virtual int32_t getSide() const;
	virtual void	onRunOnRoad();
	virtual bool	needDel() const;
	virtual void	destroy();
	virtual bool	isDungeonMonster();
	virtual void	postDamage(int32_t damge, UnitHandle launcher);
	
public:
	void init(Dungeon *dungeon, const CfgDungeonMonster &cfgDungeonMonster, const CfgMonster &cfgmonster, const CfgMapMonster &cfgmapmonster, Buff *pBuff);
	void reset();
	
	int32_t getWave() const;
	int32_t getDungeonMid() const;
	int32_t getDungeonId() const;
	void	runOnRoad();
	void	addEventHp( const HPEvent& hpEvent );
	void	remove();
	int32_t	getMoney() const;
	int8_t	getDungeonHard() const;
	int8_t	getDungeonQuality() const;

protected:
	virtual void	die();
	virtual void	onCorpse();

private:
	Dungeon*			m_dungeon;
	CfgDungeonMonster	m_cfgDungeonMonster;
	HPEventList			m_eventHp;
	PosList				m_road;
	bool				m_delFlag;
};

