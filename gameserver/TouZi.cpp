#include "TouZi.h"
#include "Player.h"
#include "GameService.h"
#include "CfgData.h"
#include "FunctionOpen.h"

using namespace Answer;

CTouZi::CTouZi()
{
	OnCleanUp();
}

CTouZi::~CTouZi()
{
}

void CTouZi::OnLoadFromDB( const PlayerDBData& dbData )
{
	m_SevenDayTouZiTime	= dbData.m_TouZiData.m_SevenDayTouZiTime;
	m_SevenDayRecord	= dbData.m_TouZiData.m_SevenDayRecord;
	m_MonthTouZiTime	= dbData.m_TouZiData.m_MonthTouZiTime;
	m_MonthTouZiRecord	= dbData.m_TouZiData.m_MonthTouZiRecord;
}

void CTouZi::OnSaveToDB( PlayerDBData& dbData )
{
	dbData.m_TouZiData.m_SevenDayTouZiTime	= m_SevenDayTouZiTime;
	dbData.m_TouZiData.m_SevenDayRecord		= m_SevenDayRecord;
	dbData.m_TouZiData.m_MonthTouZiTime		= m_MonthTouZiTime;
	dbData.m_TouZiData.m_MonthTouZiRecord	= m_MonthTouZiRecord;
}

void CTouZi::OnCleanUp()
{
	m_SevenDayTouZiTime		= 0;
	m_SevenDayRecord		= 0;
	m_MonthTouZiTime		= 0;
	m_MonthTouZiRecord		= 0;
}

void CTouZi::OnUpdate( int64_t curTick )
{
}

void CTouZi::OnDaySwitch( int32_t nDiffDays )
{
	SendTouZiInfo();
	SendTouZiIcon();
}

void CTouZi::GetInterestsProtocol( ProcIdList& procList )
{
	procList.push_back( CM_ASK_TOU_ZI_INFO );
	procList.push_back( CM_GET_TOU_ZI_REWARD );
	procList.push_back( CM_TOU_ZI );
}

int32_t CTouZi::DispatchNetDatas( ProcId_t nProcId, Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	switch( nProcId )
	{
	case CM_ASK_TOU_ZI_INFO:
		{
			return OnAskTouZiInfo( inPacket );
		}
	case CM_GET_TOU_ZI_REWARD:
		{
			return OnGetTouZiReward( inPacket );
		}
	case CM_TOU_ZI:
		{
			return OnTouZi( inPacket );
		}
	}
	return ERR_OK;
}

int32_t CTouZi::OnTouZi( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket || NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}

	int8_t nType = inPacket->readInt8();

	// ��ҪVIP
	if ( m_pPlayer->GetPlayerVip().GetVipLevel() <= 0 )
	{
		return ERR_SYETEM_ERR;
	}

	int32_t nGongGaoId = 0;

	if ( nType == 1 ) // 7��Ͷ��
	{
		// ���߿�����7��
		if ( CFG_DATA.getServerDiffDay() + 1 > 7 )
		{
			return ERR_SYETEM_ERR;
		}
		if ( m_SevenDayTouZiTime > 0 )
		{
			return ERR_SYETEM_ERR;
		}
		if ( !m_pPlayer->DecCurrency( CURRENCY_GOLD, 10000, GCR_TOU_ZHI_SHI_QU, nType ) )
		{
			return ERR_SYETEM_ERR;
		}
		m_SevenDayTouZiTime = m_pPlayer->getNow();
		nGongGaoId = 499;
	}
	else if ( nType == 2 ) // �¶�Ͷ��
	{
		if ( m_MonthTouZiTime > 0 )
		{
			return ERR_SYETEM_ERR;
		}
		// ���Ѳ���7��������δ��ȫ��7�ս���������Ͷ��
		if ( m_SevenDayTouZiTime > 0 && !IsAllGetSevenDayTouZi() )
		{
			return ERR_SYETEM_ERR;
		}
		// ���û���7��Ͷ�ʣ�������7���
		if ( m_SevenDayTouZiTime <= 0 )
		{
			if ( CFG_DATA.getServerDiffDay() + 1 <= 7 )
			{
				return ERR_SYETEM_ERR;
			}
		}
		if ( !m_pPlayer->DecCurrency( CURRENCY_GOLD, 50000, GCR_TOU_ZHI_SHI_QU, nType ) )
		{
			return ERR_SYETEM_ERR;
		}
		m_MonthTouZiTime = m_pPlayer->getNow();
		nGongGaoId = 500;
	}
	else
	{
		return ERR_SYETEM_ERR;
	}

	SendTouZiInfo();
	SendTouZiIcon();
	GAME_SERVICE.replySuccess( m_pPlayer->getGateIndex(), inPacket->getProc(), nType );
	BroadcastTouZi( nGongGaoId );
	return ERR_OK;
}

int32_t CTouZi::OnGetTouZiReward( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket || NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	int8_t nType	= inPacket->readInt8();
	int16_t nIndex	= inPacket->readInt16();
	if ( nIndex <= 0 )
	{
		return ERR_SYETEM_ERR;
	}
	if ( nType == 1 )
	{
		return GetSevenTouZiReward( nIndex );
	}
	else if ( nType == 2 )
	{
		return GetMonthTouZiReward( nIndex );
	}
	return ERR_OK;
}

int32_t CTouZi::OnAskTouZiInfo( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	SendTouZiInfo();
	return ERR_OK;
}

int32_t CTouZi::GetSevenTouZiReward( int16_t nIndex )
{
	if ( NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	if ( m_SevenDayTouZiTime <= 0 )
	{
		return ERR_SYETEM_ERR;
	}

	const SevenTouZi* pCfg = CFG_DATA.GetTouZiTable().GetSevenTouZi( nIndex );
	if ( NULL == pCfg )
	{
		return ERR_SYETEM_ERR;
	}

	int32_t nNewRecord = (1 << (nIndex - 1)) | m_SevenDayRecord;
	if ( m_SevenDayRecord == nNewRecord )
	{
		return ERR_SYETEM_ERR;
	}

	if ( pCfg->nType == 1 )
	{
		if ( DayTime::daydiff( m_SevenDayTouZiTime ) + 1 < pCfg->nCondition )
		{
			return ERR_SYETEM_ERR;
		}
	}
	else if ( pCfg->nType == 2 )
	{
		if ( m_pPlayer->getLevel() < pCfg->nCondition )
		{
			return ERR_SYETEM_ERR;
		}
	}
	else
	{
		return ERR_SYETEM_ERR;
	}

	if ( !m_pPlayer->GetBag().AddItem( pCfg->vItem, IACR_ACTIVITY ) )
	{
		return ERR_SYETEM_ERR;
	}

	m_SevenDayRecord = nNewRecord;
	SendTouZiInfo();
	SendTouZiIcon();
	return ERR_OK;
}

int32_t CTouZi::GetMonthTouZiReward( int16_t nIndex )
{
	if ( NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	if ( m_MonthTouZiTime <= 0 )
	{
		return ERR_SYETEM_ERR;
	}

	int32_t nNewRecord = (1 << (nIndex - 1)) | m_MonthTouZiRecord;
	if ( m_MonthTouZiRecord == nNewRecord )
	{
		return ERR_SYETEM_ERR;
	}

	const MonthTouZi* pCfg = CFG_DATA.GetTouZiTable().GetMonthTouZi( nIndex );
	if ( NULL == pCfg )
	{
		return ERR_SYETEM_ERR;
	}

	int32_t nDiffDay = DayTime::daydiff( m_MonthTouZiTime );
	if ( nDiffDay + 1 < pCfg->nDay )
	{
		return ERR_SYETEM_ERR;
	}

	if ( !m_pPlayer->GetBag().AddItem( pCfg->vItem, IACR_ACTIVITY ) )
	{
		return ERR_SYETEM_ERR;
	}

	m_MonthTouZiRecord = nNewRecord;
	SendTouZiInfo();
	SendTouZiIcon();

	if ( pCfg->nGongGaoId > 0 )
	{
		BroadcastTouZi( pCfg->nGongGaoId );
	}
	return ERR_OK;
}

void CTouZi::SendTouZiInfo()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_TOU_ZI_INFO );
	if ( NULL == packet )
	{
		return;
	}
	packet->writeInt32( m_SevenDayTouZiTime );
	packet->writeInt32( m_SevenDayRecord );
	packet->writeInt32( m_MonthTouZiTime );
	packet->writeInt32( m_MonthTouZiRecord );
	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.sendPacketTo( m_pPlayer->getGateIndex(), packet );
}

void CTouZi::GetTouZiIconState( IconStateList& IconList )
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	if ( !m_pPlayer->GetPlayerFunctionOpen().IsOpened( FT_TOU_ZI ) )
	{
		return;
	}
	IconList.push_back( GetTouZiIconStu() );
}

void CTouZi::SendTouZiIcon()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	if ( !m_pPlayer->GetPlayerFunctionOpen().IsOpened( FT_TOU_ZI ) )
	{
		return;
	}
	ShowIcon stu = GetTouZiIconStu();

	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_ONE_ICON );
	if ( NULL == packet )
	{
		return;
	}
	packet->writeInt32( stu.nId );
	packet->writeInt8(  stu.nState );
	packet->writeInt32( stu.nLeftTime );
	packet->writeInt8( stu.IconLeft );
	packet->writeInt32( stu.IconRight );
	packet->writeInt8( stu.Effects );
	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.sendPacketTo( m_pPlayer->getGateIndex(), packet );

	// ����������Ҳ��Ҫ����
	if ( stu.nId == 133 )
	{
		ShowIcon StuTmp = {};
		StuTmp.nId		= 134;
		StuTmp.nState	= 4;
		StuTmp.nLeftTime= -1;

		Answer::NetPacket *packet2 = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_ONE_ICON );
		if ( NULL == packet2 )
		{
			return;
		}
		packet2->writeInt32( StuTmp.nId );
		packet2->writeInt8(  StuTmp.nState );
		packet2->writeInt32( StuTmp.nLeftTime );
		packet2->writeInt8( StuTmp.IconLeft );
		packet2->writeInt32( StuTmp.IconRight );
		packet2->writeInt8( StuTmp.Effects );
		packet2->setSize( packet2->getWOffset() );
		GAME_SERVICE.sendPacketTo( m_pPlayer->getGateIndex(), packet2 );
	}
}

bool CTouZi::IsAllGetSevenDayTouZi()
{
	return CFG_DATA.GetTouZiTable().IsAllGetSevenDay( m_SevenDayRecord );
}

bool CTouZi::IsAllGetMonthTouZi()
{
	return CFG_DATA.GetTouZiTable().IsAllGetMonthTouZi( m_MonthTouZiRecord );
}

int32_t CTouZi::GetSevenDayRewardCount()
{
	if ( NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	if ( m_SevenDayTouZiTime <= 0 )
	{
		return 0;
	}

	int32_t Count = 0;
	int32_t nDiffDay = DayTime::daydiff( m_SevenDayTouZiTime );
	const SevenTouZiMap& TouZiMap = CFG_DATA.GetTouZiTable().GetSevenDayTable();
	SevenTouZiMap::const_iterator it = TouZiMap.begin();
	for ( ; it != TouZiMap.end(); ++it )
	{
		const SevenTouZi& rCfg = it->second;
		if ( rCfg.nType == 1 )
		{
			if ( nDiffDay + 1 < rCfg.nCondition )
			{
				continue;
			}
		}
		else if ( rCfg.nType == 2 )
		{
			if ( m_pPlayer->getLevel() < rCfg.nCondition )
			{
				continue;
			}
		}
		else
		{
			continue;
		}
		int32_t nNewRecord = (1 << (rCfg.nId - 1)) | m_SevenDayRecord;
		if ( m_SevenDayRecord != nNewRecord )
		{
			++Count;
		}
	}
	return Count;
}

int32_t CTouZi::GetMonthRewardCount()
{
	if ( NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	if ( m_MonthTouZiTime <= 0 )
	{
		return 0;
	}

	int32_t Count = 0;
	int32_t nDiffDay = DayTime::daydiff( m_MonthTouZiTime );
	const MonthTouZiMap& TouZiMap = CFG_DATA.GetTouZiTable().GetMonthTable();
	MonthTouZiMap::const_iterator it = TouZiMap.begin();
	for ( ; it != TouZiMap.end(); ++it )
	{
		const MonthTouZi& rCfg = it->second;
		if ( nDiffDay + 1 >= rCfg.nDay )
		{
			int32_t nNewRecord = (1 << (rCfg.nDay - 1)) | m_MonthTouZiRecord;
			if ( m_MonthTouZiRecord != nNewRecord )
			{
				++Count;
			}
		}
	}
	return Count;
}

ShowIcon CTouZi::GetTouZiIconStu()
{
	ShowIcon stu = {};

	stu.nId			= 134;	// 7��Ͷ��ͼ��ID
	stu.nLeftTime	= -1;
	stu.nState		= 2;	// AS_RUNNING

	if ( m_SevenDayTouZiTime <= 0 )
	{
		if ( CFG_DATA.getServerDiffDay() + 1 > 7 )
		{
			stu.nId = 133;	// �¶�Ͷ��ͼ��ID
		}
	}
	else if ( IsAllGetSevenDayTouZi() )
	{
		stu.nId = 133;	// �¶�Ͷ��ͼ��ID
	}

	if ( IsAllGetMonthTouZi() )
	{
		stu.nState = 4;	// AS_FINISHED
	}
	else
	{
		stu.nState = 2;	// AS_RUNNING
	}

	if ( stu.nId == 134 )
	{
		stu.IconRight = GetSevenDayRewardCount();
	}
	else
	{
		stu.IconRight = GetMonthRewardCount();
	}

	return stu;
}

void CTouZi::BroadcastTouZi( int32_t nGongGaoId )
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	if ( nGongGaoId <= 0 )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_FAMILY_WAR_ICON );
	if ( NULL == packet )
	{
		return;
	}
	packet->writeInt32( nGongGaoId );
	packet->writeInt64( m_pPlayer->getCid() );
	packet->writeUTF8( m_pPlayer->getName() );
	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.worldBroadcast( packet );
}

//==============================================================================
// CfgTouZiTable ʵ��
//==============================================================================

const SevenTouZi* CfgTouZiTable::GetSevenTouZi( int32_t nId ) const
{
	SevenTouZiMap::const_iterator it = m_SevenTouZiMap.find( nId );
	if ( it != m_SevenTouZiMap.end() )
	{
		return &(it->second);
	}
	return NULL;
}

const MonthTouZi* CfgTouZiTable::GetMonthTouZi( int32_t nId ) const
{
	MonthTouZiMap::const_iterator it = m_MonthTouZiMap.find( nId );
	if ( it != m_MonthTouZiMap.end() )
	{
		return &(it->second);
	}
	return NULL;
}

bool CfgTouZiTable::IsAllGetSevenDay( int32_t nRecord ) const
{
	SevenTouZiMap::const_iterator it = m_SevenTouZiMap.begin();
	for ( ; it != m_SevenTouZiMap.end(); ++it )
	{
		int32_t nNewRecord = (1 << (it->second.nId - 1)) | nRecord;
		if ( nRecord != nNewRecord )
		{
			return false;
		}
	}
	return true;
}

bool CfgTouZiTable::IsAllGetMonthTouZi( int32_t nRecord ) const
{
	MonthTouZiMap::const_iterator it = m_MonthTouZiMap.begin();
	for ( ; it != m_MonthTouZiMap.end(); ++it )
	{
		int32_t nNewRecord = (1 << (it->second.nDay - 1)) | nRecord;
		if ( nRecord != nNewRecord )
		{
			return false;
		}
	}
	return true;
}
