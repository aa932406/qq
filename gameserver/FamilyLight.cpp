
#include "FamilyLight.h"
#include "GameService.h"
#include "DBService.h"
#include "Timer.h"
#include "ActivityMap.h"
#include "FamilyManager.h"
#include "ActivityManager.h"

using namespace Answer;

CFamilyLight::CFamilyLight( const CfgActivity& cfgActivity )
:CActivity( cfgActivity )
{

}

CFamilyLight::~CFamilyLight()
{

}

void CFamilyLight::Init()
{
	CActivity::Init();
}

void CFamilyLight::OnUpdate( CActivityMap* pMap )
{
	CActivity::OnUpdate( pMap );
	if ( NULL == pMap )
	{
		return;
	}

	int64_t curTick = pMap->getTick();
	for ( PlayerScoreMap::iterator iter = m_mPlayerScore.begin(); iter != m_mPlayerScore.end(); ++iter )
	{
		PlayerScore& score = iter->second;
		if ( score.bInActivity )
		{
			Player* player = GAME_SERVICE.getPlayer( score.nCharId, pMap->GetRunnerId() );
			if ( NULL == player )
			{
				continue;
			}

			if ( player->getMapId() != pMap->GetId() )
			{
				continue;
			}

			int32_t narea = checkArea( player->getCurrentTile() );
			if ( narea != score.nArea )
			{
				score.nArea = narea;
				SendPlayerActivityScore( player, getLeftTime() );
			}

			if ( curTick - score.nLastTick >= 10000  )
			{
				score.nLastTick = curTick;
				int32_t Level = 0;
				if ( player->GetSoulLevel() > 0 )
				{
					Level = 130 + player->GetSoulLevel();
				}
				else
				{
					Level = player->getLevel();
				}
				int32_t nExp = CFG_DATA.GetFamilyLightExpTable().GetExp( Level );
				int32_t nAddRate = getExpRate( player, score );
				VipCfg* pVipCfg = CFG_DATA.GetVipTable().GetVipCfg( player->GetPlayerVip().GetMaxVipLevel() );
				if ( pVipCfg != NULL )
				{
					nAddRate += pVipCfg->FamilyLightAddRate;
				}
				int32_t nAddExp = nExp * nAddRate / 100;
				if ( player->isAlive() )
				{
					player->addExp( nAddExp );

					player->GetCharPet().OnAddExp( nAddExp/20, true );
					score.nExp += nAddExp;
				}
				SendPlayerActivityScore( player, getLeftTime() );
			}
		}
	}
}

bool CFamilyLight::OnSitRevive( Player* player )
{
	if ( NULL == player )
	{
		return false;
	}
	if ( player->GetPlayerVip().GetVipType() <= VT_TI_YAN_CARD )
	{
		return false;
	}
	return true;
}

void CFamilyLight::reset()
{
	CActivity::reset();
	m_mPlayerScore.clear();
}

// bool CFamilyLight::checkData()
// {
// 	int32_t startDays = TIMER.GetDaysFromStart();	// �������Ϸ�������쿪��
// 	if ( startDays == 2 )
// 	{
// 		return true;
// 	}
// 	else if ( startDays < 2 )
// 	{
// 		return false;
// 	}
// 	else
// 	{
// 		return CActivity::checkData();
// 	}
// }

void CFamilyLight::addPlayer( Player* player )
{
	CActivity::addPlayer( player );
	if ( NULL == player )
	{
		return;
	}

	PlayerScoreMap::iterator iter = m_mPlayerScore.find( player->getCid() );
	if ( iter != m_mPlayerScore.end() )
	{
		iter->second.bInActivity	= true;
		iter->second.nLastTick		= player->getTick();
	}
	else
	{
		PlayerScore score = {};
		score.nCharId		= player->getCid();
		score.bInActivity	= true;
		score.nLastTick		= player->getTick();
		m_mPlayerScore.insert( PlayerScoreMap::value_type( score.nCharId, score ) );
	}
	SendPlayerActivityScore( player, getLeftTime() );
}

void CFamilyLight::removePlayer( Player* player, bool islogout )
{
	CActivity::removePlayer( player, islogout );
	if ( NULL == player )
	{
		return;
	}
	PlayerScoreMap::iterator iter = m_mPlayerScore.find( player->getCid() );
	if ( iter != m_mPlayerScore.end() )
	{
		iter->second.bInActivity = false;
	}
}

void CFamilyLight::SendPlayerActivityScore( Player* player, int32_t nLeftTime )
{
	if ( NULL == player )
	{
		return;
	}

	PlayerScoreMap::iterator iter = m_mPlayerScore.find( player->getCid() );
	if ( iter == m_mPlayerScore.end() )
	{
		return;
	}

	PlayerScore& score = iter->second;

	NetPacket* packet = GAME_SERVICE.popNetpacket( PACK_DISPATCH, SM_NOTIFY_ACTIVITY_SCORE );
	if ( NULL == packet )
	{
		return;
	}

	packet->writeInt32( m_cfgActivity.id );
	packet->writeInt32( score.nExp );
	packet->writeInt32( getExpRate( player, score ) );
	packet->writeInt32( nLeftTime );

	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.sendPacketTo( player->getGateIndex(), packet );
}

bool CFamilyLight::CanUseXP() const
{
	return false;
}

bool CFamilyLight::DrinkWine( Player* player, int32_t Count )
{
	if ( NULL == player )
	{
		return false;
	}

	SendPlayerActivityScore( player, getLeftTime() );
	return true;
}

//void CFamilyLight::broadcastReady()
//{
//	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM );
//	if (NULL == packet)
//	{
//		return;
//	}
//	packet->writeInt32( BCI_WORLD_FAMILYLIGHT_READY );
//	packet->setSize(packet->getWOffset());
//	GAME_SERVICE.worldBroadcast(packet);
//}
//
//void CFamilyLight::broadcastStart()
//{
//	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM );
//	if (NULL == packet)
//	{
//		return;
//	}
//	packet->writeInt32( BCI_WORLD_FAMILYLIGHT_START );
//	packet->setSize(packet->getWOffset());
//	GAME_SERVICE.worldBroadcast(packet);
//}

int32_t	CFamilyLight::checkArea( const Position& pos )
{
	int32_t nDis = pos.tileDistance( Position( 49,138 ) );
	if ( nDis < 8 )
	{
		return FLA_RED;
	}
	//else if ( nDis < 20 )
	//{
	//	return FLA_BLUE;
	//}
	return FLA_NORMAL;
}

int32_t	CFamilyLight::getExpRate( Player* player, const PlayerScore& score )
{
	if ( NULL == player )
	{
		return 0;
	}
	int32_t nAddRate = 0;
	switch ( score.nArea )
	{
	case FLA_NORMAL:	nAddRate += 100;	break;
	case FLA_BLUE:		nAddRate += 150;	break;
	case FLA_RED:		nAddRate += 200;	break;
	default:	break;
	}
	
	if ( player->HasBuffById( JUN_TUAN_LIE_JIU_BUFFI ) )
	{
		nAddRate += 100;
	}

	return nAddRate;
}

void CFamilyLight::onTimeEnd()
{
	m_nState = AS_END;
	delayKickAll( 0 );
}
