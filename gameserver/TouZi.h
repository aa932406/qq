#ifndef __TOU_ZHI_H__
#define __TOU_ZHI_H__

#include "ExtSystemBase.h"

class CTouZi
	:public CExtSystemBase
{
public:
	CTouZi();
	~CTouZi();
	virtual void			OnLoadFromDB( const PlayerDBData& dbData );
	virtual void			OnSaveToDB( PlayerDBData& dbData );
	virtual void			GetInterestsProtocol( ProcIdList& procList );
	virtual int32_t			DispatchNetDatas( ProcId_t nProcId, Answer::NetPacket *inPacket );
	virtual void			OnCleanUp();
	virtual void			OnUpdate( int64_t curTick );
	virtual void			OnDaySwitch( int32_t nDiffDays );

	void					GetTouZiIconState( IconStateList& IconList );
public:
	void					SendTouZiIcon();
	ShowIcon				GetTouZiIconStu();

private:
	int32_t					OnGetTouZiReward( Answer::NetPacket *inPacket );
	int32_t					OnAskTouZiInfo( Answer::NetPacket *inPacket );
	int32_t					OnTouZi( Answer::NetPacket *inPacket );
	int32_t					GetSevenTouZiReward( int16_t nIndex );
	int32_t					GetMonthTouZiReward( int16_t nIndex );
	void					SendTouZiInfo();
	int32_t					GetSevenDayRewardCount();
	int32_t					GetMonthRewardCount();
	bool					IsAllGetSevenDayTouZi();
	bool					IsAllGetMonthTouZi();
	void					BroadcastTouZi( int32_t nGongGaoId );

private:
	int32_t					m_SevenDayTouZiTime;	// 7��Ͷ��ʱ���
	int32_t					m_SevenDayRecord;		// 7����ȡ��¼(λͼ)
	int32_t					m_MonthTouZiTime;		// �¶�Ͷ��ʱ���
	int32_t					m_MonthTouZiRecord;		// �¶���ȡ��¼(λͼ)
};

#endif // __TOU_ZHI_H__
