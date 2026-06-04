#ifndef __TIAN_JIANG_BAO_XIANG_H__
#define __TIAN_JIANG_BAO_XIANG_H__
#include "stdafx.h"
#include "Activity.h"

struct ActivityRewardRecord
{
	int32_t		OpenBaoXiangCount;
	int8_t		IsGetSpecial;
	int32_t		BuyCount;
};
typedef std::map<CharId_t,ActivityRewardRecord> PlayerMap;

class CTianJiangBaoXiang
	:public CActivity
{
public:
	CTianJiangBaoXiang( const CfgActivity& cfgActivity );
	~CTianJiangBaoXiang();

 virtual void		reset();
 virtual int32_t	canEnter( Player* player, CActivityMap* pTargetMap ) const;
 virtual void		SendPlayerActivityInfo( Player* player );
 virtual void		SendPlayerActivityScore( Player* player, int32_t nLeftTime );	virtual void		onPlantGather( Plant* pPlant, Player *player );
	virtual void		AddPlant( Plant* plant );
	virtual int32_t		onBeginGather(  Plant* plant, Player *player );
	virtual void		addPlayer( Player* player );
	int32_t			GiveDailyReward( Player* player );
	bool				AddOpenCount( Player* player, int32_t AddCount );
	void				broadcastReady();
	void				broadcastStart();
private:
	int8_t			GradeBaoXiangState;
	PlayerMap		m_PlayerSoreMap; 
};

#endif