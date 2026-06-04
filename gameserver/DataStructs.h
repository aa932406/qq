#pragma once

#include <cmath>
#include <list>
#include <map>
#include <string>
#include <vector>

//enum SkillResultType
//{
//	SRT_NONE = 0,
//
//	SRT_CD = 1,
//	SRT_DISTANCE = 2,
//	SRT_JOB = 3,
//	SRT_NUMBER = 4,
//	SRT_RANGE = 5,
//	SRT_EFFECT = 6,
//	SRT_CONTINUE = 7,
//
//	SRT_DEF_REDUCE_SELF = 20,
//	SRT_HP_REDUCE_SELF = 21,
//	SRT_ATTACK_ADD = 22,
//	SRT_DP_ADD = 23,//�������ܣ�
//	SRT_VP_ADD = 24,//���ӱ�����
//	SRT_WUDI = 25,
//	SRT_PDEF_ADD = 26,
//	SRT_MAXHP_INCREASE = 27,
//	SRT_HP_ADD_SELF = 28,
//	SRT_HP_ADD = 29,
//	SRT_MDEF_ADD = 30,
//	SRT_SP_REDUCE_SELF = 31,
//	SRT_COLLIDE = 32,
//	SRT_REBOUND = 33,//������
//
//	SRT_ATTCK = 50,
//	SRT_PDEF_REDUCE = 51,
//	SRT_ATTCK_REDUCE = 52,
//	SRT_SP_REDUCE = 53,
//	SRT_DP_REDUCE = 54,
//	SRT_HITP_REDUCE = 55,
//	SRT_WEAK_TARGET = 56,//make target weak
//	SRT_WEAK_ATTACK_ADDON = 57,
//	SRT_HP_REDUCE_DURABLE = 58,
//	SRT_IGNORE_DEF = 59,
//	SRT_MDEF_REDUCE = 60,
//	SRT_DIZZY_TARGET = 61,//ѣ�Σ�
//	SRT_FORCE_ATTACK_SELF = 62,//ǿ�ƹ�����
//	SRT_BINGDONG_TARGET = 63,//����;
//	SRT_IMBIBE_HP = 64,//��ȡĿ������ֵ;
//	SRT_BURN_TARGET = 65,//����Ŀ��;
//	SRT_BURN_ADDON = 66,//����Ŀ���˺�����
//	SRT_CONFUSION_TARGET = 67,//ʹĿ��������״̬
//	SRT_VP_REDUCE_TARGET = 68,
//
//	SRT_VICEGENERAL_PDEF_ADD = 90,
//	SRT_VICEGENERAL_MDEF_ADD = 91,
//	SRT_VICEGENERAL_ATTACK_ADD = 92,
//	SRT_VICEGENERAL_HP_ADD = 93,
//};
//
//enum SkillResultModify
//{
//	SRM_CD = 101,
//	SRM_DISTANCE = 102,
//	SRM_JOB = 103,
//	SRM_NUMBER = 104,
//	SRM_RANGE = 105,
//	SRM_EFFECT = 106,
//	SRM_CONTINUE = 107,
//
//	SRM_DEF_REDUCE_SELF = 120,
//	SRM_HP_REDUCE_SELF = 121,
//	SRM_ATTACK_ADD = 122,
//	SRM_DP_ADD = 123,//�������ܣ�
//	SRM_VP_ADD = 124,//���ӱ�����
//	SRM_WUDI = 125,
//	SRM_PDEF_ADD = 126,
//	SRM_MAXHP_INCREASE = 127,
//	SRM_HP_ADD_SELF = 128,
//	SRM_HP_ADD = 129,
//	SRM_MDEF_ADD = 130,
//	SRM_SP_REDUCE_SELF = 131,
//	SRM_COLLIDE = 132,
//	SRM_REBOUND = 133,//������
//
//	SRM_ATTCK = 150,
//	SRM_PDEF_REDUCE = 151,
//	SRM_ATTCK_REDUCE = 152,
//	SRM_SP_REDUCE = 153,
//	SRM_DP_REDUCE = 154,
//	SRM_HITP_REDUCE = 155,
//	SRM_WEAK_TARGET = 156,//make target weak
//	SRM_WEAK_ATTACK_ADDON = 157,
//	SRM_HP_REDUCE_DURABLE = 158,
//	SRM_IGNORE_DEF = 159,
//	SRM_MDEF_REDUCE = 160,
//	SRM_DIZZY_TARGET = 161,//ѣ�Σ�
//	SRM_FORCE_ATTACK_SELF = 162,//ǿ�ƹ�����
//	SRM_BINGDONG_TARGET = 163,//����;
//	SRM_IMBIBE_HP = 164,//��ȡĿ������ֵ;
//	SRM_BURN_TARGET = 165,//����Ŀ��;
//	SRM_BURN_ADDON = 166,//����Ŀ���˺�����
//	SRM_CONFUSION_TARGET = 167,//ʹĿ��������״̬
//	SRM_VP_REDUCE_TARGET = 168,
//
//	SRM_VICEGENERAL_PDEF_ADD = 190,
//	SRM_VICEGENERAL_MDEF_ADD = 191,
//	SRM_VICEGENERAL_ATTACK_ADD = 192,
//	SRM_VICEGENERAL_HP_ADD = 193,
//};

//enum FormationEffect
//{
//	FE_NONE                       =0,
//	FE_PDEF_ADD               = 1, //�������������
//	FE_MDEF_ADD              =2,//��߷���������
//	FE_ATTACK_ADD           =3,//��߹�����
//	FE_SKILL_EFFECT_ADD  =4,//��߼���Ч��
//
//};
//enum EquipElement
//{
//	EE_NONE = 0,
//	EE_METAL = 1,
//	EE_WOOD = 2,
//	EE_WATER = 3,
//	EE_FIRE = 4,
//	EE_EARTH = 5,
//	EE_NUM = 6,
//};
//
//enum EquipItemLifeReason
//{
//	EILR_NPC_SHOP,
//	EILR_DROP_ITEM,
//};

enum AcSpecialType
{
	AST_FAMILY_LEADER = 1,
	AST_FEUDAL_PRINCES,
	AST_KING,
	AST_TOP_KING,
	AST_FAMILY_NUMBER,
	AST_KILL_PLAYER,
	AST_OPEN_BAG_SLOT,
	AST_MOUNT_COUNT,
	AST_SPECIAL_MOUNT,
	AST_KILL_ONE_PLAYER,
};

enum CurrencyChangeReason
{

	GM_NONE								= 0,
	GM_DEBUG_CMD						= 1,

// ͭǮʹ��
 	MCR_CHR_SHOP_COST					= 2,		//�����̵�
 	MCR_MONEY_GAIN_ITEM					= 3,
 	MCR_TASK							= 4,		//��������
 	MCR_BUY_BACK_ITEM					= 5,		//����
 	MCR_DUNGEON_REWARD					= 6,		//���� ����
 	MCR_SUPER_EXCHANGE					= 7,		//�����һ� 
 	MCR_ACHIEVEMENT_EXCHANGE			= 8,		//�ɾͶһ�
 	MCR_PK_DROP							= 9,		//PKʱ������Ǯ��
 	MCR_GET_DROP_ITEM					= 10,		//����ʰȡ
	MCR_NPC_SELL						= 11,
	MCR_COMBI_ITEM						= 12,		// ��Ʒ�ϳ�

	MCR_EQUIP_EXCHANGE_STAR				= 37,		// װ��ǿ��ת��
	MCR_PLAYER_ZHUAN_SHENG_COST_MONEY	= 38,		// ����ת������ͭǮ
	MCR_TRADE_GET_MONEY					= 39,		// ���׻��
	MCR_TRADE_CONST_MONEY				= 40,		// ����ʧȥ
	MCR_PICK_CURRENCY_ITEM				= 41,		// ʰȡ��Դ���߻�ȡ
	MCR_MAIL_CURRENCY_ITEM				= 42,		// �ʼ���Դ���߻�ȡ
	MCR_DONATION_JUE_WEI				= 43,		// ��λ���
	MCR_CYCLY_TASK_GET					= 44,		// ѭ��������
	MCR_AUCTION_SELL					= 45,		// �������
	MCR_PET_TO_BAG						= 46,		// �������
	MCR_QI_FU							= 47,		// ����
	MCR_ENTER_DUNGEON					= 48,		// ���븱��
	MCR_ATTACK_FAMIWAR_PILLAR			= 49,		// ����ս����ս��֮��
	MCR_AUCTION_BUY						= 50,		// �����й���Ԫ��
	MCR_HORSE_RACING_REWARD				= 51,		// ��������
	MCR_ENTER_ACTIVITY					= 52,		// ���뿪���䱦��
	MCR_SEARCH_BACK						= 53,		// ��Դ�һ�
	MCR_GUAN_WEI_GET					= 54,		// ��λ���
	MCR_ACHIEVEMENT_FINISHED			= 55,		// ��ɳɾ�
//=======================================================================================
//��ħʯ��ʼ	
 	GCC_PICK_CURRENCY_ITEM				= 1000,
 	GCC_MAIL_CURRENCY_ITEM				= 1001,
 	GCC_BUY_SHANG_CHENG_ITEM			= 1002,
	GCC_WEEK_ONLINE_REWARD				= 1003,		//�����߽���
	GCC_ENTER_BOSS_HOME					= 1004,		//����boss֮��
	GCC_EQUIP_EXCHANGE_STAR				= 1005,		//ħ��ת��

//=========================================================================================
//ħʯ
	GCR_START							= 2000,
	GCR_QUICK_TASK						= 2001,		//��������
	GCR_ACHIEVEMENT_EXCHANGE			= 2002,		//�ɾͶһ�
	GCR_QUICK_ACHIEVEMENT				= 2003,		//���ٳɾ�
	GCR_DUNGEON_REWARD					= 2004,		//��������
	GCR_BUY_CYCLE_TASK_COUNT			= 2005,		//����ѭ������
	GCR_RECHARGE						= 2006,		//��ֵ
	GCR_CREATE_FAMILY					= 2007,
	GCR_USE_LIQUAN						= 2008,		//ʹ����ȯ
	GCR_SUPER_EXCHANGE					= 2009,		//�����һ�
	GCR_GET_DROP_ITEM					= 2010,		//����ʰȡ
	GCR_AUTOBUYBAG						= 2011,		//�Զ�������Ʒ
	GCR_MOUNT_QUALITY_UP				= 2012,		// ��������
	GCR_EQUIP_UPGRADE_AUTOBUY			= 2013,		// װ�������Զ�����
	GCR_EQUIP_UPQUALITY_AUTOBUY			= 2014,		// װ����Ʒ�Զ�����
	GCR_EQUIP_UPSTAR_AUTOBUY			= 2015,		// װ�������Զ�����
	GCR_BAG_SLOT_OPEN					= 2016,		// ��������
	GCR_DEPOT_GET						= 2017,		// �Ӳֿ���ȡ��
	GCR_DEPOT_SLOT_OPEN					= 2018,		// ����ֿ����
	GCR_TRADE_GET_GOLD					= 2019,		// ���׻��
	GCR_TRADE_CONST_GOLD				= 2020,		// ����ʧȥ
	GCR_RANSOM_ITEM						= 2021,		// ���
	GCR_PICK_CURRENCY_ITEM				= 2022,		// ʰȡ��Դ����
	GCR_MAIL_CURRENCY_ITEM				= 2023,		// �ʼ���Դ����
	GCR_PET_EGG_REHATCH					= 2024,		// �������·���
	GCR_PET_EGG_QUICK_HATCH				= 2025,		// ������ٷ���
	GCR_REVIVE							= 2026,		// ԭ�ظ���
	GCR_TASK_CYCLE_REFRESH_STAR			= 2027,		// �ճ�����ˢ���Ǽ�
	GCR_TASK_CYCLE_GET_REWARD			= 2028,		// �ճ�����౶��ȡ����
GCR_LEVEL_REFIN					= 2058,		// 等级精炼
	GCR_DONATION_JUE_WEI				= 2029,		// ��λ����
	GCR_FAMILY_CONTRIBUTE				= 2030,		// ���ɾ���
	GCR_BUY_FA_BAO_RES					= 2031,		// ���򷨱���Դ
	GCR_BUY_SHANG_CHENG_ITEM			= 2032,		// �����̳���Ʒ
	GCR_DUNGEON_DOUBLE_REWARD			= 2033,		// ����˫����ȡ
	GCR_DUNGEON_BUILD_TOWER				= 2034,		// ��������
	GCR_INSIDE_PET_EXP_ILLUSION_DOUBLE	= 2035,		// ˫������
	GCR_INSIDE_PET_EXP_ILLUSION_GOLD	= 2036,		// ħʯ����
	GCR_AUCTION_BUY						= 2037,		// �����й������
	GCR_AUCTION_SELL					= 2038,		// �����г���ħʯ
	GCR_CHOU_JIANG						= 2039,		// �齱
	GCR_PET_TO_BAG						= 2040,		// ��������������
	GCR_QI_FU							= 2041,		// ����
	GCR_ENTER_DUNGEON					= 2042,		// ���븱��
	GCR_GET_OFFLINE_EXP					= 2043,		// ��ȡ���߶౶
	GCR_BUY_VIP_CARD					= 2044,		// ����vip��
	GCR_ENTER_BOSS_HOME					= 2045,		// ����boss֮��
	GCR_EQUIP_EXCHANGE_STAR				= 2046,		// ħ��ת��
	GCR_BUY_TE_HUI_LI_BAO				= 2047,		// �����·��ػ����
	GCR_TOU_ZHI_HUO_DE					= 2048,		// Ͷ�ʻ��
	GCR_TOU_ZHI_SHI_QU					= 2049,		// Ͷ��ʧȥ
	GCR_HUO_YUE_DU						= 2050,		// ��Ծ��
	GCR_HALL_OF_FAME_BUY_TIMES			= 2051,		// �����ù������
	GCR_HALL_OF_FAME_CLEAR_CD			= 2052,		// ��������CD
	GCR_ITEM_COMBI_AUTO_BUY				= 2053,		// ��Ʒ�ϳ��Զ�����
	GCR_BUY_THREE_PET_LI_BAO			= 2054,		// �����������
	GCR_BUY_AC_SOCRE					= 2055,		// ����ѫ�»���
	GCR_BUY_WAR_VICTORY_GIFT			= 2056,		// ����սʤ�����
	GCT_WAR_VICTORY_FAN_LI				= 2057,		// сʤ����ֵ����
	GCR_EQUIP_GOLD_BACK				= 2058,		// 装备回收
	GCR_EQUIP_BUY_BACK				= 2059,		// 装备回购

//==================================================================================
//Ԫ��
	VCR_STRART							= 3000,
	VCR_KILL_MONSTER					= 3001,		// �������
	VCR_BASE_ILLUSION					= 3002,		// ���ó����
	VCR_GROW_ILLUSION					= 3003,		// ���ó����
	VCR_LUCKY_ILLUSION					= 3004,		// ���ó�����
	VCR_EXP_ILLUSION					= 3005,		// ���ó�����


//=======================================================================================
//boss���ֺͳ齱����
	SCR_ON_CHOU_JIANG					= 4000,		//�齱
	SCR_KILL_BOSS						= 4001,		//ɱ��
	SCR_BUY_ITEM						= 4002,		//������Ʒ

//=======================================================================================
// ����
	HCR_HALL_OF_FAME_FIGHT				= 5000,		// ��ս
	HCR_BUY_GUAN_WEI					= 5001,		// �����λ
	HCR_HALL_OF_FAME_RANK_REWARD		= 5002,		// ��������

//�ɾͻ���
	ACHIEVEMNET_SOCRE_GET				= 6001,		// �ɾ�	
	ACHIEVEMNET_UP_XUN_ZHANG			= 6002,		// ����ѫ��
	ACHIEVEMENT_BUY_SOCRE				= 6003,		// �������
};

enum ITEM_ADD_REASON
{
 	IACR_NONE					= 0,
 	IACR_DUNGEON_REWARD			= 1,	//���� ����
 	IACR_MAIL					= 2,	//�ʼ���ȡ
 	IACR_EXCHANGE_CODE			= 3,	//���� ��
 	IACR_ACHIEVEMENT_EXCHANGE	= 3,	//�ɾ� �һ�
 	IACR_ACTIVITY				= 4,	//� ����
 	IACR_PICK					= 5,	//ʰȡ
	IACR_TIAN_JIANG_BAO_XIANG_DAILY_REWARD	= 9,	// �߽������ճ�����
 	IACR_LIBAO					= 6,	//���
 	IACR_TASK					= 7,	//���� ����
 	IACR_ONLINE_REWAR			= 8,	//�� �� ����
 	IACR_COMBINE				= 9,	//�ϲ�
 	IACR_CHR_SHOP				= 10,	//���� �̵� ����
 	IACR_SIGN					= 11,	//ǩ������
 	IACR_SEVEN_LOGIN			= 12,	//�����½
 	IACR_REMOVE_GEM				= 13,	//������ʯ
 	IACR_SUIJILIBAO				= 14,	//������
 	IACR_YELLOW_STONE_REWARD	= 15,	//����ÿ�����
 	IACR_YEAR_YELLOW_STONE_REWARD	= 16,	//��ѻ������ÿ�����
 	IACR_YELLOW_STONE_GROW_UP		= 17,	//����ɳ����
	IACR_DEPOT_GET				= 18,	//�ֿ��ȡ
	IACR_DEPOT_SAVE				= 19,	//����ֿ�
	IACR_GEM_TO_BAG				= 20,	//ժȡ��ʯ
	IACR_RANSOM_ITEM			= 21,	//���
	IACR_TRADE					= 22,	//����
	IACR_BUY_SHANG_CHENG_ITEM	= 23,	//�̳ǹ���
	IACR_CHOU_JIANG				= 24,	//�齱
	IACR_BUY_CHOU_JIANG_SHOP	= 25,	//�齱�̵깺��
	IACR_FROM_PET				= 26,	//�ӻ�������
	IACR_DWW_HD					= 27,	//��θ���
	IACR_GET_VIP_GIFT			= 28,	//��ȡvip���
	IACR_PALNT_GET				= 29,   //�ɼ�������
	IACR_EXP_BALL				= 30,	//��������
	IACR_SHOU_CHONG				= 31,	//�׳�	IACR_NEW_SERVER_TE_HUI			= 32,
	IACR_SCORE_SHOP_COST			= 33,
	IACR_SCORE_SHOP_GET				= 34,	//�·��ػ�
	IACR_EVERYDAY_CHONG_ZHI		= 33,	//ÿ�ճ�ֵ
	IACR_KAI_FU_HUO_DONG		= 34,	//�����
	IACR_HUO_YUE_DU				= 35,	//��Ծ��
	IACR_HALL_OF_FAME_REWARD	= 36,	//�����ý���
	IACR_VIP_CARD_GIFT			= 37,	//VIP�����
	IACR_FAMILY_TASK_REWARD		= 38,	//��������
	IACR_TOTAL_CHONG_ZHI		= 39,	//�����ۼƳ�ֵ
	IACR_VAR_VICTORY			= 40,	//��ս70����
	IACR_EQUIP_BUY_BACK			= 41,	// 装备回购
	IACR_KAI_HUO_EQUIP_BACK		= 42,	// 开服活动装备回收
};

enum ITEM_DEL_REASON
{
 	IDCR_NONE						= 0,
 	IDCR_ACHIEVEMENT_QUICK			= 1,	// �ɾ� �������
 	IDCR_BAG_USE					= 2,	// ����  ʹ��
 	IDCR_ADD_GEM					= 3,	// ��Ƕ
 	IDCR_FLY						= 4,	// ����
 	IDCR_DUNGEON					= 5,	// ����  ����
 	IDCR_COMBINE					= 6,	// �ϲ�
	IDCR_VICEGENERAL_USE_EXP_PILL	= 7,	// �佫ʹ�þ���
	IDCR_EQUIP_UPSTAR				= 8,	// װ������
	IDCR_EQUIP_UPGRADE				= 9,	// װ������
	IDCR_EQUIP_UPQUALITY			= 10,	// װ����Ʒ
	IDCR_MOUNT_HEART				= 11,	// ��������֮��
	IDCR_MOUNT_LEAR_SKILL			= 12,	// ѧϰ���＼��
	IDCR_MOUNT_SKILL_UP				= 13,	// �������＼��
	IDCR_MOUNT_LEVEL_UP				= 14,	// ��������
	IDCR_EQUIP_OPEN_GEM_HOLE		= 15,	// ��ʯ����
	IDCR_GEM_TO_EQUIP				= 16,	// ��ʯ��Ƕ
	IDCR_TRADE						= 17,	// ���� 
	IDCR_DIE_DROP					= 18,	// ��������
	IDCR_PET_USE_ITEM				= 19,	// ����ʹ�õ���
	IDCR_SKILL_LEVEL_UP				= 20,	// ��������
	IDCR_BACK_CITY					= 21,	// ʹ�ûسǾ�
	IDCR_SUI_JI						= 22,	// ʹ�������
	IDCR_INSIDE_PET_EXP_ILLUSION	= 23,	// ���ó�����
	IDCR_INSIDE_PET_USE_LUCKY_ITEM	= 24,	// ���ó�ʹ������ֵ����
	IDCR_INSIDE_PET_CHANGE_PHASE	= 25,	// ���ó�ı�����
	IDCR_AUCTION_SELL				= 26,	// ����
	IDCR_CHOU_JIANG					= 27,	// �齱
	IDCR_TO_PET						= 28,	// ��������
	IDCR_QI_FU						= 29,	// ��������
	IDCR_FAMILY_WAR_SIT_REVIVE		= 30,	// ����սԭ�ظ���
	IDCR_FAMILY_WAR_PET_REVIVE		= 31,	// ����ս�������
	IDCR_PUT_IN_EXP_BALL			= 32,	// װ��������
	IDCR_USE_MULTI_ITEM				= 33,	// ʹ�ö��
	IDCR_ENTER_BOSS_HOME			= 34,	// ����boss֮��
	IDCR_SITE_REVIVE				= 35,	//
	IDCR_SCORE_SHOP_COST			= 36,	// 积分商店消耗
	IDCR_EQUIP_GOLD_BACK			= 37,	// 装备回收
};

enum ItemEffectType
{
	IET_NONE						= 0,
	IET_MONEY_GAIN					= 1,	// ���
	IET_CHIXUHUIXUE					= 2,	// ������Ѫ
	IET_SHUNJIANHUIXUE				= 3,	// ˲���Ѫ
	IET_CHONGHUIXUE					= 4,	// �����Ѫ
	IET_CHIXUHUIMO					= 5,	// ������ħ
	IET_SHUJIANHUIMO				= 6,	// ˲���ħ -- ����

	IET_JINGYANDAN					= 24,	// ���鵤

	IET_GIFT						= 29,	// ���
	IET_AUTO_PET_GIFT				= 30,	// �Զ�ʹ�õĳ������

	IET_EXPERIENCECARD				= 33,	// VIP ���鿨

 	IET_LIQUAN						= 55,	// ��ȯ

 	IET_YUANBAO						= 70,	// Ԫ��

 	IET_SUIJILIBAO					= 72,	// ������

 	IET_SUPER_EXCHANGE				= 75,	// �����һ�
 	IET_MAXHP_DAOJU					= 76,	// ���Ѫ��buff����
 	IET_ATTACK_DAOJU				= 77,	// ������buff����
 	IET_VP_DAOJU					= 78,	// ���ӱ���buff����
 	IET_MUTI_EXP					= 79,	// �౶���鵤

	IET_CURRENCY_RES				= 82,	// ��Դ��Ʒ

	IET_SHU_GUANG_SUI_PIAN			= 98,	// �����Ƭ
	IET_DI_LONG_SUI_PIAN			= 99,	// ������Ƭ
	IET_FU_WEN_SUI_PIAN				= 100,	// ������Ƭ
	IET_PET_ADD_THIRD				= 101,	// ������

	IET_ENTER_DUNGEON				= 103,	// ���븱������
	IET_FAMOLY_WAR_PET_REVIVE		= 104,	// ����ս�������
	IET_FAMOLY_LIGHT_WINE			= 105,	// �����Ҿ�
	IET_LEVEL_UP					= 106,	// ������
	IET_JIU_SHU_CARD				= 135,	// ���꿨
	IET_HUN_LI_UP				    = 136,	// ����������
};


// �������
enum ITEM_SPECIAL_ID
{
	ISI_OPEN_THIRD_GEM_HOLE			= 7002,		// װ������������
	ISI_OPEN_FIRST_GEM_HOLE			= 7003,		// װ������һ����
	ISI_OPEN_SECOND_GEM_HOLE		= 7004,		// װ�����ڶ�����
	ISI_RELIVE						= 33,		// �������
	ISI_TELEPORT					= 34,		// ���͵���
	ISI_PET_KIWI_FRUIT				= 7036,		// �����
	ISI_PET_ELECTRIC_POTION			= 7037,		// �罬ҩˮ
	ISI_PET_EXP_GRAIN				= 7038,		// ʥ��ħ��
	ISI_EXP_BALL					= 7039,		// ��ͨ������
	ISI_SPECIAL_EXP_BALL			= 7040,		// �ؼ�������
	ISI_PET_LUCKY_GRASS				= 7041,		// ���˲�
	ISI_PET_GOD_GRASS				= 7042,		// ���ɲ�
	ISI_BACK_CITY_JUAN				= 7078,		// �سǾ�
	ISI_SUI_JI_JUAN					= 7079,		// �����
	ISI_ZHI_ZUN_PI_SA				= 7188,		// ��������
	ISI_FAMILY_WAR_SIT_REVIVE		= 7227,		// ����սԭ�ظ���
	ISI_EMPTY_EXP_BALL				= 7278,		// �վ�����
	ISI_XIAO_JV_HUA					= 7289,		// С�ջ�
	ISI_XIAO_LA_BA					= 7290,		// С����
	ISI_FU_HUO_DAN					= 7308,		// ���
};

enum  AcDailyType
{
	 ADT_MOUNT_COUNT = 1,
	 ADT_FENGYINBANG = 2,
	 ADT_KUNLUNJING = 3,
	 ADT_JIUTIANTA = 4,
	 ADT_CYCLE_TASK = 5,
	 ADT_VICEGENERAL = 6,
	 ADT_FAMILY_TASK = 7,
	 ADT_KINGDOM_TASK =8,
	 ADT_DAILY_ITEM =9,
};

enum  AchievementWorkType
{
	AWT_MUST = 1,
	AWT_NOT_BE = 2,
};

enum AcGrowUpType
{
	AGUT_PLAYER_LEVEL = 1,
	AGUT_ENHANCE =2,
	AGUT_STAATEGICS = 3,
	AGUT_USER_PILL_COUNT = 4,
	AGUT_MOUNT_LEVEL = 5,
	AGUT_MOUNT_XISUI = 6, 
	AGUT_LACKEY_LEVEL = 7,
	AGUT_SKILL_LEVEL = 8,
	AGUT_VICEGENERAL_WUYI_LEVEL= 9,
	AGUT_VICEGENERAL_GENGU_LEVEL = 10,
	AGUT_ZHENFA = 11,
	AGUT_SHENGWANG = 12,
	AGUT_GEM =13,
};

enum ItemBroadcast
{
	IB_PICK = 1,
	IB_TREASURE = 2,
};


enum GainType
{
	GT_EXP			= 1,
	GT_MONEY		= 2,
	GT_GOLD			= 3,
	GT_CASH			= 4,
	GT_SHU_GUANG	= 5,		// �����Ƭ
	GT_DI_LONG		= 6,		// ������Ƭ
	GT_FU_WEN		= 7,		// ������Ƭ
	GT_CHOU_JINAG	= 8,		// �齫����
	GT_BOSS_SCORE	= 9,		// BOSS����
	GT_BIND_MONEY	= 10,		// �󶨽��
	GT_HONOR		= 11,		// ����
	GT_AC_SOCRE		= 12,		// �ɾͻ���
};

enum NpcFunc
{
	NF_NORMAL	= 0,	// NpcFuncExtra
	NF_TASK		= 1,	// ����
};

enum NpcFuncExtra
{
	NFE_INTRODUC		= 1034,		// ����
	NFE_EQUIPSET		= 1036,		// װ��ǿ��
	NFE_ROLE			= 1037,		// ��ɫ
	NFE_GEMMY			= 1038,		// ��ʯ��Ƕ
	NFE_GANGINFO		= 1039,		// ������Ϣ
	NFE_EQUIPBUILD		= 1040,		// װ������
	NFE_PETWARE			= 1041,		// ���ֿ�
	NFE_SINGLEFUBEN		= 1042,		// ���˸���
	NFE_VIPFUBEN		= 1043,		// vip����
	NFE_MULTIFUBEN		= 1044,		// ���˸���
	NFE_EXPFUBEN		= 1045,		// ���鸱��
	NFE_ZHUSHEN			= 1046,		// ���񸱱�
	NFE_STALL			= 1047,		// ̯λ
	NPE_MLRQ			= 1048,		// ħ������
};

enum ActionType
{
	AT_NONE = 0,
	AT_ITEM = 1,
	AT_SKILL = 2,
};

enum PlayerTeamStatus
{
	PTS_FREE = 0,
	PTS_TEAM_LEADER = 1,
	PTS_TEAM_MEMBER = 2,
};

#define  ONLINE_REWARD_BEGIN_ID  8
enum PlayerRecord
{
	//��ʱ�ļ�¼��ʼ-----
	PR_MIN_TEMP_VALUE						= 0,	
	PR_IN_BOSS_HOME							= 1,		//��boss֮�ұ��
	PR_MAX_TEMP_VALUE						= 1000,
	//��ʱ�ļ�¼����-----

	PR_DAILY_CHECK_LAST_UPDATE				= 1001,		// �����ˢ��ʱ��
	PR_MINUTE_CHECK_TIME					= 1002,
	PR_TRAILER_KINGDOM						= 1003,
	PR_TRAILER_MAPID						= 1004,
	PR_TRAILER_X							= 1005,
	PR_TRAILER_Y							= 1006,
	PR_TRAILER_HP							= 1007,
	PR_TRAILER_END_TIME						= 1008,
	PR_WORLD_CHAT_LAST_TIME					= 1009,		//�������ʱ��
	
	RP_PK_RANK_COUNT						= 1010,		// ÿ�� ɱ�˼�¼
	RP_LOGIN_COUNT							= 1011,		// ��½���� 
	PR_DEAD_TIME							= 1012,		// ��������
	PR_FLY_ICON_INT							= 1013,		// ���ڱ���ͼ����еı��
	RP_DAILY_SEVEN_LOGIN_REWARD				= 1014,		// �����½������ȡ��Ϣ
	RP_LEVEL_GIFT_REWARD					= 1015,		// �ȼ������ȡ���
	RP_WEEK_ONLINE_TIME						= 1016,		// ������ʱ��
	RP_LAST_WEEK_ONLINE_TIME				= 1017,		// ��������ʱ��
	PR_WEEK_CHECK_LAST_UPDATE				= 1018,		// ����ˢ��ʱ��
	PR_ACCUMULATIVE_OFFLINE_TIME			= 1019,		// �ۼ�����ʱ��
	PR_VIP_EXP								= 1020,		// vip����
	PR_VIP_GIFT_RECORD						= 1021,		// vip�����¼
	RP_VIP_LAST_ADD_EXP_TIME				= 1022,		// �������vip�����ʱ��
	PR_MAX_VIP_LEVEL						= 1023,		// ���vip�ȼ�
	RP_IN_BOSS_HOME_ENT_TIME				= 1024,		// ��boss֮�ҵ����ʱ��
	RP_CYCLE_FLUSH							= 1025,		// ѭ������ˢ���ܴ���
	RP_SHOU_CHONG_LI_BAO					= 1026,		// �׳�״̬
	RP_BUY_TE_HUI_RECORD					= 1027,		// ���򿪷��ػݼ�¼
	RP_KAI_FU_HUO_DONG_LEVEL				= 1028,		// ������ڼ�ﵽ�ĵȼ�
	RP_KAI_FU_HUO_DONG_PET_POINT			= 1029,		// �������������Ǽ�
	RP_FIRST_JUN_TUAN_ZHAN_STATE			= 1030,		// ��һ�ξ���ս���
	RP_KAI_FU_HUO_DONG_BATTLE				= 1031,		// ������ڼ����ս����
	RP_KAI_FU_HUO_DONG_REWARD				= 1032,		// �����������ȡ���
	PR_FALL_OF_FAME_CD_TIME					= 1033,		// ������CDʱ��
	RP_CUR_GUAN_WEI							= 1034,		// ��ǰ��λ
	RP_THREE_PET_GIFT						= 1035,		// ��������Ƿ���ȡ
	RP_TOTAL_CHI_ZHI_VALUES					= 1036,		// �����ۼƳ�ֵ�
	RP_TOTAL_CHI_ZHI_REWARD					= 1037,		// �����ۼƳ�ֵ��ȡ��¼
	RP_WAR_VICTORY_CHONG_ZHI_VALUES			= 1038,		// ��սʤ�����ֵ

	// ÿ�������ļ�¼��ʼ----------------------------------
	PR_DAILY_CLEAR_BEGIN					= 2000,
	RP_DAILY_PK_MARK						= 2001,		// ÿ��PK����
//	RP_EVERY_DAY_GET_GOLD					= 2002,		// ��� ÿ����ȡԪ��
	RP_DAILY_ONLIN_TIME						= 2003,		// ÿ������ʱ��
	RP_DAILY_ONLIE_REWARD_INFO				= 2004,		// ÿ������ʱ�佱��
	RP_DAILY_DWW_TIMES						= 2005,		// ��θ����������
	RP_DAILY_DWW_LAST_TIME					= 2006,		// ����μӴ�θ��
	RP_IS_USED_VIP_TI_YAN_CARD				= 2007,		// �Ƿ�����ʹ��vip���鿨
	RP_IN_BOSS_HOME_TIME					= 2008,		// ��boss֮�ҵ�ʱ��
	PR_KILL_TRAILER_COUNT					= 2009,
	PR_SEARCH_BACK_REWARD_INFO				= 2010,		// ��Դ�һؼ�¼
	PR_EVERY_DAY_CHONG_ZHI_GIFT_RECORD		= 2011,		// ÿ�ճ�ֵ�����ȡ���
	PR_HUO_YUE_DU_REWARD_INFO				= 2012,		// ��Ծ����ȡ���
	PR_XIAO_JV_HUA_SHI_YONG					= 2013,		// С�ջ�ʹ������
	PR_TERRITORY_WAR_REVIVE_TIMES			= 2014,		// ���ս�����¼
	PR_FALL_OF_FAME_FIGHT_TIMES				= 2015,		// ��ս����
	PR_FALL_OF_FAME_BUY_TIMES				= 2016,		// �������
	RP_CUR_GUAN_WEI_DAILY_REWARD			= 2017,		// ��λ�ճ�������Ϣ
	RP_GET_VIP_CARD_GIFT					= 2018,		// ��ȡvip�����
	RP_FAMILY_TASK_COUNT					= 2019,		// �����������
	RP_FAMILY_TASK_REWARD					= 2020,		// ����������
	RP_VIP_SITEREVIVE_TIMES					= 2021,		// vipÿ��ԭ�ظ������
	RP_WAR_VICTORY_LOGIN					= 2022,		// ÿ�տ�սʤ��ÿ�յ�½�����ȡ���
	RP_WAR_VICTORY_BUY_GIFT					= 2023,		// ��սʤ��ÿ���ػݹ������

	RP_DUNGEON_DAILY_START					= 3000,		// ����ÿ���콱��¼��ʼ
	RP_DUNGEON_DAILY_END					= 3999,		// ����ÿ���콱��¼����
	RP_YESTERDAY_DUNGEON_RECORD_START		= 4000,		// ���ո���������ʼ
	RP_YESTERDAY_DUNGEON_RECORD_END			= 4999,		// ���ո�����������

	PR_YEllOW_DAILY							= 7000,		// ����ÿ�����
	PR_YEAR_YEllOW_DAILY					= 7001,		// ��ѻ������ÿ�����

	PR_BUY_CYCLE_TIMES						= 8003,		// ����ѭ���������
	PR_QI_FU_MONEY							= 8004,		// ����ͭǮ
	PR_QI_FU_EXP							= 8005,		// ��������
	PR_QI_FU_MONEY_TOTAL					= 8006,		// ����ͭǮ�ܼ�
	PR_QI_FU_EXP_TOTAL					= 8007,		// ���������ܼ�

	PR_ACTIVITY_DAILY_REAWRD_START			= 9000,		// �ÿ�ս�����ʼ
	RP_ACTIVITY_DAILY_REAWRD_END			= 9999,		// �ÿ�ս�������

	PR_DAILY_CLEAR_END						= 19999,
	// ÿ�������ļ�¼����----------------------------------

	PR_DUNGEON_COMPLETED_BEING				= 20000,	// ����ͨ�ؼ�¼��ʼ
	PR_DUNGEON_COMPLETED_END				= 29999,	// ����ͨ�ؼ�¼����

	PR_GEM_SLOT_RECORD						= 37200,	// ��ʯ��Ƕ�׿���״̬
	PR_TEAM_AUTO_ACCEPT_INVITE				= 37201,
	PR_TEAM_AUTO_ACCEPT_APPLY				= 37202,
	PR_FIRST_ENTER_GAME						= 37203,
	PR_LEARNED_XP_SKILL						= 37204,
	PR_OPEN_THIRED_FIGHT_PET				= 37205,	// ������
	PR_LAST_FINISH_UP_TOWER_DUNGEON			= 37206,	// �����ɵ���������
	PR_RETROACTIVE_TIMES					= 37207,	// ��ǩ����
};

enum PlayerAction
{
	PA_STAND = 1,
	PA_SIT = 17,
	PA_COLLECT = 16,
};


#define  PK_PROTECT_TIME 3600
enum PkMode
{
	PK_MODE_FREE		= 0,	//��Ч
	PK_MODE_PEACE		= 1,	//��ƽģʽ
	PK_MODE_TEAM		= 2,	//���ģʽ
	PK_MODE_LEGION		= 3,	//����ģʽ
	PK_MODE_GOOD_EVIL	= 4,	//�ƶ�ģʽ
	PK_MODE_SERVER		= 5,	//����ģʽ
	PK_MODE_ALL			= 6,	//ȫ��ģʽ	
	PK_MODE_COUNT,
};

enum NameColour
{
	NAME_COLOUR_WHITE	= 1,	//����
	NAME_COLOUR_RED		= 2,	//����
	NAME_COLOUR_BLACK	= 3,	//����
};

enum PkValue
{
	PK_VALUE_WHITE		= 20,	//����pkֵ
	PK_VALUE_RED		= 100,	//����pkֵ
};

enum QiFuType
{
	QT_MONEY = 1,			//����ͭǮ
	QT_EXP	 = 2,			//��������
};

enum MonsterType
{
	MT_MONSTER = 0,
	MT_TRAP = 1,
};

enum ExchangeGetType
{
	EGT_ITEM = 1,
	EGT_EQUIP = 2,
	EGT_MONEY = 3,
	EGT_EXP = 4,
	EGT_RAND_ITEM = 11,
};

//��ͼ���
enum MapType
{
	MT_NORMAL		= 1,		// ��ͨ��ͼ
	MT_DUNGEON		= 2,		// ������ͼ
	MT_ACTIVITY		= 4,		// ���ͼ
};

enum PlantType
{
	PT_NORMAL			= 0,
	PT_GRADE_BAO_XIANG	= 1,	//��еĸ߼�����
};

enum RegionType
{
	RNT_SAFE = 1,
	RNT_TRADE = 2,
	RNT_TASK = 3,
};

enum InstanceMoveReason
{
	IMR_PULL_BACK	= 1,		// ����
	IMR_TRANSFER	= 2,		// ����
	IMR_SKILL		= 3,		// ����
	IMR_ACTIVITY	= 4,
	IMR_JUMP		= 5,
	IMR_SPOUSE_MOVE = 6,
	IMR_HIT			= 7,
	IMR_OPEN_STALL	= 8,		// ��̯
};

enum SuperExchangeType
{
	SET_EXP = 1,
	SET_MONEY = 2,
	SET_JUNGONG = 3,
	SET_JIANGXING = 4,
	SET_WUHUN = 5,
	SET_CASH = 6,
	SET_GOLD = 7,
};

enum BenefitType
{
	BT_NORMAL = 1,
	BT_TIRED = 2,
	BT_HURT = 3,
};

enum ConsumeCostType
{
	CCT_NONE = 0,
	CCT_MONEY = 1,
	CCT_SYS_GOLD = 2,
	CCT_GOLD = 3,
};

struct SuperExchangeToItem
{
	int32_t type;
	int32_t value;
};

typedef std::vector<SuperExchangeToItem> SuperExchangeToItemVector;

#define PULL_BACK_DISTANCE 6
#define TRANSFER_DISTANCE 18

#define MAX_DROPITEM_SIZE 60

#define PLAYER_GENERAL_CD_MS 1250
#define ITEM_CD_GROUP_COUNT 7

#define MAX_UNIT_BUFF_NUM 14

typedef std::list<int8_t> Int8List;
typedef std::vector<int8_t> Int8Vector;
typedef std::list<int16_t> Int16List;
typedef std::vector<int16_t> Int16Vector;
typedef std::list<int32_t> Int32List;
typedef std::vector<int32_t> Int32Vector;
typedef std::map<int32_t, int32_t> Int32Int32Map;
typedef std::map<int32_t, Int32Vector> Int32Int32VectorMap;
typedef std::list<CharId_t> CharIdList;
typedef std::map<EntityId_t, int32_t> EntityIdInt32Map;

struct DropItem 
{
	int32_t itemId;
	int8_t	itemClass;
	int8_t	bindType;
	int32_t itemCount;
	Time_t	endTime;	
	int64_t	srcId;
	int32_t costType;
	int32_t costValue;
};

struct DungeonNumReward
{
	int32_t exp;
	int32_t money;
	int32_t gold;
	int32_t jungong;
	int32_t wuhuen;
	int32_t jiangxing;
};
 
struct  DungeonGroupReward
{
	  int32_t dungeonID;
	  MemChrBagVector items;
};
typedef std::vector<DungeonGroupReward>DungeonGroupRewardVector;

struct HPEvent
{
	int32_t id;
	int32_t minhp;
	int32_t maxhp;
};
typedef std::list<HPEvent> HPEventList;

struct EquipPatchEnhanceInfo
{
	 int32_t money;
	 int32_t time;
};


struct UnitAttr 
{
	int32_t hp;			// ����ֵ
	int32_t mp;			// ����ֵ
	int32_t pp;			// ����ֵ
	int32_t xp;			// ��ɱֵ
};

struct UnitHandle 
{
	explicit UnitHandle( EntityId_t id_ = 0, int32_t type_ = 0 )
		: id(id_), type(type_)
	{

	}

	bool operator==(const UnitHandle &param) const
	{
		return id == param.id && type == param.type;
	}

	bool operator!=(const UnitHandle &param) const
	{
		return id != param.id || type != param.type;
	}

	bool isEmpty() const
	{
		return id == 0 || type == 0;
	}

	void clear()
	{
		id = 0;
		type = 0;
	}

	EntityId_t id;
	int32_t type;
};

struct AttrAddon
{
	int32_t index;
	int32_t addon;
};
typedef std::list<AttrAddon> AttrAddonList;
typedef std::vector<AttrAddon> AttrAddonVector;
typedef std::map<int32_t,AttrAddon>AttrAddonTable;

struct AttrAddonArtifact
{
	int32_t index;
	int32_t addon;
	int32_t ratio;
};
typedef std::vector<AttrAddonArtifact> AttrAddonArtifactVector;

struct SkillResult 
{
	explicit SkillResult(EntityId_t unitid_ = 0, int32_t unittype_ = 0, int32_t hpchange_ = 0, int32_t state_ = 0, int32_t buffid_ = 0, int32_t buffDuration_ = 0)
		: unitid(unitid_), unittype(unittype_), hpchange(hpchange_), attackState(state_), buffid(buffid_), buffDuration(buffDuration_)
	{

	}

	EntityId_t unitid;
	int32_t unittype;
	int32_t hpchange;
	int32_t attackState;
	int32_t buffid;
	int32_t buffDuration;
};
typedef std::vector<SkillResult> SkillResultVector;

struct Position 
{
	explicit Position(int32_t x_ = 0, int32_t y_ = 0)
		: x(x_), y(y_)
	{

	}

	bool operator==(const Position &param) const
	{
		return x == param.x && y == param.y;
	}

	bool operator!=(const Position &param) const
	{
		return x != param.x || y != param.y;
	}

	double distance(const Position &pos) const
	{
		return sqrt(static_cast<double>((pos.x-x)*(pos.x-x) + (pos.y - y)*(pos.y - y)));
	}

	int32_t tileDistance(const Position &pos) const
	{
		return (std::max)(std::abs(x-pos.x), std::abs(y-pos.y)/2);
	}

	int32_t x;
	int32_t y;
};
typedef std::vector<Position> PositionVector;

struct TaskRequest 
{
	int32_t param1;
	int32_t param2;
	int32_t param3;
};

struct activityRankData
{
	int32_t rank;
	int32_t id;//����ID������ID�����ID
	int32_t count; //�������Σ�����ʱ�ã�
	int32_t jifen;
	int32_t aid;
	int32_t kingDomId;

	activityRankData()
	{
		reset();
	}

	void reset()
	{
		rank=0;
		id=0;
		count =0;
		jifen=0;
		aid=0;
		kingDomId = 0;
	}
};
typedef std::vector<activityRankData> ActivityRankVector;

struct FormationLocationType
{
	int32_t        isplayer;
	int32_t        roleID;
    
	bool operator==(const FormationLocationType&LocationType)
	{
		return (isplayer == LocationType.isplayer && roleID == LocationType.roleID);
	}

	void operator = (const FormationLocationType&LocationType)
	{
		   isplayer = LocationType.isplayer;
		   roleID   = LocationType.roleID;
	}
};

struct FormationRoleLocation
{
	FormationLocationType  frontRoleID;
	FormationLocationType  leftMidRoleID;
	FormationLocationType  mainMidRoleID;
	FormationLocationType  rightMidRoleID;
	FormationLocationType  leftBacktRoleID;
	FormationLocationType  mainBacktRoleID;
	FormationLocationType  rightBackRoleID;

	bool operator==(const FormationRoleLocation &roleLocation)
	{
		return  (frontRoleID == roleLocation.frontRoleID        && leftMidRoleID ==roleLocation.leftMidRoleID   && leftMidRoleID ==roleLocation.leftMidRoleID           && 
			rightMidRoleID == roleLocation.rightMidRoleID   && leftBacktRoleID == roleLocation.leftBacktRoleID  && mainBacktRoleID ==roleLocation.mainBacktRoleID   && 
			rightBackRoleID ==roleLocation.rightBackRoleID  );
	}

	void operator = (const FormationRoleLocation &roleLocation)
	{
		frontRoleID = roleLocation.frontRoleID   ;        
		leftMidRoleID  = roleLocation.leftMidRoleID ;    
		leftMidRoleID  = roleLocation.leftMidRoleID ;
		rightMidRoleID  = roleLocation.rightMidRoleID ;
		leftBacktRoleID  = roleLocation.leftBacktRoleID ;
		mainBacktRoleID  = roleLocation.mainBacktRoleID ;
		rightBackRoleID = roleLocation.rightBackRoleID ;
	}
};

struct AcChallengeFBInfo
{
	  int32_t   id;
	  int32_t   point;
};
typedef std::vector<AcChallengeFBInfo>AcChallengeFBInfoVector;

struct AcExchangeItem
{
       int32_t id;
	   int32_t count;
	   Job_t job;
};
typedef std::vector<AcExchangeItem>AcExchangeItemVector;

struct AcExchange
{
	int32_t group;
	int32_t point;
	int32_t group_first;
	int32_t money;
	int32_t exp;
	int32_t gold;
	int32_t jiangxing;
	int32_t wuhuen;
	AcExchangeItemVector item;
};

struct TaskMonster 
{
	int32_t tid;
	int32_t condition;
	TaskRequest request;
};

struct DailyRecord
{
     int32_t id;
	 int32_t mark;
	 int32_t need_count;
	 int32_t count;
	 int32_t type;
};
typedef std::vector<DailyRecord>DailyRecordVector;
typedef std::map<int32_t,DailyRecord>DailyRecordTable;

struct FamilyRegion
{
	int32_t family_index;
	int32_t x;
	int32_t y;
};
typedef std::vector<FamilyRegion>FamilyRegionVector;
typedef std::map<int32_t,FamilyRegion>FamilyRegionTable;


#define TASK_CYCLE_GROUP_COUNT 10

enum MonsterBroadcastState
{
	MBS_INACTIVE,
	MBS_ALIVE,
	MBS_DEAD,
};

struct MountAttr
{
	int32_t attr;
	int32_t pdef;
	int32_t mdef;
	int32_t hp;
	int32_t vp;
	int32_t bp;
	int32_t htip;
};
typedef std::vector<MountAttr> mountAttrVector;

struct MonsterBroadcast 
{
	int32_t mid;
	int32_t state;
	std::string last_killer;
	int32_t next_revive;
	int32_t kingdom;
};
typedef std::vector<MonsterBroadcast> MonsterBroadcastVector;


struct MonsterSkill 
{
	int32_t maxHp;
	int32_t minHp;
	int32_t skillid;
	bool	done;
};
//typedef std::vector<MonsterSkill> MonsterSkillVector;

struct MonsterWave 
{
	bool alive()
	{
		return count > killed+escaped;
	}
	int32_t count;
	int32_t	killed;
	int32_t	escaped;
	int64_t startTime;
	bool	rewardWave;
};
typedef std::map<int32_t, MonsterWave> Int32MonsterWaveMap;

struct MonsterWait 
{
	int32_t id;
	int32_t times;
	int64_t	lastTick;
	bool	rewardWave;
};
typedef std::list<MonsterWait> MonsterWaitList;

//#define  MAX_GAMBLE_RECORD 10
//struct GambleRecord
//{
//	int32_t cid;
//	std::string name;
//	int32_t type;
//	int32_t item;
//	int32_t item_type;
//	int32_t count;
//};
//typedef std::vector<GambleRecord>GambleRecordVector;

enum Direction
{
	INVALID_DIRECTION = -1,
	DOWN = 0,
	DOWN_RIGHT = 1,
	RIGHT = 2,
	UP_RIGHT = 3,
	UP = 4,
	UP_LEFT = 5,
	LEFT = 6,
	DOWN_LEFT = 7,	
	DIRECTION_NUM = 8,
};

enum LogSignInfoFlag
{
	LSIF_NEW_MONTH = 1,
	LSIF_SIGN = 2,
	LSIF_GET_REWARD = 3,
};

#define PI 3.14159265

#define MAX_PLAYER_LEVEL 130
#define MAX_NO_POTENTIAL_PLAYER_LEVEL 10
#define MAX_FIRST_PLAYER_LEVEL 190
#define MAX_SECOND_PLAYER_LEVEL 250
#define MAX_THREE_PLAYER_LEVEL 300

#define DEAL_SLOT_NUM 11 //0 is valid, 10 slot for items, last slot for money
#define STALL_SLOT_NUM 25 //slot 0 for invalid check, max 24 slot

enum DealResult
{
	DR_SUCCESS = 0,
	DR_CANCELED = 1,
	DR_FAILED = 2,
};

struct BagSlot 
{
	int32_t bag;
	int32_t slot;
};
typedef std::vector<BagSlot> BagSlotVector;
typedef std::vector<BagSlotVector> BagSlotVectorVector;

class Unit;
typedef std::list<Unit*> UnitList;
typedef std::vector<Unit*> UnitVector;

class Monster;
typedef std::list<Monster*> MonsterList;
//typedef std::map<int32_t, Monster*> Int32MonsterMap;
typedef std::map<EntityId_t, Monster*> EntityIdMonsterMap;

class Npc;
typedef std::list<Npc*> NpcList;

class Player;
typedef std::list<Player*> PlayerList;
typedef std::vector<Player*> PlayerVector;
typedef std::map<CharId_t, Player*> CharIdPlayerMap;

class CObjPet;
typedef std::list<CObjPet*> ObjPetList;
typedef std::map<PetId_t,CObjPet*> PetIdObjPetMap;

class Trailer;
typedef std::list<Trailer*> TrailerList;
typedef std::map<EntityId_t, Trailer*> EntityIdTrailerMap;

//class CDropItemGroup;
//typedef std::list<CDropItemGroup*> DropItemGroupList;

class Plant;
typedef std::list<Plant*> PlantList;

class Trap;
typedef std::list<Trap*> TrapList;

struct ChangeJobIndex
{
	int32_t  job;
	int32_t  replace_id;
};
typedef std::vector<ChangeJobIndex> ChangeJobIndexVector;

enum GameMsgCode
{
	GMC_UNKNOWN,

	GMC_PLAYER_ENTER_MAP,
	GMC_PLAYER_LEAVE_MAP,
	GMC_ADD_DUNGEON,			// ��������
	GMC_DEL_DUNGEON,			// ���ո���
	GMC_PLAYER_LOGOUT,			// ��ҵǳ�
	GMC_ACTIVITY_START,
	GMC_MO_LING_RU_QIN_START,	// ħ�����ֿ�ʼ
	GMC_KILL_MONST,				// ɱ������
	GMC_ACTIVITY_STOP,
};

class GameMsg 
{
public:
	GameMsg()
	{

	}

	void reset()
	{
		msgcode = GMC_UNKNOWN;
		ptr1 = NULL;
		ptr2 = NULL;
		ptr3 =NULL;
		iparam1 = 0;
		iparam2 = 0;
		effecttick = 0;
	}

	GameMsgCode msgcode;
	void *ptr1;
	void *ptr2;
	void *ptr3;
	int32_t iparam1;
	int32_t iparam2;
	int64_t effecttick;
};
typedef Answer::SafeQueue<GameMsg*> GameMsgQueue;


#define EX_ACTIVITY_NUM 3   //�������ֵ�
#define BI_HERO_BUFF 9002//Ӣ�۸���Ӣ��״̬

enum SpecialBuffId
{
	JUN_TUAN_LIE_JIU_BUFFI	= 135,		//�����Ҿ�buffid
};
enum TD_DUNGEON_SKILL_ID
{
	DUNGEON_SKILL_ID_1	= 150,
	DUNGEON_SKILL_ID_2  = 151,
	DUNGEON_SKILL_ID_3  = 152,
};
enum DungeonDropGroupType
{
	DDGT_ORDINARY = 1,
	DDGT_HERO	  = 2,
};

enum AddXiuweiType
{
	AXT_XIUWEIDAN = 1,
	AXT_LINGTAIFUDI = 2,
};

#define DAILY_READ_MAX 10

#define PRISON_MAP_ID 70003		// ������ͼid
#define PRISON_X    50			// ��������x
#define PRISON_Y	50			// ��������y

#define ADD_PP_VALUES1	5
#define ADD_PP_VALUES2	10
#define ADD_PP_VALUES3  15

typedef std::list<UnitHandle> UnitHandlerList;
struct UseSkill
{
	UseSkill()
	{
		CleanUp();
	}

	void CleanUp()
	{
		skillId	= 0;
		srcPos.x = 0;
		srcPos.y = 0;
		tarPos.x = 0;
		tarPos.y = 0;
		targets.clear();
	}

	int32_t			skillId;
	Position		srcPos;
	Position		tarPos;
	UnitHandlerList	targets;
};

struct ShowIcon
{
	int32_t nId;
	int8_t  nState;
	int32_t nLeftTime;
	int8_t	IconLeft;
	int32_t IconRight;
	int8_t Effects;
};
typedef std::list<ShowIcon> IconStateList;

#define MAX_FAMILY_TASK_COUNT 10				//��������������

enum HD_ID
{
	HUODONG_DA_TING	    = 1,		// �����
	JIANG_LI_DA_TING	= 2,		// ��������
	DA_WEI_WANG_HD_ID	= 6,		// ��θ���
	JI_ZHAN_BOSS		= 9,		// ��սboss
	VIP_GUA_JI			= 14,		// vip�һ�
	CHOU_CHONG			= 15,		// �״γ�ֵ
	NEW_SERVER_TE_HUI	= 16,		// �·��ػ�
	EVERYDAY_CHOU_ZHI	= 17,		// ÿ�ճ�ֵ
	TOU_ZI				= 18,		// Ͷ��
	KAI_FU_HUO_DONG_ID	= 19,		// �����
	TERRITORY_WAR_ICON	= 24,		// ���ս
	HALL_OF_FAME_ICON	= 25,		// ������
	SCORE_SHOP			= 26,		// �����̳�
	THREE_PET_ICON		= 27,		// �������ͼ��
	VIP_CARD_ICON		= 28,		// vip�����
	CHOU_JIANG_ICON		= 29,		// �齱
	QI_FU_ICON			= 30,		// ����
	TOTAL_CHONG_ZHI		= 31,		// �����ۼƳ�ֵ
	WAR_VICTORY_ICON	= 32,		// ��սʤ��ͼ��
};

enum SysSetting
{
	SS_PETEGG			= 1,	//�Զ��������޵�
	SS_TEAM				= 2,	//��ֹ���������Ҽ������
	SS_FRIEND			= 3,	//��ֹ���˽��Ҽ�Ϊ����
	SS_TRADE			= 4,	//��ֹ�������ҷ�����
};

enum APPEND_ATTR_TYPE
{
	FAMILY_WAR_WIN		= 1,	//����սʤ�����ų�����
};

#define WAR_VICTORY_BOSS_ID  477115		//��սʤ��bossid
