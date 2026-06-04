#include "stdafx.h"

#include "Dungeon.h"
#include "GameService.h"
#include "MonsterDungeon.h"
#include "PoolManager.h"

MonsterDungeon::MonsterDungeon()
	: m_dungeon(NULL), m_delFlag( false )
{

}

MonsterDungeon::~MonsterDungeon()
{

}

void MonsterDungeon::refresh()
{
	int64_t curHp = GetHP();
	int64_t maxHp = GetMaxHP();
	int32_t hpPercent = roundInt( curHp * 100.0f / maxHp );
	for ( HPEventList::iterator it = m_eventHp.begin(); it != m_eventHp.end(); ++it )
	{
		if ( hpPercent >= it->minhp && hpPercent < it->maxhp )
		{
			m_dungeon->onMonsterHPEvent( this, it->id );
		}
	}
	Monster::refresh();
}

bool MonsterDungeon::isDungeonMonster()
{
	return true;
}

//int32_t MonsterDungeon::getOwner() const
//{
//	if ( NULL == m_dungeon )
//	{
//		return 0;
//	}
//
//	if ( getSide() == 1 )
//	{
//		Player* player = m_dungeon->GetLeader();
//		if ( player != NULL )
//		{
//			return player->getCid();
//		}
//	}
//
//	return 0;
//}

void MonsterDungeon::init(Dungeon *dungeon, const CfgDungeonMonster &cfgDungeonMonster, const CfgMonster &cfgmonster, const CfgMapMonster &cfgmapmonster, Buff *pBuff)
{
	if (NULL == dungeon)
	{
		return;
	}
	m_dungeon = dungeon;
	m_cfgDungeonMonster = cfgDungeonMonster;
	if ( m_cfgDungeonMonster.road.size() > 0 )
	{
		m_road = m_cfgDungeonMonster.road;
	}
	if ( m_cfgDungeonMonster.life > 0 )
	{
		SetLifeTime( getNow() + m_cfgDungeonMonster.life );
	}

	AttrAddonVector vAttrAddon;
	Monster::init(cfgmonster, cfgmapmonster, MS_STAND, &vAttrAddon);
}

void MonsterDungeon::reset()
{
	Monster::reset();

	generateEntityId();

	m_dungeon = NULL;
	m_eventHp.clear();
	m_road.clear();
	m_delFlag = false;
}

int32_t MonsterDungeon::getWave() const
{
	return m_cfgDungeonMonster.wave;
}

void  MonsterDungeon::addEventHp( const HPEvent& hpEvent )
{
	m_eventHp.push_back(hpEvent);
}

int32_t MonsterDungeon::getSide() const
{
	return m_cfgDungeonMonster.side;
}

int32_t MonsterDungeon::getDungeonMid() const
{
	return m_cfgDungeonMonster.id;
}

int32_t MonsterDungeon::getDungeonId() const
{
	return m_dungeon->getDungeonId();
}

int32_t MonsterDungeon::getMoney() const
{
	return m_cfgDungeonMonster.money;
}

void MonsterDungeon::die()
{
	Monster::die();
	m_dungeon->onMonsterDie(this);
}

void MonsterDungeon::onCorpse()
{
	Monster::onCorpse();
	if ( getState() == MS_GHOST && m_dungeon != NULL && m_cfgmonster.revive_time < 0 )
	{
		m_delFlag = true;;
	}
}

bool MonsterDungeon::needDel() const
{
	return m_delFlag;
}

void MonsterDungeon::destroy()
{
	POOL_MANAGER.push<MonsterDungeon>( this );
}

void MonsterDungeon::remove()
{
	Monster::leaveMap();
	GAME_SERVICE.removeMonster( this );
	m_delFlag = true;
}

void MonsterDungeon::onRunOnRoad()
{

}

void MonsterDungeon::onArriveTarget()
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
		if ( m_dungeon != NULL )
		{
			m_dungeon->onMonsterArriveRoadEnd( this );
		}
	}
}

void MonsterDungeon::runOnRoad()
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

void MonsterDungeon::postDamage(int32_t damge, UnitHandle launcher)
{
	if ( m_dungeon )
	{
		Player* player = NULL;
		if ( launcher.type == ET_PLAYER )
		{
			player = GAME_SERVICE.getPlayer( launcher.id, GetRunnerId() );
		}
		else if ( launcher.type == ET_PET )
		{
			CObjPet* pPet = GAME_SERVICE.getPet( launcher.id, GetRunnerId() );
			if ( pPet != NULL )
			{
				player = pPet->GetPlayer();
			}
		}
		if ( player != NULL )
		{
			m_dungeon->AddPlayerDamage( player->getCid(), damge );
		}
	}
	Monster::postDamage( damge, launcher );
}

int8_t MonsterDungeon::getDungeonHard() const
{
	return 0;
}

int8_t MonsterDungeon::getDungeonQuality() const
{
	return 0;
}

