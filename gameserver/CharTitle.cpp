#include "stdafx.h"
#include "CharTitle.h"
#include "Player.h"
#include "Timer.h"
#include "DataStruct.h"
#include "GameService.h"

CExtCharTitle::CExtCharTitle()
{
}

CExtCharTitle::~CExtCharTitle()
{
}

void CExtCharTitle::OnCleanUp()
{
	m_vTitles.clear();
	m_nLastExpireCheck = 0;
}

void CExtCharTitle::OnLoadFromDB( const PlayerDBData& dbData )
{
	m_vTitles.clear();
	m_nLastExpireCheck = 0;
	const std::vector<MemChrTitleData::MemChrTitle>& dbTitles = dbData.m_TitleData.m_vTitles;
	for ( size_t i = 0; i < dbTitles.size(); ++i )
	{
		MemTitle title;
		title.nType		= dbTitles[i].nType;
		title.nValue		= dbTitles[i].nValue;
		title.nExpireTime	= dbTitles[i].nExpireTime;
		m_vTitles.push_back( title );
	}

	// Load from DB, clean expired immediately
	CleanExpiredTitles();

	// Sync to client
	SendTitleInfo();
}

void CExtCharTitle::OnSaveToDB( PlayerDBData& dbData )
{
	dbData.m_TitleData.m_vTitles.clear();
	for ( TitleVector::const_iterator it = m_vTitles.begin(); it != m_vTitles.end(); ++it )
	{
		MemChrTitleData::MemChrTitle title;
		title.nType		= it->nType;
		title.nValue		= it->nValue;
		title.nExpireTime	= it->nExpireTime;
		dbData.m_TitleData.m_vTitles.push_back( title );
	}
}

void CExtCharTitle::OnUpdate( int64_t curTick )
{
	(void)curTick;
	// Check expired titles every 10 seconds
	int32_t nowTime = Answer::DayTime::now();
	if ( m_nLastExpireCheck == 0 || nowTime - m_nLastExpireCheck >= 10 )
	{
		m_nLastExpireCheck = nowTime;
		CleanExpiredTitles();
	}
}

void CExtCharTitle::OnDaySwitch( int32_t nDiffDays )
{
	// On day change, clean expired titles
	CleanExpiredTitles();
}

void CExtCharTitle::CleanExpiredTitles()
{
	if ( m_vTitles.empty() )
	{
		return;
	}

	int32_t nowTime = Answer::DayTime::now();
	bool bRemoved = false;

	for ( TitleVector::iterator it = m_vTitles.begin(); it != m_vTitles.end(); )
	{
		if ( it->nExpireTime != 0 && nowTime >= it->nExpireTime )
		{
			LOG_INFO("CExtCharTitle::CleanExpiredTitles: cid=%lld type=%d value=%d expire=%d now=%d",
				m_pPlayer ? m_pPlayer->getCid() : 0,
				it->nType, it->nValue, it->nExpireTime, nowTime);
			it = m_vTitles.erase(it);
			bRemoved = true;
			continue;
		}
		++it;
	}


	// Sync to client if any titles expired
	if ( bRemoved )
	{
		LOG_INFO("CExtCharTitle::CleanExpiredTitles: cid=%lld removed expired titles",
			m_pPlayer ? m_pPlayer->getCid() : 0);
		SendTitleInfo();

		// Send notification (use 0 for type/value since multiple titles may have expired)
		SendTitleNotice( BCI_TITLE_EXPIRED, 0, 0 );
	}
}

void CExtCharTitle::SendTitleInfo()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_TITLE_INFO );

	// Write title count
	int32_t nSize = (int32_t)m_vTitles.size();
	packet->writeInt32( nSize );

	// Write each title entry
	for ( TitleVector::const_iterator it = m_vTitles.begin(); it != m_vTitles.end(); ++it )
	{
		packet->writeInt32( it->nType );
		packet->writeInt32( it->nValue );
		packet->writeInt32( it->nExpireTime );
	}

	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.sendPacketTo( m_pPlayer->getGateIndex(), packet );

	LOG_DEBUG("CExtCharTitle::SendTitleInfo: cid=%lld count=%d",
		m_pPlayer->getCid(), nSize);
}

void CExtCharTitle::SendTitleNotice( int32_t nNoticeType, int32_t nTitleType, int32_t nTitleValue )
{
	if ( NULL == m_pPlayer )
	{
		return;
	}

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM );
	if ( NULL == packet )
	{
		return;
	}

	packet->writeInt32( nNoticeType );
	packet->writeUTF8( m_pPlayer->getName() );
	packet->writeInt64( m_pPlayer->getCid() );
	packet->writeInt32( nTitleType );
	packet->writeInt32( nTitleValue );

	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.sendPacketTo( m_pPlayer->getGateIndex(), packet );

	LOG_DEBUG("CExtCharTitle::SendTitleNotice: cid=%lld notice=%d type=%d value=%d",
		m_pPlayer->getCid(), nNoticeType, nTitleType, nTitleValue);
}

void CExtCharTitle::RemoveTitle( int32_t nType, int32_t nParam )
{
	bool bRemoved = false;
	for (TitleVector::iterator it = m_vTitles.begin(); it != m_vTitles.end(); )
	{
		if (it->nType == nType)
		{
			// If param is specified, only remove if value matches
			if (nParam == 0 || it->nValue == nParam)
			{
				it = m_vTitles.erase(it);
				bRemoved = true;
				continue;
			}
		}
		++it;
	}

	// Sync to client if any titles were removed
	if ( bRemoved )
	{
		SendTitleInfo();
	}
}

bool CExtCharTitle::CheckAddTitle( int32_t nType, int32_t nValue )
{
	// Check if player already has this or better title
	for (TitleVector::iterator it = m_vTitles.begin(); it != m_vTitles.end(); ++it)
	{
		if (it->nType == nType)
		{
			// Already has a title of this type
			// If existing value is >= new value, skip (already has equal or better)
			if (it->nValue >= nValue)
			{
				return false;
			}
			// Otherwise remove old and add new
			m_vTitles.erase(it);
			break;
		}
	}

	AddTitle(nType, nValue);
	return true;
}

void CExtCharTitle::AddTitle( int32_t nType, int32_t nValue, int32_t nExpireTime )
{
	MemTitle title;
	title.nType = nType;
	title.nValue = nValue;
	title.nExpireTime = nExpireTime;
	m_vTitles.push_back(title);

	LOG_INFO("CExtCharTitle::AddTitle: cid=%lld type=%d value=%d expire=%d",
		m_pPlayer ? m_pPlayer->getCid() : 0, nType, nValue, nExpireTime);

	// Sync to client
	SendTitleInfo();

	// Send notification
	SendTitleNotice( BCI_TITLE_ADDED, nType, nValue );
}

int32_t CExtCharTitle::GetTitleValue( int32_t nType ) const
{
	for (TitleVector::const_iterator it = m_vTitles.begin(); it != m_vTitles.end(); ++it)
	{
		if (it->nType == nType)
		{
			return it->nValue;
		}
	}
	return 0;
}

bool CExtCharTitle::HasTitle( int32_t nType ) const
{
	for (TitleVector::const_iterator it = m_vTitles.begin(); it != m_vTitles.end(); ++it)
	{
		if (it->nType == nType)
		{
			return true;
		}
	}
	return false;
}
