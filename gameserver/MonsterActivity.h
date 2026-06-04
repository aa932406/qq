#pragma once

#include "Monster.h"

class CActivityMap;
class MonsterActivity
	: public Monster
{
public:
	MonsterActivity();
	virtual ~MonsterActivity();

public:
	virtual CharId_t getOwner();
	virtual void	refresh();
	virtual void	onArriveTarget();
	virtual int32_t getSide() const;
	virtual void	onRunOnRoad();
	virtual bool	needDel() const;
	virtual void	destroy();
	virtual void	postDamage(int32_t damge, UnitHandle launcher);
	virtual int32_t	GetHurtIncreace() const;
	virtual bool	isActivityMonster();

public:
	void	init( CActivityMap *pActivityMap, const CfgActivityMonster &cfgActivityMonster, const CfgMonster &cfgmonster, const CfgMapMonster &cfgmapmonster, Buff *pBuff );
	void	reset();

	void	setOwner( CharId_t owner );
	void	setKiller( CharId_t nKiller );
	int32_t getWave() const;
	int32_t getActivityMonsterId() const;
	void	runOnRoad();
	void	addEventHp( const HPEvent& hpEvent );
	void	remove();

	int32_t	AddFamilyMoney( int32_t nMoney );
	int32_t	GetFamilyMoney() const;

protected:
	virtual void	die();
	virtual void	onCorpse();
	virtual void	broadcastBasicData();

private:
	void	checkHpEvent( int64_t curTick );
	void	checkFamilyMoney( int64_t curTick );

private:
	CfgActivityMonster	m_cfgActivityMoster; 
	CActivityMap*		m_pActivityMap;
	HPEventList			m_hpEvents;
	PosList				m_road;
	CharId_t			m_nOwner;
	bool				m_delFlag;

	// ����ս
	int32_t				m_nFamilyMoney;
	int64_t				m_nFamilyMoneyTick;
};


