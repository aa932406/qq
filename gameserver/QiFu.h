#ifndef __QI_FU_H__
#define __QI_FU_H__


#include "ExtSystemBase.h"
#include "CfgData.h"


class CQiFu:
	public CExtSystemBase
{
public:
	CQiFu();
	~CQiFu();
	virtual void	GetInterestsProtocol( ProcIdList& procList );
	virtual int32_t DispatchNetDatas( ProcId_t nProcId, Answer::NetPacket *inPacket );

	int32_t			OnQiFu( Answer::NetPacket *inPacket );
	void			SendQiFuInfo();

	void			GetChouJiangStu( IconStateList& IconList );
	ShowIcon		GetShowIconStu();
	void			SendHuoDongIcon();

private:
	bool			CheckQiFuType( int8_t Type );
	void			SendQiFuSuccess( int8_t Type, int32_t AddCount, int8_t Double = 1 );
	void			SendQIFuIcon();
public:
	void			GetQiFuStu( IconStateList& IconList );
};

#endif

