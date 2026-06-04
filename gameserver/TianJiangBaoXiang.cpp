#include "TianJiangBaoXiang.h"
#include "GameService.h"
#include "PlantActivity.h"
#include "MapManager.h"
#include "ActivityManager.h"
#include "DBService.h"

enum GRADE_BAO_XIANG_STATE
{
	NOT_FLUSH			= 0,		//û��ˢ��
	ALREADY_FLUSH		= 1,		//��ˢ��
	ALREADY_GATHER		= 2,		//�ѱ��ɼ�
};

#define MAX_CAN_GATHER	  10		//���ɼ���
#define SUB_MOMEY	      1000000   //������Ҫ��ͭǮ
CTianJiangBaoXiang::CTianJiangBaoXiang( const CfgActivity& cfgActivity )
:CActivity( cfgActivity )
{
	GradeBaoXiangState = 0;
}

CTianJiangBaoXiang::~CTianJiangBaoXiang()
{

}void CTianJiangBaoXiang::addPlayer( Player *player )
{
	CActivity::addPlayer( player );
	if ( NULL == player )
	{
		return;
	}
	player->SetPkProtectTime( 0 );

	ActivityRewardRecord RewardRecord = {};
	PlayerMap::iterator ItRecord = m_PlayerSoreMap.find( player->getCid() );
	if ( ItRecord == m_PlayerSoreMap.end() )
	{
		m_PlayerSoreMap[player->getCid()] = RewardRecord;
	}
}int32_t CTianJiangBaoXiang::canEnter( Player* player, CActivityMap* pTargetMap ) const
{
	if ( NULL == player || NULL == pTargetMap )
	{
		return ERR_SYETEM_ERR;
	}
	return ERR_OK;
}

void CTianJiangBaoXiang::reset()
{
	CActivity::reset();
	GradeBaoXiangState					= 0;
	m_PlayerSoreMap.clear(); 
}

void CTianJiangBaoXiang::SendPlayerActivityInfo( Player* player )
{
	if ( NULL == player )
	{
		return;
	}

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_NOTIFY_ACTIVITY_INFO );
	if ( NULL == packet )
	{
		return;
	}
	packet->writeInt32( m_cfgActivity.id );
	packet->writeInt32( getNextStartTime() );
	packet->writeInt64( ACTIVITY_MANAGER.GetFamilyWarWinner() );
	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.sendPacketTo( player->getGateIndex(), packet );
}

void CTianJiangBaoXiang::SendPlayerActivityScore( Player* player, int32_t nLeftTime )
{
	if ( NULL == player )
	{
		return;
	}
	int32_t AlreadGrageCount = 0;
	int8_t  IsGetSpecial	 = 0;
	int32_t BuyCount		 = 0;
	PlayerMap::iterator it = m_PlayerSoreMap.find( player->getCid());
	if ( it != m_PlayerSoreMap.end() )
	{
		AlreadGrageCount	= it->second.OpenBaoXiangCount;
		IsGetSpecial		= it->second.IsGetSpecial;
		BuyCount			= it->second.BuyCount;
	}
	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_NOTIFY_ACTIVITY_SCORE );
	if ( NULL == packet )
	{
		return;
	}

	packet->writeInt32( m_cfgActivity.id );
	packet->writeInt8( GradeBaoXiangState );
	packet->writeInt32( BuyCount + 10 - AlreadGrageCount );
	packet->writeInt8( IsGetSpecial );
	packet->writeInt32( nLeftTime );
	packet->writeInt32( BuyCount );
	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.sendPacketTo( player->getGateIndex(), packet );
}

void CTianJiangBaoXiang::AddPlant( Plant* plant )
{
	if ( NULL == plant )
	{
		return;
	}
	if ( plant->GetPlantType() == PT_GRADE_BAO_XIANG )
	{
		GradeBaoXiangState = ALREADY_FLUSH;
		Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM );
		if (NULL != packet)
		{
			packet->writeInt32( BCI_GRADE_BAO_XIANG );
			packet->setSize(packet->getWOffset());
			GAME_SERVICE.worldBroadcast(packet);
		}
	}
	PlayerList::iterator it = m_players.begin();
	for ( ; it != m_players.end(); ++it )
	{
		SendPlayerActivityScore( *it, getLeftTime() );
	}
}

int32_t	CTianJiangBaoXiang::GiveDailyReward( Player* player )
{
	if ( NULL == player )
	{
		return ERR_SYETEM_ERR;
	}
	ActivityRewardRecord RewardRecord = {};
	PlayerMap::iterator ItRecord = m_PlayerSoreMap.find( player->getCid());
	if ( ItRecord != m_PlayerSoreMap.end() )
	{
		RewardRecord = ItRecord->second;
	}
	if ( RewardRecord.IsGetSpecial != 0 )
	{
		return ERR_SYETEM_ERR;
	}
	MemChrBagVector addItem;
	Int32Vector::const_iterator it = m_cfgActivity.gift_id.begin();
	for ( ; it != m_cfgActivity.gift_id.end(); ++it )
	{
		MemChrBag stu = {};
		stu.itemClass	= IC_NORMAL;
		stu.itemId		= *it;
		stu.itemCount	= 1;
		stu.bind		= IBS_BIND;
		addItem.push_back(stu);
	}
	if ( !player->GetBag().AddItem( addItem, IACR_TIAN_JIANG_BAO_XIANG_DAILY_REWARD ) )
	{
		return ERR_INVALID_DATA;
	}
	RewardRecord.IsGetSpecial = 1;
	m_PlayerSoreMap[player->getCid()] = RewardRecord;
	SendPlayerActivityScore( player, getLeftTime() ); 
	return ERR_OK;
}

void CTianJiangBaoXiang::onPlantGather( Plant* pPlant, Player *player )
{
	if ( NULL == pPlant || NULL == player )
	{
		return;
	}
	if ( pPlant->GetPlantType() == PT_GRADE_BAO_XIANG )
	{
		GradeBaoXiangState = ALREADY_GATHER;
		PlayerList::iterator it = m_players.begin();
		for ( ; it != m_players.end(); ++it )
		{
			SendPlayerActivityScore( *it,getLeftTime() );
		}
	}
	else
	{
		m_PlayerSoreMap[player->getCid()].OpenBaoXiangCount += 1;
		SendPlayerActivityScore( player, getLeftTime() );
	}

	LogActivity logActivity = {};
	logActivity.cid = player->getCid();
	logActivity.actid = GetId();
	logActivity.acttype = GetType();
	logActivity.time = player->getNow();
	logActivity.param = pPlant->getPlantId();

	DB_SERVICE.InsertActivityLog( logActivity );
}

int32_t CTianJiangBaoXiang::onBeginGather(  Plant* plant, Player *player )
{
	if ( NULL == player || NULL == plant )
	{
		return ERR_SYETEM_ERR;
	}
	if ( plant->GetPlantType() == PT_GRADE_BAO_XIANG )
	{
		return ERR_OK;
	}
	PlayerMap::iterator it = m_PlayerSoreMap.find( player->getCid());
	if ( it == m_PlayerSoreMap.end() )
	{
		return ERR_OK;
	}
	if ( it->second.OpenBaoXiangCount >= it->second.BuyCount + 10 )
	{
		return ERR_SYETEM_ERR;
	}
	return ERR_OK;
}

bool CTianJiangBaoXiang::AddOpenCount( Player* player, int32_t AddCount )
{
	if ( NULL == player )
	{
		return false;
	}
	if ( getState() != AS_RUNNING )
	{
		return false;
	}
	PlayerMap::iterator it = m_PlayerSoreMap.find( player->getCid() );
	if ( it == m_PlayerSoreMap.end() )
	{
		return false;
	}
	if ( it->second.BuyCount + AddCount > 5 )
	{
		return false;
	}
	it->second.BuyCount += AddCount;
	return true;
}

void CTianJiangBaoXiang::broadcastReady()
{
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM );
	if ( NULL == packet )
	{
		return;
	}
	packet->writeInt32( 142 );
	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.worldBroadcast( packet );
}

void CTianJiangBaoXiang::broadcastStart()
{
	Answer::NetPacket *packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_NOTICE_PARAM );
	if ( NULL == packet )
	{
		return;
	}
	packet->writeInt32( 143 );
	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.worldBroadcast( packet );
}