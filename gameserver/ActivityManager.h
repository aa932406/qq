//////////////////////////////////////////////////////////////////////////
// �������
//////////////////////////////////////////////////////////////////////////

#ifndef __TPOC_ACTIVITY_MANAGER_H__
#define __TPOC_ACTIVITY_MANAGER_H__

#include "Activity.h"


enum ActivityId
{
	JUN_TUAN_ZHAN_ID		= 1,		//����ս
	FEI_TIAN_SHENG_JV_ID    = 2,		//�������
	JUN_TUAN_ZHI_GUANG_ID	= 3,		//����֮��
	JUN_TUAN_ZHI_GUANG_ID2  = 4,		//����֮��2
	MO_LING_RU_QING_ID		= 5,		//ħ������
	MO_LING_RU_QING_ID2		= 6,		//ħ������2
	DA_TI_HUONG_DONG_ID		= 7,		//����
	DA_TI_HUONG_DONG_ID2	= 8,		//����2
	TIAN_JIANG_BAO_XIANG	= 9,		//�콵����
	CTERRITORYWAR_1			= 11,		//���ս
	CTERRITORYWAR_2			= 12,		//���ս
	CTERRITORYWAR_3			= 13,		//���ս
	CTERRITORYWAR_4			= 14,		//���ս
	CTERRITORYWAR_5			= 15,		//���ս
	SQIDER_QUEEN_ID_1		= 16,		//֩��Ů��1
	SQIDER_QUEEN_ID_2		= 17,		//֩��Ů��2
	SQIDER_QUEEN_ID_3		= 18,		//֩��Ů��3
	SQIDER_QUEEN_ID_4		= 19,		//֩��Ů��4
};

class CActivityManager
{
	typedef std::map<int32_t,CActivity*> ActivityMap;
public:
	CActivityManager();
	virtual ~CActivityManager();

public:
	void Init();
	void OnUpdate();

	void	SendActivityInfo( Player* player, int32_t nId );
	int32_t	GiveDailyReward( Player* player, int32_t nId );
	int32_t	HaveRewardCount( Player* Player, int32_t nId );
	void    NotifyActivityInfo( Player* player );

	// new v2 functions
	void	SendActivityRankInfo( Player* player, int32_t nId );
	void	UpdateActivityState( Player* player, int32_t nId, int8_t nState );
	int32_t	AddTianJiangBaoXiangCount();

public:
	bool		IsActivityRunning(int32_t id);
	bool		IsActivityRunningByType( int32_t nType );
	int32_t		GetCurActivityId( int32_t nType );
	void		AppendActivityState(Answer::NetPacket *packet);
	CActivity*	GetActivity( int32_t id );


	FamilyId_t	GetFamilyWarWinner();
	std::string GetFamilyWarLeaderName();
	std::string GetFamilyWarWinnerName();
	void		OnFamilyWarResult( int32_t nActId, FamilyId_t nFamilyId, int16_t nWinTimes , string FamilyName, string LeadyerName );
	void		OnTerritoryWarResult( int32_t nActId, std::string winners );
	void		OnCityWarResult( int32_t nActId, int32_t nIndex, FamilyId_t nFamilyId, int32_t nWinTime,
		CharId_t nLeader, CharId_t nFirst, CharId_t nSecond, CharId_t nThird,
		const std::string& strFirstFamily, const std::string& strSecondFamily, const std::string& strThirdFamily );
	void		GetActicityIconState( IconStateList& iconList );
	void		OnTerritoryWarInfo( Player* player );
	int8_t		GetTerritoryWarState();

	void		StopSqiderQueen();

	// City war winner management
	void		SetCityWarWinner(FamilyId_t nFamilyId);
	FamilyId_t	GetCityWarWinner();

private:
	Answer::Mutex	m_mutex;

	int32_t			m_nNowDay;
	ActivityMap		m_mActivity;

	FamilyId_t		m_nFamilyWarWinner;
	int32_t			m_nFamilyWarWinTimes;

	// new v2
	std::map<int32_t, FamilyId_t>	m_mWinFamily;		// city war winners by connid
};
#define ACTIVITY_MANAGER Answer::Singleton<CActivityManager>::instance()

#endif	//__TPOC_ACTIVITY_MANAGER_H__
