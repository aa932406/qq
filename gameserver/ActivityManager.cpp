#include "stdafx.h"

#include "ActivityManager.h"
#include "GameService.h"
#include "Timer.h"
#include "DaTiHD.h"
#include "FamilyWar.h"
#include "TerritoryWar.h"
#include "CityWar.h"
#include "HorseRacing.h"
#include "FamilyLight.h"
#include "MoLingRuQin.h"
#include "TianJiangBaoXiang.h"
#include "VipGuaJi.h"
#include "SpiderQueen.h"
#include <algorithm>

using namespace Answer;

CActivityManager::CActivityManager()
{
}

CActivityManager::~CActivityManager()
{

}

//���ʼ������
void CActivityManager::Init()
{
	const CfgActivityTable &cfgAllActivity = CFG_DATA.getAllActivity();
	CfgActivityTable::const_iterator iter = cfgAllActivity.begin();
	CfgActivityTable::const_iterator eiter = cfgAllActivity.end();

	bool TerritorySign = true;
	for ( ; iter != eiter; ++iter )
	{
		const CfgActivity& cfgActivity = iter->second;
		CActivity* pActivity = NULL;
		switch ( cfgActivity.typeId )
		{
		case ATI_FAMILY_WAR:		pActivity = new CFamilyWar( cfgActivity );						break;
		case ATI_HORSE_RACING:		pActivity = new CHorseRacing( cfgActivity );					break;
		case ATI_FAMILY_LIGHT:		pActivity = new CFamilyLight( cfgActivity );					break;
		case ATI_MO_LING_RU_QIN:	pActivity = new CMoLingRuQin( cfgActivity );					break;
		case ATI_DA_TI_HUO_DONG:	pActivity = new CDaTiHD( cfgActivity );							break;
		case ATI_BAO_XIANG:			pActivity = new CTianJiangBaoXiang(cfgActivity);				break;
		case ATI_TERRITORY_WAR:		pActivity = new CTerritoryWar( cfgActivity, TerritorySign );	break;
		case ATI_CITY_WAR:			pActivity = new CCityWar( cfgActivity );						break;
		case ATI_SPIDER_QUEEN:		pActivity = new CSqiderQueen( cfgActivity );					break;
		default: break;
		}
		if ( pActivity != NULL )
		{
			pActivity->Init();
			m_mActivity[pActivity->GetId()] = pActivity;
		}
		else
		{
			LOG_ERROR( "CActivityManager::Init() actid=%d create Activity fail!!!\n", cfgActivity.id );
		}
	}
}

void CActivityManager::OnUpdate()
{
	VIP_GUA_JI_SINGLETON.CheckStar();
	for ( ActivityMap::iterator iter = m_mActivity.begin(); iter != m_mActivity.end(); ++iter )
	{
		CActivity* pActivity = iter->second;
		if ( pActivity != NULL )
		{
			pActivity->CheckActivity();
		}
	}
}

void CActivityManager::SendActivityInfo( Player* player, int32_t nId )
{
	ActivityMap::iterator iter = m_mActivity.find( nId );
	if ( iter != m_mActivity.end() )
	{
		CActivity* pActivity = iter->second;
		if ( pActivity != NULL )
		{
			pActivity->SendPlayerActivityInfo( player );
		}
	}
}

// new v2 functions

void CActivityManager::SendActivityRankInfo( Player* player, int32_t nId )
{
	// Note: Original pseudocode called CActivity virtual function (offset 9).
	// Using SendPlayerActivityInfo as the closest available API.
	ActivityMap::iterator iter = m_mActivity.find( nId );
	if ( iter != m_mActivity.end() )
	{
		CActivity* pActivity = iter->second;
		if ( pActivity != NULL )
		{
			pActivity->SendPlayerActivityInfo( player );
		}
	}
}

void CActivityManager::UpdateActivityState( Player* player, int32_t nId, int8_t nState )
{
	// Note: This function tracks activity state changes from external sources.
	// The actual state update is handled by CActivity::CheckActivity().
	// This is a notification hook for cross-activity dependencies.
	ActivityMap::iterator iter = m_mActivity.find( nId );
	if ( iter == m_mActivity.end() )
	{
		return;
	}

	CActivity* pActivity = iter->second;
	if ( pActivity == NULL )
	{
		return;
	}

	// Handle special cases when activity state changes
	// TODO: The original pseudocode handled type 11 (world boss) NPC spawning here.
	// The GMC message code (GMC_ADD_WORLD_BOSS_NPC) doesn't exist in this codebase.
}

int32_t CActivityManager::AddTianJiangBaoXiangCount()
{
	// Note: The original pseudocode called CTianJiangBaoXiang::AddOpenCount().
	// This API doesn't exist in the current codebase - stubbed as TODO.
	return 0;
}

int32_t CActivityManager::GiveDailyReward( Player* player, int32_t nId )
{
	ActivityMap::iterator iter = m_mActivity.find( nId );
	if ( iter != m_mActivity.end() )
	{
		CActivity* pActivity = iter->second;
		if ( pActivity != NULL )
		{
			return pActivity->GiveDailyReward( player );
		}
	}
	return ERR_INVALID_DATA;
}

int32_t	CActivityManager::HaveRewardCount( Player* Player, int32_t nId )
{
	ActivityMap::iterator iter = m_mActivity.find( nId );
	if ( iter != m_mActivity.end() )
	{
		CActivity* pActivity = iter->second;
		if ( pActivity != NULL )
		{
			return pActivity->HaveRewardCount( Player );
		}
	}
	return 0;
}

bool CActivityManager::IsActivityRunning( int32_t id )
{
	ActivityMap::iterator iter = m_mActivity.find( id );
	if ( iter == m_mActivity.end() )
	{
		return false;
	}

	CActivity* pActivity = iter->second;
	if ( NULL == pActivity )
	{
		return false;
	}

	return pActivity->IsRuning();
}

bool CActivityManager::IsActivityRunningByType( int32_t nType )
{
	for ( ActivityMap::iterator iter = m_mActivity.begin(); iter != m_mActivity.end(); ++iter )
	{
		CActivity* pActivity = iter->second;
		if ( pActivity != NULL && pActivity->GetType() == nType && pActivity->IsRuning() )
		{
			return true;
		}
	}
	return false;
}

int32_t CActivityManager::GetCurActivityId( int32_t nType )
{
	for ( ActivityMap::iterator iter = m_mActivity.begin(); iter != m_mActivity.end(); ++iter )
	{
		CActivity* pActivity = iter->second;
		if ( pActivity != NULL && pActivity->GetType() == nType && pActivity->IsRuning() )
		{
			return pActivity->GetId();
		}
	}
	return 0;
}

CActivity* CActivityManager::GetActivity( int32_t id )
{
	ActivityMap::iterator iter = m_mActivity.find( id );
	if ( iter != m_mActivity.end() )
	{
		return iter->second;
	}

	return NULL;
}

//��ȡ���л������д�뵽���ݰ���
void CActivityManager::AppendActivityState(Answer::NetPacket *packet)
{
	if (NULL == packet)
	{
		return;
	}

	int32_t nCount = 0;
	int32_t nPos = packet->getWOffset();
	packet->writeInt8( 0 );
	packet->writeInt32( 0 );

	for ( ActivityMap::iterator iter = m_mActivity.begin(); iter != m_mActivity.end(); ++iter )
	{
		CActivity* pActivity = iter->second;
		if( pActivity != NULL )
		{
			int8_t nState = pActivity->GetState();
			// new v2: map states (0->1, 4->5)
			int8_t nMapState = nState;
			if ( nState == 0 )
			{
				nMapState = 1;
			}
			else if ( nState == 4 )
			{
				nMapState = 5;
			}

			packet->writeInt32( pActivity->GetId() );
			packet->writeInt32( nMapState );
			++nCount;
		}
	}

	uint32_t oldwoffset = packet->getWOffset();
	packet->setWOffset( nPos );
	packet->writeInt8( 0 );
	packet->writeInt32( nCount );
	packet->setWOffset( oldwoffset );
}

FamilyId_t	CActivityManager::GetFamilyWarWinner()
{
	for ( ActivityMap::iterator iter = m_mActivity.begin(); iter != m_mActivity.end(); ++iter )
	{
		CFamilyWar* pFamilyWar = dynamic_cast<CFamilyWar*>( iter->second );
		if ( pFamilyWar != NULL )
		{
			return pFamilyWar->GetWinFamily();
		}
	}
	return 0;
}

std::string CActivityManager::GetFamilyWarWinnerName()
{
	for ( ActivityMap::iterator iter = m_mActivity.begin(); iter != m_mActivity.end(); ++iter )
	{
		CFamilyWar* pFamilyWar = dynamic_cast<CFamilyWar*>( iter->second );
		if ( pFamilyWar != NULL )
		{
			return pFamilyWar->GetFamilyName();
		}
	}
	return "";
}

std::string CActivityManager::GetFamilyWarLeaderName()
{
	for ( ActivityMap::iterator iter = m_mActivity.begin(); iter != m_mActivity.end(); ++iter )
	{
		CFamilyWar* pFamilyWar = dynamic_cast<CFamilyWar*>( iter->second );
		if ( pFamilyWar != NULL )
		{
			return pFamilyWar->GetFamilyLeaderName();
		}
	}
	return "";
}

void CActivityManager::OnFamilyWarResult( int32_t nActId, FamilyId_t nFamilyId, int16_t nWinTimes, string FamilyName, string LeadyerName  )
{
	//m_nFamilyWarWinner		= nFamilyId;
	//m_nFamilyWarWinTimes	= nWinTimes;
	ActivityMap::iterator iter = m_mActivity.find( nActId );
	if ( iter != m_mActivity.end() )
	{
		CFamilyWar* pFamilyWar = dynamic_cast<CFamilyWar*>( iter->second );
		if ( pFamilyWar != NULL )
		{
			pFamilyWar->OnFamilyWarResult( nFamilyId, nWinTimes, FamilyName, LeadyerName );
		}
	}
}

void CActivityManager::OnCityWarResult( int32_t nActId, int32_t nIndex, FamilyId_t nFamilyId, int32_t nWinTime,
	CharId_t nLeader, CharId_t nFirst, CharId_t nSecond, CharId_t nThird,
	const std::string& strFirstFamily, const std::string& strSecondFamily, const std::string& strThirdFamily )
{
	ActivityMap::iterator iter = m_mActivity.find( nActId );
	if ( iter != m_mActivity.end() )
	{
		CCityWar* pCityWar = dynamic_cast<CCityWar*>( iter->second );
		if ( pCityWar != NULL )
		{
			pCityWar->OnCityWarResult( nIndex, nFamilyId, nWinTime, nLeader, nFirst, nSecond, nThird,
				strFirstFamily, strSecondFamily, strThirdFamily );
		}
	}
}

void CActivityManager::OnTerritoryWarResult( int32_t nActId, std::string winners )
{
	ActivityMap::iterator iter = m_mActivity.find( nActId );
	if ( iter != m_mActivity.end() )
	{
		CTerritoryWar* pTerritoryWar = dynamic_cast<CTerritoryWar*>( iter->second );
		if ( pTerritoryWar != NULL )
		{
			pTerritoryWar->OnTerritoryWarResult( winners );
		}
	}
}

void CActivityManager::GetActicityIconState( IconStateList& iconList )
{
	for ( ActivityMap::iterator iter = m_mActivity.begin(); iter != m_mActivity.end(); ++iter )
	{
		CActivity* pActivity = iter->second;
		if ( pActivity != NULL )
		{
			pActivity->GetIconState( iconList );
		}
	}
}

void CActivityManager::OnTerritoryWarInfo( Player* player )
{
	if ( NULL == player )
	{
		return;
	}

	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_NOTIFY_ACTIVITY_TERRITORY_INFO );
	if (NULL == packet)
	{
		return;
	}

	int32_t nCount = 0;
	for ( ActivityMap::iterator iter = m_mActivity.begin(); iter != m_mActivity.end(); ++iter )
	{
		CActivity* pActivity = iter->second;
		if ( pActivity != NULL && pActivity->GetType() == ATI_TERRITORY_WAR )
		{
			CTerritoryWar* pTerritoryWar = dynamic_cast<CTerritoryWar*>( pActivity );
			if ( pTerritoryWar != NULL )
			{
				pTerritoryWar->AppendTerritoryWarInfo( packet, nCount );
			}
		}
	}

	uint32_t offset = packet->getWOffset();
	packet->setWOffset( 0 );
	packet->writeInt32( nCount );
	packet->setWOffset( offset );
	packet->setSize( offset );

	GAME_SERVICE.sendPacketTo( player->getGateIndex(), packet );
}

int8_t CActivityManager::GetTerritoryWarState()
{
	for ( ActivityMap::iterator iter = m_mActivity.begin(); iter != m_mActivity.end(); ++iter )
	{
		CActivity* pActivity = iter->second;
		if ( pActivity != NULL && pActivity->GetType() == ATI_TERRITORY_WAR )
		{
			return pActivity->GetState();
		}
	}
	return AS_NOT_START;
}



void CActivityManager::SetCityWarWinner(FamilyId_t nFamilyId)
{
	m_mWinFamily.clear();
	if (nFamilyId > 0)
	{
		// Store winner for each connection (line)
		m_mWinFamily[0] = nFamilyId;
	}
}

FamilyId_t CActivityManager::GetCityWarWinner()
{
	std::map<int32_t, FamilyId_t>::iterator iter = m_mWinFamily.find(0);
	if (iter != m_mWinFamily.end())
	{
		return iter->second;
	}
	return 0;
}

void CActivityManager::StopSqiderQueen()
{
	for ( ActivityMap::iterator iter = m_mActivity.begin(); iter != m_mActivity.end(); ++iter )
	{
		CActivity* pActivity = iter->second;
		CSqiderQueen* pSqiderQueen = dynamic_cast<CSqiderQueen*>(pActivity);
		if ( pSqiderQueen != NULL  )
		{
			if ( pSqiderQueen->IsRuning() )
			{
				pSqiderQueen->StopActivityBySqiderQueenDie();
			}
		}
	}
}

void CActivityManager::NotifyActivityInfo( Player* player )
{
	for ( ActivityMap::iterator iter = m_mActivity.begin(); iter != m_mActivity.end(); ++iter )
	{
		CActivity* pActivity = iter->second;
		if ( pActivity != NULL  )
		{
			pActivity->NotifyActivityInfo( player );
		}
	}
}