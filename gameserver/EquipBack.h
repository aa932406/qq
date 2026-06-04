#ifndef __EQUIP_BACK_H__
#define __EQUIP_BACK_H__

#include <string>
#include <list>
#include <map>
#include "CfgData.h"
#include "DataStructs.h"
#include "ExtSystemBase.h"

class Player;

// 回购记录
struct BackRecord
{
	BackRecord()
	{
		nId = 0;
		nType = 0;
		nTime = 0;
		name = "";
	}
	int32_t		nId;			// 配置ID
	int8_t		nType;			// 类型: 1=金币回收 2=回购
	int32_t		nTime;			// 时间
	std::string	name;			// 玩家名称
};

// 回购仓库
struct BackDepot
{
	int32_t		nId;			// 物品ID
	int32_t		Count;			// 库存数量
};

// 回购限制信息
struct BackLimitInfo
{
	BackLimitInfo()
	{
		nId = 0;
		nCount = 0;
		sLastName = "";
	}
	BackLimitInfo& operator=( const BackLimitInfo& other )
	{
		if ( this != &other )
		{
			nId = other.nId;
			nCount = other.nCount;
			sLastName = other.sLastName;
		}
		return *this;
	}
	int32_t		nId;			// 配置ID
	int32_t		nCount;			// 已使用次数
	std::string	sLastName;		// 最后操作玩家名
};

typedef std::list<BackRecord>		BackRecordList;
typedef std::map<int32_t, BackDepot>		BackDepotMap;
typedef std::map<int32_t, BackLimitInfo>	BackLimitInfoMap;

// 装备回购系统 - 服务器全局管理器
class CEquipBack
{
public:
	CEquipBack();
	~CEquipBack();

	void Init( int32_t line );

	// 协议处理
	int32_t OnGoldBack( Player* pPlayer, Answer::NetPacket* packet );
	int32_t OnBuyBack( Player* pPlayer, Answer::NetPacket* packet );

	// 开服活动回收
	void KiaFuEquipBack( int32_t Index, int32_t EquipId, int32_t Mid, Player* pPlayer );

	// 发送信息
	void SendEquipBackInfo( Player* pPlayer );
	void SendEquipBackOnRecord( Player* pPlayer, BackRecord* p_stu );
	void SendEquipBackCountChange( Player* pPlayer, int32_t nId );
	void SendEquipBackLimitChange( Player* pPlayer, int32_t nId );

	// 数据库操作
	void AddRecord( BackRecord* p_stu );
	void AddLimitCount( BackLimitInfo* p_stu );
	void ChangeDeptCount( BackDepot stu );

	// 数据库同步更新
	void UpdateEquipRecord( int32_t nId, int8_t nType, int32_t nTime, std::string* p_Name );
	void UpdateEquipBackInfo( int32_t nId, int8_t nType, int32_t nCount, std::string* p_Name );

private:
	// 辅助函数
	int32_t TranseCurrencyItem( CURRENCY_TYPE id );

	Answer::Mutex	m_RecordLock;
	BackRecordList	m_BackRecord;			// 回购记录(最多50条)

	Answer::Mutex	m_DepotLock;
	BackDepotMap	m_BackDepotMap;			// 回购仓库

	Answer::Mutex	m_LimitLock;
	BackLimitInfoMap	m_BackLimitInfoMap;	// 限制次数信息
};

#endif
