#include "ScoreShop.h"
#include "GameService.h"
#include "DataStruct.h"


enum ScoreShopType
{
	ChouJinagScore	= 1,
	BossScore		= 2,
};

ScoreShop::ScoreShop()
{
	m_ItemLimit.clear();
	m_ItemDayLimit.clear();
}

ScoreShop::	~ScoreShop()
{

}

void ScoreShop::OnLoadFromDB( const PlayerDBData& dbData )
{
	m_ItemLimit = dbData.m_ScoreShopData.m_LiMitMap;
}

void ScoreShop::OnSaveToDB( PlayerDBData& dbData )
{
	dbData.m_ScoreShopData.m_LiMitMap = m_ItemLimit;
}

void ScoreShop::GetInterestsProtocol( ProcIdList& procList )
{
	procList.push_back( CM_BUY_SCORE_SHOP_ITEM );
	procList.push_back( CM_ASK_SCORE_SHOP_INFO );
}

int32_t	ScoreShop::DispatchNetDatas( ProcId_t nProcId, Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}

	switch( nProcId )
	{
	case CM_BUY_SCORE_SHOP_ITEM:
		{
			return OnBuyItem( inPacket );
		}
	case CM_ASK_SCORE_SHOP_INFO:
		{
			SendLimitInfo();
			break;
		}
	default:
		return ERR_SYETEM_ERR;
	}
	return ERR_OK;
}

int32_t ScoreShop::OnBuyItem( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket || NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t Index	= inPacket->readInt32();
	int32_t Count	= inPacket->readInt32();
	if ( Count <= 0 )
	{
		return ERR_SYETEM_ERR;
	}

	// 先尝试新的 ScoreShopCfg 配置
	ScoreShopCfg* pScoreShop = CFG_DATA.GetScoreShopCfg( Index );
	if ( pScoreShop )
	{
		int32_t nLayNum = CFG_DATA.getOverlay( pScoreShop->itemId, pScoreShop->itemClass );
		if ( Count * pScoreShop->itemCount > nLayNum )
		{
			return ERR_SYETEM_ERR;
		}
		if ( m_pPlayer->GetBag().GetFreeSlotCount() <= 0 )
		{
			return ERR_SYETEM_ERR;
		}
		if ( pScoreShop->CostType < 0 || pScoreShop->CostValue <= 0 )
		{
			return ERR_SYETEM_ERR;
		}

		int32_t CostValue = Count * pScoreShop->CostValue;
		if ( m_pPlayer->GetCurrency( (CURRENCY_TYPE)pScoreShop->CostType ) < CostValue )
		{
			return ERR_SYETEM_ERR;
		}
		if ( pScoreShop->LimitType > 0 )
		{
			int32_t Limit = GetLimitCount( Index );
			if ( Count + Limit > pScoreShop->LimitCount )
			{
				return ERR_SYETEM_ERR;
			}
		}

		// 扣除物品消耗
		if ( pScoreShop->CostItems.size() > 0 )
		{
			ItemDataList CostItems;
			CostItems.assign( pScoreShop->CostItems.begin(), pScoreShop->CostItems.end() );
			for ( ItemDataList::iterator it = CostItems.begin(); it != CostItems.end(); ++it )
			{
				it->m_nCount *= Count;
			}
			if ( !m_pPlayer->GetBag().RemoveItem( CostItems, IDCR_SCORE_SHOP_COST ) )
			{
				return ERR_SYETEM_ERR;
			}
		}

		if ( !m_pPlayer->DecCurrency( (CURRENCY_TYPE)pScoreShop->CostType, CostValue, SCR_BUY_ITEM, Index ) )
		{
			return ERR_SYETEM_ERR;
		}

		MemChrBag item = {};
		item.itemId		= pScoreShop->itemId;
		item.itemClass	= pScoreShop->itemClass;
		item.itemCount	= pScoreShop->itemCount * Count;
		item.bind		= pScoreShop->bind;

		MemChrBagVector Items;
		Items.push_back( item );
		m_pPlayer->GetBag().AddItem( Items, IACR_SCORE_SHOP_GET );

		if ( pScoreShop->LimitType > 0 )
		{
			AddLimitCount( Index, Count );
			SendLimitInfo( Index );
		}
		if ( pScoreShop->IsRecord > 0 )
		{
			AddServerRecord( &item );
		}

		GAME_SERVICE.replySuccess( m_pPlayer->getGateIndex(), inPacket->getProc(), Index );
		return ERR_OK;
	}

	// 回退到旧的 ChouJiangShop 配置
	ChouJiangShop* pShop = CFG_DATA.GetChouJiangCfg().GetCJShop( Index );
	if ( NULL == pShop )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t nLayNum = CFG_DATA.getOverlay( pShop->ItemId, pShop->ItemClass );
	if ( nLayNum < Count )
	{
		return ERR_SYETEM_ERR;
	}
	if ( pShop->ItemClass == IC_PET_EGG )
	{
		if ( m_pPlayer->GetCharPet().GetEggBagAddHatchBagFreeCount() <= 0 )
		{
			return ERR_SYETEM_ERR;
		}
	}
	else if ( m_pPlayer->GetBag().GetbagFreeSize() <= 0 )
	{
		return ERR_SYETEM_ERR;
	}
	if ( pShop->Limit > 0 )
	{
		int32_t Limit = GetLimitCount( Index );
		if ( Limit + Count > pShop->Limit )
		{
			return ERR_SYETEM_ERR;
		}
	}
	
	if ( pShop->Type == ChouJinagScore )
	{
		if ( pShop->NeedScore <= 0 )
		{
			return ERR_SYETEM_ERR;
		}
		if (  m_pPlayer->GetCurrency( CURRENCY_CHOUJINAG ) < pShop->NeedScore * Count )
		{
			return ERR_SYETEM_ERR;
		}
		if ( !m_pPlayer->DecCurrency( CURRENCY_CHOUJINAG, pShop->NeedScore * Count, SCR_BUY_ITEM, Index ) )
		{
			return ERR_SYETEM_ERR;
		}
	}
	else if ( pShop->Type == BossScore )
	{
		if ( pShop->NeedScore <= 0 )
		{
			return ERR_SYETEM_ERR;
		}
		if (  m_pPlayer->GetCurrency( CURRENCY_BOSS ) < pShop->NeedScore*Count )
		{
			return ERR_SYETEM_ERR;
		}
		if ( !m_pPlayer->DecCurrency( CURRENCY_BOSS, pShop->NeedScore*Count, SCR_BUY_ITEM, Index ))
		{
			return ERR_SYETEM_ERR;
		}
	}
	else
	{
		return ERR_SYETEM_ERR;
	}
	MemChrBag item = {};
	item.itemId		= pShop->ItemId;	
	item.itemClass	= pShop->ItemClass;
	item.itemCount	= pShop->ItemCount*Count;	
	item.bind		= pShop->Bind;
	if ( item.itemClass == IC_PET_EGG )
	{
		std::list<int32_t> Eggs;
		Eggs.push_back(pShop->ItemId);
		m_pPlayer->GetCharPet().AddEggs(Eggs);
	}
	else
	{
		m_pPlayer->GetBag().AddItem( item, IACR_BUY_CHOU_JIANG_SHOP );
	}
	if ( pShop->Limit > 0 )
	{
		AddLimitCount( Index, Count );
		SendLimitInfo( Index );
	}
	GAME_SERVICE.replySuccess( m_pPlayer->getGateIndex(),inPacket->getProc() ,Index);
	return ERR_OK;
}

void ScoreShop::AddServerRecord( MemChrBag* Item )
{
	if ( NULL == m_pPlayer || NULL == Item )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_PROC, SM_SEND_SCORE_SHOP_RECORD );
	if ( NULL == packet )
	{
		return;
	}
	packet->writeInt32( m_pPlayer->getGateIndex() );
	packet->writeInt64( m_pPlayer->getCid() );
	packet->writeUTF8( m_pPlayer->getName() );
	packet->writeInt8( Item->itemClass );
	packet->writeInt32( Item->itemId );
	packet->writeInt32( Item->itemCount );
	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.sendPacket( packet );
}

void ScoreShop::SendLimitInfo( int32_t Index )
{
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_SCORE_SHOP_LIMIT_INFO );
	if (NULL == packet)
	{
		return;
	}
	if ( Index > 0 )
	{
		ItemLimitMap::iterator it = m_ItemLimit.find(Index);
		if ( it != m_ItemLimit.end() )
		{
			packet->writeInt32( 1 );
			packet->writeInt32( it->first );
			packet->writeInt32( it->second );
		}
		else
		{
			packet->writeInt32( 1 );
			packet->writeInt32( 0 );
			packet->writeInt32( 0 );
		}

	}
	else
	{
		int32_t nCount = m_ItemLimit.size();
		packet->writeInt32( nCount );
		ItemLimitMap::iterator iter = m_ItemLimit.begin();
		ItemLimitMap::iterator eiter = m_ItemLimit.end();
		for ( ; iter != eiter; ++iter )
		{
			packet->writeInt32( iter->first );
			packet->writeInt32( iter->second );
		}
	}

	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);
}

int32_t ScoreShop::GetLimitCount( int32_t index )
{
	ItemLimitMap::iterator it = m_ItemLimit.find(index);
	if ( it != m_ItemLimit.end() )
	{
		return it->second;
	}
	return 0;
}

void ScoreShop::AddLimitCount( int32_t index, int32_t count )
{
	ItemLimitMap::iterator it = m_ItemLimit.find(index);
	if ( it != m_ItemLimit.end() )
	{
		it->second += count;
	}
	else
	{
		m_ItemLimit[index] = count;
	}
}

void ScoreShop::OnDaySwitch( int32_t nDiffDays )
{
	// 清理每日限制，保留每周限制(LimitType!=1或不存在的配置视为每日限制)
	ItemLimitMap::iterator it = m_ItemLimit.begin();
	while ( it != m_ItemLimit.end() )
	{
		ScoreShopCfg* pShop = CFG_DATA.GetScoreShopCfg( it->first );
		if ( pShop == NULL || pShop->LimitType == 1 )
		{
			m_ItemLimit.erase( it++ );
		}
		else
		{
			++it;
		}
	}
	SendLimitInfo();
}

void ScoreShop::GetIconState( IconStateList& iconList )
{
	ShowIcon icon = {};
	icon.nId		= SCORE_SHOP;
	icon.nState		= AS_RUNNING;
	icon.nLeftTime	= -1;
	iconList.push_back( icon );
}
