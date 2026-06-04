#ifndef __HUO_YUE_DU_H__
#define __HUO_YUE_DU_H__
#include "ExtSystemBase.h"
enum HuoYueDuType
{
	HYDT_ACTIVITY		= 1,	//�
	HYDT_DUNGEON		= 2,	//����
	HYDT_QI_FU_EXP		= 3,	//��������
	HYDT_QI_FU_MONEY    = 4,	//����ͭǮ
	HYDT_KILL_BOSS		= 5,	//��ɱboss 
	HYDT_DA_WEI_WANG	= 6,	//��θ��  
	HYDT_SIGN			= 7,	//ǩ��
	HYDT_ILLUSION		= 8,	//�û�      
	HYDT_KILL_MONSTER	= 9,	//��ɱָ���� 
	HYDT_COST_CASH		= 10,	//���Ѱ�ħʯ
	HYDT_COST_GOLD		= 11,	//����ħʯ
	HYDT_EQUIP_STAT_UP	= 12,	//װ��ǿ�� 
	HYDT_CYCLE_TASK		= 13,	//ѭ������
	HYDT_FAMILY_DONATE	= 14,	//���ž���
};

class CHuoYueDu
	:public CExtSystemBase
{
public:
	CHuoYueDu();
	~CHuoYueDu();

	virtual void			GetInterestsProtocol( ProcIdList& procList );
	virtual int32_t			DispatchNetDatas( ProcId_t nProcId, Answer::NetPacket *inPacket );
	virtual void			OnLoadFromDB( const PlayerDBData& dbData );
	virtual void			OnSaveToDB( PlayerDBData& dbData );
	virtual void			OnCleanUp();
	virtual void			OnDaySwitch( int32_t nDiffDays );

	void					AddHuoYueDuRecord( int8_t Type, int32_t Effect = 0, bool Complete = false );
	void					SendHuoYueDuInfo();
	int32_t					RewardCount();
	void					GetHuoYueDuIcon( IconStateList& IconList );
	void					SendHuoYueDuIcon();

private:
	int32_t					OnAskHuoYueDuInfo( Answer::NetPacket *inPacket );
	int32_t					OnGetHuoYueDuReward( Answer::NetPacket *inPacket );
	int32_t					OnSec( Answer::NetPacket *inPacket );

	int32_t					CalculateHuoYueDu();
	ShowIcon				GetHuoYueDuIconStu();
	void					AddKiaFuHuoYueDu( int32_t Value );

private:
	HuoYueDuRecordMap		m_HuoYueDuRecord;
};

#endif