
#include "QiFu.h"
#include "GameService.h"


CQiFu::CQiFu()
{

}

CQiFu::~CQiFu()
{

}


void CQiFu::GetInterestsProtocol( ProcIdList& procList )
{
	procList.push_back( CM_ASK_QI_FU_INFO );
	procList.push_back( CM_ASK_QI_FU );
}

int32_t CQiFu::DispatchNetDatas( ProcId_t nProcId, Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}

	switch( nProcId )
	{
	case CM_ASK_QI_FU_INFO:
		{
			SendQiFuInfo();
			break;
		}
	case CM_ASK_QI_FU:
		{
			return OnQiFu( inPacket );
		}
	default:
		return ERR_SYETEM_ERR;
	}
	return ERR_OK;
}

bool CQiFu::CheckQiFuType( int8_t Type )
{
	switch( Type )
	{
	case QT_MONEY:
		{
			return true;
		}
	case QT_EXP:
		{
			return true;
		}
	default:
		return false;
	}
}


int32_t CQiFu::OnQiFu( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket || NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	int8_t Type = inPacket->readInt8();
	if ( !CheckQiFuType( Type ) )
	{
		return ERR_SYETEM_ERR;
	}
	int8_t Times = 0;
	int32_t QiFuMoneyTimes = 0;
	int32_t QiFuExpTimes = 0;
	int8_t VipLevel = m_pPlayer->GetPlayerVip().GetVipLevel();
	VipCfg* pVipCfg = CFG_DATA.GetVipTable().GetVipCfg( VipLevel );
	if ( NULL == pVipCfg )
	{
		return ERR_SYETEM_ERR;
	}

	if ( Type == QT_MONEY )
	{
		QiFuMoneyTimes = pVipCfg->QiFuMoneyTimes;
		Times = m_pPlayer->GetOperateLimit().GetLimitCount( PR_QI_FU_MONEY );
		if ( Times >= QiFuMoneyTimes )
		{
			return ERR_SYETEM_ERR;
		}
	}
	else if ( Type == QT_EXP )
	{
		QiFuExpTimes = pVipCfg->QiFuExpTimes;
		Times = m_pPlayer->GetOperateLimit().GetLimitCount( PR_QI_FU_EXP );
		if ( Times >= QiFuExpTimes )
		{
			return ERR_SYETEM_ERR;
		}
	}

	int32_t nLevel = m_pPlayer->getLevel();
	QiFuCfg* pQiFuCfg = CFG_DATA.GetQiFuTable().GetQiFuCfg( Type, nLevel, Times + 1 );
	if ( NULL == pQiFuCfg )
	{
		return ERR_SYETEM_ERR;
	}

	int32_t CriticalStrike = 1;
	int32_t nRand = RANDOM.generate( 1, 100 );
	if ( pQiFuCfg->Rate >= nRand )
	{
		CriticalStrike = 2;
	}

	ItemData Item = {};
	Item.m_nClass = pQiFuCfg->CostItemClass;
	Item.m_nCount = pQiFuCfg->CostItemCount;
	Item.m_nId    = pQiFuCfg->CostItemId;

	if ( ( Item.m_nId <= 0 || Item.m_nCount <= 0 ) && ( pQiFuCfg->CostGold < 0 ))
	{
		return ERR_SYETEM_ERR;
	}

	if ( Item.m_nId > 0 && Item.m_nCount > 0 )
	{
		if ( !m_pPlayer->GetBag().RemoveItem( Item, IDCR_QI_FU ) )
		{
			return ERR_SYETEM_ERR;
		}
	}

	if ( pQiFuCfg->CostGold > 0 )
	{
		if ( !m_pPlayer->DecCurrency( CURRENCY_GOLD, pQiFuCfg->CostGold, GCR_QI_FU, Times ) )
		{
			return ERR_SYETEM_ERR;
		}
	}

	int32_t AddCount = 0;
	if ( pQiFuCfg->GetExp > 0 )
	{
		AddCount = CriticalStrike * pQiFuCfg->GetExp;
		m_pPlayer->AddCurrency( CURRENCY_CASH, AddCount, MCR_QI_FU );
	}
	if ( pQiFuCfg->GetMoney > 0 )
	{
		AddCount = CriticalStrike * pQiFuCfg->GetMoney;
		m_pPlayer->AddCurrency( CURRENCY_MONEY, AddCount, MCR_QI_FU );
	}

	if ( Type == QT_MONEY )
	{
		m_pPlayer->GetOperateLimit().AddLimitCount( PR_QI_FU_MONEY, 1 );
		m_pPlayer->GetOperateLimit().AddLimitCount( PR_QI_FU_MONEY_TOTAL, AddCount );
		m_pPlayer->GetPlayerHuoYueDu().AddHuoYueDuRecord( HYDT_QI_FU_MONEY );
		m_pPlayer->GetAchievemnet().AddAchievement( AT_QI_FU_MONEY );
	}
	else if ( Type == QT_EXP )
	{
		m_pPlayer->GetOperateLimit().AddLimitCount( PR_QI_FU_EXP, 1 );
		m_pPlayer->GetOperateLimit().AddLimitCount( PR_QI_FU_EXP_TOTAL, AddCount );
		m_pPlayer->GetPlayerHuoYueDu().AddHuoYueDuRecord( HYDT_QI_FU_EXP );
		m_pPlayer->GetAchievemnet().AddAchievement( AT_QI_FU_EXP );
	}
	else
	{
		return ERR_SYETEM_ERR;
	}

	SendQiFuInfo();
	SendQiFuSuccess( Type, AddCount, static_cast<int8_t>(CriticalStrike) );
	SendQIFuIcon();
	GAME_SERVICE.replySuccess( m_pPlayer->getGateIndex(), inPacket->getProc(), AddCount );
	return ERR_OK;
}


void CQiFu::SendQiFuSuccess( int8_t Type, int32_t AddCount, int8_t Double )
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_QI_FU_SUCCESS );
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt8( Type );
	packet->writeInt32( AddCount );
	packet->writeInt8( Double );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);	
}

void CQiFu::SendQiFuInfo()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_QI_FU_INFO );
	if (NULL == packet)
	{
		return;
	}
	packet->writeInt8( (int8_t)m_pPlayer->GetOperateLimit().GetLimitCount( PR_QI_FU_MONEY ));
	packet->writeInt8( (int8_t)m_pPlayer->GetOperateLimit().GetLimitCount( PR_QI_FU_EXP ));
	packet->writeInt32( m_pPlayer->GetOperateLimit().GetLimitCount( PR_QI_FU_MONEY_TOTAL ));
	packet->writeInt32( m_pPlayer->GetOperateLimit().GetLimitCount( PR_QI_FU_EXP_TOTAL ));
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);	
}

void CQiFu::GetChouJiangStu( IconStateList& IconList )
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	if (!m_pPlayer->GetPlayerFunctionOpen().IsOpened(FT_QI_FU) )
	{
		return;
	}
	IconList.push_back(GetShowIconStu());
}

ShowIcon CQiFu::GetShowIconStu()
{
	ShowIcon stu = {};
	stu.nId			= QI_FU_ICON;
	stu.nState		= AS_RUNNING;
	stu.nLeftTime	= -1;
	return stu;
}

void CQiFu::SendHuoDongIcon()
{
	if (!m_pPlayer->GetPlayerFunctionOpen().IsOpened(FT_QI_FU) )
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_SEND_ONE_ICON);
	if (NULL == packet)
	{
		return;
	}
	ShowIcon stu = GetShowIconStu();
	packet->writeInt32( stu.nId );
	packet->writeInt8( stu.nState );
	packet->writeInt32( stu.nLeftTime );
	packet->writeInt8( stu.IconLeft );
	packet->writeInt32( stu.IconRight );
	packet->writeInt8( stu.Effects );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.worldBroadcast( packet );
}

void CQiFu::SendQIFuIcon()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	if ( !m_pPlayer->GetPlayerFunctionOpen().IsOpened( FT_QI_FU ) )
	{
		return;
	}
	ShowIcon stu = GetShowIconStu();
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_ONE_ICON );
	if ( NULL == packet )
	{
		return;
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

void CQiFu::GetQiFuStu( IconStateList& IconList )
{
	GetChouJiangStu( IconList );
}