#pragma once


/*
* MSG_CM_CODE	�ͻ��˷��͸�����˵�Э��	0~10000
* MSG_SM_CODE	����˷��͸��ͻ��˵�Э��	10000~20000
* MSG_IM_CODE	����˸�������֮���Э��	>20000
*/

/*
* �쿪		CM_ZK	400~699		SM_ZK	11400~11699
* ������	CM_WMF	700~899		SM_WMF	11700~11899
* ���ν�	CM_WXJ	900~999		SM_WXJ	11900~11999
*/

/*
* CM_CHARACTER_MIN					= 0,
* CM_GAME_MIN						= 5,
* CM_GATE_MIN						= 1000,
* CM_LOGIN_MIN						= 2000,
* CM_SOCIAL_MIN						= 3000,
* CM_KINDOM_MIN						= 4000,
* CM_GM_MIN							= 9000,
*/
enum MSG_CM_CODE
{
	CM_CHARACTER_MIN		=0,
	CM_GET_CHARACTER		=1,		//��ȡ���
	CM_NEW_CHARACTER		=2,		//�����
	CM_DEL_CHARACTER		=3,		// ���� ɾ�������Ϣ
	CM_CHARACTER_MAX		=4,

	CM_GAME_MIN				=5,
	CM_ENTER_GAME			=6,		//������Ϸ
	CM_ENTER_GAME_ROBOT		=7,		//�ͷ���δʹ��
	CM_LOGOUT				=8,		
	CM_SYNC_TIME			=9,		//ͬ��ʱ��
	CM_MOVE					=10,	//�ƶ�
	CM_JUMP					=11,	//��Ծ
	CM_TRAILER_MOVE			=12,	//׷���ƶ�
	CM_HIT					=13,	//���ܳ��
	CM_UNIT_THROWED			=14,
	CM_ARRIVE				=15,	//����
	CM_SWITCH_MAP			=16,	//ѡ���ͼ
	//CM_GETMATCHINFO			=17,

	CM_ENTER_DUNGEON					=18,	//���븱��
	CM_ENTER_ACHIEVEMENT_DUNGEON		=19,	//����ɾ͸���
	CM_BUY_DULTI_DUNGEON				=20,
	CM_LEAVE_DUNGEON					=21,	//�뿪����
	CM_LEAVE_ACTDUNGEON					=22,
	CM_DUNGEON_ENTRUST					=23,	// ����//��ʼί�и���
	CM_ENTRUST_STOP						=24,	// ����//ֹͣί��
	CM_ENTRUST_COLLING					=25,	// ����//ί����ȴ
	CM_ENTRUST_COLLECT					=26,	// ����//ί���ռ�
	CM_ENTRUST_QUERY					=27,	// ����//����ί�и�����ѯ
	CM_DUNGEON_SELECT_REWARD			=28,	//ѡ�񸱱�����
	CM_ENTER_ACTIVIY					=29,	//����
	CM_LEAVE_ACTIVIY					=30,	//�뿪�
	CM_GETAWARD_ACTIVIY					=31,	//��ȡ��Ʒ

	CM_ENTER_FAMILY_ACTIVITY			=32,	// ����
	CM_LEAVE_FAMILY_ACTIVITY			=33,	// ����
	CM_GET_EXACTIVITY_GIFT				=34,	// ����

	CM_ENTER_FAMILY_POT_ACTIVITY		=35,	// ����//������ɻ��
	CM_QUERY_FAMILY_POT_INFO			=36,	// ����//��ѯ���ɻ��

	CM_ENTER_FAMILY_MAP					=37,	// ����//������ɵ�ͼ
	CM_LEAVE_FAMILY_MAP					=38,	// ����//�뿪���ɵ�ͼ
	CM_SELECT_UNIT						=39,	//ѡ��Ӣ��or�佫
	CM_DO_UNIT_SKILL					=40,	//ʹ�ü���
	CM_PICK_DROPITEM					=41,	//��ժ��ֹ
	CM_BEGIN_GATHER						=42,	//��ʼ�ɼ�
	CM_END_GATHER						=43,	//�����ɼ�
	CM_USE_TRAP							=44,	//ʹ�ô�����
	CM_SWITCH_PK_MODE					=45,	//PKģʽ

	// ���Э��
	CM_QUERY_TEAMS_AROUND				=46,	// ��Ѱ��������
	//CM_CREATE_TEAM						=47,	// ��������
	//CM_INVITE_INTO_TEAM					=48,	// ����������
	//CM_CHANGE_TEAM_NAME					=49,	// ����
	//CM_SET_TEAM_LEADER					=50,	// �ӳ�ת��
	//CM_KICKOUT_TEAM_MEMBER				=51,	// ����
	//CM_LEAVE_TEAM						=52,	// �뿪����
	//CM_APPLY_INTO_TEAM					=53,	// �������
	//CM_REPLY_TEAM_INVITE				=54,	// �ظ��������
	//CM_REPLY_TEAM_APPLY					=55,	// �ظ��������

	CM_SAFE_REVIVE						=56,	//�سǸ���
	CM_ON_SITE_REVIVE					=57,	//��������
	CM_STRONG_REVIVE					=58,	//
	CM_QUERY_CHR_INFO					=59,	//�鿴����
	CM_UPGRADE_LEVEL					=60,
	CM_KINGDOM_SELECT					=61,	// ����//ѡ���ѹ�����Ա
	CM_CHANGE_KINGDOM					=62,	// ����//�ѹ�
	CM_SELECT_KINGDOM					=63,	// ����//ѡ�����
	CM_EXCHANGE_EQUIP					=64,	// ����//װ���ı�
	CM_SWITCH_MOUNT						=65,
	CM_EXCHANGE_ARTIFACT				=66,	//null
	CM_ADD_ARTIFACT_EXP					=67,	//null
	CM_ADD_ARTIFACT_MAX_LEVEL			=68,	//null
	CM_ADD_ARTIFACT_QUALITY				=69,	//null
	CM_MOUNT_XISUI_UPGRADE				=70,	// ����//����ϴ�� 
	CM_MOUNT_USER_XIUWEIDAN				=71,	// ����//ʹ�����ʵ�ѵ��(����)
	CM_QUERY_BAG_ITEMS					=72,	// ����//��ѯ����

	CM_QUERY_SIGN_INFO					=73,	//ǩ����Ϣ
	CM_SIGN								=74,	//ÿ��ǩ��
	CM_GET_SIGN_REWARD					=75,	//��ȡǩ������
	/*�񽫴�˵��ת����*/
	CM_QUERY_AC_EXCHANGE				=76,	//��ѯ�ɾ�
	CM_QUERY_AC_OUTDOOR					=77,	//��˵Ұ��
	CM_QUERY_AC_CAVE					=78,	//��Ѩ
	CM_QUERY_AC_GROW_UP					=79,	//�ɳ����ɾͣ�
	CM_QUERY_AC_CHALLENGE				=80,	//��ս��ת����
	CM_QUERY_AC_SPECIAL					=81,	//����ɾ�
	CM_QUERY_AC_DAILY					=82,
	CM_QUERY_AC_TASK					=83,	//�ɾ�����  
	CM_GET_ACHIEVEMENT_REWARD			=84,	//��ȡ�ɾͽ���
	CM_GET_AC_EXCHANGE_REWARD			=85,	//��ȡ��ս�ɾͽ���
	CM_QUICK_AC_DAILY					=86,	//��������ճ��ɾ�
	CM_QUICK_AC_OUTDOORCAVE				=87,	//����ʯ�������

	CM_ADD_BAGSLOT						=88,	//���Ӱ���λ��
	CM_USE_ITEM							=89,	//ʹ�ð�����Ʒ
	CM_PATCH_USE_ITEM					=90,	//����ʹ�õ���
	CM_MOVE_ITEM						=91,	//�ƶ�װ�� 
	CM_SPLIT_ITEM						=92,
	CM_DESTROY_ITEM						=93,
	CM_SORT_BAG							=94,	//��������
	CM_SELL_ITEM						=95,	//����������Ʒ
	CM_ITEM_COMBINE						=96,	// ����//�ϳ�
	CM_EQUIP_ENHANCE					=97,	// ����//ǿ��װ��
	CM_QUERY_EQUIP_ENHANCE_TIME			=98,	// ����//�鿴ǿ��װ��
	CM_RESET_ENHANCE_TIME				=99,	// ����
	CM_EQUIP_SMITTHING					=100,	// ����//װ����������
	CM_EQUIP_MAGIC_SMITH				=101,	// ����//��������װ��
	CM_EQUIP_DISMANTLE					=102,	// ����//�ֽ�װ��
	CM_UNDRESS_SHIZHUANG				=103,	// ����//�ѵ�ʱװ
	CM_MOUNT_XISUI						=104,	// ����//����ϴ��
	CM_EQUIP_ADDGEM						=105,	// ����//��Ƕ��ʯװ��
	CM_EQUIP_REMOVEGEM					=106,	// ����//ȡ����Ƕ��ʯ
	CM_STRATEGICS_READ					=107,	// ����//�Ķ�����
	CM_STRATEGICS_EXERCISE				=108,	// ����//Strategic ����ѵ��
	CM_STRATEGICS_QUERY					=109,	// ����//��ѯ����
	CM_QUERY_SKILL_LIST					=110,	//��ѯ����LIST
	CM_UPGRADE_SKILL_LEVEL				=111,	//����SKILL LEVEL
	CM_QUERY_ZHENFA_LIST				=112,	// ����//��ѯ�ķ�LIST
	CM_UPGRADE_ZHENGFA_LEVEL			=113,	// ����//�����󷨵ȼ�
	CM_QUERY_TASK_LIST					=114,
	CM_RECEIVE_TASK						=115,	//��������
	CM_SUBMIT_TASK						=116,	//�ύ����
	CM_GIVEUP_TASK						=117,	//��������
	CM_SET_TASK_CAN_SUBMIT				=118,	//���������ύ 
	CM_RECEIVE_TASK_CYCLE				=119,	// ���� ����ѭ������
	CM_SUBMIT_TASK_CYCLE				=120,	// ���� �ύѭ������
	CM_RECEIVE_TASK_FAMILY				=121,	//���ܰ�������
	CM_SUBMIT_TASK_FAMILY				=122,	//�ύ��������
	CM_RECEIVE_TASK_KINGDOM				=123,	//���ܹ�������
	CM_SUBMIT_TASK_KINGDOM				=124,	//�ύ��������
	CM_TALK_WITH_NPC					=125,	//��NPC��̸
	CM_QUICK_DONE						=126,	//�������
	CM_TELEPORT							=127,	//���� 
	CM_TELEPORT_ACTIVITY				=128,	//�����
	CM_QUERY_VICE_GENERAL_LIST			=129,	// ����//��ѯ�����б�
	CM_QUERY_LAKEY						=130,	// ����//��ѯ���
	CM_MONSTER_MOVE						=131,
	CM_KICK_OUT_MOVE					=132,	//�����ƶ�
	CM_VICE_GENERAL_MOVE				=133,	// ����//�����ƶ�
	CM_VICEGENERAL_FREE					=134,	// ����
	CM_VICEGENERAL_MAIN_SETTING			=135,	// ����
	CM_VICEGENERAL_MODE_SETTING			=136,	// ����
	CM_VICEGENERAL_GENGU_UPGRADE		=137,	// ����//����ǿ������
	CM_VICEGENERAL_WUYI_UPGRADE			=138,	// ����//������������
	CM_MOUNT_UPGRADE					=139,	//�������
	CM_SHIZHUANG_CHANGE_FORM			=140,	//��װ -- ����
	CM_VICE_GENERAL_DO_SKILL			=141,	// ����//����ʹ�ü���
	CM_ADD_ACTION						=142,
	CM_REMOVE_ACTION					=143,
	CM_EXCHANGE_ACTION					=144,	//��������λ��
	CM_SET_AUTOFIGHT					=145,	//�����Զ�ս��
	CM_SET_SYSTEM_SETTING				=146,	//ϵͳ����
	CM_QUERY_GAME_SHOP					=147,	//�ͻ���δʹ��
	CM_BUY_GAME_SHOP_ITEM				=148,	//�����̳���Ʒ
	CM_BUY_GAME_CHR_ITEM				=149,	//��������̳�
	CM_BUY_TOP_SHOP_ITEM				=150,	//�����̳�������Ʒ
	CM_BUY_BACK_CHR_ITEM				=151,	//���ظ����̳���Ʒ
	CM_BUY_RESOURCE						=152,	//�һ��ķ�
	CM_BUY_TASK_COUNT					=153,	//�����������
	CM_GET_ONLINE_REWARD				=154,	//��ȡ���߽���
	CM_ASK_ONLINE_TEME_INFO				=155,	//��������ʱ����Ϣ
	CM_GET_WEEK_ONLINE_REWARD			=156,	//��ȡ�����߽���
	CM_QUERY_MONSTER_BROADCAST			=157,
//	CM_GET_OFFLINE_EXP					=158,	//����//��ȡ���߾���
	CM_TELEPORT_BY_ITEM					=159,	//����
	CM_BUY_TELEPORT_COUNT				=160,	//�����ʹ���
	CM_CLICK_PAY_BUTTON					=161,	//��ֵ
	CM_MAP_ENTERED						=162,
	CM_DEBUG_CMD						=163,	//����������

	CM_GET_BLESS_EXP					=164,	//null
	CM_QUERY_PLAYER_INFO				=165,	//��ѯ�����Ϣ
	CM_MOUNT_PUTUP						=166,	//����ʹ������ ����ʵ�֣�
	CM_PILL_USE							=167,	//����//��ҩʹ��
	CM_LACKEY_UPGRADE					=168,	//����//����ū��
	CM_QUERY_DAILY_ITEM					=169,	//��ѯ��Ծ����Ŀ
	CM_GET_DAILY_REWARD					=170,	// ���� ��ȡ��Ծ����
	CM_DAILY_SEARCHBACK_REWARD			=171,	// ���� ��ȡ�ճ�����
	CM_DAILY_FAMILY_REWARD				=172,	// ���� ����ٺ»
	CM_DAILY_KINGDOM_REWARD				=173,	// ���� ����ٺ»
	CM_DAILY_WUSHEN_REWARD				=174,	// ���� ����ٺ»a
	CM_DAILY_LEVEL_REWARD				=175,	// ���� �ȼ�ٺ»
	CM_GET_FAV_REWARD					=176,	//�ղغͱ����ݷ�ʽ��Ľ���
	CM_QUERY_LAN_AND_VAL_GIFT_INFO		=177,	//��ѯԪ������˫�ڻ�콱���
	CM_GET_LAN_AND_VAL_GIFT				=178,	//��ȡԪ�����˽�˫�ڻ����
	CM_DAILY_CLOSE_TABLE				=179,	//�ر��ճ���Ծ�����
	CM_QUERY_CHARGE_REWARDS_INFO		=180,
	CM_GET_CHARGE_REWARD				=181,	//��ȡ�׳����
	CM_SECOND_DAY_LOGIN					=182,	//�ڶ����½����
	CM_BUY_READ_COUNT					=183,	// ����//���� �Ķ�����
	CM_OPEN_LEVEL						=184,	// ����//ת��
	CM_GAMBLE							=185,	//���� //ҡ����ϡ������
	CM_BUY_PVP_STATE					=186,	//���� ���״̬
	CM_QUERY_DAILY_PK_INFO				=187,	//��ѯ ÿ��PK ��Ϣ
	
	CM_GOTO_ENEMY_KILLER_RANK			=188,	//���͵����� λ��	// ����
	CM_GET_DAILY_PK_REWARD				=189,	//��ȡ ÿ��PK ����
	CM_QUERY_KILLER_RANK_SELF			=190,
	CM_BUY_LEVEL_EQUIP					=191,	// ����
	CM_QUERY_EVERY_GOLD					=192,
	CM_GET_EVERY_GOLD					=193,
	CM_FAMILY_BREAK						=194,	// ���� �������
	CM_GET_LEVEL_GIFT					=195,	//��ȡ �ȼ����
	CM_QUERY_LEVEL_GIFT					=196,	//��ѯ �ȼ���� 
	CM_GAME_PUBLIC_CHAT					=197,	//����
	CM_QUERY_GAMBLE_DEPOT_ITEM			=198,	
	CM_GET_GAMBLE_DEPOT_ITEM			=199,
	CM_GET_LEVEL_GOLD					=200,	//��ȡ  �ȼ�Ԫ��
	CM_QUERY_LEVEL_GOLD					=201,	//��ѯ �ȼ�Ԫ��

	CM_GET_SEVEN_LOGIN					=202,	//���������ȡ

	CM_QUERY_SEVEN_LOGIN				=203,
	
	CM_QUERY_NEW_SERVER_ACTIVITY_INFO	=204,
	CM_GET_NEW_SERVER_ACTIVITY_REWARD	=205,

	CM_GET_NEW_SERVER_ACTIVITY_GOLD		=206,

	CM_GET_MAGIC_WEAPON					=207,	// ����//װ������
	CM_GET_MAGIC_WEAPON_INFO			=208,	// ����//��ȡ������Ϣ
	CM_MAGIC_WEAPON_UPGRADE				=209,	// ����//���
	CM_USER_JOIN						=210,
	//CM_TEAM_DUNGEON_START				=211,	// ����//��Ӹ�����ʼ
	//CM_TEAM_DUNGEON_KICK_OUT			=212,	// ����//�߳�����
	CM_USE_JIANSHELING					=213,	// ����//ʹ�ý�����
	CM_VICEGENERAL_UPGRADE				=214,	// ����//��������
	CM_EXCHANGE_GAMBLE_GOLD_EQUIP		=215,	// ����

	//CM_BUY_VIP							=216,
	CM_YELLOW_STONE						=217,	//QQ������Ȩ	
	CM_GET_YELLOW_AWARD					=218,	//��ȡQQ�������

	CM_GET_STRATEGICS_PAGE					= 219,			// ����//������ҳ��ȡ
	CM_FLUSH_STRATEGICS_PAGE				= 220,			// ����//ˢ����ҳ
	//CM_LEAVE_DUNGEON_TEAM					= 221,			// ����//�뿪������� ����
	CM_ADVANCED_TRIALS_QUERY				= 222,			// ����//����������ʼ				
	CM_ADVANCED_TRIALS_RESET				= 223,			// ����//��������
	CM_ADVANCED_TRIALS_RAIDS_START			= 224,			// ����//���׸���ɨ����ʼ
	CM_ADVANCED_TRIALS_RAIDS_QUERY			= 225,			// ����//����ɨ����Ϣ
	CM_ADVANCED_TRIALS_RAIDS_STOP			= 226,			// ����//ɨ��ֹͣ
	CM_ADVANCED_TRIALS_RAIDS_REWARD			= 227,			// ����//ɨ��ʱ�䵽,��ȡ����
	CM_ADVANCED_TRIALS_RAIDS_COOLING		= 228,			// ����//��ȴɨ��
	CM_SHOP_MYSTERIOUS_QUERY				= 229,			//��ʼ�������̵�(�İ��)
	CM_SHOP_MYSTERIOUS_FLUSH				= 230,			//ˢ�������̵�
	CM_SHOP_MYSTERIOUS_BUY					= 231,			//�����̵� ����

	CM_VICE_GENERAL_TIME_QUERY				= 232,			// ����//�佫��ļʱ����Ϣ
	CM_VICE_GENERAL_RECRUIT					= 233,			// ����//�佫��ļ
	CM_VICE_GENERAL_SHOW					= 234,			// ����//Ԥ���佫
	CM_VICE_GENERAL_FORWARD					= 235,			// ����//�佫�̳�
	CM_VICEGENERAL_GENGU_REPLACE			= 236,			// ����//�佫�����滻
	CM_VICEGENERAL_GENGU_CANCLE				= 237,			// ����//�佫����ȡ��


	CM_PB_THY_RELIEF_RESIDENT				= 250,			// ����//��������
	CM_PB_THY_RELIEF_SERVANT				= 251,			// ����//������ʿ
	CM_PB_THY_HIGH_RECRUIT					= 252,			// ����//�߼���ļ
	CM_PB_THY_RECRUIT_EXP					= 253,			// ����//��������ֿ�
	CM_PB_THY_RECRUIT_MONEY					= 254,			// ����//����ͭǮ�ֿ�
	CM_MOUNT_CHANGESHAPE					= 255,			//���ﻯ��

	CM_VICEGENERAL_USEREXP					= 256,			// ����//����ʹ�þ��鵤
	CM_VICEGENERAL_FIGHTMODE				= 257,			// ����//��������ս��ģʽ ���� ����
	CM_VICEGENERAL_FIRE						= 258,			// ����//�������

	CM_BUY_THREE_LIMIT_SHOP					= 259,			// ����//���򿪷�3���̳���Ʒ

//===============================
//���ﻹ��40��Э�������
//===============================

	CM_PREVENT_WALLOW						= 299,			//������
	CM_PB_THY_QUERY							= 300,			// ����
	CM_PB_THY_GET_EXP						= 301,			// ����
	CM_PB_THY_GET_MONEY						= 302,			// ����
	CM_PB_THY_RELIEF						= 303,			// ����//����
	CM_PB_THY_RECRUIT						= 304,			// ����//����
	CM_PB_THY_RELIEF_COOLING				= 305,			// ����
	CM_PB_THY_RECRUIT_COOLING				= 306,			// ����

	CM_PB_YWC_ACTIVATION					= 307,			// ����//����������
	CM_PB_YWC_CULTIVATION					= 308,			// ����//��ʼ������ѵ��
	CM_PB_YWC_GET_EXP						= 309,			// ����//��ȡ����������
	CM_PB_YWC_COOLING						= 310,			// ����//��ȥ������
	CM_PB_YWC_QUERY							= 311,			// ����//������
	CM_PB_YWC_BUY_EXERCISE_COUNT			= 312,			// ����//�������䳡��������

	CM_PB_FYB_QUERY							= 313,			// ����//������ѯ	
	CM_PB_FYB_GET_WUHUEN					= 314,			// ����
	CM_PB_FYB_AGAINSEAL						= 315,			// ����

	CM_GB_SLD_REFINING_PILL					= 316,			// ����
	CM_GB_SLD_ACTIVE						= 317,			// ����
	CM_GB_SLD_QUERY							= 318,			// ����

	CM_GB_SJT_PRODUCTION					= 319,  		// ����//����ȡ��Ʒ���ٲ�԰��
	CM_GB_SJT_SHEJITU_QUERY					= 320,			// ����

	CM_GB_LTFD_CULTIVATION					= 321,			// ����//ι��
	CM_GB_LTFD_GET_XIUWEI					= 322,			// ����//��ȡ�ɳ�ֵ
	CM_GB_LTFD_COOLING						= 323,			// ����//ι��������ȴ
	CM_GB_LTFD_QUERY						= 324,			// ����

	CM_SB_KLJ_QUERY							= 325,			// ����//��ѯǬ����
	CM_SB_KLJ_AGAINCHALLENGE				= 326,			// ����

	CM_SB_JTT_QUERY							= 327,			// ����//��ѯ����Ź�
	CM_SB_JTT_GET_ITEM						= 328,			// ����
	CM_SB_JTT_AGAINCHALLENGE				= 329,			// ����
	CM_SB_LHT_CHANGEJOB						= 330,			// ����

	CM_SB_FM_QUERY							= 331,			// ����
	CM_SB_DTT_QUERY							= 332,			// ����//��ѯ������
		
	CM_UPDATE_FLY_ICON_INT					= 333,			//�����������ܵ���
//============================================
// ���ﻹ�кö�Э�������
//============================================
	CM_ACTIVITY_BANQUET_TOAST				= 400,

	/*
	* CM_ZK	401~699
	*/
	//�����ֿ�
	CM_GET_DEPOT_ITEM							= 401,			//ȡ�زֿ���Ʒ
	CM_SAVE_DEPOT_ITEM							= 402,			//����Ʒ���ֿ�
	CM_GET_DEPOT_CURRENCY						= 403,			//ȡ���ֿ����
	CM_SAVE_DEPOT_CURRENCY						= 404,			//����ҵ��ֿ�
	CM_SORT_DEPOT								= 405,			//�����ֿ�
	CM_OPEN_DEPOT_SLOT							= 406,			//�����ֿ����
	//��ʯ��Ƕ
	CM_NEW_ADD_GEM								= 407,			//��Ƕ��ʯ
	CM_NEW_REMOVE_GEM							= 408,			//ժȡ��ʯ
	CM_OPQN_GEM_HOLE							= 409,			//װ������
	//װ�����
	CM_ASK_RANSOM_INFO							= 410,			//����װ�������Ϣ
	CM_ASK_SPOILS_INFO							= 411,			//����ս��Ʒ��Ϣ
	CM_ASK_RANSOM_ITEM							= 412,			//���������Ʒ							
	//����
	CM_REQUEST_ASK_MOUNT_TRAIN_INFO				= 413,			//����������Ϣ
	CM_REQUEST_TRAIN_MOUNT						= 414,			//��������
	CM_REQUEST_REQEST_EAT_MOUNT_HEART			= 415,			//������֮��
	CM_REQUEST_ACTIVE_MOUNT_USE_DEFAULT_MOUNT	= 416,			//����
	CM_REQUEST_STUDY_AND_UP_MOUNT_SKILL			= 417,			//ѧϰ���������＼��
	CM_REQUEST_ASK_MOUNT_SKILL_INFO				= 418,			//�������＼����Ϣ
	//����
	CM_REQUEST_TRADE							= 419,			//������
	CM_AGREE_TRADE								= 420,			//ͬ�⽻��
	CM_ADD_TRADE_ITEM							= 421,			//�����ױ����м���Ʒ
	CM_REMOVE_TRADE_ITEM						= 422,			//�Ƴ����ױ����е���Ʒ
//	CM_REQUSET_SHUI_JI							= 423,			//�������
	CM_SUB_TRADE_MONEY							= 424,			//���ٱ����е���Դ
	CM_LOCK_TRADE								= 425,			//�������ױ���
	CM_SURED_TRADE								= 426,			//ȷ������
	CM_CANCEL_TRADE								= 427,			//ȡ������
	CM_ADD_TRADE_MONEY							= 428,			//���ӽ��ױ����е���Դ
	//�ʼ�
	CM_READ_MAIL								= 429,			//���ʼ�
	CM_ASK_MAIL_LIST							= 430,			//�����ʼ��б�
	CM_GET_FU_JIAN								= 431,			//��ȡ����
	CM_DEL_MAIL									= 432,			//ɾ���ʼ�
	//�̳�
	CM_ASK_SHANG_CHENG_LIMIT_INFO				= 433,			//�����̳�������Ϣ
	CM_BUY_SHANG_CHENG_ITEM						= 434,			//�����̳���Ʒ

	//����
	CM_ASK_FRIEND_REWARD_INFO					= 436,			//������ѽ�����Ϣ
	CM_ASK_GET_FRIEND_REWARD					= 437,			//�����ȡ����
	//����boss��Ϣ
	CM_ASK_BOSS_INFO							= 438,			//����boss��Ϣ

	//����ϵͳ
	CM_BUY_FA_BAO_RES				= 446,			//���򷨱���Դ
	CM_ASK_FA_BAO_INFO							= 440,			//���󷨱���Ϣ
	CM_FA_BAO_PEI_YANG							= 441,			//��������

	//����ս������Ϣ
	CM_ASK_BATTLE_INFO							= 442,			//����ս������Ϣ
	//����
	//��λ
	CM_JUE_WEI_DONATE_MONEY						= 444,			//��λ����
	CM_ASK_JUE_WEI_INFO							= 445,			//�����λ��Ϣ
	//�齱
	CM_TIDY_CHOU_JIANG_BAG						= 448,			//�����齱����
	CM_GET_CHOU_JIANG_ITEM						= 449,			//ȡ���齱��Ʒ
	CM_ASK_CHOU_JIANG_RECORD					= 450,			//����齱��¼ 
	CM_BUY_SCORE_SHOP_ITEM						= 451,			//��������̵���Ʒ
	CM_ASK_CHOU_JIANG							= 452,			//����齱
	CM_ASK_SCORE_SHOP_INFO						= 453,			//��������̳�������Ϣ
	//����
	CM_ASK_QI_FU_INFO							= 454,			//����������Ϣ
	CM_ASK_QI_FU								= 455,			//��������
	//��θ��
	CM_ASK_DWW_INFO								= 456,			//�����θ����Ϣ
	CM_ASK_DWW_REWARD							= 457,			//�����θ������
	//���߾���
	CM_ASK_OFFLINE_INFO							= 459,			//�������߾�����Ϣ
	CM_GET_OFFLINE_EXP							= 460,			//��ȡ���߾���
	//������������
	CM_ASK_HUO_DONG_DA_TING_INFO				= 462,			//������������
	//vip
	CM_BUY_VIP									= 463,			//����vip��
	CM_GET_VIP_GIFT								= 464,			//VIP���
	CM_GET_VIP_CARD_GIFT						= 465,			//vip�����
	//����boss֮��
	CM_BUY_ENTER_BOSS_HOME						= 467,			//����boss֮��
	CM_LEAVE_BOSS_HOME							= 468,			//�뿪boss֮��
	//����vip�һ���ͼ
	CM_ENTER_VIP_GUA_JI_MAP						= 469,			//����vip�һ���ͼ
	//����ħ������
	CM_ENTER_MO_LING_RU_QIN						= 472,			//����ħ������
	//��Դ�һ�
	CM_ASK_SEARCHBACK_INFO						= 475,			//������Դ�һ���Ϣ
	CM_ASK_GET_SEARCHBACK_REWARD				= 476,			//�һ���Դ
	//������
	CM_PUT_IN_EXP_BALL							= 477,			//װ�Ͼ�����
	CM_ASK_EXP_BALL_INFO						= 478,			//����������Ϣ
	//�����ʼ��Ϸ
	CM_CLICK_GAME								= 480,			//�����ʼ��Ϸ
	//ʹ�ö����Ʒ
	CM_USE_MMULTI_ITEM							= 481,			//ʹ�ö����Ʒ
	//��Ӫ�
	CM_GET_SHOU_CHONG_REWARD					= 482,			//���׳����
	CM_ASK_SHOU_CHONG_INFO						= 483,			//�����׳���Ϣ
	CM_ASK_SHOU_TE_HUI_INFO						= 484,			//�����·��ػ�
	CM_BUY_TEI_HUI_GIFT							= 485,			//�����ػ����
	CM_ASK_EVERYDAY_CHONG_ZHI_INFO				= 486,			//����ÿ�ճ�ֵ��Ϣ
	CM_GET_EVERYDAY_CHONG_ZHI_LI_BAO			= 487,			//��ȡÿ�ճ�ֵ���
	CM_GET_THREE_PET_GIFT						= 488,			//��ȡ3�����			
	CM_GET_TOTAL_CHONG_ZHI_GIFT					= 489,			//������ֵ���
	//Ͷ��
	CM_ASK_TOU_ZI_INFO							= 490,			//����Ͷ����Ϣ
	CM_GET_TOU_ZI_REWARD						= 491,			//��ȡͶ��
	CM_TOU_ZI									= 492,			//Ͷ��
	
	//�����
	CM_ASK_KAI_FU_HUO_DAO_INFO					= 495,			//���󿪷����Ϣ
	CM_GET_KAI_FU_HUO_DAO_WARD					= 496,			//��ȡ��������
	CM_ASK_KAI_FU_HUO_DONG_STATE				= 497,			//���󿪷��״̬

	//��Ծ��
	CM_ASK_HUO_YUE_DU_INFO						= 500,			//�����Ծ����Ϣ
	CM_GET_HUO_YUE_DU_REWARD					= 501,			//��ȡ��Ծ�Ƚ���
	CM_ASK_MIAO_HUO_YUE_DU						= 502,			//���Ծ��
	//��λ
	CM_ASK_GUAN_WEI_INFO						= 505,			//�����λ��Ϣ
	CM_ASK_GUAN_WEI_REWARD						= 506,			//��ȡ��λ����
	CM_UP_GUAN_WEI								= 507,			//������λ
	CM_WEAR_QI_SHI								= 508,			//�����ʿ
	//�ɾ�
	CM_ACHIEVEMENT_GET_REWARD					= 510,			//��ȡ�ɾͽ���
	CM_ACHIEVEMENT_UP_XUN_ZHANG					= 511,			//����ѫ��
	CM_ACHIEVEMENT_BUY_XUN_ZHANG_SOCER			= 512,			//����ѫ�»���

	//���󿪷��ۼƳ�ֵ��Ϣ
	CM_ASK_TOTAL_CHONG_ZHI_INFO					= 515,			//���󿪷��ۼƳ�ֵ��Ϣ
	CM_GET_MOBILE_PHONE_GIFT			= 518,			// 手机绑定礼包
	CM_GET_ZERO_BUY_PET_GIFT			= 519,			// 0元购宠物
	CM_DB_GET_MOBILE_PHONE_GIFT		= 20257,		// 数据库返回手机绑定礼包
	//��ս70
	CM_ASK_VAR_VICTORY_INFO						= 516,			//����սʤ�����Ϣ
	CM_GET_VAR_VICTORY_REWARD					= 517,			//����սʤ������
	//ɱ��ͳ��
	CM_ASK_KILL_MONSTER_COUNT					= 600,			//����ɱ����Ϣ
	/*
	* CM_WMF	700~899
	*/

	// װ���淨
	CM_EQUIP_REQUEST_INFO						= 710,			// ����װ����Ϣ
	CM_EQUIP_DRESS								= 711,			// ��װ��
	CM_EQUIP_UNDRESS							= 712,			// ��װ��
	CM_EQUIP_UP_GRADE							= 713,			// װ������
	CM_EQUIP_UP_STAR							= 714,			// װ������
	CM_EQUIP_UP_QUALITY							= 715,			// װ����Ʒ����װ���죩
	CM_EQUIP_ADD_GEM							= 716,			// ��ʯ��Ƕ
	CM_EQUIP_REMOVE_GEM							= 717,			// ��ʯժȡ
	CM_EQUIP_ITEM_COMBI							= 718,			// ���ߺϳ�
	CM_EQUIP_EXCHANGE_STAR						= 719,			// ǿ��ת��

	// ��Ӳ���
	CM_TEAM_SET_AUTO_OPERATE					= 720,			// ���Ĭ������
	//CM_TEAM_DESTROY_TEAM						= 721,			// �ӳ���ɢ����

	// ����ϵͳ
	CM_PET_REQUEST_INFO							= 730,			// ���������Ϣ
	CM_PET_FIGHTING								= 731,			// ��ս
	CM_PET_REST									= 732,			// ��Ϣ
	CM_PET_ZOARIUM								= 733,			// ����
	CM_PET_ZOARIUM_OFF							= 734,			// �������
	CM_PET_RIDE									= 735,			// ���
	CM_PET_CHANGE_NAME							= 736,			// �������
	CM_PET_RELEASE								= 737,			// ��������
	CM_PET_HATCHING								= 738,			// ����
	CM_PET_MUTI_HATCHING						= 739,			// ��������
	CM_PET_REHATCHING							= 740,			// ���·���
	CM_PET_OPEN_EGG								= 741,			// �򿪳��ﵰ
	CM_PET_DROP_EGG								= 742,			// ������
	CM_PET_USE_EXP_ITEM							= 743,			// ʹ�þ�����ߣ�ʥ��ħ�����������ؼ�������
	CM_PET_USE_PHASE_ITEM						= 744,			// ʹ�øı����Ե��ߣ��罬ҩˮ�������
	CM_PET_STOP_HATCHING						= 745,			// ֹͣ����
	CM_PET_QUICK_HATCHING						= 746,			// ���ٷ���
	CM_PET_EGG_MOVE								= 747,			// ���ﵰ�ƶ�
	CM_PET_LEARN_SKILL							= 748,			// ����ѧϰ���ܣ�������ѧϰ��
	CM_PET_FORGET_SKILL							= 749,			// ������������
	CM_PET_ILLUSION								= 750,			// �û�
	CM_PET_DEPORT								= 751,			// �ֿ��ȡ����
	CM_PET_SET_KNIGHT							= 752,			// ������ʿ
	CM_PET_USE_LUCKY_ITEM						= 753,			// ʹ������ֵ����
	CM_PET_MOVE_TO_BAG							= 754,			// ���������
	CM_PET_GET_FROM_BAG							= 755,			// �ӱ����ƶ���������
	CM_PET_ILLUSION_BY_LI_BAO					= 756,			// ����û�
	// XP
	CM_START_XP									= 760,			// ��ʼXP

	CM_LEVEL_REFINING					= 768,			// 等级精炼
	CM_LEVEL_REFIN_INFO					= 769,			// 等级精炼信息
	

	// ѭ������
	CM_TASK_CYCLE_RECEIVE						= 770,			// ����ѭ������
	CM_TASK_CYCLE_SUBMIT						= 771,			// �ύѭ������
	CM_TASK_CYCLE_REFRESH_STAR					= 772,			// ˢ��ѭ�������Ǽ�
	CM_TASK_CYCLE_BUY_TIMES						= 773,			// ����ѭ���������

	// ����
	CM_FAMILY_CONTRIBUTION						= 780,			// ���ž���
	CM_FAMILY_REGIST_PET						= 781,			// ע�����
	CM_FAMILY_UNREGIST_PET						= 782,			// ȡ��ע�����
	CM_FAMILY_RECEIVE_TASK						= 783,			// �Ӿ�������
	CM_FAMILY_GET_TASK_REWARD					= 784,			// ��ȡ����
	// ����
	CM_DUNGEON_BUILD_TOWER						= 790,			// ��������
	CM_DUNGEON_START							= 791,			// ������ʼ�������ȴ��׶Σ�
	CM_DUNGOEN_BUY_TOWER						= 792,			// ������

	// ��Ӹ���
	CM_TEAM_DUNGEON_CREATE						= 800,			// ���󴴽���Ӹ���
	CM_TEAM_DUNGEON_JOIN						= 801,			// ���������Ӹ���
	CM_TEAM_DUNGEON_START						= 802,			// ��Ӹ�����ʼ

	// ����ϵͳ
	CM_INSIDE_PET_REQUEST_INFO					= 810,			// ����������Ϣ
	CM_INSIDE_PET_BASE_ILLUSION					= 811,			// ����
	CM_INSIDE_PET_GROW_ILLUSION					= 812,			// ����
	CM_INSIDE_PET_LUCKY_ILLUSION				= 813,			// ����
	CM_INSIDE_PET_EXP_ILLUSION					= 814,			// ����
	CM_INSIDE_PET_USE_LUCKY_ITEM				= 815,			// ʹ������ֵ����
	CM_INSIDE_PET_USE_PHASE_ITEM				= 816,			// ʹ�����Ե���
	CM_INSIDE_PET_BUY_EXP						= 817,			// ������ֵ
	CM_INSIDE_PET_REQUEST_BASE_ILLUSION			= 818,			// ���������Ϣ
	CM_INSIDE_PET_ITEM_ILLUSION					= 819,			// ��Ʒ�û�

	// ��̯
	CM_OPEN_STALL								= 820,			// ��̯
	CM_CLOSE_STALL								= 821,			// ��̯

	// �
	CM_REQUEST_ACTIVITY_INFO					= 830,			// ������Ϣ
	CM_REQUEST_ACTIVITY_FAMILY_WAR_PILLAR_INFO	= 831,			// ����ս��֮����Ϣ
	CM_ACTIVITY_FAMILYWAR_ADD_PILLAR_MONEY		= 832,			// ս��֮��ע������ʽ�
	CM_ACTIVITY_GET_DAILY_REWARD				= 833,			// ÿ�ջ����
	CM_ACTIVITY_REQUEST_TERRITORY_INFO			= 834,			// �������ս��Ϣ

	// ����
	CM_ADD_SOUL									= 840,			// �һ�����
	CM_UPGRADE_SOUL_LEVEL						= 841,			// ��������

	// ������
	CM_HALL_OF_FAME_FIGHT						= 850,			// ��ս
	CM_HALL_OF_FAME_BUY_TIMES					= 851,			// �������
	CM_HALL_OF_FAME_CLEAR_CD					= 852,			// ��CD
	CM_HALL_OF_FAME_INFO						= 853,			// ���������Ϣ

	/*
	* CM_WXJ	900~998
	*/

	CM_GAME_MAX,
	

	CM_GATE_MIN					= 1000,
	CM_GATE_LOGIN				= 1001,
	CM_GATE_LOGIN_ROBOT			= 1002,
	CM_GATE_SWITCH_LINE			= 1003,
	CM_GATE_QUERY_LINE			= 1004,
	CM_GATE_LOAD_UI				= 1005,			//���ǽ���������
	CM_GATE_MAX,

	CM_LOGIN_MIN				= 2000,
	CM_USER_LOGIN				= 2001,
	CM_LOGIN_MAX,


	//���
	CM_SOCIAL_MIN				= 3000,

	CM_QUERY_MAIL_LIST			= 3001,
	CM_GET_MAIL_INFO			= 3002,
	CM_GET_MAIL_POST			= 3003,
	CM_SEND_MAIL				= 3004,
	CM_DELETE_MAIL				= 3005,
//	CM_READ_MAIL				= 3006,
	CM_SEND_SYS_MAIL			= 3007,

	CM_RELATION_REQUEST_LIST	= 3008,			//��ϵ�б����������ȵȣ�
	CM_ADD_RELATION				= 3009,			//���Ӻ���
	CM_DELETE_RELATION			= 3010,			//ɾ������
	CM_REMOVE_BAD_RELATION		= 3011,

	CM_FAMILY_REQUEST_LIST				= 3012,			// �����б�
	CM_FAMILY_REQUEST_LOGS				= 3013,			// �鿴������־
	CM_FAMILY_REQUEST_INFO				= 3014,			// ������Ϣ
	CM_FAMILY_REQUEST_MEMBER_LIST		= 3015,			// �鿴���ɳ�Ա�б�
	CM_FAMILY_REQUEST_TOTOM_LIST		= 3016,			// �鿴����ͼ��
	CM_FAMILY_REQUEST_APPLIERS			= 3017,			// ����������Ϣ
	CM_FAMILY_REQUEST_PLAYER_PETS		= 3018,			// �鿴ע�����
	CM_FAMILY_EDIT_NOTICE				= 3019,			// �༭���ɹ���
	CM_FAMILY_CREATE					= 3020,			// ��������
	CM_FAMILY_APPLY						= 3021,			// �������
	CM_FAMILY_CANCLE_APPLY				= 3022,			// ȡ������
	CM_FAMILY_DESTROY					= 3023,			// ��ɢ����
	CM_FAMILY_REQUEST_REGIST_PETS		= 3024,			// ���ɳ����б�
	CM_FAMILY_APPOINT					= 3025,			// ��������
	CM_FAMILY_APPROVE					= 3026,			// ��׼����
	CM_FAMILY_KICK						= 3027,			// ����T��
	CM_FAMILY_LEAVE						= 3028,			// �뿪����
	CM_FAMILY_ACTIVE_TOTOM				= 3029,			// �������ͼ��
	CM_FAMILY_TASK_COUNT				= 3030,			// ���������������

	//CM_TEAM_DUNGEON_ADD					= 3031,			// ����//����
	//CM_TEAM_DUNGEON_QUERY				= 3032,			// ����
	//CM_TEAM_DUNGEON_INVITE				= 3033,			// ����//���細�ڸ�������

	CM_KILLER_RANK_QUERY				= 3034,			//��ѯ
	CM_QUERY_ALL_ENEMY_INFO				= 3035,			//��ѯ ���� ���� ��Ϣ
	CM_GOTO_ENEMY						= 3036,			//׷ɱ����

	CM_RANK_REQUEST_INFO				= 3037,			// ���� //���а���Ϣ
	CM_QUERY_REAL_TIME_RANK				= 3038,			// ����
	CM_QUERY_TITLE_POSITION				= 3039,			// ����//�鿴�پ�
	CM_QUERY_KINGDOM_TOP				= 3040,
	CM_SET_KINGDOM_POSTION				= 3041,			// ����
	CM_QUERY_GAMBLE_RECORD				= 3042,			// ����
	CM_ASK_KINGDOM_POST					= 3043,			// ����
    CM_REFUSE_KINGDOM_REQUIRE			= 3044,			// ����
	CM_QUERY_LIMIT_SHOP					= 3045,
	CM_BUY_LIMIT_SHOP_ITEM				= 3046,
	CM_QUERY_MYSTERIOUS_SHOP			= 3047,			//��ѯ�����̵�
	CM_EXCHANGE_MYSTERIOUS_SHOP_ITEM	= 3048,
	
	CM_SET_OFFICE						= 3049,
	CM_USE_EXCHANGE_CODE				= 3050,

	CM_SOCIAL_PUBLIC_CHAT				= 3051,			//��������
	CM_SOCIAL_PRIVATE_CHAT				= 3052,			//˽��

	CM_CHANGE_NAME						= 3053,
	CM_CHANGE_FAMILY_NAME				= 3054,
	CM_QUERY_ACTIVITY_INFO				= 3055,			// ����
	CM_GET_AWARD_ZHIYUANQIANXIAN		= 3056,			// ����
	CM_GET_JUANXIANWUZI_INFO			= 3057,			// ����

	// ���ϵͳ
	//CM_QUERY_TEAMS_AROUND				= 3058,			// ��Ѱ��������
	CM_CREATE_TEAM						= 3059,			// ��������
	CM_INVITE_INTO_TEAM					= 3060,			// ����������
	CM_SET_TEAM_LEADER					= 3061,			// �ӳ�ת��
	CM_KICKOUT_TEAM_MEMBER				= 3062,			// ����
	CM_LEAVE_TEAM						= 3063,			// �뿪����
	CM_APPLY_INTO_TEAM					= 3064,			// �������
	CM_REPLY_TEAM_INVITE				= 3065,			// �ظ��������
	CM_REPLY_TEAM_APPLY					= 3066,			// �ظ��������
	CM_TEAM_DESTROY_TEAM				= 3067,			// �ӳ���ɢ����
	//CM_TEAM_SET_AUTO_OPERATE			= 3068,			// ���Ĭ������
	//����
	CM_FAMILY_AUTO_AGREE				= 3069,			// �����Զ�
	//����ϵͳ
	CM_ASK_FRIEND_LIST					= 3071,			// ��������б�
	CM_ASK_ADD_FRIEND					= 3072,			// �������Ӻ���
	CM_REPLAY_ADD_FRIEND				= 3073,			// ���������
	CM_DELETE_FRIEND					= 3074,			// ɾ������
	CM_SIFT_FRIEND						= 3075,			// ɸѡ����
	CM_FIND_PLAYER						= 3076,			// ���Һ���
	CM_GET_PLAYER_INFO					= 3077,			// ��ȡ�����Ϣ

	CM_ASK_JUE_WEI_RANK					= 3078,			// �����λ���а�
	CM_ANSWER_QUESTIONS					= 3079,			// �ش�����

	// ���а�
	CM_ASK_PET_RANK_INFO				= 3080,			// ����������а���Ϣ
	CM_ASK_PET_RANK_LIST				= 3081,			// ����������а��б���
	CM_ASK_GAME_RANK					= 3082,			// ������Ϸ���а���Ϣ
	CM_REQUEST_WORSHIP_INFO				= 3083,			// ��������Ϣ
	CM_WORSHIP							= 3084,			// ���

	// ��Ӹ���
	CM_TEAM_DUNGEON_REQUEST_LIST		= 3090,			// ������Ӹ����б�
	CM_TEAM_DUNGEON_LEAVE				= 3091,			// �뿪��Ӹ���
	CM_TEAM_DUNGEON_KICK				= 3092,			// ��Ӹ�������
	CM_TEAM_DUNGEON_READY				= 3093,			// ��Ӹ�����ʼ
	CM_TEAM_DUNGEON_SET_AUTO_START		= 3094,			// ������Ա�Զ�����
	// ��������
	CM_ASK_UP_TOWER_DUNGEON_RANK		= 3100,			// ���󸱱����а�
	//����
	CM_ASK_DA_TI_INFO					= 3101,			// ���������Ϣ

	CM_ASK_ONE_TOU_ZI_RECORD			= 3102,			// ����һ��Ͷ����Ϣ
	CM_ASK_ALL_TOU_ZI_RECORD			= 3103,			// ��������Ͷ����Ϣ

	// ����
	CM_AUCTION_SELL						= 3110,			// �ϼ�
	CM_AUCTION_BUY						= 3111,			// ����
	CM_AUCTION_CANCEL					= 3112,			// �¼�
	CM_AUCTION_REQUEST_ITEM_LIST		= 3113,			// ����������Ϣ
	CM_AUCTION_REQUEST_SELF_ITEMS		= 3114,			// �����Լ��������б�
	CM_AUCTION_REQUEST_RECORD			= 3115,			// �����׼�¼

	// ������
	CM_HALL_OF_FAME_RANK				= 3120,			// �������������а�
	CM_FAMILY_OPEN_TOTOM				= 3121,			// �������ͼ��
	CM_HALL_OF_FAME_GET_REWARD			= 3122,			// ���������ý���

	CM_SOCIAL_MAX,
	//������ 

	CM_KINGDOM_MIN						= 4000,
	
	CM_KINGDOM_MAX,

	CM_GM_MIN							= 9000,
	CM_GM_LOGIN							= 9001,
	CM_GM_BAN_LOGIN						= 9002,
	CM_GM_BAN_CHAT						= 9003,
	CM_GM_KICK_USER						= 9004,
	CM_GM_ANNOUNCEMENT					= 9005,
	CM_GM_SENDMAIL						= 9006,
	CM_GM_BAN_IP						= 9007,
	CM_GM_RELOAD_INFO					= 9008,
	CM_GM_EXACTIVITY					= 9009,
	CM_GM_CLOSE_LINE					= 9900,
	CM_GM_RELOAD_ITEM					= 9901,
	CM_GM_MAX,
};

/*
*SM_GAME_MIN						= 10000,
*SM_GM_MIN							= 19000,
*SM_PHP_MIN							= 19500,	��
*/
enum MSG_SM_CODE
{
	SM_MIN						= 10000,
	SM_LINE_LIST				= 10001,
	SM_LINE_CLOSED				= 10002,
	SM_KICK_OUT					= 10003,
	SM_USER_TOKEN				= 10004,
	SM_CHARACTER_LIST			= 10005,
	SM_OPEN_PANEL				= 10006,
	SM_SYNC_TIME				= 10007,
	SM_LOGIN_INFO				= 10008,
	SM_MOVE						= 10009,
	SM_KICK_OUT_MOVE			= 10010,
	SM_UNIT_THROWED				= 10011,
	SM_INSTANT_MOVE				= 10012,
	SM_SELF_ENTER_AREA			= 10013,
	SM_UNIT_ENTER_AREA			= 10014,
	SM_ENTER_MAP				= 10015,
	SM_LEAVE_MAP				= 10016,
	SM_PLYAER_INTO_MAP			= 10017,
	SM_MONSTER_INTO_MAP			= 10018,
	SM_NPC_INTO_MAP				= 10019,
	SM_PET_INTO_MAP				= 10020,
	SM_LACKEY_INTO_MAP			= 10022,
	// ����һ��
	SM_TRAILER_INTO_MAP			= 10023,
	SM_DUNGEON_EVENT			= 10024,
	SM_DUNGEON_MOVIE			= 10025,
	SM_STACKS_ADD_INTO_MAP		= 10026,		// ���ӵ�ͼ��ײ
	SM_STACKS_REMOVE_FROM_MAP	= 10027,		// �Ƴ���ͼ��ײ
	SM_FAMILY_ACTIVITY_NAME		= 10028,		// ����
	SM_FAMILY_ACTIVITY_FAIL		= 10029,		// ����
	SM_FAMILY_POT_ACTIVITY_END	= 10030,		// ����
	SM_FAMILY_POST_INFO			= 10031,		// ����
	SM_FAMILY_POST_UPDATE_PLANT	= 10032,		// ����
	SM_FAMILY_CHR_POST_INFO		= 10033,		// ����
	SM_MONSTER_PLAYER_DAMAGE_INFO	= 10034,
	SM_WORLD_BOSS_ACTIVITY_RESULT	= 10035,
	SM_FINISH_DUNGEON			= 10036,		// �����������
	SM_DUNGEON_RESULT			= 10037,
	SM_SKILL_CHANT				= 10038,
	SM_SKILL_ACTION				= 10039,
	SM_SKILL_RESULT				= 10040,
	SM_UNIT_BASIC_DATA			= 10041,
	SM_UNIT_BUFF_LIST			= 10042,
	SM_UNIT_BUFF_END			= 10043,
	SM_PLAYER_STATUS			= 10044,
	SM_ADD_DROP_ITEM			= 10045,
	SM_REMOVE_DROP_ITEM			= 10046,
	SM_ADD_TRAP					= 10047,
	SM_REMOVE_TRAP				= 10048,
	SM_TRAP_STATE				= 10049,
	SM_FIGHT_EXP_MONEY			= 10050,
	SM_ADD_PLANT				= 10051,
	SM_REMOVE_PLANT				= 10052,
	SM_END_GATHER				= 10053,
	SM_END_SIT					= 10054,		// ����

	// �������Э��
	SM_TEAMS_AROUND				= 10055,		// �·���������
	SM_TEAM_MEMBERS				= 10056,		// �·�������Ϣ
	SM_TEAM_MEMBER_BASIC_DATA	= 10057,		// ���������Ϣ������
	SM_TEAM_INVITE				= 10058,		// ת��������Ϣ
	SM_TEAM_APPLY				= 10059,		// ת��������Ϣ
	SM_TEAM_LEAVED				= 10060,		// ����뿪����

	SM_EQUIP_INFO				= 10061,
	SM_EQUIP_ENHANCE_TIME		= 10062,		// ����
	SM_EQUIP_DISMANTLE_RESULT	= 10063,		// ����
	SM_EQUIP_MAGIC_SMITH		= 10064,		// ����
	SM_ITEM_INFO				= 10065,		// ����
	SM_NEW_ITEM					= 10066,		// ����
	SM_ITEM_EFFECT				= 10067,
	SM_STORE_INFO				= 10068,
	SM_CHR_INFO					= 10069,
	SM_CHR_INFO_TYPE			= 10070,
	SM_CHR_KINGDOM_INFO			= 10071,		// ����
	SM_VIEW_STAR_FINISHED		= 10072,
	SM_BAG_ITEMS				= 10073,
	SM_BAG_ITEMS_CHANGE			= 10074,
	SM_MAGIC_WEAPON_INFO		= 10075,
	SM_GOLD_CASH_CHANGE			= 10076,
	SM_JUNGONG_CHANGE			= 10077,
	SM_WUHUEN_CHANGE			= 10078,
	SM_JIANGCING_CHANGE			= 10079,
	SM_ZHIYUANQIANXIAN_WUZI_CHANGE	= 10080,		// ����
	SM_KINGDOM_CONTRIBUTE_CHANGE	= 10081,
	SM_MONEY_CHANGE					= 10082,
	SM_GAIN_INFO					= 10083,
	SM_REWARD_INFO					= 10084,		// ����
	SM_EXPIRE_INFO					= 10085,
	SM_BUY_ITEM_INFO				= 10086,
	SM_SELL_ITEM_INFO				= 10087,
	SM_CONSUME_ITEM_INFO			= 10088,
	SM_ZHENFA_LIST					= 10089,		// ����
	SM_SKILL_LIST					= 10090,
	SM_STRATEGICS_LIST				= 10091,		// ����
	SM_NEW_SKILL					= 10092,
	SM_REFUGEE_KILL_MONSTER			= 10093,
	SM_TASK_LIST					= 10094,
	SM_TASK_CYCLE					= 10095,
	SM_TASK_KINGDOM					= 10096,
	SM_TASK_FAMILY					= 10097,
	SM_TASK_KILLED_MONSTER			= 10098,
	SM_TASK_RECEIVED				= 10099,
	SM_TASK_CAN_SUBMIT				= 10100,
	SM_TASK_SUBMITTED				= 10101,
	SM_TASK_GIVEUPED				= 10102,
	SM_TASK_CYCLE_RECEIVED			= 10103,
	SM_TASK_CYCLE_SUBMITTED			= 10104,
	SM_TASK_FAMILY_RECEIVED			= 10105,
	SM_TASK_FAMILY_SUBMITTED		= 10106,
	SM_TASK_KINGDOM_RECEIVED		= 10107,
	SM_TASK_KINGDOM_SUBMITTED		= 10108,
	SM_ENTERUST_INFO				= 10109,
	SM_DAILY_ITEM_INFO				= 10110,
	SM_DAILY_ITEM_CHANGE			= 10111,
	SM_DAILY_MARK_CHANGE			= 10112,
	SM_SHIZHUANG_FORM_INFO			= 10113,		// ����
	SM_ENTRUST_GROUP_REWARD			= 10114,		// ����
	SM_CHARGE_REWARD_INFO			= 10115,
	SM_DAILY_REWARD					= 10116,
	SM_ACTVIVTY_JIFEN				= 10117,
	SM_ACTIVITY_SOUTHINVADE			= 10118,
	SM_ACTIVITY_KILLERIST_BOARDCAST	= 10119,
	SM_ACTVIVTY_DATA				= 10120,		// ����
	SM_ACTVITY_PLANTS				= 10121,
	SM_ACTVIVTY_KING_MSG			= 10122,
	SM_SECOND_DAY_LOGIN				= 10123,
	SM_SEARCH_BACK_INFO				= 10124,
	SM_VICE_GENERAL_DIE				= 10125,
	SM_AC_EXCHANGE					= 10126,
	SM_AC_OUTDOOR					= 10127,
	SM_AC_CAVE						= 10128,
	SM_AC_GROW_UP					= 10129,
	SM_AC_CHALLENGE					= 10130,
	SM_AC_SPECIAL					= 10131,
	SM_AC_DAILY						= 10132,
	SM_AC_TASK						= 10133,

	SM_WIND_POINT					= 10134,
	SM_FIND_EXP						= 10135,
    SM_VICEGENERAL_INFO				= 10136,
	SM_VICEGENERAL_SUITE_QUITE		= 10137,
	SM_VICEGENERAL_HP_INFO			= 10138,
	SM_MOUNT_NEW_LEVEL				= 10139,
	SM_VICE_GENERAL_LIST			= 10140,
	SM_NEW_VICE_GENERAL				= 10141,
	SM_VICE_GENERAL_NEW_LEVEL		= 10142,
	SM_LACKEY_INFO					= 10143,		// ����
	SM_LACKEY_ADD_MPHP				= 10144,		// ����
	SM_MOUNT_LIST					= 10145,
	SM_FLY_COUNT					= 10146,
	SM_PILL_LIST					= 10147,		//����
	SM_ACTION_LIST					= 10148,
	SM_AUTO_FIGHT					= 10149,
	SM_SYSTEM_SETTING				= 10150,
	SM_GAME_SHOP					= 10151,
	SM_GAME_SHOP_LIMIT				= 10152,
	SM_GAME_MYSTERIOUS_SHOP			= 10153,
	SM_UPDATE_MYSTERIOUS_SHOP		= 10154, 
	SM_ONLINE_REWARD_STATE			= 10155,		//��������ʱ����Ϣ
	SM_LAN_AND_VAL_GIFT_INFO		= 10156,
	SM_CHR_RECORD					= 10157,
	SM_ANNOUCEMENT					= 10158,
	SM_GAME_EVENT					= 10159,
	SM_KILLED_BY_PLAYER				= 10160,

	SM_MONSTER_BROADCAST			= 10161,
	SM_GM_ANNOUNCEMENT				= 10162,
	SM_GAMBLE_BORADCAST				= 10163,		// ����

	SM_TEAM_DUNGEON_BORADCAST		= 10164,		// ����
	SM_TEAM_DUNGEON_INFO			= 10165,		// ����
	SM_INVITE_TEAM_DUNGEON			= 10166,		// ����

	SM_ACTIVITY_PKSORCE				= 10167,		//pk���������Ӯ
	SM_ACTIVITY_MATCH_DATA			= 10168,		//�������ݸ���
	SM_ACTIVITY_MSG					= 10169,
	SM_ACTIVITY_RESLUT				= 10170,		//���ͱ������
	SM_ACTIVITY_QUIZ				= 10171,
	SM_ACTIVITY_STARTFRONT			= 10172,		//���ʼǰ
	SM_ACTIVITY_STATE				= 10173,
	SM_ACTIVITY_MAP_STATE			= 10174,
	SM_ACTIVITY_EVENT				= 10175,
	SM_CHR_ACTIVITY					= 10176,
	SM_CHR_ACTIVITY_GAIN			= 10177,
	SM_DRINK_EVENT					= 10178,
	SM_GOLD_EVENT					= 10179,
	SM_OFFLINE_EXP_DATA				= 10180,
	SM_KINGDOM_RANK					= 10181,
	SM_PRIVATE_CHAT					= 10182,
	SM_FAMILY_STATE_DATA			= 10183,
	SM_MAIL_LIST					= 10184,
	SM_MAIL_INFO					= 10185,
	SM_GUESS_INFO					= 10186,
	SM_NEW_MAIL						= 10187,
	SM_HAVE_NO_READ_MAIL			= 10188,
	SM_FRIEND_INFO					= 10189,
	SM_RELATION_MESSAGE				= 10190,
	SM_FAMILY_LIST					= 10191,				// �·������б�
	SM_FAMILY_NOTICE				= 10192,				// �·����ɹ���
	SM_FAMILY_INFO					= 10193,				// �·�������Ϣ
	SM_FAMILY_MEMBER_INFO			= 10194,				// �·����ɳ�Ա��Ϣ
	SM_FAMILY_TOTOM_INFO			= 10195,				// �·�ͼ����Ϣ
	SM_FAMILY_APPLIERS				= 10196,				// �·�����������Ϣ
	SM_FAMILY_PETS					= 10197,				// �·����ɵǼǳ���
	SM_FAMILY_EVENT					= 10198,				// �·�����֪ͨ
	SM_FAMILY_PLAYER_PETS			= 10199,				// �·������Ǽǻ���
	SM_FAMILY_APPLY_RESULT			= 10200,				// �·�������
	SM_FAMILY_LOGS					= 10201,				// �·�������־
	SM_FAMILY_JOINED				= 10202,				// �ɹ��������
	SM_FAMILY_TASK_COUNT			= 10203,				// �·�������������
	SM_REAL_TIME_RANK_RESULT		= 10204,
	SM_REAL_TIME_RANK_TIME			= 10205,
	SM_TITLE_POSITION				= 10206,				// ����
	SM_FAMILY_WAR_EVENT				= 10207,
	SM_FAMILY_WAR_WINNER			= 10208,				// ����
	SM_PEOPLEBOOK_TAOYUAN			= 10209,				// ����
	SM_PEOPLEBOOK_YANWUCHANG		= 10210,				// ����
	SM_PEOPLEBOOK_FENGYINBANG		= 10211,				// ����
	SM_GLEBEBOOK_SHENLONGDING		= 10212,				// ����
	SM_GLEBEBOOK_SHEJITU			= 10213,				// ����
	SM_GLEBEBOOK_LINGTAIFUDI		= 10214,				// ����
	SM_SKYBOOK_KUNLUNJING			= 10215,				// ����
	SM_SKYBOOK_JIUTIANTA			= 10216,				// ����
	SM_SKYBOOK_FUMO					= 10217,				// ����
	SM_SKYBOOK_DENGTIANTI			= 10218,				// ����
	SM_BANQUET_TOAST				= 10219,
	SM_PVP_INFO						= 10220,
	SM_DAILY_PK_INFO				= 10221,
	SM_ENEMY_INFO					= 10222,
	SM_ENEMY_INFO_SINGLE			= 10223,
	SM_TRYOUT_VIP_INFO				= 10224,
	SM_KILLER_RANK_INFO				= 10225,				// ����
	SM_KILLER_RANK_SELF				= 10226,
	SM_EVERY_DAY_GET_GOLD			= 10227,
	SM_LEVEL_GIFT					= 10228,
	SM_LEVEL_GOLD					= 10229,
	SM_GAMBLE_SINGLE_INFO			= 10230,
	SM_PUBLIC_CHAT					= 10231,
	SM_GAMBLE_DEPOT					= 10232,				//ϡ�������䱦�ֿ�
	SM_SIGN_INFO					= 10233,				//����ǩ����Ϣ
	SM_JUANXIANWUZI_INFO			= 10234,				// ����
	SM_SEVEN_LOGIN					= 10235,

	SM_NEW_SERVER_ACTIVITY_INFO		= 10236,
	SM_NSA_JIUTIANTA				= 10237,
	SM_NSA_EQUIP					= 10238,
	SM_NSA_GEM						= 10239,
	SM_NAS_LATER					= 10240,
	SM_SHOP_LIMIT_NOTE				= 10241,
	SM_CHR_LOGIN_IN_INFO			= 10242,

	SM_GAIN_VICEGENERAL_BROADCAST	= 10243,
	SM_VICEGENERAL_UPGRADE_BROADCAST	= 10244,
	SM_MOUNT_UPGRADE_BROADCAST			= 10245,
	SM_VICEGENERAL_GENGU_BROADCAST		= 10246,
	SM_VICEGENERAL_WUYI_BROADCAST		= 10247,
	SM_OPEN_LEVEL_BROADCAST				= 10248,				// ����
	SM_CHANGE_JOB_BROADCAST				= 10249,				// ����
	SM_LACKEY_UPGRADE_BROADCAST			= 10250,				// ����
	SM_ITEM_COMBINE_BROADCAST			= 10251,				// ����
	SM_SUBMIT_GOLD_EQUIP_BROADCAST		= 10252,				// ����
	SM_READ_BROADCAST					= 10253,				// ����
	SM_VIP_BROADCAST					= 10254,
	SM_RECHARGE_REWARD_BROADCAST		= 10255,
	SM_GET_DIAMOND_RING					= 10256,

	SM_CHANGE_POSITION					= 10257,				// ����

	SM_KINGDOM_MIN						= 10258,
	SM_KINGDOM_POSTION					= 10259,
	SM_KINGDOM_FENG						= 10260,
	SM_KINGDOM_CREAT					= 10261,			//��������
	SM_KINGDOM_GUANJUE					= 10262,			//�پ��ܹ�
	SM_KINGDOM_TWEAL_CONFIG				= 10263,			//�����ӳ�������Ϣ
	SM_KINGDOM_TOP						= 10264,			//
	SM_KINGDOM_MAX						= 10265,

	SM_BANQUET_INFO						= 10266,
	SM_GAMBLE							= 10267,				// ����
	SM_QUERY_PLAYER_INFO				= 10268,
	SM_QUERY_ACTIVITY_INFO				= 10269,
	SM_EXACTIVITY_INFO					= 10270,				//����
	SM_EXACTIVITY_CFG					= 10271,				//����

	SM_CHARGE_SUCCESS					= 10272,
	SM_FLY_ICON_INT						= 10273,

	SM_YELLOW_STONE						= 10274,
	SM_PREVENT_WALLOW					= 10275,				//������
	SM_SKYBOOK_ADVANCEDTRIALSQUERY		= 10276,				// ����//����������ʼ��
	SM_ADVANCEDTRIALS_RAIDS_INFO		= 10277,				// ����//��������ɨ����Ϣ
	SM_ADVANCEDTRIALS_BOSS_RESULT		= 10278,				// ����//��������BOSS�ֵ� ֪ͨ
	SM_MYSTERIOUS_SHOP_QUERY			= 10279,				//�����̵��ʼ��
	
	SM_VICE_GENERAL_TIME_QUERY			= 10280,				//�佫��ļʱ����Ϣ
	SM_VICE_GENERAL_RECRUIT				= 10281,				//��ļ���佫
	SM_VICE_GENERAL_SHOW				= 10282,				//Ԥ���佫
	SM_VICE_GENERAL_WORLD_BROADCAST		= 10283,				//����㲥����佫

	SM_ACTIVITY_STARTFIVEFRONT			= 10299,				//��ǰ5��������

	SM_HIGH_TAOYUAN_ERROR				= 10300,				// ����//�߼�����ʧ��
	SM_THREE_SHOP						= 10301,				// ����//3���̳ǹ�����Ϣ



	/*
	* SM_ZK	11400~11699
	*/
	SM_NOTIFY_MOUNT_INFO				= 11407,		//֪ͨ������Ϣ
	SM_NOTIFY_MOUNT_TRAIN_RESULT		= 11408,		//֪ͨ�����������
	SM_NOTIFY_MOUNT_SKILL_INFO			= 11409,		//֪ͨ���＼����Ϣ

	SM_NOTIFY_TI_SHI_INFO				= 11415,		//��ʾ��Ϣ
	SM_NOTIFY_MOUNT_UPDATE				= 11416,		//�������׹���

	SM_NOTIFY_KILL_MONSTER_COUNT		= 11417,		//�·�ɱ��ͳ����Ϣ
	SM_NOTIFY_KILL_MONSTER_CHANGE		= 11418,		//�·�ɱ�����仯��Ϣ

	SM_NOTIFY_DEPOT_CURRENCY_INFO		= 11419,		//�·��ֿ������Ϣ
	SM_NOTIFY_DEPOT_INFO				= 11420,		//�·��ֿ���Ϣ
	SM_NOTIFY_DEPOT_ITEM				= 11421,		//�·��ֿ���Ʒ
	//����ϵͳ
	SM_SEND_FRIEND_LIST					= 11422,		//�·������б�
	SM_SEND_SIFT_FRIEND					= 11423,		//�·�ɸѡ����
	SM_SEND_FRIEND_REQUEST				= 11424,		//�·���������
	SM_SEND_FRIEND_REWARD				= 11425,		//�·����ѽ���
	
	SM_NOTIFY_EQUIP_UP_STAR				= 11426,		//װ��ǿ������
	//����
	SM_SEND_ASK_TRADE					= 11427,		//���ͽ�������
	SM_TRADE_ITEM_CHANGE				= 11428,		//�·���������Ʒ�仯
	SM_TARGET_TRADE_ITEM_CHANGE			= 11429,		//֪ͨ�Է���������Ʒ�仯
	SM_TRADE_MONEY_CHANGE				= 11430,		//�·���������Ǯ�仯
	SM_TARGET_MONEY_CHANGE				= 11431,		//֪ͨ�Է���������Ǯ�仯
	SM_SEND_TRADE_SUCCEED				= 11432,		//֪ͨ���׳ɹ�
	SM_SEND_OPERATE_TO_TARGET			= 11433,		//�Ѳ���֪ͨ�Է�

	//װ�����
	SM_SEND_RANSOM_INFO					= 11434,		//����װ�������Ϣ
	SM_SEND_SPOILS_INFO					= 11435,		//����ս��Ʒ��Ϣ
	//�ʼ�					
	SM_SEND_MAIL_LIST					= 11436,		//�����ʼ��б�
	SM_SEND_MAIL_CONTENT				= 11437,		//�����ʼ�����
	SM_SEND_NEW_MAIL					= 11438,		//�������ʼ�
	//�̳�
	SM_SEND_ITEM_LIMIT_COUNT			= 11439,		//�����̳����Ƹ���
	//����
	SM_SEND_FA_BAO_INFO					= 11440,		//���ͷ�����Ϣ
	SM_SEND_FA_BAO_GONG_GAO				= 11441,		//��������
	SM_SEND_ALL_FA_BAO_INFO			= 11446,
	//����boss��Ϣ
	SM_SEND_BOSS_INFO					= 11442,		//����boos��Ϣ
	SM_SEND_BOSS_GONG_GAO				= 11443,		//BOSS���������
	//����ս������Ϣ
	SM_SEND_BATTLE_INFO					= 11444,		//����ս����Ϣ
	//��λ
	SM_SEND_JUE_WEI_DONATE_MONEY		= 11445,		//���;�λ���׵�ͭǮ
	SM_SEND_JUE_WEI_RANK				= 11446,		//���;�λ��������
	SM_SEND_JUE_TOTAL_RANK				= 11447,		//���;�λ�����а�
	SM_SEND_JUE_WEI_GONG_GAO			= 11448,		//����
	SM_LEVEL_REFIN_BROADCAST	= 0x2CD6,
	SM_FAMILY_WAR_ICON			= 0x2CD7,
	SM_SEND_LEVEL_REFIN			= 0x2D0A,

	//����
	SM_SEND_DA_TI_READY_START			= 11450,		//׼����ʼ
	SM_SEND_DA_TI_RANK					= 11451,		//�������а�
	SM_SEND_QUESTIONS_TO_CLIENT			= 11452,		//����������ͻ���
	SM_SEND_CHAR_DA_TI_INFO				= 11453,		//���͸�����Ϣ
	SM_SEND_DA_TI_HUO_DONG_ID			= 11454,		//���ʹ���id

	//��������
	SM_SEND_ZSZZ_DUNGEON_RANK			= 11456,		//��������֮ս������Ϣ
	SM_ZSZZ_DUNGEON_GONG_GAO			= 11457,		//ͨ�ع���
	//֪ͨ�ͼ��
	SM_SEND_ALL_ICON					= 11458,		//��½�·����лͼ��
	SM_SEND_ONE_ICON					= 11459,		//�·������ͼ��
	//�齱ϵͳ
	SM_SEND_CHOU_JIANG_RECORD			= 11465,		//���͸��˳齱��¼
	SM_SEND_ADD_CHOU_JIANG_RECORD		= 11466,		//����һ�����˳齱��¼
	SM_SEND_GLOBAL_CHOU_JIANG_RECORD	= 11467,		//����ȫ���齱��¼
	SM_SEND_ADD_GLOBAL_CHOU_JIANG_RECORD= 11468,		//����һ��ȫ���齱��¼
	SM_SEND_SCORE_SHOP_LIMIT_INFO		= 11469,		//�̳�������Ϣ
	SM_SEND_SCORE_SHOP_RECORD			= 11470,		//����̳ǹ�¼
	SM_SEND_ALL_CHOU_JIANG_ITEM			= 11470,		//���ͳ齱������Ϣ
	SM_SEND_CHOU_JIANG_ITEM_CHANGE		= 11471,		//���ͳ齱������Ʒ�ı�
	SM_SEND_CHOU_JIANG_GONG_GAO			= 11472,		//���ͳ齱����
	SM_SEND_GET_ITEM					= 11473,		//�齱�鵽����Ʒ
	//����ϵͳ
	SM_SEND_QI_FU_INFO					= 11475,		//����������Ϣ
	SM_SEND_QI_FU_SUCCESS				= 11476,		//���������ɹ�
	//��ʾ��Ϣ
	SM_GM_BROADCAST						= 11477,		// GM��̨���͹���
	SM_SEND_NOTICE_PARAM				= 11478,		//������ʾ��Ϣ����
	//��θ��
	SM_SEND_DWW_INFO					= 11479,		//���ʹ�θ����Ϣ
	//���������
	SM_SEND_HD_DA_TING_INFO				= 11480,		//���ͻ��������
	//VIP
	SM_SEND_VIP_INFO					= 11481,		//����vip��Ϣ
	// boss֮��
	SM_SEND_BOSS_HOME_INFO				= 11485,		//����boss֮����Ϣ
	SM_SEND_BOSS_CHANGE					= 11486,		//boss��Ϣ�ı�
	// �ɼ�
	SM_PLANT_GAIN_ITEM					= 11489,		//���Ͳɼ���ȡ��Ʒ
	//��Դ�һ�
	SM_SEND_SEARCH_BACK_INFO			= 11490,		//������Դ�һ���Ϣ
	//������
	SM_SEND_EXP_BALL_INFO				= 11491,		//���;�������Ϣ
	//��Ӫ�
	SM_SEND_SHOU_CHONG_INFO				= 11492,		//�����׳���Ϣ
	SM_SEND_NEW_SERVER_FAVORABLE		= 11493,		//�����·��ػ���Ϣ
	SM_SEND_EVERYDAY_CHONG_ZHI			= 11494,		//����ÿ�ճ�ֵ��Ϣ
	SM_SEND_TOTAL_CHONG_ZHI				= 11495,		//�ۼƳ�ֵ��Ϣ

	//Ͷ��
	SM_SEND_TOU_ZI_INFO					= 11496,		//����Ͷ����Ϣ
	SM_SEND_GET_RESULT					= 11497,		//��ȡ�ɹ�
	SM_SEND_TOU_ZI_RECORD				= 11498,		//����Ͷ�ʼ�¼
	// �����
	SM_SEND_KAI_FU_HUO_DONG_INFO		= 11500,		//���Ϳ������Ϣ
	SM_SEND_KAI_FU_HUO_DONG_STATE		= 11501,		//���ͻ״̬
	// ��Ծ��
	SM_SEND_HUO_YUE_DU_INFO				= 11502,		//���ͻ�Ծ����Ϣ
	//��λ
	SM_SEND_GUAN_WEI_INFO				= 11504,		//���͹�λ��Ϣ
	SM_SEND_QI_SHI_INFO					= 11505,		//������ʿ��Ϣ
	SM_SEND_TITLE_INFO			= 11506,
	//�ɾ�
	SM_SEND_ACHIEVEMENT_INFO			= 11510,		//���ͳɾ�ϵͳ��Ϣ
	//���Ϳ�սʤ�������
	SM_SEND_WAR_VICTORY_INFO			= 11515,		//���Ϳ�սʤ�����Ϣ
	/*
	* SM_WMF	11700~11899
	*/

	SM_UPDATE_FIGHT_STATE				= 11700,	// ����ս��״̬

	//����
	SM_SEND_BAG_DIRTY					= 11703,	// �·�����������
	SM_SEND_BAG_INFO					= 11704,	// �·���������
	SM_SEND_BAG_SELL_ITEMS				= 11705,	// �·��ع���Ϣ
	SM_SEND_CHAR_SHOP_LINIT				= 11706,	// �����̵��޹���Ϣ

	// װ����
	SM_SEND_EQUIP_INFO					= 11710,	// ����װ����Ϣ
	SM_SEND_GEM_INFO					= 11711,	// ���ͱ�ʯ��Ƕ��Ϣ
	SM_SEND_OPEN_GEM_HOLE				= 11712,	// ���ͱ�ʯ������Ϣ
	// ����
	SM_SEND_CURRENCY_INFO				= 11720,	// ���ͻ�����Ϣ

	// ����
	SM_SEND_PET_INFO					= 11730,	// ���ͻ�����Ϣ
	SM_SEND_PET_EGG_INFO				= 11731,	// ���ͳ��ﵰ��Ϣ
	SM_SEND_PET_LIST					= 11732,	// ������һ����б�
	SM_SEND_PET_LEARN_NEW_SKILL			= 11733,	// ���ͻ���ѧϰ���¼���
	SM_PET_BROADCASE					= 11734,	// ���޹���
	SM_SEND_PET_STATUS					= 11735,	// ����״̬���°��������ͣ�

	// XP����
	SM_XP_INFO							= 11740,	// XP��Ϣ
	SM_SEND_XP_EXP_BOARD				= 11741,	// ���XP�������

	// �������а�
	SM_PET_RANK_INFO					= 11750,	// �������а���Ϣ
	SM_PET_RANK_LIST					= 11751,	// �������а�
	SM_PET_SET_FIGHT					= 11752,	// �����ս��Ļ����Ч

	// ����
	SM_DUNGEON_INFO_BOARD				= 11760,	// �·�������Ϣ���

	// ��Ӹ���
	SM_TEAM_DUNGEON_SEND_LIST			= 11770,	// �·���Ӹ����б�
	SM_TEAM_DUNGEON_MEMBER_INFO			= 11771,	// �·���Ӹ�����Ա��Ϣ
	SM_TEAM_DUNGEON_MEMBER_LEAVE		= 11772,	// �·�����뿪��Ӹ�����Ϣ
	SM_TEAM_DUNGEON_START				= 11773,	// �·���Ӹ�������������ʱ��ʼ��
	SM_TEAM_DUNGEON_DAMAGE_LIST			= 11774,	// �·���Ӹ����˺�ͳ��

	// ����
	SM_INSIDE_PET_SEND_BASE_INFO		= 11780,	// �·�������Ϣ
	SM_INSIDE_PET_SEND_BASE_ILLUSION	= 11781,	// �·����޳�����Ϣ

	// ��Ϸ���а�
	SM_GAME_RANK_DATA					= 11790,	// �·���Ϸ���а�
	SM_SELF_WORSHIP_INFO				= 11791,	// �·�������������Ϣ
	SM_NOTIFY_WORSHIP_INFO				= 11792,	// �·���ҳ����Ϣ

	// ����
	SM_AUCTION_ITEM_LIST				= 11800,	// �·���������
	SM_AUCTION_SELF_ITEMS				= 11801,	// �·���������
	SM_AUCTION_RECORD					= 11802,	// �·����׼�¼
	SM_AUCTION_SELL_ITEM				= 11803,	// �·��ϼܳɹ���Ϣ

	// �
	SM_NOTIFY_ACTIVITY_INFO				= 11810,	// �·����Ϣ
	SM_NOTIFY_ACTIVITY_FAMILY_SCORE		= 11811,	// �·�����
	SM_NOTIFY_ACTIVITY_PLAYER_SCORE		= 11812,	// �·�����
	SM_NOTIFY_FAMILY_WAR_PILLAR_INFP	= 11813,	// �·�ս��֮����Ϣ
	SM_NOTIFY_ACTIVITY_SCORE			= 11814,	// �·�������Ϣ
	SM_NOTIFY_ACTIVITY_RESULT			= 11815,	// �·�����
	SM_NOTIFY_ACTIVITY_TERRITORY_INFO	= 11816,	// �·����ս��Ϣ
	SM_NOTIFY_FAMILY_WAR_WIN_FAMILY		= 11817,	// ����սʤ������
	// ����
	SM_SOUL_INFO						= 11900,	// �·�������Ϣ

	// ������
	SM_HALL_OF_FAME_INFO				= 11910,	// �����ý�����Ϣ
	SM_HALL_OF_FAME_BATTLE				= 11911,	// ������ս�����
	SM_HALL_OF_FAME_RANK_INFO			= 11912,	// ���������а�

	// 装备回购系统
	SM_EQUIP_BACK_BROADCAST			= 11920,	// 装备回购广播公告
	SM_SEND_EQUIP_BACK_INFO			= 11921,	// 装备回购完整信息
	SM_SEND_EQUIP_BACK_ON_RECORD	= 11922,	// 单条回购记录
	SM_SEND_EQUIP_BACK_COUNT_CHANGE	= 11923,	// 仓库库存变化
	SM_SEND_EQUIP_BACK_LIMIT_CHANGE	= 11924,	// 限制次数变化

	/*
	* SM_WXJ	11900~11999
	*/

	SM_MAX,

	SM_GM_MIN				= 19000,
	SM_GM_CHAT				= 19001,

	SM_PHP_MIN				= 19500,
	SM_PHP_BUY				= 19501,
	SM_PHP_MAX,
};

/*
* IM_MIN							= 20000,
* IM_SOCIAL_GLOBAL_MIN,
* IM_SOCIAL_GAME_MIN,
* IM_SOCIAL_SPECIFIC_GAME_MIN,
* IM_SOCIAL_ALL_GAME_MIN,
* IM_GAME_SOCIAL_MIN,
* IM_DB_GAME_SERVICE_CONNECT,
* IM_DB_LOG_ARTIFACT_ADD_EXP,
* IM_PHP_MIN						= 25000,
*/
enum MSG_IM_CODE
{
	IM_MIN = 20000,
	IM_CHARACTER_SERVICE_CONNECT,
	IM_GAME_SERVICE_CONNECT,
	IM_LOGIN_SERVICE_CONNECT,
	IM_SOCIAL_SERVICE_CONNECT,
	IM_GAME_GATE_USER_SAVED,
	IM_GATE_ADD_USER,
	IM_GATE_REMOVE_USER,
	IM_GATE_UPDATE_BAN_CHAT,
	IM_GATE_RELOAD_ITEM,
	IM_GAME_GATE_KICK_USER,
//================================================================
	IM_SOCIAL_GLOBAL_MIN,
	IM_SOCIAL_ADD_PLAYER_INFO,		//����
	IM_SOCIAL_UPDATE_PLAYER_INFO,
	IM_SOCIAL_ADD_PLAYER,
	IM_SOCIAL_REMOVE_PLAYER,
	IM_SOCIAL_REMOVE_FAMILY_APPLIER,		// ����
	IM_SOCIAL_DELETE_CHARACTER,				// ����
	IM_SOCIAL_NEW_MINUTE_COME,
	IM_SOCIAL_SEND_MAIL,
	IM_SOCIAL_FAMILY_WAR,
	IM_SOCIAL_ACTIVITY_CLEAR,
	IM_SOCIAL_ADD_KILLER,					// ����
	IM_SOCIAL_UPDATE_GAMBLE,				// ����
	IM_SOCIAL_FAMILY_ACTIVITY_PLAYER,		// ����
	IM_SOCIAL_WORLD_BOSS_ACTIVITY_RESULT,	// ����
	IM_SOCIAL_REAL_TIME_RANK_RESET,			// ����

	IM_SOCIAL_UPDATE_PET_DATA,		// ���³������а���Ϣ
	IM_SOCIAL_DELETE_PET_DATA,		// ���³������а���Ϣ

	IM_SOCIAL_REQUEST_PET_RANK_DATA,	// ��Ϸ������������а���Ϣ

	IM_SOCIAL_DA_TI_START_READY,		// ׼����ʼ����
	IM_SOCIAL_DA_TI_START,				// ������ʼ
	IM_SOCIAL_DA_TI_END,				// ��������
	IM_SOCIAL_QUESTIONS,				// ������Ŀ��Ϣ
	IM_SOCIAL_RAND_ANSWER,				// �����
	IM_SOCIAL_UPDATE_ZSZZ,				// ��������֮ս������Ϣ
	IM_SOCIAL_SYN_SERVER_DAY,			// �����ڼ���
	IM_SOCIAL_REQUEST_FAMILY_INFO,		// ����һ�¾�����Ϣ
	IM_SOCIAL_GLOBAL_MAX,

//===================================================
	IM_SOCIAL_GAME_MIN,
	//IM_SOCIAL_GAME_SYN_FAMILY_LEVEL,
	//IM_SOCIAL_GAME_CHANGE_FAMILY_INFO,
	IM_SOCIAL_GAME_SEND_MAIL,
	IM_SOCIAL_GAME_SEND_MAIL_INFO,
	IM_SOCIAL_GAME_GET_MAIL_POST,
	IM_SOCIAL_GAME_USE_EXCHANGE_CODE,
	//IM_SOCIAL_GAME_CHANGE_FAMILY_NAME,
	IM_SOCIAL_GAME_GOTO_ENEMY,							// ����
	//IM_SOCIAL_GAME_LEADER_DOWN,
	//IM_SOCIAL_GAME_UPDATE_LEADER_CID,
	//IM_SOCIAL_GAME_UPDATE_FAMILY_CONTRIBUTE,

	// ����
	IM_SOCIAL_GAME_CREATE_FAMILY,						// ��������
	IM_SOCIAL_GAME_UPDATE_FAMILY,						// ������Ұ�����Ϣ
	IM_SOCIAL_GAME_NEED_UPDATE_FAMILY,					// ������Ϣ����

	// ���
	IM_SOCIAL_GAME_UPDATE_TEAM_INFO,

	// ���
	IM_SOCIAL_GAME_WORSHIP,								// ���

	// XP
	IM_SOCIAL_GAME_UPDATE_XP,							// ͬ��XP��Ϣ
	IM_SOCIAL_GAME_ADD_XP,								// ���XP���������Ϣ

	// ��Ӹ���
	IM_SOCIAL_GAME_TEAM_DUNGEON_START,					// ��Ӹ�����ʼ
	IM_SOCIAL_GAME_TEAM_DUNGEON_UPDATE_INFO,			// ��Ӹ�����Ϣ����

	// ����
	IM_SOCIAL_GAME_AUCTION_SELL,						// ��Ʒ�ϼ�
	IM_SOCIAL_GAME_AUCTION_BUY,							// ������Ʒ
	IM_SOCIAL_GAME_ADD_CURRENCY,						// ���ӻ���

	// �
	IM_SOCIAL_GAME_FAMILYWAR_PILLAR_ADD_MONEY,			// ս��֮��ע������ʽ�

	// ������
	IM_SOCIAL_GAME_HALL_OF_FAME_FIGHT_RESULT,			// ��������ս���
	IM_SOCIAL_GAME_HALL_OF_FAME_GET_REWARD,				// ��������ȡ����
	IM_SOCIAL_GAME_HALL_OF_FAME_RANK_REWARD_INFO,		// ���������а�����Ϣ

	// ����
	IM_SOCIAL_GAME_UPDATE_FRIEND_LEVEL_UP,				//��������
	IM_SOCIAL_GAME_SEND_JUE_WEI_RANK,					//���;�λ���а�
	IM_SOCIAL_GAME_ALREADY,								//�����¼
	IM_SOCIAL_GAME_ADD_FRIEND,							//�ɾ����ͳ��
	//�齱
	IM_SOCIAL_GAME_ASK_CHOU_JIANG,						//����齱
	// ����ȫ��������Ʒ
	IM_SOCIAL_GAME_BUY_LIMIT_ITEM,						// ����ȫ��������Ʒ			
	IM_SOCIAL_GAME_MAX,
//=======================================================
	IM_SOCIAL_SPECIFIC_GAME_MIN,
	IM_SOCIAL_GAME_ADD_APPLIER,							// ����
	IM_SOCIAL_GAME_REAL_TIME_RANK_BASE,					// ����
	IM_SOCIAL_SPECIFIC_GAME_MAX,

	IM_SOCIAL_ALL_GAME_MIN,
	IM_SOCIAL_ALL_GAME_UPDATE_PET_RANK,					// ���³������а�
	IM_SOCIAL_ALL_GAME_UPDATE_FAMILY,					// ���°�����Ϣ
	IM_SOCIAL_ALL_GAME_CHANGE_OWNER,					// ���װ�����������
	IM_SOCIAL_ALL_GAME_KAI_FU_HUO_DAO_RANK,				// ͬ����������а�
	IM_SOCIAL_ALL_GAME_MAX,

	IM_SOCIAL_SINGLE_GAME_MIN,
	IM_SOCIAL_SINGLE_GAME_INIT_PET_RANK,				// ��ʼ���������а�
	IM_SOCIAL_SINGLE_GAME_TEAM_DUNGEON_CREATE,			// ������Ӹ���
	IM_SOCIAL_SINGLE_GAME_DA_TI_RANK,					// ���ʹ������а���Ϸ��
	IM_SOCIAL_SINGLE_GAME_SEND_SYSTEM_MAIL,				// ����ϵͳ�ʼ�
	IM_SOCIAL_SINGLE_GAME_SEND_SYSTEM_MAIL2,			// ����ϵͳ�ʼ�
	IM_SOCIAL_SINGLE_GAME_MAX,

	//����ڲ�
	IM_GAME_SOCIAL_MIN,
	IM_GAME_SOCIAL_CREATE_FAMILY,
	IM_GAME_SOCIAL_SEND_MAIL,
	IM_GAME_SOCIAL_GET_MAIL_POST,
	IM_GAME_SOCIAL_ADD_CONTRIBUTION,
	IM_GAME_SOCIAL_USE_EXCHANGE_CODE,
	IM_GAME_SOCIAL_CHANGE_NAME,
	IM_GAME_SOCIAL_CHANGE_FAMILY_NAME,
	IM_GAME_SOCIAL_GOTO_ENEMY,
	IM_GAME_SOCIAL_BREAK_FAMILY,
	IM_GAME_SOCIAL_UPDATE_ZHENLONGZHIQI,

	IM_GAME_SOCIAL_ADD_FAMILY_CONTRIBUTE,		// ���ž���
	IM_GAME_SOCIAL_UPDATE_PET_CONTRIBUTE,		// ͬ����Ҿ��Ź��׶�
	IM_GAME_SOCIAL_FAMILY_REGIST_PET,			// ���Ż��޵Ǽ�
	IM_GAME_SOCIAL_FAMILY_UNREGIST_PET,			// ���Ż���ȡ���Ǽ�
	IM_GAME_SOCIAL_FAMILY_TASK_COUTN,			// ���Ӿ���������ɴ���
	IM_GAME_SOCIAL_FAMILY_ADD_CONTRIBUTION,		// ���Ӿ��Ź���
	IM_GAME_SOCIAL_SET_TEAM_AUTO_OPERATE,		// ���Ĭ������
	IM_GAME_SOCIAL_LEAVE_TEAM,					// ǿ���뿪����

	IM_GAME_SOCIAL_WORSHIP,						// ���ӳ�ݼ�¼
	IM_GAME_SOCIAL_AUCTION_SELL,				// ����������
	IM_GAME_SOCIAL_AUCTION_BUY,					// �����й���
	IM_GAME_SOCIAL_STALL,						// ��̯

	IM_GAME_SOCIAL_ADD_XP,						// XPֵ����
	IM_GAME_SOCIAL_START_XP,					// ��ʼXP����
	IM_GAME_SOCIAL_END_XP,						// ����XP����
	IM_GAME_SOCIAL_BREAK_XP,					// ����XP����
	IM_GAME_SOCIAL_RESET_XP,					// ��������xp

	IM_GAME_SOCIAL_TEAM_DUNGEON_CREATE,			// ������Ӹ���
	IM_GAME_SOCIAL_TEAM_DUNGEON_JOIN,			// ������Ӹ���
	IM_GAME_SOCIAL_TEAM_DUNGEON_START,			// ������Ӹ���
	IM_GAME_SOCIAL_TEAM_DUNGEON_LEAVE,			// �뿪��Ӹ���

	IM_GAME_SOCIAL_NOTIFY_FAMILYWAR_INFO,		// �·�����ս��Ϣ
	IM_GAME_SOCIAL_FAMILYWAR_ADD_PILLAR_MONEY,	// ս��֮��ע���ʽ�

	IM_GAME_SOCIAL_HALL_OF_FAME_FIGHT,			// ��������ս
	IM_GAME_SOCIAL_HALL_OF_FAME_REQUEST_INFO,	// ���������������Ϣ
	IM_GAME_SOCIAL_HALL_OF_FAME_REWARD_RESULT,	// ��������ȡ��������

	IM_GAME_SOCIAL_ADD_ENEMY,					// ��ɱ���ӳ���
	IM_GAME_SOCIAL_JUE_WEI_DONATE_MONEY,		// ͬ����λ����
	IM_GAME_SOCIAL_ASK_JUE_WEI_RANK,			// �����λ���а�
	IM_GAME_SOCIAL_ASK_LIMIT_SHOP_INFO,			// ����ȫ��������Ʒ��Ϣ
	IM_GAME_SOCIAL_BUY_LIMIT_SHOP_ITEM,			// ����ȫ��������Ʒ
	IM_GAME_SOCIAL_BUY_LIMIT_SHOP_SUCCEED,		// ����ȫ��������Ʒ�ɹ�
	IM_GAME_SOCIAL_ADD_CHOU_JIANG_RECORD,		// ����ȫ���齱��¼
	IM_GAME_SOCIAL_CLEAN_GLOBAL_POINT,			// ����齱ȫ������ֵ
	IM_GAME_SOCIAL_CHECK_GLOBAL_POINT,			// ���ȫ������ֵ
	IM_GAME_SOCIAL_ASK_CHOU_JIANG_RECORD,		// �����ͳ齱��¼���ͻ���
	IM_GAME_SOCIAL_ADD_TOU_ZI_RECORD,			// ����Ͷ�ʼ�¼

	IM_GAME_SOCIAL_QUERY_PLAYER_INFO,
	IM_GAME_SOCIAL_UPDATE_ACTIVITY_STATE,
	IM_GAME_SYN_DAY_SWITCH,			//ͬ������
	IM_GAME_SOCIAL_MAX,
	//������
	

	IM_DB_GAME_SERVICE_CONNECT,
	IM_DB_LOAD_PLAYER,
	IM_DB_PLAYER_LOADED,
	IM_DB_SAVE_PLAYER,
	IM_DB_SAVE_COLTH,
	IM_DB_PLAYER_SAVED,
	IM_DB_INSERT_EQUIP,
	IM_DB_UPDATE_EQUIP,
	IM_DB_DELETE_EQUIP,
	IM_DB_EQUIP_UPDATED,
	IM_DB_UPDATE_FAMILY_WAR,		// ����
	IM_DB_FAMILY_WAR_UPDATED,		// ����
	IM_DB_INSERT_LACKEY,			// ����
	IM_DB_INSERT_ITEM,
	IM_DB_UPDATE_ITEM,
	IM_DB_DELETE_ITEM,
	IM_DB_ITEM_UPDATED,

	IM_DB_INSERT_PET,
	IM_DB_UPDATE_PET,
	IM_DB_DELETE_PET,
	IM_DB_PET_UPDATED,
	IM_DB_PET_DELETED,

	IM_DB_LOG_ARTIFACT_ADD_EXP,
	IM_DB_LOG_ARTIFACT_ADD_MAX_LEVEL,
	IM_DB_LOG_ARTIFACT_ADD_QUALITY,
	IM_DB_LOG_EQUIP_ADD_GEM,
	IM_DB_LOG_EQUIP_ADD_GEM_SLOT,
	IM_DB_LOG_EQUIP_APPEND_STAR,
	IM_DB_LOG_EQUIP_COMBINE,
	IM_DB_LOG_EQUIP_FORGE,
	IM_DB_LOG_EQUIP_LIFE,
	IM_DB_LOG_EQUIP_REFRESH_BIND,
	IM_DB_LOG_EQUIP_REFRESH_ELEMENT,
	IM_DB_LOG_EQUIP_REFRESH_FLOAT_ATTR,
	IM_DB_LOG_EQUIP_REMOVE_GEM,
	IM_DB_LOG_EQUIP_REMOVE_STAR,
	IM_DB_LOG_EQUIP_STRENGTHEN,
	IM_DB_LOG_EQUIP_UPGRADE,

	IM_DB_LOG_PLAYER_LOGIN,
	IM_DB_LOG_PLAYER_DEAL,
	IM_DB_USER_PAYED,
	IM_DB_LOG_TASK_RECEIVE,
	IM_DB_LOG_TASK_SUBMIT,
	IM_DB_LOG_EQUIP_ENHANCE,
	IM_DB_LOG_ITEM_COMBINE,
	IM_DB_LOG_EQUIP_SMITHING,// �ϳ�װ��.��ʯ
	IM_DB_LOG_EQUIP_MAGIC_SMITH,//����װ��
	IM_DB_LOG_EQUIP_DISMANTLE,
	IM_DB_LOG_IN_DUNGEON,
	IM_DB_LOG_OUT_DUNGEON,
	IM_DB_LOG_CREATE_FAMILY,
	IM_DB_LOG_JOIN_FAMILY,
	IM_DB_LOG_OUT_FAMILY,
	IM_DB_LOG_ACHIEVEMENT,
	IM_DB_LOG_PK,
	IM_DB_LOG_ACTIVITY_DATA,			// ���־
	IM_DB_LOG_ITEM_CHANGE,
	IM_DB_LOG_KILL_BOSS,				// BOSS��ɱ��־
	IM_DB_LOG_DAILY_GIFT,				// ÿ�������ȡ��־
	IM_GATE_QQ_TO_PLAYER,				// ��QQƽ̨�������Ĵ���player��
	IM_DB_INSERT_MEMYELLOWSTONE,		// ��¼������ȡ��¼
	IM_DB_LOG_VICEGENERAL_CHANGE,		// ������ļ���log
	IM_DB_LOG_CURRENCY,					// ������־
	IM_DB_ADD_RANSOM_INFO,				// ���������Ʒ
	IM_DB_DLE_RANSOM_INFO,				// ɾ�������Ʒ
	IM_DB_UPDATE_RANSOM,				// ���������Ϣ
	IM_DB_SEND_MAIL_TO_DB,				// �����ʼ���dbserver
	IM_DB_UPDATE_NEW_MAIL,				// �������ʼ���gameserver
	IM_DB_NEW_MINUTE,
	IM_DB_SAVE_FAMILY_WAR_RESULT,		// �洢����ս���
	IM_DB_UPDATE_FAMILY_WAR_RESULT,		// ���¾���ս�����������·
	IM_DB_SAVE_CITY_WAR_RESULT,			// 存储城战结果
	IM_DB_UPDATE_CITY_WAR_RESULT,		// 更新城战结果（来自DB回调）
	IM_DB_SAVE_TERRITORY_WAR_RESULT,	// �洢���ս���
	IM_DB_UPDATE_TERRITORY_WAR_RESULT,	// �������ս�����������·
	IM_DB_UPDATE_LOGIN_TIME,			// ���±�ǽ���ʱ��			
	IM_DB_UPDATE_GM_BROADCAST,			// ���¹���		
	IM_DB_UPDATE_GM_BAN_CHAT,			// ���½���	
	IM_DB_UPDATE_GM_SEAL,				// ���·��

	IM_PHP_MIN = 25000,
	IM_DB_USER_PAY = 25001,
	CM_GATE_PHP_PROP,

	IM_MAX,
};
