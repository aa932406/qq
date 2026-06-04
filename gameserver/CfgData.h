#pragma once
//////////////////////////////////////////////////////////////////////////
//author:zxj		modify time:2012-7-7
//description:��Ϸ�нṹ�Ľṹ����
//////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include "Shared.h"
#include "actStructs.h"

struct AddAttribute
{
	AddAttribute()
		: m_nAddAttrType( 0 )
		, m_nAddAttrValue( 0 )
	{
	}
	uint8_t	m_nAddAttrType;
	int32_t	m_nAddAttrValue;
};
typedef std::list<AddAttribute> AddAttrList;

class ChrMount;

//����ݽṹ
struct CfgActivity 
{
	int32_t id;							// id��ʶ��
	int16_t	typeId;						// ���
	Int32Vector weekday;				// ���ʱ�䷶Χ(1234567);s
	int32_t begin_date;					// ���ʼʱ��
	int32_t end_date;					// �����ʱ��
	int32_t line;						// ����
	Int32Vector start_hour;				// ���ʱ��
	int32_t duration;					// ��ܳ���
	int32_t level;

	Int32Vector maps;					// ������ͼ
	int32_t target_mapid;				// ��ǰ�ĵ�ͼID
	Int32Vector target_regiona;			// ������1
	Int32Vector target_regionb;			// ������2
	int32_t state;						// ���״̬����
	Int32Vector gift_id;				// ��
	Int32Vector daily_reward;			// �ճ�����
	int32_t exp_add_interval;
	int32_t	iconid;						// �ͼ��ID
};
//��б�
typedef std::map<int32_t, CfgActivity> CfgActivityTable;

//�����
typedef std::list<Position> PosList;
struct CfgActivityMonster 
{
	int32_t id;
	int32_t wave;//���ﲨ��
	int32_t mid;
	int32_t x;
	int32_t y;
	int32_t count;
	int32_t side;
	PosList road;
	int32_t	delay;	// ˢ�ּ��
	int32_t	times;	// ˢ�ִ���
};
//���л�еĹ����
typedef std::map<int32_t, CfgActivityMonster> CfgActivityMonsterTable;

//�Npc
struct CfgActivityNpc 
{
	int32_t id;							//ID��ʶ��
	int32_t activity_id;				//�ID
	int32_t npc_id;						//ncp ID
	int32_t count;						//NPC����
	Int32Vector region_id;				//����IDs
};
typedef std::list<CfgActivityNpc> CfgActivityNpcList;
typedef std::map<int32_t, CfgActivityNpc> CfgActivityNpcTable;


//�ֲ��
struct CfgActivityPlant 
{
	int32_t id;							//ID��ʶ��
	int32_t activity_id;				//�ID
	int32_t plant_id;					//ֲ��ID
	int32_t count;						//ֲ�����
	int32_t wave;						//����
	int32_t region_id;					//����ID
	int32_t whoplant;					//�ɼ���
	std::vector<Position>	EnterPosVector;
};

typedef std::list<CfgActivityPlant> CfgActivityPlantList;

typedef std::map<int32_t, CfgActivityPlant> CfgActivityPlantTable;

//�������
struct CfgActivityTrap 
{
	int32_t id;
	int32_t tid;
	int32_t x;
	int32_t y;
};
typedef std::map<int32_t, CfgActivityTrap> CfgActivityTrapTable;

struct CfgActivityDrop
{
	int32_t id;
	int32_t activity_id;
	int32_t monster_min_level;
	int32_t drop_group_id;
	int32_t probability;
};
typedef std::map<int32_t, CfgActivityDrop> CfgActivityDropTable;

// ��Ӫս���뽱��
struct CfgFamilyWarJoinReward 
{
	int32_t level;
	int32_t addExp;
};

typedef std::list<CfgFamilyWarJoinReward> CfgFamilyWarJoinRewardList;
struct CfgFamilyWarJoinRewardTable 
{
	void Add( int32_t nLevel, int32_t nExp )
	{
		CfgFamilyWarJoinReward reward;
		reward.level	= nLevel;
		reward.addExp	= nExp;
		m_reward.push_back( reward );
	}

	int32_t GetReward( int32_t nLevel ) const
	{
		CfgFamilyWarJoinRewardList::const_iterator iter = m_reward.begin();
		CfgFamilyWarJoinRewardList::const_iterator eiter = m_reward.end();
		for ( ; iter != eiter; ++iter )
		{
			if ( nLevel <= iter->level )
			{
				return iter->addExp;
			}
		}
		return 0;
	}
	CfgFamilyWarJoinRewardList m_reward;
};

class CfgFamilyLightExpTable 
{
public:
	void Add( int32_t nLevel, int32_t nAddExp )
	{
		vLevel.push_back( nLevel );
		vAddExp.push_back( nAddExp );
	}

	int32_t GetExp( int32_t nLevel ) const
	{
		int32_t idx = -1;
		int32_t isize = vLevel.size();
		for ( int32_t i = 0; i < isize; ++i )
		{
			if ( nLevel >= vLevel[i] )
			{
				idx = i;
			}
			else
			{
				break;
			}
		}
		if ( idx < 0 || idx >= isize )
		{
			return 0;
		}
		return vAddExp[idx];
	}

private:
	Int32Vector vLevel;
	Int32Vector vAddExp;
};

// ������Խ���
struct CfgHorseRacingReward 
{
	int32_t nIndex;
	int32_t nMaxIndex;
	int32_t	nExp;			// ����
	int32_t	nMoney;			// ��Ǯ
	MemChrBagVector			Items;
};
typedef std::list<CfgHorseRacingReward> CfgHorseRacingRewardList;
class CfgHorseRacingRewardTable
{
public:
	CfgHorseRacingRewardTable(){}
	~CfgHorseRacingRewardTable(){}

	void Add( const CfgHorseRacingReward& reward )
	{
		m_rewards.push_back( reward );
	}

	const CfgHorseRacingReward* GetReward( int32_t nIndex ) const
	{
		CfgHorseRacingRewardList::const_iterator iter = m_rewards.begin();
		CfgHorseRacingRewardList::const_iterator eiter = m_rewards.end();
		for ( ; iter != eiter; ++iter )
		{
			if ( nIndex >= iter->nIndex && nIndex <= iter->nMaxIndex )
			{
				return &(*iter);
			}
		}
		return NULL;
	}

private:
	CfgHorseRacingRewardList m_rewards;
};


///////////////////////////////////////////////////////////////////////////////
struct CfgAnnoucementTime
{
	int32_t id;
	Int32Vector weekday;
	int32_t begin_date;
	int32_t end_date;
	int32_t hour;
	int32_t minute;
	int32_t annoucement_id;
};
typedef std::vector<CfgAnnoucementTime> CfgAnnoucementTimeTable;

struct BuffAttr //���ܹ������ԡ�����С֮���
{
	int32_t getRatio(int32_t level)
	{
		return roundInt(ratio_base + ratio_ratio*level*0.1);
	}

	int32_t getAddon(int32_t level)
	{
		return roundInt(addon_base + addon_ratio*level);
	}

	int32_t attr;
	int32_t ratio_base;
	int32_t ratio_ratio;
	int32_t addon_base;
	int32_t addon_ratio;
};

struct CfgBuff //����Ч��
{
	int32_t id;
	int32_t beneficial;
	int32_t special;
	int32_t duration;//����
	int32_t interval;//���
	BuffAttr buffAttr[2];
	int8_t	isShow;	//�Ƿ�Ҫ�����ͻ�����ʾ
};
typedef std::map<int32_t, CfgBuff> CfgBuffTable;

struct CfgDungeon 
{
	int32_t id;				// ����ID
	std::string name;		// ��������
	int32_t mapid;			// ��ͼID
	int16_t x;				// �����X
	int16_t y;				// �����Y
	int8_t	type;			// ���� DungeonType
	int32_t group_id;		// �����飨�������������
	int32_t duration;		// ����ʱ��
	int16_t level;			// ����ȼ�
	int16_t	maxLevel;		// �ȼ�����
	int8_t	vip;			// ����VIP�ȼ�
	int32_t last_id;		// ǰһ������ID
	int32_t next_id;		// ��һ������ID
	int8_t	player_num;		// ��������
	int8_t	weekday;		// ÿ�ܿ�����
	int8_t	start_hour;		// ÿ�տ�ʼʱ��
	int8_t	end_hour;		// ÿ�ս���ʱ��
	int8_t	daily_count;	// ÿ�մ�������
	int32_t player_buff;	// ����BUFF
	int32_t star;			// ����
	int32_t	double_cost;	// ˫������
	int32_t	reward_time;	// ��ȡ����ʱ��(��)
	ItemData costItem;		// ��������
	MemChrBag rewardOnce;	// ��ͨ����
	MemChrBag rewardItem;	// ͨ�ؽ���
	int32_t	  Battle;		// ս��������֮սͨ�ؼӵ�ս����
	int32_t  costGold;		// ���ĵ�Ԫ��
	int32_t  costMoney;		// ���ĵ�ͭǮ
};
typedef std::vector<CfgDungeon> CfgDungeonVector;
typedef std::map<int32_t, CfgDungeon> CfgDungeonTable;

struct CfgDungeonReward
{
	int32_t dungeonID;
	int32_t exp;
	int32_t money;
	int32_t cash;
	int32_t groupID;
};
typedef std::vector<CfgDungeonReward> CfgDungeonRewardVector;
typedef std::map<int32_t, CfgDungeonReward> CfgDungeonRewardTable;

enum DUNGEON_DROP_TYPE
{
	DDT_ITEM	= 0,
	DDT_TRAP	= 1,
};
struct CfgDungeonDrop
{
	int32_t type;
	int32_t item_id;
	int8_t	item_class;
	int8_t	bind_type;
	int32_t count;
	int32_t weight;
	int32_t probability;
};
typedef std::list<CfgDungeonDrop> CfgDungeonDropList;

class CfgDungeonDropGroup 
{
public:
	CfgDungeonDropGroup(){ CleanUp(); }
	~CfgDungeonDropGroup(){}

	void CleanUp()
	{
		m_nMaxWight = 0;
		m_lstDropItems.clear();
	}

	void Add( CfgDungeonDrop& drop )
	{
		m_nMaxWight += drop.weight;
		drop.weight = m_nMaxWight;
		m_lstDropItems.push_back( drop );
	}

	const CfgDungeonDrop* RandDrop() const
	{
		int32_t nRand = RANDOM.generate( 0, m_nMaxWight );
		for ( CfgDungeonDropList::const_iterator iter = m_lstDropItems.begin(); iter != m_lstDropItems.end(); ++iter )
		{
			if ( nRand < iter->weight )
			{
				int32_t nRand2 = RANDOM.generate( 0, 1000 );
				if ( nRand2 < iter->probability )
				{
					return &(*iter);
				}
			}
		}
		return NULL;
	}

private:
	int32_t				m_nMaxWight;
	CfgDungeonDropList	m_lstDropItems;
};
typedef std::map<int32_t, CfgDungeonDropGroup> CfgDungeonDropGroupTable;

struct CfgDungeonMonster 
{
	int32_t id;
	int32_t wave;//���ﲨ��
	int32_t mid;
	int32_t x;
	int32_t y;
	int32_t count;
	int32_t side;
	PosList road;
	int32_t	delay;	// ˢ�ּ��
	int32_t	times;	// ˢ�ִ���
	int32_t	money;	// ������Ǯ
};
typedef std::map<int32_t, CfgDungeonMonster> CfgDungeonMonsterTable;

struct CfgDungeonPlant 
{
	int32_t id;
	int32_t pid;
	int32_t x;
	int32_t y;
};
typedef std::map<int32_t, CfgDungeonPlant> CfgDungeonPlantTable;

struct CfgDungeonTrap 
{
	int32_t id;
	int32_t tid;
	int32_t x;
	int32_t y;
};
typedef std::map<int32_t, CfgDungeonTrap> CfgDungeonTrapTable;

struct CfgSignReward
{
	int32_t count;
    MemChrBagVector ItemVector;
};
typedef std::map<int8_t,CfgSignReward>CfgSignRewardTable;

struct CfgAppendAttr
{
	int32_t			Id;
	int8_t			Job;
	AddAttrList		AttrList;
};
typedef std::list<CfgAppendAttr> AppendAttrTable;

struct CfgOnlineReward
{
	int8_t				Id;
	int32_t				NeedTime;
	MemChrBagVector		ItemVector;
};
typedef std::map<int8_t, CfgOnlineReward> CfgOnlineRewardTable;

struct CfgOffLineExp
{
	int32_t				Level;			//�ȼ�
	int32_t				MinuteExp;		//����ÿ���ӻ�Ľ���
	int32_t				TwoTimes;		//˫����ȡһ����λ����ĵ���
	int32_t				ThreeTimes;		//3����ȡһ����λ����ĵ���
	int32_t				BaseExp;		//һ����λ�ľ���
};
typedef std::map<int32_t,CfgOffLineExp> CfgOffLineExpTable;

struct CfgEquipItem
{
	Job_t  job;
	int32_t  id;
	int32_t  type;
	int32_t  bind;
	int32_t  count;
	int8_t	 star;
};
typedef std::vector<CfgEquipItem> MemChrEquipBagVector;

struct CfgSevenLoginRewrad
{
	int8_t					Day;
	MemChrEquipBagVector	ItemVector;
};
typedef std::map<int8_t, CfgSevenLoginRewrad> CfgSevenLoginRewradTable;

struct CfgGongGao
{
	int8_t		Job;
	int32_t		GongGaoId;
};
typedef std::list<CfgGongGao> GongGaoList;
struct NewServerFavorable
{
	int8_t					Index;
	int32_t					NeedGold;
	MemChrEquipBagVector	ItemVector;
	GongGaoList				GongGaoInfo;
};
typedef std::map<int8_t,NewServerFavorable> NewServerFavorableMap;


struct CfgWeekOnlineReward
{
	int32_t		Week;
	int32_t		Cash;
	int32_t		UpperLimit;
};
typedef std::map<int32_t,CfgWeekOnlineReward> CfgWeekOnlineRewardTable;

struct CfgLevelGift
{
	int8_t				Index;
	int32_t				Level;
	MemChrBagVector		ItemVector;
};
typedef std::map<int32_t,CfgLevelGift> CfgLevelGiftTable;

struct CfgItemBase 
{
	int32_t id;
	std::string name;
	std::string desc;
	int32_t type;
	int32_t level;
	int32_t grade;
	Job_t job;
	int32_t in_value;
	int32_t out_value;
	int32_t bind;
	int32_t combine;
	int32_t quality;
	std::string url;
	std::string drop_url;
};

struct CfgItem : public CfgItemBase
{
	std::string effect;//����Ч��
	std::string use_method;
	int32_t downgrade;
	int32_t group_id;
	int32_t cd_group;
	int32_t overlay;
	int32_t can_sell;
	int32_t broadcast;
	int32_t valid_time;
	int32_t item_Grade;
};
typedef std::map<int32_t, CfgItem*> CfgItemTable;

struct CfgItemGift 
{
	int32_t id;
	int32_t item;
	int32_t type;
	int32_t count;
	int8_t  bind;
	int32_t job;
};
typedef std::vector<CfgItemGift> CfgItemGiftVector;
typedef std::map<int32_t, CfgItemGiftVector*> CfgItemGiftTable;

struct CfgFamilyLevel
{
	int32_t level;
	Int32Vector rewards;
};
typedef std::map<int32_t,CfgFamilyLevel>CfgFamilyLevelTable;

struct CfgItemGiftRandom 
{
	int32_t id;
	int32_t item;
	int32_t type;
	int32_t count;
	int8_t	bind;
	int32_t static_probability;
	int32_t sum_probability;
	int8_t	job;
};
typedef std::vector<CfgItemGiftRandom> CfgItemGiftRandomVector;
typedef std::map<int32_t, CfgItemGiftRandomVector*> CfgItemGiftRandomTable;

struct CfgJob 
{
	int32_t id;
	int32_t job_task;
	int32_t attack_attr;
	int32_t base_skill;
};
typedef std::map<int32_t, CfgJob> CfgJobTable;

struct CfgMovie
{
	int32_t id;
};
typedef std::map<int32_t,CfgMovie> CfgMovieTable;

struct CfgLevelExp 
{
	int32_t level;
	int64_t upgrade_exp;
	int64_t max_exp;
	int32_t pet_exp;
	int32_t mount_exp;
	int32_t vicegeneral_id;
};
typedef std::map<int32_t, CfgLevelExp> CfgLevelExpTable;

//��ͼ���ݽṹ
struct CfgMap 
{
	int32_t id;									//ID
	std::string name;							//����
	int32_t type;								//���
	int32_t param;								//�Id����
	int32_t width;								//��
	int32_t height;								//��
	int32_t revive;								//����
	int32_t pk_mode;							//pkģʽ
	int32_t anti_protect;						//����ģʽ
	int32_t isMount;							//�Ƿ�����
	int32_t isFly;								//�Ƿ���Է�
	int32_t isVicegeneral;						//�Ƿ�ั��
	int32_t player_level;						//��ҵȼ�
	int32_t player_level_max;					//�����ߵȼ�
	int32_t team_member;						//�Ŷӳ�Ա��
	int32_t hide_mini;							//����С��ͼ
	int32_t runnerId;							//������ͼ�߳�
	int8_t	jump;

	bool isType( const MapType& mt ) const
	{
		return (type & mt) != 0;
	}
};
typedef std::map<int32_t, CfgMap> CfgMapTable;

//�����ڵ�ͼλ�����ݽṹ
struct CfgMapMonster 
{
	int32_t id;							//����Ψһid
	int32_t mapid;						//��ͼID		
	int32_t monsterid;					//����ID
	int32_t x;							//xλ��
	int32_t y;							//yλ��
};
typedef std::map<int32_t, CfgMapMonster> CfgMonsterMap;
//һ�ŵ�ͼ�Ϲ����б�
typedef std::vector<CfgMapMonster> CfgMapMonsterVector;
//���е�ͼ�Ϲ����б�
typedef std::map<int32_t, CfgMapMonsterVector> CfgMapMonsterTable;

struct CfgMapPlant 
{
	int32_t mapid;
	int32_t plantid;
	int32_t x;
	int32_t y;
};
typedef std::vector<CfgMapPlant> CfgMapPlantVector;
typedef std::map<int32_t, CfgMapPlantVector> CfgMapPlantTable;

struct CfgMapRect 
{
	int32_t id;
	int32_t from_map;
	int32_t from_x;
	int32_t from_y;
	int32_t to_map;
	int32_t to_x;
	int32_t to_y;
};
typedef std::vector<CfgMapRect> CfgMapRectVector;
typedef std::map<int32_t, CfgMapRectVector> CfgMapRectTable;

//��ͼ��Χ
struct CfgMapRegion 
{
	int32_t id;
	int32_t mapid;
	int32_t min_x;
	int32_t min_y;
	int32_t max_x;
	int32_t max_y;
	int32_t type;
};
typedef std::vector<CfgMapRegion> CfgMapRegionVector;
typedef std::map<int32_t, CfgMapRegionVector> Int32CfgMapRegionVectorMap;
typedef std::map<int32_t, CfgMapRegion> CfgMapRegionTable;

#define MAX_MONSTER_SKILL	10
struct CfgMonster // ����
{
	int32_t mid;
	std::string name;
	int32_t level;
	int32_t quality;

	int32_t hp;						// ���Ѫ��
	int32_t phy_atk_min;			// ��С��������
	int32_t phy_atk_max;			// �����������
	int32_t mag_atk_min;			// ��Сħ������
	int32_t mag_atk_max;			// ���ħ������
	int32_t phy_def;				// ��������
	int32_t mag_def;				// ħ������
	int32_t hitrate;				// ����
	int32_t dodge;					// ����
	int32_t critrate;				// ����
	int32_t dmg_add;				// �����˺�
	int32_t dmg_rdc;				// �����˺�
	int32_t dmg_add_pec;			// �˺��ӳɣ�%��
	int32_t dmg_rdc_pec;			// �˺����⣨%��
	int32_t movespeed;				// �ƶ��ٶ�
	int32_t	battle;					// ����ֵ

	int32_t exp;
	int32_t type;//����0.���� 1.����
	int32_t kingdom_contribute;
	int32_t revive_time;
	int32_t skill_id;
	MonsterSkill unique_skill[MAX_MONSTER_SKILL];
	MonsterSkill random_skill[MAX_MONSTER_SKILL];
	int32_t angry_time;
	int32_t standby;//����
	int32_t hpPercent;//��λʱѪ������ǧ�ֱ�
	int32_t ai_style;
	int32_t ai_target;
	int32_t has_kingdom;
	int32_t view_range;
	int32_t move_range;
	int32_t broadcast;
	int32_t drop_free;
	int32_t corpse_time;
	int32_t	group_id;
	int8_t	boss_sign;
	int32_t	BossSocre;
	int8_t  TaskShare;
};
typedef std::map<int32_t, CfgMonster> CfgMonsterTable;

struct CfgMonsterDropGroup 
{
	int32_t group_id;
	int32_t item_id;
	int8_t	item_class;
	int8_t	bind_type;
	int32_t probability;
	int32_t cost_type;
	int32_t cost_value;
};
typedef std::vector<CfgMonsterDropGroup> CfgMonsterDropGroupVector;
typedef std::map<int32_t, CfgMonsterDropGroupVector> CfgMonsterDropGroupTable;

struct CfgMonsterGroupDrop 
{
	int32_t mid;
	int32_t group_id;
	int32_t probability;
	int32_t begin_time;
	int32_t end_time;
	int32_t repeat;
	int8_t  job;
};
typedef std::vector<CfgMonsterGroupDrop> CfgMonsterGroupDropVector;
typedef std::map<int32_t, CfgMonsterGroupDropVector> CfgMonsterGroupDropTable;

struct CfgMonsterTaskDrop 
{
	int32_t mid;
	int32_t tid;
	int32_t item;
	int32_t probability;
};
typedef std::vector<CfgMonsterTaskDrop> CfgMonsterTaskDropVector;
typedef std::map<int32_t, CfgMonsterTaskDropVector> CfgMonsterTaskDropTable;

struct CfgBornAttr
{
	Job_t job;
	AttrAddonVector bornAttr;
};
typedef std::vector<CfgBornAttr> CfgBornAttrVector;
typedef std::map<Job_t, CfgBornAttr> CfgBornAttrTable;

struct CfgChangeJobAttr
{
	int32_t index;
	AttrAddonVector changeAttr;
};
typedef std::vector<CfgChangeJobAttr> CfgChangeJobAttrVector;
typedef std::map<int32_t, CfgChangeJobAttr> CfgChangeJobAttrTable;

struct CfgNpc 
{
	bool findMapId(int32_t mapid) const
	{
		return std::find(mapids.begin(), mapids.end(), mapid) != mapids.end();
	}

	int32_t id;
	int32_t level;
	Int32Vector mapids;
	int32_t x;
	int32_t y;
	int32_t func;
	int32_t func_extra;
	Int32Vector params;
};
typedef std::map<int32_t, CfgNpc> CfgNpcTable;

struct CfgNpcAirport
{
	int32_t id;
	int32_t npcid;
	int32_t player_kingdom;
	int32_t kingdom_id;
	int32_t map_id;
	int32_t map_x;
	int32_t map_y;
	int32_t cost;
};
typedef std::map<int32_t, CfgNpcAirport> CfgNpcAirportTable;

struct CfgGamble//ϡ������
{
	  int32_t id;
	  int32_t ratio;
	  int32_t item;
	  int32_t item_type;
	  int32_t count;
	  int32_t isBroadcast;
};
typedef std::vector<CfgGamble>CfgGambleVector;

struct CfgGambleEquip
{
	int32_t id;
	int32_t type;
	MemJobItemTable equip;
};
typedef std::map<int32_t,CfgGambleEquip>CfgGambleEquipTable;

struct CfgFriendExp
{
	int32_t Level;
	int32_t Exp;
};
typedef std::map<int32_t,CfgFriendExp> CfgFriendExpTable;
struct CfgChrShop 
{
	int32_t		Index;
	int32_t		ItemId;
	int8_t		ItemClass;
	int32_t		IsBind;
	int32_t		LimitCount;
	int32_t		Price;
};
typedef std::map<int32_t, CfgChrShop> CfgChrShopTable;


struct CfgNpcKingdomShop
{
	int32_t id;
	int32_t npcid;
	int32_t item_id;
	int32_t item_type;
	int32_t cost;
	int32_t level_required;
	int32_t weight;
};
typedef std::map<int32_t, CfgNpcKingdomShop> CfgNpcKingdomShopTable;

struct CfgNpcTrade //ó��
{
	int32_t id;
	int32_t npcid;
	int32_t item_id;
	int32_t item_type;
	int32_t item_cost;
	int32_t cost_value;
	int32_t weight;
};
typedef std::map<int32_t, CfgNpcTrade> CfgNpcTradeTable;

struct CfgPlantEvent 
{
	int32_t EventId;
	int32_t Probability;
};
typedef std::vector<CfgPlantEvent> CfgPlantEventVector;

struct CfgPlant
{
	int32_t id;
	int32_t type;
	int32_t level;
	CfgPlantEventVector Events;
	int32_t EventMaxRate;
	int32_t item_cost;
	int32_t start_hour;
	int32_t end_hour;
	int32_t gather_time;
	int32_t revive_time;
};
typedef std::map<int32_t, CfgPlant> CfgPlantTable;

struct CfgPlantEventEffect
{
	int32_t		EventId;
	int32_t		EventType;
	string		EventEffect;
	int32_t		GongGaoId;
};
typedef std::map<int32_t,CfgPlantEventEffect> PlantEventMap;


struct CfgLevelAttr
{
	int32_t job;
	int32_t level;
	AttrAddonVector addonattr;
};
typedef std::map<int32_t, CfgLevelAttr> CfgLevelAttrTable;

struct CfgPractice
{
	int32_t dungeon_id;
	int32_t first_award_exp;
	int32_t next_award_exp;
	Int32Vector award_item;
};
typedef std::map<int32_t, CfgPractice> CfgPracticeTable;


struct CfgSkill 
{
	int32_t getMoney(int32_t level)
	{
		return roundInt(money_base + money_ratio*(level*level+level)*0.1);
	}

	int32_t getMpCost(int32_t level)
	{
		 return roundInt(mp_cost_base + static_cast<int32_t>(mp_cost_ratio*(level+level*(level-20)/40)));
	}

	int32_t getJonggongCost(int32_t level)
	{
		return jungong_base +jungong_ratio*level*level;
	}

	int32_t getAttackModify(int32_t level)
	{
		return roundInt(attack_modify_base + attack_modify_ratio*(level-1));
	}

	int32_t getAttackAddon(int32_t level)
	{
		 return roundInt(attack_addon_base + static_cast<int32_t>(attack_addon_ratio*(level+level*(level-10)/20)));
	}

	int32_t getBuffRate(int32_t level)
	{
		return roundInt(buff_rate_base + buff_rate_ratio*level*0.1);
	}

	//��Ҫ���Ʋ��������ļ���
	int32_t getLevelLimit(int32_t level)
	{
		return level + level_base;
	}

	SkillId_t id;
	Job_t job;
	int32_t type;					// SKILL_TYPE
	int32_t	kind;					// SKILL_KIND
	int32_t distance;
	int32_t range;					// SKILL_RANGE
	int32_t area;					// RangeArea
	int32_t	self;					// SKILL_TARGET_POS
	int32_t target_num;
	int32_t beneficial;				// ������
	int32_t sp;						//�����ٶ�
	int32_t study_level;
	int32_t money_base;
	int32_t money_ratio;
	int32_t jungong_base;
	int32_t jungong_ratio;
	int32_t cd;
	int32_t mp_cost_base;
	int32_t mp_cost_ratio;
	int32_t angry_cost;
	int32_t special;				// SkillSpecial
	int32_t attack_type;			// SKILL_ATTACK_TYPE
	int32_t attack_modify_base;		// ������������ֵ
	int32_t attack_modify_ratio;	// ��������ϵ��
	int32_t attack_addon_base;		// �����ӳɻ���ֵ
	int32_t attack_addon_ratio;		// �����ӳ�ϵ��
	int32_t buff_rate_base;			// buff���ʻ���ֵ
	int32_t buff_rate_ratio;		// buff����ϵ��
	int32_t buff;
	int32_t level_base;
	int32_t maxLevel;
	ChangeJobIndexVector change_skill_id;
};
typedef std::map<int32_t, CfgSkill> CfgSkillTable;

#define MAX_SKILL_LEVEL		5
struct CfgSkillLevelUp 
{
	SkillId_t	nSkillId;
	int32_t		nBookId[MAX_SKILL_LEVEL];
};
typedef std::map<SkillId_t, CfgSkillLevelUp> CfgSkillLevelUpMap;

class CfgSkillLevelUpTable
{
public:
	CfgSkillLevelUpTable(){}
	~CfgSkillLevelUpTable(){}

	bool Add( const CfgSkillLevelUp& skill )
	{
		m_mSkillLevelUpMap[skill.nSkillId] = skill;
		return true;
	}

	int32_t GetBook( int32_t nSkillId, int32_t nLevel ) const
	{
		if ( nLevel < 0 || nLevel >= MAX_SKILL_LEVEL )
		{
			return 0;
		}
		CfgSkillLevelUpMap::const_iterator iter = m_mSkillLevelUpMap.find( nSkillId );
		if ( iter == m_mSkillLevelUpMap.end() )
		{
			return 0;
		}
		const CfgSkillLevelUp& cfgSkill = iter->second;
		return cfgSkill.nBookId[nLevel];
	}
private:
	CfgSkillLevelUpMap	m_mSkillLevelUpMap;
};

struct CfgTask 
{
	int32_t id;
	char  name[MAX_NAME_CCH_LENGTH+1];
	int32_t type;
	int32_t group;
	int32_t can_giveup;
	int32_t pretask;
	int32_t	posttask;
	int32_t main_order;
	Job_t job;
	int32_t level;
	int32_t max_level;
	int32_t kingdom;
	int32_t start_npc;
	int32_t end_npc;
	MemChrBagVector items_receive;
	int32_t award_exp;
	int32_t award_money;
	int32_t gold;
	int32_t dilong;
	int32_t rongyu;
	int32_t fuwen;
	int32_t shuguang;
	int32_t dungeon;
	MemChrBagVector award_item;
	MemChrJobBagVector award_optional;
	int32_t condition;
	int32_t done_count;
	TaskRequest request;
	int32_t JunTuanZiJin;
	int32_t GongXian;
};
typedef std::map<int32_t, CfgTask> CfgTaskTable;
typedef std::vector<CfgTask> CfgTaskVector;

struct CfgTrailer 
{
	int32_t id;
	std::string name;
	int32_t level;
	int32_t pdef;
	int32_t mdef;
	int32_t maxhp;
	int32_t sp;
	int32_t time;

	void reset()
	{
		id =0;
		level=0;
		pdef =0;
		mdef =0;
		maxhp=0;
		maxhp=0;
		sp=0;
		time=0;
		name="";
	}
	CfgTrailer()
	{
		reset();
	}
};
typedef std::map<int32_t, CfgTrailer> CfgTrailerTable;

struct CfgTrap 
{
	int32_t id;
	int32_t cd;
	int32_t delay;			// �ӳ���Ӧ
	int32_t	event_type;		// ����Ч��
	std::string effect;		// Ч������
	int32_t item_cost;		// ��������
	int32_t life;			// ��������(��)
};
typedef std::map<int32_t, CfgTrap> CfgTrapTable;


struct cfgParam
{
	int32_t param1;
	int32_t param2;
};

typedef std::vector<cfgParam> cfgParams;
typedef std::map<int16_t, cfgParams> cfgParamsTable;


//��ͨ�����չ
struct cfgExActivity
{
	int16_t actid;
	int32_t starttime;
	int32_t endtime;
	cfgParams gifts;
	char strgifts[200];
};
typedef std::map<int16_t,cfgExActivity> cfgExActivitys;

struct MemFamily 
{
	int32_t id;
	int32_t leaderID;
	int32_t level;
};
typedef std::vector<MemFamily> MemFamilyVector;

struct SysServerRatio 
{
	int32_t sid;
	int32_t begin_date;
	int32_t end_date;
	int32_t exp_ratio;
};
typedef std::map<int32_t, SysServerRatio> SysServerRatioTable;

//����װ�����ݽṹ
struct CfgMountEquip
{
	int32_t		id;
	std::string name;
	std::string desc;
	std::string url;
	int32_t		limit_level;
};
typedef std::map<int32_t, CfgMountEquip> CfgMountEquipTable;

////�������ݽṹ
//struct CfgKingdom
//{
//	int32_t id;								//����ID
//	std::string name;						//��������
//	int32_t task_id;						//ѡ����Һ�����ID
//	int32_t random_gift;					//������
//};
////�����б�
//typedef std::map<int32_t, CfgKingdom> CfgKingdomTable;

struct CfgYellowStone
{
	int32_t id;
	int32_t yellow_type; //1:����ÿ�����(�ȼ�Ϊ����ȼ�) 2:��ѻ������ÿ����� 3:����ɳ����(�ȼ�Ϊ����ȼ�) 4:�����������
	int32_t level; //�ȼ�
	MemChrBagVector awards;
};
typedef std::map<int32_t, CfgYellowStone> CfgYellowStoneTables;


// װ����
#define MAX_EQUIP_BASE_ATTR_COUNT	4
struct CfgEquip
{
	int32_t		m_nId;										// id
	int8_t		m_nType;									// ���� EQUIP_TYPE
	int32_t		m_nLevel;									// �ȼ�
	int32_t		m_nSoulLevel;								// �����ȼ�
	int8_t		m_nJob;										// ְҵ
	int8_t		m_nQuality;									// Ʒ��
	int32_t		m_nSuitId;									// ��װID
	int32_t		m_nPrice;									// �ۼ�
	int32_t		m_nGrade;									// ����
	int32_t		m_nRansomWorth;								// ��ؼ�ֵ
	int32_t		m_Battle;
	AttrAddon	m_vAttr[MAX_EQUIP_BASE_ATTR_COUNT];			// ��������
	AttrAddon	m_StarAttr[MAX_EQUIP_BASE_ATTR_COUNT];		// ǿ����������
	int32_t		m_nWuHunExp;								// ��꾭��
	int8_t		m_nBroadcast;								// �����Ƿ񹫸�
};
typedef std::map<int32_t, CfgEquip> CfgEquipMap;

class CfgEquipUpGrade
{
public:
	CfgEquipUpGrade(){ CleanUp(); }
	void CleanUp()
	{
		m_nId				= 0;
		m_nGiveId			= 0;
		m_nFailLine			= 0;
		m_nSuccessLine		= 0;
		m_nFullLucky		= 0;
		m_nGetLucky			= 0;
		m_nRate				= 0;
		m_nTotalRate		= 0;
		bzero( &m_CostUsualItem, sizeof( m_CostUsualItem ) );
		bzero( &m_CostSpecialItem, sizeof( m_CostSpecialItem ) );
	}
	int32_t			m_nId;					// ԭװ��id					
	int32_t			m_nGiveId;				// �������װ��Id		
	int32_t			m_nFailLine;			// ��ʼ�б���������ֵ							
	int32_t			m_nSuccessLine;			// �ض��ɹ�������ֵ
	int32_t			m_nFullLucky;			// ����ֵ���ֵ
	int32_t			m_nGetLucky;			// ʧ�ܻ�õ�����ֵ
	int32_t			m_nRate;				// ��������
	int32_t			m_nTotalRate;			// ������ֵ
	ItemData		m_CostUsualItem;		// ��ͨ��Ʒ			
	ItemData		m_CostSpecialItem;		// ������Ʒ					
};
typedef std::map<int32_t, CfgEquipUpGrade> CfgEquipUpGradeMap;

class CfgEquipUpQuality
{
public:
	CfgEquipUpQuality(){ CleanUp(); }
	void CleanUp()
	{
		m_nId				= 0;
		m_nGiveId			= 0;
		m_nFailLine			= 0;
		m_nSuccessLine		= 0;
		m_nFullLucky		= 0;
		m_nGetLucky			= 0;
		m_nRate				= 0;
		m_nTotalRate		= 0;
		m_OpenHoleRate		= 0;
		m_OpenSecondHoleRate	= 0;
		bzero( &m_CostUsualItem, sizeof( m_CostUsualItem ) );
		bzero( &m_CostSpecialItem, sizeof( m_CostSpecialItem ) );
	}
	int32_t			m_nId;					// ԭװ��id					
	int32_t			m_nGiveId;				// �������װ��Id		
	int32_t			m_nFailLine;			// ��ʼ�б���������ֵ							
	int32_t			m_nSuccessLine;			// �ض��ɹ�������ֵ
	int32_t			m_nFullLucky;			// ����ֵ���ֵ
	int32_t			m_nGetLucky;			// ʧ�ܻ�õ�����ֵ
	int32_t			m_nRate;				// ��������
	int32_t			m_nTotalRate;			// ������ֵ
	ItemData		m_CostUsualItem;		// ��ͨ��Ʒ			
	ItemData		m_CostSpecialItem;		// ������Ʒ	
	int32_t			m_OpenHoleRate;			// ������һ��ʯ�׵ĸ���
	int32_t			m_OpenSecondHoleRate;	// �����ڶ���ʯ�׵ĸ���
};
typedef std::map<int32_t, CfgEquipUpQuality> CfgEquipUpQualityMap;

class CfgEquipUpStar
{
public:
	CfgEquipUpStar(){ CleanUp(); }
	void CleanUp()
	{
		m_nStar			= 0;
		m_nFailLine		= 0;
		m_nSuccessLine	= 0;
		m_nFullLucky	= 0;
		m_nFailAddLucky	= 0;
		m_nRate			= 0;
		m_nTotalRate	= 0;
		bzero( &m_CostUsualItem, sizeof( m_CostUsualItem ) );
		bzero( &m_CostSpecialItem,sizeof( m_CostSpecialItem ));
	}
	int32_t			m_nStar;								// �Ǽ�
	int32_t			m_nFailLine;							// �ض�ʧ����
	int32_t			m_nSuccessLine;							// �ض��ɹ���
	int32_t			m_nFullLucky;							// ������ֵ
	int32_t			m_nFailAddLucky;						// ʧ�ܼӵ�����ֵ
	int32_t			m_nRate;								// �ɹ�����
	int32_t			m_nTotalRate;							// �ɹ��ܸ���
	ItemData		m_CostUsualItem;						// ������ͨ�����б�
	ItemData		m_CostSpecialItem;						// �����������
};
typedef std::map<int32_t, CfgEquipUpStar> CfgEquipUpStarMap;

#define MAX_EQUIP_ADD_ATTR_COUNT	10
class CfgEquipAddAttr
{
public:
	CfgEquipAddAttr(){ CleanUp(); }
	void CleanUp()
	{
		m_nLevel		= 0;
		m_nMaxRate		= 0;
		m_lstAddAttr.clear();
	}
	int32_t GetAttrValue( int32_t nAttr ) const
	{
		CfgAddAttrList::const_iterator iter = m_lstAddAttr.begin();
		CfgAddAttrList::const_iterator eiter = m_lstAddAttr.end();
		for ( ; iter != eiter; ++iter )
		{
			if ( iter->m_nAttr == nAttr )
			{
				return iter->m_nValue;
			}
		}
		return 0;
	}
	int32_t GetRandAttr() const
	{
		if ( m_nMaxRate <= 0 )
		{
			return 0;
		}
		int32_t nRand = RANDOM.generate( 0, m_nMaxRate );
		CfgAddAttrList::const_iterator iter = m_lstAddAttr.begin();
		CfgAddAttrList::const_iterator eiter = m_lstAddAttr.end();
		for ( ; iter != eiter; ++iter )
		{
			if ( nRand < iter->m_nRate )
			{
				return iter->m_nAttr;
			}
		}
		return 0;
	}
	int32_t			m_nLevel;								// �ȼ�
	int32_t			m_nMaxRate;								// �ܸ���
	CfgAddAttrList	m_lstAddAttr;							// ���������б�
};
typedef std::map<int32_t, CfgEquipAddAttr> CfgEquipAddAttrMap;

#define EQUIP_GOAL_STAR		1		// ǿ���Ǽ�
#define EQUIP_GOAL_GEM		2		// ��ʯ�ȼ�
class CfgEquipGoal
{
public:
	CfgEquipGoal(){ CleanUp(); }
	void CleanUp()
	{
		m_nIndex	= 0;
		m_nType		= 0;
		m_nParam	= 0;
		bzero( m_vAddAttr, sizeof( m_vAddAttr ) );
	}
	
	int32_t		m_nIndex;								// ����
	int8_t		m_nType;								// ����
	int32_t		m_nParam;								// ����
	AttrAddon	m_vAddAttr[MAX_EQUIP_ADD_ATTR_COUNT];	// ���������б�
};
typedef std::map<int32_t, CfgEquipGoal> CfgEquipGoalMap;

class CfgEquipSuit
{
public:
	CfgEquipSuit(){ CleanUp(); }
	void CleanUp()
	{
		m_nId	= 0;
		m_lstEquips.clear();
		m_lstSuitAttr.clear();
	}

	AttrAddonList GetAddAttr( int32_t nCount ) const
	{
		AttrAddonList attrList;
		CfgSuitAttrList::const_iterator iter = m_lstSuitAttr.begin();
		CfgSuitAttrList::const_iterator eiter = m_lstSuitAttr.end();
		for ( ; iter != eiter; ++iter )
		{
			if ( iter->m_nCount > nCount )
			{
				continue;
			}
			AttrAddon attr = {};
			attr.index = iter->m_nAttr;
			attr.addon = iter->m_nValue;
			attrList.push_back( attr );
		}
		return attrList;
	}

	int32_t			m_nId;									// ����
	Int32List		m_lstEquips;							// ��װ����װ���б� 
	CfgSuitAttrList	m_lstSuitAttr;							// ���������б�
};
typedef std::map<int32_t, CfgEquipSuit> CfgEquipSuitMap;

class CfgEquipTable
{
public:
	void AddEquip( const CfgEquip& equip )
	{
		m_mEquip[equip.m_nId] = equip;
	}

	const CfgEquip*	GetEquip( int32_t id ) const
	{
		CfgEquipMap::const_iterator iter = m_mEquip.find( id );
		if ( iter != m_mEquip.end() )
		{
			return &( iter->second );
		}
		return NULL;
	}

	void AddEquipUpGrade( const CfgEquipUpGrade& equip )
	{
		m_mEquipUpGrade[equip.m_nId] = equip;
	}

	const CfgEquipUpGrade* GetEquipUpGrade( int32_t id ) const
	{
		CfgEquipUpGradeMap::const_iterator iter = m_mEquipUpGrade.find( id );
		if ( iter != m_mEquipUpGrade.end() )
		{
			return &( iter->second );
		}
		return NULL;
	}

	void AddEquipUpQuality( const CfgEquipUpQuality& equip )
	{
		m_mEquipUpQuality[equip.m_nId] = equip;
	}

	const CfgEquipUpQuality* GetEquipUpQuality( int32_t id ) const
	{
		CfgEquipUpQualityMap::const_iterator iter = m_mEquipUpQuality.find( id );
		if ( iter != m_mEquipUpQuality.end() )
		{
			return &( iter->second );
		}
		return NULL;
	}

	void AddEquipUpStar( const CfgEquipUpStar& equip )
	{
		m_mEquipUpStar[equip.m_nStar] = equip;
	}

	const CfgEquipUpStar* GetEquipUpStar( int32_t nStar ) const
	{
		CfgEquipUpStarMap::const_iterator iter = m_mEquipUpStar.find( nStar );
		if ( iter != m_mEquipUpStar.end() )
		{
			return &( iter->second );
		}
		return NULL;
	}

	void AddEquipAddAttr( const CfgEquipAddAttr& equip )
	{
		m_mEquipAddAttr[equip.m_nLevel] = equip;
	}

	const CfgEquipAddAttr* GetEquipAddAttr( int32_t nLevel ) const
	{
		CfgEquipAddAttrMap::const_iterator iter = m_mEquipAddAttr.find( nLevel );
		if ( iter != m_mEquipAddAttr.end() )
		{
			return &( iter->second );
		}
		return NULL;
	}

	void AddEquipGoal( const CfgEquipGoal& equip )
	{
		m_mEquipGoal[equip.m_nIndex] = equip;
	}

	const CfgEquipGoal* GetEquipGoal( int8_t nType, int32_t nParam ) const
	{
		int32_t	nMaxParam = 0;
		CfgEquipGoalMap::const_iterator findIter = m_mEquipGoal.end();
		CfgEquipGoalMap::const_iterator iter = m_mEquipGoal.begin();
		CfgEquipGoalMap::const_iterator eiter = m_mEquipGoal.end();
		for ( ; iter != eiter; ++iter )
		{
			const CfgEquipGoal& stu = iter->second;
			if ( stu.m_nType == nType && stu.m_nParam <= nParam )
			{
				if ( stu.m_nParam > nMaxParam )
				{
					findIter = iter;
					nMaxParam = stu.m_nParam;
				}
			}
		}
		if ( findIter != eiter )
		{
			return &( findIter->second );
		}
		return NULL;
	}

	void AddEquipSuit( const CfgEquipSuit& equip )
	{
		m_mEquipSuit[equip.m_nId] = equip;
	}

	const CfgEquipSuit* GetEquipSuit( int32_t nId ) const
	{
		CfgEquipSuitMap::const_iterator iter = m_mEquipSuit.find( nId );
		if ( iter != m_mEquipSuit.end() )
		{
			return &( iter->second );
		}
		return NULL;
	}

	bool AddWuHunHoleExp( int32_t nHole, int32_t nExp )
	{
		m_mWuHunHoleExp[nHole] = nExp;
		return true;
	}

	int32_t GetWuHunHoleExp( int32_t nHole ) const
	{
		Int32Int32Map::const_iterator iter = m_mWuHunHoleExp.find( nHole );
		if ( iter != m_mWuHunHoleExp.end() )
		{
			return iter->second;
		}

		return 0;
	}

	bool AddWuHunMoHunExp( int32_t nLevel, int32_t nExp )
	{
		m_mWuHunMoHunExp[nLevel] = nExp;
		return true;
	}

	int32_t GetWuHunMoHunExp( int32_t nLevel ) const
	{
		Int32Int32Map::const_iterator iter = m_mWuHunMoHunExp.find( nLevel );
		if ( iter != m_mWuHunMoHunExp.end() )
		{
			return iter->second;
		}

		return 0;
	}
private:
	CfgEquipMap				m_mEquip;
	CfgEquipUpGradeMap		m_mEquipUpGrade;
	CfgEquipUpQualityMap	m_mEquipUpQuality;
	CfgEquipUpStarMap		m_mEquipUpStar;
	CfgEquipAddAttrMap		m_mEquipAddAttr;
	CfgEquipGoalMap			m_mEquipGoal;
	CfgEquipSuitMap			m_mEquipSuit;
	Int32Int32Map			m_mWuHunHoleExp;
	Int32Int32Map			m_mWuHunMoHunExp;
};

// ��ʯ��
#define MAX_GEM_BASE_ATTR_COUNT		5
// struct CfgItemGem
// {
// 	int32_t		m_nId;
// 	int8_t		m_nType;
// 	int8_t		m_nGemLevel;
// 	int32_t		m_nLevel;
// 	int32_t		m_nPrice;
// 	int8_t		m_nQuality;
// 	int32_t		m_nLayNum;
// };

//�±�ʯ�ṹ
struct CfgItemGem
{
	int32_t		m_nId;
	int8_t		m_nType;
	int8_t		m_nGemLevel;
	int32_t		m_nLevel;
	int32_t		m_nPrice;
	int8_t		m_nQuality;
	int32_t		m_nLayNum;
	int32_t		m_EffectType;  //��ʯЧ������
	int32_t		m_AddRate;	   //��ʯЧ������
	int32_t		m_nGrade;
	int32_t		m_nRansomWorth;//��ؼ�ֵ
	int32_t		m_InValue;
	int32_t		m_OutValue;
//	AttrAddon	m_vAttr[MAX_GEM_BASE_ATTR_COUNT];
	int8_t		m_nBroadcast;
};
typedef std::map<int32_t, CfgItemGem> CfgGemMap;

class CfgItemGemTable
{
public:
	void	Add( const CfgItemGem& gem )
	{
		m_mGemMap[gem.m_nId] = gem;
	}

	const CfgItemGem*	GetItemGem( int32_t id ) const
	{
		CfgGemMap::const_iterator iter = m_mGemMap.find( id );
		if ( iter != m_mGemMap.end() )
		{
			return &( iter->second );
		}
		return NULL;
	}
private:
	CfgGemMap	m_mGemMap;
};

// �ϳɱ�
class CfgItemCombi
{
public:
	CfgItemCombi()
	{
		m_nId			= 0;
		CostList.clear();
		m_nGiveId		= 0;		
		m_nGiveClass	= 0;	
		m_nGiveCount	= 0;	
		m_nMoney		= 0;		
		m_nRate			= 0;		
		m_nTotalRate	= 0;	
	}
	int32_t	m_nId;
	ItemDataList CostList;//�ϳ�Ҫ�۳����б�
	int32_t m_nGiveId;				// �ϳɸ������ID
	int8_t	m_nGiveClass;			// �ϳɸ����������
	int32_t	m_nGiveCount;			// �ϳɸ����������
	int32_t	m_nMoney;				// �ϳ�����ͭǮ
	int32_t m_nRate;				// �ϳɳɹ�����
	int32_t m_nTotalRate;			// �ϳɳɹ��ܸ���
	int8_t	m_IsGongGao;			// �Ƿ񹫸�
};
typedef std::map<int32_t, CfgItemCombi> CfgItemCombiMap;

class CfgItemCombiTable
{
public:
	void	Add( const CfgItemCombi& itemCombi )
	{
		m_mItemCombi[itemCombi.m_nId]	= itemCombi;
	}

	const CfgItemCombi*	GetItemCombi( int32_t key ) const
	{
		CfgItemCombiMap::const_iterator iter = m_mItemCombi.find( key );
		if ( iter != m_mItemCombi.end() )
		{
			return &( iter->second );
		}
		return NULL;
	}
private:
	CfgItemCombiMap	m_mItemCombi;
};

struct CfgBagSlotOpenTime 
{
	int32_t	m_nSlot;			// ������
	int32_t	m_nSeconds;			// ����������Ҫʱ��
	int32_t m_nBagAddExp;		// ����������þ���
	int32_t m_nBagAddHp;		// �����������HP
	int32_t m_nDepotNeedSeconds;// �����ֿ���Ҫ��ʱ��
	int32_t m_nDepotAddExp;		// �����ֿ��þ���
	int32_t m_nDepotAddHp;		// �����ֿ���HP
};
typedef std::map<int32_t, CfgBagSlotOpenTime> CfgBagSlotOpenTimeMap;
class CfgBagSlotOpenTimeTable
{
public:
	void	Add( const CfgBagSlotOpenTime& stu )
	{
		m_mBagSlotOpenTime[stu.m_nSlot] = stu;
	}

	const CfgBagSlotOpenTime*	Get( int32_t nOpenSlot ) const
	{
		CfgBagSlotOpenTimeMap::const_iterator iter = m_mBagSlotOpenTime.find( nOpenSlot );
		if ( iter != m_mBagSlotOpenTime.end() )
		{
			return &( iter->second );
		}
		return NULL;
	}

	int32_t GetNeedSeconds( int32_t nOpenSlot, int8_t Type ) const
	{
		CfgBagSlotOpenTimeMap::const_iterator iter = m_mBagSlotOpenTime.find( nOpenSlot );
		if ( iter != m_mBagSlotOpenTime.end() )
		{
			if ( Type = CBT_BAG )
			{
				return iter->second.m_nSeconds;
			}
			else
			{
				return iter->second.m_nDepotNeedSeconds;
			}
		}
		return 0;
	}

	int32_t GetNeedSeconds( int32_t nFromSlot, int32_t nToSlot, int8_t Type ) const
	{
		int32_t nSeconds = 0;
		for ( int32_t i = nFromSlot; i <= nToSlot; ++i )
		{
			CfgBagSlotOpenTimeMap::const_iterator iter = m_mBagSlotOpenTime.find( i );
			if ( iter != m_mBagSlotOpenTime.end() )
			{
				if ( Type == CBT_BAG )
				{
					nSeconds += iter->second.m_nSeconds;
				}
				else
				{
					nSeconds += iter->second.m_nDepotNeedSeconds;
				}
			}
		}
		return nSeconds;
	}
private:
	CfgBagSlotOpenTimeMap	m_mBagSlotOpenTime;
};

#define MAX_PET_ATTR_RATE	100
// ���Գ�ʼ����Ӧ�İٷֱ�
class CfgPetAttrInitRateTable
{
	friend class CfgData;
public:
	CfgPetAttrInitRateTable()
	{
		bzero( m_vStartRate, sizeof( m_vStartRate ) );
	}

	int32_t	GetStartRate() const
	{
		if ( m_nMaxStartRate < 1 )
		{
			return 0;
		}

		int32_t nRate	= RANDOM.generate( 1, m_nMaxStartRate );
		for ( int32_t i = 0; i < MAX_PET_ATTR_RATE; ++i )
		{
			if ( nRate <= m_vStartRate[i] )
			{
				return i+1;
			}
		}
		return 0;
	}

	int32_t	GetLuckyRate() const
	{
		if ( m_nMaxLuckyRate < 1 )
		{
			return 0;
		}

		int32_t nRate	= RANDOM.generate( 1, m_nMaxLuckyRate );
		for ( int32_t i = 0; i < MAX_PET_ATTR_RATE; ++i )
		{
			if ( nRate <= m_vLuckyRate[i] )
			{
				return i+1;
			}
		}
		return 0;
	}

	int32_t	GetGrowRate() const
	{
		if ( m_nMaxGrowRate < 1 )
		{
			return 0;
		}

		int32_t nRate	= RANDOM.generate( 1, m_nMaxGrowRate );
		for ( int32_t i = 0; i < MAX_PET_ATTR_RATE; ++i )
		{
			if ( nRate <= m_vGrowRate[i] )
			{
				return i+1;
			}
		}
		return 0;
	}

private:
	bool add( int32_t nPercent, int32_t nStartRate, int32_t nLuckyRate, int32_t nGrowRate )
	{
		if ( nPercent <= 0 || nPercent > MAX_PET_ATTR_RATE )
		{
			return false;
		}
		m_nMaxStartRate += nStartRate;
		m_vStartRate[nPercent-1] = m_nMaxStartRate;

		m_nMaxLuckyRate += nLuckyRate;
		m_vLuckyRate[nPercent-1] = m_nMaxLuckyRate;
		
		m_nMaxGrowRate += nGrowRate;
		m_vGrowRate[nPercent-1] = m_nMaxGrowRate;
		return true;
	}

private:
	int32_t m_nMaxStartRate;
	int32_t	m_vStartRate[MAX_PET_ATTR_RATE];
	int32_t m_nMaxLuckyRate;
	int32_t	m_vLuckyRate[MAX_PET_ATTR_RATE];
	int32_t m_nMaxGrowRate;
	int32_t	m_vGrowRate[MAX_PET_ATTR_RATE];
};

#define MAX_PET_SKILL_RATE	100000	// ����������ʻ���
class CfgPetData 
{
public:
	CfgPetData(){ CleanUp(); }
	~CfgPetData(){}

	void CleanUp()
	{
		m_nPetId			= 0;
		m_strName			= "";
		m_nLucky			= 0;
		m_nRein				= 0;
		m_nOrderJob			= 0;
		m_nPetJob			= 0;
		m_nRare				= 0;
		m_nPhase			= PP_INVALID;
		m_nHatchTime		= 0;
		m_nMutiHatchTime	= 0;
		m_nGrowRatio		= 0;
		m_nXxoo				= 0;
		m_nSkillId			= 0;
		m_nSecondSkillId	= 0;
		m_Zoarium			= 0;
		bzero( m_vMaxAttr, sizeof( m_vMaxAttr ) );
		bzero( m_vMaxPoints, sizeof( m_vMaxPoints ) );
		bzero( m_vSkill, sizeof( m_vSkill ) );
		bzero( m_vSkillRate, sizeof( m_vSkillRate ) );
	}

	SkillId_t RandSkill() const
	{
		int32_t nRand = RANDOM.generate( 0, MAX_PET_SKILL_RATE );
		for ( int32_t i = 0; i < PET_SKILL_COUNT; ++i )
		{
			if ( nRand < m_vSkillRate[i] )
			{
				return m_vSkill[i];
			}
		}
		return 0;
	}

public:
	int32_t			m_nPetId;						// ����ID
	std::string		m_strName;						// ��������
	Job_t			m_nOrderJob;					// ָ������
	Job_t			m_nPetJob;						// ��������
	int32_t			m_nRare;						// ������
	int32_t			m_nRein;						// ת����������
	PET_PHASE		m_nPhase;						// ����=0���������
	int32_t			m_nLucky;						// ����ֵ
	int32_t			m_vMaxAttr[PET_ATTR_COUNT];		// ��������
	int32_t			m_vMaxPoints[PET_ATTR_COUNT];	// �������
	SkillId_t		m_nSkillId;						// �����츳����
	SkillId_t		m_nSecondSkillId;				// �ڶ��츳����
	SkillId_t		m_vSkill[PET_SKILL_COUNT];		// ���＼��
	SkillId_t		m_vSkillRate[PET_SKILL_COUNT];	// ���＼�ܸ���
	int32_t			m_nHatchTime;					// ����ʱ��
	int32_t			m_nMutiHatchTime;				// ��������ʱ��
	int32_t			m_nRideSkin;					// ���Ƥ��
	int32_t			m_nGrowRatio;					// �ɳ�ϵ��
	int8_t			m_nXxoo;						// XO������
	int8_t			m_Zoarium;						// �Ƿ��ܺ���
	int32_t			m_WuHunExp;						// ��꾭��
};
typedef std::map<int32_t, CfgPetData>	CfgPetDataMap;

struct PetPackageCost 
{
	int32_t nPoints;
	int8_t	nCostType;
	int32_t nCostValue;
	int8_t  nXoCostType;
	int32_t nXoCostValue;
};
typedef std::list<PetPackageCost> PetPackageCostList;

typedef std::list<PetId_t>	PetIdList;

struct CWuHunExp
{
	int32_t Points;
	int32_t Exp;
};
typedef std::list<CWuHunExp> WuHunExpList;

struct PetBuyInfo
{
	int32_t ShopId;
	int32_t Points;
};
typedef map<int32_t,PetBuyInfo> PetBuyMap;

struct FamilyTaskReward
{
	int32_t		Index;
	int32_t		NeedCount;
	MemChrBagVector Rewards;
};
typedef map<int32_t,FamilyTaskReward>  FamilyTaskRewardMap;
class CfgPetTable
{
public:
	CfgPetTable(){}
	~CfgPetTable(){}

	void Add( const CfgPetData& pet )
	{
		m_mPetCfgData[pet.m_nPetId] = pet;
	}
	
	void AddWuHunExp( const CWuHunExp& WuHunExp )
	{
		m_WuHunExpList.push_back( WuHunExp );
	}

	void AddPetBuy( PetBuyInfo& PetBuy )
	{
		m_PetBuyMap[PetBuy.ShopId] = PetBuy;
	}
	
	int32_t GetBuyPetPoints( int32_t ShopId ) const
	{
		PetBuyMap::const_iterator it = m_PetBuyMap.find( ShopId );
		if ( it != m_PetBuyMap.end() )
		{
			return it->second.Points;
		}
		return 0;
	}

	int32_t GetWuHunExp( int32_t Points ) const
	{
		WuHunExpList::const_iterator it = m_WuHunExpList.begin();
		for ( ; it != m_WuHunExpList.end(); ++it )
		{
			if ( Points < it->Points )
			{
				return it->Exp;
			}
		}
		return 0;
	}

	const CfgPetData* GetPet( int32_t nPetId ) const
	{
		CfgPetDataMap::const_iterator findIter = m_mPetCfgData.find( nPetId );
		if ( findIter != m_mPetCfgData.end() )
		{
			return &( findIter->second );
		}
		return NULL;
	}

	void AddInitPet( Job_t job, const Int32List& pets )
	{
		m_vInitPet[job] = pets;
	}

	const Int32List& GetInitPets( Job_t job ) const
	{
		return m_vInitPet[job];
	}

	void AddPetPackageCost( const PetPackageCost& cost )
	{
		m_lstPetPackageCost.push_back( cost );
	}

	const PetPackageCost* GetPackageCost( int32_t nPoints ) const
	{
		PetPackageCostList::const_iterator iter = m_lstPetPackageCost.begin();
		PetPackageCostList::const_iterator eiter = m_lstPetPackageCost.end();
		for ( ; iter != eiter; ++iter )
		{
			if ( nPoints < iter->nPoints )
			{
				return &(*iter);
			}
		}
		return NULL;
	}

private:
	CfgPetDataMap		m_mPetCfgData;
	Int32List			m_vInitPet[PJ_JOB_COUNT];
	PetPackageCostList	m_lstPetPackageCost;
	WuHunExpList		m_WuHunExpList;
	PetBuyMap			m_PetBuyMap;
};

struct CfgPetEgg
{
	int32_t nId;
	int32_t nPetId;
	int32_t nLevel;
	int32_t nRein;
	int32_t vAttr[PET_ATTR_COUNT];
	int32_t nHatchingTime;
	int32_t nMutiHatchingTime;
	int32_t nPrice;
	int32_t nWuHunExp;
	int8_t	broadcast;
};
typedef std::map<int32_t, CfgPetEgg> CfgPetEggMap;
class CfgPetEggTable
{
public:
	CfgPetEggTable()
	{
		CleanUp();
	}

	void CleanUp()
	{
		m_mPetEgg.clear();
	}

	bool Add( const CfgPetEgg& petEgg )
	{
		m_mPetEgg[petEgg.nId] = petEgg;
		return true;
	}

	const CfgPetEgg* GetEgg( int32_t nId ) const
	{
		CfgPetEggMap::const_iterator iter = m_mPetEgg.find( nId );
		if ( iter != m_mPetEgg.end() )
		{
			return &(iter->second);
		}
		return NULL;
	}

private:
	CfgPetEggMap	m_mPetEgg;
};

struct CfgPetIllusionGrow 
{
	int32_t nMainPointMin;
	int32_t nMainPointMax;
	int32_t nVicePoint;
	int32_t nVicePointMax;
	int32_t nViceLevel;
	Int32Vector nShopIds;
	int32_t Compensation;
};

typedef std::list<CfgPetIllusionGrow> CfgPetIllusionGrowList;
class CfgPetIllusionTable
{
public:
	CfgPetIllusionTable(){}
	~CfgPetIllusionTable(){}
	bool AddGrow( const CfgPetIllusionGrow& grow )
	{
		m_lstPetIllusionGrow.push_back( grow );
		return true;
	}

	const CfgPetIllusionGrow*	GetGrow( int32_t nMainPoint ) const
	{
		CfgPetIllusionGrowList::const_iterator iter = m_lstPetIllusionGrow.begin();
		CfgPetIllusionGrowList::const_iterator eiter = m_lstPetIllusionGrow.end();
		for ( ; iter != eiter; ++iter )
		{
			if ( nMainPoint >= iter->nMainPointMin && nMainPoint < iter->nMainPointMax )
			{
				return &(*iter);
			}
		}
		return NULL;
	}
	const CfgPetIllusionGrow* GetGrowViceGrow( int32_t VicePoint ) const
	{
		CfgPetIllusionGrowList::const_iterator iter = m_lstPetIllusionGrow.begin();
		CfgPetIllusionGrowList::const_iterator eiter = m_lstPetIllusionGrow.end();
		for ( ; iter != eiter; ++iter )
		{
			if ( VicePoint >= iter->nVicePoint && VicePoint < iter->nVicePointMax )
			{
				return &(*iter);
			}
		}
		return NULL;
	}
	bool IsRightShopId( int32_t nMainPoint, int32_t ShopId ) const
	{
		CfgPetIllusionGrowList::const_iterator iter = m_lstPetIllusionGrow.begin();
		CfgPetIllusionGrowList::const_iterator eiter = m_lstPetIllusionGrow.end();
		for ( ; iter != eiter; ++iter )
		{
			if ( nMainPoint >= iter->nMainPointMin && nMainPoint < iter->nMainPointMax )
			{
				Int32Vector::const_iterator itShop = iter->nShopIds.begin();
				for ( ;itShop != iter->nShopIds.end(); ++itShop )
				{
					if ( *itShop == ShopId )
					{
						return true;
					}
				}
				return false;
			}
		}
		return false;
	}
private:
	CfgPetIllusionGrowList	m_lstPetIllusionGrow;
};

#define MAX_KNIGHT_QUALITY_SIZE		20
enum PET_KNIGHT_EFFECT
{
	PKE_ADD_EXP			= 1,	//������ӳ�
	PKE_REMINE_JUMP		= 2,	//��������Ծ���ĵ�����
	PKE_ADD_BATTLE		= 3,	//��ս�����ӳ�
	PKE_REMINE_DAMAGE	= 4,	//���˺�����
	PKE_ADD_DAMAGE		= 5,	//���˺��ӳ�
	PKE_ADD_MAX_HP		= 6,	//������ֵ�������
};

class CfgPetKnight 
{
public:
	CfgPetKnight(){ CleanUp(); }
	~CfgPetKnight(){}
	void CleanUp()
	{
		nLevel		= 0;
		nTitle		= 0;
		nEffectType	= 0;
		compIdList.clear();
		vQuality[MAX_KNIGHT_QUALITY_SIZE];
		vEffectValue[MAX_KNIGHT_QUALITY_SIZE];
	}

	bool IsInCompList( int32_t nPetId ) const
	{
		if ( compIdList.empty() )
		{
			return true;
		}
		Int32List::const_iterator iter =  compIdList.begin();
		Int32List::const_iterator eiter =  compIdList.end();
		for ( ; iter != eiter; ++iter )
		{
			if ( *iter == nPetId )
			{
				return true;
			}
		}
		return false;
	}
	bool IsNeedCal() const
	{
		switch( nEffectType )
		{
		case PKE_ADD_BATTLE:
		case PKE_REMINE_DAMAGE:
		case PKE_ADD_DAMAGE:
		case PKE_ADD_MAX_HP:
			return true;
		default: break;
		}
		return false;
	}
	int32_t GetEffectValue( int32_t nPoints ) const
	{
		int32_t nMax = -1;
		for ( int32_t i = 0; i < MAX_KNIGHT_QUALITY_SIZE; ++i )
		{
			if ( vQuality[i] < 0 )
			{
				break;
			}
			if ( nPoints < vQuality[i] )
			{
				break;
			}

			nMax = i;
		}

		if ( nMax >= 0 )
		{
			return vEffectValue[nMax];
		}
		return 0;
	}
	int32_t		nLevel;										// ����ȼ�
	int32_t		nTitle;										// �ƺţ�
	Int32List	compIdList;									// ���������ĳ����б�
	int8_t		nEffectType;								// ��������
	int32_t		vQuality[MAX_KNIGHT_QUALITY_SIZE];			// Ʒ��
	int32_t		vEffectValue[MAX_KNIGHT_QUALITY_SIZE];		// �ӳ�
};

class CfgPetKnightTable
{
	friend class CfgData;
public:
	CfgPetKnightTable(){ CleanUp(); }
	~CfgPetKnightTable(){}
	void CleanUp()
	{
		for ( int32_t i = 0; i < PET_KNIGHT_SIZE; ++i )
		{
			m_vKnight[i].CleanUp();
		}
	}

	const CfgPetKnight* GetKnight( int8_t nKnight ) const
	{
		if ( nKnight <= PK_NONE || nKnight >= PET_KNIGHT_SIZE )
		{
			return NULL;
		}
		return &m_vKnight[nKnight];
	}
private:
	CfgPetKnight	m_vKnight[PET_KNIGHT_SIZE];
};

struct PetHatchPoolOpenLevel 
{
	int32_t	nLevel;
	int32_t nVipLevel;
};

struct CfgPetLuckyItem
{
	int32_t nLucky;
	int8_t	nClass;
	int32_t	nId;
	int32_t	nCount;
	int32_t nRate;
};
typedef std::map<int32_t,CfgPetLuckyItem> CfgPetLuckyItemMap;

class CfgCharPetTable
{
friend class CfgData;
public:
	CfgCharPetTable(){CleanUp();}
	~CfgCharPetTable(){}
	void CleanUp()
	{
		bzero( m_vHatchPool, sizeof( m_vHatchPool ) );
		bzero( m_vMutiHatchPool, sizeof( m_vMutiHatchPool ) );
	}
	
	int32_t	GetHatchPoolSize( int32_t nLevel, int32_t nVipLevel ) const
	{
		for ( int32_t i = 0; i < PET_HATCH_POOL_SIZE; ++i )
		{
			if ( m_vHatchPool[i].nLevel > nLevel && m_vHatchPool[i].nVipLevel > nVipLevel )
			{
				return i;
			}
		}
		return PET_HATCH_POOL_SIZE;
	}

	int32_t GetMutiHatchPoolSize( int32_t nLevel, int32_t nVipLevel ) const
	{
		for ( int32_t i = 0; i < PET_MUTI_HATCH_POOL_SIZE; ++i )
		{
			if ( m_vMutiHatchPool[i].nLevel > nLevel && m_vMutiHatchPool[i].nVipLevel > nVipLevel )
			{
				return i;
			}
		}
		return PET_MUTI_HATCH_POOL_SIZE;
	}

	const CfgPetLuckyItem*	GetPetLuckyItem( int32_t nLucky ) const
	{
		CfgPetLuckyItemMap::const_iterator iter = m_mPetLucyItem.find( nLucky );
		if ( iter != m_mPetLucyItem.end() )
		{
			return &(iter->second);
		}
		return NULL;
	}

private:
	void addPetLuckyUseItem( const CfgPetLuckyItem& stu )
	{
		m_mPetLucyItem[stu.nLucky] = stu;
	}

private:
	PetHatchPoolOpenLevel	m_vHatchPool[PET_HATCH_POOL_SIZE];
	PetHatchPoolOpenLevel	m_vMutiHatchPool[PET_MUTI_HATCH_POOL_SIZE];
	CfgPetLuckyItemMap		m_mPetLucyItem;
};

struct CfgPetGift
{
	int32_t		nId;
	int32_t		nPetBaseId;
	int32_t		nLevel;
	int32_t		nGrowTimes;
	int32_t		m_vMaxAttr[PET_ATTR_COUNT];		// ��������
};
typedef std::map<int32_t, CfgPetGift> PetGiftMap;

struct FamilyTask
{	
	int32_t TaskId;
	int32_t MinLevel;
	int32_t MaxLevel;
};
typedef std::list<FamilyTask> FamilyTaskList;
typedef std::vector<FamilyTask> FamilyTaskVector;
class CfgFamilyTask
{
public:
	CfgFamilyTask(){ CleanUp();}
	~CfgFamilyTask(){};
	void CleanUp()
	{
		m_FamilyTaskList.clear();
	}
	void AddFamilyTask( int32_t nTaskId, int32_t nMinLevel, int32_t nMaxLevel )
	{
		FamilyTask task = { nTaskId, nMinLevel, nMaxLevel };
		m_FamilyTaskList.push_back( task );
	}
	int32_t GetFamilyTask( int32_t Level ) const
	{
		Int32Vector vTask;
		vTask.reserve( m_FamilyTaskList.size() );
		FamilyTaskList::const_iterator it = m_FamilyTaskList.begin();
		for ( ; it != m_FamilyTaskList.end(); ++it )
		{
			if ( Level >= it->MinLevel && Level <= it->MaxLevel )
			{
				vTask.push_back( it->TaskId );
			}
		}
		std::random_shuffle( vTask.begin(), vTask.end() );
		return vTask.front();
	}
private:
	FamilyTaskList m_FamilyTaskList;
};


#define MAX_CYCLE_STARS			10

struct TaskCycle 
{
	int32_t nTaskId;
	int32_t nMinLevel;
	int32_t nMaxLevel;
};
typedef std::list<TaskCycle> TaskCycleList;
typedef std::vector<TaskCycle> TaskCycleVector;

struct CycleStarRate
{
	int32_t		Level;
	int32_t		Rate[MAX_CYCLE_STARS];
	int32_t		TenStarTimes;
};
typedef std::list<CycleStarRate> StarRateList;

class CfgTaskCycleTable
{
public:
	CfgTaskCycleTable(){ CleanUp(); }
	~CfgTaskCycleTable(){}

	void CleanUp()
	{
		m_StarList.clear();
		m_lstTask.clear();
	}

	bool AddTask( int32_t nTaskId, int32_t nMinLevel, int32_t nMaxLevel )
	{
		TaskCycle task = { nTaskId, nMinLevel, nMaxLevel };
		m_lstTask.push_back( task );
		return true;
	}

	int32_t RandTask( int32_t nLevel ) const
	{
		Int32Vector vTask;
		vTask.reserve( m_lstTask.size() );
		int32_t FirstTask = 0;
		TaskCycleList::const_iterator iter = m_lstTask.begin();
		TaskCycleList::const_iterator eiter = m_lstTask.end();
		if ( iter != eiter )
		{
			FirstTask = iter->nTaskId;
		}
		for ( ; iter != eiter; ++iter )
		{
			if ( nLevel >= iter->nMinLevel && nLevel <= iter->nMaxLevel )
			{
				vTask.push_back( iter->nTaskId );
			}
		}

		if ( vTask.empty() )
		{
			return FirstTask;
		}

		std::random_shuffle( vTask.begin(), vTask.end() );
		return vTask.front();
	}

	bool AddStarRate( CycleStarRate& StarRate)
	{
		m_StarList.push_back( StarRate );
		return true;
	}

	int32_t GetTenStarTimes( int32_t Level ) const
	{
		StarRateList::const_iterator it =	m_StarList.begin();
		for ( ; it != m_StarList.end(); ++it )
		{
			if ( Level <= it->Level )
			{
				return it->TenStarTimes;
			}
		}
		return 0;
	}

	int8_t RandStar( int32_t Level ) const
	{
		StarRateList::const_iterator it =	m_StarList.begin();
		for ( ; it != m_StarList.end(); ++it )
		{
			if ( Level <= it->Level )
			{
				int32_t nRate	= RANDOM.generate( 1, 10000 );
				for ( int8_t i = 0; i < MAX_CYCLE_STARS; i++ )
				{
					if ( nRate <= it->Rate[i] )
					{
						return i+1;
					}
					nRate -= it->Rate[i];
				}
			}
		}
		return 0;
	}

private:
	StarRateList	m_StarList;
	TaskCycleList	m_lstTask;
};

class CfgFamilyPetRegistTable 
{
public:
	bool Add( int32_t nLevel, int32_t nCount )
	{
		m_mLevelPet[nLevel] = nCount;
		return true;
	}

	int32_t GetLevelCount( int32_t nLevel ) const
	{
		std::map<int32_t, int32_t>::const_iterator iter = m_mLevelPet.find( nLevel );
		if ( iter != m_mLevelPet.end() )
		{
			return iter->second;
		}
		return 0;
	}
private:
	std::map<int32_t, int32_t>	m_mLevelPet;
};

class CfgFamily 
{
public:
	CfgFamily(){ CleanUp(); }
	~CfgFamily(){}
	void CleanUp()
	{
		nLevel		= 0;
		nExp		= 0;
		nBattle		= 0;
		nMaxMembers	= 0;
		nTotemCount	= 0;
		bzero( vPosition, sizeof( vPosition ) );
	}

	int32_t		nLevel;								// �ȼ�
	int32_t		nExp;								// �����׶�
	int32_t		nBattle;							// ����ս����
	int32_t		nMaxMembers;						// �������
	int32_t		nTotemCount;						// ���ͼ�ڸ���
	int32_t		vPosition[FAMILY_POSITION_COUNT];	// ְλ����
};

struct WarVictoryHd		//��ս�
{
	int8_t				Index;
	int8_t				Type;
	int32_t				StartTime;
	int32_t				EndTime;
	int32_t				NeedGold;
	MemChrBagVector		Rewards;
	int32_t				Times;
	GongGaoList			GongGaoId;
};
typedef std::map<int8_t,WarVictoryHd> WarVictoryHdMap;


class CfgFamilyTable
{
public:
	CfgFamilyTable(){ CleanUp(); }
	~CfgFamilyTable(){}

	void CleanUp()
	{
		for ( int32_t i = 0; i < MAX_FAMILY_LEVEL; ++i )
		{
			m_vFamily[i].CleanUp();
		}
		bzero( m_vTotomActiveLevel, sizeof( m_vTotomActiveLevel ) );
		bzero( m_vFamilyPosition, sizeof( m_vFamilyPosition ) );
	}

	bool Add( const CfgFamily& family )
	{
		m_vFamily[family.nLevel-1] = family;
		return true;
	}

	bool Add( int8_t nPosition, const CfgFamilyPosition& position )
	{
		if ( nPosition < 0 || nPosition >= FAMILY_POSITION_COUNT )
		{
			return false;
		}

		m_vFamilyPosition[nPosition] = position;
		return true;
	}

	bool AddActiveTotom( int32_t nTotomId, int32_t nLevel )
	{
		if ( nTotomId <= 0 || nTotomId > MAX_PET_ID )
		{
			return false;
		}

		m_vTotomActiveLevel[nTotomId] = nLevel;
		return true;
	}

	const CfgFamily* GetFamilyInfo( int32_t nLevel ) const
	{
		if ( nLevel <= 0 || nLevel > MAX_FAMILY_LEVEL )
		{
			return NULL;
		}
		return &m_vFamily[nLevel-1];
	}

	int32_t GetTotomActiveLevel( int32_t nTotomId ) const
	{
		if ( nTotomId <= 0 || nTotomId > MAX_PET_ID )
		{
			return NULL;
		}
		return m_vTotomActiveLevel[nTotomId];
	}

	const CfgFamilyPosition* GetFamilyPosition( int8_t nPosition ) const
	{
		if ( nPosition < 0 || nPosition >= FAMILY_POSITION_COUNT )
		{
			return NULL;
		}
		return &m_vFamilyPosition[nPosition];
	}


private:
	CfgFamily			m_vFamily[MAX_FAMILY_LEVEL];
	int32_t				m_vTotomActiveLevel[MAX_PET_ID+1];						// ͼ�ڼ���������ŵȼ�
	CfgFamilyPosition	m_vFamilyPosition[FAMILY_POSITION_COUNT];				// ����ְλ����
};

struct TeamBuff 
{
	int32_t nExpRatio;
	int32_t nHPRatio;
};

class CfgTeamTable
{
public:
	CfgTeamTable(){ CleanUp(); }
	~CfgTeamTable(){}

	void CleanUp()
	{
		bzero( m_vBuff, sizeof( m_vBuff ) );
	}

	bool Add( int32_t nNum, int32_t nExpRatio, int32_t nHPRatio )
	{
		int32_t nIndex = nNum - 1;
		if ( nIndex < 0 || nIndex >= MAX_TEAM_MEMBER_COUNT )
		{
			return false;
		}
		if ( nExpRatio > 0 )
		{
			m_vBuff[nIndex].nExpRatio = nExpRatio;
		}
		if ( nHPRatio > 0 )
		{
			m_vBuff[nIndex].nHPRatio = nHPRatio;
		}
		return true;
	}

	int32_t GetTeamExpRatio( int32_t nNum ) const
	{
		int32_t nIndex = nNum - 1;
		if ( nIndex < 0 || nIndex >= MAX_TEAM_MEMBER_COUNT )
		{
			return 0;
		}
		return m_vBuff[nIndex].nExpRatio;
	}

	int32_t GetTeamHPRatio( int32_t nNum ) const
	{
		int32_t nIndex = nNum - 1;
		if ( nIndex < 0 || nIndex >= MAX_TEAM_MEMBER_COUNT )
		{
			return 0;
		}
		return m_vBuff[nIndex].nHPRatio;
	}

private:
	TeamBuff m_vBuff[MAX_TEAM_MEMBER_COUNT];
};

class CfgInsidePetTable
{
	typedef std::map<int32_t, AttrAddonVector>	AttrAddonMap;
public:
	void Add( int32_t nLevel, int32_t nExp, const AttrAddonVector& attrAddons1, const AttrAddonVector& attrAddons2, const AttrAddonVector& attrAddons3 )
	{
		m_mLevelExp[nLevel]		= nExp;
		m_mAttrAddon1[nLevel]	= attrAddons1;
		m_mAttrAddon2[nLevel]	= attrAddons2;
		m_mAttrAddon3[nLevel]	= attrAddons3;
	}

	const AttrAddonVector& GetAttrAddon( int32_t nLevel, Job_t job ) const
	{
		const AttrAddonMap* pMap = NULL;
		switch ( job )
		{
		case PJ_ZHANSHI:	pMap = &m_mAttrAddon1;	break;
		case PJ_FASHI:		pMap = &m_mAttrAddon2;	break;
		case PJ_LONGQI:		pMap = &m_mAttrAddon3;	break;
		default:break;
		}
		AttrAddonMap::const_iterator iter = pMap->find( nLevel );
		if ( iter != pMap->end() )
		{
			return iter->second;
		}
		return m_empty;
	}

	int32_t GetLevelExp( int32_t nLevel ) const
	{
		Int32Int32Map::const_iterator iter = m_mLevelExp.find( nLevel );
		if ( iter != m_mLevelExp.end() )
		{
			return iter->second;
		}
		return -1;
	}

	void AddExpItem( int32_t nId, int32_t nExp )
	{
		m_mExpItem[nId] = nExp;
	}

	int32_t	GetItemExp( int32_t nId ) const
	{
		Int32Int32Map::const_iterator iter = m_mExpItem.find( nId );
		if ( iter != m_mExpItem.end() )
		{
			return iter->second;
		}
		return -1;
	}

private:
	AttrAddonVector	m_empty;
	AttrAddonMap	m_mAttrAddon1;
	AttrAddonMap	m_mAttrAddon2;
	AttrAddonMap	m_mAttrAddon3;
	Int32Int32Map	m_mLevelExp;

	Int32Int32Map	m_mExpItem;
};

class CfgSoulAttrTable
{
	typedef std::map<int32_t, AttrAddonVector>	AttrAddonMap;
public:
	void Add( int32_t nLevel, int32_t nExp, const AttrAddonVector& attrAddons1, const AttrAddonVector& attrAddons2, const AttrAddonVector& attrAddons3, int32_t Battle )
	{
		m_mLevelExp[nLevel]		= nExp;
		m_mAttrAddon1[nLevel]	= attrAddons1;
		m_mAttrAddon2[nLevel]	= attrAddons2;
		m_mAttrAddon3[nLevel]	= attrAddons3;
		m_Battle[nLevel]		= Battle;
	}

	int32_t GetAddBattle( int32_t nLevel ) const
	{
		Int32Int32Map::const_iterator iter = m_Battle.find( nLevel );
		if ( iter != m_Battle.end() )
		{
			return iter->second;
		}
		return 0;
	}
	const AttrAddonVector& GetAttrAddon( int32_t nLevel, Job_t job ) const
	{
		const AttrAddonMap* pMap = NULL;
		switch ( job )
		{
		case PJ_ZHANSHI:	pMap = &m_mAttrAddon1;	break;
		case PJ_FASHI:		pMap = &m_mAttrAddon2;	break;
		case PJ_LONGQI:		pMap = &m_mAttrAddon3;	break;
		default:break;
		}
		AttrAddonMap::const_iterator iter = pMap->find( nLevel );
		if ( iter != pMap->end() )
		{
			return iter->second;
		}
		return m_empty;
	}

	int32_t GetLevelExp( int32_t nLevel ) const
	{
		Int32Int32Map::const_iterator iter = m_mLevelExp.find( nLevel );
		if ( iter != m_mLevelExp.end() )
		{
			return iter->second;
		}
		return -1;
	}

private:
	AttrAddonVector	m_empty;
	AttrAddonMap	m_mAttrAddon1;
	AttrAddonMap	m_mAttrAddon2;
	AttrAddonMap	m_mAttrAddon3;
	Int32Int32Map	m_mLevelExp;
	Int32Int32Map   m_Battle;
};


struct HallOfFameReward 
{
	int32_t nIndexMin;
	int32_t nIndexMax;
	int32_t nHonor;
	//ItemDataList items;
	MemChrBagVector items;
};
typedef std::list<HallOfFameReward> HallOfFameRewardList;

class CfgHallOfFameTable
{
public:
	CfgHallOfFameTable(){}
	~CfgHallOfFameTable(){}

public:
	void CleanUp()
	{
		m_rewards.clear();
	}

	void AddReward( const HallOfFameReward& reward )
	{
		m_rewards.push_back( reward );
	}

	const HallOfFameReward* GetReward( int32_t nIndex ) const
	{
		HallOfFameRewardList::const_iterator iter = m_rewards.begin();
		HallOfFameRewardList::const_iterator eiter = m_rewards.end();
		for ( ; iter != eiter; ++iter )
		{
			if ( nIndex >= iter->nIndexMin && nIndex <= iter->nIndexMax )
			{
				return &(*iter);
			}
		}
		return NULL;
	}

private:
	HallOfFameRewardList	m_rewards;
};
//wmf end

struct PkDropRate
{
	int32_t		PkValues;			//pkֵ
	int32_t		EquipCount;			//����װ����
	int32_t		EquipRate;			//װ���������
	int32_t		UsualCountRate;		//��ͨ��Ʒ�����������
	int32_t		UsualDropRate;		//��ͨ��Ʒ�������
	int32_t		SpecialCountRate;	//������Ʒ�����������
	int32_t		SpecialDropRate;	//������Ʒ�������
	int32_t		OverlayRate;		//�������������
	int32_t		MoneyRate;			//ͭǮ�������
};


struct QiangHuaWorthTable
{
	int32_t Worth[EQUIP_MAX_STAT];
};
typedef std::map<int8_t,QiangHuaWorthTable> QiangHuaWorthMap;

class CfgFaBao
{
public:
	CfgFaBao()
	{
		FaBaoLevel		= 0;
		NeedRes			= 0;
		m_AttrList.clear();
	}
	int32_t			FaBaoId;
	int32_t			NextFaBaoId;
	int32_t			FaBaoType;
	int32_t			FaBaoLevel;		
	int32_t			NeedRes;
	AddAttrList		m_AttrList;
};
typedef std::map<int32_t, CfgFaBao> FaBaoMap;

class FaBaoTable
{
public:
	FaBaoTable();
CfgFaBao*		GetFaBaoCfg( int32_t FaBaoId );
void			AddFaBao( CfgFaBao& Stu );
private:
	FaBaoMap	m_FaBaoTable;		//��ⷨ����
};

struct FunctionOpenCfg
{
	int32_t		Type;
	int32_t		TaskId;
	int32_t		Level;
};
typedef std::map<int32_t,FunctionOpenCfg> FunctionOpenCfgMap;

struct QiFuCfg
{
	int32_t GetMoney;
	int32_t	GetExp;
	int32_t CostGold;
	int8_t  CostItemClass;
	int32_t	CostItemId;
	int32_t CostItemCount;
	int32_t Rate;		// �������� (1-100)
};

typedef map<int8_t, QiFuCfg> QiFuTimesMap;		// <��������,����>
typedef map<int32_t, QiFuTimesMap> QiFuCfgMap;	// <�ȼ�,����> 

class CQiFuTable
{
public:
	CQiFuTable();
	void		InitQiFuTable();
	QiFuCfg*	GetQiFuCfg( int8_t QiFuType,int32_t Level, int8_t QiFuTimes );
private:
	QiFuCfgMap	m_QiFuMoneyCfg;
	QiFuCfgMap	m_QiFuExpCfg;
};

struct ScoreShopCfg
{
	int32_t		Index;			// �̵�����
	int32_t		PlayerLevel;	// �����ҵȼ�
	int8_t		itemClass;		// ��Ʒ����
	int32_t		itemId;			// ��ƷID
	int32_t		itemCount;		// ��Ʒ����
	int8_t		bind;			// �Ƿ��϶�
	int8_t		CostType;		// ���ѻ��ҵ�����
	int32_t		CostValue;		// ���ѽ��
	ItemDataList	CostItems;	// ���ѵ�����б�
	int8_t		LimitType;		// �������ͣ�1=ÿ�� 2=����
	int32_t		LimitCount;	// ���ƴ���
	int8_t		IsRecord;		// �Ƿ��¼
};
typedef std::map<int32_t, ScoreShopCfg>	ScoreShopCfgTable;

struct VipCfg
{
	int8_t		VipLevel;
	int8_t		QiFuMoneyTimes;		// ����ͭǮ�Ĵ���
	int8_t		QiFuExpTimes;		// ��������Ĵ���
	int8_t		Retroactive;		// ��ǩ�Ĵ���
	int32_t		NeedVipExp;			// ��Ҫvip����
	int32_t		ExpRate;			// ��־���ӳ�
	int32_t		OpenBag;			// �������ı���
	int32_t		DailyTaskTimes;		// �ճ��������
	MemChrBagVector Item;			// vip���
	AttrAddonVector	AtttVector;		// vip����
	int32_t		PetDeport;			// ���ӳ���ֿ�ĸ���
	int32_t		FamilyLightAddRate;	// ����֮��ӳ�
	int32_t		HallOfFameBuyTimes;	// �����ù������
	int32_t		SiteRevive;			// ÿ�츴�����
};
typedef map<int8_t, VipCfg> VipCfgMap;

struct VipCardCfg
{
	int8_t		VipCardId;
	int32_t		AddVipExp;
	int32_t		DailyAddExp;
	int32_t		AddVipTime;
	int32_t		NeedGold;
	int32_t		ExpRate;
	int32_t		AddPlayerExp;
	int32_t		AddPetExp;
	MemChrBagVector Items;
	AddAttrList AddAttr;
};

typedef map<int32_t,VipCardCfg> VipCardCfgMap;

class VipTable
{
public:
	VipTable();
	void		InitVipTable();
	VipCfg*		GetVipCfg( int8_t VipLevel );
	int8_t		GetVipLevel( int32_t VipExp );
private:
	VipCfgMap m_VipCfgMap;
};

// 7��Ͷ�ʽ����䱸
struct SevenTouZi
{
	int32_t		nId;			// ����
	int32_t		nType;			// 1=ʱ������, 2=�ȼ�����
	int32_t		nCondition;		// ��������
	MemChrBag	vItem;			// ������Ʒ
};
typedef std::map<int32_t, SevenTouZi> SevenTouZiMap;

// �¶�Ͷ�ʽ����䱸
struct MonthTouZi
{
	int32_t		nDay;			// �������
	MemChrBag	vItem;			// ������Ʒ
	int32_t		nGongGaoId;		// ���汾ID
};
typedef std::map<int32_t, MonthTouZi> MonthTouZiMap;

class CfgTouZiTable
{
public:
	CfgTouZiTable(){ m_SevenTouZiMap.clear(); m_MonthTouZiMap.clear(); }

	void					InitTouZiTable();

	const SevenTouZi*	GetSevenTouZi( int32_t nId ) const;
	const MonthTouZi*	GetMonthTouZi( int32_t nId ) const;
	bool				IsAllGetSevenDay( int32_t nRecord ) const;
	bool				IsAllGetMonthTouZi( int32_t nRecord ) const;
	const SevenTouZiMap&	GetSevenDayTable() const { return m_SevenTouZiMap; }
	const MonthTouZiMap&	GetMonthTable() const { return m_MonthTouZiMap; }

	void	AddSevenTouZi( const SevenTouZi& stu ) { m_SevenTouZiMap[stu.nId] = stu; }
	void	AddMonthTouZi( const MonthTouZi& stu ) { m_MonthTouZiMap[stu.nDay] = stu; }

private:
	SevenTouZiMap	m_SevenTouZiMap;
	MonthTouZiMap	m_MonthTouZiMap;
};

class CfgSysMail
{
public:
	CfgSysMail()
	{
		sender_name		= "";
		title			= "";
		content			= "";
		item.clear();
	}
	int32_t id;
	std::string sender_name;
	std::string title;
	std::string content;
	MemChrBagVector  item;
};
typedef std::map<int32_t,CfgSysMail>CfgSysMailTable;

struct BuyFaBaoResCfg
{
	int32_t	id;
	int8_t	FaBaoResType;
	int32_t	NeedGold;
	int32_t GetResValues;
};
typedef std::map<int32_t, BuyFaBaoResCfg> FaBaoResMap;

class BossInfo
{
public:
	BossInfo()
	{
		m_BossId				= 0;
		m_TransferPos.x			= 0;
		m_TransferPos.y			= 0;
		m_RevivePosVector.clear();
		m_IsShow				= 0;
		m_NeedResetReviveTime	= 0;
	}
	int32_t					m_BossId;
	Position				m_TransferPos;		// ��������
	std::vector<Position>	m_RevivePosVector;	// ���������б�
	int8_t					m_NeedResetReviveTime;
	int8_t					m_IsShow;
};
typedef std::map<int32_t, BossInfo> BossInfoMap;

struct CfgQuestions
{
	int32_t QuestionId;
	int8_t AnswerId;
};
typedef std::vector<CfgQuestions> QuestionsVector;

struct ChouJiangShop
{
	int32_t		Index;
	int32_t		ItemId;
	int8_t		ItemClass;
	int32_t		ItemCount;
	int32_t		NeedScore;
	int8_t		Type;
	int32_t		Limit;
	int8_t		Bind;
};
typedef std::map<int32_t,ChouJiangShop> CJShopMap;

struct ChouJiangCost
{
	int8_t	ChouJiangType;
	int32_t	CostGold;
	int32_t CostGold2;
	int8_t	CostItemClass;
	int32_t CostItemId;
	int32_t CostItemCount;
	int32_t	AddScore;
};

typedef std::map<int32_t,ChouJiangCost> CJCostMap;

struct ChouJiangCfg
{
	bool IsEmpty()
	{
		if ( ItemId == 0 || ItemCount == 0 )
		{
			return true;
		}
		return false;
	}
	int8_t	ChouJiangType;
	int8_t	ItemClass;
	int32_t	ItemId;
	int32_t ItemCount;
	int8_t	Quality;
	int8_t	Star;
	int32_t	Probability;
	int8_t  IsRecord;
	int8_t	IsGongGao;
};
typedef std::list<ChouJiangCfg> CJCfgList;

struct CfgKaiFuHuoDongData
{
	int32_t		Index;
	int8_t		Type;
	int32_t		StartDay;
	int32_t		EndDay;
	int32_t		GetRewardDay;
	int8_t		Conditions;
	int32_t		Parm1;
	int32_t		Parm2;
	int32_t		LimitCount;
	MemChrBagVector ItemVector;
};

typedef std::map<int32_t,CfgKaiFuHuoDongData>   KaiFuHuoDongCfg;

class ChouJiangConfig
{
public:
	ChouJiangConfig();
	void InitCJConfig();
	void InitShopMap();
	void InitCostMap();
	void InitCfgList();
	ChouJiangCost*  GetCJCost( int8_t ChouJiangType );
	ChouJiangShop*	GetCJShop( int8_t Index );
	ChouJiangCfg	GetCJCfg( int8_t ChouJiangType, bool IsSpecial = false );
	CJShopMap m_CJShopMap;
	CJCostMap m_CJCostMap;
	CJCfgList m_CJCfgList;
};

struct CfgBossHome
{
	int32_t		MapId;
	int32_t		Grade;
	int32_t		NeedVipLevel;
	int32_t		NeedGold;
	int32_t		IntervalTime;
	int32_t     NeedCash;
	ItemData	CostItem;
	std::vector<Position>	EnterPosVector;
};

typedef std::map<int32_t, CfgBossHome> BossHomeTable;

struct VipGuaJiMap
{
	int32_t			MapId;
	int32_t			StartTime;
	int32_t			EndTime;
	int32_t			VipLevel;
};
typedef std::map<int32_t,VipGuaJiMap> VipGuaJiMapTable;

struct CfgMoLingRuQinMapInfo
{
	int32_t			MapId;
	int32_t			NpcId;
	int32_t			NpcCount;
	std::vector<Position>	m_RevivePosVector;
};
typedef std::map<int32_t, CfgMoLingRuQinMapInfo> MoLingRuQinMap;

struct CurrencyStu
{
	int8_t		Type;
	int32_t		Values;
};
typedef std::list<CurrencyStu> CurrencyList;
struct CfgZiYuanZhaoHui
{
	int32_t				Index;				//id
	int32_t				Type;				//��Դ�һص�����
	int32_t				Times;				//�ܴ���
	int32_t				Id;					//�����ǻ ���ǻ������, �����Ǹ������� ����groupid
	int32_t				NeedMoney;			//�һ���Ҫ��ͭǮ
	int32_t				NeedGold;			//�һ���Ҫ��Ԫ��
	CurrencyList		GetCurrencyList;	//��õ���Դ
	int32_t				GetExpValues;		//��õľ���
};
typedef std::map<int32_t,CfgZiYuanZhaoHui> CfgZYZHMap;

struct CfgEquipExchange
{
	int8_t		Level;
	int8_t		CostType;
	int32_t		Money;
	int32_t		MoShi;
	int32_t		BindMoShi;
};
typedef std::map<int8_t,CfgEquipExchange> CfgEquipExchangeMap;

struct CfgEverydayChongZhi
{
	int8_t					Index;
	int32_t					NeedGold;
	MemChrEquipBagVector	ItemVector;
	GongGaoList				GongGaoInfo;
};
typedef std::map<int8_t,CfgEverydayChongZhi> EverydayChongZhiMap;

struct CfgTotalChongZhi
{
	int8_t					Index;
	int32_t					NeedGold;
	MemChrEquipBagVector	ItemVector;
	GongGaoList				GongGaoInfo;
	int32_t					NewServerday;
};
typedef std::map<int8_t,CfgTotalChongZhi> TotalChongZhiMap;

struct CfgHuoYueDu
{
	int32_t			Index;
	int8_t			Type;
	int32_t			Effect;
	int32_t			Count;
	int32_t			Gold;
	int32_t			AddHuoYueDu;
	bool			IsCanSec;					//�Ƿ������
};

typedef	std::map<int32_t,CfgHuoYueDu> HuoYueDuTable;

struct CfgHuoYueDuReward
{
	int32_t					Id;
	int32_t					NeedHuoYueDu;
	MemChrBagVector			Items;
};
typedef std::map<int8_t,CfgHuoYueDuReward> HuoYueDuRewardTable;

struct CfgGuanWei
{
	int32_t   GuanWei;
	int32_t	  NeedLevel;
	int32_t   NeedWeiWang;
	int32_t	  GetExp;
	int32_t   GetMoney;
	MemChrBagVector			Items;
	int32_t   MailId;
	int32_t	  Battle;
	AddAttrList Attrs;
};
typedef std::map<int32_t,CfgGuanWei> GuanWeiMap;

#define QI_SHI_COUNT 6					//�����ʿ�ĸ���
#define MAX_QI_SHI_QUALITY_SIZE 10		//���Ը���
struct CfgQiShi
{
	int32_t			QiShiPos;
	int32_t			NeedGuanWei;
	int32_t			AddBattle;
	int32_t			AttrType;
	int32_t			AttrType2;
	int32_t			vQuality[MAX_QI_SHI_QUALITY_SIZE];			// Ʒ��
	int32_t			vEffectValue[MAX_QI_SHI_QUALITY_SIZE];		// �ӳ�
	int32_t			vEffectValue2[MAX_QI_SHI_QUALITY_SIZE];		// �ӳ�
};
typedef std::map<int32_t,CfgQiShi> QiShiMap;

struct AchievementRequest 
{
	int32_t param1;
	int32_t param2;
	int32_t param3;
};

struct CfgAchievement
{
	int32_t				Index;
	int32_t				Socre;
	CurrencyList		ZhiYuan;
	int8_t				Group;
	AchievementRequest	Request;
};
typedef std::map<int32_t,CfgAchievement> AchievementTask;

class AchievementTable
{
public:
	AchievementTable(){ OnCleanUP(); }
	~AchievementTable(){};
	void	OnCleanUP();
	void    AddAchievementTask( CfgAchievement Stu );
	const CfgAchievement* GetAchievementTask( int32_t Index ) const;
	const AchievementTask GetAchievementTaskMap() const;
private:
	AchievementTask m_AchievementTaskMap;
};

struct CfgXunZhang
{
	int32_t		EquipId;
	int32_t		CostSocre;
	int32_t		NextEquipId;
};
typedef map<int32_t,CfgXunZhang> XunZhangMap;

struct CfgBuyAcSocre
{
	int32_t		Index;
	int8_t		CostType;
	int32_t		CostValues;
	int32_t		GetValues;
};
typedef map<int32_t, CfgBuyAcSocre> BuyAcSocerMap;

struct CfgHuanHuaNeedRoleLevel
{
	int32_t MinQualit;
	int32_t MaxQualit;
	int32_t NeedLevel;
};
typedef list<CfgHuanHuaNeedRoleLevel> HuanHuaNeedRoleLevelList;


class CfgData
{
public:
	CfgData();
	~CfgData();

public:
	bool init(int32_t equipIdInterval, int32_t viceGeneralIdInterval, int32_t lackeyInterval, int32_t debug);
	void reload();

public:
	const int32_t getServerStartTime();
	const int32_t getServerStartDayTime();
	const int32_t getServerDiffTime();

	const CfgActivityTable&	getAllActivity();
	CfgActivity*			getActivity(int32_t id);
	const CfgMapEventList&	getActivityEvents( int32_t activity_id, MapId_t nMapId );
	
	CfgActivityMonster*		getActivityMonster( int32_t id );
	CfgActivityNpc*			getActivityNpc(int32_t id);
	CfgActivityPlant*		getActivityPlant( int32_t Id );
	CfgActivityDropTable*	getActivityDrops();
	CfgActivityTrap*		getActivityTrap( int32_t id );

	const CfgAnnoucementTimeTable& getAllAnnoucementTime() const;

	CfgBuff* getBuff(int32_t id);

	const CfgDungeonTable& getDungeonAll();
	CfgDungeon* getDungeon(int32_t id);
	const CfgMapEventList& getDungeonEvent( int32_t dungonid );
	CfgDungeonMonster* getDungeonMonster(int32_t id);
	CfgDungeonPlant* getDungeonPlant(int32_t id);
	CfgDungeonTrap* getDungeonTrap(int32_t id);

	const CfgEquip* getEquip(int32_t id) const;
	CfgChrShop* getChrShop( int32_t index );

	CfgGamble* getGamble(int32_t ratio,int32_t type);
	CfgGambleEquip* getGambleEquip(int32_t id);

	CfgItemTable getAllItem();
	CfgItem* getItem(int32_t id);
	CfgItemGiftVector* getItemGift(int32_t id);
	CfgItemGiftRandomVector* getItemGiftRandom(int32_t id);

	CfgJob* getJob(int32_t id);
	bool getMovie(int32_t id);

	const CfgMapTable& getMapAll();
	CfgMap* getMap(int32_t id);
	Int32Vector* getFamilyBossActivityReward(int32_t level);

	CfgMapMonsterVector* getMonstersOnMap(int32_t mapid);
	CfgMapMonster* GetMapMonsterInfo( int32_t id );
	CfgMapPlantVector* getPlantOnMap(int32_t mapid);

	CfgMapRegion* getMapRegion(int32_t id);
	CfgMapRegionVector* getRegionOnMap(int32_t mapid);

	CfgMonster* getMonster(int32_t mid);
	bool isMonsterBroadcast(int32_t mid);
	CfgMonsterDropGroupVector* getMonsterDropGroup(int32_t group_id);
	CfgMonsterGroupDropVector* getMonsterGroupDrop(int32_t mid);
	CfgMonsterTaskDropVector* getMonsterTaskDrop(int32_t mid);
	const CfgDungeonDrop* randDungeonDrop( int32_t dungeonId );
	CfgDungeonReward* getDungeonReward(int32_t dungeonID);

	CfgBornAttr*  getBornAttr(Job_t jobindex);
	CfgChangeJobAttr* getChangeJobAttr(int32_t index);

	CfgNpc* getNpc(int32_t npcid);
	const CfgNpcTable& getNpcAll();
	CfgNpcAirport* getNpcAirport(int32_t id);
	CfgNpcKingdomShop* getNpcKingdomShop(int32_t id);

	CfgPlant* getPlant(int32_t id);

	CfgSkill* getSkill(int32_t id);
	CfgTask* getTask(int32_t id);


	CfgYellowStone* getYellowStone(int32_t id);
	PkDropRate*		GetPkDropRate( int32_t PkValues );
	int32_t getTaskDailyWindItem(int32_t tid, int32_t wind_point);


	CfgTrailer* getTrailer();
	CfgTrap* getTrap(int32_t id);
	CfgItemBase* getItemBase(int32_t id, int32_t type);
	std::string getItemName(int32_t id, int32_t type);
	int32_t getOverlay(int32_t id, int32_t itemClass);
	int32_t getInValue(int32_t id, int32_t type);
	int32_t getOutValue(int32_t id, int32_t type);
	bool canSell(int32_t id, int32_t itemClass);
	CfgLevelExp* getUpgradeExp(int32_t level);
	int64_t getNeedLevelExp(int32_t level);
	int64_t getMaxExp(int32_t level);
	int32_t	GetPetExp( int32_t level );
	CfgLevelAttr getLevelAttr(int32_t job,int32_t level);

	int32_t getBaseJob(int32_t job);
	bool isBanChat(int32_t uid, int32_t nowTime);

	AttrAddonVector getItemEffect(int32_t id);

	void onAvgLevelUpdated(int32_t avgLevel);
	AttrAddonVector parseItemEffect(int32_t id,const std::string &str);
	MemChrBagVector parseAllItemString(int32_t id, const std::string &strItems);

	int32_t getCreateTime();
	int32_t getDebug();

	void calcAchievementCount();
	void onBanChatUpdated(int32_t action, int32_t uid, int32_t expire_time);

	const CfgEquipTable&			GetEquipTable() const;
	const CfgItemGemTable&			GetItemGemTable() const;
	const CfgItemCombiTable&		GetItemCombiTable() const;
	const CfgBagSlotOpenTimeTable&	GetBagSlotOpenTimeTable() const;
	const CfgPetTable&				GetPetTable() const;
	const CfgPetEggTable&			GetPetEggTable() const;
	const CfgPetAttrInitRateTable&	GetPetAttrInitRateTable() const;
	const CfgPetIllusionTable&		GetPetIllusionTable() const;
	const CfgSkillLevelUpTable&		GetSkillLevelUpTable() const;
	const CfgTaskCycleTable&		GetTaskCycleTable() const;
	const CfgFamilyTask&			GetFamilyTaskTable() const;
	const CfgPetKnightTable&		GetPetKnightTable() const;
	const CfgFamilyPetRegistTable&	GetFamilyPetRegistTable() const;
	const CfgFamilyTable&			GetFamilyTable() const;
	const CfgCharPetTable&			GetCharPetTable() const;
	const CfgTeamTable&				GetTeamTable() const;
	const CfgInsidePetTable&		GetInsidePetTable() const;
	const CfgFamilyWarJoinRewardTable&	GetFamilyWarJoinRewardTable() const;
	const CfgFamilyLightExpTable&	GetFamilyLightExpTable() const;
	const CfgSoulAttrTable&			GetSoulAttrTable() const;
	const CfgHorseRacingRewardTable&	GetHorseRacingRewardTable() const;
	const CfgHallOfFameTable&		GetHallOfFameTable() const;
	const AchievementTable&			GetAchievementTable() const;
	int32_t						GetFriendExpByLevel( int32_t Level );
	int32_t						GetQiangHuaWorth( int8_t Quality, int8_t Star );
	CfgSysMail*					GetSysMail( int32_t Id );

	BossInfo*					GetBossInfo( int32_t BossId );
	BossInfoMap&				GetBossInfoMap();
	FaBaoTable&					GetFaBaoTable();
	CfgGameShop*				GetGameShop(int32_t nShopId );
	CfgGameShop*				GetGameShopItem( int8_t Class, int32_t Id );

	
	BuyFaBaoResCfg*				GetBuyFaBaoResCfg( int32_t Id );
	QuestionsVector				GetAllQuestions();	
	CfgPetGift*					GetPetCfg( int32_t nId );
	ChouJiangConfig&			GetChouJiangCfg();
	ScoreShopCfg*				GetScoreShopCfg( int32_t Index );
	CQiFuTable&					GetQiFuTable();
	VipTable&					GetVipTable();
	MemChrBagVector				GetSignReward( int8_t SiginCount );	
	CfgSignRewardTable&			GetSignRewardTable();
	CfgOnlineReward*			GetOnlineRewardCfg( int8_t id );
	CfgOnlineRewardTable&		 GetOnLineRewardTable();
	CfgSevenLoginRewrad*		GetSevenLoginRewardCfg( int8_t Day );
	CfgLevelGift*				GetLevelGiftCfg( int8_t Index );
	CfgLevelGiftTable&			GetLeveGiftTable();
	CfgWeekOnlineReward*		GetWeekOnlineReward( int32_t Week );
	CfgOffLineExp*				GetOfflineExpCfg( int32_t Level );
	VipCardCfg*					GetVipCardCfg( int8_t VipType );
	CfgBossHome*				GetBossHomeCfg( int32_t MapId );
	bool						IsBossHomeMap( int32_t MapId );
	VipGuaJiMap*				GetVipGuaJiMapCfg( int32_t MapId);
	bool						IsVipGuaJiMap( int32_t MapId );
	FunctionOpenCfgMap&			GetOpenFunctionTable();
	FunctionOpenCfg*			GetOpenFunctionCfg( int32_t FunctionId );
	CfgMoLingRuQinMapInfo*		GetMLRQMapInfo( int32_t MapId );
	CfgItemGiftVector*			getPetGift(int32_t id);
	CfgPlantEventEffect*		GetPlantEvent( int32_t EventId );
	CfgZYZHMap&					GetZYZHList();
	CfgZiYuanZhaoHui*			GetZiYuanZhaoHui( int32_t Index );
	CfgEquipExchange*			GetEquipExchange( int8_t Level );
	MemChrEquipBagVector&		GetShouChongLiBao();
	NewServerFavorable*			GetNewServerFavorable( int8_t Index );
	NewServerFavorableMap&		GetNewServerFavorableCfg();
	EverydayChongZhiMap&		GetEveryDayTable();
	CfgEverydayChongZhi*		GetEveryDayChongZhiCfg( int8_t Index );
	CfgTouZiTable&				GetTouZiTable();
	KaiFuHuoDongCfg&			GetKaiFuHuoDongCfg();
	CfgHuoYueDu*				GetHuoYueDuCfg( int32_t Index );
	HuoYueDuTable&				GetHuoYueDuTable();
	CfgHuoYueDuReward*			GetHuoYueDuReward( int32_t Index );
	HuoYueDuRewardTable&		GetHuoYueDuRewardTable();
	CfgGuanWei*					GetGuanWeiCfg( int32_t index );
	CfgQiShi*					GetQiShiCfg( int32_t index );
	AddAttrList					GetQiShiAttr( int32_t index, int32_t Quality  );
	AddAttrList					GetAppendAttr( int32_t Id, int8_t Job );
	NewServerFavorable&			GetThreePetGift();
	FamilyTaskReward*			GetFamilyReward( int32_t Index );
	CfgXunZhang*				GetXunZhangCfg( int32_t EquipId );
	CfgBuyAcSocre*				GetBuyAcScoreCfg( int32_t Index );
	TotalChongZhiMap&			GetTotalChongZhiTable();
	CfgTotalChongZhi*			GetTotalChongZhiCfg( int8_t Index );
	int32_t						GetHuanHuaNeedRoleLevel( int32_t Points );
	WarVictoryHd*				GetWarVictoryHdCfg( int8_t Index );
private:
	void getExParams(cfgParams& params,const std::string& str);

	void fetchcfExActivity();
	//void fetchKingdom();
	void fetchActivity();
	void fetchGmtBanChat();
	void fetchAnnoucementTime();
	void fetchBuff();
	void fetchDungeon();
	void fetchDungeonEvent();
	void fetchDungeonMonster();
	void fetchDungeonPlant();
	void fetchDungeonTrap();
	void fetchFamily();
	void fetchChrShop();
	void fetchItem(bool bSend);
	void fetchJob();
	void fetchMovie();
	void fetchLevelExp();
	void fetchLevelAttr();
	void fetchBuleprint();
	void fetchMap();							//��ȡ��ͼ��Ϣ��
	void fetchMapMonster();			
	void fetchMapPlant();						
	void fetchMapRegion();						
	void fetchMonster();
	void fetchMonsterDropGroup();
	void fetchMonsterGroupDrop();
	void fetchMonsterTaskDrop();
	void fetchNpc();
	void fetchGamble();
	void fetchBornAttr();
	void fetchPlant();
	void fetchSkill();
	void fetchTask();
	void fetchTrailer();
	void fetchTrap();
	void fetchFormation();
	void fetchSignReward();
	void fetchYellowStone();

	ChangeJobIndexVector parseChangeJobEffect(int32_t id, const std::string &str);
	FamilyRegionVector parseFamilyRegionEffect(int32_t id, const std::string &str);
	AttrAddonVector parseEquipEffect(int32_t id, const std::string &str);
	AttrAddonVector parseEffect(int32_t id, const std::string &str);
	Int32Vector parseShejituEffect(int32_t id,const std::string &str);
	Int32Vector parseInt32VectorString(int32_t id,const std::string &str);
	MemChrEquipBagVector parseEquipItemString(int32_t id, const std::string &strItems);
	MemChrBagVector parseTaskItemString(int32_t id, const std::string &strItems);
	void parseItemStringWithJob(int32_t id,const std::string &strItems,MemChrBagVector &items,MemChrJobBagVector&jobItems);
	MemChrBagVector parseItemString(int32_t id, const std::string &strItems);
	MemChrBag parseDailyPkString(int32_t id, const std::string &strItems);
	MemChrJobBagVector parseTaskItemJobString(int32_t id, const std::string &strItems);
	GongGaoList parseGongGaoString(int32_t id, const std::string &strItems);
	void parseMonsterSkill(int32_t id, MonsterSkill (&vSkill)[MAX_MONSTER_SKILL], const std::string &strSkill);
	TaskRequest parseTaskCondition(int32_t id, int32_t condition, const std::string &strRequest);
	MemJobItemTable parseGambleEquip(int32_t id,const std::string &strItems);
	std::vector<Position> paresPosition( const std::string &strPos );
	Int32Vector parseAchievementString(int32_t id,const std::string &str);
    AcExchangeItemVector paresAcExchangeString(int32_t id,const std::string &str);
	AttrAddonVector parasePetAttrAddon( const std::string& addonAttr );
	CurrencyList	paraseCurrency( const std::string& str );
	AchievementRequest parseAchievementCondition(int32_t id, int32_t condition, const std::string &strRequest);
	void parasItemData( ItemData& data, const std::string& str );
	void parasItemData( MemChrBag& data, const std::string& str );
	void parasKaiFuHuoDongCondition( std::string& Effect, int32_t Conditions, int32_t& Param1, int32_t& Param2 );
	void fetchServerConfig();
	void sendNewItems(const CfgItemTable &items);

	// ��ʼ��װ����
	void InitEquipTable();
	void InitEquipUpGradeTable();
	void InitEquipUpQualityTable();
	void InitEquipUpStarTable();
	void InitEquipAddAttrTable();
	void InitEquipGoalTable();
	void InitEquipSuitTable();
	void InitWuHunHoleTable();				// ��ʯ�׶�Ӧ���
	void InitWuHunMoHunTable();				// ǿ���ȼ���Ӧ���

	void InitItemGemTable();				// ��ʼ����ʯ��
	void InitItemCombiTable();				// ��ʼ���ϳɱ�
	void InitBagSlotOpenTimeTable();		// ��ʼ����������ʱ���
	void InitPlayerInitPetTable();			// ��ʼ������Я�������
	void InitPetPackageTable();				// ��ʼ����������
	void InitPetTable();					// ��ʼ�����ޱ�
	void InitPetEggTable();					// ��ʼ�����޵���
	void InitPetAttrInitRateTable();		// ��ʼ���������԰ٷֱȶ�Ӧ��
	void InitPetIllusionTable();			// ��ʼ�����޻û���
	void InitPetKnightTable();				// ��ʼ��������ʿ��
	void InitSkillLevelUpTable();			// ��ʼ������������
	void InitTaskCycleStarTable();			// ��ʼ��ѭ�������
	void InitFamilyTable();					// ��ʼ�����ű�
	void InitFamilyPositionTable();			// ����ְλ��
	void InitFamilyPetRegistTable();		// ��ʼ������ǼǱ�
	void InitCharPetHatchPoolTable();		// ��ʼ��������������ñ�
	void InitPetLucyItemTable();			// ��ʼ����������ֵ�û����߱�
	void InitTeamBuffTable();				// ��ʼ�����Buff��
	void InitInsidePetTable();				// ��ʼ�����ޱ�
	void InitInsidePetExpItemTable();		// ��ʼ�����޿����ɵ��߱�
	void InitFamilyWarJoinRewardTable();	// ��ʼ������ս���뽱����
	void InitFamilyLightExpTable();			// ��ʼ������֮�⾭�齱����
	void InitSoulAttrTable();				// ��ʼ��������
	void InitHorseRacingRewardTable();		// ��ʼ��������Խ�����
	void InitHallOfFameRewardTable();		// ��������������

	void InitFriendExpTable();				//��ʼ�����Ѿ����

	void InitPkDropRateTable();				//��ʼ��pk�������
	void InitQiangHuaWorthTable();			//��ʼ��ǿ����ֵ��
	void InitSysMail();						//��ʼ��ϵͳ�ʼ�
	void InitBossInfo();					//��ʼ��boss��Ϣ
	void InitFaBaoTable();					//��ʼ��������Ϣ��
	void InitBuyFaBaoResTable();			//��ʼ�����򷨱���Դ��
	void InitDaTiHD();						//��ʼ������
	void InitShangChengTable();				//��ʼ���̳Ǳ�
	void InitPetGiftTable();				//��ʼ�����������
	void InitOnLimeReward();				//��ʼ������ʱ��
	void InitSevenLoginReward();			//��ʼ�������½����
	void InitLevelGift();					//��ʼ���ȼ����
	void InitWeekOnlineReward();			//��ʼ�����߽������
	void InitOffLineExpTable();				//��ʼ�����߾����
	void InitVipCardTable();				//��ʼ��vipcard
	void InitBossHomeTable();				//��ʼ��boss֮�ұ�
	void InitVipGuaJiMap();					//��ʼ��vip�һ���ͼ
	void InitFunctionOpenTable();			//��ʼ�����ܿ���
	void InitMoLingRuQinMap();				//��ʼ��ħ�����ֵ�ͼ
	void InitAutoPetGfit();					//��ʼ���Զ�ʹ�õĳ������
	void InitPlantEventTable();				//��ʼ���ɼ����¼���
	void InitZiYuanZhaoHuiTable();			//��ʼ����Դ�һر�
	void InitMoHuaHuanYiTable();			//��ʼ��ħ��ת�Ʊ�
	void InitShouChongLiBao();				//��ʼ���׳����
	void InitNewServerFavorable();			//�·��ػ����
	void InitEveryDayChongZhi();			//��ʼ��ÿ�ճ�ֵ
	void InitKaiFuHuoDongTable();			//��ʼ�����
	void InitHuoYueDuTable();				//��ʼ����Ծ�ȱ�
	void InitGuangWeiTable();				//��ʼ����λ��
	void InitAppendAttrTable();				//��ʼ���������Ա�
	void InitwuHunExpTable();				//��ʼ����꾭���
	void InitPetBuyTable();					//��ʼ���������û�
	void InitFamilyRewardTable();			//��ʼ�����Ž�����
	void InitAchievementTable();			//��ʼ���ɾͱ�
	void InitXunZhangTable();				//��ʼ��ѫ�±�
	void InitBuyAcSocreTable();				//��ʼ������ɾͻ���
	void InitTotalChongZhiTable();			//�����ۼƳ�ֵ
	void InitHuanHuaNeedRoleLevelTable();	//��ʼ���û��ȼ������
	void InitScoreShopTable();					// ��ʼ��������̳�

	void InitWarVictoryTable();				//��ʼ����սʤ��������
private:
	CfgMapEventList	m_emptyEvents;
	CfgActivityTable m_activities;
	CfgMapEventTable m_activityEvents;
	CfgActivityMonsterTable m_activityMonsters;
	CfgActivityNpcTable m_activityNpcs;
	CfgActivityPlantTable m_activityPlants;
	CfgActivityDropTable m_activityDrops;
	CfgActivityTrapTable m_activityTraps;

	CfgAnnoucementTimeTable m_annoucementTimes;
	CfgBuffTable m_buffs;

	CfgDungeonTable m_dungeons;
	CfgDungeonRewardTable m_dungeonReward;
	CfgDungeonDropGroupTable  m_dungeonDropGroup;
	CfgMapEventMap m_dungeonEvents;
	CfgDungeonMonsterTable m_dungeonMonsters;
	CfgDungeonPlantTable m_dungeonPlants;
	CfgDungeonTrapTable m_dungeonTraps;

	GmtBanChatTable m_banChats;
	Answer::Mutex m_banChatsLock;

	CfgItemTable m_items;
	Answer::RwLock m_itemsLock;

	CfgItemGiftTable m_itemGifts;
	Answer::RwLock m_itemGiftsLock;

	CfgItemGiftRandomTable m_itemGiftRandoms;
	Answer::RwLock m_itemGiftRandomsLock;

	CfgMountEquipTable m_mountEquip;
	//CfgKingdomTable m_kingdoms;
	CfgJobTable m_jobs;
	CfgMovieTable m_movie;
	CfgLevelExpTable m_levelExps;
	CfgMapTable m_maps;
	CfgMapMonsterTable m_mapMonsters;
	CfgMonsterMap	   m_CfgMapMonsters;

	CfgMapPlantTable m_mapPlants;
	CfgMapRegionTable m_mapRegions;
	Int32CfgMapRegionVectorMap m_mapRegionsByMapId;
	CfgMonsterTable m_monsters;
	Int32Vector m_monsterBroadcasts;
	CfgMonsterDropGroupTable m_monsterDropGroups;
	CfgMonsterGroupDropTable m_monsterGroupDrops;
	CfgMonsterTaskDropTable m_monsterTaskDrops;
	CfgNpcTable m_npcs;
	CfgBornAttrTable m_bornAttr;
	CfgChangeJobAttrTable m_changeAttr;
	CfgNpcAirportTable m_npcAirports;
	CfgChrShopTable m_chrShops;
	CfgPlantTable m_plants;
	CfgSkillTable m_skills;
	CfgTaskTable m_tasks;
	CfgTrailer m_tailers;
	CfgTrapTable m_traps;
	CfgLevelAttrTable m_levelAttrs;
	CfgGambleVector  m_gamble;
	CfgGambleVector  m_gamble_second;
	CfgGambleVector  m_gamble_three;
	CfgGambleEquipTable m_gambleEquip;
	CfgFamilyLevelTable  m_familyLevel;
	CfgYellowStoneTables m_yellowStone;

	CfgEquipTable			m_cfgEquip;					// װ����
	CfgItemGemTable			m_cfgItemGem;				// ��ʯ��
	CfgItemCombiTable		m_cfgItemCombi;				// �ϳɱ�
	CfgBagSlotOpenTimeTable	m_cfgBagSlotOpenTime;		// ��������ʱ���
	CfgPetTable				m_cfgPetTable;				// ���ޱ�
	CfgPetEggTable			m_cfgPetEggTable;			// ���޵���
	CfgPetAttrInitRateTable	m_cfgPetAttrInitRateTable;	// ���޳�ʼ�����԰ٷֱȶ�Ӧ��
	CfgPetIllusionTable		m_cfgPetIllusionTable;		// ���޻û���
	CfgPetKnightTable		m_cfgPetKnightTable;		// ������ʿ��
	CfgSkillLevelUpTable	m_cfgSkillLevelUpTable;		// ����������
	CfgTaskCycleTable		m_cfgTaskCycleTable;		// ѭ�������
	CfgFamilyTable			m_cfgFamilyTable;			// ���ű�
	CfgFamilyPetRegistTable	m_cfgFamilyPetRegistTable;	// ����ǼǱ�
	CfgCharPetTable			m_cfgCharPetTable;			// ��ҳ���ϵͳ���ñ�
	CfgTeamTable			m_cfgTeamTable;				// �����Ϣ��
	CfgInsidePetTable		m_cfgInsidePetTable;		// �������ñ�
	CfgFamilyWarJoinRewardTable	m_cfgFamilyWarJoinRewardTable;	// ����ս���뽱��
	CfgFamilyLightExpTable	m_cfgFamilyLightExpTable;	// ����֮�⾭�齱��
	CfgSoulAttrTable		m_cfgSoulAttrTable;			// �������Ա�
	CfgHorseRacingRewardTable	m_cfgHorseRacingRewardTable;	// ������Խ���
	CfgHallOfFameTable		m_cfgHallOfFameTable;		// �����ñ�

	cfgExActivitys m_cfgExActivitys;
	Answer::RwLock m_exActivityLock;

	int32_t m_avgLevel;
	int32_t m_createTime;
	int32_t m_debug;
	int32_t m_startServerTime;

	CfgFriendExpTable m_FriendExpTable;
	std::list<PkDropRate> m_PkDropRateTable;
	QiangHuaWorthMap	  m_QiangHuaWorthTable;
	CfgSysMailTable		  m_sysMail;
	BossInfoMap			  m_BossInfo;
	FaBaoTable			  m_FaBaoTable;
	FaBaoResMap			  m_FaBaoResMap;
	QuestionsVector		  m_QuestionsVct;
	GameShopMap			  m_GameShopMap;
	PetGiftMap			  m_PetGiftTable;
	ChouJiangConfig		  m_ChouJiangConfig;
	ScoreShopCfgTable	  m_ScoreShopCfgTable;
	CQiFuTable			  m_QiFuTable;
	VipTable			  m_VipTable;
	CfgSignRewardTable    m_signReward;
	CfgOnlineRewardTable  m_OnLineReward;
	CfgSevenLoginRewradTable m_SevenLoginReward;
	CfgLevelGiftTable		 m_LevelGift;
	CfgWeekOnlineRewardTable m_WeekOnlineReward;
	CfgOffLineExpTable		 m_OffLineExpTable;
	VipCardCfgMap			 m_VipCardTable;
	BossHomeTable			 m_BossHomeTable;
	VipGuaJiMapTable		 m_VipGuaJiMap;
	FunctionOpenCfgMap		 m_FunctionOpenCfg;
	MoLingRuQinMap			 m_MoLingRuQinMap;
	CfgItemGiftTable		 m_PetGifts;
	PlantEventMap			 m_PlantEventMap;
	CfgZYZHMap				 m_ZiYuanZhaoHuiTable;	
	CfgEquipExchangeMap		 m_CfgEquipExchangeMap;
	MemChrEquipBagVector	 m_ShouChongLiBao;
	NewServerFavorableMap	 m_NewServerFavorable;
	NewServerFavorable		 m_ThreePetGift;
	EverydayChongZhiMap		 m_EveryDayChongZhiTable;
	CfgTouZiTable			 m_TouZiTable;				// Ͷ�������
	KaiFuHuoDongCfg			 m_KaiFuHuoDongTable;
	HuoYueDuTable			 m_HuoYueDuTable;
	HuoYueDuRewardTable		 m_HuoYueDuRewardTable;
	GuanWeiMap				 m_GuanWeiCfg;
	QiShiMap				 m_QiShiCfg;
	AppendAttrTable			 m_AppendAttrTable;
	CfgFamilyTask			 m_FamilyTaskTable;
	FamilyTaskRewardMap		 m_FamilyTaskReward;
	AchievementTable		 m_AchievementTable;
	XunZhangMap				 m_XunZhangMap;
	BuyAcSocerMap			 m_BuyAcSocreMap;
	TotalChongZhiMap		 m_TotalChongZhiTable;
	HuanHuaNeedRoleLevelList m_HuanHuaNeedRoleLevelList;
	WarVictoryHdMap			 m_WarVictoryHdMap;
};
#define CFG_DATA Answer::Singleton<CfgData>::instance()


