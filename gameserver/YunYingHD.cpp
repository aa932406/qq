#include "YunYingHD.h"
#include "CfgData.h"
#include "Player.h"
#include "EquipManager.h"
#include "GameService.h"
#include "DBService.h"

CYunYingHD::CYunYingHD()
{
	m_TotalChongZhiDay = 0;
	m_nLastTick = 0;
	m_nLastTeHui = 0;
}

CYunYingHD::~CYunYingHD()
{

}

void CYunYingHD::OnLoadFromDB( const PlayerDBData& dbData )
{
	CfgTotalChongZhi* pCfg = CFG_DATA.GetTotalChongZhiCfg( 1 );
	if ( NULL == pCfg )
	{
		return;
	}
	m_TotalChongZhiDay = pCfg->NewServerday;
}

void CYunYingHD::OnSaveToDB( PlayerDBData& dbData )
{

}

void CYunYingHD::GetInterestsProtocol( ProcIdList& procList )
{
	procList.push_back( CM_GET_SHOU_CHONG_REWARD );
	procList.push_back( CM_ASK_SHOU_CHONG_INFO );

	procList.push_back( CM_ASK_SHOU_TE_HUI_INFO );
	procList.push_back( CM_BUY_TEI_HUI_GIFT );

	procList.push_back( CM_ASK_EVERYDAY_CHONG_ZHI_INFO );
	procList.push_back( CM_GET_EVERYDAY_CHONG_ZHI_LI_BAO );

	procList.push_back( CM_GET_THREE_PET_GIFT );

	procList.push_back( CM_GET_TOTAL_CHONG_ZHI_GIFT );
	procList.push_back( CM_ASK_TOTAL_CHONG_ZHI_INFO );

	procList.push_back( CM_GET_MOBILE_PHONE_GIFT );
	procList.push_back( CM_GET_ZERO_BUY_PET_GIFT );
}

int32_t	CYunYingHD::DispatchNetDatas( ProcId_t nProcId, Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	switch( nProcId )
	{
	case CM_GET_SHOU_CHONG_REWARD:
		{
			return OnGetShouChongReward( inPacket );
		}
	case CM_ASK_SHOU_CHONG_INFO:
		{
			SendShouChongInfo();
			break;
		}
	case CM_BUY_TEI_HUI_GIFT:
		{
			return OnGetBuyTeHuiItem( inPacket );
		}
	case CM_ASK_SHOU_TE_HUI_INFO:
		{
			SendTeHuiInfo();
			break;
		}
	case CM_ASK_EVERYDAY_CHONG_ZHI_INFO:
		{
			SendEveryDayChongZhiInfo();
			break;
		}
	case CM_GET_EVERYDAY_CHONG_ZHI_LI_BAO:
		{
			return OnGetEveryDayChongZhiGift( inPacket );
		}
	case CM_GET_THREE_PET_GIFT:
		{
			return OnGetThreePetGift( inPacket );
		}
	case CM_GET_TOTAL_CHONG_ZHI_GIFT:
		{
			return OnGetTotalChongZhi( inPacket );
		}
	case CM_ASK_TOTAL_CHONG_ZHI_INFO:
		{
			SendTotalChongZhiInfo();
			break;
		}
	case CM_GET_MOBILE_PHONE_GIFT:
		{
			return OnGetMobilePhoneGift( inPacket );
		}
	case CM_GET_ZERO_BUY_PET_GIFT:
		{
			return OnGetZeroBuyPetGift( inPacket );
		}
	default:
		break;
	}
	return ERR_OK;
}

void CYunYingHD::OnCleanUp()
{
	m_TotalChongZhiDay = 0;
	m_nLastTick = 0;
	m_nLastTeHui = 0;
}

void CYunYingHD::OnUpdate( int64_t curTick )
{
	if ( curTick - m_nLastTick > 999 )
	{
		m_nLastTick = curTick;
		checkTeHuiTime();
	}
}

void CYunYingHD::OnDaySwitch( int32_t nDiffDays )
{
	SendEveryDayChongZhiInfo();
	SendEveryDayChongZhiIcon();
	SendTotalChongZhiIcon();
}

int32_t	CYunYingHD::OnGetShouChongReward( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket || NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	if ( GetShouChongState() != 1 )
	{
		return ERR_SYETEM_ERR;
	}
	MemChrEquipBagVector& Item = CFG_DATA.GetShouChongLiBao();
	MemChrBagVector addItemVt;
	MemEquipVector vEquip;
	if (!Item.empty())
	{
		for (MemChrEquipBagVector::iterator it =Item.begin();it !=  Item.end();++it)
		{
			if (m_pPlayer->getJob() == it->job || it->job == 0 )
			{
				MemChrBag  chrbag = {};
				chrbag.itemId    = it->id;
				chrbag.itemClass = it->type;
				chrbag.itemCount = it->count;
				chrbag.bind		 = it->bind;
				chrbag.endTime	 = 0;
				if ( chrbag.itemClass == IC_EQUIP )
				{
					MemEquip equip = EQUIP_MANAGER.CreateMemEquip( chrbag.itemId, m_pPlayer->getSid(), m_pPlayer->getCid(), it->star );
					if ( equip.base <= 0 || equip.id <= 0 )
					{
						return false;
					}
					chrbag.srcId		= equip.id;	
					vEquip.push_back( equip );
				}
				addItemVt.push_back(chrbag);
			}
		}
	}
	m_pPlayer->autoUseItem(addItemVt);
	if ( !m_pPlayer->GetBag().AddItemsAndEggs( addItemVt,  IACR_SHOU_CHONG ) )
	{
		return ERR_SYETEM_ERR;
	}
	if ( !vEquip.empty() )
	{
		m_pPlayer->sendEquipInfo( vEquip );
	} 
	m_pPlayer->updateRecord( RP_SHOU_CHONG_LI_BAO, 1 );
	GongGao( BCI_SHOU_CHONG_LI_BAO );
	SendShouChongIcon();
	SendShouChongInfo();

	LogDailyGift logDailyGift = {};
	logDailyGift.cid = m_pPlayer->getCid();
	logDailyGift.type = DGT_SHOU_CHONG;
	logDailyGift.giftid = 0;
	logDailyGift.time = m_pPlayer->getNow();

	DB_SERVICE.InsertDailyGiftLog( logDailyGift );
	return ERR_OK;
}

int32_t	CYunYingHD::OnGetBuyTeHuiItem( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket || NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	int8_t		Index = inPacket->readInt8();
	NewServerFavorable* pCfg = CFG_DATA.GetNewServerFavorable( Index );
	if ( NULL == pCfg )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t OldRecord = m_pPlayer->getRecord( RP_BUY_TE_HUI_RECORD );
	int32_t NewRecord = OldRecord | ( 1 << (Index - 1) );
	if ( OldRecord == NewRecord )
	{
		return ERR_SYETEM_ERR;
	}
	if ( pCfg->NeedGold <= 0 )
	{
		return ERR_SYETEM_ERR;
	}
	int64_t HaveMoney = m_pPlayer->GetCurrency( CURRENCY_GOLD );
	if ( HaveMoney < pCfg->NeedGold )
	{
		return ERR_SYETEM_ERR;
	}
	MemChrEquipBagVector& Item = pCfg->ItemVector;
	MemChrBagVector addItemVt;
	MemEquipVector vEquip;
	if (!Item.empty())
	{
		for (MemChrEquipBagVector::iterator it =Item.begin();it !=  Item.end();++it)
		{
			if (m_pPlayer->getJob() == it->job || it->job == 0 )
			{
				MemChrBag  chrbag = {};
				chrbag.itemId    = it->id;
				chrbag.itemClass = it->type;
				chrbag.itemCount = it->count;
				chrbag.bind		 = it->bind;
				chrbag.endTime	 = 0;
				if ( chrbag.itemClass == IC_EQUIP )
				{
					MemEquip equip = EQUIP_MANAGER.CreateMemEquip( chrbag.itemId, m_pPlayer->getSid(), m_pPlayer->getCid(), it->star );
					if ( equip.base <= 0 || equip.id <= 0 )
					{
						return false;
					}
					chrbag.srcId		= equip.id;	
					vEquip.push_back( equip );
				}
				addItemVt.push_back(chrbag);
			}
		}
	}
	if ( !m_pPlayer->DecCurrency( CURRENCY_GOLD, pCfg->NeedGold, GCR_BUY_TE_HUI_LI_BAO, Index ))
	{
		return ERR_SYETEM_ERR;
	}
	if ( !m_pPlayer->GetBag().AddItemsAndEggs( addItemVt,  IACR_NEW_SERVER_TE_HUI ) )
	{
		return ERR_SYETEM_ERR;
	}
	if ( !vEquip.empty() )
	{
		m_pPlayer->sendEquipInfo( vEquip );
	} 
	m_pPlayer->updateRecord( RP_BUY_TE_HUI_RECORD, NewRecord );
	GongGaoList::iterator it = pCfg->GongGaoInfo.begin();
	for ( ; it != pCfg->GongGaoInfo.end(); ++it )
	{
		if ( it->Job == 0 || it->Job == m_pPlayer->getJob() )
		{
			GongGao( it->GongGaoId );
		}
	}
	SendTeHuiInfo();
	SendTeHuiIcon();
	return ERR_OK;
}

int32_t	CYunYingHD::OnGetEveryDayChongZhiGift( Answer::NetPacket *inPacket )
{
	if ( NULL == m_pPlayer || NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	int8_t	Index = inPacket->readInt8();
	CfgEverydayChongZhi* pCfg =CFG_DATA.GetEveryDayChongZhiCfg( Index );
	if ( NULL == pCfg )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t TodayPayed = m_pPlayer->GetTodayPayGold();
	if ( pCfg->NeedGold > TodayPayed )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t OldRecord = m_pPlayer->getRecord( PR_EVERY_DAY_CHONG_ZHI_GIFT_RECORD );
	int32_t NewRecord = OldRecord | ( 1 << (Index - 1) );
	if ( OldRecord == NewRecord )
	{
		return ERR_SYETEM_ERR;
	}
	MemChrEquipBagVector& Item = pCfg->ItemVector;
	MemChrBagVector addItemVt;
	MemEquipVector vEquip;
	if (!Item.empty())
	{
		for (MemChrEquipBagVector::iterator it =Item.begin();it !=  Item.end();++it)
		{
			if (m_pPlayer->getJob() == it->job || it->job == 0 )
			{
				MemChrBag  chrbag = {};
				chrbag.itemId    = it->id;
				chrbag.itemClass = it->type;
				chrbag.itemCount = it->count;
				chrbag.bind		 = it->bind;
				chrbag.endTime	 = 0;
				if ( chrbag.itemClass == IC_EQUIP )
				{
					MemEquip equip = EQUIP_MANAGER.CreateMemEquip( chrbag.itemId, m_pPlayer->getSid(), m_pPlayer->getCid(), it->star );
					if ( equip.base <= 0 || equip.id <= 0 )
					{
						return false;
					}
					chrbag.srcId		= equip.id;	
					vEquip.push_back( equip );
				}
				addItemVt.push_back(chrbag);
			}
		}
	}
	if ( !m_pPlayer->GetBag().AddItemsAndEggs( addItemVt,  IACR_EVERYDAY_CHONG_ZHI ) )
	{
		return ERR_SYETEM_ERR;
	}
	if ( !vEquip.empty() )
	{
		m_pPlayer->sendEquipInfo( vEquip );
	} 
	m_pPlayer->updateRecord( PR_EVERY_DAY_CHONG_ZHI_GIFT_RECORD, NewRecord );
	GongGaoList::iterator it = pCfg->GongGaoInfo.begin();
	for ( ; it != pCfg->GongGaoInfo.end(); ++it )
	{
		if ( it->Job == 0 || it->Job == m_pPlayer->getJob() )
		{
			GongGao( it->GongGaoId );
		}
	}
	SendEveryDayChongZhiInfo();
	SendEveryDayChongZhiIcon();

	LogDailyGift logDailyGift = {};
	logDailyGift.cid = m_pPlayer->getCid();
	logDailyGift.type = DGT_SHOU_CHONG;
	logDailyGift.giftid = Index;
	logDailyGift.time = m_pPlayer->getNow();

	DB_SERVICE.InsertDailyGiftLog( logDailyGift );
	return ERR_OK;
}

int32_t	CYunYingHD::OnGetThreePetGift( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket || NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	if ( m_pPlayer->getRecord( RP_THREE_PET_GIFT ) > 0 )
	{
		return ERR_SYETEM_ERR;
	}
	NewServerFavorable&	pCfg = CFG_DATA.GetThreePetGift();
	if ( pCfg.NeedGold <= 0 )
	{
		return ERR_SYETEM_ERR;
	}
	int64_t HaveMoney = m_pPlayer->GetCurrency( CURRENCY_GOLD );
	if ( HaveMoney < pCfg.NeedGold )
	{
		return ERR_SYETEM_ERR;
	}
	MemChrEquipBagVector& Item = pCfg.ItemVector;
	MemChrBagVector addItemVt;
	MemEquipVector vEquip;
	if (!Item.empty())
	{
		for (MemChrEquipBagVector::iterator it =Item.begin();it !=  Item.end();++it)
		{
			if (m_pPlayer->getJob() == it->job || it->job == 0 )
			{
				MemChrBag  chrbag = {};
				chrbag.itemId    = it->id;
				chrbag.itemClass = it->type;
				chrbag.itemCount = it->count;
				chrbag.bind		 = it->bind;
				chrbag.endTime	 = 0;
				if ( chrbag.itemClass == IC_EQUIP )
				{
					MemEquip equip = EQUIP_MANAGER.CreateMemEquip( chrbag.itemId, m_pPlayer->getSid(), m_pPlayer->getCid(), it->star );
					if ( equip.base <= 0 || equip.id <= 0 )
					{
						return false;
					}
					chrbag.srcId		= equip.id;	
					vEquip.push_back( equip );
				}
				addItemVt.push_back(chrbag);
			}
		}
	}
	if ( !m_pPlayer->GetBag().AddItemsAndEggs( addItemVt,  IACR_NEW_SERVER_TE_HUI ) )
	{
		return ERR_SYETEM_ERR;
	}
	if ( !m_pPlayer->DecCurrency( CURRENCY_GOLD, pCfg.NeedGold, GCR_BUY_THREE_PET_LI_BAO, pCfg.Index ))
	{
		return ERR_SYETEM_ERR;
	}
	m_pPlayer->updateRecord( RP_THREE_PET_GIFT , 1 );
	if ( !vEquip.empty() )
	{
		m_pPlayer->sendEquipInfo( vEquip );
	} 
	GongGaoList::iterator it = pCfg.GongGaoInfo.begin();
	for ( ; it != pCfg.GongGaoInfo.end(); ++it )
	{
		if ( it->Job == 0 || it->Job == m_pPlayer->getJob() )
		{
			GongGao( it->GongGaoId );
		}
	}
	GAME_SERVICE.replySuccess( m_pPlayer->getGateIndex(), inPacket->getProc() );
	SendThreePetGiftIcon();
	return ERR_OK;
}

int32_t	CYunYingHD::OnGetTotalChongZhi( Answer::NetPacket *inPacket )
{
	if ( NULL == m_pPlayer || NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	int8_t	Index = inPacket->readInt8();
	CfgTotalChongZhi* pCfg =CFG_DATA.GetTotalChongZhiCfg( Index );
	if ( NULL == pCfg )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t ZhongZhiValues = m_pPlayer->getRecord(RP_TOTAL_CHI_ZHI_VALUES);
	if ( pCfg->NeedGold > ZhongZhiValues )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t OldRecord = m_pPlayer->getRecord( RP_TOTAL_CHI_ZHI_REWARD );
	int32_t NewRecord = OldRecord | ( 1 << (Index - 1) );
	if ( OldRecord == NewRecord )
	{
		return ERR_SYETEM_ERR;
	}
	MemChrEquipBagVector& Item = pCfg->ItemVector;
	MemChrBagVector addItemVt;
	MemEquipVector vEquip;
	if (!Item.empty())
	{
		for (MemChrEquipBagVector::iterator it =Item.begin();it !=  Item.end();++it)
		{
			if (m_pPlayer->getJob() == it->job || it->job == 0 )
			{
				MemChrBag  chrbag = {};
				chrbag.itemId    = it->id;
				chrbag.itemClass = it->type;
				chrbag.itemCount = it->count;
				chrbag.bind		 = it->bind;
				chrbag.endTime	 = 0;
				if ( chrbag.itemClass == IC_EQUIP )
				{
					MemEquip equip = EQUIP_MANAGER.CreateMemEquip( chrbag.itemId, m_pPlayer->getSid(), m_pPlayer->getCid(), it->star );
					if ( equip.base <= 0 || equip.id <= 0 )
					{
						return false;
					}
					chrbag.srcId		= equip.id;	
					vEquip.push_back( equip );
				}
				addItemVt.push_back(chrbag);
			}
		}
	}
	if ( !m_pPlayer->GetBag().AddItemsAndEggs( addItemVt,  IACR_TOTAL_CHONG_ZHI ) )
	{
		return ERR_SYETEM_ERR;
	}
	if ( !vEquip.empty() )
	{
		m_pPlayer->sendEquipInfo( vEquip );
	} 
	m_pPlayer->updateRecord( RP_TOTAL_CHI_ZHI_REWARD, NewRecord );
	GongGaoList::iterator it = pCfg->GongGaoInfo.begin();
	for ( ; it != pCfg->GongGaoInfo.end(); ++it )
	{
		if ( it->Job == 0 || it->Job == m_pPlayer->getJob() )
		{
			GongGao( it->GongGaoId );
		}
	}
	SendTotalChongZhiIcon();
	SendTotalChongZhiInfo();
	return ERR_OK;
}

int8_t CYunYingHD::GetShouChongState()
{
	if ( m_pPlayer->GetTotalPayGold() <= 0 )
	{
		return 0;
	}

	if ( m_pPlayer->getRecord(RP_SHOU_CHONG_LI_BAO) == 0 )
	{
		return 1;
	}
	else
	{
		return 2;
	}
}


void CYunYingHD::SendShouChongInfo()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_SEND_SHOU_CHONG_INFO);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt8( GetShouChongState() );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);		
}

void CYunYingHD::SendShouChongIcon()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	if ( !m_pPlayer->GetPlayerFunctionOpen().IsOpened( FT_SHOU_CHONG_ICON ) )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_SEND_ONE_ICON);
	if (NULL == packet)
	{
		return;
	}
	ShowIcon stu = GetShouChongIconStu();
	packet->writeInt32( stu.nId );
	packet->writeInt8(  stu.nState );
	packet->writeInt32( stu.nLeftTime );
	packet->writeInt8( stu.IconLeft );
	packet->writeInt32( stu.IconRight );
	packet->writeInt8( stu.Effects );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);	
}

void CYunYingHD::GetShouChongIconState( IconStateList& IconList )
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	if ( !m_pPlayer->GetPlayerFunctionOpen().IsOpened( FT_SHOU_CHONG_ICON ) )
	{
		return;
	}
	if ( GetShouChongState() > 1 )
	{
		return;
	}
	IconList.push_back( GetShouChongIconStu() );
}

ShowIcon CYunYingHD::GetShouChongIconStu()
{
	ShowIcon stu = {};
	stu.nId			= CHOU_CHONG;
	if ( GetShouChongState() > 1 )
	{
		stu.nState  = AS_TIME_OUT;
	}
	else
	{
		stu.nState		= AS_RUNNING;
	}
	stu.nLeftTime	= -1;
	stu.Effects		= 1;
	return stu;
}

void CYunYingHD::SendTeHuiInfo()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_SEND_NEW_SERVER_FAVORABLE);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32( m_pPlayer->getRecord( RP_BUY_TE_HUI_RECORD ) );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);	
}

void CYunYingHD::SendTeHuiIcon()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	if ( !m_pPlayer->GetPlayerFunctionOpen().IsOpened( FT_NEW_SERVER_TE_HUI ) )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_SEND_ONE_ICON);
	if (NULL == packet)
	{
		return;
	}
	ShowIcon stu = GetTeHuiIconStu();
	packet->writeInt32( stu.nId );
	packet->writeInt8(  stu.nState );
	packet->writeInt32( stu.nLeftTime );
	packet->writeInt8( stu.IconLeft );
	packet->writeInt32( stu.IconRight );
	packet->writeInt8( stu.Effects );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);	
}

void CYunYingHD::GetTeHuiIconState( IconStateList& IconList )
{
 	if ( NULL == m_pPlayer )
 	{
 		return;
 	}
 	if ( !m_pPlayer->GetPlayerFunctionOpen().IsOpened( FT_NEW_SERVER_TE_HUI ) )
 	{
 		return;
 	}
	if ( !IsHaveTeHuiGift() )
	{
		return;
	}
 	IconList.push_back( GetTeHuiIconStu() );
}

ShowIcon CYunYingHD::GetTeHuiIconStu()
{
 	ShowIcon stu = {};
 	stu.nId			= NEW_SERVER_TE_HUI;
	if ( !IsHaveTeHuiGift() )
	{
		stu.nState		= AS_TIME_OUT;
	}
	else
	{
		stu.nState		= AS_RUNNING;
	}
 	stu.nLeftTime	= -1;
 	stu.Effects		= 1;
 	return stu;
}

bool CYunYingHD::IsHaveTeHuiGift()
{
	int32_t OldRecord = m_pPlayer->getRecord( RP_BUY_TE_HUI_RECORD );
	NewServerFavorableMap CfgTeHuiMap = CFG_DATA.GetNewServerFavorableCfg();
	NewServerFavorableMap::iterator it = CfgTeHuiMap.begin();
	for ( ; it != CfgTeHuiMap.end(); ++it )
	{
		int32_t NewRecord = OldRecord | ( 1 << ( it->first - 1 ) );
		if ( OldRecord == NewRecord )
		{
			continue;
		}
		return true;
	}
	return false;
}

void CYunYingHD::SendEveryDayChongZhiInfo()
{
	SendEveryDayChongZhiInfo( 3 );
}

void CYunYingHD::SendEveryDayChongZhiInfo( int8_t nType )
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	if ( nType != 3 )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_SEND_EVERYDAY_CHONG_ZHI);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt8( nType );
	packet->writeInt32( m_pPlayer->GetTodayPayGold() );
	packet->writeInt32( m_pPlayer->getRecord( PR_EVERY_DAY_CHONG_ZHI_GIFT_RECORD ) );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);
}

void CYunYingHD::SendEveryDayChongZhiIcon()
{
	SendEveryDayChongZhiIcon( 3 );
}

void CYunYingHD::SendEveryDayChongZhiIcon( int8_t nType )
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	if ( !CanShowEveryChongZhiIcon() )
	{
		return;
	}
	if ( !m_pPlayer->GetPlayerFunctionOpen().IsOpened( FT_EVERYDAY_SHOUCHONG ) )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_SEND_ONE_ICON);
	if (NULL == packet)
	{
		return;
	}
	ShowIcon stu = GetEveryDayChongZhiIconStu();
	packet->writeInt32( stu.nId );
	packet->writeInt8(  stu.nState );
	packet->writeInt32( stu.nLeftTime );
	packet->writeInt8( stu.IconLeft );
	packet->writeInt32( stu.IconRight );
	packet->writeInt8( stu.Effects );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);	
}

void CYunYingHD::GetEVeryDayChongZhiIconState( IconStateList& IconList )
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	if ( !CanShowEveryChongZhiIcon() )
	{
		return;
	}
	if ( !m_pPlayer->GetPlayerFunctionOpen().IsOpened( FT_EVERYDAY_SHOUCHONG ) )
	{
		return;
	}
	if ( !IsHaveEveryDayChongZhiGift() )
	{
		return;
	}
	IconList.push_back( GetEveryDayChongZhiIconStu() );
}

ShowIcon CYunYingHD::GetEveryDayChongZhiIconStu()
{
	ShowIcon stu = {};
	stu.nId			= EVERYDAY_CHOU_ZHI;
	if ( !IsHaveEveryDayChongZhiGift() )
	{
		stu.nState		= AS_TIME_OUT;
	}
	else
	{
		stu.nState		= AS_RUNNING;
	}
	stu.nLeftTime	= -1;
	stu.Effects		= 1;
	return stu;
}

bool CYunYingHD::IsHaveEveryDayChongZhiGift()
{
	int32_t OldRecord = m_pPlayer->getRecord( PR_EVERY_DAY_CHONG_ZHI_GIFT_RECORD );
	EverydayChongZhiMap CfgEveryDayChongZhiMap = CFG_DATA.GetEveryDayTable();
	EverydayChongZhiMap::iterator it = CfgEveryDayChongZhiMap.begin();
	for ( ; it != CfgEveryDayChongZhiMap.end(); ++it )
	{
		int32_t NewRecord = OldRecord | ( 1 << ( it->first - 1 ) );
		if ( OldRecord == NewRecord )
		{
			continue;
		}
		return true;
	}
	return false;
}

void CYunYingHD::GongGao( int32_t GongGaoId )
{
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM );
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32( GongGaoId );
	packet->writeUTF8( m_pPlayer->getName() );
	packet->writeInt64( m_pPlayer->getCid() );
	packet->setSize(packet->getWOffset());
	packet->setProc( SM_SEND_NOTICE_PARAM );
	GAME_SERVICE.worldBroadcast(packet);
}


void CYunYingHD::SendThreePetGiftIcon()
{
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_SEND_ONE_ICON);
	if (NULL == packet)
	{
		return;
	}
	ShowIcon stu = GetThreePetGiftIconStu();
	packet->writeInt32( stu.nId );
	packet->writeInt8(  stu.nState );
	packet->writeInt32( stu.nLeftTime );
	packet->writeInt8( stu.IconLeft );
	packet->writeInt32( stu.IconRight );
	packet->writeInt8( stu.Effects );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);	
}

void CYunYingHD::GetThreePetGiftIconState( IconStateList& IconList )
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	if ( m_pPlayer->getRecord( RP_THREE_PET_GIFT ) > 0 )
	{
		return;
	}
	IconList.push_back( GetThreePetGiftIconStu() );
}

ShowIcon CYunYingHD::GetThreePetGiftIconStu()
{
	ShowIcon stu = {};
	if ( NULL == m_pPlayer )
	{
		return stu;
	}
	stu.nId			= THREE_PET_ICON;
	if ( m_pPlayer->getRecord( RP_THREE_PET_GIFT ) > 0 )
	{
		stu.nState		= AS_TIME_OUT;
	}
	else
	{
		stu.nState		= AS_RUNNING;
	}
	stu.nLeftTime	= -1;
	return stu;
}

int32_t CYunYingHD::HaveTotalChongZhiRewardCount()
{
	int32_t HaveCount = 0;
	int32_t OldRecord = m_pPlayer->getRecord( RP_TOTAL_CHI_ZHI_REWARD );
	TotalChongZhiMap CfgTotalChongZhiMap = CFG_DATA.GetTotalChongZhiTable();
	TotalChongZhiMap::iterator it = CfgTotalChongZhiMap.begin();
	for ( ; it != CfgTotalChongZhiMap.end(); ++it )
	{
		int32_t NewRecord = OldRecord | ( 1 << ( it->first - 1 ) );
		if ( OldRecord == NewRecord )
		{
			continue;
		}
		if ( it->second.NeedGold > m_pPlayer->getRecord(RP_TOTAL_CHI_ZHI_VALUES) )
		{
			continue;
		}
		HaveCount++;
	}
	return HaveCount;
}

void CYunYingHD::SendTotalChongZhiIcon()
{
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_SEND_ONE_ICON);
	if (NULL == packet)
	{
		return;
	}
	ShowIcon stu = GetTotalChongZhiIconStu();
	packet->writeInt32( stu.nId );
	packet->writeInt8(  stu.nState );
	packet->writeInt32( stu.nLeftTime );
	packet->writeInt8( stu.IconLeft );
	packet->writeInt32( stu.IconRight );
	packet->writeInt8( stu.Effects );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);	
}

bool CYunYingHD::CanShowEveryChongZhiIcon()
{
	if ( NULL == m_pPlayer )
	{
		return false;
	}
	if ( m_pPlayer->GetTotalPayGold() <= 0 )
	{
		return false;
	}
	int32_t nRecord = m_pPlayer->getRecord( RP_SHOU_CHONG_LI_BAO );
	return nRecord != (nRecord | 2);
}

int32_t CYunYingHD::getTeHuiLimitTime()
{
	if ( NULL == m_pPlayer )
	{
		return 0;
	}
	int32_t nLimitTime = 0;
	int32_t nStartTime = m_pPlayer->getCreateTime();
	int32_t nNowTime = m_pPlayer->getNow();
	int32_t OldRecord = m_pPlayer->getRecord( RP_BUY_TE_HUI_RECORD );
	NewServerFavorableMap CfgTeHuiMap = CFG_DATA.GetNewServerFavorableCfg();
	NewServerFavorableMap::iterator it = CfgTeHuiMap.begin();
	for ( ; it != CfgTeHuiMap.end(); ++it )
	{
		int32_t NewRecord = OldRecord | ( 1 << ( it->first - 1 ) );
		if ( OldRecord != NewRecord )
		{
			int32_t nTime = nStartTime + it->second.nLimitTime - nNowTime;
			if ( nTime > 0 && ( nLimitTime == 0 || nLimitTime > nTime ) )
			{
				nLimitTime = nTime;
			}
		}
	}
	return nLimitTime;
}

void CYunYingHD::checkTeHuiTime()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	int32_t nLimitTime = 0;
	int8_t nIndex = 0;
	int32_t nStartTime = m_pPlayer->getCreateTime();
	int32_t nNowTime = m_pPlayer->getNow();
	int32_t OldRecord = m_pPlayer->getRecord( RP_BUY_TE_HUI_RECORD );
	NewServerFavorableMap CfgTeHuiMap = CFG_DATA.GetNewServerFavorableCfg();
	NewServerFavorableMap::iterator it = CfgTeHuiMap.begin();
	for ( ; it != CfgTeHuiMap.end(); ++it )
	{
		int32_t NewRecord = OldRecord | ( 1 << ( it->first - 1 ) );
		if ( OldRecord != NewRecord )
		{
			int32_t nTime = nStartTime + it->second.nLimitTime - nNowTime;
			if ( nTime > 0 && ( nLimitTime == 0 || nLimitTime > nTime ) )
			{
				nIndex = it->first;
				nLimitTime = nTime;
			}
		}
	}
	if ( m_nLastTeHui != nIndex )
	{
		m_nLastTeHui = nIndex;
		SendTeHuiIcon();
	}
}

void CYunYingHD::SendZeroBuyPetIcon()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	int32_t nEndTime = m_pPlayer->getRecord( 37502 );
	if ( nEndTime <= 0 )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_ONE_ICON );
	if ( NULL == packet )
	{
		return;
	}
	ShowIcon stu = {};
	stu.nId = 174;
	stu.nState = AS_RUNNING;
	stu.nLeftTime = nEndTime - m_pPlayer->getNow();
	if ( stu.nLeftTime < 0 )
	{
		stu.nLeftTime = 0;
	}
	packet->writeInt32( stu.nId );
	packet->writeInt8( stu.nState );
	packet->writeInt32( stu.nLeftTime );
	packet->writeInt8( stu.IconLeft );
	packet->writeInt32( stu.IconRight );
	packet->writeInt8( stu.Effects );
	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.sendPacketTo( m_pPlayer->getGateIndex(), packet );
}

void CYunYingHD::SendMobilePhoneGiftIcon()
{
	// Stub - Mobile phone gift icon not yet implemented
}

void CYunYingHD::SendAdultGiftIcon()
{
	// Stub - Adult gift icon not yet implemented
}

int32_t CYunYingHD::OnGetMobilePhoneGift( Answer::NetPacket *inPacket )
{
	// Stub - Mobile phone gift not yet implemented with DB integration
	return ERR_SYETEM_ERR;
}

int32_t CYunYingHD::OnGetZeroBuyPetGift( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket || NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	if ( m_pPlayer->getRecord( 37502 ) <= 0 || m_pPlayer->getRecord( 37503 ) > 0 )
	{
		return ERR_SYETEM_ERR;
	}
	return ERR_SYETEM_ERR;
}

void CYunYingHD::SendTotalChongZhiInfo()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_SEND_TOTAL_CHONG_ZHI);
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt32( m_pPlayer->getRecord( RP_TOTAL_CHI_ZHI_VALUES ) );
	packet->writeInt32( m_pPlayer->getRecord( RP_TOTAL_CHI_ZHI_REWARD ) );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);	
}

void CYunYingHD::GetTotalChongZhiIconState( IconStateList& IconList )
{
	int32_t ServerDay = CFG_DATA.getServerDiffTime();
	if ( HaveTotalChongZhiRewardCount() <= 0 &&  ServerDay + 1 > m_TotalChongZhiDay )
	{
		return;
	}
	IconList.push_back( GetTotalChongZhiIconStu() );	
}

bool CYunYingHD::AllGetTotalChongZhiReward()
{
	int32_t HaveCount = 0;
	int32_t OldRecord = m_pPlayer->getRecord( RP_TOTAL_CHI_ZHI_REWARD );
	TotalChongZhiMap CfgTotalChongZhiMap = CFG_DATA.GetTotalChongZhiTable();
	TotalChongZhiMap::iterator it = CfgTotalChongZhiMap.begin();
	for ( ; it != CfgTotalChongZhiMap.end(); ++it )
	{
		int32_t NewRecord = OldRecord | ( 1 << ( it->first - 1 ) );
		if ( OldRecord == NewRecord )
		{
			continue;
		}
		return false;
	}
	return true;
}

ShowIcon CYunYingHD::GetTotalChongZhiIconStu()
{
	ShowIcon stu = {};
	if ( NULL == m_pPlayer )
	{
		return stu;
	}
	int32_t ServerDay = CFG_DATA.getServerDiffTime();
	if ( AllGetTotalChongZhiReward() )
	{
		stu.nState		= AS_TIME_OUT;
	}
	else if ( HaveTotalChongZhiRewardCount() <= 0 &&  ServerDay + 1 > m_TotalChongZhiDay )
	{
		stu.nState		= AS_TIME_OUT;
	}
	else
	{
		stu.nState		= AS_RUNNING;
	}
	stu.IconRight	= HaveTotalChongZhiRewardCount();
	stu.nId			= TOTAL_CHONG_ZHI;
	stu.nLeftTime	= -1;
	return stu;
}

void CYunYingHD::AddTotalChongZhiCount( int32_t AddCount )
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	int32_t ServerDay = CFG_DATA.getServerDiffTime();
	if ( ServerDay >= m_TotalChongZhiDay )
	{
		return;
	}
	m_pPlayer->GetOperateLimit().AddLimitCount( RP_TOTAL_CHI_ZHI_VALUES,AddCount );
	if ( HaveTotalChongZhiRewardCount() > 0 )
	{
		SendTotalChongZhiIcon();
	}
	SendTotalChongZhiInfo();
}