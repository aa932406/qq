#include "DailyActivity.h"
#include "GameService.h"
#include "DBService.h"
#include "DaTiHD.h"
#include "ActivityManager.h"
#include "EquipManager.h"
#include "VipGuaJi.h"
using namespace Answer;
const int32_t	GET_DWW_REWARD_TIMES = 2;						//ÿ�������ȡ��θ������2��
const int32_t   HOUR_HAVE_SECONDS	 = 60 * 60;					//һСʱ������
const int32_t	DAY_HAVE_SECONDS	 = 24 *HOUR_HAVE_SECONDS;	//һ�������
const float		SEARCH_BACK_RATE	 = 0.7f;					//����һر���
enum HuoDongDaTingType
{
	JING_CAI_HD			= 1,	//���ʻ
	DAILY_DUNGEON		= 2,	//�ճ�����
	DAILY_TASK			= 3,	//�ճ�����
};

enum JingCaiHDType
{
	DA_TI_HUO_DONG		= 1,	//����
	OPEN_BAO_XIANG		= 2,	//�����䣨�콵���䣩
	MO_LING_RU_QIN		= 3,	//ħ������
	JUN_TUAN_ZHI_GUANG	= 4,	//����֮��
	FEI_TIAN_SHENG_JV	= 5,	//�������
	DA_TI_HUO_DONG2		= 6,	//����2
	JUN_TUAN_ZHI_GUANG2	= 7,	//����֮��2
	MO_LING_RU_QIN2		= 8,	//ħ������
	JUN_TUAN_WAR		= 9,	//����ս
	SAFETY_GUA_JI		= 10,	//��ȫ�һ�
	SQIDERQUEEN_1		= 11,	//֩��Ů��1
	SQIDERQUEEN_2		= 12,	//֩��Ů��2
	SQIDERQUEEN_3		= 13,	//֩��Ů��3
	SQIDERQUEEN_4		= 14,	//֩��Ů��4
	JCHD_TERRIORY_WAR	= 15,	// ���ս

	JING_CAI_HD_COUNT,
};

enum DailyTaskType
{
	DTT_CYCLE_TASK		= 1,	//ѭ������
	DA_WEI_WANG			= 2,	//��θ��
	FTT_FAMILY_TASK		= 3,	//��������
	DDT_COUNT,
};

enum JingCaiHDState
{
	STATE_CLOSED			= 1,	//δ��ʼ
	STATE_CAN_CONDUCT		= 2,	//����ǰ��
	STATE_END				= 3,	//�ѽ���
	STATE_FINISH			= 4,	//�����
};

DailyActivity::DailyActivity()
{
	OnCleanUp();
}

DailyActivity::~DailyActivity()
{

}

void DailyActivity::OnCleanUp()
{
	m_signInfo.CleanUp();
	m_LastUpdate		= 0;
	m_LastUpdateIcon	= 0;
	m_IsLogin			= false;
	m_ActivityRecord.clear();
}

void DailyActivity::OnUpdate( int64_t curTick )
{	
	if ( curTick - m_LastUpdate >= 1000 )
	{	
		m_pPlayer->updateRecord( RP_DAILY_ONLIN_TIME, m_pPlayer->getRecord( RP_DAILY_ONLIN_TIME ) + 1 );
		m_pPlayer->updateRecord( RP_WEEK_ONLINE_TIME, m_pPlayer->getRecord( RP_WEEK_ONLINE_TIME ) + 1 );
		m_LastUpdate = curTick;
	}
	UpDateJiangLiDatingIcon( curTick );
}

void DailyActivity::OnLoadFromDB( const PlayerDBData& dbData )
{
	m_signInfo			= dbData.signInfo.data;
	m_ActivityRecord	= dbData.signInfo.m_ActivityRecord;
}

void DailyActivity::OnSaveToDB( PlayerDBData& dbData )
{
	dbData.signInfo.data				= m_signInfo;
	dbData.signInfo.m_ActivityRecord	= m_ActivityRecord;
}

void DailyActivity::OnDaySwitch( int32_t nDiffDays )
{
	refreshSignInfo();
	refreshWeekTime();
	DaySwitch( nDiffDays );
	if ( m_IsLogin )
	{
		SendJiangLiDaTingIcon();
	}
}

void DailyActivity::GetInterestsProtocol( ProcIdList& procList )
{
	procList.push_back( CM_QUERY_SIGN_INFO );
	procList.push_back( CM_SIGN	 );
	procList.push_back( CM_GET_SIGN_REWARD );

	procList.push_back( CM_GET_ONLINE_REWARD );
	procList.push_back( CM_ASK_ONLINE_TEME_INFO );
	procList.push_back( CM_GET_WEEK_ONLINE_REWARD );
	
	procList.push_back( CM_ASK_DWW_INFO );
	procList.push_back( CM_ASK_DWW_REWARD );

	procList.push_back( CM_GET_SEVEN_LOGIN );
	procList.push_back( CM_QUERY_SEVEN_LOGIN );

	procList.push_back( CM_ASK_OFFLINE_INFO );
	procList.push_back( CM_GET_OFFLINE_EXP );

	procList.push_back( CM_QUERY_LEVEL_GIFT );
	procList.push_back( CM_GET_LEVEL_GIFT );

	procList.push_back( CM_ASK_HUO_DONG_DA_TING_INFO );

	procList.push_back( CM_ASK_SEARCHBACK_INFO );
	procList.push_back( CM_ASK_GET_SEARCHBACK_REWARD );
	procList.push_back( IM_SOCIAL_GAME_ALREADY );
}
	
int32_t	DailyActivity::DispatchNetDatas( ProcId_t nProcId, Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_INVALID_DATA;
	}
	switch( nProcId )
	{
	case CM_QUERY_SIGN_INFO:
		{
			return onQuerySignInfo( inPacket );
		}
	case CM_SIGN:
		{
			return onSign( inPacket );
		}
	case CM_GET_SIGN_REWARD:
		{
			return onGetSignReward( inPacket );
		}
	case CM_GET_ONLINE_REWARD:
		{
			return OnGetOnLineReward( inPacket );
		}
	case CM_ASK_ONLINE_TEME_INFO:
		{
			return OnQueryOnLineInfo( inPacket );
		}
	case CM_GET_WEEK_ONLINE_REWARD:
		{
			return OnGetWeekOnLineReward( inPacket );
		}
	case CM_ASK_DWW_INFO:
		{
			return OnQueryDWWInfo( inPacket );
		}
	case CM_ASK_DWW_REWARD:
		{
			return OnQueryDWWReward( inPacket );
		}
	case CM_GET_SEVEN_LOGIN:
		{
			return OnGetSevenLoginRewrad( inPacket );
		}
	case CM_QUERY_SEVEN_LOGIN:
		{
			return OnQuerySevenLoginInfo( inPacket );
		}
	case CM_QUERY_LEVEL_GIFT:
		{
			return OnQueryLevelGiftInfo( inPacket );
		}
	case CM_GET_LEVEL_GIFT:
		{
			return OnGetLevelGift( inPacket );
		}
	case CM_ASK_OFFLINE_INFO:
		{
			return OnQueryOffLineExpInfo( inPacket );
		}
	case CM_GET_OFFLINE_EXP:
		{
			return OnGetOffLineExp( inPacket );
		}
	case CM_ASK_HUO_DONG_DA_TING_INFO:
		{
			return OnQueryHuoDaoDaTingData( inPacket );
		}
	case CM_ASK_SEARCHBACK_INFO:
		{
			SendSearchBackInfo();
			break;
		}
	case CM_ASK_GET_SEARCHBACK_REWARD:
		{
			return OnGetSearchBackReward( inPacket );
		}
	case IM_SOCIAL_GAME_ALREADY:
		{
			return RecordEnterNumber( inPacket );
			break;
		}
	default:
		return ERR_INVALID_DATA;
	}
	return ERR_OK;
}
	
int32_t DailyActivity::onSign( Answer::NetPacket *inPacket )
{
	if (NULL == inPacket || NULL == m_pPlayer )
	{
		return ERR_INVALID_DATA;
	}
	int8_t	nDay		= inPacket->readInt8();
	tm tmNow = m_pPlayer->getLocalNow();
	if ( nDay > 0 )
	{
		int32_t oldSignRecord = m_signInfo.sign_record;
		m_signInfo.sign_record |= 1 << (tmNow.tm_mday - 1);
		if (m_signInfo.sign_record == oldSignRecord)
		{
			return ERR_INVALID_DATA;
		}
		m_pPlayer->GetPlayerHuoYueDu().AddHuoYueDuRecord( HYDT_SIGN );
	}
	else if ( nDay == 0 )
	{
		int8_t VipLevel = m_pPlayer->GetPlayerVip().GetVipLevel();
		VipCfg* pVipCfg = CFG_DATA.GetVipTable().GetVipCfg( VipLevel );
		if ( NULL == pVipCfg )
		{
			return ERR_SYETEM_ERR;
		}
		int8_t	Retroactive = m_pPlayer->GetOperateLimit().GetLimitCount( PR_RETROACTIVE_TIMES );
		if ( Retroactive >= pVipCfg->Retroactive )
		{
			return ERR_SYETEM_ERR;
		}
		int32_t i = tmNow.tm_mday -1 ;
		int32_t KaiFuDay = m_pPlayer->getCreateTime();
		tm KaiFuTm = DayTime::localnow( KaiFuDay );
		while( i > 0 )
		{
			if ( tmNow.tm_mon == KaiFuTm.tm_mon && i < KaiFuTm.tm_mday )
			{
				break;
			}
			int8_t	Retroactive = m_pPlayer->GetOperateLimit().GetLimitCount( PR_RETROACTIVE_TIMES );
			if ( Retroactive >= pVipCfg->Retroactive )
			{
				break;
			}
			if ( (m_signInfo.sign_record & ( 1 << (i -1) ) ) > 0 )
			{
				i--;
				continue;
			}
			m_signInfo.sign_record |= 1 << (i -1);
			m_pPlayer->GetOperateLimit().AddLimitCount( PR_RETROACTIVE_TIMES, 1 );
			i--;
		}
	}
	else 
	{
		return ERR_SYETEM_ERR;
	}
	m_signInfo.refresh_time = m_pPlayer->getNow();
	sendSignInfo();
	SendJiangLiDaTingIcon();
	return ERR_OK;
}

int32_t	DailyActivity::onGetSignReward( Answer::NetPacket *inPacket )
{
	if (NULL == inPacket)
	{
		return ERR_INVALID_DATA;
	}
	int32_t count = inPacket->readInt32();

	int32_t i = 0;
	int32_t signCount = 0;
	while (i < 31)
	{
		signCount += (m_signInfo.sign_record >> i) & 1;
		i++;
	}
	if (signCount < count)
	{
		return ERR_INVALID_DATA;
	}
	vector<int8_t>::iterator it = m_signInfo.sign_reward.begin();
	for ( ; it != m_signInfo.sign_reward.end(); ++ it )
	{
		if ( *it == count )
		{
			return ERR_SYETEM_ERR;
		}
	}
	//������
	MemChrBagVector items = CFG_DATA.GetSignReward(count);
	if ( items.empty() )
	{
		return ERR_SYETEM_ERR;
	}
	if ( !m_pPlayer->GetBag().AddItemsAndEggs( items,IACR_SIGN ) )
	{
		return ERR_SYETEM_ERR;
	}
	m_signInfo.sign_reward.push_back( count );
	sendSignInfo();
	SendJiangLiDaTingIcon();
	return ERR_OK;
}

int32_t	DailyActivity::onQuerySignInfo(Answer::NetPacket *inPacket)
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	sendSignInfo();
	return ERR_OK;
}

int32_t	DailyActivity::OnGetOnLineReward( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket || NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	int8_t Id = inPacket->readInt8(); 
	CfgOnlineReward* pReward = CFG_DATA.GetOnlineRewardCfg( Id );
	if ( NULL == pReward )
	{
		return ERR_SYETEM_ERR;
	}
	if ( GetTodayOnLineTime() < pReward->NeedTime )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t oldRecord  = m_pPlayer->getRecord( RP_DAILY_ONLIE_REWARD_INFO );
	int32_t NewRecord = oldRecord | ( 1 << (Id - 1) );
	if ( oldRecord == NewRecord )
	{
		return ERR_INVALID_DATA;
	}
	if ( !m_pPlayer->GetBag().AddItemsAndEggs( pReward->ItemVector, IACR_ONLINE_REWAR) )
	{
		return ERR_SYETEM_ERR;
	}
	m_pPlayer->updateRecord( RP_DAILY_ONLIE_REWARD_INFO, NewRecord ); 

	SendOnlineRewardInfo();
	SendJiangLiDaTingIcon();
	return ERR_OK;
}

int32_t	DailyActivity::OnQueryOnLineInfo( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	SendOnlineRewardInfo();
	return ERR_OK;
}

int32_t	DailyActivity::OnGetWeekOnLineReward( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t Week  = GetNewSeverWeek();
	if ( Week  <= 1 ) //��һ�ܿ϶�û��
	{
		return ERR_SYETEM_ERR;
	}
	CfgWeekOnlineReward* pWeekReward = CFG_DATA.GetWeekOnlineReward( Week - 1 );
	if ( NULL == pWeekReward )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t LastWeekOnLineHour = GetLastWeekOnlineTime() / HOUR_HAVE_SECONDS;
	if ( LastWeekOnLineHour > pWeekReward->UpperLimit )
	{
		LastWeekOnLineHour = pWeekReward->UpperLimit;
	}
	if ( LastWeekOnLineHour <= 0 )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t GetCash = LastWeekOnLineHour * pWeekReward->Cash;
	if ( GetCash <= 0 )
	{
		return ERR_SYETEM_ERR;
	}
	m_pPlayer->updateRecord( RP_LAST_WEEK_ONLINE_TIME, 0 );
	if ( !m_pPlayer->AddCurrency( CURRENCY_CASH, GetCash, GCC_WEEK_ONLINE_REWARD ) )
	{
		LOG_ERROR("WEEK ONLINE REWARD ERR Cid=%lld,time=%d,GetCash=%d\n",m_pPlayer->getCid(),m_pPlayer->getNow(),GetCash);
	}
	SendOnlineRewardInfo();
	SendJiangLiDaTingIcon();
	return ERR_OK;
}

int32_t	DailyActivity::OnQueryDWWInfo( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	SendDWWInfo();
	return ERR_OK;
}

int32_t	DailyActivity::OnQueryDWWReward( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket || NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	if ( m_pPlayer->getRecord( RP_DAILY_DWW_TIMES ) >= GET_DWW_REWARD_TIMES )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t nNow = m_pPlayer->getNow();
	if ( nNow - m_pPlayer->getRecord( RP_DAILY_DWW_LAST_TIME ) < HOUR_HAVE_SECONDS )
	{
		return ERR_SYETEM_ERR;
	}
	m_pPlayer->GetOperateLimit().AddLimitCount( RP_DAILY_DWW_TIMES, 1 );
	m_pPlayer->updateRecord( RP_DAILY_DWW_LAST_TIME, nNow );
	MemChrBag stu = {};
	stu.itemClass	= IC_NORMAL;
	stu.itemId		= ISI_ZHI_ZUN_PI_SA;
	stu.itemCount	= 1;
	stu.bind		= IBS_BIND;
	if ( m_pPlayer->GetBag().GetbagFreeSize() < 1 )
	{
		DB_SERVICE.OnSendSysMail( m_pPlayer->getCid(), DaWeiWangHD, stu );
	}
	else
	{
		m_pPlayer->GetBag().AddItem( stu, IACR_DWW_HD );
	}
	SendDWWInfo();
	SendDWWHDIcon();
	m_pPlayer->GetPlayerHuoYueDu().AddHuoYueDuRecord( HYDT_DA_WEI_WANG );
	return ERR_OK;
}

int32_t	DailyActivity::OnQuerySevenLoginInfo( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	SendSevenLoginInfo();
	return ERR_OK;
}

int32_t	DailyActivity::OnGetSevenLoginRewrad( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket || NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	int8_t Day	= inPacket->readInt8();
	if ( Day > 7 )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t OldRecord = m_pPlayer->getRecord( RP_DAILY_SEVEN_LOGIN_REWARD );
	int32_t NewRecord = OldRecord | ( 1 << (Day - 1) );

	if ( OldRecord == NewRecord )
	{
		return ERR_SYETEM_ERR;
	}
	CfgSevenLoginRewrad* pReward = CFG_DATA.GetSevenLoginRewardCfg( Day );
	if ( NULL == pReward )
	{
		return ERR_SYETEM_ERR;
	}
	MemChrBagVector addItemVt;
	MemEquipVector vEquip;
	if (!pReward->ItemVector.empty())
	{
		for (MemChrEquipBagVector::iterator it =pReward->ItemVector.begin();it !=  pReward->ItemVector.end();++it)
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
	if ( !m_pPlayer->GetBag().AddItemsAndEggs( addItemVt,  IACR_SEVEN_LOGIN ) )
	{
		return ERR_SYETEM_ERR;
	}
	if ( !vEquip.empty() )
	{
		m_pPlayer->sendEquipInfo( vEquip );
	}
	m_pPlayer->updateRecord( RP_DAILY_SEVEN_LOGIN_REWARD,NewRecord );
	SendSevenLoginInfo();
	SendJiangLiDaTingIcon();
	return ERR_OK;
}

int32_t	DailyActivity::OnQueryLevelGiftInfo( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	SendLevelGiftInfo();
	return ERR_OK;
}

int32_t	DailyActivity::OnGetLevelGift( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket || NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	int8_t	Index = inPacket->readUInt8();
	CfgLevelGift* pLevelGift =CFG_DATA.GetLevelGiftCfg( Index );
	if ( NULL == pLevelGift )
	{
		return ERR_SYETEM_ERR;
	}
	if ( pLevelGift->Level > m_pPlayer->getLevel() )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t OldRecord = m_pPlayer->getRecord( RP_LEVEL_GIFT_REWARD );
	int32_t NewRecord = OldRecord | ( 1 << (Index - 1) );
	if ( OldRecord == NewRecord )
	{
		return ERR_SYETEM_ERR;
	}
	if ( !m_pPlayer->GetBag().AddItemsAndEggs( pLevelGift->ItemVector,IACR_SEVEN_LOGIN) )
	{
		return ERR_SYETEM_ERR;
	}
	m_pPlayer->updateRecord( RP_LEVEL_GIFT_REWARD,NewRecord );
	SendLevelGiftInfo();
	SendJiangLiDaTingIcon();
	return ERR_OK;
}

int32_t	DailyActivity::OnQueryOffLineExpInfo( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	SendOffLineInfo();
	return ERR_OK;
}

int32_t	DailyActivity::OnGetOffLineExp( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket || NULL == m_pPlayer )
	{
		return ERR_SYETEM_ERR;
	}
	int8_t Times = inPacket->readInt8();
	CfgOffLineExp* pOfflineExp = CFG_DATA.GetOfflineExpCfg( m_pPlayer->getLevel() );
	if ( NULL == pOfflineExp )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t OfflineSeconds = m_pPlayer->getRecord( PR_ACCUMULATIVE_OFFLINE_TIME );
	int32_t OfflineMinutes = 0;
	if ( OfflineSeconds > 3 * DAY_HAVE_SECONDS )
	{	
		OfflineMinutes = 3 * DAY_HAVE_SECONDS / 60;
	}
	else
	{
		OfflineMinutes = OfflineSeconds / 60;
	}
	double CanAddExp = OfflineMinutes * pOfflineExp->MinuteExp;
	int32_t NeedGold  = 0;
	if ( Times == 2 )
	{
		NeedGold = incInt( CanAddExp / pOfflineExp->BaseExp  * pOfflineExp->TwoTimes / 100 );
		CanAddExp *= 2;
	}
	else if ( Times == 3 )
	{
		NeedGold =  incInt( CanAddExp / pOfflineExp->BaseExp  * pOfflineExp->ThreeTimes / 100 );
		CanAddExp *= 3;
	}
	if ( CanAddExp <= 0 )
	{
		return ERR_SYETEM_ERR;
	}

	if ( NeedGold > 0 )
	{
		if ( !m_pPlayer->DecCurrency( CURRENCY_GOLD, NeedGold,GCR_GET_OFFLINE_EXP ) )
		{
			return ERR_SYETEM_ERR;
		}
	}
	m_pPlayer->updateRecord( PR_ACCUMULATIVE_OFFLINE_TIME, 0 );
	m_pPlayer->addExp( static_cast<int64_t>(CanAddExp) );
	SendOffLineInfo();
	SendJiangLiDaTingIcon();
	return ERR_OK;
}

int32_t	DailyActivity::OnQueryHuoDaoDaTingData( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	int8_t Type = inPacket->readInt8();
	SendHuoDaoDaTingData( Type );
	return ERR_OK;
}

void DailyActivity::SendDailyActivityInfo()
{
// 	sendSignInfo();
// 	SendOnlineRewardInfo();
// 	SendSevenLoginInfo();
// 	SendLevelGiftInfo();
// 	SendOffLineInfo();
	m_IsLogin = true;
}

void DailyActivity::sendSignInfo()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_SIGN_INFO);
	if(NULL == packet)
	{
		return;
	}
	packet->writeInt32(m_signInfo.sign_record);
	int8_t nSize = m_signInfo.sign_reward.size();
	packet->writeInt8( nSize );
	vector<int8_t>::iterator it = m_signInfo.sign_reward.begin();
	for ( ; it != m_signInfo.sign_reward.end(); ++ it )
	{
		packet->writeInt8( *it );
	}
	packet->writeInt8(m_pPlayer->getRecord(PR_RETROACTIVE_TIMES));
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);
}

void DailyActivity::refreshSignInfo()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	if (DayTime::monthdiff(m_signInfo.refresh_time) != 0)
	{
		m_pPlayer->GetOperateLimit().UpdateLimitCount( PR_RETROACTIVE_TIMES, 0 );
		m_signInfo.sign_record = 0;
		m_signInfo.sign_reward.clear();
	}
	sendSignInfo();
}

int32_t DailyActivity::GetTodayOnLineTime()
{
	if ( NULL == m_pPlayer )
	{
		return 0;
	}
	return	m_pPlayer->getRecord( RP_DAILY_ONLIN_TIME );
}

void DailyActivity::SendOnlineRewardInfo()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_ONLINE_REWARD_STATE);
	if(NULL == packet)
	{
		return;
	}
	packet->writeInt32( GetTodayOnLineTime() );
	packet->writeInt32( m_pPlayer->getRecord( RP_DAILY_ONLIE_REWARD_INFO ) );
	packet->writeInt32( GetNewSeverWeek() );
	packet->writeInt32( GetWeekOnlineTime() );
	packet->writeInt32( GetLastWeekOnlineTime() );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);
}

void DailyActivity::SendDWWInfo()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_SEND_DWW_INFO);
	if(NULL == packet)
	{
		return;
	}
	packet->writeInt32( m_pPlayer->getRecord( RP_DAILY_DWW_TIMES ) );
	packet->writeInt32( m_pPlayer->getRecord( RP_DAILY_DWW_LAST_TIME ) );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);
}

void DailyActivity::GetDaWeiWangIcon( IconStateList& IconList )
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	ShowIcon stu = {};
	stu.nId			= DA_WEI_WANG_HD_ID;
	stu.nState		= AS_RUNNING;
	stu.nLeftTime	= -1;
	int32_t Times   = m_pPlayer->getRecord( RP_DAILY_DWW_TIMES );
	if ( Times > 0 )
	{
		stu.nLeftTime = HOUR_HAVE_SECONDS - ( m_pPlayer->getNow() - m_pPlayer->getRecord( RP_DAILY_DWW_LAST_TIME ) );
	}
	if ( Times < GET_DWW_REWARD_TIMES )
	{
		IconList.push_back( stu );
	}
}
		
void DailyActivity::SendDWWHDIcon()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	int32_t Times = m_pPlayer->getRecord( RP_DAILY_DWW_TIMES );
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_SEND_ONE_ICON);
	if (NULL == packet)
	{
		return;
	}
	ShowIcon stu = {};
	stu.nId			= DA_WEI_WANG_HD_ID;
	stu.nLeftTime	= -1;
	if ( Times > 0 )
	{
		stu.nLeftTime = HOUR_HAVE_SECONDS - ( m_pPlayer->getNow() - m_pPlayer->getRecord( RP_DAILY_DWW_LAST_TIME ) );
	}
	if ( Times >= GET_DWW_REWARD_TIMES )
	{
		stu.nState		= AS_END;
	}
	else
	{
		stu.nState		= AS_RUNNING;
	}
	packet->writeInt32( stu.nId );
	packet->writeInt8( stu.nState );
	packet->writeInt32( stu.nLeftTime );
	packet->writeInt8( stu.IconLeft );
	packet->writeInt32( stu.IconRight );
	packet->writeInt8( stu.Effects );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);	
}

void DailyActivity::SendSevenLoginInfo()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_SEVEN_LOGIN);
	if(NULL == packet)
	{
		return;
	}
	packet->writeInt32( m_pPlayer->getRecord( RP_LOGIN_COUNT ) );
	packet->writeInt32( m_pPlayer->getRecord( RP_DAILY_SEVEN_LOGIN_REWARD ) );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);
}

void DailyActivity::SendLevelGiftInfo()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_LEVEL_GIFT);
	if(NULL == packet)
	{
		return;
	}
	packet->writeInt32( m_pPlayer->getRecord( RP_LEVEL_GIFT_REWARD ) );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);
}

int32_t	DailyActivity::GetNewSeverWeek()
{
	int32_t ServerStartTime = CFG_DATA.getServerStartDayTime();
	return DayTime::weekdiff( ServerStartTime ) + 1;
}

int32_t DailyActivity::GetWeekOnlineTime()
{
	if ( NULL == m_pPlayer )
	{
		return 0;
	}
	return	m_pPlayer->getRecord( RP_WEEK_ONLINE_TIME );
}

int32_t DailyActivity::GetLastWeekOnlineTime()
{
	if ( NULL == m_pPlayer )
	{
		return 0;
	}
	return m_pPlayer->getRecord( RP_LAST_WEEK_ONLINE_TIME );
}

void DailyActivity::refreshWeekTime()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	int32_t DiffWeek = DayTime::weekdiff( m_pPlayer->getRecord( PR_WEEK_CHECK_LAST_UPDATE ) );
	if ( DiffWeek == 1 )
	{
		m_pPlayer->updateRecord( RP_LAST_WEEK_ONLINE_TIME, m_pPlayer->getRecord(RP_WEEK_ONLINE_TIME) );
		m_pPlayer->updateRecord( RP_WEEK_ONLINE_TIME, 0 );
	}
	else if ( DiffWeek > 1 )
	{
		m_pPlayer->updateRecord( RP_LAST_WEEK_ONLINE_TIME, 0 );
		m_pPlayer->updateRecord( RP_WEEK_ONLINE_TIME, 0 );
	}

	m_pPlayer->updateRecord( PR_WEEK_CHECK_LAST_UPDATE, m_pPlayer->getNow() );
	SendOnlineRewardInfo();
}

void DailyActivity::SendOffLineInfo()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_OFFLINE_EXP_DATA);
	if(NULL == packet)
	{
		return;
	}
	int32_t OfflineSeconds = m_pPlayer->getRecord( PR_ACCUMULATIVE_OFFLINE_TIME );
	if ( OfflineSeconds > 3 * DAY_HAVE_SECONDS )
	{
		OfflineSeconds = 3 * DAY_HAVE_SECONDS;
	}
	packet->writeInt32( OfflineSeconds );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);
}

void DailyActivity::SendHuoDaoDaTingData( int8_t Type )
{		
	if ( NULL == m_pPlayer )
	{
		return;
	}
	if ( Type == JING_CAI_HD )
	{
		NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_SEND_HD_DA_TING_INFO );
		if(NULL == packet)
		{
			return;
		}
		packet->writeInt8( Type );
		packet->writeInt32( JING_CAI_HD_COUNT - 1 );

		//����
		int8_t	State = 1;
		if (ACTIVITY_MANAGER.GetActivity(DA_TI_HUONG_DONG_ID) != NULL )
		{
			State = ACTIVITY_MANAGER.GetActivity(DA_TI_HUONG_DONG_ID)->GetState();
			if ( State ==  AS_NOT_START )
			{
				State = STATE_CLOSED;
			}
			else if ( State == AS_TIME_OUT )
			{
				State = STATE_END;
			}
		}
		packet->writeInt8( DA_TI_HUO_DONG );
		packet->writeInt8( State );

		//������
		State = 1;
		if (ACTIVITY_MANAGER.GetActivity(TIAN_JIANG_BAO_XIANG) != NULL )
		{
			State = ACTIVITY_MANAGER.GetActivity(TIAN_JIANG_BAO_XIANG)->GetState();
			if ( State ==  AS_NOT_START )
			{
				State = STATE_CLOSED;
			}
			else if ( State == AS_TIME_OUT )
			{
				State = STATE_END;
			}
		}
		packet->writeInt8( OPEN_BAO_XIANG );
		packet->writeInt8( State );

		//ħ������
		State = 1;
		if (ACTIVITY_MANAGER.GetActivity(MO_LING_RU_QING_ID) != NULL )
		{
			State = ACTIVITY_MANAGER.GetActivity(MO_LING_RU_QING_ID)->GetState();
			if ( State ==  AS_NOT_START )
			{
				State = STATE_CLOSED;
			}
			else if ( State == AS_TIME_OUT )
			{
				State = STATE_END;
			}
		}
		packet->writeInt8( MO_LING_RU_QIN );
		packet->writeInt8( State );

		//����֮��
		State = 1;
		if (ACTIVITY_MANAGER.GetActivity(JUN_TUAN_ZHI_GUANG_ID) != NULL )
		{
			State = ACTIVITY_MANAGER.GetActivity(JUN_TUAN_ZHI_GUANG_ID)->GetState();
			if ( State ==  AS_NOT_START )
			{
				State = STATE_CLOSED;
			}
			else if ( State == AS_TIME_OUT )
			{
				State = STATE_END;
			}
		}
		packet->writeInt8( JUN_TUAN_ZHI_GUANG );
		packet->writeInt8( State );

		//�������
		State = 1;
		if (ACTIVITY_MANAGER.GetActivity(FEI_TIAN_SHENG_JV_ID) != NULL )
		{
			State = ACTIVITY_MANAGER.GetActivity(FEI_TIAN_SHENG_JV_ID)->GetState();
			if ( State ==  AS_NOT_START )
			{
				State = STATE_CLOSED;
			}
			else if ( State == AS_TIME_OUT )
			{
				State = STATE_END;
			}
		}
		packet->writeInt8( FEI_TIAN_SHENG_JV );
		packet->writeInt8( State );

		//����2
		State = 1;
		if (ACTIVITY_MANAGER.GetActivity(DA_TI_HUONG_DONG_ID2) != NULL )
		{
			State = ACTIVITY_MANAGER.GetActivity(DA_TI_HUONG_DONG_ID2)->GetState();
			if ( State ==  AS_NOT_START )
			{
				State = STATE_CLOSED;
			}
			else if ( State == AS_TIME_OUT )
			{
				State = STATE_END;
			}
		}
		packet->writeInt8( DA_TI_HUO_DONG2 );
		packet->writeInt8( State );
		
		//����֮��2
		State = 1;
		if (ACTIVITY_MANAGER.GetActivity(JUN_TUAN_ZHI_GUANG_ID2) != NULL )
		{
			State = ACTIVITY_MANAGER.GetActivity(JUN_TUAN_ZHI_GUANG_ID2)->GetState();
			if ( State ==  AS_NOT_START )
			{
				State = STATE_CLOSED;
			}
			else if ( State == AS_TIME_OUT )
			{
				State = STATE_END;
			}
		}
		packet->writeInt8( JUN_TUAN_ZHI_GUANG2 );
		packet->writeInt8( State );

		//ħ������2
		State = 1;
		if (ACTIVITY_MANAGER.GetActivity(MO_LING_RU_QING_ID2) != NULL )
		{									
			State = ACTIVITY_MANAGER.GetActivity(MO_LING_RU_QING_ID2)->GetState();
			if ( State ==  AS_NOT_START )
			{
				State = STATE_CLOSED;
			}
			else if ( State == AS_TIME_OUT )
			{
				State = STATE_END;
			}
		}
		packet->writeInt8( MO_LING_RU_QIN2 );
		packet->writeInt8( State );

		//����ս
		State = 1;
		if (ACTIVITY_MANAGER.GetActivity(JUN_TUAN_ZHAN_ID) != NULL )
		{
			State = ACTIVITY_MANAGER.GetActivity(JUN_TUAN_ZHAN_ID)->GetState();
			if ( State ==  AS_NOT_START )
			{
				State = STATE_CLOSED;
			}
			else if ( State == AS_TIME_OUT )
			{
				State = STATE_END;
			}
		}
		packet->writeInt8( JUN_TUAN_WAR );
		packet->writeInt8( State );

		//vip�һ�
		State = VIP_GUA_JI_SINGLETON.GetState();
		if ( State == AS_NOT_START )
		{
			State = STATE_CLOSED;
		}
		packet->writeInt8( SAFETY_GUA_JI );
		packet->writeInt8( State );

		//֩��Ů��1
		State = 1;
		if (ACTIVITY_MANAGER.GetActivity(SQIDER_QUEEN_ID_1) != NULL )
		{
			State = ACTIVITY_MANAGER.GetActivity(SQIDER_QUEEN_ID_1)->GetState();
			if ( State ==  AS_NOT_START )
			{
				State = STATE_CLOSED;
			}
			else if ( State == AS_TIME_OUT )
			{
				State = STATE_END;
			}
		}
		packet->writeInt8( SQIDERQUEEN_1 );
		packet->writeInt8( State );

		//֩��Ů��2
		State = 1;
		if (ACTIVITY_MANAGER.GetActivity(SQIDER_QUEEN_ID_2) != NULL )
		{
			State = ACTIVITY_MANAGER.GetActivity(SQIDER_QUEEN_ID_2)->GetState();
			if ( State ==  AS_NOT_START )
			{
				State = STATE_CLOSED;
			}
			else if ( State == AS_TIME_OUT )
			{
				State = STATE_END;
			}
		}
		packet->writeInt8( SQIDERQUEEN_2 );
		packet->writeInt8( State );

		//֩��Ů��3
		State = 1;
		if (ACTIVITY_MANAGER.GetActivity(SQIDER_QUEEN_ID_3) != NULL )
		{
			State = ACTIVITY_MANAGER.GetActivity(SQIDER_QUEEN_ID_3)->GetState();
			if ( State ==  AS_NOT_START )
			{
				State = STATE_CLOSED;
			}
			else if ( State == AS_TIME_OUT )
			{
				State = STATE_END;
			}
		}
		packet->writeInt8( SQIDERQUEEN_3 );
		packet->writeInt8( State );

		//֩��Ů��4
		State = 1;
		if (ACTIVITY_MANAGER.GetActivity(SQIDER_QUEEN_ID_4) != NULL )
		{
			State = ACTIVITY_MANAGER.GetActivity(SQIDER_QUEEN_ID_4)->GetState();
			if ( State ==  AS_NOT_START )
			{
				State = STATE_CLOSED;
			}
			else if ( State == AS_TIME_OUT )
			{
				State = STATE_END;
			}
		}
		packet->writeInt8( SQIDERQUEEN_4 );
		packet->writeInt8( State );

		// ���ս
		State = ACTIVITY_MANAGER.GetTerritoryWarState();
		if ( State ==  AS_NOT_START )
		{
			State = STATE_CLOSED;
		}
		else if ( State == AS_TIME_OUT )
		{
			State = STATE_END;
		}
		packet->writeInt8( JCHD_TERRIORY_WAR );
		packet->writeInt8( State );
		

		packet->setSize(packet->getWOffset());
		GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);
	}
	else if ( Type == DAILY_TASK )
	{
		NetPacket *packet = GAME_SERVICE.popNetpacket(PACK_DISPATCH, SM_SEND_HD_DA_TING_INFO );
		if(NULL == packet)
		{
			return;
		}
		packet->writeInt8( Type );
		packet->writeInt32( DDT_COUNT - 1 );
		packet->writeInt8( DTT_CYCLE_TASK );
		packet->writeInt32( m_pPlayer->GetCharTaskCycle().GetSurplusTimes() );
		packet->writeInt8( DA_WEI_WANG );
		packet->writeInt32( GET_DWW_REWARD_TIMES - m_pPlayer->getRecord( RP_DAILY_DWW_TIMES ) );
		packet->writeInt8( FTT_FAMILY_TASK );
		packet->writeInt32( MAX_FAMILY_TASK_COUNT - m_pPlayer->getRecord( RP_FAMILY_TASK_COUNT ) );
		packet->setSize(packet->getWOffset());
		GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);
	}
}

void DailyActivity::GetHuoDongDaTingIcon( IconStateList& IconList )
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	if ( !m_pPlayer->GetPlayerFunctionOpen().IsOpened(FT_HUO_DONG_DA_TING))
	{
		return;
	}
	ShowIcon stu = {};
	stu.nId			= HUODONG_DA_TING;
	stu.nState		= AS_RUNNING;
	stu.nLeftTime	= -1;
	stu.IconRight	+= m_pPlayer->GetPlayerHuoYueDu().RewardCount();
	stu.IconRight   += HaveActivityRewardCount();
	if ( stu.IconRight > 0 )
	{
		stu.Effects = 1;
	}
	IconList.push_back( stu );
}

void DailyActivity::SendHuoDongDaTingIcon()
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	if ( !m_pPlayer->GetPlayerFunctionOpen().IsOpened(FT_HUO_DONG_DA_TING))
	{
		return;
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket(Answer::PACK_DISPATCH, SM_SEND_ONE_ICON);
	if (NULL == packet)
	{
		return;
	}
	ShowIcon stu = {};
	stu.nId			= HUODONG_DA_TING;
	stu.nState		= AS_RUNNING;
	stu.nLeftTime	= -1;
	stu.IconRight	+= m_pPlayer->GetPlayerHuoYueDu().RewardCount();
	stu.IconRight   += HaveActivityRewardCount();
	if ( stu.IconRight > 0 )
	{
		stu.Effects = 1;
	}
	packet->writeInt32( stu.nId );
	packet->writeInt8(  stu.nState );
	packet->writeInt32( stu.nLeftTime );
	packet->writeInt8( stu.IconLeft );
	packet->writeInt32( stu.IconRight );
	packet->writeInt8( stu.Effects );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);		
}

int32_t DailyActivity::HaveActivityRewardCount()
{
	int32_t Count = 0;
	std::list<int32_t> ActivityId;
	ActivityId.clear();
	ActivityId.push_back( JUN_TUAN_ZHAN_ID );
	ActivityId.push_back( CTERRITORYWAR_1 );
	ActivityId.push_back( CTERRITORYWAR_2 );
	ActivityId.push_back( CTERRITORYWAR_3 );
	ActivityId.push_back( CTERRITORYWAR_4 );
	ActivityId.push_back( CTERRITORYWAR_5 );
	std::list<int32_t>::iterator it = ActivityId.begin();
	for ( ; it != ActivityId.end(); ++it )
	{
		Count += ACTIVITY_MANAGER.HaveRewardCount( m_pPlayer,*it );
	}
	return Count;
}

void DailyActivity::GetJiangLiDaTingIcon( IconStateList& IconList )
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	if ( !m_pPlayer->GetPlayerFunctionOpen().IsOpened(FT_JIANG_LI_DA_TING))
	{
		return;
	}
	ShowIcon stu = GetShowIconStu();
	IconList.push_back( stu );
}

void DailyActivity::SendJiangLiDaTingIcon()
{
	if ( !m_pPlayer->GetPlayerFunctionOpen().IsOpened(FT_JIANG_LI_DA_TING))
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
	packet->writeInt8(  stu.nState );
	packet->writeInt32( stu.nLeftTime );
	packet->writeInt8( stu.IconLeft );
	packet->writeInt32( stu.IconRight );
	packet->writeInt8( stu.Effects );
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);		
}

ShowIcon DailyActivity::GetShowIconStu()
{
	ShowIcon stu = {};
	stu.nId			= JIANG_LI_DA_TING;
	stu.nState		= AS_RUNNING;
	stu.nLeftTime	= -1;
	if ( !TodayIsSign() )
	{
		stu.IconLeft = 1;
	}
	else if ( HaveSearchBackReward() )
	{
		stu.IconLeft = 2;
	}
	int32_t RewardCount = 0;
	if ( HaveSignReward() )
	{
		RewardCount++;
	}
	if ( HaveOnLineReward() || HaveWeekReward() )
	{
		RewardCount++;
	}
	if ( HaveSevenLoginReward() )
	{
		RewardCount++;
	}
	if ( HaveLevelReward() )
	{
		RewardCount++;
	}
	if ( HaveOffLineReard() )
	{
		RewardCount++;
	}
	if ( HaveSearchBackReward ())
	{
		RewardCount++;
	}
	stu.IconRight	= RewardCount;
	if ( stu.IconLeft > 0 || stu.IconRight > 0 )
	{
		stu.Effects = 1;
	}
	return stu;
};

bool DailyActivity::TodayIsSign()
{
	tm tmNow = m_pPlayer->getLocalNow();

	if ( (( m_signInfo.sign_record >> (tmNow.tm_mday - 1) ) & 1) > 0 )
	{
		return true;
	}
	return false;
}

bool DailyActivity::HaveSignReward()
{
	tm tmNow = m_pPlayer->getLocalNow();
	int32_t i = 0;
	int32_t signCount = 0;
	while ( i < tmNow.tm_mday )
	{
		signCount += (m_signInfo.sign_record >> i) & 1;
		i++;
	}

	CfgSignRewardTable ReardTable = CFG_DATA.GetSignRewardTable();
	CfgSignRewardTable::iterator it = ReardTable.begin();
	for ( ; it != ReardTable.end(); ++it )
	{
		if ( it->second.count <= signCount )
		{
			bool IsGet = false;
			vector<int8_t>::iterator itRecord = m_signInfo.sign_reward.begin();
			for ( ; itRecord != m_signInfo.sign_reward.end(); ++ itRecord )
			{
				if ( *itRecord == it->second.count )
				{
					IsGet = true;
					break;
				}
			}
			if ( !IsGet )
			{
				return true;
			}
		}
	}
	return false;
}

bool DailyActivity::HaveOnLineReward()
{

	int32_t TodayTime = GetTodayOnLineTime();
	int32_t RewardRecord  = m_pPlayer->getRecord( RP_DAILY_ONLIE_REWARD_INFO );
	CfgOnlineRewardTable OnLineRewardTable = CFG_DATA.GetOnLineRewardTable();
	CfgOnlineRewardTable::iterator it = OnLineRewardTable.begin();
	for ( ; it != OnLineRewardTable.end(); ++it )
	{
		if ( it->second.NeedTime <= TodayTime )
		{
			if ( ( ( RewardRecord >> ( it->first -1 ) ) & 1 ) <= 0 )
			{
				return true;
			}
		}
	}
	return false;
}

bool DailyActivity::HaveWeekReward()
{
	int32_t Week  = GetNewSeverWeek();
	if ( Week  <= 1) //��һ�ܿ϶�û��
	{
		return false;
	}
	if ( GetLastWeekOnlineTime() / HOUR_HAVE_SECONDS > 0 )
	{
		return true;
	}
	return false;
}

bool DailyActivity::HaveSevenLoginReward()
{
	if ( NULL == m_pPlayer )
	{
		return false;
	}
	int32_t LoginCount = m_pPlayer->getRecord( RP_LOGIN_COUNT );

	int32_t RewardRecord = m_pPlayer->getRecord( RP_DAILY_SEVEN_LOGIN_REWARD );
	int32_t i = 0;
	while( i < LoginCount && i < 7 )
	{
		if ( ( ( RewardRecord >> i ) & 1 ) <= 0 )
		{
			return true;
		}
		i++;
	}
	return false;
}

bool DailyActivity::HaveLevelReward()
{	
	if ( NULL == m_pPlayer )
	{
		return false;
	}
	int32_t Level = m_pPlayer->getLevel();
	int32_t RewardRecord = m_pPlayer->getRecord( RP_LEVEL_GIFT_REWARD );
	CfgLevelGiftTable LevelGiftTable =	CFG_DATA.GetLeveGiftTable();
	CfgLevelGiftTable::iterator it = LevelGiftTable.begin();
	for ( ; it != LevelGiftTable.end(); ++it )
	{
		if ( it->second.Level <= Level )
		{
			if ( ( ( RewardRecord >> ( it->first -1 ) ) & 1 ) <= 0 )
			{
				return true;
			}
		}
	}
	return false;
}

bool DailyActivity::HaveOffLineReard()
{
	int32_t OfflineSeconds = m_pPlayer->getRecord( PR_ACCUMULATIVE_OFFLINE_TIME );
	return OfflineSeconds > 60*60;
}

bool DailyActivity::HaveSearchBackReward()
{
	CfgZYZHMap&	pCfgMap = CFG_DATA.GetZYZHList();
	CfgZYZHMap::iterator it = pCfgMap.begin();
	for ( ; it != pCfgMap.end(); ++it )
	{
		SearchBackInfo stu = {};
		if ( IsAlreadyGetSearchBackReward( it->first ) )
		{
			continue;
		}
		stu = GetSearchBackInfo( it->first );
		if ( stu.LeftTimes <= 0 )
		{
			continue;
		}
		return true;
	}
	return false;
}

void DailyActivity::OnLevelUp()
{
	if ( HaveLevelReward() )
	{
		SendJiangLiDaTingIcon();
	}
}

void DailyActivity::UpDateJiangLiDatingIcon( int64_t curTick )
{
	if ( NULL == m_pPlayer )
	{
		return;
	}

	if ( curTick - m_LastUpdateIcon > 60 * 1000 )
	{
		if ( HaveOnLineReward() )
		{
			SendJiangLiDaTingIcon();
		}
		m_LastUpdateIcon = curTick;
	}
}

void DailyActivity::RecordEnterNumber( int32_t ActivityType, int32_t ActivityId )
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	m_pPlayer->GetAchievemnet().AddAchievement( AT_ACTIVITY, ActivityType,ActivityId );
	m_ActivityRecord[ActivityType].JoinedIdSet.insert( ActivityId );
	m_pPlayer->GetPlayerHuoYueDu().AddHuoYueDuRecord( HYDT_ACTIVITY, ActivityType );
}

void DailyActivity::AddYesterdayRecord( int32_t ActivityType, int32_t ActivityId )
{
	if ( NULL == m_pPlayer )
	{
		return;
	}
	m_ActivityRecord[ActivityType].YesterdayRecord.insert( ActivityId );
}

int32_t DailyActivity::GetActivityRecord( int32_t SearchBackType )
{
	JoinedActivityRecord::iterator it = m_ActivityRecord.find( SearchBackType );
	if ( it != m_ActivityRecord.end() )
	{
		return it->second.YesterdayRecord.size();
	}
	return 0;
}

void DailyActivity::DaySwitch( int32_t nDiffDays )
{
	if ( nDiffDays == 0 )
	{
		return;
	}
	JoinedActivityRecord::iterator it = m_ActivityRecord.begin();
	for ( ; it != m_ActivityRecord.end(); ++it )
	{
		if ( nDiffDays == 1)
		{
			it->second.YesterdayRecord  = it->second.JoinedIdSet;
			it->second.JoinedIdSet.clear();
		}
		else if( nDiffDays > 1 ||  nDiffDays < 0 )
		{
			it->second.YesterdayRecord.clear();
			it->second.JoinedIdSet.clear();
		}
	}
}

SearchBackInfo DailyActivity::GetSearchBackInfo( int32_t Index )
{	
	SearchBackInfo stu = {};
	if ( NULL == m_pPlayer )
	{
		return stu;
	}


	int32_t Diff = DayTime::daydiff( m_pPlayer->getCreateTime());
	if ( Diff == 0 )
	{
		return stu;
	}
	CfgZiYuanZhaoHui*	pCfg = CFG_DATA.GetZiYuanZhaoHui( Index );
	if ( NULL == pCfg )
	{
		return stu;
	}
	stu.Index = pCfg->Index;
	if ( pCfg->Type == SBT_ACTIVITY )
	{
		stu.LeftTimes = pCfg->Times - GetActivityRecord( pCfg->Id );
	}
	else if ( pCfg->Type = SBT_DUNGEON )
	{
		stu.LeftTimes = pCfg->Times - m_pPlayer->getRecord( pCfg->Id + SPECIAL_FLAG );
	}
	return stu;
}

void DailyActivity::SendSearchBackInfo()
{
	std::list<SearchBackInfo> SearchBackList;
	CfgZYZHMap&	pCfgMap = CFG_DATA.GetZYZHList();
	CfgZYZHMap::iterator it = pCfgMap.begin();
	for ( ; it != pCfgMap.end(); ++it )
	{
		SearchBackInfo stu = {};
		if ( IsAlreadyGetSearchBackReward( it->first ) )
		{
			continue;
		}
		stu = GetSearchBackInfo( it->first );
		if ( stu.LeftTimes <= 0 )
		{
			continue;
		}
		SearchBackList.push_back( stu );
	}
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_SEARCH_BACK_INFO );
	if (NULL == packet)
	{
		return;
	}
	int32_t nSize = SearchBackList.size();
	packet->writeInt32( nSize );
	std::list<SearchBackInfo>::iterator iter = SearchBackList.begin();
	for ( ; iter != SearchBackList.end(); ++iter )
	{
		packet->writeInt32( iter->Index );
		packet->writeInt32( iter->LeftTimes );
	}
	packet->setSize(packet->getWOffset());
	GAME_SERVICE.sendPacketTo(m_pPlayer->getGateIndex(), packet);		
}

bool DailyActivity::IsAlreadyGetSearchBackReward( int32_t Index )
{
	int32_t RewardRecord = m_pPlayer->getRecord( PR_SEARCH_BACK_REWARD_INFO );
	int32_t i = 0;
	if ( ( ( RewardRecord >> ( Index - 1 ) ) & 1 ) > 0 )
	{
		return true;
	}
	return false;
}

int32_t DailyActivity::RecordEnterNumber( Answer::NetPacket *inPacket )
{
	if ( NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t ActivityId		= inPacket->readInt32();
	int32_t ActivityType	= inPacket->readInt32();
	RecordEnterNumber( ActivityType, ActivityId );
	return ERR_OK;
}

int32_t DailyActivity::OnGetSearchBackReward( Answer::NetPacket *inPacket )
{
	if ( NULL == m_pPlayer || NULL == inPacket )
	{
		return ERR_SYETEM_ERR;
	}
	int8_t GetType = inPacket->readInt8();
	int32_t Index  = inPacket->readInt32();
	SearchBackInfo stu = GetSearchBackInfo( Index );
	if ( stu.LeftTimes <= 0 )
	{
		return ERR_SYETEM_ERR;
	}
	int32_t OldRecord = m_pPlayer->getRecord( PR_SEARCH_BACK_REWARD_INFO );
	int32_t NewRecord = OldRecord | ( 1 << ( Index - 1 ) ); 
	if ( OldRecord == NewRecord )
	{
		return ERR_SYETEM_ERR;
	}
	CfgZiYuanZhaoHui* pCfg = CFG_DATA.GetZiYuanZhaoHui( Index );
	float SearchBackRate = 1.0f;
	if ( GetType == 1 )					//ͭǮ�һ�
	{
		if ( pCfg->NeedMoney <= 0 )
		{
			return ERR_SYETEM_ERR;
		}
		if ( !m_pPlayer->GetCurrency().DecMoneyAndNoBind( pCfg->NeedMoney, MCR_SEARCH_BACK ) )
		{
			return ERR_SYETEM_ERR;
		}
		SearchBackRate = SEARCH_BACK_RATE;
	}
	else if( GetType == 2 )				//Ԫ���һ�
	{
		if ( pCfg->NeedGold <= 0 )
		{
			return ERR_SYETEM_ERR;
		}
		if ( !m_pPlayer->DecCurrency( CURRENCY_GOLD, pCfg->NeedGold, MCR_SEARCH_BACK ) )
		{
			return ERR_SYETEM_ERR;
		}
	}
	else
	{
		return ERR_SYETEM_ERR;
	}
	stu.LeftTimes--;

	if ( pCfg->Type == SBT_ACTIVITY )
	{
		AddYesterdayRecord( pCfg->Id, static_cast<int32_t>( m_pPlayer->getTick() ) );
	}
	else if ( pCfg->Type = SBT_DUNGEON )
	{
		m_pPlayer->updateRecord( pCfg->Id + SPECIAL_FLAG, m_pPlayer->getRecord( pCfg->Id + SPECIAL_FLAG ) + 1 );
	}

	if ( stu.LeftTimes <= 0 )
	{
		m_pPlayer->updateRecord( PR_SEARCH_BACK_REWARD_INFO, NewRecord );
	}
	//���Ӿ���
	if ( pCfg->GetExpValues > 0 )
	{
		m_pPlayer->addExp( static_cast<int32_t>(pCfg->GetExpValues * SearchBackRate ) );
	}
	//������Դ
	CurrencyList::iterator it = pCfg->GetCurrencyList.begin();
	for ( ; it != pCfg->GetCurrencyList.end(); ++it )
	{
		if ( it->Values > 0 )
		{
			m_pPlayer->AddCurrency( static_cast<CURRENCY_TYPE>(it->Type),static_cast<int32_t>( it->Values * SearchBackRate ), MCR_SEARCH_BACK );
		}
	}
	SendSearchBackInfo();
	SendJiangLiDaTingIcon();
	return ERR_OK;
}

int32_t DailyActivity::GetJoinActivityCount( int32_t Type )
{
	JoinedActivityRecord::iterator it = m_ActivityRecord.find(Type);
	if ( it != m_ActivityRecord.end() )
	{
		return	it->second.JoinedIdSet.size();
	}
	return 0;
}

bool DailyActivity::TodayHaveJoineThisActivity( int32_t ActivityType, int32_t HuoDongId )
{
	JoinedActivityId::iterator it = m_ActivityRecord[ActivityType].JoinedIdSet.find( HuoDongId );
	if ( it != m_ActivityRecord[ActivityType].JoinedIdSet.end() )
	{
		return true;
	}
	return false;
}