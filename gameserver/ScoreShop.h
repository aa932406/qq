#ifndef __SCORE_SHOP_H__
#define __SCORE_SHOP_H__
#include "ExtSystemBase.h"
#include "CfgData.h"

class ScoreShop:
	public CExtSystemBase
{
public:
	ScoreShop();
	~ScoreShop();
	virtual void		OnLoadFromDB( const PlayerDBData& dbData );
	virtual void		OnSaveToDB( PlayerDBData& dbData );
	virtual void		OnDaySwitch( int32_t nDiffDays );
	virtual void		GetInterestsProtocol( ProcIdList& procList );
	virtual int32_t		DispatchNetDatas( ProcId_t nProcId, Answer::NetPacket *inPacket );

	void				GetIconState( IconStateList& iconList );
private:
	ItemLimitMap		m_ItemLimit;							// 限制次数
	int32_t				OnBuyItem( Answer::NetPacket *inPacket );
	void				SendLimitInfo( int32_t Index = 0 );
	int32_t				GetLimitCount( int32_t index );
	void				AddLimitCount( int32_t index, int32_t count );
	void				AddServerRecord( MemChrBag* Item );
};

#endif
