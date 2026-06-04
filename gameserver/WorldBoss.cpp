#include "WorldBoss.h"
#include "GameService.h"
#include "MapManager.h"
#include "Monster.h"
#include "ActivityManager.h"
#include "Timer.h"
#include "DayTime.h"
#include "DBService.h"
using namespace Answer;


CWorldBoss::CWorldBoss()
{
	m_BossMap.clear();
	m_WarVictoryBoss = NULL;
	m_lastUpdateTick = 0;
}

CWorldBoss::~CWorldBoss()
{

}

void CWorldBoss::Init()
{
	BossInfoMap BossMap = CFG_DATA.GetBossInfoMap();
	BossInfoMap::iterator it = BossMap.begin();
	for ( ; it != BossMap.end(); ++it )
	{
		CfgMapMonster* pCfgMapMonster = CFG_DATA.GetMapMonsterInfo(it->first);
		if ( NULL == pCfgMapMonster )
		{
			continue;
		}

		CfgMonster* pMonster = CFG_DATA.getMonster( pCfgMapMonster->monsterid );
		if ( NULL == pMonster )
		{
			continue;
		}
		WorldBossInfo stu;
		stu.BossId			 = it->first;
		stu.Mid				 = pMonster->mid;
		stu.BossType		 = pMonster->boss_sign;
		stu.MapId			 = pCfgMapMonster->mapid;
		stu.DieTime			 = 0;
		if ( stu.BossType == BOSS_TYPE_SPIDER_QUEEN )
		{
			stu.ReviveTimes	 = 0;
		}
		else
		{
			int32_t ReviveTime = GetBossRevieTime( stu.BossId );
			if ( ReviveTime <= 0 )
			{
				stu.ReviveTimes	 = pMonster->revive_time / 1000;
			}
			else
			{
				stu.ReviveTimes	 = ReviveTime;
			}
		}
		m_BossMap[it->first] = stu;
	}
}

int32_t CWorldBoss::GetBossRevieTime( int32_t BossId )
{
	BossInfo* pBossInfo = CFG_DATA.GetBossInfo( BossId );
	if ( NULL == pBossInfo )
	{
		return 0;
	}
	if ( pBossInfo->m_NeedResetReviveTime <= 0 )
	{
		return 0;
	}
	int32_t DiffDay = CFG_DATA.getServerDiffTime();
	switch ( DiffDay )
	{
	case 0:
		{
			return 5 * 60;
		}
	case 1:
		{
			return 10 * 60;
		}
	case 2:
		{
			return 20 * 60;
		}
	case 3:
		{
			return 30 * 60;
		}
	case 4:
		{
			return 40 * 60;
		}
	case 5:
		{
			return 50 * 60;
		}
	default:
		return 0;
	}
}

Position CWorldBoss::GetRevivePos( int32_t BossId )
{
	Position Pos;
	BossInfo* pBossInfo = CFG_DATA.GetBossInfo( BossId );
	if ( NULL == pBossInfo )
	{
		return Pos;
	}
	std::vector<Position> PosVector = pBossInfo->m_RevivePosVector;
	std::random_shuffle( PosVector.begin(), PosVector.end());
	if ( PosVector.begin() == PosVector.end() )
	{
		return Pos;
	}
	return *(PosVector.begin());
}

void CWorldBoss::PacketBossInfo( Answer::NetPacket *packet, int8_t BossType )
{
	if ( NULL == packet )
	{
		return;
	}
	int32_t nSize = 0;
	int32_t NowTime = Answer::DayTime::now();
	tm	NowLocal	= Answer::DayTime::localnow();
	int32_t OldOffset = packet->getWOffset();
	packet->writeInt32( nSize );
	packet->writeInt8( BossType );
	BoosMap::iterator it = m_BossMap.begin();
	for ( ; it != m_BossMap.end(); ++it )
	{
		if ( it->second.BossType == BossType  )
		{
			BossInfo* pBossInfo = CFG_DATA.GetBossInfo( it->first );
			if ( NULL == pBossInfo || pBossInfo->m_IsShow <= 0 )
			{
				continue;
			}
			packet->writeInt32( it->second.BossId );
			packet->writeInt32( it->second.Mid );
			packet->writeInt32( it->second.MapId );
			if ( BossType == BOSS_TYPE_SPIDER_QUEEN )
			{
				int32_t LeftTime = 0;
				if ( it->second.DieTime > 0 )
				{
					int32_t Seconds = NowLocal.tm_hour * 60 * 60 + NowLocal.tm_min * 60 + NowLocal.tm_sec;
					LeftTime = it->second.ReviveTimes * 60 - Seconds; 
					if ( LeftTime < 0 )
					{
						LeftTime = it->second.ReviveTimes * 60 - Seconds + 86400;
					}
				}
				packet->writeInt32( LeftTime );
			}
			else
			{
				packet->writeInt32( it->second.DieTime > 0 ? (it->second.DieTime + it->second.ReviveTimes - NowTime) : 0 );
			}
			packet->writeInt64( it->second.KillerId );
			packet->writeUTF8( it->second.KillerName );
			nSize++;
		}
	}
	int32_t NewOffset = packet->getWOffset();
	packet->setWOffset( OldOffset );
	packet->writeInt32( nSize );
	packet->setWOffset( NewOffset );
}

void CWorldBoss::PacketBossHomeInfo( Answer::NetPacket *packet, int32_t MapId )
{
	if ( NULL == packet )
	{
		return;
	}
	int32_t nSize = 0;
	int32_t NowTime = Answer::DayTime::now();
	int32_t OldOffset = packet->getWOffset();
	packet->writeInt32( nSize );
	packet->writeInt8( BOSS_TYPE_BOSS_HOME );
	BoosMap::iterator it = m_BossMap.begin();
	for ( ; it != m_BossMap.end(); ++it )
	{
		if ( it->second.BossType == BOSS_TYPE_BOSS_HOME && it->second.MapId == MapId )
		{
			packet->writeInt32( it->second.BossId );
			packet->writeInt32( it->second.Mid );
			packet->writeInt32( it->second.MapId );
			packet->writeInt32( it->second.DieTime > 0 ? (it->second.DieTime + it->second.ReviveTimes - NowTime) : 0 );
			packet->writeInt64( it->second.KillerId );
			packet->writeUTF8( it->second.KillerName );
			nSize++;
		}
	}
	int32_t NewOffset = packet->getWOffset();
	packet->setWOffset( OldOffset );
	packet->writeInt32( nSize );
	packet->setWOffset( NewOffset );
}

void CWorldBoss::UpdateBossInfo( int32_t BossId, int32_t DieTime, CharId_t KillerId, std::string KillerName , int32_t ReviveTime)  //dieTime = 0 ��ʾ����
{
	BoosMap::iterator it = m_BossMap.find( BossId );
	if ( it != m_BossMap.end() )
	{
		m_BossMap[BossId].DieTime = DieTime;
		if ( KillerId > 0 )
		{
			m_BossMap[BossId].KillerId		= KillerId;
			m_BossMap[BossId].KillerName	= KillerName;
		}
		if ( DieTime > 0 )
		{
			if ( it->second.BossType == BOSS_TYPE_SPIDER_QUEEN )
			{
				it->second.ReviveTimes = ReviveTime;
				if ( IsAllSpiderQueenDie() )
				{
					ACTIVITY_MANAGER.StopSqiderQueen();
				}
			}
			else
			{
				int32_t ReviveTime = GetBossRevieTime( it->first );
				if ( ReviveTime > 0 )
				{
					it->second.ReviveTimes	 = ReviveTime;
				}
			}

		}
		
		if ( it->second.BossType == BOSS_TYPE_SPIDER_QUEEN || it->second.BossType == BOSS_TYPE_WORLD_BOSS )
		{
			if ( KillerId > 0 )
			{
				GongGao( DieTime, it->second.Mid, BossId, it->second.KillerName, it->second.MapId, KillerId );
			}
		}
		if ( it->second.BossType == BOSS_TYPE_BOSS_HOME )
		{
			int32_t NowTime = Answer::DayTime::now();
			Map* pMap = MAP_MANAGER.GetMap( it->second.MapId);
			if ( NULL != pMap )
			{
				NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_SEND_BOSS_CHANGE);
				if (NULL == packet)
				{
					return;
				}
				packet->writeInt32( it->second.BossId );
				packet->writeInt32( it->second.Mid );
				packet->writeInt32( it->second.MapId );
				packet->writeInt32( it->second.DieTime > 0 ? (it->second.DieTime + it->second.ReviveTimes - NowTime) : 0 );
				packet->writeInt64( it->second.KillerId );
				packet->writeUTF8( it->second.KillerName );
				packet->setSize( packet->getWOffset() );
				pMap->broadcast( packet );
			}
		}
	}
}

void CWorldBoss::GongGao( int32_t DieTime, int32_t Mid, int32_t BossId, std::string KillerName, int32_t MapId, CharId_t KillerId )
{
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_SEND_BOSS_GONG_GAO);
	if (NULL == packet)
	{
		return;
	}
	if ( DieTime > 0 )		
	{
		packet->writeInt8( 0 );		//0 ��ʾ����
		packet->writeInt32( Mid );
		packet->writeUTF8( KillerName );
		packet->writeInt64( KillerId );
	}
	else	
	{
		packet->writeInt8( 1 );		//1 ��ʾ����
		packet->writeInt32( Mid );
		packet->writeInt32( BossId );
		packet->writeInt32( MapId );
	}
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.broadcast( packet );
}

void CWorldBoss::GetWorldBossIcon( IconStateList& IconList )
{
	ShowIcon stu = {};
	stu.nId			= JI_ZHAN_BOSS;
	stu.nState		= AS_RUNNING;
	stu.nLeftTime	= -1;
	IconList.push_back( stu );
}

void CWorldBoss::SendWorldBossIcon( Player *pPlayer)
{
	if ( NULL == pPlayer )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_SEND_ONE_ICON);
	if (NULL == packet)
	{
		return;
	}
	ShowIcon stu = {};
	stu.nId			= JI_ZHAN_BOSS;
	stu.nState		= AS_RUNNING;
	stu.nLeftTime	= -1;

	packet->writeInt32( stu.nId );
	packet->writeInt8( stu.nState );
	packet->writeInt32( stu.nLeftTime );
	packet->writeInt8( stu.IconLeft );
	packet->writeInt32( stu.IconRight );
	packet->writeInt8( stu.Effects );
	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.sendPacketTo(pPlayer->getGateIndex(), packet);	
}

void CWorldBoss::AddMonst( Monster* pMonster )
{
	if ( NULL == pMonster )
	{
		return;
	}
	m_MonsterList.push_back( pMonster );
}

bool CWorldBoss::IsInHuoDongTiem()
{
	tm LocalNow = DayTime::localnow();
	int32_t Minute = LocalNow.tm_hour * 60 + LocalNow.tm_min;
	for ( int32_t i = 0; i < ReviveTimeCount; i++ )
	{
		if ( Minute >= ReviveTime[i] && Minute <= ReviveTime[i] + REVIVE_TIME )
		{
			return true;
		}
	}
	return false;
}

int32_t CWorldBoss::GetSpiderQueenReviveTime()
{
	tm LocalNow = DayTime::localnow();
	int32_t Minute = LocalNow.tm_hour * 60 + LocalNow.tm_min;
	for ( int32_t i = 0; i < ReviveTimeCount; i++ )
	{
		if ( ReviveTime[i] > Minute )
		{
			return ReviveTime[i];
		}
	}
	return ReviveTime[0];
}

bool CWorldBoss::IsAllSpiderQueenDie()
{
	MonsterList::iterator it = m_MonsterList.begin();
	for ( ; it != m_MonsterList.end(); ++it )
	{
		if ( (*it)->GetHP() > 0 )
		{
			return false;
		}
	}
	return true;
}

void CWorldBoss::KillAllSpiderQueen()
{
	MonsterList::iterator it = m_MonsterList.begin();
	for ( ; it != m_MonsterList.end(); ++it )
	{
		if ( (*it)->GetHP() > 0 )
		{
			Map* pMap = (*it)->getMap();
			if ( NULL != pMap )
			{
				MAP_MANAGER.PostMsg( pMap->GetRunnerId(), GMC_KILL_MONST, pMap, *it );
			}
		}
	}
}

bool CWorldBoss::IsWarVictoryBossTime()
{
	WarVictoryHd* pWarVictory = CFG_DATA.GetWarVictoryHdCfg( 1 );
	if ( NULL == pWarVictory )
	{
		return false;
	}
	int32_t NowTime =  Answer::DayTime::now();
	if ( NowTime < pWarVictory->StartTime || NowTime > pWarVictory->EndTime )
	{
		return false;
	}
	tm LocalNow = DayTime::localnow();
	int32_t Minute = LocalNow.tm_hour * 60 + LocalNow.tm_min;
	for ( int32_t i = 0; i < WarVictoryBossReviveTimeCount; i++ )
	{
		if ( Minute >= WarVictoryBossReviveTime[i] && Minute <= WarVictoryBossReviveTime[i] + WAR_VICTORY_BOSS_REVIVE_TIME )
		{
			return true;
		}
	}
	return false;
}

void CWorldBoss::AddWarVictoryBoss( Monster* pMonster )
{
	if ( NULL == pMonster )
	{
		return;
	}
	m_WarVictoryBoss = pMonster;
}

void CWorldBoss::KillWarVictoryBoss()
{
	if ( NULL == m_WarVictoryBoss )
	{
		return;
	}

	if ( m_WarVictoryBoss->GetHP() > 0 )
	{
		Map* pMap = m_WarVictoryBoss->getMap();
		if ( NULL != pMap )
		{
			MAP_MANAGER.PostMsg( pMap->GetRunnerId(), GMC_KILL_MONST, pMap, m_WarVictoryBoss );
			GongGao( BCI_WAR_VICTORY_BOSS_LEAVE );
		}
	}
}

void CWorldBoss::Update( int64_t CurTick )
{
	if ( CurTick - m_lastUpdateTick > 1000 )
	{
		m_lastUpdateTick = CurTick;
	}
	if ( NULL == m_WarVictoryBoss )
	{
		return;
	}
	if ( !IsWarVictoryBossTime() )
	{
		KillWarVictoryBoss();
	}
}

void CWorldBoss::GongGao( int32_t GongGaoId, Player* pPlayer )
{
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM );
	if (NULL == packet)
	{
		return;
	}

	packet->writeInt32( GongGaoId );
	if ( GongGaoId ==  BCI_WAR_VICTORY_BOSS_KILLED )
	{
		if ( pPlayer != NULL )
		{
			packet->writeUTF8( pPlayer->getName() );
			packet->writeInt64( pPlayer->getCid() );
		}
		else
		{
			packet->writeUTF8( "" );
			packet->writeInt64( 0 );
		}
	}

	packet->setSize(packet->getWOffset());
	packet->setProc( SM_SEND_NOTICE_PARAM );
	GAME_SERVICE.worldBroadcast(packet);
}
