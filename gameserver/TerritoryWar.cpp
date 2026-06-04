
#include "TerritoryWar.h"
#include "GameService.h"
#include "DBService.h"
#include "Timer.h"
#include "DayTime.h"
#include "ActivityMap.h"
#include "ActivityManager.h"

#define MAX_REVIVE_TIMES	3		// �������

using namespace Answer;

CTerritoryWar::CTerritoryWar( const CfgActivity& cfgActivity, bool& special )
:CActivity( cfgActivity ), m_nLastTick( 0 ), m_bSpecialSign( false ), m_nTerritoryState( TWS_NOT_START )
{
	if ( special )
	{
		m_bSpecialSign = true;
		special = false;
	}
}

CTerritoryWar::~CTerritoryWar()
{

}

void CTerritoryWar::Init()
{
	CActivity::Init();

	MySqlDBGuard db(DBPOOL);

	CharIdList winners;
	char mySql[1024] = {0};
	snprintf( mySql, 1023, "SELECT * FROM `mem_territory_war` WHERE `actid`=%d ORDER BY `time` DESC LIMIT 1", GetId() );
	MySqlQuery result = db.query( mySql );
	std::string strWinners = result.getStringValue("winners");
	OnTerritoryWarResult( strWinners );
}

void CTerritoryWar::OnUpdate( CActivityMap* pMap )
{
	CActivity::OnUpdate( pMap );
	if ( NULL == pMap )
	{
		return;
	}

	if ( m_nState != AS_RUNNING )
	{
		return;
	}

	int64_t curTick = pMap->getTick();
	if ( curTick - m_nLastTick > 1000 )
	{
		m_nLastTick = curTick;

		int32_t nNowTime = TIMER.GetNow();

		if ( m_nTerritoryState == TWS_READY )
		{
			int32_t startMinute = m_cfgActivity.start_hour[0];
			for ( uint32_t i = 0; i < m_cfgActivity.start_hour.size(); ++i )
			{
				int32_t tend = DayTime::dayzero( nNowTime ) + ( m_cfgActivity.start_hour[i] + m_cfgActivity.duration ) * 60;
				if ( tend > nNowTime )
				{
					startMinute = m_cfgActivity.start_hour[i];
					break;
				}
			}
			int32_t startTime = DayTime::dayzero( nNowTime ) + startMinute * 60;
			if ( nNowTime - startTime > 5 * 60 )
			{
				m_nTerritoryState = TWS_START;
				changePKMode();
				setJoiner();
				checkWin();
				broadcastTerritoryStart();
			}
		}
	}
}

void CTerritoryWar::changePKMode()
{
	PlayerList::iterator iter = m_players.begin();
	PlayerList::iterator eiter = m_players.end();
	for ( ; iter != eiter; ++iter )
	{
		Player* player = *iter;
		if ( player != NULL )
		{
			if( player->IsInTeam() )
			{
				player->setPkMode( PK_MODE_TEAM, false );
			}
			else
			{
				player->setPkMode( PK_MODE_ALL, false );
			}
		}
	}
}

void CTerritoryWar::setJoiner()
{
	PlayerList::iterator iter = m_players.begin();
	PlayerList::iterator eiter = m_players.end();
	for ( ; iter != eiter; ++iter )
	{
		Player* player = *iter;
		if ( player != NULL )
		{
			m_lstJoiner.push_back( player->getCid() );
		}
	}
}

void CTerritoryWar::reset()
{
	CActivity::reset();

	m_lstJoiner.clear();
	m_nLastTick = 0;
	m_nTerritoryState = TWS_NOT_START;
}

bool CTerritoryWar::checkData()
{
	int32_t startDays = TIMER.GetDaysFromStart();	// �������Ϸ�������쿪��
	if ( startDays == 1 )
	{
		return true;
	}
	else if ( startDays < 1 )
	{
		return false;
	}
	else
	{
		return CActivity::checkData();
	}
}

bool CTerritoryWar::checkWeek()
{
	int32_t startDays = TIMER.GetDaysFromStart();	// �������Ϸ�������쿪��
	if ( startDays == 1 )
	{
		return true;
	}
	else if ( startDays < 1 )
	{
		return false;
	}
	else
	{
		return CActivity::checkWeek();
	}
}

void CTerritoryWar::onPlayerKilled( Player* pDier, Player* pAttacker )
{
	checkWin();
}

void CTerritoryWar::addPlayer( Player* player )
{
	CActivity::addPlayer( player );
	if ( NULL == player )
	{
		return;
	}

	// ����PKģʽ
	player->setPkMode( PK_MODE_PEACE, false );

	// ���ø������
	player->GetOperateLimit().Reset( PR_TERRITORY_WAR_REVIVE_TIMES );

	// �뿪����
	player->GetCharTeam().LeaveTeam();
}

void CTerritoryWar::removePlayer( Player* player, bool islogout )
{
	CActivity::removePlayer( player, islogout );
	checkWin();
}

void CTerritoryWar::SendPlayerActivityInfo( Player* player )
{
	if ( NULL == player )
	{
		return;
	}

	NetPacket* packet = GAME_SERVICE.popNetpacket( PACK_DISPATCH, SM_NOTIFY_ACTIVITY_INFO );
	if ( NULL == packet )
	{
		return;
	}

	int32_t memberCount = m_players.size();

	packet->writeInt32( m_cfgActivity.id );
	packet->writeInt32( memberCount );					// ��������
	packet->writeInt32( getActivityTime() );			// ʱ��
	packet->setSize( packet->getWOffset() );

	GAME_SERVICE.sendPacketTo( player->getGateIndex(), packet );
}


void CTerritoryWar::SendPlayerActivityScore( Player* player, int32_t nLeftTime )
{
	if ( NULL == player )
	{
		return;
	}

	NetPacket* packet = GAME_SERVICE.popNetpacket( PACK_DISPATCH, SM_NOTIFY_ACTIVITY_SCORE );
	if ( NULL == packet )
	{
		return;
	}

	packet->writeInt32( m_cfgActivity.id );
	packet->writeInt32( getLeftReviveTimes( player ) );
	packet->writeInt32( nLeftTime );

	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.sendPacketTo( player->getGateIndex(), packet );

	//NetPacket* playerPacket = packetActivityPlayerScore( player );
	//if ( playerPacket != NULL )
	//{
	//	GAME_SERVICE.sendPacketTo( player->getGateIndex(), playerPacket );
	//}
}

Position CTerritoryWar::GetRandBornPos( Player* player )
{
	if ( NULL == player )
	{
		return Position( -1, -1 );
	}

	return getBornRandPosA();
}

bool CTerritoryWar::OnSitRevive( Player* player )
{
	return false;
}

int32_t CTerritoryWar::GetRevive( Player* player )
{
	if ( NULL == player )
	{
		return CActivity::GetRevive( player );
	}

	if ( player->GetOperateLimit().GetLimitCount( PR_TERRITORY_WAR_REVIVE_TIMES ) >= MAX_REVIVE_TIMES )
	{
		player->leaveActivity();
		return 0;
	}

	player->GetOperateLimit().AddLimitCount( PR_TERRITORY_WAR_REVIVE_TIMES, 1 );
	SendPlayerActivityScore( player, getLeftTime() );
	return m_cfgActivity.target_regiona[0];
}

int32_t CTerritoryWar::getLeftReviveTimes( Player* player )
{
	if ( NULL == player )
	{
		return 0;
	}

	return MAX_REVIVE_TIMES - player->GetOperateLimit().GetLimitCount( PR_TERRITORY_WAR_REVIVE_TIMES );
}

void CTerritoryWar::win( TeamMemberList& winners )
{
	// ����
	m_winners = winners;
	broadcastWin();

	m_nState = AS_END;
	m_nTerritoryState = TWS_END;
	saveTerritoryWarResult();
	addRewards();
	broadcastActivityResult();
	BroadcastActivityState();
	delayKickAll( 30 );
	stopActivity();
	GAME_SERVICE.broadcastHuoDongDaTingIcon();
}

void CTerritoryWar::addRewards()
{
	for ( CharIdList::iterator iter = m_lstJoiner.begin(); iter != m_lstJoiner.end(); ++iter )
	{
		MemChrBag bagItem;
		bagItem.itemClass	= IC_NORMAL;
		bagItem.itemCount	= 1;
		bagItem.bind		= IBS_BIND;
		if ( isDoubleReward() )
		{
			bagItem.itemCount	= 2;
		}
		if ( isWinner( *iter ) )
		{
			bagItem.itemId = m_cfgActivity.gift_id[0];
		}
		else
		{
			bagItem.itemId = m_cfgActivity.gift_id[1];
		}
		DB_SERVICE.OnSendSysMail( *iter, MI_TERRITORY_WAR_REWARD, bagItem );
	}
}

void CTerritoryWar::saveTerritoryWarResult()
{
	int32_t nTime = DayTime::dayzero( TIMER.GetNow() ) + m_cfgActivity.start_hour[0] * 60;
	std::stringstream ss;
	for ( TeamMemberList::iterator iter = m_winners.begin(); iter != m_winners.end(); ++iter )
	{
		ss << iter->nCharId;
		ss << ":";
		ss << iter->strName;
		ss << "|";
	}
	DB_SERVICE.SaveTerritoryWarResult( GetId(), ss.str(), nTime );
}

bool CTerritoryWar::isWinner( CharId_t cid )
{
	for ( TeamMemberList::iterator iter = m_winners.begin(); iter != m_winners.end(); ++iter )
	{
		if ( iter->nCharId == cid )
		{
			return true;
		}
	}
	return false;
}

void CTerritoryWar::broadcastActivityResult()
{
	CharIdList winList;
	CharIdList failList;
	for ( CharIdList::iterator iter = m_lstJoiner.begin(); iter != m_lstJoiner.end(); ++iter )
	{
		if ( isWinner( *iter ) )
		{
			winList.push_back( *iter );
		}
		else
		{
			failList.push_back( *iter );
		}
	}

	if ( !winList.empty() )
	{
		NetPacket* packet = GAME_SERVICE.popNetpacket( PACK_DISPATCH, SM_NOTIFY_ACTIVITY_RESULT );
		if ( NULL == packet )
		{
			return;
		}

		packet->writeInt32( m_cfgActivity.id );
		packet->writeInt8( 1 );

		packet->setSize( packet->getWOffset() );
		GAME_SERVICE.broadcast( packet, winList );
	}

	if ( !failList.empty() )
	{
		NetPacket* packet = GAME_SERVICE.popNetpacket( PACK_DISPATCH, SM_NOTIFY_ACTIVITY_RESULT );
		if ( NULL == packet )
		{
			return;
		}

		packet->writeInt32( m_cfgActivity.id );
		packet->writeInt8( 0 );

		packet->setSize( packet->getWOffset() );
		GAME_SERVICE.broadcast( packet, failList );
	}
}

void CTerritoryWar::onActivityStart()
{
	m_winners.clear();
	m_lstJoiner.clear();
	m_nTerritoryState = TWS_READY;
}

void CTerritoryWar::onTimeEnd()
{
	Player* winner = NULL;
	PlayerList::iterator iter = m_players.begin();
	PlayerList::iterator eiter = m_players.end();
	for ( ; iter != eiter; ++iter )
	{
		Player* player = *iter;
		if ( player != NULL )
		{
			if ( player->isAlive() || getLeftReviveTimes( player ) > 0 )
			{
				if ( NULL == winner )
				{
					winner = player;
					continue;
				}

				if ( player->getBattle() > winner->getBattle() )
				{
					winner = player;
				}
			}
		}
	}

	if ( winner != NULL )
	{
		TeamMemberList winners;
		if ( winner->IsInTeam() )
		{
			winners = winner->GetCharTeam().GetTeamMembers();
		}
		else
		{
			winners.push_back( TeamMember( winner->getCid(), winner->getName(), winner->getGateIndex() ) );
		}
		win( winners );
	}

	CActivity::onTimeEnd();
}

int32_t	CTerritoryWar::getNextStartTime()
{
	if ( IsRuning() || m_cfgActivity.start_hour.empty() )
	{
		return 0;
	}

	int32_t nNowTime = TIMER.GetNow();
	const tm& localnow	= TIMER.GetLocalNow();
	int32_t nowMinute	= localnow.tm_hour * 60 + localnow.tm_min;
	int32_t startMinute = m_cfgActivity.start_hour[0];
	for ( uint32_t i = 0; i < m_cfgActivity.start_hour.size(); ++i )
	{
		int32_t startTime = DayTime::dayzero( nNowTime ) + m_cfgActivity.start_hour[i] * 60;
		if ( startMinute > nNowTime )
		{
			startMinute = m_cfgActivity.start_hour[i];
			break;
		}
	}

	int32_t days = -1;
	int32_t startDays = TIMER.GetDaysFromStart();	// �������Ϸ�������쿪��
	if ( checkData() && checkWeek() )
	{
		if( nowMinute < startMinute )
		{
			days = 0;
		}
	}
	else if ( startDays < 1 )
	{
		days = 1 - startDays;
	}

	if ( days < 0 )
	{
		if ( m_cfgActivity.weekday.empty() )
		{
			return 0;
		}
		int32_t weekday = TIMER.GetWeekDay();
		int32_t nextweekday = 0;

		Int32Vector::const_iterator iter = m_cfgActivity.weekday.begin();
		for ( ; iter != m_cfgActivity.weekday.end(); ++iter )
		{
			if ( weekday == *iter )
			{
				++iter;
				break;
			}
		}

		if ( iter == m_cfgActivity.weekday.end() )
		{
			nextweekday = m_cfgActivity.weekday.front();
		}
		else
		{
			nextweekday = *iter;
		}

		if ( nextweekday < weekday )
		{
			days = nextweekday + 7 - weekday;
		}
		else
		{
			days = nextweekday - weekday;
		}
	}

	int32_t time = DayTime::dayzero( nNowTime );
	time += days * 24 * 60 * 60 + startMinute*60;
	return time;
}
//
//Answer::NetPacket* CTerritoryWar::packetActivityScore()
//{
//	return packetActivityFamilyScore();
//}
//
//Answer::NetPacket* CTerritoryWar::packetActivityFamilyScore()
//{
//	NetPacket* packet = GAME_SERVICE.popNetpacket( PACK_DISPATCH, SM_NOTIFY_ACTIVITY_FAMILY_SCORE );
//	if ( NULL == packet )
//	{
//		return NULL;
//	}
//
//	//packet->writeInt32( m_cfgActivity.id );
//	//if ( m_pPillar != NULL )
//	//{
//	//	FamilyInfo familyInfo = FAMILY_MANAGER.GetFamilyInfo( m_pPillar->GetFamilyId() );
//	//	packet->writeInt64( familyInfo.nId );
//	//	packet->writeUTF8( familyInfo.strName );
//	//}
//	//else
//	//{
//	//	packet->writeInt64( 0 );
//	//	packet->writeUTF8( "" );
//	//}
//
//	//int8_t nCount = 0;
//	//uint32_t offset = packet->getWOffset();
//	//packet->writeInt8( nCount );
//	//for ( FamilyScoreList::iterator iter = m_lstFamilyScore.begin(); iter != m_lstFamilyScore.end(); ++iter )
//	//{
//	//	packet->writeInt64( iter->nFamilyId );
//	//	packet->writeUTF8( iter->strFamilyName );
//	//	packet->writeInt32( iter->nScore );
//	//	++nCount;
//	//	if ( nCount >= 4 )
//	//	{
//	//		break;
//	//	}
//	//}
//
//	//uint32_t oldOffset = packet->getWOffset();
//	//packet->setWOffset( offset );
//	//packet->writeInt8( nCount );
//	//packet->setWOffset( oldOffset );
//
//	//packet->setSize( packet->getWOffset() );
//	return packet;
//}
//
//Answer::NetPacket* CTerritoryWar::packetActivityPlayerScore( Player* player )
//{
//	if ( NULL == player )
//	{
//		return NULL;
//	}
//
//	PlayerScoreMap::iterator iter = m_mPlayerScore.find( player->getCid() );
//	if ( iter == m_mPlayerScore.end() )
//	{
//		return NULL;
//	}
//
//	const PlayerScore& score = iter->second;
//	NetPacket* packet = GAME_SERVICE.popNetpacket( PACK_DISPATCH, SM_NOTIFY_ACTIVITY_PLAYER_SCORE );
//	if ( NULL == packet )
//	{
//		return NULL;
//	}
//
//	packet->writeInt32( m_cfgActivity.id );
//	packet->writeInt32( score.nExp );
//	packet->writeInt32( score.nMoney );
//	packet->writeInt32( score.nKillCount );
//
//	packet->setSize( packet->getWOffset() );
//	return packet;
//}

int32_t CTerritoryWar::GiveDailyReward( Player* player )
{
	if ( NULL == player )
	{
		return ERR_INVALID_DATA;
	}

	TeamMemberList::iterator iter = m_winners.begin();
	for ( ; iter != m_winners.end(); ++iter )
	{
		if ( player->getCid() == iter->nCharId )
		{
			break;
		}
	}

	if ( iter == m_winners.end() )
	{
		return ERR_INVALID_DATA;
	}

	int32_t limitId = PR_ACTIVITY_DAILY_REAWRD_START + GetId();
	if ( player->GetOperateLimit().CheckCountIsLimited( limitId, 1 ) )
	{
		return ERR_INVALID_DATA;
	}

	MemChrBag addItem;
	addItem.itemClass	= IC_NORMAL;
	addItem.itemId		= m_cfgActivity.daily_reward[0];
	addItem.itemCount	= 1;
	addItem.bind		= IBS_BIND;
	if ( m_winners.size() == 1 )
	{
		addItem.itemCount = 2;
	}

	if ( !player->GetBag().AddItem( addItem, IACR_ACTIVITY ) )
	{
		return ERR_INVALID_DATA;
	}

	player->GetOperateLimit().AddLimitCount( limitId, 1 );
	player->GetPlayerDailyActivity().SendHuoDongDaTingIcon();
	return ERR_OK;
}

bool CTerritoryWar::CanUseXP() const
{
	return false;
}

//void CTerritoryWar::broadcastReady()
//{
//	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM );
//	if (NULL == packet)
//	{
//		return;
//	}
//	packet->writeInt32( BCI_WORLD_FAMILYWAR_READY );
//	packet->setSize(packet->getWOffset());
//	GAME_SERVICE.worldBroadcast(packet);
//}

void CTerritoryWar::broadcastStart()
{
	if ( !m_bSpecialSign )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM );
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32( BCI_TERRITORY_ACTIVITY_START );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CTerritoryWar::broadcastTerritoryStart()
{
	if ( !m_bSpecialSign )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM );
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32( BCI_TERRITORY_BATTLE_READY );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast(packet);
}

void CTerritoryWar::broadcastWin()
{
	if ( m_winners.empty() )
	{
		return;
	}
	else if ( m_winners.size() == 1 )
	{
		Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM );
		if (NULL == packet)
		{
			return;
		}
		packet->writeInt32( BCI_TERRITORY_WIN_SINGLE );
		packet->writeUTF8( m_winners.front().strName );
		packet->writeInt32( GetId() );
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.worldBroadcast(packet);
	}
	else
	{
		Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM );
		if (NULL == packet)
		{
			return;
		}
		packet->writeInt32( BCI_TERRITORY_WIN_TEAM );
		packet->writeUTF8( m_winners.front().strName );
		packet->writeInt32( GetId() );
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.worldBroadcast(packet);
	}
}

bool CTerritoryWar::isDoubleReward() const
{
	int32_t startDays = TIMER.GetDaysFromStart();	// �������Ϸ���ڶ���˫������
	return startDays <= 1;
}

bool CTerritoryWar::checkWin()
{
	if ( m_nState != AS_RUNNING )
	{
		return false;
	}

	if ( m_nTerritoryState != TWS_START )
	{
		return false;
	}

	int32_t teamid = 0;
	int32_t member = 0;
	TeamMemberList winners;
	PlayerList::iterator iter = m_players.begin();
	PlayerList::iterator eiter = m_players.end();
	for ( ; iter != eiter; ++iter )
	{
		Player* player = *iter;
		if ( player != NULL )
		{
			if ( player->isAlive() || getLeftReviveTimes( player ) > 0 )
			{
				if ( player->IsInTeam() )
				{
					int32_t curTeamId = player->GetTeamId();
					if ( teamid > 0 )
					{
						if ( teamid != curTeamId )
						{
							return false;
						}
					}
					else
					{
						if ( member > 0 )
						{
							return false;
						}
						teamid = player->GetTeamId();
						winners = player->GetCharTeam().GetTeamMembers();
					}
				}
				else
				{
					if ( member > 0 )
					{
						return false;
					}
					winners.push_back( TeamMember( player->getCid(), player->getName(), player->getGateIndex() ) );
				}
				++member;
			}
		}
	}

	win( winners );
	return true;
}

void CTerritoryWar::OnTerritoryWarResult( std::string winners )
{
	m_winners.clear();
	StringVector vStr = Answer::StringUtility::split( winners, "|" );
	for ( uint32_t i = 0; i < vStr.size(); ++i )
	{
		StringVector vWinner = Answer::StringUtility::split( vStr[i], ":" );
		if ( vWinner.size() == 2 )
		{
			m_winners.push_back( TeamMember( _atoi64( vWinner[0].c_str() ), vWinner[1], 0 ) );
		}
	}
	GAME_SERVICE.broadcastHuoDongDaTingIcon();
}

void CTerritoryWar::AppendTerritoryWarInfo( Answer::NetPacket* packet, int32_t& nCount )
{
	if ( NULL == packet )
	{
		return;
	}

	if ( 0 == nCount )
	{
		packet->writeInt32( nCount );
		packet->writeInt32( getNextStartTime() );
		packet->writeInt32( getActivityTime() );
	}

	packet->writeInt32( GetId() );
	packet->writeInt8( m_nTerritoryState );
	packet->writeInt8( m_winners.size() );
	for ( TeamMemberList::iterator iter = m_winners.begin(); iter != m_winners.end(); ++iter )
	{
		packet->writeInt64( iter->nCharId );
		packet->writeUTF8( iter->strName );
	}

	++nCount;
}

int32_t  CTerritoryWar::HaveRewardCount( Player* Player )
{
	if ( NULL == Player )
	{
		return 0;
	}

	TeamMemberList::iterator iter = m_winners.begin();
	for ( ; iter != m_winners.end(); ++iter )
	{
		if ( Player->getCid() == iter->nCharId )
		{
			break;
		}
	}

	if ( iter == m_winners.end() )
	{
		return 0;
	}

	int32_t limitId = PR_ACTIVITY_DAILY_REAWRD_START + GetId();
	if ( Player->GetOperateLimit().CheckCountIsLimited( limitId, 1 ) )
	{
		return 0;
	}
	return 1;
}
