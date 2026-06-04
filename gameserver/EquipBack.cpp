#include "EquipBack.h"
#include "GameService.h"
#include "Player.h"
#include "DBService.h"
#include "Bag.h"
#include "Task.h"

//----------------------------------------------------------------------
// CEquipBack 构造/析构
//----------------------------------------------------------------------
CEquipBack::CEquipBack()
{
	m_BackRecord.clear();
	m_BackDepotMap.clear();
	m_BackLimitInfoMap.clear();
}

CEquipBack::~CEquipBack()
{
	m_BackRecord.clear();
	m_BackDepotMap.clear();
	m_BackLimitInfoMap.clear();
}

//----------------------------------------------------------------------
// Init - 从数据库加载数据
//----------------------------------------------------------------------
void CEquipBack::Init( int32_t line )
{
	// line == 9 跳过 (调试线)
	if ( line == 9 )
	{
		return;
	}

	// TODO: 从数据库加载 m_BackRecord, m_BackDepotMap, m_BackLimitInfoMap
	// SELECT * FROM `mem_equip_back_record` order by nTime desc Limit 0, 50
	// SELECT * FROM `mem_equip_back_depot`
}

//----------------------------------------------------------------------
// OnGoldBack - 装备回收
//----------------------------------------------------------------------
int32_t CEquipBack::OnGoldBack( Player* pPlayer, Answer::NetPacket* packet )
{
	if ( NULL == pPlayer || NULL == packet )
	{
		return ERR_SYETEM_ERR;
	}

	pPlayer->GetTask().updateTaskCount( 42, 1 );

	int32_t nId = packet->readInt32();
	const EquipBack* pCfg = CFG_DATA.GetEquipBackCfg( nId );
	if ( NULL == pCfg )
	{
		return ERR_SYETEM_ERR;
	}

	// 读取客户端传来的槽位信息
	Int32Vector vSlot;
	pPlayer->queryBagInfo( packet, vSlot );

	// 构建需要删除的物品列表
	ItemDataList ItemList;
	Int32List::const_iterator it = pCfg->nEquipList.begin();
	Int32List::const_iterator eit = pCfg->nEquipList.end();
	for ( ; it != eit; ++it )
	{
		ItemData item = {};
		item.m_nClass = IC_EQUIP;
		item.m_nCount = 1;
		item.m_nId = *it;
		ItemList.push_back( item );
	}

	if ( ItemList.empty() )
	{
		return ERR_SYETEM_ERR;
	}

	if ( pCfg->nType == 1 )
	{
		// 类型1: 金币回收
		if ( pCfg->nLimitNum <= 0 )
		{
			return ERR_SYETEM_ERR;
		}

		// 检查开服天数
		if ( pCfg->nDisplayDay > 0 )
		{
			int32_t DayDiff = CFG_DATA.getServerDiffDay( SVT_NORMAL ) + 1;
			if ( pCfg->nDisplayDay > DayDiff )
			{
				return ERR_SYETEM_ERR;
			}
		}

		// 检查限制次数
		{
			Answer::MutexGuard lock( m_LimitLock );
			BackLimitInfoMap::iterator itLimit = m_BackLimitInfoMap.find( nId );
			if ( itLimit != m_BackLimitInfoMap.end() && itLimit->second.nCount >= pCfg->nLimitNum )
			{
				return ERR_SYETEM_ERR;
			}
		}

		if ( pCfg->nRecovType < 0 || pCfg->nRecovValues <= 0 )
		{
			return ERR_SYETEM_ERR;
		}

		// 删除物品
		if ( !pPlayer->GetBag().RemoveItem( vSlot, ItemList, IDCR_EQUIP_GOLD_BACK ) )
		{
			return ERR_SYETEM_ERR;
		}

		// 发放回收奖励
		pPlayer->AddCurrency( (CURRENCY_TYPE)pCfg->nRecovType, pCfg->nRecovValues, GCR_EQUIP_GOLD_BACK, 0 );

		// 更新限制次数
		{
			BackLimitInfo stu;
			stu.nId = nId;
			stu.nCount = 1;
			stu.sLastName = pPlayer->getName();
			AddLimitCount( &stu );
		}
		SendEquipBackLimitChange( pPlayer, nId );

		// 广播公告
		{
			Answer::NetPacket* packet_0 = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_EQUIP_BACK_BROADCAST );
			if ( packet_0 )
			{
				packet_0->writeInt32( 451 );
				packet_0->writeUTF8( pPlayer->getName() );
				packet_0->writeInt64( pPlayer->getCid() );
				ItemDataList::iterator itItem = ItemList.begin();
				packet_0->writeInt32( itItem->m_nId );
				packet_0->writeInt32( pCfg->nRecovValues );
				packet_0->setSize( packet_0->getWOffset() );
				GAME_SERVICE.worldBroadcast( packet_0 );
			}
		}
	}
	else if ( pCfg->nType == 2 )
	{
		// 类型2: 开服回收
		int32_t DayDiff = CFG_DATA.getServerDiffDay( SVT_NORMAL );
		if ( DayDiff + 1 > pCfg->nOpenDay )
		{
			return ERR_SYETEM_ERR;
		}

		if ( pCfg->nLimitNum <= 0 )
		{
			return ERR_SYETEM_ERR;
		}

		// 检查限制次数
		{
			Answer::MutexGuard lock( m_LimitLock );
			BackLimitInfoMap::iterator itLimit = m_BackLimitInfoMap.find( nId );
			if ( itLimit != m_BackLimitInfoMap.end() && itLimit->second.nCount >= pCfg->nLimitNum )
			{
				return ERR_SYETEM_ERR;
			}
		}

		if ( pCfg->nRecovType < 0 || pCfg->nRecovValues <= 0 )
		{
			return ERR_SYETEM_ERR;
		}

		// 删除物品
		if ( !pPlayer->GetBag().RemoveItem( vSlot, ItemList, IDCR_EQUIP_GOLD_BACK ) )
		{
			return ERR_SYETEM_ERR;
		}

		// 发放回收奖励
		pPlayer->AddCurrency( (CURRENCY_TYPE)pCfg->nRecovType, pCfg->nRecovValues, GCR_EQUIP_GOLD_BACK, 0 );

		// 更新限制次数
		{
			BackLimitInfo stu;
			stu.nId = nId;
			stu.nCount = 1;
			stu.sLastName = pPlayer->getName();
			AddLimitCount( &stu );
		}
		SendEquipBackLimitChange( pPlayer, nId );

		// 广播公告
		{
			Answer::NetPacket* packet_1 = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_EQUIP_BACK_BROADCAST );
			if ( packet_1 )
			{
				packet_1->writeInt32( 443 );
				packet_1->writeInt64( pPlayer->getCid() );
				packet_1->writeUTF8( pPlayer->getName() );
				packet_1->writeInt32( nId );
				packet_1->writeInt8( pCfg->nRecovType );
				packet_1->writeInt32( pCfg->nRecovValues );
				packet_1->setSize( packet_1->getWOffset() );
				GAME_SERVICE.worldBroadcast( packet_1 );
			}
		}
	}
	else
	{
		return ERR_SYETEM_ERR;
	}

	// 记录
	{
		BackRecord stu;
		stu.nId = nId;
		stu.nType = 1;
		stu.nTime = Answer::DayTime::now();
		stu.name = pPlayer->getName();

		SendEquipBackOnRecord( pPlayer, &stu );
		AddRecord( &stu );
	}

	return ERR_OK;
}

//----------------------------------------------------------------------
// OnBuyBack - 装备回购
//----------------------------------------------------------------------
int32_t CEquipBack::OnBuyBack( Player* pPlayer, Answer::NetPacket* packet )
{
	if ( NULL == pPlayer || NULL == packet )
	{
		return ERR_SYETEM_ERR;
	}

	int32_t nId = packet->readInt32();
	const EquipBack* pCfg = CFG_DATA.GetEquipBackCfg( nId );
	if ( NULL == pCfg )
	{
		return ERR_SYETEM_ERR;
	}

	if ( pCfg->nType != 1 )
	{
		return ERR_SYETEM_ERR;
	}

	if ( pCfg->nRecovType < 0 || pCfg->nRecovValues <= 0 )
	{
		return ERR_SYETEM_ERR;
	}

	// 检查库存
	BackDepotMap::iterator itDepot;
	{
		Answer::MutexGuard lock( m_DepotLock );
		itDepot = m_BackDepotMap.find( nId );
		if ( itDepot == m_BackDepotMap.end() || itDepot->second.Count <= 0 )
		{
			return ERR_SYETEM_ERR;
		}
	}

	// 构建物品列表
	MemChrBagVector ItemVt;
	Int32List::const_iterator it = pCfg->nEquipList.begin();
	Int32List::const_iterator eit = pCfg->nEquipList.end();
	for ( ; it != eit; ++it )
	{
		MemChrBag item = {};
		item.itemClass = IC_EQUIP;
		item.itemCount = 1;
		item.itemId = *it;
		ItemVt.push_back( item );
	}

	// 检查背包空格
	if ( pPlayer->GetBag().GetFreeSlotCount() < (int32_t)ItemVt.size() )
	{
		return ERR_SYETEM_ERR;
	}

	// 扣除货币
	if ( !pPlayer->DecCurrency( (CURRENCY_TYPE)pCfg->nBuyBackType, pCfg->nBuyBackValue, GCR_EQUIP_BUY_BACK, 0 ) )
	{
		return ERR_SYETEM_ERR;
	}

	// 减少库存
	BackDepot deptStu;
	deptStu.nId = nId;
	deptStu.Count = -1;  // 减少1
	ChangeDeptCount( deptStu );

	// 发放物品
	pPlayer->GetBag().AddItem( ItemVt, IACR_EQUIP_BUY_BACK );

	// 记录
	{
		BackRecord stu;
		stu.nId = nId;
		stu.nType = 2;
		stu.nTime = Answer::DayTime::now();
		stu.name = pPlayer->getName();

		SendEquipBackOnRecord( pPlayer, &stu );
		SendEquipBackCountChange( pPlayer, nId );
		AddRecord( &stu );
	}

	// 广播公告
	{
		Answer::NetPacket* SendPacket = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_EQUIP_BACK_BROADCAST );
		if ( SendPacket )
		{
			SendPacket->writeInt32( 452 );
			SendPacket->writeUTF8( pPlayer->getName() );
			SendPacket->writeInt64( pPlayer->getCid() );
			MemChrBagVector::iterator itItem = ItemVt.begin();
			SendPacket->writeInt32( itItem->itemId );
			SendPacket->writeInt32( pCfg->nBuyBackValue );
			SendPacket->setSize( SendPacket->getWOffset() );
			GAME_SERVICE.worldBroadcast( SendPacket );
		}
	}

	return ERR_OK;
}

//----------------------------------------------------------------------
// KiaFuEquipBack - 开服活动自动回收(服务器调用)
//----------------------------------------------------------------------
void CEquipBack::KiaFuEquipBack( int32_t Index, int32_t EquipId, int32_t Mid, Player* pPlayer )
{
	if ( NULL == pPlayer )
	{
		return;
	}

	const EquipBack* pCfg = CFG_DATA.GetEquipBackCfg( Index );
	if ( NULL == pCfg )
	{
		return;
	}

	// 检查EquipId是否在配置列表中
	bool bFound = false;
	Int32List::const_iterator it = pCfg->nEquipList.begin();
	Int32List::const_iterator eit = pCfg->nEquipList.end();
	for ( ; it != eit; ++it )
	{
		if ( *it == EquipId )
		{
			bFound = true;
			break;
		}
	}

	if ( !bFound || pCfg->nEquipList.empty() )
	{
		return;
	}

	// 检查开服天数和限制
	int32_t DayDiff = CFG_DATA.getServerDiffDay( SVT_NORMAL );
	if ( DayDiff + 1 > pCfg->nOpenDay || pCfg->nLimitNum <= 0 )
	{
		return;
	}

	// 检查限制次数
	{
		Answer::MutexGuard lock( m_LimitLock );
		BackLimitInfoMap::iterator itLimit = m_BackLimitInfoMap.find( Index );
		if ( itLimit != m_BackLimitInfoMap.end() && itLimit->second.nCount >= pCfg->nLimitNum )
		{
			return;
		}
	}

	if ( pCfg->nRecovType < 0 || pCfg->nRecovValues <= 0 )
	{
		return;
	}

	// 发放系统邮件
	MemChrBag item = {};
	item.itemClass = IC_ITEM;
	item.itemId = TranseCurrencyItem( (CURRENCY_TYPE)pCfg->nRecovType );
	item.itemCount = pCfg->nRecovValues;

	std::stringstream Param;
	Param << EquipId;
	DB_SERVICE.OnSendSysMail( 0, pPlayer->getCid(), 6373, &item, IACR_KAI_HUO_EQUIP_BACK, &Param.str(), 0 );

	// 广播公告
	{
		Answer::NetPacket* packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_EQUIP_BACK_BROADCAST );
		if ( packet )
		{
			packet->writeInt32( 441 );
			packet->writeInt64( pPlayer->getCid() );
			packet->writeUTF8( pPlayer->getName() );
			packet->writeInt8( 2 );
			packet->writeInt32( EquipId );
			packet->writeInt32( 1 );
			packet->writeInt8( pCfg->nRecovType );
			packet->writeInt32( pCfg->nRecovValues );
			packet->setSize( packet->getWOffset() );
			GAME_SERVICE.worldBroadcast( packet );
		}
	}

	// 更新限制
	{
		BackLimitInfo stu;
		stu.nId = Index;
		stu.nCount = 1;
		stu.sLastName = pPlayer->getName();
		AddLimitCount( &stu );
	}
	SendEquipBackLimitChange( pPlayer, Index );
}

//----------------------------------------------------------------------
// SendEquipBackInfo - 发送装备回购完整信息给玩家
//----------------------------------------------------------------------
void CEquipBack::SendEquipBackInfo( Player* pPlayer )
{
	if ( NULL == pPlayer )
	{
		return;
	}

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_EQUIP_BACK_INFO );
	if ( NULL == packet )
	{
		return;
	}

	// 记录列表
	packet->writeInt32( (int32_t)m_BackRecord.size() );
	BackRecordList::iterator it = m_BackRecord.begin();
	BackRecordList::iterator eit = m_BackRecord.end();
	for ( ; it != eit; ++it )
	{
		packet->writeInt32( it->nId );
		packet->writeInt8( it->nType );
		packet->writeUTF8( it->name );
		packet->writeInt32( it->nTime );
	}

	// 仓库库存
	packet->writeInt32( (int32_t)m_BackDepotMap.size() );
	BackDepotMap::iterator dIt = m_BackDepotMap.begin();
	BackDepotMap::iterator dEit = m_BackDepotMap.end();
	for ( ; dIt != dEit; ++dIt )
	{
		packet->writeInt32( dIt->first );
		packet->writeInt32( dIt->second.Count );
	}

	// 限制信息
	packet->writeInt32( (int32_t)m_BackLimitInfoMap.size() );
	BackLimitInfoMap::iterator lIt = m_BackLimitInfoMap.begin();
	BackLimitInfoMap::iterator lEit = m_BackLimitInfoMap.end();
	for ( ; lIt != lEit; ++lIt )
	{
		packet->writeInt32( lIt->first );
		packet->writeInt32( lIt->second.nCount );
		packet->writeUTF8( lIt->second.sLastName );
	}

	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.sendPacketTo( pPlayer->getGateIndex(), packet );
}

//----------------------------------------------------------------------
// SendEquipBackOnRecord - 发送单条记录更新
//----------------------------------------------------------------------
void CEquipBack::SendEquipBackOnRecord( Player* pPlayer, BackRecord* p_stu )
{
	if ( NULL == pPlayer || NULL == p_stu )
	{
		return;
	}

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_EQUIP_BACK_ON_RECORD );
	if ( NULL == packet )
	{
		return;
	}

	packet->writeInt32( p_stu->nId );
	packet->writeInt8( p_stu->nType );
	packet->writeUTF8( p_stu->name );
	packet->writeInt32( p_stu->nTime );
	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.sendPacketTo( pPlayer->getGateIndex(), packet );
}

//----------------------------------------------------------------------
// SendEquipBackCountChange - 发送库存数量变化
//----------------------------------------------------------------------
void CEquipBack::SendEquipBackCountChange( Player* pPlayer, int32_t nId )
{
	if ( NULL == pPlayer )
	{
		return;
	}

	Answer::MutexGuard lock( m_DepotLock );
	BackDepotMap::iterator it = m_BackDepotMap.find( nId );
	if ( it == m_BackDepotMap.end() )
	{
		return;
	}

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_EQUIP_BACK_COUNT_CHANGE );
	if ( NULL == packet )
	{
		return;
	}

	packet->writeInt32( it->first );
	packet->writeInt32( it->second.Count );
	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.sendPacketTo( pPlayer->getGateIndex(), packet );
}

//----------------------------------------------------------------------
// SendEquipBackLimitChange - 发送限制次数变化
//----------------------------------------------------------------------
void CEquipBack::SendEquipBackLimitChange( Player* pPlayer, int32_t nId )
{
	if ( NULL == pPlayer )
	{
		return;
	}

	Answer::MutexGuard lock( m_LimitLock );
	BackLimitInfoMap::iterator it = m_BackLimitInfoMap.find( nId );
	if ( it == m_BackLimitInfoMap.end() )
	{
		return;
	}

	Answer::NetPacket* packet = GAME_SERVICE.popNetpacket( Answer::PACK_DISPATCH, SM_SEND_EQUIP_BACK_LIMIT_CHANGE );
	if ( NULL == packet )
	{
		return;
	}

	packet->writeInt32( it->first );
	packet->writeInt32( it->second.nCount );
	packet->writeUTF8( it->second.sLastName );
	packet->setSize( packet->getWOffset() );
	GAME_SERVICE.sendPacketTo( pPlayer->getGateIndex(), packet );
}

//----------------------------------------------------------------------
// AddRecord - 添加记录(线程安全)
//----------------------------------------------------------------------
void CEquipBack::AddRecord( BackRecord* p_stu )
{
	if ( NULL == p_stu )
	{
		return;
	}

	Answer::MutexGuard lock( m_RecordLock );

	if ( m_BackRecord.size() > 50 )
	{
		m_BackRecord.pop_front();
	}

	m_BackRecord.push_back( *p_stu );

	// TODO: 持久化到数据库
	// DB_SERVICE.SaveEquipBackRecord( p_stu->nId, p_stu->nType, p_stu->nTime, &p_stu->name );
}

//----------------------------------------------------------------------
// AddLimitCount - 添加/更新限制次数(线程安全)
//----------------------------------------------------------------------
void CEquipBack::AddLimitCount( BackLimitInfo* p_stu )
{
	if ( NULL == p_stu )
	{
		return;
	}

	Answer::MutexGuard lock( m_DepotLock );

	BackLimitInfoMap::iterator it = m_BackLimitInfoMap.find( p_stu->nId );
	if ( it != m_BackLimitInfoMap.end() )
	{
		it->second.nCount += p_stu->nCount;
		it->second.sLastName = p_stu->sLastName;
		it->second.nId = p_stu->nId;
	}
	else
	{
		m_BackLimitInfoMap[p_stu->nId] = *p_stu;
	}

	// 同步到DB
	it = m_BackLimitInfoMap.find( p_stu->nId );
	if ( it != m_BackLimitInfoMap.end() )
	{
		// TODO: DB_SERVICE.SaveEquipBackCount( p_stu->nId, 2, it->second.nCount, &it->second.sLastName );
	}
}

//----------------------------------------------------------------------
// ChangeDeptCount - 更改仓库库存(线程安全)
//----------------------------------------------------------------------
void CEquipBack::ChangeDeptCount( BackDepot stu )
{
	Answer::MutexGuard lock( m_DepotLock );

	BackDepotMap::iterator it = m_BackDepotMap.find( stu.nId );
	if ( it != m_BackDepotMap.end() )
	{
		it->second.Count += stu.Count;
		it->second.nId = stu.nId;
	}
	else
	{
		m_BackDepotMap[stu.nId] = stu;
	}

	// 同步到DB
	it = m_BackDepotMap.find( stu.nId );
	if ( it != m_BackDepotMap.end() )
	{
		// TODO: DB_SERVICE.SaveEquipBackCount( stu.nId, 1, it->second.Count, &std::string("") );
	}
}

//----------------------------------------------------------------------
// UpdateEquipRecord - 从数据库同步更新记录
//----------------------------------------------------------------------
void CEquipBack::UpdateEquipRecord( int32_t nId, int8_t nType, int32_t nTime, std::string* p_Name )
{
	if ( NULL == p_Name )
	{
		return;
	}

	Answer::MutexGuard lock( m_RecordLock );

	if ( m_BackRecord.size() > 50 )
	{
		m_BackRecord.pop_front();
	}

	BackRecord stu;
	stu.nId = nId;
	stu.nType = nType;
	stu.nTime = nTime;
	stu.name = *p_Name;
	m_BackRecord.push_back( stu );
}

//----------------------------------------------------------------------
// UpdateEquipBackInfo - 从数据库同步更新仓库/限制信息
//----------------------------------------------------------------------
void CEquipBack::UpdateEquipBackInfo( int32_t nId, int8_t nType, int32_t nCount, std::string* p_Name )
{
	if ( nType == 1 )
	{
		Answer::MutexGuard lock( m_DepotLock );

		BackDepotMap::iterator it = m_BackDepotMap.find( nId );
		if ( it != m_BackDepotMap.end() )
		{
			it->second.Count = nCount;
			it->second.nId = nId;
		}
		else
		{
			BackDepot stu;
			stu.nId = nId;
			stu.Count = nCount;
			m_BackDepotMap[nId] = stu;
		}
	}
	else if ( nType == 2 )
	{
		if ( NULL == p_Name )
		{
			return;
		}

		Answer::MutexGuard lock( m_DepotLock );

		BackLimitInfoMap::iterator it = m_BackLimitInfoMap.find( nId );
		if ( it != m_BackLimitInfoMap.end() )
		{
			it->second.nCount = nCount;
			it->second.sLastName = *p_Name;
			it->second.nId = nId;
		}
		else
		{
			BackLimitInfo stu;
			stu.nId = nId;
			stu.nCount = nCount;
			stu.sLastName = *p_Name;
			m_BackLimitInfoMap[nId] = stu;
		}
	}
}

//----------------------------------------------------------------------
// TranseCurrencyItem - 货币类型转物品ID
//----------------------------------------------------------------------
int32_t CEquipBack::TranseCurrencyItem( CURRENCY_TYPE id )
{
	switch ( id )
	{
	case CURRENCY_MONEY:
		return CURRENCY_MONEY_ID;
	case CURRENCY_GOLD:
		return CURRENCY_GOLD_ID;
	case CURRENCY_CASH:
		return CURRENCY_CASH_ID;
	case CURRENCY_VIGOUR:
		return CURRENCY_VIGOUR_ID;
	default:
		return 0;
	}
}
