#ifndef __YUN_YING_HD_H__
#define __YUN_YING_HD_H__
#include "ExtSystemBase.h"
class CYunYingHD
	:public CExtSystemBase
{
public:
	CYunYingHD();
	~CYunYingHD();
	virtual void			OnLoadFromDB( const PlayerDBData& dbData );
	virtual void			OnSaveToDB( PlayerDBData& dbData );
	virtual void			GetInterestsProtocol( ProcIdList& procList );
	virtual int32_t			DispatchNetDatas( ProcId_t nProcId, Answer::NetPacket *inPacket );
	virtual void			OnCleanUp();
	virtual void			OnUpdate( int64_t curTick );
	virtual void			OnDaySwitch( int32_t nDiffDays );
private:
	int32_t					OnGetShouChongReward( Answer::NetPacket *inPacket );
	int32_t					OnGetBuyTeHuiItem( Answer::NetPacket *inPacket );
	int32_t					OnGetEveryDayChongZhiGift( Answer::NetPacket *inPacket );
	int32_t					OnGetThreePetGift( Answer::NetPacket *inPacket );
	int32_t					OnGetTotalChongZhi( Answer::NetPacket *inPacket );
	int32_t					OnGetMobilePhoneGift( Answer::NetPacket *inPacket );
	int32_t					OnGetZeroBuyPetGift( Answer::NetPacket *inPacket );
public:
	void					SendShouChongIcon();
	void					SendShouChongInfo();
	void					GetShouChongIconState( IconStateList& IconList );

	void					SendTeHuiInfo();
	void					SendTeHuiIcon();
	void					GetTeHuiIconState( IconStateList& IconList );

	void					SendEveryDayChongZhiInfo();
	void					SendEveryDayChongZhiIcon();
	void					GetEVeryDayChongZhiIconState( IconStateList& IconList );

	void					SendThreePetGiftIcon();
	void					GetThreePetGiftIconState( IconStateList& IconList );
	ShowIcon				GetThreePetGiftIconStu();

	void					SendTotalChongZhiIcon();
	void					SendTotalChongZhiInfo();
	void					GetTotalChongZhiIconState( IconStateList& IconList );
	ShowIcon				GetTotalChongZhiIconStu();
	void					AddTotalChongZhiCount( int32_t AddCount );
	int32_t					HaveTotalChongZhiRewardCount();
	bool					AllGetTotalChongZhiReward();
private:
	int8_t					GetShouChongState();
	ShowIcon				GetShouChongIconStu();

	ShowIcon				GetTeHuiIconStu();
	bool					IsHaveTeHuiGift();
	int32_t					getTeHuiLimitTime();
	void					checkTeHuiTime();

	void					SendEveryDayChongZhiInfo( int8_t nType );
	void					SendEveryDayChongZhiIcon( int8_t nType );
	ShowIcon				GetEveryDayChongZhiIconStu();
	bool					IsHaveEveryDayChongZhiGift();
	bool					CanShowEveryChongZhiIcon();

	void					GongGao( int32_t GongGaoId );

	void					SendZeroBuyPetIcon();
	void					SendMobilePhoneGiftIcon();
	void					SendAdultGiftIcon();

	int32_t					m_TotalChongZhiDay;
	int64_t					m_nLastTick;
	int8_t					m_nLastTeHui;
};

#endif