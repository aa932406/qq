#include "stdafx.h"
#include "Trap.h"
#include "ActivityManager.h"
#include "ActivityMap.h"
#include "GameService.h"
#include "MonsterActivity.h"
#include "PlantActivity.h"
#include "DBService.h"
#include "MonsterBuff.h"
#include "PoolManager.h"
#include "Timer.h"
#include "NpcActivity.h"
using namespace Answer;
using namespace std;
CActivityMap::CActivityMap()
:m_pActivity( NULL ), m_nStartTick( 0 )
{
}

CActivityMap::~CActivityMap()
{
}

void CActivityMap::init( const CfgMap &cfgmap )
{
	Map::init( cfgmap );
	clear();
}

void CActivityMap::clear()
{
	IMapEvent::clear();
	//m_pActivity = NULL;	// ��ʱ����
	m_nStartTick = 0;
	if ( !m_actMonsters.empty() )
	{
		for ( ActivityMonsterList::iterator iter = m_actMonsters.begin(); iter != m_actMonsters.end(); ++iter )
		{
			MonsterActivity* pMonster = *iter;
			if ( pMonster != NULL )
			{
				pMonster->remove();
			}
		}
		m_actMonsters.clear();
	}
	if ( !m_actPlants.empty() )
	{
		for ( ActivityPlantList::iterator iter = m_actPlants.begin(); iter != m_actPlants.end(); ++iter )
		{
			PlantActivity* plant = *iter;
			if ( plant != NULL )
			{
				removePlant( plant->getEntityId() );
				POOL_MANAGER.push<PlantActivity>( plant );
			}
		}
		m_actPlants.clear();
	}
	if ( !m_traps.empty() )
	{
		for ( TrapList::iterator iter = m_traps.begin(); iter != m_traps.end(); ++iter )
		{
			Trap* pTrap = *iter;
			if ( pTrap != NULL )
			{
				pTrap->leaveMap();
				POOL_MANAGER.push<Trap>( pTrap );
			}
		}
		m_traps.clear();
	}
}

bool CActivityMap::IsActivityMap() const
{
	if ( m_cfgmap.isType( MT_NORMAL ) )
	{
		if ( NULL == m_pActivity )
		{
			return false;	// �δ��ʼ
		}
	}
	return true;
}

void CActivityMap::update()
{
	Map::update();
	if ( m_pActivity != NULL )
	{
		m_pActivity->OnUpdate( this );
		if ( m_pActivity->IsRuning() )
		{
			int64_t curTick = TIMER.GetTick();
			checkEvents();
			checkAddMonster( curTick );
		}
	}
}

void CActivityMap::checkEvents()
{
	for ( CfgMapEventList::iterator it = m_events.begin(); it != m_events.end(); ++it )
	{
		checkEvent( *it );
	}
}

int32_t CActivityMap::canEnter(Player *player)
{
	if ( NULL == player )
	{
		return ERR_INVALID_DATA;
	}

	if ( m_pActivity != NULL )
	{
		return m_pActivity->canEnter( player, this );
	}
	else if ( (GetType() & MT_NORMAL) != 0 )
	{
		return Map::canEnter( player );
	}

	return ERR_MAP_ACTIVITY_NOT_STARTED;
}

int32_t CActivityMap::getPkMode() const
{
	int32_t pkMode = PK_MODE_FREE;
	if ( m_pActivity != NULL )
	{
		pkMode = m_pActivity->getPkMode();
	}

	if ( pkMode == PK_MODE_FREE )
	{
		pkMode = Map::getPkMode();
	}

	return pkMode;
}

bool CActivityMap::canRevive()
{
	if ( m_pActivity != NULL )
	{
		return m_pActivity->canRevive();
	}

	return Map::canRevive();
}

int32_t CActivityMap::getReive( Player* player )
{
	if ( m_pActivity != NULL )
	{
		// new v2: skip activity revive for type 2 and 14
		int16_t nType = m_pActivity->GetType();
		if ( nType != 2 && nType != 14 )
		{
			return m_pActivity->GetRevive( player );
		}
	}

	return Map::getReive( player );
}

void CActivityMap::removePlayer( Player *player, bool islogout )
{
	// new v2: call activity removePlayer BEFORE Map::removePlayer
	if ( m_pActivity != NULL )
	{
		m_pActivity->removePlayer( player, islogout );
	}
	Map::removePlayer( player, islogout );

	// new v2: clean up battle tracking
	m_CidBattle.erase( player->getCid() );
}

void CActivityMap::addPlayer( Player *player, int32_t x, int32_t y )
{
	Map::addPlayer( player, x, y );
	if ( m_pActivity != NULL )
	{
		m_pActivity->addPlayer( player );
	}

	// new v2: track player battle power
	m_CidBattle[player->getCid()] = player->getBattle();
}

bool CActivityMap::OnSitRevive( Player* player )
{
	if ( m_pActivity != NULL && m_pActivity->IsRuning() )
	{
		return m_pActivity->OnSitRevive( player );
	}
	return Map::OnSitRevive( player );
}

bool CActivityMap::CanUseXP() const
{
	if ( m_pActivity != NULL && m_pActivity->IsRuning() )
	{
		return m_pActivity->CanUseXP();
	}
	return Map::CanUseXP();
}

bool CActivityMap::CanUsePet() const
{
	if ( m_pActivity != NULL && m_pActivity->IsRuning() )
	{
		return m_pActivity->CanUsePet( GetId() );
	}
	return Map::CanUsePet();
}

void CActivityMap::onActivityStart( CActivity* pActivity )
{
	if ( NULL == pActivity )
	{
		return;
	}
	m_pActivity = pActivity;
	IMapEvent::init( CFG_DATA.getActivityEvents( pActivity->GetId(), GetId() ) );
	m_nStartTick = TIMER.GetTick();
}

void CActivityMap::onActivityStop( CActivity* pActivity )
{
	clear();
}

// new v2 functions

bool CActivityMap::CanSitRevive()
{
	if ( m_pActivity != NULL )
	{
		return m_pActivity->IsRuning();
	}
	return Map::canRevive();
}

bool CActivityMap::SpecialSitRevive()
{
	// TODO: Original pseudocode delegated to CActivity vtable function (offset 58).
	// Need to implement proper logic when the corresponding CActivity function exists.
	return false;
}

int32_t CActivityMap::GetTop10Battle()
{
	std::vector<int32_t> battleVt;
	for ( std::map<int64_t, int32_t>::iterator iter = m_CidBattle.begin(); iter != m_CidBattle.end(); ++iter )
	{
		battleVt.push_back( iter->second );
	}

	std::sort( battleVt.begin(), battleVt.end() );

	int32_t count = 0;
	int32_t sum = 0;
	for ( std::vector<int32_t>::reverse_iterator it = battleVt.rbegin(); it != battleVt.rend() && count < 10; ++it )
	{
		sum += *it;
		++count;
	}

	return ( count > 0 ) ? ( sum / count ) : 0;
}

int32_t CActivityMap::HaveAliveMonster()
{
	int32_t nCount = 0;
	for ( ActivityMonsterList::iterator iter = m_actMonsters.begin(); iter != m_actMonsters.end(); ++iter )
	{
		MonsterActivity* pMonster = *iter;
		if ( pMonster != NULL && pMonster->isAlive() )
		{
			++nCount;
		}
	}
	return nCount;
}

int32_t CActivityMap::HaveAlivePet()
{
	// Note: Original pseudocode was truncated. Checking alive pets in the map.
	int32_t nCount = 0;
	PlayerList tList = m_players;
	for ( PlayerList::iterator iter = tList.begin(); iter != tList.end(); ++iter )
	{
		Player* pPlayer = *iter;
		if ( pPlayer != NULL )
		{
			nCount += pPlayer->GetCharPet().GetAliveFightPetCount();
		}
	}
	return nCount;
}

void CActivityMap::onPlayerDie(Player *pDier)
{

}

// ɱ����
void CActivityMap::onPlayerKilled( Player* pDier, Player *pKiller )
{
	if ( NULL == m_pActivity || !m_pActivity->IsRuning() )
	{
		return;
	}

	m_pActivity->onPlayerKilled( pDier, pKiller );
}

//��������
void CActivityMap::onMonsterDie( MonsterActivity *monster )
{
	if ( NULL == m_pActivity || NULL == monster || !m_pActivity->IsRuning() )
	{
		return;
	}

	m_pActivity->onMonsterDie( monster );
	
	Int32MonsterWaveMap::iterator it = m_monsterWave.find( monster->getWave() );
	if ( it != m_monsterWave.end() )
	{
		it->second.killed++;
	}

	for (CfgMapEventList::iterator it = m_events.begin(); it != m_events.end(); ++it )
	{
		if ( it->bOpen && !it->bDone )
		{
			if ( it->trigger_type == AETT_MONSTER_WAVE )
			{
				checkEvent( *it );
			}
			else if (it->trigger_type == AETT_MONSTER_ID)
			{
				if ( it->trigger_param[0] == monster->getActivityMonsterId() )
				{
					triggerEvent( *it, monster );
				}
			}
		}
	}
}

void CActivityMap::onMonsterHPEvent( MonsterActivity *monster, int32_t id )
{
	if ( monster == NULL )
	{
		return;
	}

	for ( CfgMapEventList::iterator it = m_events.begin(); it != m_events.end(); ++it )
	{
		if ( it->bOpen && !it->bDone && it->trigger_id == id )
		{
			triggerEvent( *it, monster );
		}
	}
}

//���ﱻ��ɱ
void CActivityMap::onMonsterDie(Monster *monster, Player *player)
{
	Map::onMonsterDie(monster, player);
	MonsterActivity* pMonster = dynamic_cast<MonsterActivity*>( monster );
	if ( m_pActivity != NULL && pMonster != NULL )
	{
		m_pActivity->onMonsterDie( pMonster, player );
	}
}

int32_t CActivityMap::onBeginGather(  Plant* plant, Player *player )
{
	if ( m_pActivity == NULL )
	{
		return ERR_SYETEM_ERR;
	}
	return m_pActivity->onBeginGather( plant, player );
}

void CActivityMap::onPlantGather( Plant* plant, Player *player )
{
	if ( plant == NULL )
	{
		return;
	}
	if ( m_pActivity == NULL )
	{
		return;
	}
	m_pActivity->onPlantGather( plant, player );
	for ( CfgMapEventList::iterator it = m_events.begin(); it != m_events.end(); ++it )
	{
		if ( it->bOpen && !it->bDone && it->trigger_type == AETT_PLANT_GATHER && it->trigger_param[0] == plant->getPlantId() )
		{
			triggerEvent( *it );
		}
	}
}

void CActivityMap::onMonsterArriveRoadEnd( MonsterActivity *monster )
{
}

void CActivityMap::onMonsterDamaged( MonsterActivity* pMonster, int32_t nDamage, const UnitHandle& launcher )
{
	if ( NULL == m_pActivity )
	{
		return;
	}

	if ( !m_pActivity->IsRuning() )
	{
		return;
	}

	Player* pAttacker = NULL;
	if ( launcher.type == ET_PLAYER )
	{
		pAttacker = GAME_SERVICE.getPlayer( launcher.id, GetRunnerId() );
	}
	else if ( launcher.type == ET_PET )
	{
		CObjPet* pPet = GAME_SERVICE.getPet( launcher.id, GetRunnerId() );
		if ( pPet != NULL )
		{
			pAttacker = pPet->GetPlayer();
		}
	}
	if ( pAttacker != NULL )
	{
		m_pActivity->onMonsterDamaged( pMonster, nDamage, pAttacker );
	}
}

Position CActivityMap::GetBornPos( Player* player )
{
	if ( m_pActivity != NULL )
	{
		return m_pActivity->GetRandBornPos( player );
	}
	return Position( -1, -1 );
}

CActivity*	CActivityMap::GetActivity()
{
	return m_pActivity;
}

void CActivityMap::checkEvent( CfgMapEvent &mapEvent )
{
	if ( !mapEvent.bOpen || mapEvent.bDone )
	{
		return;
	}

	switch ( mapEvent.trigger_type )
	{
	case AETT_TIME_LINE: //ʱ�䵽��
		{
			if ( !mapEvent.trigger_param.empty() && getTick() - m_nStartTick >= mapEvent.trigger_param[0]*1000 )
			{
				triggerEvent( mapEvent );
			}
		}
		break;
	case AETT_MULTI_MONSTER_WAVE:
		{
			if ( mapEvent.trigger_param.size() > 1 )
			{
				int32_t nsize = mapEvent.trigger_param.size();
				if ( nsize > 0 )
				{
					int32_t  count  = 0;
					for ( int32_t i = 0; i < count; ++i )
					{
						Int32MonsterWaveMap::iterator it = m_monsterWave.find(mapEvent.trigger_param[i]);
						if ( it != m_monsterWave.end() && !it->second.alive() )
						{
							++count;
						}
					}

					if ( count == nsize )
					{
						triggerEvent( mapEvent );
					}
				}
			}
		}
		break;
	case AETT_MONSTER_WAVE_TIMELINE:
		{
			if (mapEvent.trigger_param.size() == 2)
			{
				Int32MonsterWaveMap::iterator it = m_monsterWave.find(mapEvent.trigger_param[0]);
				if ( it != m_monsterWave.end() )
				{
					if ( !( it->second.alive() || getTick() - it->second.startTime > mapEvent.trigger_param[1]*1000 ) )
					{
						triggerEvent( mapEvent );
					}
				}
			}			
		}
		break;
	case AETT_ARRIVE_AREA:
		{
			if( mapEvent.trigger_param.size() == 4 )
			{
				Position ps1( mapEvent.trigger_param[0], mapEvent.trigger_param[1] );
				Position ps2( mapEvent.trigger_param[2], mapEvent.trigger_param[3] );
				for ( PlayerList::iterator it = m_players.begin();it != m_players.end();++it )
				{
					Player* pPlayer = *it;
					if ( pPlayer != NULL && pPlayer->isInRectangle( ps1, ps2 ) )
					{
						triggerEvent( mapEvent, pPlayer );
					}
				}
			}
		}
		break;
	default:
		break;
	}
}

void CActivityMap::triggerEvent( CfgMapEvent &mapEvent, Unit* pUnit )
{
	if ( NULL == m_pActivity )
	{
		return;
	}
	switch( mapEvent.event_type )
	{
	case AEET_ADD_MONSTER:
		generateMonster( mapEvent.effect );
		break;
	case AEET_ADD_TRAP:
		{
			StringVector strIds = StringUtility::split(mapEvent.effect, ":");
			for (StringVector::iterator it = strIds.begin(); it != strIds.end(); ++it)
			{
				CfgActivityTrap *pCfgActivityTrap = CFG_DATA.getActivityTrap( atoi( it->c_str() ) );
				if ( pCfgActivityTrap != NULL)
				{
					CfgTrap *pCfgTrap = CFG_DATA.getTrap( pCfgActivityTrap->tid );
					if (pCfgTrap != NULL)
					{
						Position trapPos( pCfgActivityTrap->x, pCfgActivityTrap->y );
						if ( pUnit != NULL )
						{
							trapPos = pUnit->getCurrentTile();
						}
						if ( trapPos.x > 0 && trapPos.y > 0 )
						{
							Trap *pTrap = POOL_MANAGER.pop<Trap>();
							if (NULL == pTrap)
							{
								continue;
							}
							pTrap->init( this, trapPos, *pCfgTrap );
							addTrap(pTrap);
						}
					}
				}
			}
		}
		break;
 	case AEET_ADD_PLANT:
		{
			for ( ActivityPlantList::iterator iter = m_actPlants.begin(); iter != m_actPlants.end(); )
			{
				PlantActivity* pPlant = *iter;
				if ( pPlant->GetPlantType() != PT_GRADE_BAO_XIANG )
				{
					pPlant->broadcastRemove();
					removePlant( pPlant->getEntityId() );
					iter = m_actPlants.erase( iter );
				}
				else 
				{
					++iter;
				}
			}
			StringVector strIds = StringUtility::split(mapEvent.effect, ":");
			for (StringVector::iterator it = strIds.begin(); it != strIds.end(); ++it)
			{
				CfgActivityPlant* pCfgActivityPlan= CFG_DATA.getActivityPlant( atoi(it->c_str() ) );
				if ( pCfgActivityPlan == NULL )
				{
					break;
				}

				CfgPlant *pCfgPlant = CFG_DATA.getPlant(pCfgActivityPlan->plant_id);
				if (pCfgPlant == NULL)
				{
					continue;
				}
				if ( pCfgActivityPlan->region_id > 0 )
				{
					CfgMapRegion *pCfgRegion = CFG_DATA.getMapRegion(pCfgActivityPlan->region_id);
					if (pCfgRegion != NULL && pCfgRegion->mapid == GetId())
					{
						for (int32_t i = 0; i < pCfgActivityPlan->count; ++i)
						{
							Position pos = getRandomWalkablePositionInRegion(*pCfgRegion);
							if (pos.x > 0 && pos.y > 0)
							{
								CfgMapPlant mapPlant;
								mapPlant.mapid = GetId();
								mapPlant.plantid = pCfgPlant->id;
								mapPlant.x = pos.x;
								mapPlant.y = pos.y;

								PlantActivity *plant = POOL_MANAGER.pop<PlantActivity>();
								if ( NULL == plant )
								{
									continue;
								}
								plant->init( this, *pCfgPlant, mapPlant );
								addPlant( plant );
								m_actPlants.push_back( plant );
								m_pActivity->AddPlant( plant );
							}
						}
					}
				}
				else
				{
					std::vector<Position> PosVector = pCfgActivityPlan->EnterPosVector;
					int32_t nSize = PosVector.size();
					if ( nSize <= 0 )
					{
						break;
					}
					std::random_shuffle( PosVector.begin(), PosVector.end());

					for (int32_t i = 0; i < pCfgActivityPlan->count && i < nSize; ++i)
					{
						Position pos = PosVector[i];
						if (pos.x > 0 && pos.y > 0)
						{
							CfgMapPlant mapPlant;
							mapPlant.mapid = GetId();
							mapPlant.plantid = pCfgPlant->id;
							mapPlant.x = pos.x;
							mapPlant.y = pos.y;

							PlantActivity *plant = POOL_MANAGER.pop<PlantActivity>();
							if ( NULL == plant )
							{
								continue;
							}
							plant->init( this, *pCfgPlant, mapPlant );
							addPlant( plant );
							m_actPlants.push_back( plant );
							m_pActivity->AddPlant( plant );
						}
					}
				}
				
			}
		}
		break;
	case AEET_ACTIVE_EVENT:
		{
			Int32List events;
			StringVector strIds = StringUtility::split(mapEvent.effect, ":");
			for (StringVector::iterator it = strIds.begin();it != strIds.end();++it)
			{
				events.push_back( atoi( it->c_str() ) );
			}
			openEvents( events );
		}
		break;
	case AEET_REMOVE_MONSTER:
		{
			StringVector strIds = StringUtility::split(mapEvent.effect, ":");
			for (StringVector::iterator it = strIds.begin();it != strIds.end();++it)
			{
				for ( ActivityMonsterList::iterator iter = m_actMonsters.begin(); iter != m_actMonsters.end();++iter )
				{
					MonsterActivity* pMonster = *iter;
					if ( pMonster != NULL && pMonster->getActivityMonsterId() == atoi( it->c_str() ) )
					{
						pMonster->remove();
						m_actMonsters.erase( iter );
						break;
					}
				}
			}
		}
		break;
	case AEET_ADD_STACK:
		{
			PosList stacks;
			StringVector strstacks = StringUtility::split(mapEvent.effect, "|");
			for ( StringVector::iterator it = strstacks.begin();it != strstacks.end();++it )
			{
				StringVector vstack = StringUtility::split( *it, ":" );
				if ( vstack.size() == 2 )
				{
					stacks.push_back( Position( atoi( vstack[0].c_str() ), atoi( vstack[1].c_str() ) ) );
				}
			}
			AddStack( stacks );
		}
		break;
	case AEET_REMOVE_STACK:
		{
			PosList stacks;
			StringVector strstacks = StringUtility::split(mapEvent.effect, "|");
			for ( StringVector::iterator it = strstacks.begin();it != strstacks.end();++it )
			{
				StringVector vstack = StringUtility::split( *it, ":" );
				if ( vstack.size() == 2 )
				{
					stacks.push_back( Position( atoi( vstack[0].c_str() ), atoi( vstack[1].c_str() ) ) );
				}
			}
			RemoveStack( stacks );
		}
		break;
	case AEET_BROADCAST:
		{
			Monster* pMonster = dynamic_cast<Monster*>( pUnit );
			if ( pMonster != NULL )
			{
				Player* pKiller = pMonster->getKiller();
				if ( pKiller != NULL )
				{
					int32_t bcid = atoi( mapEvent.effect.c_str() );
					Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM );
					if ( packet != NULL )
					{
						packet->writeInt32( bcid );
						packet->writeUTF8( pKiller->getName() );
						packet->writeInt64( pKiller->getCid() );
						packet->setSize(packet->getWOffset());
						GAME_SERVICE.worldBroadcast(packet);
					}
				}
			}
		}
		break;
	default:break;
	}

	mapEvent.bDone = true;
}

void CActivityMap::generateMonster( const std::string &str )
{
	StringVector strIds = StringUtility::split(str, ":");
	for (StringVector::iterator it = strIds.begin(); it != strIds.end(); ++it)
	{
		MonsterWait stu = {};
		stu.id = atoi( it->c_str() );
		m_waitMonster.push_back( stu );
	}
}

void CActivityMap::checkAddMonster( int64_t curTick )
{
	MonsterWaitList::iterator iter = m_waitMonster.begin();
	MonsterWaitList::iterator eiter = m_waitMonster.end();
	for ( ; iter != eiter; )
	{
		if ( flashMonster( *iter, curTick ) )
		{
			++iter;
		}
		else
		{
			m_waitMonster.erase( iter++ );
		}
	}
}

bool CActivityMap::flashMonster( MonsterWait& waitAddMonster, int64_t curTick )
{
	CfgActivityMonster *pCfgActivityMonster = CFG_DATA.getActivityMonster( waitAddMonster.id );
	if ( NULL == pCfgActivityMonster )
	{
		return false;
	}

	if ( pCfgActivityMonster->count < 0 )
	{
		return false;
	}

	CfgMonster *pCfgMonster = CFG_DATA.getMonster( pCfgActivityMonster->mid );
	if ( NULL == pCfgMonster )
	{
		return false;
	}

	if ( curTick - waitAddMonster.lastTick < pCfgActivityMonster->delay )
	{
		return true;
	}

	++waitAddMonster.times;
	waitAddMonster.lastTick = curTick;

	if ( pCfgActivityMonster->count == 1 )
	{
		flashMonster( pCfgActivityMonster, pCfgMonster, pCfgActivityMonster->x, pCfgActivityMonster->y );
	}
	else
	{
		std::vector<Position> positions;
		for (int32_t x = -2; x < 2; ++x)
		{
			for (int32_t y = -2; y < 2; ++y)
			{
				Position pos( pCfgActivityMonster->x+x, pCfgActivityMonster->y+y);
				if ( isWalkablePosition( pos.x, pos.y ) )
				{
					positions.push_back(pos);
				}
			}
		}

		random_shuffle(positions.begin(), positions.end());

		for ( int32_t i = 0; i < pCfgActivityMonster->count && i < static_cast<int32_t>(positions.size()); ++i )
		{
			flashMonster( pCfgActivityMonster, pCfgMonster, positions[i].x, positions[i].y );
		}
	}

	if ( waitAddMonster.times >= pCfgActivityMonster->times )
	{
		return false;	// ˢ����б���ɾ��
	}
	return true;
}

void CActivityMap::AddActivityNpc( string& Effect )
{	
	if ( NULL == m_pActivity )
	{
		return;
	}
	StringVector strIds = StringUtility::split(Effect, ":");
	int32_t EndTime = m_pActivity->getLeftTime();
	for (StringVector::iterator it = strIds.begin(); it != strIds.end(); ++it)
	{
		CfgActivityNpc* pNpcAct = CFG_DATA.getActivityNpc( atoi((*it).c_str()) );
		if ( NULL == pNpcAct )
		{
			continue;
		}
		Int32Vector regionIds = pNpcAct->region_id;
		if ( regionIds.size() <= 0 )
		{
			continue;
		}
		random_shuffle(regionIds.begin(), regionIds.end());
		CfgMapRegion *pCfgRegion = CFG_DATA.getMapRegion(regionIds[0]);
		if (pCfgRegion != NULL && pCfgRegion->mapid == GetId())
		{
			for ( int32_t i = 0; i < pNpcAct->count; i++ )
			{
				Position pos = getRandomWalkablePositionInRegion(*pCfgRegion);
				if ( pos.x <= 0 || pos.y <= 0 )
				{
					continue;
				}
				const CfgNpc* pNpc = CFG_DATA.getNpc( pNpcAct->npc_id );
				if ( NULL == pNpc )
				{
					break;
				}
				NpcActivity *npc = new NpcActivity;
				if (NULL == npc)
				{
					continue;
				}
				npc->init(*pNpc,m_pActivity->GetId(),EndTime);
				addNpc(npc, pos.x,pos.y);	
			}
		}
	}
}

void CActivityMap::flashMonster( CfgActivityMonster* pCfgActivityMonster, CfgMonster *pCfgMonster, int32_t x, int32_t y, MonsterBuff* pMonsterBuff )
{
	if ( NULL == m_pActivity )
	{
		return;
	}
	MonsterActivity *pMonster = POOL_MANAGER.pop<MonsterActivity>();
	if ( NULL == pMonster )
	{
		return;
	}

	CfgMapMonster cfgmapmonster = {};
	cfgmapmonster.mapid = GetId();
	cfgmapmonster.monsterid = pCfgActivityMonster->mid;
	cfgmapmonster.x = x;
	cfgmapmonster.y = y;

	pMonster->init( this, *pCfgActivityMonster, *pCfgMonster, cfgmapmonster, pMonsterBuff );
	m_pActivity->onMonsterAdd( pMonster );
	addMonster( pMonster, cfgmapmonster.x, cfgmapmonster.y );
	addMonsterHPEventInfo( pMonster );
	pMonster->runOnRoad();

	Int32MonsterWaveMap::iterator itMonsterWave = m_monsterWave.find( pCfgActivityMonster->wave );
	if (itMonsterWave != m_monsterWave.end())
	{
		itMonsterWave->second.count++;
	}
	else
	{
		MonsterWave monsterWave = {};
		monsterWave.count = 1;
		monsterWave.startTime = getTick();
		m_monsterWave.insert( Int32MonsterWaveMap::value_type( pCfgActivityMonster->wave, monsterWave ) );
	}

	m_actMonsters.push_back( pMonster );
}

void CActivityMap::addMonsterHPEventInfo( MonsterActivity* pMonster )
{
	if (NULL == pMonster)
	{
		return;
	}
	for (CfgMapEventList::iterator it = m_events.begin(); it != m_events.end(); ++it)
	{
		if (it->trigger_type == AETT_HP_PERCENT && it->trigger_param[0] ==HPET_MONSTER  && it->trigger_param.size() == 4)
		{
			if ( it->trigger_param[1]== pMonster->getActivityMonsterId() &&  it->trigger_param[2] > 0 && it->trigger_param[3] > 0 )
			{
				HPEvent hpEvent = {};
				hpEvent.id		= it->trigger_id;
				hpEvent.minhp	= it->trigger_param[2];
				hpEvent.maxhp	= it->trigger_param[3];

				pMonster->addEventHp( hpEvent );
			}	    
		}
	}
}
