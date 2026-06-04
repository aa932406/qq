#include "HuoYueDu.h"
#include "Player.h"
#include "GameService.h"
#include "FunctionOpen.h"

using namespace Answer;

CHuoYueDu::CHuoYueDu()
{
	OnCleanUp();
}

CHuoYueDu::~CHuoYueDu()
{

}

void CHuoYueDu::GetInterestsProtocol( ProcIdList& procList )
{
	procList.push_back( CM_ASK_HUO_YUE_DU_INFO );
	procList.push_back( CM_GET_HUO_YUE_DU_REWARD );
	procList.push_back( CM_ASK_MIAO_HUO_YUE_DU );
}

int32_t	CHuoYueDu::DispatchNetDatas( ProcId_t nProcId, Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	switch ( nProcId )
	{
	case CM_ASK_HUO_YUE_DU_INFO:
		{
			return OnAskHuoYueDuInfo( inPacket );
		}
	case CM_GET_HUO_YUE_DU_REWARD:
		{
			return OnGetHuoYueDuReward( inPacket );
		}
	case CM_ASK_MIAO_HUO_YUE_DU:
		{
			return OnSec( inPacket );
		}
	}
	return ERR_OK;
}

void CHuoYueDu::OnLoadFromDB( const PlayerDBData& dbData )
{
	m_HuoYueDuRecord	= dbData.m_HuoYueDuData.m_HuoYueDuRecord;
}

void CHuoYueDu::OnSaveToDB( PlayerDBData& dbData )
{
	dbData.m_HuoYueDuData.m_HuoYueDuRecord = m_HuoYueDuRecord;
}

void CHuoYueDu::OnCleanUp()
{
	m_HuoYueDuRecord.clear();
}

void CHuoYueDu::OnDaySwitch( int32_t nDiffDays )
{
	m_HuoYueDuRecord.clear();

	if ( NULL == m_pPlayer )
		return;

	int32_t nLevel = m_pPlayer->getLevel();
	if ( nLevel >= m_pPlayer->GetMaxLevel() )
		AddHuoYueDuRecord( HYDT_DA_WEI_WANG, 0, true );

	// TODO: Decompiled also adds: if all equips top → HYDT_EQUIP_STAT_UP (needs CExtEquip::IsAllPosTop), if mini client → HYDT_ILLUSION (needs IsMiniClient)
}

int32_t	CHuoYueDu::OnAskHuoYueDuInfo( Answer::NetPacket *inPacket )
{
	SendHuoYueDuInfo();
	return ERR_OK;
}

int32_t	CHuoYueDu::OnGetHuoYueDuReward( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket || NULL == m_pPlayer )
		return ERR_SYETEM_ERR;

	int32_t Index = inPacket->readInt32();
	CfgHuoYueDuReward* pReward = CFG_DATA.GetHuoYueDuReward( Index );
	if ( NULL == pReward )
		return ERR_SYETEM_ERR;

	int32_t OldRecord = m_pPlayer->getRecord( PR_HUO_YUE_DU_REWARD_INFO );
	int32_t NewRecord = OldRecord | ( 1 << (Index - 1) );
	if ( OldRecord == NewRecord )
		return ERR_SYETEM_ERR;

	if ( pReward->NeedHuoYueDu > CalculateHuoYueDu() )
		return ERR_SYETEM_ERR;

	if ( !m_pPlayer->GetBag().AddItemsAndEggs( pReward->Items, IACR_HUO_YUE_DU ) )
		return ERR_SYETEM_ERR;

	m_pPlayer->updateRecord( PR_HUO_YUE_DU_REWARD_INFO, NewRecord );
	SendHuoYueDuIcon();
	m_pPlayer->GetPlayerDailyActivity().SendHuoDongDaTingIcon();
	SendHuoYueDuInfo();
	return ERR_OK;
}

int32_t	CHuoYueDu::OnSec( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket || NULL == m_pPlayer )
		return ERR_SYETEM_ERR;

	int32_t Index = inPacket->readInt32();
	CfgHuoYueDu* pHuoYueDu = CFG_DATA.GetHuoYueDuCfg( Index );
	if ( NULL == pHuoYueDu || !pHuoYueDu->IsCanSec )
		return ERR_SYETEM_ERR;

	HuoYueDuRecordMap::iterator it = m_HuoYueDuRecord.find(Index);
	if ( it != m_HuoYueDuRecord.end() && it->second.IsSec == 1 )
		return ERR_SYETEM_ERR;

	if ( !m_pPlayer->DecCurrency( CURRENCY_GOLD, pHuoYueDu->Gold, GCR_HUO_YUE_DU, pHuoYueDu->Index ) )
		return ERR_SYETEM_ERR;

	// Before completing, sync accumulated HuoYueDu to limited counter
	if ( m_HuoYueDuRecord[Index].FinishTimes < pHuoYueDu->Count )
	{
		AddKiaFuHuoYueDu( pHuoYueDu->AddHuoYueDu );
		// CFestivalDoubleEleven singleton access depends on integration pattern
		// CFestivalDoubleEleven::instance().AddHuoYueDu( m_pPlayer, pHuoYueDu->AddHuoYueDu );
	}

	m_HuoYueDuRecord[Index].IsSec		= 1;
	m_HuoYueDuRecord[Index].FinishTimes = pHuoYueDu->Count;

	SendHuoYueDuIcon();
	m_pPlayer->GetPlayerDailyActivity().SendHuoDongDaTingIcon();
	SendHuoYueDuInfo();
	return ERR_OK;
}

void CHuoYueDu::AddHuoYueDuRecord( int8_t Type, int32_t Effect, bool Complete )
{
	if ( NULL == m_pPlayer )
		return;

	bool NeedSys = false;
	HuoYueDuTable CfgTable = CFG_DATA.GetHuoYueDuTable();
	HuoYueDuTable::iterator it = CfgTable.begin();

	for ( ; it != CfgTable.end(); ++it )
	{
		CfgHuoYueDu& rCfg = it->second;

		if ( rCfg.Type != Type )
			continue;

		// Effect filter for ACTIVITY/DUNGEON/KILL_MONSTER
		if ( (Type == HYDT_ACTIVITY || Type == HYDT_DUNGEON || Type == HYDT_KILL_MONSTER) && rCfg.Effect != Effect )
			continue;

		HuoYueDuRecord& record = m_HuoYueDuRecord[it->first];

		if ( Complete )
		{
			// Direct complete (used by OnDaySwitch)
			if ( record.FinishTimes < rCfg.Count )
			{
				record.FinishTimes = rCfg.Count;
				NeedSys = true;
			}
		}
		else switch ( Type )
		{
		case HYDT_ACTIVITY:
		{
			int32_t JoinCount = m_pPlayer->GetPlayerDailyActivity().GetJoinActivityCount( Effect );
			record.FinishTimes = (JoinCount >= rCfg.Count) ? rCfg.Count : JoinCount;
			NeedSys = true;
			break;
		}
		case HYDT_DUNGEON:
		{
			int32_t JoinCount = m_pPlayer->getRecord( Effect );
			record.FinishTimes = (JoinCount >= rCfg.Count) ? rCfg.Count : JoinCount;
			NeedSys = true;
			break;
		}
		case HYDT_QI_FU_EXP:
		case HYDT_QI_FU_MONEY:
		case HYDT_KILL_BOSS:
		case HYDT_DA_WEI_WANG:
		case HYDT_SIGN:
		case HYDT_ILLUSION:
		case HYDT_EQUIP_STAT_UP:
		case HYDT_CYCLE_TASK:
		{
			if ( record.FinishTimes < rCfg.Count )
			{
				record.FinishTimes++;
				NeedSys = true;
			}
			break;
		}
		case HYDT_KILL_MONSTER:
		{
			if ( rCfg.Effect == Effect && record.FinishTimes < rCfg.Count )
			{
				record.FinishTimes++;
				NeedSys = true;
			}
			break;
		}
		case HYDT_COST_CASH:
		case HYDT_COST_GOLD:
		case HYDT_FAMILY_DONATE:
		{
			if ( record.FinishTimes < rCfg.Count )
				NeedSys = true;
			record.FinishTimes += Effect;
			if ( record.FinishTimes > rCfg.Count )
				record.FinishTimes = rCfg.Count;
			break;
		}
		default:
			break;
		}

		break;
	}

	if ( NeedSys )
	{
		SendHuoYueDuInfo();
		m_pPlayer->GetPlayerDailyActivity().SendHuoDongDaTingIcon();
	}
}

void CHuoYueDu::SendHuoYueDuInfo()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_SEND_HUO_YUE_DU_INFO);
	if(NULL == packet)
	{
		return;
	}
	int32_t nSize = m_HuoYueDuRecord.size();
	packet->writeInt32( CalculateHuoYueDu() );
	packet->writeInt32( m_pPlayer->getRecord( PR_HUO_YUE_DU_REWARD_INFO ) );
	packet->writeInt32( nSize );
	HuoYueDuRecordMap::iterator it = m_HuoYueDuRecord.begin();
	for ( ; it != m_HuoYueDuRecord.end(); ++it )
	{
		packet->writeInt32( it->first );
		packet->writeInt32( it->second.FinishTimes );
		packet->writeInt8( it->second.IsSec );
	}
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);
}

int32_t CHuoYueDu::CalculateHuoYueDu()
{
	int32_t Count = 0;
	HuoYueDuRecordMap::iterator it = m_HuoYueDuRecord.begin();
	for ( ; it != m_HuoYueDuRecord.end(); ++it )
	{
		CfgHuoYueDu* pHuoYueDu = CFG_DATA.GetHuoYueDuCfg( it->first );
		if ( NULL == pHuoYueDu )
		{
			continue;
		}
		if ( it->second.FinishTimes >= pHuoYueDu->Count )
		{
			Count += pHuoYueDu->AddHuoYueDu;
		}
	}
	return Count;
}

int32_t CHuoYueDu::RewardCount()
{
	int32_t Count = 0;
	int32_t HuoYueDu = CalculateHuoYueDu();
	HuoYueDuRewardTable RewardTable = CFG_DATA.GetHuoYueDuRewardTable();
	HuoYueDuRewardTable::iterator it = RewardTable.begin();
	for ( ; it != RewardTable.end(); ++it )
	{
		if ( HuoYueDu >= it->second.NeedHuoYueDu )
		{
			int32_t Record = m_pPlayer->getRecord( PR_HUO_YUE_DU_REWARD_INFO );
			if ( ( Record & ( 1 << (it->second.Id - 1) ) ) > 0 )
			{
				continue;
			}
			Count++;
		}
	}
	return Count;
}

void CHuoYueDu::AddKiaFuHuoYueDu( int32_t Value )
{
	if ( NULL == m_pPlayer )
		return;

	// Only valid within first 9 days of server
	if ( CFG_DATA.getServerDiffTime() > 9 )
		return;

	// Limit counter - decompiled uses PR 1926
	// Decompiled uses PR_LIMIT 1926 for KiaFuHuoYueDu

}

void CHuoYueDu::GetHuoYueDuIcon( IconStateList& IconList )
{
	if ( NULL == m_pPlayer )
		return;

	if ( !m_pPlayer->GetPlayerFunctionOpen().IsOpened( FT_HUO_YUE_DU ) )
		return;

	IconList.push_back( GetHuoYueDuIconStu() );
}

void CHuoYueDu::SendHuoYueDuIcon()
{
	if ( NULL == m_pPlayer )
		return;

	if ( !m_pPlayer->GetPlayerFunctionOpen().IsOpened( FT_HUO_YUE_DU ) )
		return;

	// Send icon state - in decompiled: Player::SendIconState
	m_pPlayer->GetPlayerDailyActivity().SendHuoDongDaTingIcon();
}

ShowIcon CHuoYueDu::GetHuoYueDuIconStu()
{
	ShowIcon stu = {};
	if ( NULL == m_pPlayer )
		return stu;

	stu.nId = 120;  // ICON_HUO_YUE_DU
	stu.nState = 2;  // always show if reached here
	stu.nLeftTime = -1;
	stu.IconRight = RewardCount();
	if ( stu.IconRight > 0 )
		stu.Effects = 1;
	return stu;
}

