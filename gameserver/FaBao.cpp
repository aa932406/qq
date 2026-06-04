#include "stdafx.h"
#include "FaBao.h"
#include "Player.h"
#include "GameService.h"
#include "CfgData.h"
#include "KaiFuHuoDong.h"

CFaBao::CFaBao()
{
	OnCleanUp();
}

CFaBao::~CFaBao()
{

}

void CFaBao::OnCleanUp()
{
	bzero( m_FaBaoId, sizeof( m_FaBaoId ) );
	bzero( m_FaBaoDress, sizeof( m_FaBaoDress ) );
	bzero( m_UpFaBaoResource, sizeof( m_UpFaBaoResource ) );
}

void CFaBao::OnLoadFromDB( const PlayerDBData& dbData )
{
	memcpy( m_FaBaoId, dbData.m_FaBaoData.m_FaBaoId, sizeof( m_FaBaoId ) );
	memcpy( m_FaBaoDress, dbData.m_FaBaoData.m_FaBaoDress, sizeof( m_FaBaoDress ) );
	memcpy( m_UpFaBaoResource, dbData.m_FaBaoData.m_UpFaBaoResource, sizeof( m_UpFaBaoResource ) );
}

void CFaBao::OnSaveToDB( PlayerDBData& dbData )
{
	memcpy( dbData.m_FaBaoData.m_FaBaoId, m_FaBaoId, sizeof( m_FaBaoId ) );
	memcpy( dbData.m_FaBaoData.m_FaBaoDress, m_FaBaoDress, sizeof( m_FaBaoDress ) );
	memcpy( dbData.m_FaBaoData.m_UpFaBaoResource, m_UpFaBaoResource, sizeof( m_UpFaBaoResource ) );
}

void CFaBao::GetInterestsProtocol( ProcIdList& procList )
{
	procList.push_back( CM_ASK_FA_BAO_INFO );
	procList.push_back( CM_FA_BAO_PEI_YANG );
	procList.push_back( CM_BUY_FA_BAO_RES );
}

int32_t CFaBao::DispatchNetDatas( ProcId_t nProcId, Answer::NetPacket *inPacket )
{
	if ( NULL == m_pPlayer || NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	switch( nProcId )
	{
	case CM_ASK_FA_BAO_INFO:	return onAskFaBaoInfo( inPacket );
	case CM_FA_BAO_PEI_YANG:	return onFaBaoPeiYang( inPacket );
	case CM_BUY_FA_BAO_RES:		return onBuyFaBaoRes( inPacket );
	default:
		return ERR_SYETEM_ERR;
	}
}

bool CFaBao::IsFaBaoType( int8_t Type )
{
	return ( Type >= 0 && Type < FA_BAO_TYPE_COUNT );
}

bool CFaBao::IsFaBaoResType( int8_t Type )
{
	return ( Type >= 0 && Type < FA_BAO_RES_COUNT );
}

int32_t CFaBao::onBuyFaBaoRes( Answer::NetPacket* inPacket )
{
	if ( NULL == inPacket || NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t ResId = inPacket->readInt32();
	BuyFaBaoResCfg* pFaBaoResCfg = CFG_DATA.GetBuyFaBaoResCfg( ResId );
	if ( NULL == pFaBaoResCfg )
	{
		return ERR_SYETEM_ERR;
	}
	if ( !CheckFaBaoResType( static_cast<FaBaoResourceType>(pFaBaoResCfg->FaBaoResType) ) )
	{
		return ERR_SYETEM_ERR;
	}
	if ( pFaBaoResCfg->GetResValues <= 0 || pFaBaoResCfg->NeedGold <= 0 )
	{
		return ERR_SYETEM_ERR;
	}
	if ( m_pPlayer->GetCurrency(CURRENCY_GOLD) < pFaBaoResCfg->NeedGold )
	{
		return ERR_SYETEM_ERR;
	}
	if ( !m_pPlayer->DecCurrency( CURRENCY_GOLD, pFaBaoResCfg->NeedGold, GCR_BUY_FA_BAO_RES, pFaBaoResCfg->id ) )
	{
		return ERR_SYETEM_ERR;
	}
	AddFaBaoRes( static_cast<int8_t>(pFaBaoResCfg->FaBaoResType), pFaBaoResCfg->GetResValues );
	return ERR_OK;
}

int32_t CFaBao::onAskFaBaoInfo( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket || NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	int8_t nType = inPacket->readInt8();
	if ( !IsFaBaoType( nType ) )
	{
		return ERR_SYETEM_ERR;
	}
	sendFaBaoInfo( nType );
	return ERR_OK;
}

int32_t CFaBao::onFaBaoPeiYang( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket || NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	int8_t nType = inPacket->readInt8();
	if ( !IsFaBaoType( nType ) )
	{
		return ERR_SYETEM_ERR;
	}

	int32_t FaBaoLevel = m_FaBaoId[nType];
	CfgFaBao* pFaBao = CFG_DATA.GetFaBaoTable().GetFaBaoCfg( nType, FaBaoLevel );
	if ( NULL == pFaBao )
	{
		return ERR_SYETEM_ERR;
	}
	if ( pFaBao->NeedCurr < 0 )
	{
		return ERR_SYETEM_ERR;
	}
	if ( pFaBao->NeedCurr > m_UpFaBaoResource[nType] )
	{
		return ERR_SYETEM_ERR;
	}
	if ( NULL == CFG_DATA.GetFaBaoTable().GetFaBaoCfg( nType, FaBaoLevel + 1 ) )
	{
		return ERR_SYETEM_ERR;
	}
	if ( m_pPlayer->getLevel() < pFaBao->nNeedLevel )
	{
		return ERR_SYETEM_ERR;
	}

	m_UpFaBaoResource[nType] -= pFaBao->NeedCurr;
	m_pPlayer->sendGainInfo( nType + 100, -pFaBao->NeedCurr, m_pPlayer->benefitType() );
	m_FaBaoId[nType] = FaBaoLevel + 1;

	CfgFaBao* pNextFaBao = CFG_DATA.GetFaBaoTable().GetFaBaoCfg( nType, m_FaBaoId[nType] );
	if ( pNextFaBao != NULL && pNextFaBao->GongGaoId > 0 )
	{
		broadcastLevelUp( nType, pNextFaBao->GongGaoId );
	}

	sendFaBaoInfo( nType );
	m_pPlayer->recalcAttr();

	if ( nType == FA_BAO_TYPE_HUN_DUN )
	{
		KAI_FU_HUO_DONG.SendKaiFuHuoDongIcon( m_pPlayer );
	}

	GAME_SERVICE.replySuccess( m_pPlayer->getGateIndex(), inPacket->getProc(), m_FaBaoId[nType] );
	return ERR_OK;
}

void CFaBao::broadcastLevelUp( int8_t Type, int32_t GongGaoId )
{
	if ( NULL == m_pPlayer || !IsFaBaoType( Type ) )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_FA_BAO_GONG_GAO );
	if ( NULL == packet )
	{
		return;
	}
	packet->writeInt32( GongGaoId );
	packet->writeInt64( m_pPlayer->getCid() );
	packet->writeUTF8( m_pPlayer->getName() );
	packet->writeInt8( Type );
	packet->writeInt32( m_FaBaoId[Type] );
	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.worldBroadcast( packet );
}

void CFaBao::AddPlayerAttr()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}

	for ( int32_t i = 0; i < FA_BAO_TYPE_COUNT; ++i )
	{
		CfgFaBao* pFaBao = CFG_DATA.GetFaBaoTable().GetFaBaoCfg( static_cast<int8_t>(i), m_FaBaoId[i] );
		if ( NULL == pFaBao )
		{
			continue;
		}
		AttrAddonVector::iterator it = pFaBao->vAttr.begin();
		for ( ; it != pFaBao->vAttr.end(); ++it )
		{
			if ( static_cast<CObjAttrs::Index_T>( it->index ) == CObjAttrs::ATTR_BATTLE )
			{
				continue;
			}
			m_pPlayer->AddAttrValue( static_cast<CObjAttrs::Index_T>( it->index ), it->addon );
		}
	}
}

void CFaBao::sendFaBaoInfo( int8_t nType )
{
	if ( NULL == m_pPlayer || !IsFaBaoType( nType ) )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_FA_BAO_INFO );
	if ( NULL == packet )
	{
		return;
	}
	packet->writeInt8( nType );
	packet->writeInt32( m_FaBaoId[nType] );
	packet->writeInt32( m_UpFaBaoResource[nType] );
	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.sendPacketTo( m_pPlayer->getGateIndex(), packet );
}

void CFaBao::SendAllFaBaoInfo()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_ALL_FA_BAO_INFO );
	if ( NULL == packet )
	{
		return;
	}
	packet->writeInt8( static_cast<int8_t>( FA_BAO_TYPE_COUNT ) );
	for ( int32_t i = 0; i < FA_BAO_TYPE_COUNT; ++i )
	{
		packet->writeInt8( static_cast<int8_t>( i ) );
		packet->writeInt32( m_FaBaoId[i] );
		packet->writeInt32( m_UpFaBaoResource[i] );
	}
	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.sendPacketTo( m_pPlayer->getGateIndex(), packet );
}

void CFaBao::AddFaBaoRes( int8_t nType, int32_t AddValues )
{
	if ( NULL == m_pPlayer || !IsFaBaoType( nType ) || AddValues <= 0 )
	{
		return;
	}
	m_UpFaBaoResource[nType] += AddValues;
	m_pPlayer->sendGainInfo( nType + 100, AddValues, m_pPlayer->benefitType() );
	sendFaBaoInfo( nType );
}

bool CFaBao::DecFaBaoRes( int8_t nType, int32_t DecValues )
{
	if ( DecValues <= 0 )
	{
		return false;
	}
	if ( !IsFaBaoResType( nType ) )
	{
		return false;
	}
	m_UpFaBaoResource[nType] -= DecValues;
	sendFaBaoInfo( nType );
	return true;
}

int32_t CFaBao::GetFaBaoRes( int8_t nType )
{
	if ( !IsFaBaoResType( nType ) )
	{
		return 0;
	}
	return m_UpFaBaoResource[nType];
}

bool CFaBao::CheckFaBaoType( FaBaoType Type )
{
	return IsFaBaoType( static_cast<int8_t>( Type ) );
}

bool CFaBao::CheckFaBaoResType( FaBaoResourceType Type )
{
	return IsFaBaoResType( static_cast<int8_t>( Type ) );
}

int32_t CFaBao::GetFaBaoCount( int32_t& FaBaoSumLevel )
{
	int32_t FaBaoCount = 0;
	for ( int32_t i = 0; i < FA_BAO_TYPE_COUNT; ++i )
	{
		if ( m_FaBaoDress[i] == 1 )
		{
			FaBaoCount++;
			CfgFaBao* pFaBao = CFG_DATA.GetFaBaoTable().GetFaBaoCfg( static_cast<int8_t>(i), m_FaBaoId[i] );
			if ( NULL == pFaBao )
			{
				continue;
			}
			FaBaoSumLevel += pFaBao->FaBaoLevel;
		}
	}
	return FaBaoCount;
}

int32_t CFaBao::GetBattleFaBaoInfo( int8_t& FaBaoLevel, int32_t& FaBaoBattle )
{
	if ( m_FaBaoDress[FA_BAO_TYPE_FU_WEN] != 1 )
	{
		return ERR_SYETEM_ERR;
	}
	CfgFaBao* pFaBao = CFG_DATA.GetFaBaoTable().GetFaBaoCfg( FA_BAO_TYPE_FU_WEN, m_FaBaoId[FA_BAO_TYPE_FU_WEN] );
	if ( NULL == pFaBao )
	{
		return ERR_SYETEM_ERR;
	}
	FaBaoLevel = pFaBao->FaBaoLevel;
	AttrAddonVector::iterator it = pFaBao->vAttr.begin();
	for ( ; it != pFaBao->vAttr.end(); ++it )
	{
		if ( static_cast<CObjAttrs::Index_T>( it->index ) == CObjAttrs::ATTR_BATTLE )
		{
			FaBaoBattle += it->addon;
		}
	}
	return ERR_OK;
}

void CFaBao::PackFaBaoInfo( Answer::NetPacket *inPacket )
{
	if ( inPacket == NULL )
	{
		return;
	}
	inPacket->writeInt8( static_cast<int8_t>( FA_BAO_TYPE_COUNT ) );
	for ( int32_t i = 0; i < FA_BAO_TYPE_COUNT; ++i )
	{
		inPacket->writeInt8( static_cast<int8_t>( i ) );
		inPacket->writeInt32( m_FaBaoId[i] );
		inPacket->writeInt32( m_UpFaBaoResource[i] );
	}
}

int32_t CFaBao::GetFaBaoLevel( int8_t Type )
{
	if ( !IsFaBaoType( Type ) )
	{
		return 0;
	}
	return m_FaBaoId[Type];
}
