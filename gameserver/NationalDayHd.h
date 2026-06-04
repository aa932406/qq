#ifndef __NATIONAL_DAY_HD_H__
#define __NATIONAL_DAY_HD_H__

#include "ExtSystemBase.h"
#include "CfgData.h"
#include "DataStructs.h"

class Player;

enum NATIONAL_HD_TYPE
{
	HHT_LOGIN		= 1,
	HHT_RECHARGE	= 2,
	HHT_COST_GOLD	= 3,
	HHT_KILL_BOSS	= 4,
	HHT_CBT			= 5,
	HHT_CHOU_JIANG	= 6,
	HHT_LUN_HUI_TA	= 7,
	HHT_YJSK		= 8,
};

struct Param2
{
	int32_t nParam1;
	int32_t nParam2;

	Param2(int32_t p1 = 0, int32_t p2 = 0)
		: nParam1(p1), nParam2(p2)
	{
	}
};

class CNationalDayHd : public CExtSystemBase
{
public:
	CNationalDayHd();
	virtual ~CNationalDayHd();

public:
	virtual void OnCleanUp();
	virtual void Init(Player* pPlayer);

public:
	void	OnLoadFromDB(const PlayerDBData* dbData);
	void	OnSaveToDB(PlayerDBData* dbData);

	void	paraseNational(std::string* p_NationalInfo);
	std::string	SaveNationalInfo();

	bool	GetNationalExp(int32_t nType, int32_t nId);
	bool	UnLockHighReward();
	bool	GetSeniorReward(int32_t nLevel);
	bool	GetOrdinaryReward(int32_t nLevel);
	bool	BuyLevel(int32_t BuyLv);

	void	AddNationalValue(NATIONAL_HD_TYPE nType, int32_t ParamValue1, int32_t ParamValue2);
	void	AddExp(int32_t AddExp);
	void	CheckLevelUp();
	void	SendLevelInfo();
	void	PackHdInfo(Answer::NetPacket* packet);

	int32_t	CanGetCount();

public:
	std::map<int32_t, Param2>	m_NationalInfo;
	int32_t						m_Level;
	int32_t						m_Exp;
	int32_t						m_UnLockReward;
	int32_t						m_OrdinaryReward;
	int32_t						m_SeniorReward;
};

#endif // __NATIONAL_DAY_HD_H__
