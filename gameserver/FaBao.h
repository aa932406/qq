#ifndef __FA_BAO_H__
#define __FA_BAO_H__
#include "ExtSystemBase.h"

class CFaBao
	:public CExtSystemBase
{
public:
	CFaBao();
	~CFaBao();

	virtual void			OnLoadFromDB( const PlayerDBData& dbData );
	virtual void			OnSaveToDB( PlayerDBData& dbData );
	virtual void			OnCleanUp();
	virtual void			GetInterestsProtocol( ProcIdList& procList );
	virtual int32_t			DispatchNetDatas( ProcId_t nProcId, Answer::NetPacket *inPacket );

public:
	void					AddFaBaoRes( int8_t nType, int32_t AddValues );
	bool					DecFaBaoRes( int8_t nType, int32_t DecValues );
	int32_t					GetFaBaoRes( int8_t nType );
	void					AddPlayerAttr();
	int32_t					GetFaBaoCount( int32_t& FaBaoSumLevel );
	int32_t					GetBattleFaBaoInfo( int8_t& FaBaoLevel, int32_t& FaBaoBattle );
	void					PackFaBaoInfo( Answer::NetPacket *inPacket );
	int32_t					GetFaBaoLevel( int8_t Type );
	void					SendAllFaBaoInfo();
	bool					IsFaBaoType( int8_t Type );
	bool					IsFaBaoResType( int8_t Type );
	bool					CheckFaBaoType( FaBaoType Type );
	bool					CheckFaBaoResType( FaBaoResourceType Type );
private:
	int32_t					onBuyFaBaoRes( Answer::NetPacket* inPacket );
	int32_t					onAskFaBaoInfo( Answer::NetPacket *inPacket );
	int32_t					onFaBaoPeiYang( Answer::NetPacket *inPacket );
	void					sendFaBaoInfo( int8_t nType );
	void					broadcastLevelUp( int8_t Type, int32_t GongGaoId );
private:
	int8_t	m_FaBaoId[FA_BAO_TYPE_COUNT];
	int8_t	m_FaBaoDress[FA_BAO_TYPE_COUNT];
	int32_t m_UpFaBaoResource[FA_BAO_RES_COUNT];
};

#endif
