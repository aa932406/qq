#include "stdafx.h"
#include "LevelRefining.h"
#include "Player.h"
#include "GameService.h"
#include "CfgData.h"

using namespace Answer;

CLevelRefining::CLevelRefining()
{
}

CLevelRefining::~CLevelRefining()
{
}

void CLevelRefining::GetInterestsProtocol( ProcIdList& procList )
{
	procList.push_back( CM_LEVEL_REFIN_INFO );
	procList.push_back( CM_LEVEL_REFINING );
}

int32_t CLevelRefining::DispatchNetDatas( ProcId_t nProcId, Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_INVALID_DATA;
	}
	switch( nProcId )
	{
	case CM_LEVEL_REFIN_INFO:
		{
			SendRefinInfo();
		}
		break;
	case CM_LEVEL_REFINING:
		{
			return OnRefining( inPacket );
		}
		break;
	default:
		break;
	}
	return ERR_OK;
}

int32_t CLevelRefining::OnRefining( Answer::NetPacket* pData )
{
	if ( NULL == pData || NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}

	int32_t nTime = m_pPlayer->getRecord( 2081 );
	int32_t nLevel = m_pPlayer->getLevel();

	LevelRefinCfg *pCfg = CFG_DATA.GetRefining( nLevel );
	if ( NULL == pCfg )
	{
		return ERR_SYETEM_ERR;
	}

	if ( pCfg->nLimit <= nTime )
	{
		return ERR_SYETEM_ERR;
	}

	if ( static_cast<int32_t>(pCfg->vNeedGold.size()) <= nTime || pCfg->vNeedGold[nTime] <= 0 )
	{
		return ERR_SYETEM_ERR;
	}

	if ( !m_pPlayer->DecCurrency( CURRENCY_GOLD, pCfg->vNeedGold[nTime], GCR_LEVEL_REFIN ) )
	{
		return ERR_SYETEM_ERR;
	}

	// 升级
	int32_t nUpLevel = pCfg->nLevelUp[nTime];
	if ( nUpLevel > m_pPlayer->GetMaxLevel() )
	{
		nUpLevel = m_pPlayer->GetMaxLevel();
	}
	m_pPlayer->SetLevel( nUpLevel );

	// 公告
	if ( pCfg->nGongGaoId > 0 )
	{
		NetPacket *packet = GAME_SERVICE.popNetpacket( PACK_DISPATCH, SM_LEVEL_REFIN_BROADCAST );
		if ( packet != NULL )
		{
			packet->writeInt32( pCfg->nGongGaoId );
			packet->writeUTF8( m_pPlayer->getName() );
			packet->writeInt64( m_pPlayer->getCid() );
			packet->setSize( packet->getWOffset() );
			GAME_SERVICE.worldBroadcast( packet );
		}
	}

	m_pPlayer->updateRecord( 2081, nTime + 1 );
	SendRefinInfo();

	GAME_SERVICE.replySuccess( m_pPlayer->getGateIndex(), pData->getProc() );
	SendIcon();

	return ERR_OK;
}

void CLevelRefining::SendRefinInfo()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}

	NetPacket *packet = GAME_SERVICE.popNetpacket( PACK_DISPATCH, SM_SEND_LEVEL_REFIN );
	if ( NULL == packet )
	{
		return;
	}

	int32_t nRecord = m_pPlayer->getRecord( 2081 );
	packet->writeInt32( nRecord );
	packet->writeInt32( GetLeftTime() );
	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.sendPacketTo( m_pPlayer->getGateIndex(), packet );
}

int32_t CLevelRefining::GetLeftTime()
{
	int32_t nNow = m_pPlayer->getNow();
	int32_t nStartDayZero = CFG_DATA.getServerStartDayTime();
	return nStartDayZero + 259200 - nNow;
}

void CLevelRefining::GetIcon( IconStateList& IconList )
{
	if ( NULL == m_pPlayer )
	{
		return;
	}

	int32_t nServerDay = CFG_DATA.getServerDiffTime() / 86400;
	if ( nServerDay < 0 || nServerDay + 1 > 3 )
	{
		return;
	}

	if ( !m_pPlayer->GetPlayerFunctionOpen().IsOpened( 186 ) )
	{
		return;
	}

	ShowIcon icon = {};
	icon.nId = 109;
	icon.nState = ( nServerDay == 2 && m_pPlayer->getRecord( 2081 ) > 4 ) ? 4 : 2;
	icon.nLeftTime = GetLeftTime();
	IconList.push_back( icon );
}

void CLevelRefining::SendIcon()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}

	int32_t nServerDay = CFG_DATA.getServerDiffTime() / 86400;
	if ( nServerDay < 0 || nServerDay + 1 > 3 )
	{
		return;
	}

	if ( !m_pPlayer->GetPlayerFunctionOpen().IsOpened( 186 ) )
	{
		return;
	}

	ShowIcon stu = {};
	stu.nId = 109;
	stu.nState = ( nServerDay == 2 && m_pPlayer->getRecord( 2081 ) > 4 ) ? 4 : 2;
	stu.nLeftTime = GetLeftTime();

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
}
