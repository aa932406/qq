#include "stdafx.h"

#include "ActivityMap.h"
#include "PoolManager.h"
#include "Map.h"
#include "MonsterActivity.h"
#include "SkillBuff.h"
#include "GameService.h"

using namespace Answer;

MonsterActivity::MonsterActivity()
: m_pActivityMap( NULL ), m_nOwner(0), m_delFlag( false ), m_nFamilyMoney( 0 ), m_nFamilyMoneyTick( 0 )
{
}

MonsterActivity::~MonsterActivity()
{

}

void MonsterActivity::refresh()
{
	if ( NULL == m_pActivityMap )
	{
		return;
	}

	int64_t curTick = m_pActivityMap->getTick();

	checkHpEvent( curTick );
	checkFamilyMoney( curTick );

	Monster::refresh();
}

CharId_t MonsterActivity::getOwner()
{
	return m_nOwner;
}

void MonsterActivity::setOwner( CharId_t owner )
{
	m_nOwner = owner;
}

void MonsterActivity::setKiller( CharId_t nKiller )
{
	m_killer.id = nKiller;
	m_killer.type = 1;
}

void MonsterActivity::init( CActivityMap *pActMap, const CfgActivityMonster &cfgActivityMonster, const CfgMonster &cfgmonster, const CfgMapMonster &cfgmapmonster )
{
	if ( NULL == pActMap )
	{
		return;
	}

	m_pActivityMap = pActMap;
	m_cfgActivityMoster = cfgActivityMonster;
	if ( m_cfgActivityMoster.road.size() > 0)
	{
		m_road = m_cfgActivityMoster.road;
	}

	if ( m_cfgActivityMoster.left > 0 )
	{
		SetLifeTime( getNow() + m_cfgActivityMoster.left );
	}

	AttrAddonVector vAttrAddon;
	Monster::init(cfgmonster, cfgmapmonster, MS_STAND, &vAttrAddon);

	if ( m_cfgActivityMoster.buff > 0 )
	{
		CfgBuff *pCfgBuff = CFG_DATA.getBuff( m_cfgActivityMoster.buff );
		if ( pCfgBuff != NULL )
		{
			SkillBuff *pBuff = new SkillBuff( *this, *pCfgBuff );
			UnitHandle launcher;
			launcher.id = getUnitId();
			launcher.type = getType();
			if ( pBuff->init( 0, 0, launcher, launcher ) )
			{
				addBuff( pBuff );
			}
		}
	}
}

void MonsterActivity::reset()
{
	Monster::reset();

	generateEntityId();

	m_pActivityMap = NULL;
	m_hpEvents.clear();
	m_road.clear();
	m_delFlag = false;

	m_nFamilyId = 0;
	m_nFamilyMoney = 0;
	m_nFamilyMoneyTick = 0;
}

int32_t MonsterActivity::getWave() const
{
	return m_cfgActivityMoster.wave;
}

int32_t MonsterActivity::getSide() const
{
	return m_cfgActivityMoster.side;
}

int32_t MonsterActivity::getActivityMonsterId() const
{
	return m_cfgActivityMoster.id;
}

void  MonsterActivity::addEventHp( const HPEvent& hpEvent )
{
	m_hpEvents.push_back( hpEvent );
}

bool MonsterActivity::isActivityMonster()
{
	return true;
}

void MonsterActivity::die()
{
	Monster::die();
	m_pActivityMap->onMonsterDie( this );
	m_nFamilyMoney = 0;
}

void MonsterActivity::onCorpse()
{
	Monster::onCorpse();
	if ( getState() == MS_GHOST && m_pActivityMap != NULL && m_cfgmonster.revive_time < 0 )
	{
		m_delFlag = true;
	}
}

int32_t MonsterActivity::GetHurtIncreace() const
{
	if ( getActivityMonsterId() == ACTICITY_MONSTER_ID_FAMILY_WAR_PILLAR )
	{
		if ( m_nFamilyMoney <= 0 )
		{
			return 10;
		}
	}
	return 1;
}

void MonsterActivity::broadcastBasicData()
{
	if (m_pMap != NULL)
	{
		int8_t nflag = m_bDie ? 1 : 0;
		NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_UNIT_BASIC_DATA);
		if (NULL == packet)
		{
			return;
		}
		packet->writeInt64(getUnitId());
		packet->writeInt8(getType());
		packet->writeInt32( GetHP() );
		packet->writeInt32( GetMaxHP() );
		packet->writeInt16( GetMoveSpeed() );
		packet->writeInt8( nflag );
		packet->writeInt32(getDeadTime());
		packet->setSize(packet->getWOffset());
		if ( m_cfgActivityMoster.id == ACTICITY_MONSTER_ID_FAMILY_WAR_STON || m_cfgActivityMoster.id == ACTICITY_MONSTER_ID_FAMILY_WAR_PILLAR )	// ս��֮����ʥս��ʯ
		{
			m_pMap->broadcast( packet );
		}
		else
		{
			m_pMap->broadcastAreaAround( packet, this );
		}
	}
}

bool MonsterActivity::needDel() const
{
	return m_delFlag;
}

void MonsterActivity::destroy()
{
	POOL_MANAGER.push<MonsterActivity>( this );
}

void MonsterActivity::remove()
{
	leaveMap();
	GAME_SERVICE.removeMonster( this );
	m_delFlag = true;
}

void MonsterActivity::onRunOnRoad()
{

}

void MonsterActivity::postDamage(int32_t damge, UnitHandle launcher)
{
	Monster::postDamage( damge, launcher );
	if ( m_pActivityMap != NULL )
	{
		m_pActivityMap->onMonsterDamaged( this, damge, launcher );
	}
}

void MonsterActivity::onArriveTarget()
{
	if ( getState() != MS_RUN_ON_ROAD )
	{
		return;
	}

	if ( !m_road.empty() )
	{
		Position curPos = getCurrentTile();
		Position tarPos = m_road.front();
		m_road.pop_front();
		while ( curPos == tarPos )
		{
			tarPos = m_road.front();
			m_road.pop_front();
			if ( m_road.empty() )
			{
				break;
			}
		}

		setTargetTile( tarPos.x, tarPos.y );
		broadcastMove();
	}
	else
	{
		setState( MS_STAND );
		if ( m_pActivityMap != NULL )
		{
			m_pActivityMap->onMonsterArriveRoadEnd( this );
		}
	}
}

void MonsterActivity::runOnRoad()
{
	if ( !m_road.empty() )
	{
		m_target.clear();
		Position curPos = getCurrentTile();
		Position tarPos = m_road.front();
		m_road.pop_front();
		while ( curPos == tarPos )
		{
			tarPos = m_road.front();
			m_road.pop_front();
			if ( m_road.empty() )
			{
				break;
			}
		}

		setTargetTile( tarPos.x, tarPos.y );
		broadcastMove();
		setState( MS_RUN_ON_ROAD );
	}
}

void MonsterActivity::checkHpEvent( int64_t curTick )
{
	if ( NULL == m_pActivityMap )
	{
		return;
	}
	int32_t	curHp = GetHP();
	int32_t	maxHp = GetMaxHP();
	int32_t	hpPercent = roundInt( curHp * 100.0f / maxHp );
	for ( HPEventList::iterator it = m_hpEvents.begin();it != m_hpEvents.end();++it)
	{
		if ( hpPercent >= it->minhp && hpPercent < it->maxhp )
		{
			m_pActivityMap->onMonsterHPEvent( this, it->id );
		}
	}
}

int32_t MonsterActivity::GetFamilyMoney() const
{
	return m_nFamilyMoney;
}

int32_t MonsterActivity::AddFamilyMoney( int32_t nMoney )
{
	if ( !isAlive() )
	{
		return 0;
	}
	if ( m_nFamilyId == 0 )
	{
		return 0;
	}

	m_nFamilyMoney += nMoney;
	return m_nFamilyMoney;
}

void MonsterActivity::checkFamilyMoney( int64_t curTick )
{
	if ( curTick - m_nFamilyMoneyTick < 5000 )
	{
		return;
	}

	if ( !isAlive() )
	{
		return;
	}

	if ( m_nFamilyMoney <= 0 )
	{
		return;
	}

	if ( GetHP() == GetMaxHP() )
	{
		return;
	}

	int32_t needHp = GetMaxHP() - GetHP();
	if ( needHp > 10000 )
	{
		needHp = 10000;
	}

	int32_t costMoney = ( needHp % 100 == 0 ) ? ( needHp / 100 ) : ( needHp / 100 + 1 );
	if ( costMoney > m_nFamilyMoney )
	{
		costMoney = m_nFamilyMoney;
	}

	m_nFamilyMoney -= costMoney;
	AddHP( costMoney * 100 );
	m_nFamilyMoneyTick = curTick;
}
