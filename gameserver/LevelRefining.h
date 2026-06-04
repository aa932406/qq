#pragma once

#include "ExtSystemBase.h"

class Player;

class CLevelRefining
	: public CExtSystemBase
{
public:
	CLevelRefining();
	virtual ~CLevelRefining();

public:
	virtual void	GetInterestsProtocol( ProcIdList& procList );
	virtual int32_t DispatchNetDatas( ProcId_t nProcId, Answer::NetPacket *inPacket );

public:
	void	GetIcon( IconStateList& IconList );
	void	SendIcon();
	void	SendRefinInfo();

private:
	int32_t	OnRefining( Answer::NetPacket* pData );
	int32_t	GetLeftTime();
};

