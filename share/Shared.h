#pragma once

#include "Attribute.h"
#include "PDUDefine.h"
#include "Type.h"
#include "stdafx.h"
#include <vector>
#include <sstream>
#include <set>
#define MAX_CONNECTION 10000
#define MAX_SQL_LENGTH 4096

/*
* һ��������100w����ɫ
* 32λ�ֶο��Կ�2100����
* �����������CharId�ĳ�UInt������64λ
*/
#define MAX_SERVER_CHAR_ID	1000000

#define GAME_SERVICE_LINE_SOCIAL 1

enum ErrorCode
{
	ERR_OK = 0,
	ERR_LEAVE_KINGDOM,
	ERR_SYETEM_ERR,
	ERR_FOR_GAME_CLIENT_BASE = 10001,
	ERR_INVALID_DATA,
	ERR_WRONG_PASSWORD,
	ERR_NAME_EXIST,
	ERR_GENERAL_CD,
	ERR_ALREADY_IN_CHANT,
	ERR_TARGET_NOT_EXIST,
	ERR_NOT_ENOUGH_HP,
	ERR_NOT_ENOUGH_MP,
	ERR_NOT_ENOUGH_MONEY,
	ERR_NOT_ENOUGH_JUNGONG,
	ERR_NOT_ENOUGH_WUHUEN,
	ERR_LEVEL_TOO_LOW,
	ERR_LEVEL_TOO_HIGH,
	ERR_DISTANCE_TOO_LONG,
	ERR_BAG_IS_FULL,
	ERR_BAD_LUCK,
	ERR_IN_DEAL,
	ERR_ALREADY_IN_GATHER,
	ERR_BREAK,
	ERR_NOT_ENOUGH_LEFT_AMOUNT,
	ERR_DUNGEON_PLAYER_LEVEL,
	ERR_DUNGEON_DAILY_COUNT,
	ERR_PLAYER_NOT_EXIST,
	ERR_SKILL_CD,
	ERR_NOT_ENOUGH_ANGRY,
	ERR_SWITCH_MAP_WITH_TRAILER,
	ERR_NPC_ACQUIRE_LIMIT,
	ERR_NPC_DONATE_LIMIT,
	ERR_ITEM_CD,
	ERR_LACK_OF_FAMILY_MONEY,
	ERR_FAMILY_LEVEL_LIMIT,
	ERR_FAMILY_TREE_LIMIT,
	ERR_FAMILY_LEVEL_LOW,
	ERR_FAMILY_ALREADY_APPLIED,
	ERR_FAMILY_POSITION_LIMIT,
	ERR_FAMILY_ACTIVITY_LIMIT,
	ERR_DELETE_CHR_AS_FAMILY_LEADER,
	ERR_GM_BAN_LOGIN,
	ERR_GM_BAN_CHAT,
	ERR_WRONG_VERSION,
	ERR_EXCHANGE_CODE_USED,
	ERR_EXCHANGE_CODE_NOT,
	ERR_EXCHANGE_CODE_SAME,
	ERR_TREASURE_STORE_IS_FULL,
	ERR_EXP_GAIN_LIMIT,
	ERR_AUCTION_ITEM_DELETED,
	ERR_AUCTION_ITEM_EXPIRED,
	ERR_AUCTION_ITEM_SELLED,
	ERR_AUCTION_ITEM_IN_SELL,
	ERR_PLAYER_OFF_LINE,
	ERR_EXCHANGE_CODE_GOLD_LIMIT,
	ERR_TOAST_COUNT_LIMIT,
	ERR_DELAY_EXCHANGE_SLOT_NEED,
	ERR_DUNGEON_TRAILER,
	ERR_MAP_PLAYER_LEVEL,
	ERR_MAP_TEAM_MEMBER_COUNT,
	ERR_MAP_ACTIVITY_NOT_STARTED,
	ERR_MAP_ACTIVITY_ENTRY_CLOSED,
	ERR_MAP_ACTIVITY_ENTRY_KING,
	ERR_MAP_CANNOT_REVIVE,
	ERR_MAP_ACTIVITY_OTHER_LINE,
	ERR_MAP_CHALLENGE_TIME_END,
	ERR_MAP_FAMILY_NEED,
	ERR_MAP_FAMILY_ATTACK,
	ERR_MAP_CONNOT_GO,
	ERR_FLAG_CANNOT_ATTACK,
	ERR_STRATEGICE_READ_NOT,
	ERR_NOT_ENOUGH_STUFF,
	ERR_TASK_FAIL,
	ERR_APPLY_COUNT_FULL,
	ERR_CHANGE_JOB_FAIL,
	ERR_MOUNT_INHERITANCE_FAIL,
	ERR_MOUNT_FORM_EXIST,
	ERR_MOUNT_FORM_GRADE,
	ERR_MOUNT_FORM_EXCHANGE,
	ERR_NOT_ENOUGH_GOLD,
	ERR_MAP_ACTIVITY_NOTADD_FAMILY,
	ERR_MAP_ACTIVITY_PKIN,
	ERR_MAP_ACTIVITY_NOTADD_KINGDOM,
	ERR_MAP_ACTIVITY_INPLAY,
	ERR_MAP_ACTIVITY_INDUM,
	ERR_MAP_ACTIVITY_NOTMAP,
	ERR_MAP_ACTIVITY_NOTREG,
	ERR_MAP_ACTIVITY_LEVEL,
	ERR_SKILL_MAXLEVEL,
	ERR_MAIL_FULL,
	ERR_RELATION_BAD,
	ERR_IS_BEING_ACTIVE,
	ERR_ZHAOXIAN_FAIL,
	ERR_FIRST_COST_Q,
	ERR_TREASON_KINGDOM,
	ERR_TEAM_DUNGEON_FAIL,
	ERR_VICEGENERAL_UPGRADE,
	ERR_MOUNT_UPGRADE,
	ERR_POST_UP,
	ERR_ACTIVITY_DATA_EMPTY,
	ERR_DUNGEON_TIME_OUT,
	ERR_EXACTVITY_HSAGET,
	ERR_FAMILY_ACTIVITY_SUCCESS,
	ERR_FAMILY_NOT_LEVEL,
	ERR_FAMILY_NOT,
	ERR_MOUNT_XISUI,
	ERR_VICEGENERAL_GENGU,
	ERR_GET_SEVEN_LOGIN,
	ERR_DUNGEON_COMMON = 10106,//δͨ����ͨ��������Ӧħ���������ܽ���
	ERR_READ_STRATEGICS_FAIL = 10107,//�����Ķ�ʧ��
};

enum ExActivityCode
{
	EXACTIVITY_MIN =0,
	EXACTIVITY_CHARGE,//��ֵ�
	EXACTIVITY_YUANBAOUSE, //����Ԫ��
	EXACTIVITY_IET_XINGYUNQUAN,//�����س�
	EXACTIVITY_IET_SUPER_XINGYUNQUAN, //�����ű�
	EXACTIVITY_IET_MOJIAYAOSHI, //ī�ұ���
	EXACTIVITY_VIP,//�����ȼ��
	EXACTIVITY_SAVEDB_REALTIME = 25,
	EXACTIVITY_XISUIDAN,//�´μ����µĻ��ʱ���������Ҫ��ʱд�����ݿ����ŵ�XISUIDAN֮ǰ
	EXACTIVITY_GENGUDAN,
	EXACTIVITY_XIUWEIDAN,
	EXACTIVITY_VICEGERNERALJINJIEDAN,
	EXACTIVITY_ZUOJIJINJIEDAN,
	EXACTIVITY_BINGFA,
	EXACTIVITY_END,
	EXACTIVITY_MAX =50,
	MOUNT_UPGRADE,
	VICEGERNERAL_UPGRADE,
	EQUIP_ENHENCE,
};

enum FamilyWarEvent
{
	FWE_ATTACK = 1,
	FWE_DEFENSE = 2,
};

enum MoneyCostType
{
	MCT_EQUIP_ENHANCE = 1,//װ��ǿ��
};

enum LogoutReason
{
	LR_DISCONNECT = 1,
	LR_KICK_SELF = 2,
	LR_SWITCH_LINE = 3,
	LR_KICK_BY_GM = 4,
	LR_SAVE_INTERVAL = 5,
};

enum updateFamilyInfoReason
{
	UFIR_NONE,
	UFIR_CREATE,
	UFIR_LEAVE,
	UFIR_JOIN,
};

enum ChatChannel
{
	CC_WORLD = 1,
	CC_KINGDOM = 2,
	CC_FAMILY = 3,
	CC_TEAM = 4,
	CC_AREA = 5,
	CC_COLOR_WORLD = 6,
};

enum TaskType /*10��ʼΪ֧������*/
{
	TT_TRUNK	= 1,	// ��������
	TT_CYCLE	= 3,	// ѭ������
	TT_FAMILY	= 37,	// ��������
};

enum TaskCondition
{
	TC_RECEIVE				= 1,		//�Ի�
	TC_ITEM					= 2,		//��Ҫ��Ʒ
	TC_USE_ITEM				= 3,		//ʹ����Ʒ
	TC_MONSTER_ID			= 4,		//ɱ��ָ������
	TC_MONSTER_LEVEL		= 5,		//ɱ��ָ���ȼ�����					
	TC_PLAYER_LEVEL			= 6,		//�ȼ��ﵽ

	TC_PET_ILLUSION			= 7,		//����û�
	TC_UP_EQUIP_STAR		= 8,		//װ��ǿ��
	TC_UP_EQUIP_GRADE		= 9,		//װ������
	TC_UP_EQUIP_QUALITY		= 10,		//װ����Ʒ
	TC_FINISH_TASK_TYPE		= 11,		//���ָ����������
	TC_PROTECT_BY_CLIENT	= 12,		//�ͻ�������
	TC_PLANT				= 13,		//�ɼ�����	
	TC_DUNGEON				= 14,		//��ɸ���
	TC_USER_SKILL			= 16,		//ʹ��ָ������
	TC_HALL_OF_FAME			= 17,		//���ָ��������������
	TC_DRESS_EQUIP			= 18,		//��������������װ��
	TC_EQUIP_STAR_COUNT		= 19,		//ǿ��xx��xx��
	TC_PET_POINTS_COUNT		= 20,		//ӵ�м������ǵĳ���
	TC_KILL_BOSS_COUNT		= 21,		//ɱ��boss
};

enum AchievementType
{
	AT_TEAM						= 1,	//���
	AT_FRIEND					= 2,	//����
	AT_FAMILY					= 3,	//����
	AT_EQUIP_UP_STAR			= 4,	//ǿ��װ��
	AT_PET_ILLUSION				= 5,	//����û�
	AT_LOGIN					= 6,	//��½
	AT_LEVEL					= 7,	//�ȼ�
	AT_THREE_PET				= 8,	//����
	AT_PET_POINTS_COUNT			= 9,	//ӵ�м������ǳ���
	AT_PET_KNIGHT				= 10,	//��������
	AT_DRESS_EQUIP				= 11,	//��������������װ��
	AT_KILL_MONSTER_BY_MID		= 12,	//ɱָ����
	AT_KILL_BOSS				= 13,	//ɱboss
	AT_KILL_MONSSTER			= 14,	//ɱ��
	AT_DUNGEON					= 15,	//��ɸ���
	AT_GUAN_WEI					= 16,	//��
	AT_FA_BAO					= 17,	//����
	AT_INSID_PET_POINTS			= 18,	//���ó���������
	AT_WU_LING_POINTS			= 19,	//��������
	AT_HUAN_LING_POINTS			= 20,	//��������
	AT_LUCK_POINTS				= 21,	//����ֵ
	AT_ACTIVITY					= 22,	//�μӻ
	AT_QI_FU_EXP				= 23,	//��������
	AT_QI_FU_MONEY				= 24,	//�������
	AT_CHOU_JIANG				= 25,	//�齱
	AT_JUE_WEI					= 26,	//��λ
	AT_HALL_FAME				= 27,	//��ս����			
};

#define WU_LING_RATE	100

// ����״̬
enum TaskState
{
	TS_UNKNOWN		= 0,
	TS_UNRECEIVABLE	= 1,		// ���ɽ�
	TS_CAN_RECEIVE	= 2,		// �ɽ�
	TS_DOING		= 3,		// ������
	TS_CAN_SUBMIT	= 4,		// �����
	TS_SUBMITED		= 5,		// ���ύ
};

enum DungeonType
{
	DT_TASK			= 1,		// ���߸���
	DT_ACHIEVEMENT	= 5,		// �ɾ͸���
	DT_EQUIP		= 11,		// װ������
	DT_TEAM			= 12,		// ��Ӹ���
	DT_BOSS			= 13,		// boss����
	DT_VIP			= 14,		// VIP���� �ػ�
	DT_TD			= 15,		// ���鸱�� ����
	DT_UP_TOWER		= 16,		// ��������
	DT_MY_WORLD		= 17,		// ħ�����縱��
	DT_MLRQ			= 18,		// ħ������
	DT_MONEY		= 19,		// ��Ǯ���� ��XP
	DT_PET			= 20,		// ���޸���
};

enum ITEM_CLASS
{
	IC_NORMAL	= 1,		// ��ͨ����
	IC_EQUIP	= 2,		// װ��
	IC_GEM		= 3,		// ��ʯ
	IC_CURRENCY = 4,		// ����
	IC_PET_EGG	= 5,		// ���޵�
	IC_PET		= 6,		// ����			
};

enum CURRENCY_ITEM_ID
{
	CURRENCY_MONEY_ID			= 1,		// ͭǮ
	CURRENCY_GOLD_ID			= 2,		// Ԫ��
	CURRENCY_CASH_ID			= 3,		// ��Ԫ
	CURRENCY_VIGOUR_ID			= 4,		// Ԫ��
	CURRENCY_HORNOR_ID			= 5,		// ����
};

enum ItemQuality
{
	IQ_WHITE	= 1,	// ��ͨ
	IQ_GREEN	= 2,	// ��Ʒ
	IQ_BLUE		= 3,	// ��Ʒ
	IQ_PURPLE	= 4,	// ��Ʒ
	IQ_GOLD		= 5,	// ��Ʒ
};

// ����
enum SKILL_ATTACK_TYPE
{
	SAT_PATTACK = 1,
	SAT_MATTACK = 2,
};

enum SKILL_TYPE
{
	SK_ACTIVE	= 1,
	SK_PASSIVE	= 2,
};

enum SKILL_KIND
{
	SK_NORMAL		= 1,	// ��ͨ����
	SK_XP			= 2,	// XP����
	SK_PUSH			= 3,	// ���˼���
	SK_HORSERACING	= 4,	// ��������
};

enum SKILL_RANGE
{
	SR_SELF			= 0,	// ����
	SR_SOLO			= 1,	// ����
	SR_LINE			= 2,	// ֱ��
	SR_AROUND		= 3,	// Բ��
	SR_HALF_ROUND	= 4,	// ��Բ

	SR_TEAM			= 10,	// С��
};

enum SKILL_TARGET_POS
{
	STP_TARGET	= 0,	// Ŀ�굥λ
	STP_SELF	= 1,	// �Լ�
	STP_POS		= 2,	// Ŀ��λ��
};


enum KingdomId
{
	KI_SHU	=1,
	KI_WU =2,
	KI_NEUTRAL = 3,			//����
	KI_HOME = 99,
};

enum RankType
{
	RT_MIN = 0,
	RT_LEVEL = 1,
	RT_FIGHT_POWER = 2,
	RT_WEALTH = 3,
	RT_SHENGWANG = 4,
	//5ԭ��������
	RT_SINGLE_CONTEST = 6,
	RT_FAMILY_CONTEST = 7,
	RT_LACKEY_LEVEL = 8,
	RT_ENHANCE_LEVEL = 9,
	RT_TOTAL_FIGHT_POWER = 10,
	RT_VICEGENERAL_FIGHT = 11,
	RT_MAX,
};

enum RealTimeRankType
{
	RTRT_LEVEL = 1,
	RTRT_FIGHT_POWER = 2,
};

enum GameMailType
{
	GMT_PLAYER = 1,
	GMT_FAMILY = 2,
};

enum AcOutdoorCaveType
{
	AOCT_RECORD =1,
	AOCT_ITEM =2,
};

enum YellowType
{
	YELLOW_DAILY = 1,//����ÿ�����
	YELLOW_YEAR = 2,//��ѻ������ÿ�����
	YELLOW_GROW_UP = 3,//����ɳ����
	YELLOW_NEW = 4,//�����������
};


#define N_MAX_IN_RECEIVE_BLESS 30//the max number in receive list
#define GOLD_COST_SUPER_BLESS 5//the gold cost for super bless

#define MAX_GM_ANNOUNCEMENT 606
// GM����
struct GmtAnnouncement 
{
	int32_t id;
	int32_t type;
	char content[MAX_GM_ANNOUNCEMENT];
	int32_t begin_time;
	int32_t end_time;
	int32_t interval;
	int32_t enable;

	int32_t last_action_time;
};
typedef std::map<int32_t, GmtAnnouncement> GmtAnnouncementTable;
struct GmtBanChat 
{
	int32_t uid;
	int32_t expire_time;
};
typedef std::map<int32_t, GmtBanChat> GmtBanChatTable;

#define MAX_NAME_LENGTH 8
#define MAX_NAME_CCH_LENGTH 30
#define MAX_EQUIP_NAME_LENGTH 60
#define MAX_ENTRUST_LENGTH  128
#define MAX_FAMILT_POST 100
#define MAX_CODE_LENGTH 20
#define MAX_OPEN_ID_LENGTH 36
#define MAX_PF_LENGTH  20
#define MAX_IP_LENGTH  16
#define MAX_IDENTITYCARD_LENGTH 18
#define MAX_ADVANCEDTRIALS_LENGTH  1000

#define SEX_MALE 1
#define SEX_FEMALE 2

#define MIN_STYLE_ID 1
#define MAX_STYLE_ID 2

#define MIN_HEAD_ID 1
#define MAX_HEAD_ID 10

#define MAX_CHARACTER_NUM 1

struct LogArtifactAddExp
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	int32_t itemid;
	int32_t baseid;
	int32_t old_exp;
	int32_t old_level;
	int32_t new_exp;
	int32_t new_level;
	int32_t stone_id;
	int32_t stone_count;
	int32_t time;
};

struct LogArtifactAddMaxLevel
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	int32_t itemid;
	int32_t baseid;
	int32_t old_max_level;
	int32_t new_max_level;
	int32_t stone_id;
	int32_t stone_count;
	int32_t time;
};

struct LogTask
{
	CharId_t cid;
	int32_t tid;
	int32_t type;
	int32_t state;
	int32_t time;
};

struct LogEquipEnhance
{
	CharId_t cid;
    char name[MAX_NAME_CCH_LENGTH+1];
	EquipId_t equip_id;
	int32_t base_id;
	int32_t equip_quality;
	int32_t old_level;
	int32_t new_level;
	int32_t money_cost;
	int32_t sys_gold_cost;
	int32_t gold_cost;
	int32_t time;    
};

struct LogEquipAddGem
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	char equip_name[MAX_EQUIP_NAME_LENGTH+1];
	int32_t equip_quality;
	char gem_name[MAX_NAME_CCH_LENGTH+1];
	int32_t gem_level;
	int32_t time;
};

struct LogItemCombine
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	int32_t old_item_id;
	int32_t old_item_num;
	int32_t new_item_id;
	int32_t new_item_num;
	int32_t money_cost;
	int32_t sys_gold_cost;
	int32_t gold_cost;
	int32_t time;
};

struct LogEquipSmithing
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	CharId_t equip_id;
	int32_t old_base_id;
	int32_t new_base_id;
	int32_t equip_quality;
	int32_t blueprint_id;
	int32_t blueprint_quality;
	int32_t time;
};

struct LogEquipDismantle//װ�����
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH + 1];
	CharId_t equip_id;
	int32_t base;
	int32_t enhance;
	int32_t item1_id;
	int32_t item1_count;
	int32_t item2_id;
	int32_t item2_count;
	int32_t item3_id;
	int32_t item3_count;
	int32_t item4_id;
	int32_t item4_count;
	int32_t time;
};

struct LogDungeon
{
	CharId_t cid;			// ���ID
	int32_t	did;			// ����ID
	int32_t type;			// ��������
	int32_t level;			// ��ҵȼ�
	int32_t state;			// ״̬ 0 δ��� 1 ��� 2 ��;�˳�
	int32_t start_time;		// ����ʱ��
	int32_t	finish_time;	// ���ʱ��
};

struct LogBoss
{
	int32_t	mid;			// ����ID
	int32_t	type;			// boss����
	CharId_t killer;		// ��ɱ��
	int32_t	time;			// ��ɱʱ��
};

struct LogActivity
{
	CharId_t cid;			// ���ID
	int32_t	actid;			// �ID
	int32_t	acttype;		// �����
	int32_t	time;			// ����ʱ��
	int64_t param;			// ����
};

enum DAILY_GIFT_TYPE
{
	DGT_SHOU_CHONG	= 1,	// ÿ���׳�
};

struct LogDailyGift
{
	CharId_t cid;			// ���ID
	int32_t	type;			// �������
	int32_t	giftid;			// ���id
	int32_t	time;			// ����ʱ��
};

struct LogFamily
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	char family_name[MAX_NAME_CCH_LENGTH+1];
	int32_t time;
};

struct LogOnlineReward
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	int32_t  num;
	int32_t  time;
	int32_t  type;
};

struct LogAchievement
{
	CharId_t  cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	int32_t  id;
	int32_t  index;
	int32_t  type;
	int32_t  time;
};

struct LogLevelGift
{
	CharId_t  cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	int32_t type;
	int32_t time;
};

#define ITEM_CHANGE_LOG_ADD			1
#define ITEM_CHANGE_LOG_REMOVE		2
#define ITEM_CHANGE_LOG_UPDATE		3
struct LogItemChange
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	int8_t	flag;
	int32_t reason;
	int32_t id;
	int8_t	itemClass;
	int32_t count;
	int32_t time;
	int64_t srcId;
};
typedef std::list<LogItemChange> LogItemChangeList;

struct LogArtifactAddQuality 
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	int32_t itemid;
	int32_t baseid;
	int32_t old_quality;
	int32_t new_quality;
	int32_t stone_id;
	int32_t time;
};


struct LogEquipAddGemSlot 
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	EquipId_t eid;
	int32_t baseid;
	int32_t old_gem_slot_num;
	int32_t new_gem_slot_num;
	int32_t stone_id;
	int32_t time;
};

struct LogEquipAppendStar 
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	EquipId_t eid;
	int32_t baseid;
	int32_t level;
	int32_t fuxing_stone_id;
	int32_t time;
};

struct LogEquipCombine 
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	EquipId_t eid;
	int32_t baseid;
	int32_t new_baseid;
	int32_t side_equip_1;
	int32_t side_equip_2;
	int32_t side_equip_3;
	int32_t side_equip_4;
	int32_t shouhu_stone_id;
	int32_t money;
	int32_t time;
};

struct LogEquipForge 
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	EquipId_t eid;
	int32_t baseid;
	int32_t item_id;
	int32_t money;
	int32_t time;
};

struct LogEquipLife
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	EquipId_t eid;
	int32_t baseid;
	int32_t type;
	int32_t reason;
	int32_t time;
};


struct LogEquipRemoveGem
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	EquipId_t eid;
	int32_t baseid;
	int32_t slot_index;
	int32_t stone_id;
	int32_t time;
};

struct LogEquipRemoveStar 
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	EquipId_t eid;
	int32_t baseid;
	int32_t level;
	int32_t fuxing_stone_id;
	int32_t shouhu_stone_id;
	int32_t time;
};

struct LogEquipRefreshBind
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	EquipId_t eid;
	int32_t baseid;
	int32_t old_bind;
	int32_t new_bind;
	int32_t stone_id;
	int32_t time;
};

struct LogEquipRefreshElement
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	EquipId_t eid;
	int32_t baseid;
	int32_t old_element;
	int32_t new_element;
	int32_t stone_id;
	int32_t time;
};

struct LogEquipRefreshFloatAttr 
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	EquipId_t eid;
	int32_t baseid;
	int32_t old_float_attr;
	int32_t new_float_attr;
	int32_t stone_id;
	int32_t time;
};

struct LogEquipStrengthen //ǿ��װ��
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	EquipId_t eid;
	int32_t baseid;
	int32_t old_strength;
	int32_t new_strength;
	int32_t stone_id;
	int32_t stone_count;
	int32_t time;
};

struct LogEquipUpgrade
{
	CharId_t cid;
	char name[MAX_NAME_CCH_LENGTH+1];
	EquipId_t eid;
	int32_t baseid;
	int32_t new_baseid;
	int32_t upgrade_stone_id;
	int32_t upgrade_stone_count;
	int32_t crystal_stone_id;
	int32_t zhenxing_stone_id;
	int32_t money;
	int32_t time;
};

enum AuctionState
{
	AS_IN_SELL,
	AS_SELLED,
	AS_SYS_LOCK,
};

struct MemCharacter 
{
	CharId_t cid;									// ��ɫID
	char	name[MAX_NAME_CCH_LENGTH+1];			// ��ɫ����
	char	familyName[MAX_NAME_CCH_LENGTH+1];		// ��������
	Sex_t	sex;									// �Ա�
	Job_t	job;									// ְҵ
	int32_t level;									// �ȼ�
	int32_t battle;									// ս����
	int64_t exp;									// ����ֵ
	int64_t exp_minus;
	int32_t	vip;									// vipʹ�ù��Ŀ�
	int32_t vip_end_time;							// vip����ʱ��
	int32_t mapid;									// ���ڵ�ͼID
	int32_t x;										// ���ڵ�ͼ����X
	int32_t y;										// ���ڵ�ͼ����Y
	int32_t hp;										// ��ǰѪ��
	int32_t mp;										// ��ǰ����ֵ
	int32_t pp;										// ��ǰ����ֵ
	int32_t xp;										// ��ǰXPֵ
	int32_t level_gift;								// �ȼ����
	int32_t level_gold;
	int32_t seven_login;							// ���յ�½
	int32_t head;
	int32_t pk_mode;								// pkģʽ
	int32_t pk_value;								// pkֵ
	int32_t pk_killed_count;
	int32_t pk_time;
	int32_t kill_count;

	int32_t bag_open_time;							// ��������ʱ��
	int32_t bag_open_slot;							// ������ǰ�����ĸ���
	int32_t bag_can_open;							// ��ǰ�ȹ�������������
	int32_t last_task_id;							// ��һ����������ID
	int32_t logout_count;							// �ǳ�ʱ��
	int32_t pay_click_count;
	int32_t level_stay_time;
	int32_t last_login_time;
	int32_t last_logout_time;
	int32_t create_time;
	int32_t leader;
};

#define ACTION_NUMBER 13 //action 0 for invalid check, max 12 actions

struct MemPvp
{
	int32_t time;
	int32_t count;
	int32_t mode;
};

struct MemAchievement
{
	  int32_t kill_player_count;
};

struct MemAttr
{
	int32_t hp;						// ���Ѫ��
	int32_t mp;						// ���Ѫ��
	int32_t pp;						// ���Ѫ��
	int32_t xp;						// ���Ѫ��
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
	int32_t battle;					// ս����
	int32_t lucky;					// ����ֵ
};

struct MemChrAction 
{
	int32_t id;
	int32_t type;
};

#define MAX_GOLD_FIND_CCH_LENGTH 100

#define MAX_AUTO_FIGHT_CCH_LENGTH 300
#define MAX_AUTO_PICK_CCH_LENGTH 600

struct MemChrAutoFight 
{
	char fight[MAX_AUTO_FIGHT_CCH_LENGTH+1];
	char pick[MAX_AUTO_PICK_CCH_LENGTH+1];
};

enum CHAR_BAG_TYPE
{
	CBT_EQUIP	= 0,	// װ����
	CBT_BAG		= 1,	// ����
	CBT_DEPORT	= 2,	// �ֿ�
	CBT_CHOUJIANG = 3,	// �齱����
};
#define MAX_BAG_SLOT_NUM	150			//���������
#define FREE_BAG_SLOT_NUM	40			//������ʼ����С

#define FREE_DEPOT_SLOT 40				//�ֿ��ʼ����С
#define MAX_DEPOT_SLOT 150				//�ֿ������

#define MAX_SELL_ITEM_COUNT 15
#define STONE_COMBINE_NUM 10
#define MAX_EQUIP_LEVEL 120
#define EQUIP_NAMESUITE_NUM 6
#define MAX_GEM_LEVEL  9

struct ChrBagInfo 
{
	int32_t	m_nLastOpenTime;
	int32_t m_nLeftSeconds;
	int32_t m_nCanOpenSlots;
	int32_t m_nOpenedSlots;
};

struct MemChrBag 
{
	int32_t itemId;		// ����ID�����ñ��е�ID��
	int8_t	itemClass;	// ��������
	int32_t itemCount;	// ��������
	int8_t	bind;		// ������
	Time_t	endTime;	// ����ʱ��
	int64_t	srcId;		// ����ID��װ��ID...��
};
typedef std::vector<MemChrBag> MemChrBagVector;
typedef std::map<int32_t,MemChrBagVector> MemChrBagVectorTable;
typedef std::map<int32_t,MemChrBag> MemBagTable;

struct MemChrClothesBag
{
	int32_t id;				//ʱװid
	int32_t type;			//���
	int32_t job;			//ְҵ
	int32_t base;			//����id
};
typedef std::vector<MemChrClothesBag> MemChrClothesBagVector;
typedef std::map<int32_t,MemChrClothesBag>MemChrClothesBagTable;

struct MemDungeonGift
{
	int32_t index;
	MemChrBagVector  items;
};

struct MemChrJobBag
{
	Job_t  job;
	int32_t  id;
	int32_t  type;
	int32_t  bind;
	int32_t  count;
};
typedef std::vector<MemChrJobBag> MemChrJobBagVector;

struct MemChrBuff 
{
	int32_t id;
	int32_t time;
};
typedef std::vector<MemChrBuff> MemChrBuffVector;

// ǩ��
class MemChrSignInfo
{
public:
	MemChrSignInfo()
	{
		CleanUp();
	}
	void CleanUp()
	{
		sign_record		= 0;
		refresh_time	= 0;
		sign_reward.clear();
	}
	int32_t				sign_record;
	std::vector<int8_t>	sign_reward;
	int32_t				refresh_time;		//��һ��ǩ��
};

enum PlayerBornAttr
{
	PBA_STR = 0,
	PBA_WIS = 0,
	PDA_MAG = 0,
	PBA_VIT = 0,
	PBA_MEN = 0,
	PBA_AGI = 0,
	PBA_PATTACK = 11,
	PBA_MATTACK = 11,
	PBA_PDEF = 1,
	PBA_MDEF = 1,
	PBA_MAXHP = 560,
	PBA_MAXMP = 200,
	PBA_VP = 0,
	PBA_DP = 0,
	PBA_HTIP = 300,
	PBA_SP = 150,
	PBA_PREVP = 0,
	PBA_PREFORST = 0,
	PBA_PREBK = 0,
	PBA_MAXANGRY = 1000,
	PBA_REGION = 10002,
	PBA_BENGIN_REGION = 10004,
	PBA_CENTER_CITY = 20002,
	PBA_FAMILY_MAP = 20026,
};

enum PlayerJob
{
	PJ_ZHANSHI		= 1,		// սʿ
	PJ_FASHI		= 2,		// ��ʦ
	PJ_LONGQI		= 3,		// ����
	PJ_JOB_COUNT,
};

enum PetJob
{
	PET_JOB_ZHANSHI		= 1,		// սʿ
	PET_JOB_FASHI		= 2,		// ��ʦ
};

enum SkillJob
{
	SJ_ZHANSHI = 1,
	SJ_GONGSHOU = 2,
	SJ_MOUSHI = 3,
	SJ_MOKE = 4,
	SJ_NUQI = 10,
};

enum EQUIP_TYPE
{
	ET_WEAPON			= 0,		// ����
	ET_NECK				= 1,		// ����
	ET_HAND				= 2,		// ����
	ET_CLOSE			= 3,		// �·�
	ET_HEAD				= 4,		// ͷ��
	ET_SHOES			= 5,		// ѥ��
	ET_BELT				= 6,		// ����
	ET_RING				= 7,		// ��ָ
	ET_XUNZHANG			= 8,		// ѫ��
};


enum EquipSlot
{
	EQUIP_SLOT_WEAPON			= 0,		// ����
	EQUIP_SLOT_NECK				= 1,		// ����
	EQUIP_SLOT_HAND				= 2,		// ����
	EQUIP_SLOT_CLOTH			= 3,		// �·�
	EQUIP_SLOT_HEAD				= 4,		// ͷ��
	EQUIP_SLOT_SHOES			= 5,		// ѥ��
	EQUIP_SLOT_BELT				= 6,		// ����
	EQUIP_SLOT_RING				= 7,		// ��ָ
	EQUIP_SLOT_XUNZHANG			= 8,		// ѫ��
	EQUIP_SLOT_TOTAL_NUM,					// װ��������С
};

enum NEW_GEM_TYPE
{
	GEM_TYPE_BATTLE 	= 1,	//ս������ʯ
	GEM_TYPE_DAMMAGE    = 2,	//���˱�ʯ
	GEM_TYPE_EXP		= 3,	//���鱦ʯ
	GEM_TYPE_ATTACK		= 4,	//�˺���ʯ
};

enum GEM_TYPE
{
	GEM_TYPE_RED	= 1,	// �챦ʯ
	GEM_TYPE_GREEN	= 2,	// �̱�ʯ
	GEM_TYPE_BLUE	= 3,	// ����ʯ
};

enum GEM_SLOT
{
	GEM_SLOT_RED	= 0,	// �챦ʯ���
	GEM_SLOT_GREEN,			// �̱�ʯ���
	GEM_SLOT_BLUE,			// ����ʯ���
	GEM_SLOT_NUM,
};

enum CHRINFO
{
	CHR_HONORID,
	CHR_HONORIDKINGDOM,
};
enum PlayerInfoIndex
{
	PII_JOB,
	PII_LEVEL,
	PII_LAST_LOGOUT_TIME,
	PII_VIP,
	PII_VIP_LEVEL,
	PII_LEARN_XP_SKILL,
	PII_BATTLE,
	PII_JUE_WEI,
	PII_WEAPON,
	PII_CLOTH,
	PII_GUAN_WEI,
};

struct MemChrEquip // ����
{
	int32_t eid;
	int32_t bind;
	int32_t expire;
};

struct MemJobItem
{
	int32_t job;
	int32_t item;
};
typedef std::map<int32_t,MemJobItem>MemJobItemTable;

struct MemChrRelation 
{
	int32_t tcid;
	int32_t type;
};
typedef std::vector<MemChrRelation> MemChrRelationVector;

struct MemChrSkill 
{
	int32_t id;
	int32_t level;
	int64_t last_action_tick;
};
typedef std::vector<MemChrSkill> MemChrSkillVector;

#define MAX_SYSTEM_SETTING_CCH_LENGTH 300

struct MemChrSystemSetting 
{
	char setting[MAX_SYSTEM_SETTING_CCH_LENGTH+1];
};

struct MemChrTask 
{
	int32_t id;
	int32_t state;
	int32_t monster;
};
typedef std::vector<MemChrTask> MemChrTaskVector;

#define EQUIP_GEM_COUNT 3
class MemEquip 
{
public:
	MemEquip() { CleanUp(); }
	~MemEquip(){}
	void CleanUp()
	{
		id				= 0;
		nFlag			= 0;
		base			= 0;
		owner			= 0;
		star			= 0;			
		starLucky		= 0;		
		UpGradeLucky	= 0;	
		UpQuality		= 0;		
		addAttr			= 0;
		memset( GemHole, -1, sizeof( GemHole ) );
	}
	void ParesEquipString( std::string EquipGemString )
	{
		if ( EquipGemString.empty() )
		{
			return;
		}
		StringVector ItemVector = Answer::StringUtility::split( EquipGemString, ":");
		if ( ItemVector.size() == EQUIP_GEM_COUNT )
		{
			for ( int32_t i = 0; i < EQUIP_GEM_COUNT; ++i )
			{
				GemHole[i]	= atoi(ItemVector[i].c_str());
			}
		}
	}
	std::string GetEquipGemString() const
	{
		std::stringstream ss;
		for ( int32_t i = 0; i < EQUIP_GEM_COUNT; ++i )
		{
			ss << static_cast<int32_t>( GemHole[i]);
			ss << ":";
		}
		return ss.str();
	}
	int32_t GetGemSlotCount() const
	{
		int32_t nCount = 0;
		for ( int32_t i = 0; i < EQUIP_GEM_COUNT; ++i )
		{
			if ( 0 == GemHole[i] )
			{
				++nCount;
			}
		}
		return nCount;
	}

	EquipId_t	id;
	int32_t		nFlag;			// ���±��
	ItemId_t	base;
	CharId_t	owner;
	int32_t		star;			// �Ǽ�
	int32_t		starLucky;		// ��������ֵ
	int32_t		UpGradeLucky;	// ��������ֵ
	int32_t		UpQuality;		// ��Ʒ����ֵ
	int32_t		GemHole[EQUIP_GEM_COUNT];	// ��ʯ��
	int32_t		addAttr;		// ������������
};
typedef std::vector<MemEquip> MemEquipVector;
typedef std::map<EquipId_t, MemEquip> MemEquipTable;

struct MemYellowStone
{
	CharId_t cid;
	int32_t id; //cfg_yellow_stone  id
	int32_t get_time;
};
typedef std::vector<MemYellowStone> MemYellowStoneVector;

struct MemItemArtifact 
{
	int32_t id;
	int32_t base;
	int32_t owner;
	int32_t bind;
	int32_t expire;
	int32_t level;
	int32_t max_level;
	int32_t exp;
	int32_t quality;
};

struct SysUser 
{
	int32_t uid;
	int32_t sid;
	int32_t gold;
	int32_t sys_gold;
	int32_t gold_pay_total;
	int32_t prepay_gold;
	int32_t gold_cost_total;
	int32_t cash;
	int32_t map_enter_time;
	int32_t last_login_time;
	int32_t adult;
	int32_t type;
	int32_t total_login_count;
	int32_t continue_login_count;
	int32_t last_logout_time;
	int32_t total_online_time;
	int32_t total_offline_time;
	char IP[MAX_IP_LENGTH+1];
};

//��������֤
struct SysUserPreventWallow
{
	int32_t uid;
	int32_t sid;
	char    name[MAX_NAME_CCH_LENGTH+1]; //����
	char    identitycard[MAX_IDENTITYCARD_LENGTH + 1]; //����֤����
	int32_t isGrowUp; //�Ƿ������
};

#define  MAX_IDENTITYCARD_LENGTH 18

enum NewServerActivityType
{
	NSAT_VIP = 1,
	NSAT_FIGHT_POWER_RANK = 2,
	NSAT_LEVEL_RANK = 3,
	NSAT_JISHUJIUGONG = 4,
	NSAT_MOUNT_FIGHT_POWER_RANK = 5,
	NSAT_EQUIP = 6,
	NSAT_GEM = 7,
	NSAT_LEVEL_UP_TO = 8,
	NSAT_FIGHT_POWER_UP_TO = 9,
	NSAT_POINT = 10,
	NSAT_VICEGENERAL = 11,
	NSAT_GOLD_COST = 12,
};

struct OperateLimit
{
	int32_t	LimitId;
	int32_t	LimitCount;
	bool	IsChaned;
};
typedef map<int32_t, OperateLimit> OperateLimitMap;

// ��״̬
enum ITEM_BIND_STATE
{
	IBS_UNBIND	= 0,
	IBS_BIND	= 1,
};

struct ItemData
{
	int32_t	m_nId;
	int8_t	m_nClass;
	int32_t	m_nCount;
};
typedef std::list<ItemData> ItemDataList;

struct CfgAddAttr
{
	int32_t	m_nAttr;
	int32_t	m_nValue;
	int32_t	m_nRate;
};
typedef std::list<CfgAddAttr> CfgAddAttrList;

struct CfgSuitAttr
{
	int32_t	m_nCount;
	int32_t	m_nAttr;
	int32_t m_nValue;
};
typedef std::list<CfgSuitAttr> CfgSuitAttrList;

// ����
enum CURRENCY_TYPE
{
	CURRENCY_INVALID		= -1,
	CURRENCY_MONEY			= 0,		// ͭǮ
	CURRENCY_GOLD			= 1,		// Ԫ��
	CURRENCY_CASH			= 2,		// ��Ԫ
	CURRENCY_VIGOUR			= 3,		// Ԫ��
	CURRENCY_CHOUJINAG		= 4,		// �齱����
	CURRENCY_BOSS			= 5,		// BOSS����
	CURRENCY_BIND_MONEY		= 6,		// �󶨽��
	CURRENCY_HONOR			= 7,		// ����
	CURRENCY_AC_SOCRE		= 8,		// ����
	CURRENCY_TYPE_COUNT,
};

inline CURRENCY_TYPE TranseCurrency( CURRENCY_ITEM_ID id )
{
	switch( id )
	{
	case CURRENCY_MONEY_ID:		return CURRENCY_MONEY;
	case CURRENCY_GOLD_ID:		return CURRENCY_GOLD;
	case CURRENCY_CASH_ID:		return CURRENCY_CASH;
	case CURRENCY_VIGOUR_ID:	return CURRENCY_VIGOUR;
	default:	break;
	}
	return CURRENCY_INVALID;
}

struct KillMonsterInfo
{
	int32_t		m_MonsterMid;		
	int32_t		m_KillCount;		
	int8_t		m_BossSign;		
};
typedef std::map<int32_t,KillMonsterInfo> KillMonsterMap;

class ExpReward
{
public:
	ExpReward()
	{
		FriendId	= 0;
		GetExp		= 0;
		Name		= "";
		Level		= 0;
	}
	CharId_t	FriendId;
	int32_t		GetExp;
	std::string Name;
	int32_t		Level;
};
typedef std::map<CharId_t,ExpReward> FriendExpMap;


 class DropEquipInfo
{
public:
	DropEquipInfo()
	{
		CharId				= 0;
		CharName			= "";
		KillerId			= 0;	
		KillerName			= "";
		CanGetLastTime		= 0; 
		itemId				= 0;			
		itemClass			= 0;		
		itemCount			= 0;		
		bind				= 0;			
		endTime				= 0;		
		srcId				= 0;
		Mid					= 0;
	}
	CharId_t			CharId;
	std::string			CharName;
	CharId_t			KillerId;	
	std::string			KillerName;
	Time_t				CanGetLastTime; // ����ص����ʱ��				
	int32_t				itemId;			// ����ID�����ñ��е�ID��
	int8_t				itemClass;		// ��������
	int32_t				itemCount;		// ��������
	int8_t				bind;			// ������
	Time_t				endTime;		// ����ʱ��
	int64_t				srcId;			// ����ID��װ��ID...��
	int32_t				Mid;			// ����Ǳ�����ɱ��
};
 typedef std::list<DropEquipInfo> RANSOM_ITEM_LIST;

 enum UPDATE_TYPE
 {
	 ADD_RANSOM		= 1,
	 SUB_RANSOM		= 2,
 };

#define EQUIP_RANSOM_TIME 7*24*60*60	//װ�����ʱ��
#define EQUIP_MAX_STAT  12 //װ���������

#define PET_RANK_OPEN_LEVEL	60		// ����������а�����ȼ�
#define MAX_PET_ID			100		// ������ID
#define PET_SKILL_COUNT		10		// ���＼����
#define PET_SKILL_BAR_SIZE	6		// ���＼������С 
enum PET_ATTR
{
	PET_ATTR_START_HP					= 0,			// ��ʼ������		��ʼ����Ϊ����1��ʱ������ֵ������ֵ��Χ1~9999999��
	PET_ATTR_START_PHY_ATK_MIN			= 1,			// ��ʼ��С������	��ʼ����Ϊ����1��ʱ����������ֵ������ֵ��Χ1~9999999��
	PET_ATTR_START_PHY_ATK_MAX			= 2,			// ��ʼ��󹥻���	��ʼ����Ϊ����1��ʱ����������ֵ������ֵ��Χ1~9999999��
	PET_ATTR_START_PHY_DEF				= 3,			// ��ʼ������		��ʼ����Ϊ����1��ʱ����������ֵ������ֵ��Χ1~9999999��
	PET_ATTR_START_MAG_ATK_MIN			= 4,			// ��ʼ��Сħ����	��ʼħ��Ϊ����1��ʱ��ħ������ֵ������ֵ��Χ1~9999999��
	PET_ATTR_START_MAG_ATK_MAX			= 5,			// ��ʼ���ħ����	��ʼħ��Ϊ����1��ʱ��ħ������ֵ������ֵ��Χ1~9999999��
	PET_ATTR_START_MAG_DEF				= 6,			// ��ʼħ����		��ʼħ��Ϊ����1��ʱ��ħ������ֵ������ֵ��Χ1~9999999��
	PET_ATTR_START_DODGE				= 7,			// ��ʼ���ܣ�		����1��ʱ������ֵ������ֵ��Χ1~9999999��
	PET_ATTR_START_HITRATE				= 8,			// ��ʼ���У�		����1��ʱ������ֵ������ֵ��Χ1~9999999��
	PET_ATTR_START_CRITRATE				= 9,			// ��ʼ������		����1��ʱ�ı���ֵ������ֵ��Χ1~9999999��
	PET_ATTR_START_TENACITY				= 10,			// ��ʼ���ԣ�
	PET_ATTR_GROW_HP					= 11,			// �����ɳ���		Ӱ����޵���������ֵ������ֵ��Χ1~9999����С���㣬������4λ��		��1~99990000, ����10000Ϊ��ʾֵ��
	PET_ATTR_GROW_PHY_ATK_MIN			= 12,			// ��С�����ɳ���	Ӱ����޵�������С����ֵ������ֵ��Χ1~9999����С���㣬������4λ��	��1~99990000, ����10000Ϊ��ʾֵ��
	PET_ATTR_GROW_PHY_ATK_MAX			= 13,			// ��󹥻��ɳ���	Ӱ����޵�������󹥻�ֵ������ֵ��Χ1~9999����С���㣬������4λ��	��1~99990000, ����10000Ϊ��ʾֵ��
	PET_ATTR_GROW_PHY_DEF				= 14,			// �����ɳ���		Ӱ����޵����շ���ֵ������ֵ��Χ1~9999����С���㣬������4λ��		��1~99990000, ����10000Ϊ��ʾֵ��
	PET_ATTR_GROW_MAG_ATK_MIN			= 15,			// ��Сħ���ɳ���	Ӱ����޵�������Сħ��ֵ������ֵ��Χ1~9999����С���㣬������4λ��	��1~99990000, ����10000Ϊ��ʾֵ��
	PET_ATTR_GROW_MAG_ATK_MAX			= 16,			// ���ħ���ɳ���	Ӱ����޵��������ħ��ֵ������ֵ��Χ1~9999����С���㣬������4λ��	��1~99990000, ����10000Ϊ��ʾֵ��
	PET_ATTR_GROW_MAG_DEF				= 17,			// ħ���ɳ���		Ӱ����޵�����ħ��ֵ������ֵ��Χ1~9999����С���㣬������4λ��		��1~99990000, ����10000Ϊ��ʾֵ��
	PET_ATTR_GROW_DODGE					= 18,			// ���ܳɳ���		Ӱ����޵���������ֵ������ֵ��Χ1~9999����С���㣬������4λ��		��1~99990000, ����10000Ϊ��ʾֵ��
	PET_ATTR_GROW_HITRATE				= 19,			// ���гɳ���		Ӱ����޵���������ֵ������ֵ��Χ1~9999����С���㣬������4λ��		��1~99990000, ����10000Ϊ��ʾֵ��
	PET_ATTR_GROW_CRITRATE				= 20,			// �����ɳ���		����1��ʱ�ı���ֵ������ֵ��Χ1~9999999��
	PET_ATTR_GROW_TENACITY				= 21,			// ��ʼ���ԣ�
	PET_ATTR_COUNT,
};

enum INSIDE_PET_ID
{
	IPI_ZHANSHI	= 21,	// սʿ����ID
	IPI_FASHI	= 22,	// ��ʦ����
	IPI_LONGQI	= 23,	// ��������
};

inline int32_t GetInsidePetId( Job_t nJob )
{
	switch( nJob )
	{
	case PJ_ZHANSHI:	return IPI_ZHANSHI;		// սʿ
	case PJ_FASHI:		return IPI_FASHI;		// ��ʦ
	case PJ_LONGQI:		return IPI_LONGQI;		// ����
	default:break;
	}
	return -1;
}

inline bool IsInsidePet( int32_t nPetId )
{
	return ( nPetId == IPI_ZHANSHI || nPetId == IPI_FASHI || nPetId == IPI_LONGQI );
}


// ��������
enum PET_PHASE
{
	PP_INVALID	= 0,
	PP_WIND		= 1,	// ��
	PP_WATER	= 2,	// ˮ
	PP_FIRE		= 3,	// ��
	PP_EARTH	= 4,	// ��
	PP_ELECTRIC	= 5,	// ��
	PP_GOLD		= 6,	// ��

	PP_DEFAULT	= 0xFF
};

enum PET_PLACE
{
	PBT_BAG		= 0,
	PBT_DEPORT	= 1,

	PET_BAG_TYPE_SIZE,
};

enum PET_EGG_STATE
{
	PES_NORMAL		= 0,		// ����״̬
	PES_HATCHING	= 1,		// ������
	PES_CAN_OPEN	= 2,		// �������
};

enum PET_FROM_WAY
{
	PFW_NORMAL		= 0,		// һ����Դ
	PFW_BORN		= 1,		// �����Դ�
	PFW_GIFT		= 2,		// ������
};

#define PET_BAG_SIZE					20								// ��������С
#define PET_DEPORT_SIZE					15								// ���޲ֿ��С
#define PET_VIP_ADD_DEPORT_SIZE			30								// vip���ӻ��޲ֿ����ĸ���				
#define PET_LIST_SIZE					(PET_BAG_SIZE+PET_DEPORT_SIZE + PET_VIP_ADD_DEPORT_SIZE )	// �ܴ�С
#define PET_EGG_BAG_SIZE				14								// ���ޱ�����С
#define PET_HATCH_POOL_SIZE				32								// ���޷����ش�С
#define	PET_MUTI_HATCH_POOL_SIZE		40								// �������������ش�С
#define PET_EGG_REHATCH_COST			3								// �������·�������Ԫ��
#define PET_EGG_QUICK_HATCH_COST		(30*60)							// ���޿��ٷ���ÿ30��������1Ԫ��
#define PET_EGG_HATCH_TIME				(20*60)							// ���ﵰ����ʱ��		20����
#define PET_EGG_MUTI_HATCH_TIME			(6*60*60)						// ���ﵰ��������ʱ��	6Сʱ
#define PET_REIN_RATE					10								// ����ת�������洢ֵ����

// ������ʿ
enum PET_KNIGHT
{
	PK_NONE			= 0,		// ��
	PK_ZHIHUI		= 1,		// �ǻ���ʿ
	PK_LEITING		= 2,		// ������ʿ
	PK_DOUHUN		= 3,		// ������ʿ
	PK_SHENGZHAN	= 4,		// ʥս��ʿ
	PK_GUANGHUI		= 5,		// �����ʿ
	PK_SHOUHU		= 6,		// �ػ���ʿ
	PK_CAIJUE		= 7,		// �þ���ʿ
	PK_SHENGGUANG	= 8,		// ʥ����ʿ

	PET_KNIGHT_SIZE
};

enum PET_EGG_BAG_TYPE
{
	PEBT_BAG				= 1,	// ���ﵰ����
	PEBT_HATCH_POOL			= 2,	// ���������
	PEBT_MUTI_HATCH_POOL	= 3,	// ����������
};

enum PET_RANK_TYPE
{
	PRT_POINTS					= 0,	// ���֣������У�
// 	PRT_START_HP				= 1,	// ��ʼ������	
// 	PRT_START_PHY_ATK_MIN		= 2,	// ��ʼ��С������
// 	PRT_START_PHY_ATK_MAX		= 3,	// ��ʼ��󹥻���
// 	PRT_START_PHY_DEF			= 4,	// ��ʼ������	
// 	PRT_START_MAG_ATK_MIN		= 5,	// ��ʼ��Сħ����
// 	PRT_START_MAG_ATK_MAX		= 6,	// ��ʼ���ħ����
// 	PRT_START_MAG_DEF			= 7,	// ��ʼħ����	
	PRT_GROW_HP					= 8,	// �����ɳ���	
	PRT_GROW_PHY_ATK_MIN		= 9,	// ��С�����ɳ���
	PRT_GROW_PHY_ATK_MAX		= 10,	// ��󹥻��ɳ���
	PRT_GROW_PHY_DEF			= 11,	// �����ɳ���	
	PRT_GROW_MAG_ATK_MIN		= 12,	// ��Сħ���ɳ���
	PRT_GROW_MAG_ATK_MAX		= 13,	// ���ħ���ɳ���
	PRT_GROW_MAG_DEF			= 14,	// ħ���ɳ���	
};

struct SocialPetData
{
	int32_t		nBaseId;
	uint32_t	nRankFlag;
	PetId_t		nPetId;
	CharId_t	nOwner;
	int32_t		nPoints;
	char		strName[MAX_NAME_CCH_LENGTH+1];
	int32_t		vAttr[PET_ATTR_COUNT];
};

struct PetRankIndex 
{
	int32_t	nPointsIndex;
	int32_t vAttrIndex[PET_ATTR_COUNT];
};

struct DBPet
{
	PetId_t		nPetId;
	int32_t		nHP;
	int8_t		bAlive;
	int32_t		nSoul;
	int8_t		nState;
	int8_t		nBag;
	int8_t		nKnight;
	int8_t		nQiShi;
	FamilyId_t	nFamilyId;
};

struct DBPetEgg 
{
	int8_t	nBag;
	int32_t	nSlot;
	int32_t	nBaseId;
	int8_t	nState;
	int32_t	nStartTime;
	PetId_t	nPetId;
};

#define MAX_TEAM_MEMBER_COUNT	4		// ����������
struct TeamMember 
{
	TeamMember( CharId_t cid, const std::string& name, int16_t idx )
		:nCharId( cid ), strName( name ), nIndex( idx )
	{

	}
	CharId_t	nCharId;
	std::string	strName;
	int16_t		nIndex;
};
typedef std::list<TeamMember> TeamMemberList;

#define MAX_XP_POINTS			210		// XP���ֵ
#define ADD_XP_POINTS			30		// XPÿ������ֵ

enum XP_STATE
{
	XS_INCREACE		= 0,	// XP������
	XS_DO_SKILL		= 1,	// XP�ͷ���
	XS_END			= 2,	// XP�����ȴ�����
	XS_BREAK		= 3,	// δѡ��XPֱ��������
	XS_OTHER_END	= 4,	// XP��;����������ҽ���XP��
	XS_STOP			= 5,	// �ڶ���������,��һ��ֹͣ����xp
};

#define CREATE_FAMILY_COST_GOLD		100	// ����������Ҫ100Ԫ��
#define FAMILY_APPLY_COUNT			50

struct FamilyInfo 
{
	FamilyInfo(){ CleanUp(); }
	~FamilyInfo(){}
	void CleanUp()
	{
		nId			= 0;
		nLevel		= 0;
		strName		= "";
		nAddBattle	= 0;
		FamilyTaskCount	= 0;
	}

	bool IsEmpty() const
	{
		return 0 == nId;
	}

	bool TotomIsOpened( int32_t PetBaseId )
	{
		std::set<int32_t>::iterator it = OpenedTotom.find( PetBaseId );
		if ( it != OpenedTotom.end() )
		{
			return true;
		}
		return false;
	}

	FamilyId_t			nId;
	int8_t				nLevel;
	std::string			strName;
	int32_t				nAddBattle;
	std::set<int32_t>	OpenedTotom;
	int32_t				FamilyTaskCount;
};

#define MAX_FAMILY_LEVEL			10
#define FAMILY_OPEN_LEVEL			30			// ���ſ����ȼ�
#define MAX_FAMILY_CONTRIBUTE_GOLD	100000		// ���ʾ������ħʯ 10w
#define MAX_FAMILY_CONTRIBUTE_MONEY	1000000000	// ���ʾ�������Ǯ 10y
#define MAX_FAMILY_CONTRIBUTE		100000000	// ����ͨ�������ܻ�õ�����׶� 1
#define MAX_FAMILY_MONEY			1000000000	// ��������ʽ� 10y
#define FAMILY_TOTOM_ACTIVE_TIME	259200		// ���żӳ�ʱ�� 3��	3*24*60*60

struct CfgFamilyPosition 
{
	int32_t	nNeedContribution;		// �����׶�
	int32_t	nShareBattleRatio;		// ս��������
};

enum FAMILY_UPDATE_REASON
{
	FUR_CREATE	= 1,
	FUR_UPDATE	= 2,
	FUR_DELETE	= 3,
};

// ����ְ��
enum FAMILY_POSITION
{
	FP_NONE			= 0,	// ����
	FP_SENIOR		= 1,	// ����
	FP_ELIT			= 2,	// ��Ӣ
	FP_SENETE		= 3,	// ��Ա
	FP_ELDER		= 4,	// Ԫ��
	FP_VICE			= 5,	// ���ų�
	FP_LEADER		= 6,	// ���ų�
	FP_HONOR_VICE	= 7,	// �������ų�
	FP_HONOR_ELDER	= 8,	// ����Ԫ��
	FP_HONOR_SENETE	= 9,	// ������Ա

	FAMILY_POSITION_COUNT,
};

enum FAMILY_POSITION_LEVEL
{
	FPL_NONE	= 0,
	FPL_SENIOR	= 1,
	FPL_ELIT	= 2,
	FPL_SENETE	= 3,
	FPL_ELDER	= 4,
	FPL_VICE	= 5,
	FPL_LEADER	= 6,

	FAMILY_POSITION_LEVEL_COUNT,
};

inline bool IsSortPosition( int8_t nPosition )
{
	switch ( nPosition )
	{
	case FP_SENIOR:
	case FP_ELIT:
	case FP_SENETE:
	case FP_ELDER:
	case FP_VICE:
		return true;
	}
	return false;
}

inline bool IsHonorPosition( int8_t nPosition )
{
	switch ( nPosition )
	{
	case FP_HONOR_VICE:
	case FP_HONOR_ELDER:
	case FP_HONOR_SENETE:
		return true;
	}
	return false;
}

inline int8_t GetPositionLevel( int8_t nPosition )
{
	switch ( nPosition )
	{
	case FP_NONE:			return FPL_NONE;
	case FP_SENIOR:			return FPL_SENIOR;		// �����Ա
	case FP_ELIT:			return FPL_ELIT;		// ��Ӣ
	case FP_HONOR_SENETE:
	case FP_SENETE:			return FPL_SENETE;		// ��Ա��������Ա
	case FP_HONOR_ELDER:
	case FP_ELDER:			return FPL_ELDER;		// Ԫ�ϡ�����Ԫ��
	case FP_HONOR_VICE:
	case FP_VICE:			return FPL_VICE;		// ���ų����������ų�
	case FP_LEADER:			return FPL_LEADER;		// ���ų�
	}
	return 0;
}

inline int32_t CalPetRegistAddContribute( int32_t nPoints )
{
	return nPoints;
}

inline int32_t CalFamilyContribute( int32_t nMoney, int32_t nGold )
{
	return nMoney / 1000 + nGold * 200;
}

enum BossType
{
	BOSS_TYPE_LING_TU		= 1,	//����boss
	BOSS_TYPE_SPIDER_QUEEN  = 2,	//֩��Ů��
	BOSS_TYPE_WORLD_BOSS	= 3,	//����boss
	BOSS_TYPE_BOSS_HOME		= 4,	//boss֮��
};

#define MAIL_MAX_FU_JIAN_COUNT	6			//��������
#define MAIL_TITLE_MAX_LENGTH 30			//�ʼ����ⳤ��
#define MAIL_CONTENT_MAX_LENGTH 150			//�ʼ����ݳ���
#define MAIL_MAX_KEEP_TIME	14*24*60*60		//�ʼ�������ʱ��		
struct MailInfo
{
	int32_t			MailId;
	int32_t			SysMailId;
	CharId_t		SenderId;
	char			SenderName[MAX_NAME_CCH_LENGTH];
	CharId_t		ReceiveId;
	char			ReceiveName[MAX_NAME_CCH_LENGTH];
	int32_t			SendTime;
	int8_t			HasRead;
	int8_t			Extract;		//�����Ƿ���ȡ
	char			MailTitle[MAIL_TITLE_MAX_LENGTH];
	char			MailContent[MAIL_CONTENT_MAX_LENGTH];
	MemChrBag		Item[MAIL_MAX_FU_JIAN_COUNT];
	std::string		Param;
};

typedef std::map<int32_t,MailInfo> MailInfoMap;

enum TI_SHI_VALUES
{
	PET_BAG_NOT_ENOUGH		= 2,	//���ޱ�������
	PET_EGG_BAG_NOT_ENOUGH	= 3,	//���ﵰ�������Ӳ���
	FINISH_FEI_TIAN_SHENG_JV= 4,	//�Ѿ���ɹ�������Ի
	NO_TRADE				= 5,	//��ֹ����
	NO_ENTER_ACTIVITY		= 6,	//���ܴ��ͽ���,�����������ͼ
	BAG_SLOT_NOT_ENOUGH		= 2048,	//��ʾ�����ո���
};

enum FaBaoType
{
	FA_BAO_TYPE_SHU_GUANG	= 0,	//��ⷨ��
	FA_BAO_TYPE_DI_LONG		= 1,	//��������
	FA_BAO_TYPE_FU_WEN		= 2,	//���ķ���
	FA_BAO_TYPE_COUNT,
};

enum FaBaoResourceType
{	
	FA_BAO_RES_SHU_GUANG	= 0,	//�����Ƭ
	FA_BAO_RES_DI_LONG		= 1,	//������Ƭ
	FA_BAO_RES_FU_WEN		= 2,	//������Ƭ
	FA_BAO_RES_COUNT,		
};

#define  DA_TIME_MAX_TIME		30		//����ʱ��30��
#define  RAND_ANSWER_TIME		25		//�����25��

//�״̬
enum ACTIVITY_STATE
{
	AS_NOT_START	= 0,	// δ����
	AS_READY		= 1,	// ׼��
	AS_RUNNING		= 2,	// ������
	AS_END			= 3,	// ����
	AS_TIME_OUT		= 4		// ����
};

//�ʼ�����id
enum EamilId	
{
	RansomItemId	= 6,			//װ������ʼ�����
	DaTiHDFirst		= 6203,			//������һ��
	DaTiHDSecond	= 6204,			//�����ڶ���
	DaTiHDThird		= 6205,			//����������
	DaTiHDChanYu	= 6206,			//�������뽱
	DaWeiWangHD		= 6210,			//��θ�������
	PlantAddItem    = 6216,			//�ɼ����ȡ����Ʒ
	ExpBallId		= 6217,			//������id
	DungeonRewards	= 1000,			//��������

	MI_AUCTION_SELL			= 6207,			// �����������ɹ�
	MI_AUCTION_BUY			= 6208,			// �����й���ɹ�
	MI_AUCTION_DOWNLOAD		= 6209,			// �������¼ܳɹ�
	MI_FAMILYWAR_REWARD		= 6211,			// ����ս����
	MI_HORSE_RACING_REWARD1	= 6212,			// ������Ե�һ������
	MI_HORSE_RACING_REWARD2	= 6213,			// ������Եڶ�������
	MI_HORSE_RACING_REWARD3	= 6214,			// ������Ե���������

	MI_TERRITORY_WAR_REWARD	= 6218,			// ���ս����
	MI_HALL_OF_FAME_REWARD	= 6239,			// �����ý���

	MI_HORSE_RACING_REWARD4	= 6240,			// ������Ե���������
	MI_HORSE_RACING_REWARD5	= 6241,			// ������Ե���������
	MI_HORSE_RACING_REWARD6	= 6242,			// ������Ե���������
	MI_HORSE_RACING_REWARD7	= 6243,			// ������Ե���������
	MI_HORSE_RACING_REWARD8	= 6244,			// ������Եڰ�������
	MI_HORSE_RACING_REWARD9	= 6245,			// ������Եھ�������
	MI_HORSE_RACING_REWARD0	= 6246,			// ������Ե�ʮ������
	MI_HORSE_RACING_REWARD	= 6247,			// ������Ե�ʮ��֮����

	MI_CITY_WAR_REWARD		= 6250,			// 城战参与奖励
	MI_CITY_WAR_RANK1		= 6251,			// 城战第1名家族奖励
	MI_CITY_WAR_RANK2		= 6252,			// 城战第2名家族奖励
	MI_CITY_WAR_RANK3		= 6253,			// 城战第3名家族奖励
};

// ����ID
enum BROADCAST_ID
{
	MO_YU_WORLD_DUNGEON_GONG_GAO	= 1,
	BCI_WORLD_FAMILYWAR_READY		= 2,
	BCI_WORLD_FAMILYWAR_START		= 3,
	BCI_WORLD_FAMILYWAR_PILLAR_KILLED_BY_FAMILY		= 6,
	BCI_WORLD_FAMILYWAR_PILLAR_KILLED_BY_FREE		= 7,
	BCI_WORLD_FAMILYWAR_STONE_KILLED				= 8,
	BCI_WORLD_FAMILYWAR_WIN							= 9,
	BCI_WORLD_FAMILYWAR_FAIL						= 10,
	BCI_MOUNT_DIE_DROP_GONG_GAO		= 11,	// ������������
	BCI_SOUL_LEVEL_UP				= 12,	// ��������
	BCI_HORSE_RACING_WIN_1			= 13,	// ������һ��
	BCI_HORSE_RACING_WIN_2			= 14,	// �����ڶ���
	BCI_HORSE_RACING_WIN_3			= 15,	// ����������

	BCI_HORSE_RACING_READY			= 19,	// �������׼������
	BCI_HORSE_RACING_START			= 20,	// ������Կ�ʼ����
	BCI_GRADE_BAO_XIANG				= 21,	//  �߼�����ˢ�¹���
	BCI_VIP_GONG_GAO_1				= 22,	//  vip����
	BCI_VIP_GONG_GAO_2				= 23,	//  vip����
	BCI_VIP_GONG_GAO_3				= 24,	//  vip����
	BCI_TOU_ZI_MOON_CARD			= 26,	//  �¿�Ͷ�ʹ���
	BCI_TOU_ZI_LEVEL_UP				= 27,	//  �Ǽ�Ͷ��
	BCI_DA_TI_FIRST					= 28,	//  ���⹫��
	BCI_XIAO_JV_HUA_GONG_GAO		= 30,	//  ����ջ�

	BCI_TERRITORY_ACTIVITY_START	= 31,	// ���ս��ʼ
	BCI_TERRITORY_BATTLE_READY		= 32,	// ���ս��ʼ5���Ӻ�
	BCI_TERRITORY_WIN_SINGLE		= 33,	// ���ս���˻�ʤ
	BCI_TERRITORY_WIN_TEAM			= 34,	// ���ս��ӻ�ʤ
	BCI_SQ_ACTIVITY_START			= 35,	// ֩��Ů�����ʼ
	BCI_SQ_ACTIVITY_END_BY_SQ_DIE	= 36,	// ֩��Ů�����������
	BCI_SQ_ACTIVITY_END_BY_TIME		= 37,	// ֩��Ů���ʱ�䵽����

	BCI_HALL_OF_FAME_FIGHT			= 38,	// ��������ս����
	BCI_HALL_OF_FAME_RANK_1			= 39,	// �����õ�һ������
	BCI_HALL_OF_FAME_RANK_2			= 40,	// �����õڶ�������
	BCI_HALL_OF_FAME_RANK_3			= 41,	// �����õ���������
	BCI_GUAN_WEI_UP					= 42,	// ������λ����
	BCI_SHOU_CHONG_LI_BAO			= 43,	// ��ȡ�׳����

	BCI_DROP_EQUIP					= 83,	// ���װ�����乫��
	BCI_DROP_EQUIP_2				= 84,	// ���װ�����乫��
	BCI_RANSOM_EQUIP				= 85,	// ���װ��
	BCI_RANSOM_EQUIP_2				= 86,	// ���װ��
	BCI_WAR_VICTORY_BOSS_REVIVE		= 95,	// ��սboss����
	BCI_WAR_VICTORY_BOSS_LEAVE		= 96,   // ��սboss�뿪
	BCI_WAR_VICTORY_BOSS_KILLED		= 97,	// ��սboss��ɱ
	BCI_TITLE_ADDED					= 98,
	BCI_TITLE_EXPIRED				= 99,
};

// GM����
class GMBroadcast 
{
public:
	GMBroadcast()
		:nId(0),nStartTime(0),nInterval(0),nTimes(0), nLastTime(0), nCount(0)
	{
	}
	GMBroadcast( int32_t id, const std::string& text, int8_t type, int32_t start_time, int32_t interval, int32_t times )
		:nId(id), strText(text), nType(type), nStartTime(start_time), nInterval(interval), nTimes(times), nLastTime(0), nCount(0)
	{
	}

	int32_t nId;
	std::string strText;
	int8_t nType;
	int32_t nStartTime;	
	int32_t nInterval;	
	int32_t nTimes;
	int32_t nLastTime;
	int32_t nCount;
};
typedef std::list<GMBroadcast> GMBroadcastList;
typedef std::map<int32_t, GMBroadcast> GMBroadcastMap;

// ����
struct GMBanChat 
{
	CharId_t nCharId;
	int32_t	nExpireTime;
};
typedef std::list<GMBanChat> GMBanChatList;
typedef std::map<CharId_t,GMBanChat> GMBanChatMap;

// ���
struct GMSeal 
{
	CharId_t nCharId;
	int32_t	nExpireTime;
};
typedef std::list<GMSeal> GMSealList;
typedef std::map<CharId_t,GMSeal> GMSealMap;

// ������
#define HALL_OF_FAME_RANK_SIZE		2000	// ���а��С
#define HALL_OF_FAME_CD_TIME		180		// CDʱ�� 10����
#define HALL_OF_FAME_FREE_TIMES		10		// ÿ�����ս10��
#define HALL_OF_FAME_BUY_COST		10		// ��������ħʯ
#define HALL_OF_FAME_CLEAR_CD_COST	2		// ��CD����ħʯ
#define HALL_OF_FAME_REWARD_TIME	1320	// ÿ��22:00������


#define JUE_WEI_RANK_MIN_DONATE	30000000		//3ǧ��ſ�ʼ����

typedef std::map<int32_t,MailInfo> MailInfoMap;

#define BANG_SHI_SHANG_CHENG_TYPE	4 //��ʯ�̳ǵ��̳�����

enum ShangChengLimitType
{
	LIMIT_TYPE_INVALID		= 0,		 //	���޹�
	LIMIT_TYPE_CHAR			= 1,		 // �����޹�(ÿ��Ĺ������)
	LIMIT_TYPE_SERVER		= 2,		 //	ȫ���޹�(ÿ��Ĺ������)
	LIMIT_TYPE_TIME			= 3,		 //	ʱ���޹�
};

enum ShopType
{
	ST_GAME_SHOP  = 1,	//�̳�
	ST_CHAR_SHOP  = 2,	//�����̵�
	ST_SCORE_SHOP = 3,	//�����̳�
};

typedef std::map<int32_t,int32_t> ItemLimitMap;		// <�̳�id,�������>

struct CfgGameShop
{
	int32_t		ShopId;
	int32_t		ItemId;
	int32_t		ItemClass;
	int32_t		ItemBind;
	int32_t		LimitType;	
	int32_t		LimitCount;
	int32_t		LimitStartTime;
	int32_t		LimitEndTime;
	int32_t		OriginalPrice;		//ԭ��
	int32_t		Price;
	int32_t		ShopType;
	int8_t		VipLevelLimit;		//�ȼ�
};
typedef std::map<int32_t,CfgGameShop> GameShopMap;

// 0~6 ת���� 1~7
inline int8_t GetWeekDay( int8_t tm_wday )
{
	return ( tm_wday + 6 ) % 7 + 1;
}

enum AUCTION_ITEM_TYPE
{
	AIC_ITEM	= 0,		// ��������
	AIC_MONEY	= 1,		// ͭǮ����
	AIC_GOLD	= 2,		// ħʯ����
};

#define MAX_ITEM_LIST	105 //��󱳰��ռ�

struct ChouJiangRecord
{
	MemChrBag	  CJItem;
	int32_t		  Time;		//�齱ʱ��
};
typedef std::list<ChouJiangRecord> CJRecordList;

enum VipCardType
{
	VT_TI_YAN_CARD		= 1,	//vip����
	VT_STAR_CARD		= 2,	//��֮ף����
	VT_MOON_CARD		= 3,	//��֮ף����
	VT_GOD_CARD			= 4,	//��֮ף����
};
#define  VIP_CARD_TYPE_COUNT	 4		//vip����
#define  MAX_VIP_LEVEL			12		//���vip�ȼ�
#define  SPECIAL_FLAG			1000    //����������������������¼id���1000

typedef std::set<int32_t> JoinedActivityId;		//�μӹ��Ļ
struct JoinedActivityInfo
{
	JoinedActivityId	YesterdayRecord;
	JoinedActivityId	JoinedIdSet;
};
typedef std::map<int32_t,JoinedActivityInfo> JoinedActivityRecord;

enum ACTIVITY_TYPE_ID
{
	ATI_FAMILY_WAR		= 1,	// ����ս
	ATI_HORSE_RACING	= 2,	// ����
	ATI_FAMILY_LIGHT	= 3,	// ����֮��
	ATI_MO_LING_RU_QIN	= 4,	// ħ������
	ATI_DA_TI_HUO_DONG	= 5,	// ����
	ATI_BAO_XIANG		= 6,	// ������
	ATI_SAFE_GUAJI		= 7,	// ��ȫ�һ�
	ATI_TERRITORY_WAR	= 8,	// ���ս
	ATI_SPIDER_QUEEN	= 9,	// ֩��Ů��
	ATI_CITY_WAR		= 10,	// 城战
	ATI_ACTIVITY_WORLD_BOSS	= 11,	// 活动世界Boss
	ATI_CAMP_WAR		= 12,	// 阵营战
	ATI_PEERLESS_WAR	= 13,	// 无双争霸
};
#define RACING_HD_SKILL_ID	130		//������ָ�����

enum TouZiType
{
	TT_MOON_CARD		= 1,	//�¿�Ͷ��
	TT_LEVEL_UP			= 2,	//����Ͷ��
};

typedef map<int16_t,int32_t>	RecordMap; 

enum KaiFuHuoDongCondition
{
	KFHDC_LEVEL_UP			= 1,		//����
	KFHDC_PET_RANK			= 2,		//�����Ǽ����а�
	KFHDC_PET_POINT			= 3,		//�������
	KFHDC_JUN_TUAN_LEADER	= 4,		//����ս���ų�
	KFHDC_JUN_TUAN_WIN		= 5,		//����ս��ʤ����Ա
	KFHDC_FAMILY_WAR_JOIN	= 6,		//����ս���뽱
	KFHDC_BATTLE_RANK		= 7,		//ս�������а�
	KFHDC_BATTLE_POINT		= 8,		//ս����ֵ
};


struct RankInfo
{
	CharId_t			CharId;
	std::string			CharName;
	int8_t				CharJob;
	int8_t				CharSex;
	int32_t				Param;
};
typedef std::map<int32_t,RankInfo> KaiFuHuoDongRank;

struct HuoYueDuRecord
{
	int32_t			FinishTimes;			//��ɴ���
	int8_t			IsSec;					//�Ƿ�����
};
typedef std::map<int32_t, HuoYueDuRecord> HuoYueDuRecordMap;

enum SkillAttackState
{
	SAS_NORMAL = 0,
	SAS_MISS = 1,//miss
	SAS_IGNORE = 2,//����
	SAS_VIOLENT = 3,//����
	SAS_PARRY = 4,//����
};

struct AchievementState
{
	int32_t Index;
	int32_t FinishCount;
	int8_t  AcState;
	int32_t Time;
};
typedef std::map<int32_t,AchievementState> AchievementMap;

#define FAMILY_LIGHT_MAP_PARA	4	//����֮���ͼ����
