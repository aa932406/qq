#ifndef __FESTIVAL_ACTIVITY_H__
#define __FESTIVAL_ACTIVITY_H__

#include "Activity.h"
#include "CfgData.h"
#include "DataStructs.h"
#include "ItemHelper.h"

class Player;

// Exchange config struct
struct FestivalExchangeCfg
{
	ItemDataList	lCostItem;
	int32_t			nCostGold;
	MemChrBagVector	GetItems;
	int32_t			Times;
};

enum FESTIVAL_ACTIVITY_TYPE_2
{
	FAT2_COLLECTION		= 1,
	FAT2_DA_TI			= 2,
	FAT2_NPC			= 3,
	FAT2_COLLECTION2	= 4,
	FAT2_DUI_HUAN		= 5,
};

class CFestivalActivity : public CActivity
{
public:
	struct MapPos
	{
		int32_t		m_MapId;
		Position	m_Pos;
	};

public:
	CFestivalActivity( const CfgActivity& cfgActivity );
	virtual ~CFestivalActivity();

public:
	virtual void Init();

public:
	void	initCfgData();
	void	initOpenList( const std::string& str );

	void	OnDaySwitch();
	void	OnNewMinute( int32_t nMinute );

	bool	IsInTime( FESTIVAL_ACTIVITY_TYPE_2 nType );
	bool	IsInTime();
	bool	IsOpen( FESTIVAL_ACTIVITY_TYPE_2 nType );
	bool	isOpen( FESTIVAL_ACTIVITY_TYPE_2 nType );

	int32_t	getLeftTime();

	ShowIcon*	getIconState( ShowIcon* retstr, Player* player );
	void		GetIconState( Player* player, IconStateList* IconList );
	void		SendIconState( Player* player );
	bool		needShowIcon();
	void		hideIcon( int32_t nIconId );

	int32_t	GetActDay( FESTIVAL_ACTIVITY_TYPE_2 nType );
	int32_t	GetPlantTimes();
	int32_t	GetPlantTimes2();

	int32_t	DaTi( Player* player, std::string* p_DaAn );
	int32_t	GetDaTiReward( Player* player, int8_t RewardType );
	int32_t	DuiHuan( Player* player, int32_t nIndex );

protected:
	void	saveVersion();
	int32_t	loadVersion();

protected:
	int32_t		m_nDay;
	int32_t		m_nMinute;
	int32_t		m_nStartDay;
	int32_t		m_nEndDay;
	int32_t		m_nVersion;
	int32_t		m_nIcon;

	int32_t		m_vStartDay[6];
	int32_t		m_vEndDay[6];
	int8_t		m_vOpen[6];

	int32_t		m_PlantId;
	int32_t		m_PlantCount;
	int32_t		m_PlantTime;
	int32_t		m_Times;

	int32_t		m_NpcId;
	int32_t		m_NpcCount;

	int32_t						m_DuiHuanSize;
	MemChrBag					m_DayGift;
	MemChrBag					m_ActGift;

	std::vector<MapPos>							m_PlantMapPosVt;
	std::vector<MapPos>							m_NpcMapPosVt;
	std::vector<std::vector<std::string>>		m_StringVtVector;
	std::vector<FestivalExchangeCfg>			m_ExchangeCfgVt;
};

#endif // __FESTIVAL_ACTIVITY_H__
