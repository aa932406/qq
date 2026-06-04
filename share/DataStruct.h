#ifndef __DATA_STRUCT_H__
#define __DATA_STRUCT_H__

#include "Shared.h"
#include "libanswer.h"
#include <vector>
#include <list>

typedef std::list<std::string> SqlStringList;

class IDataStruct
{
public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 ) = 0;
	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 ) = 0;
	virtual void PackageData( Answer::NetPacket* packet ) = 0;
	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 ) = 0;
};

#define LOAD_FROM_CHAR_BAG	"SELECT * FROM `mem_chr_bag` WHERE `cid`=%lld AND `bag`=%d"
#define SAVE_TO_CHAR_BAG	"INSERT INTO `mem_chr_bag` (`cid`, `bag`, `slot`, `id`, `class`, `count`, `bind`, `endtime`, `srcid`) VALUES (%lld, %d, %d, %d, %d, %d, %d, %d,%lld)"
#define DEL_FROM_CHAR_BAG	"DELETE FROM `mem_chr_bag` WHERE `cid` = %lld AND `bag` = %d"

//========================================================================================================================================
class MemCharacterData : public IDataStruct
{
public:
	MemCharacterData(){ CleanUp(); }
	virtual ~MemCharacterData(){}

	void CleanUp()
	{
		weapon	= 0;
		cloth	= 0;
		bzero( &data, sizeof( data ) );
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, 
			"UPDATE `mem_character` SET `job`=%d, `level`=%d,`exp`=%lld, `exp_minus`=%lld,`vip`=%d,`vip_end_time`=%d, `battle`=%d, `mapid`=%d, `x`=%d, `y`=%d, `hp`=%d, `mp` = %d,`pp`=%d,`xp`=%d,`level_gift`=%d,`level_gold`=%d,`seven_login`=%d,`pk_mode`=%d,`pk_value`=%d,`pk_killed_count`=%d,`pk_time`=%d, `kill_count`=%d, `bag_open_time`=%d,`bag_open_slot`=%d,`bag_can_open`=%d,`last_task_id`=%d,`logout_count`=%d, `pay_click_count`=%d,`level_stay_time` = %d, `last_login_time`=%d, `last_logout_time`=%d, `weapon`=%d, `cloth`=%d WHERE `cid`=%lld", 
			data.job, 
			data.level, 
			data.exp, 
			data.exp_minus, 
			data.vip,		
			data.vip_end_time,
			data.battle,
			data.mapid, 
			data.x, 
			data.y, 
			data.hp,
			data.mp, 
			data.pp,
			data.xp, 
			data.level_gift,
			data.level_gold,
			data.seven_login,
			data.pk_mode,
			data.pk_value, 
			data.pk_killed_count,
			data.pk_time, 
			data.kill_count, 
			data.bag_open_time,
			data.bag_open_slot,
			data.bag_can_open,
			data.last_task_id, 
			data.logout_count, 
			data.pay_click_count,
			data.level_stay_time, 
			data.last_login_time, 
			data.last_logout_time, 
			weapon,
			cloth,
			data.cid );

		sqls.push_back( szSQL );
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );

		snprintf( szSQL, sizeof(szSQL)-1, "SELECT * FROM `mem_character` WHERE `uid`=%d AND `sid`=%d AND `delete_time`=0", nUid, nSid );
		Answer::MySqlQuery result = db.query(szSQL);
		if (result.getRowCount() != 1)
		{
			return false;
		}

		data.cid					=  result.getInt64Value("cid");
		snprintf( data.name, MAX_NAME_CCH_LENGTH, result.getStringValue("name") );
		data.sex					= static_cast<Sex_t>( result.getIntValue("sex") );
		data.job					= static_cast<Job_t>( result.getIntValue("job") );
		data.level					= result.getIntValue("level");
		data.exp					= result.getInt64Value("exp");
		data.exp_minus				= result.getInt64Value("exp_minus");
		data.vip					= result.getIntValue("vip");
		data.vip_end_time			= result.getIntValue("vip_end_time");
		data.battle					= result.getIntValue("battle");
		data.mapid					= result.getIntValue("mapid");
		data.x						= result.getIntValue("x");
		data.y						= result.getIntValue("y");
		data.hp						= result.getIntValue("hp");
		data.mp						= result.getIntValue("mp");
		data.pp						= result.getIntValue("pp");
		data.xp						= result.getIntValue("xp");
		data.level_gift				= result.getIntValue("level_gift");
		data.level_gold				= result.getIntValue("level_gold");
		data.seven_login			= result.getIntValue("seven_login");
		data.head					= result.getIntValue("head");
		data.pk_mode				= result.getIntValue("pk_mode");
		data.pk_value				= result.getIntValue("pk_value");
		data.pk_killed_count		= result.getIntValue("pk_killed_count");
		data.pk_time				= result.getIntValue("pk_time");
		data.kill_count				= result.getIntValue("kill_count");
		data.bag_open_time			= result.getIntValue("bag_open_time");
		data.bag_open_slot			= result.getIntValue("bag_open_slot");
		data.bag_can_open			= result.getIntValue("bag_can_open");
		data.last_task_id			= result.getIntValue("last_task_id");
		data.logout_count			= result.getIntValue("logout_count");
		data.pay_click_count		= result.getIntValue("pay_click_count");
		data.level_stay_time		= result.getIntValue("level_stay_time");
		data.last_login_time		= result.getIntValue("last_login_time");
		data.last_logout_time		= result.getIntValue("last_logout_time");
		data.create_time			= result.getIntValue("create_time");
		
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		packet->writeInt64( data.cid );
		packet->writeUTF8( data.name );
		packet->writeUTF8( data.familyName );

		packet->writeInt32( data.leader );
		packet->writeInt8( data.sex );
		packet->writeInt8( data.job );
		packet->writeInt32( data.level );
		packet->writeInt32( data.battle );
		packet->writeInt64( data.exp );
		packet->writeInt64( data.exp_minus );
		packet->writeInt32( data.vip );
		packet->writeInt32( data.vip_end_time );
		packet->writeInt32( data.mapid );
		packet->writeInt32( data.x );
		packet->writeInt32( data.y );
		packet->writeInt32( data.hp );
		packet->writeInt32( data.mp );
		packet->writeInt32( data.pp );
		packet->writeInt32( data.xp );
		packet->writeInt32( data.level_gift );
		packet->writeInt32( data.level_gold );
		packet->writeInt32( data.seven_login );
		packet->writeInt32( data.head );
		packet->writeInt32( data.pk_mode );
		packet->writeInt32( data.pk_value );
		packet->writeInt32( data.pk_killed_count );
		packet->writeInt32( data.pk_time );
		packet->writeInt32( data.kill_count );
		packet->writeInt32( data.bag_open_time );
		packet->writeInt32( data.bag_open_slot );
		packet->writeInt32( data.bag_can_open );
		packet->writeInt32( data.last_task_id );
		packet->writeInt32( data.logout_count );
		packet->writeInt32( data.pay_click_count );
		packet->writeInt32( data.level_stay_time );
		packet->writeInt32( data.last_login_time );
		packet->writeInt32( data.last_logout_time );
		packet->writeInt32( data.create_time );
		packet->writeInt32( weapon );
		packet->writeInt32( cloth );
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		data.cid					= inPacket->readInt64();
		snprintf( data.name, MAX_NAME_CCH_LENGTH, inPacket->readUTF8(true).c_str());
		snprintf( data.familyName, MAX_NAME_CCH_LENGTH, inPacket->readUTF8(true).c_str());

		data.leader					= inPacket->readInt32();
		data.sex					= inPacket->readInt8();
		data.job					= inPacket->readInt8();
		data.level					= inPacket->readInt32();
		data.battle					= inPacket->readInt32();
		data.exp					= inPacket->readInt64();
		data.exp_minus				= inPacket->readInt64();
		data.vip					= inPacket->readInt32();
		data.vip_end_time			= inPacket->readInt32();
		data.mapid					= inPacket->readInt32();
		data.x						= inPacket->readInt32();
		data.y						= inPacket->readInt32();
		data.hp						= inPacket->readInt32();
		data.mp						= inPacket->readInt32();
		data.pp						= inPacket->readInt32();
		data.xp						= inPacket->readInt32();
		data.level_gift				= inPacket->readInt32();
		data.level_gold				= inPacket->readInt32();
		data.seven_login			= inPacket->readInt32();
		data.head					= inPacket->readInt32();
		data.pk_mode				= inPacket->readInt32();
		data.pk_value				= inPacket->readInt32();
		data.pk_killed_count		= inPacket->readInt32();
		data.pk_time				= inPacket->readInt32();
		data.kill_count				= inPacket->readInt32();
		data.bag_open_time			= inPacket->readInt32();
		data.bag_open_slot			= inPacket->readInt32();
		data.bag_can_open			= inPacket->readInt32();
		data.last_task_id			= inPacket->readInt32();
		data.logout_count			= inPacket->readInt32();
		data.pay_click_count		= inPacket->readInt32();
		data.level_stay_time		= inPacket->readInt32();
		data.last_login_time		= inPacket->readInt32();
		data.last_logout_time		= inPacket->readInt32();
		data.create_time			= inPacket->readInt32();
		weapon						= inPacket->readInt32();
		cloth						= inPacket->readInt32();
	}

public:
	MemCharacter	data;
	int32_t			weapon;
	int32_t			cloth;
};
//========================================================================================================================================

//========================================================================================================================================
class MemAttrData : public IDataStruct
{
public:
	MemAttrData(){ CleanUp(); }
	virtual ~MemAttrData(){}

	void CleanUp()
	{
		bzero( &data, sizeof( data ) );
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, 
			"INSERT INTO `mem_chr_attr` (`cid`,`hp`,`mp`,`pp`,`xp`,`phy_atk_min`,`phy_atk_max`,`mag_atk_min`,`mag_atk_max`,`phy_def`,`mag_def`,`hitrate`,`dodge`,`critrate`,`dmg_add`,`dmg_rdc`,`dmg_add_pec`,`dmg_rdc_pec`,`movespeed`,`battle`,`lucky`) VALUES (%lld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d) ON DUPLICATE KEY UPDATE `hp`=%d,`mp`=%d,`pp`=%d,`xp`=%d,`phy_atk_min`=%d,`phy_atk_max`=%d,`mag_atk_min`=%d,`mag_atk_max`=%d,`phy_def`=%d,`mag_def`=%d,`hitrate`=%d,`dodge`=%d,`critrate`=%d,`dmg_add`=%d,`dmg_rdc`=%d,`dmg_add_pec`=%d,`dmg_rdc_pec`=%d,`movespeed`=%d,`battle`=%d,`lucky`=%d",
			nCid,
			data.hp,
			data.mp,
			data.pp,
			data.xp,
			data.phy_atk_min,
			data.phy_atk_max,
			data.mag_atk_min,
			data.mag_atk_max,
			data.phy_def,
			data.mag_def,
			data.hitrate,
			data.dodge,
			data.critrate,
			data.dmg_add,
			data.dmg_rdc,
			data.dmg_add_pec,
			data.dmg_rdc_pec,
			data.movespeed,
			data.battle,
			data.lucky,
			data.hp,
			data.mp,
			data.pp,
			data.xp,
			data.phy_atk_min,
			data.phy_atk_max,
			data.mag_atk_min,
			data.mag_atk_max,
			data.phy_def,
			data.mag_def,
			data.hitrate,
			data.dodge,
			data.critrate,
			data.dmg_add,
			data.dmg_rdc,
			data.dmg_add_pec,
			data.dmg_rdc_pec,
			data.movespeed,
			data.battle,
			data.lucky ); 
		sqls.push_back( szSQL );
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof(szSQL)-1, "SELECT * FROM `mem_chr_attr` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query(szSQL);
		if ( !result.eof() )
		{
			data.hp				= result.getIntValue("hp");
			data.mp				= result.getIntValue("mp");
			data.pp				= result.getIntValue("pp");
			data.xp				= result.getIntValue("xp");
			data.phy_atk_min	= result.getIntValue("phy_atk_min");
			data.phy_atk_max	= result.getIntValue("phy_atk_max");
			data.mag_atk_min	= result.getIntValue("mag_atk_min");
			data.mag_atk_max	= result.getIntValue("mag_atk_max");
			data.phy_def		= result.getIntValue("phy_def");
			data.mag_def		= result.getIntValue("mag_def");
			data.hitrate		= result.getIntValue("hitrate");
			data.dodge			= result.getIntValue("dodge");
			data.critrate		= result.getIntValue("critrate");
			data.dmg_add		= result.getIntValue("dmg_add");
			data.dmg_rdc		= result.getIntValue("dmg_rdc");
			data.dmg_add_pec	= result.getIntValue("dmg_add_pec");
			data.dmg_rdc_pec	= result.getIntValue("dmg_rdc_pec");
			data.movespeed		= result.getIntValue("movespeed");
			data.battle			= result.getIntValue("battle");
			data.lucky			= result.getIntValue("lucky");

			return true;
		}

		return false;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		packet->writeInt32( data.hp );
		packet->writeInt32( data.mp );
		packet->writeInt32( data.pp );
		packet->writeInt32( data.xp );
		packet->writeInt32( data.phy_atk_min );
		packet->writeInt32( data.phy_atk_max );
		packet->writeInt32( data.mag_atk_min );
		packet->writeInt32( data.mag_atk_max );
		packet->writeInt32( data.phy_def );
		packet->writeInt32( data.mag_def );
		packet->writeInt32( data.hitrate );
		packet->writeInt32( data.dodge );
		packet->writeInt32( data.critrate );
		packet->writeInt32( data.dmg_add );
		packet->writeInt32( data.dmg_rdc );
		packet->writeInt32( data.dmg_add_pec );
		packet->writeInt32( data.dmg_rdc_pec );
		packet->writeInt32( data.movespeed );
		packet->writeInt32( data.battle );
		packet->writeInt32( data.lucky );
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		data.hp				= inPacket->readInt32();
		data.mp				= inPacket->readInt32();
		data.pp				= inPacket->readInt32();
		data.xp				= inPacket->readInt32();
		data.phy_atk_min	= inPacket->readInt32();
		data.phy_atk_max	= inPacket->readInt32();
		data.mag_atk_min	= inPacket->readInt32();
		data.mag_atk_max	= inPacket->readInt32();
		data.phy_def		= inPacket->readInt32();
		data.mag_def		= inPacket->readInt32();
		data.hitrate		= inPacket->readInt32();
		data.dodge			= inPacket->readInt32();
		data.critrate		= inPacket->readInt32();
		data.dmg_add		= inPacket->readInt32();
		data.dmg_rdc		= inPacket->readInt32();
		data.dmg_add_pec	= inPacket->readInt32();
		data.dmg_rdc_pec	= inPacket->readInt32();
		data.movespeed		= inPacket->readInt32();
		data.battle			= inPacket->readInt32();
		data.lucky			= inPacket->readInt32();
	}	

public:
	MemAttr	data; 
};
//========================================================================================================================================

//========================================================================================================================================
class SysUserData : public IDataStruct
{
public:
	SysUserData(){ CleanUp(); }
	virtual ~SysUserData(){}

	void CleanUp()
	{
		bzero( &data, sizeof( data ) );
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, 
			"UPDATE `sys_user` SET `gold`=%d,`sys_gold`=%d, `gold_cost_total` = %d,`cash`=%d, `last_login_time`=%d, `last_logout_time`=%d, `total_online_time`=%d, `total_offline_time`=%d,`total_login_count` = %d,`continue_login_count` = %d WHERE `uid`=%d AND `sid`=%d",
			data.gold,
			data.sys_gold, 
			data.gold_cost_total,
			data.cash, 
			data.last_login_time,
			data.last_logout_time,
			data.total_online_time,
			data.total_offline_time,
			data.total_login_count,
			data.continue_login_count,
			data.uid,
			data.sid );

		sqls.push_back( szSQL ); 

		if ( data.map_enter_time != 0 )
		{
			bzero( szSQL, sizeof( szSQL ) );
			snprintf( szSQL, sizeof( szSQL ) - 1,
				"UPDATE `sys_user` SET `map_enter_time`=%d WHERE `uid`=%d AND `sid`=%d", 
				data.map_enter_time, 
				data.uid, 
				data.sid );
			sqls.push_back( szSQL );
		}
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf(szSQL, sizeof(szSQL)-1, "SELECT * FROM `sys_user` WHERE `uid`=%d AND `sid`=%d", nUid, nSid);

		Answer::MySqlQuery result = db.query(szSQL);
		if (!result.eof())
		{
			int32_t gold			= result.getIntValue("gold");
			int32_t gold_pay		= result.getIntValue("gold_pay");
			int32_t gold_pay_total	= result.getIntValue("gold_pay_total");

			data.sys_gold				= result.getIntValue("sys_gold");
			data.uid						= nUid;
			data.sid						= nSid;
			data.gold					= gold_pay;
			data.gold_pay_total			= gold_pay_total + gold_pay;
			data.prepay_gold				= result.getIntValue("prepay_gold");
			data.gold_cost_total			= result.getIntValue("gold_cost_total");
			data.cash					= result.getIntValue("cash");
			data.map_enter_time			= result.getIntValue("map_enter_time");
			data.last_login_time			= result.getIntValue("last_login_time");
			data.adult					= result.getIntValue("adult");
			data.type					= result.getIntValue("type");
			data.last_logout_time		= result.getIntValue("last_logout_time");
			data.continue_login_count	= result.getIntValue("continue_login_count");
			data.total_online_time		= result.getIntValue("total_online_time");
			data.total_offline_time		= result.getIntValue("total_offline_time");

			snprintf( data.IP, MAX_IP_LENGTH, result.getStringValue("last_login_ip") );

			if (gold_pay > 0)
			{
				bzero( szSQL, sizeof(szSQL) );
				snprintf( szSQL, sizeof(szSQL)-1, "UPDATE `sys_user` SET `gold`=`gold`+%d, `gold_pay`=`gold_pay`-%d, `gold_pay_total`=`gold_pay_total`+%d WHERE `uid`=%d AND `sid`=%d", gold_pay, gold_pay, gold_pay, data.uid, data.sid );
				db.excute( szSQL );
			}

			return true;
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		packet->writeInt32( data.uid );
		packet->writeInt32( data.sid );
		packet->writeInt32( data.gold );
		packet->writeInt32( data.sys_gold );
		packet->writeInt32( data.gold_pay_total );
		packet->writeInt32( data.prepay_gold );
		packet->writeInt32( data.gold_cost_total );
		packet->writeInt32( data.cash );
		packet->writeInt32( data.map_enter_time );
		packet->writeInt32( data.last_login_time );
		packet->writeInt32( data.adult );
		packet->writeInt32( data.type );
		packet->writeInt32( data.last_logout_time );
		packet->writeInt32( data.continue_login_count );
		packet->writeInt32( data.total_online_time );
		packet->writeInt32( data.total_offline_time );
		packet->writeUTF8( data.IP );
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		data.uid						= inPacket->readInt32();
		data.sid						= inPacket->readInt32();
		data.gold					= inPacket->readInt32();
		data.sys_gold				= inPacket->readInt32();
		data.gold_pay_total			= inPacket->readInt32();
		data.prepay_gold				= inPacket->readInt32();
		data.gold_cost_total			= inPacket->readInt32();
		data.cash					= inPacket->readInt32();
		data.map_enter_time			= inPacket->readInt32();
		data.last_login_time			= inPacket->readInt32();
		data.adult					= inPacket->readInt32();
		data.type					= inPacket->readInt32();
		data.last_logout_time		= inPacket->readInt32();
		data.continue_login_count	= inPacket->readInt32();
		data.total_online_time		= inPacket->readInt32();
		data.total_offline_time		= inPacket->readInt32();
		snprintf( data.IP, MAX_IP_LENGTH, inPacket->readUTF8(true).c_str() );
	}

public:
	SysUser	data; 
};
//========================================================================================================================================

//========================================================================================================================================
class DailyActivityData : public IDataStruct
{
public:
	DailyActivityData(){ CleanUp(); }
	virtual ~DailyActivityData(){}

	void CleanUp()
	{
		bzero( &data, sizeof( data ) );
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero(szSQL, sizeof( szSQL ));
		snprintf( szSQL, sizeof( szSQL ) - 1, "DELETE FROM `mem_chr_search_back` WHERE `Cid`=%lld", nCid );
		sqls.push_back( szSQL ); 
		JoinedActivityRecord::iterator it = m_ActivityRecord.begin();
		for ( ; it != m_ActivityRecord.end(); ++it )
		{
			snprintf( szSQL, sizeof( szSQL ) - 1,
				"INSERT INTO `mem_chr_search_back` (`Cid`, `ActivityType`, `TodayRecord`, `YesterdayRecord`) VALUES (%lld, %d, '%s', '%s') ",
				nCid, 
				it->first,
				GetActivityRecordString( it->first, 1 ).c_str(),
				GetActivityRecordString( it->first, 2 ).c_str());
				sqls.push_back( szSQL ); 
		}
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1,
			"INSERT INTO `mem_chr_sign_info` (`cid`, `sign_record`, `sign_reward`, `refresh_time`) VALUES (%lld, %d, '%s', %d) ON DUPLICATE KEY UPDATE `sign_record` = %d, `sign_reward` = '%s', `refresh_time` = %d",
			nCid, 
			data.sign_record, 
			GetSignRewardString().c_str(), 
			data.refresh_time, 
			data.sign_record, 
			GetSignRewardString().c_str(), 
			data.refresh_time );
		sqls.push_back( szSQL ); 
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf(szSQL, sizeof(szSQL)-1, "SELECT * FROM `mem_chr_sign_info` WHERE `cid`=%lld", nCid);
		Answer::MySqlQuery result = db.query( szSQL );
		if ( result.getRowCount() == 1 )
		{
			data.sign_record	= result.getIntValue("sign_record");
			InitSignRewardInfo( result.getStringValue("sign_reward") );
			data.refresh_time	= result.getIntValue("refresh_time");
		}

		bzero( szSQL, sizeof( szSQL ) );
		snprintf(szSQL, sizeof(szSQL)-1, "SELECT * FROM `mem_chr_search_back` WHERE `cid`=%lld", nCid);
		Answer::MySqlQuery result_1 = db.query( szSQL );
		while ( !result_1.eof() )
		{
			int32_t ActivityType		= result_1.getIntValue("ActivityType");
			std::string TodayRecord		= result_1.getStringValue("TodayRecord");
			std::string YdayRecord		= result_1.getStringValue("YesterdayRecord");
			InitActivityRecordInfo( ActivityType, 1, TodayRecord );
			InitActivityRecordInfo( ActivityType, 2, YdayRecord );
			result_1.nextRow();
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		packet->writeInt32( data.sign_record );
		packet->writeUTF8( GetSignRewardString() );
		packet->writeInt32( data.refresh_time );
		packet->writeInt32( m_ActivityRecord.size() );
		JoinedActivityRecord::iterator it = m_ActivityRecord.begin();
		for ( ; it != m_ActivityRecord.end(); ++it )
		{
			packet->writeInt32( it->first );
			packet->writeInt32( it->second.JoinedIdSet.size() );
			JoinedActivityId::iterator TodayIt = it->second.JoinedIdSet.begin();
			for ( ; TodayIt != it->second.JoinedIdSet.end(); ++TodayIt )
			{
				packet->writeInt32(*TodayIt);
			}
			packet->writeInt32( it->second.YesterdayRecord.size() );
			JoinedActivityId::iterator YdayIt = it->second.YesterdayRecord.begin();
			for ( ; YdayIt != it->second.YesterdayRecord.end(); ++ YdayIt )
			{
				packet->writeInt32(*YdayIt);
			}

		}
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		data.sign_record	= inPacket->readInt32();
		InitSignRewardInfo( inPacket->readUTF8(true) );
		data.refresh_time	= inPacket->readInt32();
		int32_t nSize		= inPacket->readInt32();
		for ( int32_t i = 0; i < nSize; i++ )
		{		
			JoinedActivityId TodaySet;
			JoinedActivityId YdaySet;
			int32_t ActivityType = inPacket->readInt32(); 
			int32_t TodaySize    = inPacket->readInt32(); 
			for ( int32_t j = 0; j < TodaySize; j++ )
			{
				TodaySet.insert( inPacket->readInt32() );
			}
			m_ActivityRecord[ActivityType].JoinedIdSet = TodaySet;

			int32_t YdaySize	 = inPacket->readInt32();
			for ( int32_t k = 0 ; k < YdaySize; k++ )
			{
				YdaySet.insert( inPacket->readInt32() );
			}
			m_ActivityRecord[ActivityType].YesterdayRecord = YdaySet;
		}
	}

	std::string GetSignRewardString()
	{
		std::stringstream ss;
		std::vector<int8_t>::iterator  it = data.sign_reward.begin();
		for ( ; it != data.sign_reward.end(); ++it )
		{
			ss	<< (int32_t)*it
				<< "|" ;
		}
		return ss.str();
	}

	void InitSignRewardInfo( std::string RewardString )
	{
		if ( RewardString == "" )
		{
			return;
		}
		StringVector RewardVector = Answer::StringUtility::split( RewardString, "|");
		StringVector::iterator it = RewardVector.begin();
		for ( ; it != RewardVector.end(); ++it )
		{
			data.sign_reward.push_back((int8_t)atoi((*it).c_str()));
		}
	}
	std::string GetActivityRecordString( int32_t ActivityType, int8_t flag )
	{
		std::stringstream ss;
		ss<< "|";
		JoinedActivityRecord::iterator it =	m_ActivityRecord.find( ActivityType );
		if ( it == m_ActivityRecord.end() )
		{
			return ss.str();
		}
		if ( flag == 1 )
		{
			JoinedActivityId::iterator Setit = it->second.JoinedIdSet.begin();
			for ( ; Setit != it->second.JoinedIdSet.end(); ++Setit )
			{
				ss	<< *Setit
					<< "|" ;
			}
		}
		else
		{
			JoinedActivityId::iterator Setit = it->second.YesterdayRecord.begin();
			for ( ; Setit != it->second.YesterdayRecord.end(); ++Setit )
			{
				ss	<< *Setit
					<< "|" ;
			}
		}
	
		return ss.str();
	}
	
	void InitActivityRecordInfo( int32_t ActivityType, int8_t flag, std::string RewardString )
	{
		if ( RewardString == "" )
		{
			return;
		}
		StringVector RewardVector = Answer::StringUtility::split( RewardString, "|");
		StringVector::iterator it = RewardVector.begin();
		JoinedActivityId Set;
		for ( ; it != RewardVector.end(); ++it )
		{
			Set.insert( atoi((*it).c_str()) );
		}
		if ( flag == 1 )
		{
			m_ActivityRecord[ActivityType].JoinedIdSet		= Set; 
		}
		else
		{	
			m_ActivityRecord[ActivityType].YesterdayRecord	= Set;
		}
	}

public:
	MemChrSignInfo			data;
	JoinedActivityRecord	m_ActivityRecord;
};
//========================================================================================================================================

//========================================================================================================================================
class MemChrEquipData : public IDataStruct
{
public:
	MemChrEquipData(){ CleanUp(); }
	virtual ~MemChrEquipData(){}

	void CleanUp()
	{
		bzero( charEquip, sizeof( charEquip ) );
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, DEL_FROM_CHAR_BAG, nCid, CBT_EQUIP );
		sqls.push_back( szSQL );

		for ( int i = 0; i < EQUIP_SLOT_TOTAL_NUM; ++i )
		{
			bzero( szSQL, sizeof( szSQL ) );
			if ( charEquip[i].itemId != 0 )
			{
				snprintf( szSQL, sizeof( szSQL ) - 1,
					SAVE_TO_CHAR_BAG, 
					nCid,
					CBT_EQUIP,
					i,
					charEquip[i].itemId,
					charEquip[i].itemClass,
					charEquip[i].itemCount,
					charEquip[i].bind,
					charEquip[i].endTime,
					charEquip[i].srcId );
				sqls.push_back(szSQL);
			}
		}
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, LOAD_FROM_CHAR_BAG, nCid, CBT_EQUIP );
		Answer::MySqlQuery result = db.query( szSQL );
		while ( !result.eof() )
		{
			MemChrBag equip = {};
			int32_t slot	= result.getIntValue("slot");
			equip.itemId	= result.getIntValue("id");
			equip.itemClass	= static_cast<int8_t>( result.getIntValue("class") );
			equip.itemCount	= result.getIntValue("count");
			equip.bind		= static_cast<int8_t>( result.getIntValue("bind") );
			equip.endTime	= result.getIntValue("endtime");
			equip.srcId		= result.getInt64Value("srcid");
			if ( slot >= 0 && slot < EQUIP_SLOT_TOTAL_NUM )
			{
				charEquip[slot] = equip;
			}

			result.nextRow();
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		for ( int32_t i = 0; i < EQUIP_SLOT_TOTAL_NUM; ++i )
		{
			packet->writeInt32( charEquip[i].itemId );
			packet->writeInt8( charEquip[i].itemClass );
			packet->writeInt32( charEquip[i].itemCount );
			packet->writeInt8( charEquip[i].bind );
			packet->writeInt32( charEquip[i].endTime );
			packet->writeInt64( charEquip[i].srcId );
		}
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		for (int32_t i = 0; i < EQUIP_SLOT_TOTAL_NUM; ++i)
		{
			MemChrBag equip = {};
			equip.itemId	= inPacket->readInt32();
			equip.itemClass	= inPacket->readInt8();
			equip.itemCount	= inPacket->readInt32();
			equip.bind		= inPacket->readInt8();
			equip.endTime	= inPacket->readInt32();
			equip.srcId		= inPacket->readInt64();
			charEquip[i] = equip;
		}
	}	

public:
	MemChrBag	charEquip[EQUIP_SLOT_TOTAL_NUM];
};
//========================================================================================================================================

//========================================================================================================================================
class MemChrGemData : public IDataStruct
{
public:
	MemChrGemData(){ CleanUp(); }
	virtual ~MemChrGemData(){}

	void CleanUp()
	{
		bzero( data, sizeof( data ) );
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "DELETE FROM `mem_chr_gem` WHERE `cid`=%lld", nCid );
		sqls.push_back( szSQL );

		for ( int32_t pos = 0; pos < EQUIP_SLOT_TOTAL_NUM; ++pos )
		{
			for ( int32_t slot = 0; slot < GEM_SLOT_NUM; ++slot )
			{
				bzero( szSQL, sizeof( szSQL ) );
				if ( data[pos][slot] > 0 )
				{
					snprintf( szSQL, sizeof( szSQL ) - 1,
						"INSERT INTO `mem_chr_gem` (`cid`, `pos`, `slot`, `gemid`) VALUES (%lld, %d, %d, %d)", 
						nCid,
						pos,
						slot,
						data[pos][slot] );
					sqls.push_back(szSQL);
				}
			}
		}
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_gem` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		while ( !result.eof() )
		{
			int32_t pos		= result.getIntValue("pos");
			int32_t slot	= result.getIntValue("slot");
			if ( pos >= 0 && pos < EQUIP_SLOT_TOTAL_NUM && slot >= 0 && slot < GEM_SLOT_NUM )
			{
				data[pos][slot] = result.getIntValue("gemid");
			}

			result.nextRow();
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		for ( int32_t pos = 0; pos < EQUIP_SLOT_TOTAL_NUM; ++pos )
		{
			for ( int32_t slot = 0; slot < GEM_SLOT_NUM; ++slot )
			{
				packet->writeInt32( data[pos][slot] );
			}
		}
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		for ( int32_t pos = 0; pos < EQUIP_SLOT_TOTAL_NUM; ++pos )
		{
			for ( int32_t slot = 0; slot < GEM_SLOT_NUM; ++slot )
			{
				data[pos][slot]	= inPacket->readInt32();
			}
		}
	}	

public:
	int32_t	data[EQUIP_SLOT_TOTAL_NUM][GEM_SLOT_NUM];
};
//========================================================================================================================================
class TouZiData : public IDataStruct
{
public:
	TouZiData(){ CleanUp(); }
	virtual ~TouZiData(){}

	void CleanUp()
	{
		m_SevenDayTouZiTime		= 0;
		m_SevenDayRecord		= 0;
		m_MonthTouZiTime		= 0;
		m_MonthTouZiRecord		= 0;
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1,
			"INSERT INTO `mem_chr_tou_zi` (`cid`, `MoonCardStartTime`, `TouZiValues`, `MoonCardRecord`,`LevelUpRecord`) VALUES (%lld, %d, %d, %d, %d) ON DUPLICATE KEY UPDATE `MoonCardStartTime` = %d, `TouZiValues` = %d, `MoonCardRecord` = %d, `LevelUpRecord`=%d",
			nCid, 
			m_SevenDayTouZiTime, 
			m_MonthTouZiTime, 
			m_SevenDayRecord,
			m_MonthTouZiRecord,
			m_SevenDayTouZiTime, 
			m_MonthTouZiTime, 
			m_SevenDayRecord,
			m_MonthTouZiRecord
			 );
		sqls.push_back( szSQL ); 
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_tou_zi` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		while ( !result.eof() )
		{
			m_SevenDayTouZiTime	= result.getIntValue("MoonCardStartTime");
			m_MonthTouZiTime	= result.getIntValue("TouZiValues");
			m_SevenDayRecord	= result.getIntValue("MoonCardRecord");
			m_MonthTouZiRecord	= result.getIntValue("LevelUpRecord");
			result.nextRow();
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		packet->writeInt32( m_SevenDayTouZiTime );
		packet->writeInt32( m_SevenDayRecord );
		packet->writeInt32( m_MonthTouZiTime );
		packet->writeInt32( m_MonthTouZiRecord );
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		m_SevenDayTouZiTime	= inPacket->readInt32();
		m_SevenDayRecord	= inPacket->readInt32();
		m_MonthTouZiTime	= inPacket->readInt32();
		m_MonthTouZiRecord	= inPacket->readInt32();
	}

public:
	int32_t					m_SevenDayTouZiTime;	// 7��Ͷ��ʱ��
	int32_t					m_SevenDayRecord;		// 7����ȡ��¼(λͼ)
	int32_t					m_MonthTouZiTime;		// �¶�Ͷ��ʱ��
	int32_t					m_MonthTouZiRecord;		// �¶���ȡ��¼(λͼ)
};
//========================================================================================================================================
class MemChrBagData : public IDataStruct
{
public:
	MemChrBagData(){ CleanUp(); }
	virtual ~MemChrBagData(){}

	void CleanUp()
	{
		bzero( bagData, sizeof( bagData ) );
		m_ItemLimit.clear();
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, DEL_FROM_CHAR_BAG, nCid, CBT_BAG );
		sqls.push_back( szSQL );

		for ( int slot = 0; slot < MAX_BAG_SLOT_NUM; ++slot )
		{
			bzero( szSQL, sizeof( szSQL ) );
			if ( bagData[slot].itemCount > 0 )
			{
				snprintf( szSQL, sizeof( szSQL ) - 1,
					SAVE_TO_CHAR_BAG,
					nCid, 
					CBT_BAG, 
					slot,
					bagData[slot].itemId, 
					bagData[slot].itemClass,
					bagData[slot].itemCount, 
					bagData[slot].bind,
					bagData[slot].endTime,
					bagData[slot].srcId );
				sqls.push_back( szSQL );
			}
		}
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "DELETE FROM mem_chr_shop_limit WHERE Cid = %lld AND ShopType=%d", nCid, ST_CHAR_SHOP );
		sqls.push_back( szSQL );
		ItemLimitMap::iterator it = m_ItemLimit.begin();
		for ( ; it != m_ItemLimit.end(); ++it )
		{
			bzero( szSQL, sizeof( szSQL ) );
			snprintf( szSQL, sizeof( szSQL ) - 1,
				"INSERT INTO `mem_chr_shop_limit` ( Cid,ShopType, ShopId, LimitCount ) VALUES( %lld,%d, %d, %d)",
				nCid,
				ST_CHAR_SHOP,
				it->first,
				it->second
				);
			sqls.push_back( szSQL );
		}
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, LOAD_FROM_CHAR_BAG, nCid, CBT_BAG );
		Answer::MySqlQuery result = db.query( szSQL );
		while (!result.eof())
		{
			MemChrBag slotData = {};
			int32_t slot		= result.getIntValue("slot");
			slotData.itemId		= result.getIntValue("id");
			slotData.itemClass	= static_cast<int8_t>( result.getIntValue("class") );
			slotData.itemCount	= result.getIntValue("count");
			slotData.bind		= static_cast<int8_t>( result.getIntValue("bind") );
			slotData.endTime	= result.getIntValue("endtime");
			slotData.srcId		= result.getInt64Value("srcid");

			if ( slot < 0 || slot >= MAX_BAG_SLOT_NUM )
			{
				LOG_ERROR("wrong slot data bag = %d, slot = %d, cid = %lld\n", CBT_BAG, slot, nCid );
			}
			else
			{
				bagData[slot] = slotData;
			}
			result.nextRow();
		}

		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_shop_limit` WHERE `Cid`=%lld AND ShopType=%d", nCid,ST_CHAR_SHOP );
		Answer::MySqlQuery result_1 = db.query( szSQL );
		while(!result_1.eof())
		{
			KillMonsterInfo stu;
			bzero( &stu, sizeof( stu ) );
			int32_t ShopId		 = result_1.getIntValue("ShopId");
			int32_t LimitCount	 = result_1.getIntValue("LimitCount");
			m_ItemLimit[ShopId] = LimitCount;
			result_1.nextRow();
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		uint32_t nCount = packet->getWOffset();
		int32_t slotCount = 0;
		packet->writeInt32( slotCount );
		for ( int32_t slot = 0; slot < MAX_BAG_SLOT_NUM; ++slot )
		{
			if ( bagData[slot].itemCount > 0 )
			{
				packet->writeInt32( slot );
				packet->writeInt32( bagData[slot].itemId );
				packet->writeInt8( bagData[slot].itemClass );
				packet->writeInt32( bagData[slot].itemCount );
				packet->writeInt8( bagData[slot].bind );
				packet->writeInt32( bagData[slot].endTime );
				packet->writeInt64( bagData[slot].srcId );
				++slotCount;
			}
		}
		*( (int32_t*)( packet->getBuffer() + nCount ) ) = slotCount;

		int32_t nSize = m_ItemLimit.size();
		packet->writeInt32( nSize );
		ItemLimitMap::iterator it = m_ItemLimit.begin();
		for ( ; it != m_ItemLimit.end(); ++it )
		{
			packet->writeInt32( it->first );
			packet->writeInt32( it->second );
		}
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		int32_t slotCount = inPacket->readInt32();
		for ( int32_t i = 0; i < slotCount; ++i )
		{
			MemChrBag slotData = {};

			int32_t slot		= inPacket->readInt32();
			slotData.itemId		= inPacket->readInt32();
			slotData.itemClass	= inPacket->readInt8();
			slotData.itemCount	= inPacket->readInt32();
			slotData.bind		= inPacket->readInt8();
			slotData.endTime	= inPacket->readInt32();
			slotData.srcId		= inPacket->readInt64();

			if ( slot < 0 || slot >= MAX_BAG_SLOT_NUM )
			{
				LOG_ERROR( "wrong bag slotData data bag = %d, slot = %d, cid = %lld\n", CBT_BAG, slot, nCid );
			}
			else
			{
				bagData[slot] = slotData;
			}
		}
		m_ItemLimit.clear();
		int32_t nSize			= inPacket->readInt32();
		for ( int32_t i = 0; i < nSize; ++i )
		{
			int32_t ShopId		= inPacket->readInt32();
			int32_t LimitCount  = inPacket->readInt32();
			m_ItemLimit[ShopId]  = LimitCount;
		}
	}

public:
	MemChrBag	bagData[MAX_BAG_SLOT_NUM];
	ItemLimitMap m_ItemLimit;
};
//========================================================================================================================================

//========================================================================================================================================
class MemChrSkillData : public IDataStruct
{	
public:
	MemChrSkillData(){ CleanUp(); }
	virtual ~MemChrSkillData(){}

	void CleanUp()
	{
		skillVt.clear();
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "DELETE FROM `mem_chr_skill` WHERE `cid`=%lld", nCid );
		sqls.push_back( szSQL );

		int32_t vtSize = skillVt.size();
		for ( int32_t i = 0; i < vtSize; ++i )
		{
			bzero( szSQL, sizeof( szSQL ) );
			snprintf( szSQL, sizeof( szSQL ) - 1,
				"INSERT INTO `mem_chr_skill` (`cid`, `sid`, `level`) VALUES (%lld, %d, %d)",
				nCid,
				skillVt[i].id,
				skillVt[i].level );
			sqls.push_back( szSQL );
		}
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_skill` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query(szSQL);
		skillVt.reserve( result.getRowCount() );
		while (!result.eof())
		{
			MemChrSkill skill = {};
			skill.id	= result.getIntValue("sid");
			skill.level = result.getIntValue("level");
			skillVt.push_back( skill );
			result.nextRow();
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		int32_t vtSize = skillVt.size();
		packet->writeInt32( vtSize );
		for ( int32_t i = 0; i < vtSize; ++i )
		{
			packet->writeInt32( skillVt[i].id );
			packet->writeInt32( skillVt[i].level );
		}
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		int32_t skillCount = inPacket->readInt32();
		skillVt.reserve( skillCount );
		for ( int32_t i = 0; i < skillCount; ++i )
		{
			MemChrSkill skill = {};

			skill.id	= inPacket->readInt32();
			skill.level = inPacket->readInt32();

			skillVt.push_back( skill );
		}
	}

public:
	MemChrSkillVector	skillVt;
};
//========================================================================================================================================

//========================================================================================================================================
class MemChrTaskData : public IDataStruct
{
public:
	MemChrTaskData(){ CleanUp(); }
	virtual ~MemChrTaskData(){}

	void CleanUp()
	{
		taskVt.clear();
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "DELETE FROM `mem_chr_task` WHERE `cid`=%lld", nCid );
		sqls.push_back( szSQL );

		int32_t vtSize = taskVt.size();
		for ( int32_t i = 0; i < vtSize; ++i )
		{
			bzero( szSQL, sizeof( szSQL ) );
			snprintf( szSQL, sizeof( szSQL ) - 1, 
				"INSERT INTO `mem_chr_task` (`cid`, `tid`, `state`, `monster`) VALUES (%lld, %d, %d, %d)",
				nCid,
				taskVt[i].id,
				taskVt[i].state,
				taskVt[i].monster );
			sqls.push_back( szSQL );
		}
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_task` WHERE `cid`=%lld\n", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		taskVt.reserve( result.getRowCount() );
		while ( !result.eof() )
		{
			MemChrTask task = {};
			task.id			= result.getIntValue("tid");
			task.state		= result.getIntValue("state");
			task.monster	= result.getIntValue("monster");
			taskVt.push_back(task);
			result.nextRow();
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		int32_t vtSize = taskVt.size();
		packet->writeInt32( vtSize );
		for ( int32_t i = 0; i < vtSize; ++i )
		{
			packet->writeInt32( taskVt[i].id );
			packet->writeInt32( taskVt[i].state );
			packet->writeInt32( taskVt[i].monster );
		}
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		int32_t taskCount = inPacket->readInt32();
		taskVt.reserve(taskCount);
		for ( int32_t i = 0; i < taskCount; ++i )
		{
			MemChrTask task = {};

			task.id			= inPacket->readInt32();
			task.state		= inPacket->readInt32();
			task.monster	= inPacket->readInt32();

			taskVt.push_back(task);
		}

	}

public:
	MemChrTaskVector	taskVt;
};
//========================================================================================================================================

//========================================================================================================================================
class MemChrTaskCycleData : public IDataStruct
{
public:
	MemChrTaskCycleData(){ CleanUp(); }
	virtual ~MemChrTaskCycleData(){}

	void CleanUp()
	{
		nFinishTimes		= 0;
		nTaskId				= 0;
		nStar				= 0;
		nState				= 0;
		nKills				= 0;
		nRefreshStarTimes	= 0;
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL )-1, 
			"INSERT INTO `mem_cycle_task` (`cid`,`finishtimes`, `taskid`, `star`, `state`, `conds`, `refstartimes`) VALUES (%lld,%d,%d,%d,%d,%d,%d) ON DUPLICATE KEY UPDATE `finishtimes`=%d, `taskid`=%d, `star`=%d, `state`=%d, `conds`=%d, `refstartimes`=%d ",
			nCid,
			nFinishTimes, 
			nTaskId, 
			nStar,
			nState,
			nKills,
			nRefreshStarTimes,
			nFinishTimes, 
			nTaskId, 
			nStar,
			nState,
			nKills,
			nRefreshStarTimes
			);
		sqls.push_back(szSQL);
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_cycle_task` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		if ( result.getRowCount() == 1 )
		{
			nFinishTimes		= result.getIntValue("finishtimes");
			nTaskId				= result.getIntValue("taskid");
			nStar				= result.getIntValue("star");
			nState				= result.getIntValue("state");
			nKills				= result.getIntValue("conds");
			nRefreshStarTimes	= result.getIntValue("refstartimes");
			return true;
		}
		return false;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		packet->writeInt32( nFinishTimes );
		packet->writeInt32( nTaskId );
		packet->writeInt8( nStar );
		packet->writeInt8( nState );
		packet->writeInt32( nKills );
		packet->writeInt32( nRefreshStarTimes );
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		nFinishTimes		= inPacket->readInt32();
		nTaskId				= inPacket->readInt32();
		nStar				= inPacket->readInt8();
		nState				= inPacket->readInt8();
		nKills				= inPacket->readInt32();
		nRefreshStarTimes	= inPacket->readInt32();
	}

public:
	int32_t	nFinishTimes;			// ��ǰ��ɼ�����
	int32_t	nTaskId;				// ��ǰ����ID
	int8_t	nStar;					// ��ǰ�Ǽ�
	int8_t	nState;					// ��ǰ����״̬ 2 �ɽ� 3 �ѽ� 4 ���ύ
	int32_t	nKills;					// �Ѿ�ɱ�˼�����
	int32_t	nRefreshStarTimes;		// ˢ�Ǵ���
};
//========================================================================================================================================

//========================================================================================================================================
class MemChrActionData : public IDataStruct
{
public:
	MemChrActionData(){ CleanUp(); }
	virtual ~MemChrActionData(){}

	void CleanUp()
	{
		bzero( actionArry, sizeof( actionArry ) );
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "DELETE FROM `mem_chr_action` WHERE `cid`=%lld", nCid );
		sqls.push_back( szSQL );

		for (int i = 1; i < ACTION_NUMBER; ++i)
		{
			if ( actionArry[i].id != 0 )
			{
				bzero( szSQL, sizeof( szSQL ) );
				snprintf( szSQL, sizeof( szSQL ) - 1, 
					"INSERT INTO `mem_chr_action` (`cid`, `slot`, `id`, `type`) VALUES (%lld, %d, %d, %d)",
					nCid,
					i,
					actionArry[i].id,
					actionArry[i].type );
				sqls.push_back( szSQL );
			}
		}
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_action` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		while ( !result.eof() )
		{
			MemChrAction action = {};
			int32_t slot	= result.getIntValue("slot");
			action.id		= result.getIntValue("id");
			action.type		= result.getIntValue("type");
			if ( slot > 0 && slot < ACTION_NUMBER )
			{
				actionArry[slot] = action;
			}

			result.nextRow();
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		for ( int32_t i = 1; i < ACTION_NUMBER; ++i )
		{
			packet->writeInt32( actionArry[i].id );
			packet->writeInt32( actionArry[i].type );
		}
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		for ( int32_t i = 1; i < ACTION_NUMBER; ++i )
		{
			MemChrAction action = {};

			action.id = inPacket->readInt32();
			action.type = inPacket->readInt32();

			actionArry[i] = action;
		}

	}

public:
	MemChrAction	actionArry[ACTION_NUMBER];
};
//========================================================================================================================================

//========================================================================================================================================
class MemChrAutoFightData : public IDataStruct
{
public:
	MemChrAutoFightData(){ CleanUp(); }
	virtual ~MemChrAutoFightData(){}

	void CleanUp()
	{
		bzero( &data, sizeof( data ) );
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, 
			"INSERT INTO `mem_chr_auto_fight` (`cid`, `fight`, `pick`) VALUES (%lld, '%s', '%s') ON DUPLICATE KEY UPDATE `fight`='%s', `pick`='%s'",
			nCid,
			data.fight,
			data.pick,
			data.fight,
			data.pick );
		sqls.push_back(szSQL);
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_auto_fight` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query(szSQL);
		if (!result.eof())
		{
			snprintf( data.fight, MAX_AUTO_FIGHT_CCH_LENGTH, result.getStringValue("fight") );
			snprintf( data.pick, MAX_AUTO_PICK_CCH_LENGTH, result.getStringValue("pick") );
			return true;
		}
		return false;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		packet->writeUTF8( data.fight );
		packet->writeUTF8( data.pick );
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		snprintf( data.fight, MAX_AUTO_FIGHT_CCH_LENGTH, inPacket->readUTF8(true).c_str() );
		snprintf( data.pick, MAX_AUTO_PICK_CCH_LENGTH, inPacket->readUTF8(true).c_str() );

	}

public:
	MemChrAutoFight	data;
};
//========================================================================================================================================

//========================================================================================================================================
class MemChrSystemSettingData : public IDataStruct
{
public:
	MemChrSystemSettingData(){ CleanUp(); }
	virtual ~MemChrSystemSettingData(){}

	void CleanUp()
	{
		data="";
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1,
			"INSERT INTO `mem_chr_system_setting` (`cid`, `setting`) VALUES (%lld, '%s') ON DUPLICATE KEY UPDATE `setting`='%s'",
			nCid,
			data.c_str(),
			data.c_str() );
		sqls.push_back( szSQL );
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_system_setting` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		if ( !result.eof() )
		{

			data = result.getStringValue("setting");
			
			return true;
		}
		return false;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		packet->writeUTF8( data );
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		data		= inPacket->readUTF8(true);
	}

public:
	std::string	data;
};
//========================================================================================================================================

//========================================================================================================================================
class MemChrBuffData : public IDataStruct
{
public:
	MemChrBuffData(){ CleanUp(); }
	virtual ~MemChrBuffData(){}

	void CleanUp()
	{
		buffVt.clear();
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "DELETE FROM `mem_chr_buff` WHERE `cid`=%lld", nCid );
		sqls.push_back(szSQL);
		int32_t vtSize = buffVt.size();
		for ( int32_t i = 0; i < vtSize; ++i )
		{
			bzero( szSQL, sizeof( szSQL ) );
			snprintf( szSQL, sizeof( szSQL ) - 1, 
				"INSERT INTO `mem_chr_buff` (`cid`, `item`, `time`) VALUES (%lld, %d, %d)",
				nCid, 
				buffVt[i].id, 
				buffVt[i].time );
			sqls.push_back( szSQL );
		}
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_buff` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		buffVt.reserve( result.getRowCount() );
		while ( !result.eof() )
		{
			MemChrBuff buff = {};
			buff.id		= result.getIntValue("item");
			buff.time	= result.getIntValue("time");
			buffVt.push_back( buff );
			result.nextRow();
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		int32_t vtSize = buffVt.size();
		packet->writeInt32( vtSize );
		for ( int32_t i = 0; i < vtSize; ++i )
		{
			packet->writeInt32( buffVt[i].id );
			packet->writeInt32( buffVt[i].time );
		}
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		int32_t buffCount = inPacket->readInt32();
		buffVt.reserve(buffCount);
		for ( int32_t i = 0; i < buffCount; ++i )
		{
			MemChrBuff buff = {};

			buff.id		= inPacket->readInt32();
			buff.time	= inPacket->readInt32();

			buffVt.push_back( buff );
		}
	}

public:
	MemChrBuffVector	buffVt;
};
//========================================================================================================================================

//========================================================================================================================================
class MemEquipData : public IDataStruct
{
public:
	MemEquipData(){ CleanUp(); }
	virtual ~MemEquipData(){}

	void CleanUp()
	{
		equipVt.clear();
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{

	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_equip` WHERE `owner`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		equipVt.reserve( result.getRowCount() );
		while ( !result.eof() )
		{
			MemEquip equip;
			equip.id			= result.getInt64Value( "id" );
			equip.base			= result.getIntValue( "base" );
			equip.owner			= result.getInt64Value( "owner" );
			equip.star			= result.getIntValue( "star" );
			equip.starLucky		= result.getIntValue( "starLucky" );
			equip.addAttr		= result.getIntValue( "addAttr" );
			equip.UpGradeLucky  = result.getIntValue( "UpGradeLucky" );
			equip.UpQuality		= result.getIntValue( "UpQuality" );
			std::string	GemHole = result.getStringValue( "EquipGem" );
			equip.ParesEquipString( GemHole );
			equipVt.push_back( equip );
			result.nextRow();
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		int32_t vtSize = equipVt.size();
		packet->writeInt32( vtSize );
		for ( int32_t i = 0; i < vtSize; ++i )
		{
			packet->writeInt64( equipVt[i].id );
			packet->writeInt32( equipVt[i].base );
			packet->writeInt64( equipVt[i].owner );
			packet->writeInt32( equipVt[i].star );
			packet->writeInt32( equipVt[i].starLucky );
			packet->writeInt32( equipVt[i].addAttr );
			packet->writeInt32( equipVt[i].UpGradeLucky );
			packet->writeInt32( equipVt[i].UpQuality );
			packet->writeUTF8( equipVt[i].GetEquipGemString() );
		}
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		int32_t equipCount = inPacket->readInt32();
		equipVt.reserve(equipCount);
		for ( int32_t i = 0; i < equipCount; ++i )
		{
			MemEquip equip;

			equip.id				= inPacket->readInt64();
			equip.base				= inPacket->readInt32();
			equip.owner				= inPacket->readInt64();
			equip.star				= inPacket->readInt32();
			equip.starLucky			= inPacket->readInt32();
			equip.addAttr			= inPacket->readInt32();
			equip.UpGradeLucky		= inPacket->readInt32();
			equip.UpQuality			= inPacket->readInt32();
			std::string EquipGem    = inPacket->readUTF8( true );
			equip.ParesEquipString( EquipGem );
			equipVt.push_back(equip);
		}
	}

public:
	MemEquipVector	equipVt;
};

class FriendExpReward : public IDataStruct
{
public:
	FriendExpReward(){ CleanUp(); }
	virtual ~FriendExpReward(){}

	void CleanUp()
	{
		RewardInfo.clear();
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "DELETE FROM `mem_chr_friend_exp` WHERE `Cid`=%lld", nCid );
		sqls.push_back( szSQL );

		FriendExpMap::iterator iter = RewardInfo.begin();
		FriendExpMap::iterator eiter = RewardInfo.end();
		for ( ; iter != eiter; ++iter )
		{
			ExpReward& Reward = iter->second;
			bzero( szSQL, sizeof( szSQL ) );
			snprintf( szSQL, sizeof( szSQL ) - 1, 
				"INSERT INTO `mem_chr_friend_exp` (`Cid`, `FriendId`,`GetExp`,`Level`,`Name`) VALUES (%lld, %lld, %d,%d,'%s')",
				nCid,
				Reward.FriendId,
				Reward.GetExp,
				Reward.Level,
				Reward.Name.c_str());
			sqls.push_back( szSQL );
		}
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_friend_exp` WHERE `Cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		while(!result.eof())
		{
			ExpReward Reward;
			Reward.FriendId			= result.getInt64Value("FriendId");
			Reward.GetExp			= result.getIntValue("GetExp");
			Reward.Level			= result.getIntValue("Level");
			Reward.Name				= result.getStringValue("Name");
			RewardInfo[Reward.FriendId] = Reward;
			result.nextRow();
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		if (NULL == packet)
		{
			return;
		}
		int32_t Size = RewardInfo.size();
		packet->writeInt32( Size );

		FriendExpMap::iterator iter = RewardInfo.begin();
		FriendExpMap::iterator eiter = RewardInfo.end();
		for ( ; iter != eiter; ++iter )
		{
			ExpReward& Reward = iter->second;
			packet->writeInt64( Reward.FriendId );
			packet->writeInt32( Reward.GetExp );
			packet->writeInt32( Reward.Level );
			packet->writeUTF8( Reward.Name );
		}
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		if (NULL == inPacket)
		{
			return;
		}
		int32_t Size = inPacket->readInt32();
		for (int32_t i = 0; i < Size;++i)
		{
			ExpReward Reward;

			Reward.FriendId			= inPacket->readInt64();
			Reward.GetExp			= inPacket->readInt32();
			Reward.Level			= inPacket->readInt32();
			Reward.Name				= inPacket->readUTF8(true);
			RewardInfo[Reward.FriendId] = Reward;
		}
	}

public:
	FriendExpMap	RewardInfo;
};
//========================================================================================================================================
class CFaBaoData : public IDataStruct
{
public:
	CFaBaoData(){ CleanUp(); }
	virtual ~CFaBaoData(){}

	void CleanUp()
	{
		bzero( m_FaBaoId, sizeof( m_FaBaoId ) );
		bzero( m_FaBaoDress, sizeof( m_FaBaoDress ) );
		bzero( m_UpFaBaoResource, sizeof( m_UpFaBaoResource ) );
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "DELETE FROM `mem_chr_fabao` WHERE `Cid`=%lld", nCid );
		sqls.push_back( szSQL );

		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, 
			"INSERT INTO `mem_chr_fabao` (`Cid`, `FaBaoLevel`,`FaBaoRes`, `FaBaoDress`) VALUES (%lld, '%s','%s','%s')",
			nCid,
			GetFaBaoLevelString().c_str(),
			GetFaBaoResString().c_str(),
			GetFaBaoDressString().c_str());
		sqls.push_back( szSQL );
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_fabao` WHERE `Cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		while(!result.eof())
		{
			std::string FaBaoLevelString			= result.getStringValue("FaBaoLevel");
			std::string FaBaoResString				= result.getStringValue("FaBaoRes");
			std::string FaBaoDressString			= result.getStringValue("FaBaoDress");
			ParesFaBaoResString( FaBaoResString );
			ParesFaBaoLevelString( FaBaoLevelString );
			ParesFaBaoDressString( FaBaoDressString );
			result.nextRow();
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		if (NULL == packet)
		{
			return;
		}
		packet->writeInt8( static_cast<int8_t>( FA_BAO_TYPE_COUNT) );
		for ( int32_t i = 0; i < FA_BAO_TYPE_COUNT; i++ )
		{
			packet->writeInt8( i );
			packet->writeInt32( m_FaBaoId[i] );
			packet->writeInt8( m_FaBaoDress[i] );
		}

		packet->writeInt8( static_cast<int8_t>( FA_BAO_RES_COUNT) );
		for ( int32_t i = 0; i < FA_BAO_RES_COUNT; i++ )
		{
			packet->writeInt8( i );
			packet->writeInt32( m_UpFaBaoResource[i] );
		}
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		if (NULL == inPacket)
		{
			return;
		}
		int8_t nSize = inPacket->readInt8();
		for ( int32_t i = 0; i < nSize; ++i )
		{
			int8_t nFaBaoType		= inPacket->readInt8();
			m_FaBaoId[nFaBaoType]	= inPacket->readInt32();
			m_FaBaoDress[nFaBaoType]= inPacket->readInt8();
		}

		int8_t bSize = inPacket->readInt8();
		for ( int32_t i = 0; i < bSize; ++i )
		{
			int8_t nFaBaoResType			= inPacket->readInt8();
			m_UpFaBaoResource[nFaBaoResType]= inPacket->readInt32();
		}
	}
	std::string GetFaBaoLevelString()
	{
		std::stringstream ss;
		for ( int32_t i = 0; i < FA_BAO_TYPE_COUNT; ++i )
		{
			ss << static_cast<int32_t>( m_FaBaoId[i]);
			ss << ":";
		}
		return ss.str();
	}

	void ParesFaBaoLevelString( std::string FaBaoLevelString )
	{
		if ( FaBaoLevelString.empty() )
		{
			return;
		}
		StringVector ItemVector = Answer::StringUtility::split(FaBaoLevelString, ":");
		if ( ItemVector.size() == FA_BAO_TYPE_COUNT )
		{
			for ( int32_t i = 0; i < FA_BAO_TYPE_COUNT; ++i )
			{
				m_FaBaoId[i]	= atoi(ItemVector[i].c_str());
			}
		}
	}

	std::string GetFaBaoResString()
	{
		std::stringstream ss;
		for ( int32_t i = 0; i < FA_BAO_RES_COUNT; ++i )
		{
			ss << static_cast<int32_t>( m_UpFaBaoResource[i]);
			ss << ":";
		}
		return ss.str();
	}

	void ParesFaBaoResString( std::string FaBaoResString )
	{
		if ( FaBaoResString.empty() )
		{
			return;
		}
		StringVector ItemVector = Answer::StringUtility::split(FaBaoResString, ":");
		if ( ItemVector.size() == FA_BAO_RES_COUNT )
		{
			for ( int32_t i = 0; i < FA_BAO_RES_COUNT; ++i )
			{
				m_UpFaBaoResource[i]	= atoi(ItemVector[i].c_str());
			}
		}
	}

	std::string GetFaBaoDressString()
	{
		std::stringstream ss;
		for ( int32_t i = 0; i < FA_BAO_TYPE_COUNT; ++i )
		{
			ss << static_cast<int32_t>( m_FaBaoDress[i]);
			ss << ":";
		}
		return ss.str();
	}

	void ParesFaBaoDressString( std::string FaBaoDressString )
	{
		if ( FaBaoDressString.empty() )
		{
			return;
		}
		StringVector ItemVector = Answer::StringUtility::split(FaBaoDressString, ":");
		if ( ItemVector.size() == FA_BAO_TYPE_COUNT )
		{
			for ( int32_t i = 0; i < FA_BAO_TYPE_COUNT; ++i )
			{
				m_FaBaoDress[i]	= atoi(ItemVector[i].c_str());
			}
		}
	}
public:
	int8_t	m_FaBaoId[FA_BAO_TYPE_COUNT];
	int8_t	m_FaBaoDress[FA_BAO_TYPE_COUNT];
	int32_t m_UpFaBaoResource[FA_BAO_RES_COUNT];

};
//========================================================================================================================================
class MailDBData : public IDataStruct
{
public:
	MailDBData(){ CleanUp(); }
	virtual ~MailDBData(){}

	void CleanUp()
	{
		m_MailInfo.clear();
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "DELETE FROM `mem_mail` WHERE `ReceiveId`=%lld", nCid );
		sqls.push_back( szSQL );

		MailInfoMap::iterator iter = m_MailInfo.begin();
		MailInfoMap::iterator eiter = m_MailInfo.end();
		int32_t CurTime = Answer::DayTime::now();
		for ( ; iter != eiter; ++iter )
		{
			if ( iter->second.SendTime + MAIL_MAX_KEEP_TIME < CurTime )
			{
				continue;
			}
			MailInfo Mail = iter->second;
			bzero( szSQL, sizeof( szSQL ) );
			snprintf(szSQL, sizeof(szSQL)-1, "INSERT INTO `mem_mail` (`MailId`,`SysMailId`,`SenderId`,`SenderName`,`ReceiveId`,`ReceiveName`,`SendTime`,`HasRead`,`Extract`,`MailTitle`,`MailContent`,`Item1`,`Item2`,`Item3`,`Item4`,`Item5`,`Item6`,`Param`) VALUES (%d,%d,%lld,'%s',%lld,'%s',%d,%d,%d,'%s','%s','%s','%s','%s','%s','%s','%s','%s')",
				Mail.MailId,
				Mail.SysMailId,
				Mail.SenderId,
				Mail.SenderName,
				Mail.ReceiveId,
				Mail.ReceiveName,
				Mail.SendTime,
				Mail.HasRead,
				Mail.Extract,
				Mail.MailTitle,
				Mail.MailContent,
				GetItemString(Mail.Item[0]).c_str(),
				GetItemString(Mail.Item[1]).c_str(),
				GetItemString(Mail.Item[2]).c_str(),
				GetItemString(Mail.Item[3]).c_str(),
				GetItemString(Mail.Item[4]).c_str(),
				GetItemString(Mail.Item[5]).c_str(),
				Mail.Param.c_str());
			sqls.push_back( szSQL );
		}
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_mail` WHERE `ReceiveId`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		int32_t CurTime = Answer::DayTime::now();
		while(!result.eof())
		{
			MailInfo Mail = {};
			Mail.MailId				= result.getIntValue("MailId");
			Mail.SysMailId			= result.getIntValue("SysMailId");
			Mail.SenderId			= result.getInt64Value("SenderId");
			snprintf(Mail.SenderName,MAX_NAME_CCH_LENGTH,result.getStringValue("SenderName"));
			Mail.ReceiveId			= result.getInt64Value("ReceiveId");
			snprintf(Mail.ReceiveName,MAX_NAME_CCH_LENGTH,result.getStringValue("ReceiveName"));
			Mail.SendTime			= result.getIntValue("SendTime");
			Mail.HasRead			= result.getIntValue("HasRead");
			Mail.Extract			= result.getIntValue("Extract");
			Mail.Param				= result.getStringValue("Param");
			snprintf(Mail.MailTitle,MAIL_TITLE_MAX_LENGTH,result.getStringValue("MailTitle"));
			snprintf(Mail.MailContent,MAIL_CONTENT_MAX_LENGTH,result.getStringValue("MailContent"));
			Mail.Item[0]			= ParesChrBagString( result.getStringValue("Item1") );
			Mail.Item[1]			= ParesChrBagString( result.getStringValue("Item2") );
			Mail.Item[2]			= ParesChrBagString( result.getStringValue("Item3") );
			Mail.Item[3]			= ParesChrBagString( result.getStringValue("Item4") );
			Mail.Item[4]			= ParesChrBagString( result.getStringValue("Item5") );
			Mail.Item[5]			= ParesChrBagString( result.getStringValue("Item6") );
			if ( Mail.SendTime + MAIL_MAX_KEEP_TIME > CurTime )
			{
				m_MailInfo[Mail.MailId] = Mail;
			}
			result.nextRow();
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		if (NULL == packet)
		{
			return;
		}
		int32_t nSize = m_MailInfo.size();
		packet->writeInt32( nSize );
		MailInfoMap::iterator it = m_MailInfo.begin();
		for ( ; it != m_MailInfo.end(); ++it )
		{
			MailInfo Mail = it->second;
			packet->writeInt32( Mail.MailId );
			packet->writeInt32( Mail.SysMailId );
			packet->writeInt64( Mail.SenderId );
			packet->writeUTF8( Mail.SenderName );
			packet->writeInt64( Mail.ReceiveId );
			packet->writeUTF8( Mail.ReceiveName );
			packet->writeInt32( Mail.SendTime );
			packet->writeInt8( Mail.HasRead );
			packet->writeInt8( Mail.Extract );
			packet->writeUTF8( Mail.MailTitle );
			packet->writeUTF8( Mail.MailContent );
			packet->writeUTF8( Mail.Param );
			if ( Mail.Extract == 1 || Mail.Extract == 2 )
			{ 
				int8_t Count = 0;
				int32_t Offset = packet->getWOffset();
				packet->writeInt8( Count );
				for ( int8_t i = 0; i < MAIL_MAX_FU_JIAN_COUNT; ++i )
				{
					if ( Mail.Item[i].itemId <= 0 || Mail.Item[i].itemCount <= 0 )
					{
						continue;
					}
					packet->writeInt8(i);
					packet->writeInt32( Mail.Item[i].itemId );
					packet->writeInt8( Mail.Item[i].itemClass );	
					packet->writeInt32( Mail.Item[i].itemCount );
					packet->writeInt8( Mail.Item[i].bind );	
					packet->writeInt32( Mail.Item[i].endTime );
					packet->writeInt64( Mail.Item[i].srcId );
					Count++;
				}
				*(int8_t*)( packet->getBuffer() + Offset ) =  Count;
			}
		}
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		if (NULL == inPacket)
		{
			return;
		}
		int32_t nSize = inPacket->readInt32();
		for ( int32_t i = 0; i < nSize; ++i )
		{
			MailInfo Mail = {};
			Mail.MailId			= inPacket->readInt32();
			Mail.SysMailId		= inPacket->readInt32();
			Mail.SenderId		= inPacket->readInt64();
			snprintf(Mail.SenderName, sizeof(Mail.SenderName)-1, inPacket->readUTF8(true).c_str());
			Mail.ReceiveId		= inPacket->readInt64();
			snprintf(Mail.ReceiveName, sizeof(Mail.ReceiveName)-1, inPacket->readUTF8(true).c_str());
			Mail.SendTime		= inPacket->readInt32();
			Mail.HasRead		= inPacket->readInt8();
			Mail.Extract		= inPacket->readInt8();
			snprintf(Mail.MailTitle, sizeof(Mail.MailTitle)-1, inPacket->readUTF8(true).c_str());
			snprintf(Mail.MailContent, sizeof(Mail.MailContent)-1, inPacket->readUTF8(true).c_str());
			Mail.Param			= inPacket->readUTF8(true);
			if ( Mail.Extract == 1 ||  Mail.Extract == 2 )
			{ 
				int8_t Count = inPacket->readInt8();
				if ( Count >= MAIL_MAX_FU_JIAN_COUNT )
				{
					return;
				}
				for ( int8_t i = 0; i < Count; ++i )
				{
					int8_t Index				= inPacket->readInt8();
					Mail.Item[Index].itemId		= inPacket->readInt32();
					Mail.Item[Index].itemClass	= inPacket->readInt8();
					Mail.Item[Index].itemCount	= inPacket->readInt32();
					Mail.Item[Index].bind		= inPacket->readInt8();
					Mail.Item[Index].endTime	= inPacket->readInt32();
					Mail.Item[Index].srcId		= inPacket->readInt64();
				}
			}
			m_MailInfo[Mail.MailId] = Mail;
		}
	}

	std::string GetItemString( MemChrBag & Item )
	{
		std::stringstream ss;
		if ( Item.itemCount > 0 && Item.itemId > 0 )
		{
			ss << Item.itemId << ":" << static_cast<int32_t>(Item.itemClass) << ":" << Item.itemCount << ":" << static_cast<int32_t>(Item.bind) << ":" << Item.endTime << ":" << Item.srcId;
		}
		return ss.str();
	}

	MemChrBag ParesChrBagString( std::string ItemString )
	{
		MemChrBag stu = {};
		if ( ItemString.empty() )
		{
			return stu;
		}
		StringVector ItemVector = Answer::StringUtility::split(ItemString, ":");
		if ( ItemVector.size() == 6 )
		{
			stu.itemId			= atoi(ItemVector[0].c_str());
			stu.itemClass		= (int8_t)atoi(ItemVector[1].c_str());
			stu.itemCount		= atoi(ItemVector[2].c_str());
			stu.bind			= (int8_t)atoi(ItemVector[3].c_str());
			stu.endTime			= atoi(ItemVector[4].c_str());
			stu.srcId			= _atoi64(ItemVector[5].c_str());
		}
		return stu;
	}
public:
	MailInfoMap		m_MailInfo;
};
//========================================================================================================================================
class MemChrDepotData : public IDataStruct
{
public:
	MemChrDepotData(){ CleanUp(); }
	virtual ~MemChrDepotData(){}

	void CleanUp()
	{
		bzero( gambleDepot, sizeof( gambleDepot ) );
		bzero( &DeoptInfo, sizeof( DeoptInfo ) );
		bzero( Currency, sizeof( Currency ) );
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, DEL_FROM_CHAR_BAG, nCid, CBT_DEPORT );
		sqls.push_back( szSQL );

		for ( int32_t i = 0; i < MAX_DEPOT_SLOT; ++i )
		{
			if ( gambleDepot[i].itemCount > 0)
			{
				bzero( szSQL, sizeof( szSQL ) );
				snprintf( szSQL, sizeof( szSQL ) - 1, 
					SAVE_TO_CHAR_BAG,
					nCid,
					CBT_DEPORT,
					i,
					gambleDepot[i].itemId,
					gambleDepot[i].itemClass,
					gambleDepot[i].itemCount,
					gambleDepot[i].bind,
					gambleDepot[i].endTime,
					gambleDepot[i].srcId );
				sqls.push_back(szSQL);
			}
		}
		std::string CurrencyString = GetCurrencyString();
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "INSERT INTO `mem_chr_depot` (Cid,CurrencyString,CanOpenSlots,OpenedSlots,LeftSeconds) VALUES(%lld,'%s',%d,%d,%d) on duplicate key update CurrencyString = '%s', CanOpenSlots = %d,OpenedSlots = %d,LeftSeconds=%d",
				  nCid,
				  CurrencyString.c_str(),
				  DeoptInfo.m_nCanOpenSlots,
				  DeoptInfo.m_nOpenedSlots,
				  DeoptInfo.m_nLeftSeconds,
				  CurrencyString.c_str(),
				  DeoptInfo.m_nCanOpenSlots,
				  DeoptInfo.m_nOpenedSlots,
				  DeoptInfo.m_nLeftSeconds );
		sqls.push_back(szSQL);
	}
	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof(szSQL)-1, LOAD_FROM_CHAR_BAG, nCid, CBT_DEPORT );
		Answer::MySqlQuery result = db.query( szSQL );
		int32_t gambleSlotCount = 0;
		while( !result.eof() )
		{
			MemChrBag slotData = {};
			int32_t slot		= result.getIntValue("slot");
			slotData.itemId		= result.getIntValue("id");
			slotData.itemClass	= static_cast<int8_t>( result.getIntValue("class") );
			slotData.itemCount	= result.getIntValue("count");
			slotData.bind		= static_cast<int8_t>( result.getIntValue("bind") );
			slotData.endTime	= result.getIntValue("endtime");
			slotData.srcId		= result.getInt64Value("srcid");
			if (slot >= 0 && slot < MAX_DEPOT_SLOT )
			{
				gambleDepot[slot] = slotData;
			}
			else
			{
				LOG_ERROR("wrong gamble slot data slot = %d, cid = %lld\n", slot, nCid);
			}
			result.nextRow();
		}
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof(szSQL)-1, "SELECT * FROM `mem_chr_depot` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery resultTmp = db.query( szSQL );
		while( !resultTmp.eof() )
		{
			MemChrBag slotData = {};
			std::string CurrencyString = resultTmp.getStringValue("CurrencyString");
			DeoptInfo.m_nCanOpenSlots  = resultTmp.getIntValue("CanOpenSlots");
			DeoptInfo.m_nOpenedSlots   = resultTmp.getIntValue("OpenedSlots");
			DeoptInfo.m_nLeftSeconds   = resultTmp.getIntValue("LeftSeconds");
			InitCurrency( CurrencyString );
			resultTmp.nextRow();
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		uint32_t nCount = packet->getWOffset();
		int32_t gambleSlotCount = 0;
		packet->writeInt32( gambleSlotCount );
		for (int32_t i = 0; i < MAX_DEPOT_SLOT; ++i)
		{
			if ( gambleDepot[i].itemCount > 0 )
			{
				packet->writeInt8( i );
				packet->writeInt32( gambleDepot[i].itemId );
				packet->writeInt8( gambleDepot[i].itemClass );
				packet->writeInt32( gambleDepot[i].itemCount );
				packet->writeInt8( gambleDepot[i].bind );
				packet->writeInt32( gambleDepot[i].endTime );
				packet->writeInt64( gambleDepot[i].srcId );
				++gambleSlotCount;
			}
		}
		*( (int32_t*)( packet->getBuffer() + nCount ) ) = gambleSlotCount;
		packet->writeInt32( DeoptInfo.m_nCanOpenSlots );
		packet->writeInt32( DeoptInfo.m_nOpenedSlots );
		packet->writeInt32( DeoptInfo.m_nLeftSeconds );
		std::string CurrencyString = GetCurrencyString();
		packet->writeUTF8( CurrencyString );
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		int32_t gambleSlotCount = inPacket->readInt32();
		for (int32_t i = 0; i < gambleSlotCount; ++i)
		{
			MemChrBag slotData = {};
			int32_t slot		= inPacket->readInt8();
			slotData.itemId		= inPacket->readInt32();
			slotData.itemClass	= inPacket->readInt8();
			slotData.itemCount	= inPacket->readInt32();
			slotData.bind		= inPacket->readInt8();
			slotData.endTime	= inPacket->readInt32();
			slotData.srcId		= inPacket->readInt64();

			if (slot >= 0 && slot < MAX_DEPOT_SLOT)
			{
				gambleDepot[slot] = slotData;
			}
			else
			{
				LOG_ERROR("wrong bag slotData data slotData = %d, cid = %lld\n",  slot, nCid );
			}
		} 
		DeoptInfo.m_nCanOpenSlots = inPacket->readInt32();
		DeoptInfo.m_nOpenedSlots  = inPacket->readInt32();
		DeoptInfo.m_nLeftSeconds  = inPacket->readInt32();
		std::string Currency = inPacket->readUTF8( true );
		InitCurrency( Currency );
	}
	std::string GetCurrencyString()
	{
		std::stringstream ss;
		for ( int32_t i = 0; i < CURRENCY_TYPE_COUNT; ++i )
		{
			if ( Currency[i] > 0 )
			{
				ss << "|" << i << ":" << Currency[i];
			}
		}
		return ss.str();
	}
	void		InitCurrency( std::string CurrencyString )
	{
		StringVector vStr = Answer::StringUtility::split( CurrencyString, "|" );
		int32_t isize = vStr.size();
		for ( int32_t i = 0; i < isize; ++i )
		{
			StringVector tv = Answer::StringUtility::split( vStr[i], ":" );
			if ( tv.size() != 2 )
			{
				continue;
			}
			int32_t nType = atoi( tv[0].c_str() );
			if ( nType < 0 || nType >= CURRENCY_TYPE_COUNT )
			{
				continue;
			}
			Currency[nType] = atoi( tv[1].c_str() );
		}
	}
public:
	MemChrBag	gambleDepot[MAX_DEPOT_SLOT];
	ChrBagInfo	DeoptInfo;
	int32_t		Currency[CURRENCY_TYPE_COUNT];
};
//========================================================================================================================================

//========================================================================================================================================
class MemYellowStoneData : public IDataStruct
{
public:
	MemYellowStoneData(){ CleanUp(); }
	virtual ~MemYellowStoneData(){}

	void CleanUp()
	{
		stonVt.clear();
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{

	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_yellow_stone` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		while ( !result.eof() )
		{
			MemYellowStone mys = {};
			mys.cid			= result.getInt64Value("cid");
			mys.id			= result.getIntValue("id");
			mys.get_time	= result.getIntValue("get_time");
			stonVt.push_back( mys );
			result.nextRow();
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		int32_t vtSize = stonVt.size();
		packet->writeInt32( vtSize );
		for ( int32_t i = 0; i < vtSize; ++i )
		{
			packet->writeInt64( stonVt[i].cid );
			packet->writeInt32( stonVt[i].id );
			packet->writeInt32( stonVt[i].get_time );

		}
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		int32_t mysvSize = inPacket->readInt32();
		stonVt.reserve(mysvSize);
		for ( int32_t i=0; i < mysvSize ;++i )
		{
			MemYellowStone mys = {};

			mys.cid			= inPacket->readInt64();
			mys.id			= inPacket->readInt32();
			mys.get_time	= inPacket->readInt32();

			stonVt.push_back(mys);
		}
	}

public:
	MemYellowStoneVector stonVt;
};
//========================================================================================================================================

//========================================================================================================================================
class SysUserPreventWallowData : public IDataStruct
{
public:
	SysUserPreventWallowData(){ CleanUp(); }
	virtual ~SysUserPreventWallowData(){}

	void CleanUp()
	{
		bzero( &data, sizeof( data ) );
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1,
			"UPDATE `sys_user_prevent_wallow` SET `name`='%s',`identitycard`= '%s', `isGrowUp` = %d WHERE `uid`=%d AND `sid`=%d",
			data.name,
			data.identitycard, 
			data.isGrowUp, 
			data.uid, 
			data.sid );
		sqls.push_back(szSQL);
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof(szSQL)-1, "SELECT * FROM `sys_user_prevent_wallow` WHERE `uid`= %d AND `sid`= %d", nUid, nSid );
		Answer::MySqlQuery result = db.query(szSQL);
		//û�м�¼
		if (result.eof())
		{
			data.sid		= nSid;
			data.uid		= nUid;
			data.isGrowUp	= 0;

			bzero( szSQL, sizeof(szSQL) );
			snprintf( szSQL, sizeof(szSQL)-1, "INSERT INTO `sys_user_prevent_wallow`(`uid`, `sid`,`name`,`identitycard`,`isGrowUp`) VALUES (%d, %d,'%s','%s', %d)", data.uid, data.sid, "", "", data.isGrowUp );
			db.excute( szSQL );
		}
		else
		{
			data.sid			= result.getIntValue("sid");
			data.uid			= result.getIntValue("uid");
			data.isGrowUp	= result.getIntValue("isGrowUp");

			snprintf( data.name, MAX_NAME_CCH_LENGTH,result.getStringValue("name") );
			snprintf( data.identitycard, MAX_IDENTITYCARD_LENGTH,result.getStringValue("identitycard") );
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		packet->writeInt32( data.uid );
		packet->writeInt32( data.sid );
		packet->writeUTF8( data.name );
		packet->writeUTF8( data.identitycard );
		packet->writeInt32( data.isGrowUp );
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		data.uid			= inPacket->readInt32(); 
		data.sid			= inPacket->readInt32(); 
		snprintf( data.name, MAX_NAME_CCH_LENGTH, inPacket->readUTF8(true).c_str() );
		snprintf( data.identitycard, MAX_IDENTITYCARD_LENGTH, inPacket->readUTF8(true).c_str() );
		data.isGrowUp	= inPacket->readInt32(); 
	}

public:
	SysUserPreventWallow	data;
};
//========================================================================================================================================
class ChouJiangData : public IDataStruct
{
public:
	ChouJiangData(){ CleanUp(); }
	virtual ~ChouJiangData(){}
	void CleanUp()
	{
		m_RecordList.clear();		
		m_LuckyPoint		= 0;		
		m_Score			= 0;
		bzero(&m_ItemList,sizeof(m_ItemList));
	}
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );					
		snprintf( szSQL, sizeof( szSQL ) - 1, DEL_FROM_CHAR_BAG, nCid, CBT_CHOUJIANG );			
		sqls.push_back( szSQL );			

		for ( int32_t i = 0; i < MAX_ITEM_LIST; ++i )			
		{			
			if ( m_ItemList[i].itemCount > 0)		
			{		
				bzero( szSQL, sizeof( szSQL ) );	
				snprintf( szSQL, sizeof( szSQL ) - 1, 	
					SAVE_TO_CHAR_BAG,
					nCid,
					CBT_CHOUJIANG,
					i,
					m_ItemList[i].itemId,
					m_ItemList[i].itemClass,
					m_ItemList[i].itemCount,
					m_ItemList[i].bind,
					m_ItemList[i].endTime,
					m_ItemList[i].srcId );
				sqls.push_back(szSQL);	
			}		
		}			


		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1,
		"INSERT `mem_chr_chou_jiang_score`(Cid,LuckPoint,Score) VALUES (%lld,%d,%d) ON DUPLICATE KEY UPDATE LuckPoint =%d,Score=%d", nCid,m_LuckyPoint,m_Score,m_LuckyPoint,m_Score );
		sqls.push_back( szSQL );
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "DELETE FROM `mem_chr_chou_jiang_record` WHERE `cid` = %lld", nCid );
		sqls.push_back( szSQL );					

		CJRecordList::iterator it = m_RecordList.begin();
		for ( ; it!= m_RecordList.end(); ++it )
		{
			bzero( szSQL, sizeof( szSQL ) );
			snprintf( szSQL, sizeof( szSQL ) - 1,
				"INSERT `mem_chr_chou_jiang_record`( Cid,ItemId,ItemClass,ItemCount,Time ) VALUES (%lld,%d,%d,%d,%d)",
				nCid,
				it->CJItem.itemId,
				it->CJItem.itemClass,
				it->CJItem.itemCount,
				it->Time
				);
			sqls.push_back( szSQL );
		}
	}
	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );					
		snprintf( szSQL, sizeof(szSQL)-1, LOAD_FROM_CHAR_BAG, nCid, CBT_CHOUJIANG );			
		Answer::MySqlQuery result = db.query( szSQL );			
		int32_t gambleSlotCount = 0;			
		while( !result.eof() )			
		{			
			MemChrBag slotData = {};		
			int32_t slot		= result.getIntValue("slot");
			slotData.itemId		= result.getIntValue("id");
			slotData.itemClass	= static_cast<int8_t>( result.getIntValue("class") );	
			slotData.itemCount	= result.getIntValue("count");	
			slotData.bind		= static_cast<int8_t>( result.getIntValue("bind") );
			slotData.endTime	= result.getIntValue("endtime");	
			slotData.srcId		= result.getInt64Value("srcid");
			if (slot >= 0 && slot < MAX_ITEM_LIST )		
			{		
				m_ItemList[slot] = slotData;	
			}		
			else		
			{		
				LOG_ERROR("wrong gamble slot data slot = %d, cid = %lld\n", slot, nCid);	
			}		
			result.nextRow();		
		}			

		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_chou_jiang_score` WHERE `Cid`=%lld", nCid );
		Answer::MySqlQuery result_1 = db.query( szSQL );
		while(!result_1.eof())
		{
			m_LuckyPoint = result_1.getIntValue("LuckPoint");
			m_Score		 = result_1.getIntValue("Score");
			result_1.nextRow();
		}
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_chou_jiang_record` WHERE `Cid`=%lld", nCid );
		Answer::MySqlQuery result_2 = db.query( szSQL );
		while(!result_2.eof())
		{
			ChouJiangRecord CJItem = {};
			CJItem.CJItem.itemId		= result_2.getIntValue("ItemId");
			CJItem.CJItem.itemClass		= result_2.getIntValue("ItemClass");
			CJItem.CJItem.itemCount		= result_2.getIntValue("ItemCount");
			CJItem.Time					= result_2.getIntValue("Time");
			m_RecordList.push_back(CJItem);
			result_2.nextRow();
		}
		return true;
	}
	virtual void PackageData( Answer::NetPacket* packet )
	{
		packet->writeInt32( m_LuckyPoint );
		packet->writeInt32( m_Score );
		int32_t Count = 0;
		int32_t OldOffet = packet->getWOffset();
		packet->writeInt32( Count );
		for ( int32_t i = 0; i < MAX_ITEM_LIST; ++i )
		{
			if ( m_ItemList[i].itemId <= 0 || m_ItemList[i].itemCount <= 0 )
			{
				continue;
			}
			Count++;
			packet->writeInt32( i );
			packet->writeInt32( m_ItemList[i].itemId );
			packet->writeInt8( m_ItemList[i].itemClass );
			packet->writeInt32(m_ItemList[i].itemCount );
			packet->writeInt8( m_ItemList[i].bind );
			packet->writeInt32( m_ItemList[i].endTime );
			packet->writeInt64( m_ItemList[i].srcId );
		}
		int32_t NewOffet = packet->getWOffset();
		packet->setWOffset( OldOffet );
		packet->writeInt32( Count );
		packet->setWOffset( NewOffet );
		Count = m_RecordList.size();
		packet->writeInt32( Count );
		CJRecordList::iterator it = m_RecordList.begin();
		for ( ; it != m_RecordList.end(); ++it )
		{
			packet->writeInt32(  it->CJItem.itemId );
			packet->writeInt8( it->CJItem.itemClass );
			packet->writeInt32( it->CJItem.itemCount );
			packet->writeInt32( it->Time );
		}
	}
	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		m_LuckyPoint			= inPacket->readInt32();
		m_Score					= inPacket->readInt32();
		int32_t Count			= inPacket->readInt32();
		for ( int32_t i = 0; i < Count; i++ )
		{
			int32_t Index = inPacket->readInt32();
			MemChrBag CJItem = {};
			CJItem.itemId		= inPacket->readInt32();
			CJItem.itemClass	= inPacket->readInt8();
			CJItem.itemCount	= inPacket->readInt32();
			CJItem.bind			= inPacket->readInt8();
			CJItem.endTime		= inPacket->readInt32();
			CJItem.srcId		= inPacket->readInt64();
			if ( Index < 0 || Index >= MAX_ITEM_LIST )
			{
				continue;
			}
			m_ItemList[Index]	= CJItem;
		}
		int32_t RecordCount	= 	inPacket->readInt32();
		for (int32_t j = 0; j < RecordCount; j++ )
		{
			ChouJiangRecord Record = {};
			Record.CJItem.itemId		= inPacket->readInt32();
			Record.CJItem.itemClass		= inPacket->readInt8();
			Record.CJItem.itemCount		= inPacket->readInt32();
			Record.Time					= inPacket->readInt32();
			m_RecordList.push_back(Record);
		}
	}
public:
	CJRecordList  m_RecordList;						
	MemChrBag	  m_ItemList[MAX_ITEM_LIST];		
	int32_t		  m_LuckyPoint;					
	int32_t		  m_Score;			
};
//========================================================================================================================================
class MountDBData : public IDataStruct
{
public:
	MountDBData(){ CleanUp(); }
	virtual ~MountDBData(){}
	void CleanUp()
	{
		m_Quality				= 0;										
		m_LuckyPoint			= 0;									
		m_SkillLatticeCount		= 0;							
		m_EatMountHeart			= 0;								
		m_nMountState			= 0;
		m_CurRide				= 0;
		m_SkillMap.clear();						
	}
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		std::string SkillSaveString = GetSkillSaveString();
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1,
			"INSERT INTO `mem_chr_mount` (Cid,Quality,LuckyPoint,SkillLatticeCount,EatMountHeart,MountState,CurRide,SkillMap) VALUES(%lld,%d,%d,%d,%d,%d,%d,'%s') on duplicate key update Quality=%d,LuckyPoint=%d,SkillLatticeCount=%d,EatMountHeart=%d,MountState=%d,CurRide=%d,SkillMap='%s'",
			nCid,
			m_Quality,									
			m_LuckyPoint,									
			m_SkillLatticeCount,							
			m_EatMountHeart,								
			m_nMountState,
			m_CurRide,
			SkillSaveString.c_str(),
			m_Quality,										
			m_LuckyPoint,									
			m_SkillLatticeCount,							
			m_EatMountHeart,								
			m_nMountState,
			m_CurRide,
			SkillSaveString.c_str()
			);
		sqls.push_back( szSQL );
	}
	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_mount` WHERE `Cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		std::string SkillSaveString = "";
		while(!result.eof())
		{
			m_Quality				= (uint8_t)result.getIntValue("Quality");								
			m_LuckyPoint			= result.getIntValue("LuckyPoint");										
			m_SkillLatticeCount		= (uint8_t)result.getIntValue("SkillLatticeCount");						
			m_EatMountHeart			= result.getIntValue("EatMountHeart");								
			m_nMountState			= (uint8_t)result.getIntValue("MountState");
			m_CurRide				= (uint8_t)result.getIntValue("CurRide");
			SkillSaveString			= result.getStringValue("SkillMap");
			InitMountSkill(SkillSaveString);
			return true;
		}
		return false;
	}
	virtual void PackageData( Answer::NetPacket* packet )
	{
		packet->writeInt8( m_Quality );
		packet->writeInt32( m_LuckyPoint );
		packet->writeInt8( m_SkillLatticeCount );
		packet->writeInt32( m_EatMountHeart );
		packet->writeInt8( m_nMountState );
		packet->writeInt8( m_CurRide );
		std::string SkillString = GetSkillSaveString();
		packet->writeUTF8(SkillString.c_str());
	}
	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		m_Quality				= inPacket->readUInt8();
		m_LuckyPoint			= inPacket->readInt32();
		m_SkillLatticeCount		= inPacket->readUInt8();
		m_EatMountHeart			= inPacket->readInt32();
		m_nMountState			= inPacket->readUInt8();
		m_CurRide				= inPacket->readUInt8();
		std::string SkillInfo	= inPacket->readUTF8(true);
		InitMountSkill( SkillInfo );
	}

	 std::string GetSkillSaveString( )
	{
		std::stringstream ss;
		std::map<uint8_t,uint8_t>::iterator  it = m_SkillMap.begin();
		for ( ; it != m_SkillMap.end(); ++it )
		{
			ss	<< (int32_t)it->first
				<< ":" 
				<< (int32_t)it->second
				<< "|" ;
		}
		return ss.str();
	}

	void InitMountSkill ( const std::string LoadString )
	{
		if ( LoadString == "" )
		{
			return;
		}
		StringVector mineString = Answer::StringUtility::split( LoadString, "|");
		StringVector::iterator it = mineString.begin();
		for ( ; it != mineString.end(); ++it )
		{
			StringVector SkillString = Answer::StringUtility::split( *it, ":");
			if ( SkillString.size() == 2 )
			{
				m_SkillMap.insert( make_pair<uint8_t,uint8_t>( (uint8_t)atoi(SkillString[0].c_str()), (uint8_t)atoi(SkillString[1].c_str()) ) );
			}
		}
	}
public:
	uint8_t			m_Quality;										//����Ʒ��(�Ƚ�)
	int32_t			m_LuckyPoint;									//����ֵ
	uint8_t			m_SkillLatticeCount;							//�����ļ��ܸ�������
	int32_t			m_EatMountHeart;								//�Գ�����֮�ĸ���
	uint8_t			m_nMountState;
	uint8_t			m_CurRide;
	std::map<uint8_t,uint8_t>	 m_SkillMap;						//<uint8_t ��������,uint8_t ���ܵȼ�>
};
//========================================================================================================================================
class OperateLimitDBData : public IDataStruct
{
public:
	OperateLimitDBData(){ CleanUp(); }
	virtual ~OperateLimitDBData(){}

	void CleanUp()
	{
		m_mOperateLimit.clear();
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		OperateLimitMap::iterator iter = m_mOperateLimit.begin();
		OperateLimitMap::iterator eiter = m_mOperateLimit.end();
		for ( ; iter != eiter; ++iter )
		{
			bzero( szSQL, sizeof( szSQL ) );
			snprintf( szSQL, sizeof( szSQL ) - 1,
				"INSERT `mem_char_operate_limit`(cid,LimitId,LimitCount) VALUES (%lld,%d,%d) ON DUPLICATE KEY UPDATE LimitCount=%d",
				nCid,
				iter->second.LimitId,
				iter->second.LimitCount,
				iter->second.LimitCount );
			sqls.push_back( szSQL );
		}
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_char_operate_limit` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );

		while(!result.eof())
		{
			OperateLimit limit = {};
			limit.LimitId		= result.getIntValue("LimitId");
			limit.LimitCount	= result.getIntValue("LimitCount");

			m_mOperateLimit[limit.LimitId] = limit;

			result.nextRow();
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		int32_t mapSize = m_mOperateLimit.size();
		packet->writeInt32( mapSize );
		OperateLimitMap::iterator iter = m_mOperateLimit.begin();
		OperateLimitMap::iterator eiter = m_mOperateLimit.end();
		for ( ; iter != eiter; ++iter )
		{
			packet->writeInt32( iter->second.LimitId );
			packet->writeInt32( iter->second.LimitCount );
		}
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		int32_t mapSize = inPacket->readInt32();
		for ( int32_t i = 0; i < mapSize; ++i )
		{
			OperateLimit limit = {};		
			limit.LimitId		= inPacket->readInt32();
			limit.LimitCount	= inPacket->readInt32();

			m_mOperateLimit[limit.LimitId] = limit;
		}
	}

public:
	OperateLimitMap	m_mOperateLimit;
};
//========================================================================================================================================

//========================================================================================================================================
class CurrencyDBData : public IDataStruct
{
public:
	CurrencyDBData(){ CleanUp(); }
	virtual ~CurrencyDBData(){}

	void CleanUp()
	{
		m_str = "";
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1,
			"INSERT `mem_char_currency`(cid,currency) VALUES (%lld,'%s') ON DUPLICATE KEY UPDATE currency='%s'",
			nCid,
			m_str.c_str(),
			m_str.c_str() );
		sqls.push_back( szSQL );
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_char_currency` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );

		while(!result.eof())
		{
			m_str	= result.getStringValue("currency");
			return true;
		}
		return false;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		packet->writeUTF8( m_str );
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		m_str = inPacket->readUTF8( true );
	}

public:
	std::string	m_str;
};
//========================================================================================================================================

//========================================================================================================================================
class MemPetDBData
{
public:
	MemPetDBData(){ CleanUp(); }
	~MemPetDBData(){}

	void CleanUp()
	{
		nOwner		= 0;
		nPetId		= 0;
		nBaseId		= 0;
		strName		= "";
		nLevel		= 0;
		nExp		= 0;
		nPotential	= 0;
		nRein		= 0;
		nAdjustRein	= 0;
		nNumber		= 0;
		nPhase		= 0;
		nLucky		= 0;
		records		= "";
		skills		= "";
		bzero( vAttr, sizeof( vAttr ) );

		nFlag		= 0;
		nBattle		= 0;
		nPoints		= 0;
		nFromWay	= 0;
		nBornFlag	= 0;
		nGrowTimes	= 0;
	}

	void InsertSqlString( char (&szSQL)[MAX_SQL_LENGTH] )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof(szSQL)-1, 
			"INSERT INTO `mem_pet` (pid,owner,baseid,name,level,exp,potential,rein,adj_rein,number,phase,lucky,\
			start_hp,start_phy_atk_min,start_phy_atk_max,start_phy_def,start_mag_atk_min,start_mag_atk_max,start_mag_def,start_dodge,start_hitrate,start_critrate,start_tenacity,\
			grow_hp,grow_phy_atk_min,grow_phy_atk_max,grow_phy_def,grow_mag_atk_min,grow_mag_atk_max,grow_mag_def,grow_dodge,grow_hitrate,grow_critrate,grow_tenacity,\
			points,battle,records,skills,fromway,bornflag,growtimes,delflag) \
			VALUES (%lld,%lld,%d,'%s',%d,%lld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s','%s',%d,%d,%d,%d)",
			nPetId,
			nOwner,
			nBaseId,
			strName.c_str(),
			nLevel,
			nExp,
			nPotential,
			nRein,
			nAdjustRein,
			nNumber,
			nPhase,
			nLucky,
			vAttr[PET_ATTR_START_HP],
			vAttr[PET_ATTR_START_PHY_ATK_MIN],
			vAttr[PET_ATTR_START_PHY_ATK_MAX],
			vAttr[PET_ATTR_START_PHY_DEF],
			vAttr[PET_ATTR_START_MAG_ATK_MIN],
			vAttr[PET_ATTR_START_MAG_ATK_MAX],
			vAttr[PET_ATTR_START_MAG_DEF],
			vAttr[PET_ATTR_START_DODGE],
			vAttr[PET_ATTR_START_HITRATE],
			vAttr[PET_ATTR_START_CRITRATE],
			vAttr[PET_ATTR_START_TENACITY],
			vAttr[PET_ATTR_GROW_HP],
			vAttr[PET_ATTR_GROW_PHY_ATK_MIN],
			vAttr[PET_ATTR_GROW_PHY_ATK_MAX],
			vAttr[PET_ATTR_GROW_PHY_DEF],
			vAttr[PET_ATTR_GROW_MAG_ATK_MIN],
			vAttr[PET_ATTR_GROW_MAG_ATK_MAX],
			vAttr[PET_ATTR_GROW_MAG_DEF],
			vAttr[PET_ATTR_GROW_DODGE],
			vAttr[PET_ATTR_GROW_HITRATE],
			vAttr[PET_ATTR_GROW_CRITRATE],
			vAttr[PET_ATTR_GROW_TENACITY],
			nPoints,
			nBattle,
			records.c_str(),
			skills.c_str(),
			nFromWay,
			nBornFlag,
			nGrowTimes,
			0
			);
	}

	void UpdateSqlString( char (&szSQL)[MAX_SQL_LENGTH] )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof(szSQL)-1, 
			"UPDATE `mem_pet` SET owner=%lld,baseid=%d,name='%s',level=%d,exp=%lld,potential=%d,rein=%d,adj_rein=%d,number=%d,phase=%d,lucky=%d,\
			start_hp=%d,start_phy_atk_min=%d,start_phy_atk_max=%d,start_phy_def=%d,start_mag_atk_min=%d,start_mag_atk_max=%d,start_mag_def=%d,start_dodge=%d,start_hitrate=%d,start_critrate=%d,start_tenacity=%d,\
			grow_hp=%d,grow_phy_atk_min=%d,grow_phy_atk_max=%d,grow_phy_def=%d,grow_mag_atk_min=%d,grow_mag_atk_max=%d,grow_mag_def=%d,grow_dodge=%d,grow_hitrate=%d,grow_critrate=%d,grow_tenacity=%d,\
			points=%d,battle=%d,records='%s',skills='%s',fromway=%d,bornflag=%d,growtimes=%d WHERE pid=%lld",
			nOwner,
			nBaseId,
			strName.c_str(),
			nLevel,
			nExp,
			nPotential,
			nRein,
			nAdjustRein,
			nNumber,
			nPhase,
			nLucky,
			vAttr[PET_ATTR_START_HP],
			vAttr[PET_ATTR_START_PHY_ATK_MIN],
			vAttr[PET_ATTR_START_PHY_ATK_MAX],
			vAttr[PET_ATTR_START_PHY_DEF],
			vAttr[PET_ATTR_START_MAG_ATK_MIN],
			vAttr[PET_ATTR_START_MAG_ATK_MAX],
			vAttr[PET_ATTR_START_MAG_DEF],
			vAttr[PET_ATTR_START_DODGE],
			vAttr[PET_ATTR_START_HITRATE],
			vAttr[PET_ATTR_START_CRITRATE],
			vAttr[PET_ATTR_START_TENACITY],
			vAttr[PET_ATTR_GROW_HP],
			vAttr[PET_ATTR_GROW_PHY_ATK_MIN],
			vAttr[PET_ATTR_GROW_PHY_ATK_MAX],
			vAttr[PET_ATTR_GROW_PHY_DEF],
			vAttr[PET_ATTR_GROW_MAG_ATK_MIN],
			vAttr[PET_ATTR_GROW_MAG_ATK_MAX],
			vAttr[PET_ATTR_GROW_MAG_DEF],
			vAttr[PET_ATTR_GROW_DODGE],
			vAttr[PET_ATTR_GROW_HITRATE],
			vAttr[PET_ATTR_GROW_CRITRATE],
			vAttr[PET_ATTR_GROW_TENACITY],
			nPoints,
			nBattle,
			records.c_str(),
			skills.c_str(),
			nFromWay,
			nBornFlag,
			nGrowTimes,
			nPetId
			);
	}

	void PackageData( Answer::NetPacket* packet ) const
	{
		if ( NULL == packet )
		{
			return;
		}

		packet->writeInt64( nOwner );
		packet->writeInt64( nPetId );
		packet->writeInt32( nBaseId );
		packet->writeUTF8( strName );
		packet->writeInt32( nLevel );
		packet->writeInt64( nExp );
		packet->writeInt32( nPotential );
		packet->writeInt32( nRein );
		packet->writeInt32( nAdjustRein );
		packet->writeInt32( nNumber );
		packet->writeInt8( nPhase );
		packet->writeInt32( nLucky );
		packet->writeInt8( nBornFlag );
		packet->writeInt32( nGrowTimes );
		for ( int32_t i = 0; i < PET_ATTR_COUNT; ++i )
		{
			packet->writeInt32( vAttr[i] );
		}
		packet->writeUTF8( records );
		packet->writeUTF8( skills );

		packet->writeInt32( nFlag );
		packet->writeInt32( nBattle );
		packet->writeInt32( nPoints );
		packet->writeInt8( nFromWay );
	}

	void UnPackageData( Answer::NetPacket* inPacket )
	{
		if ( NULL == inPacket )
		{
			return;
		}

		nOwner		= inPacket->readInt64();
		nPetId		= inPacket->readInt64();
		nBaseId		= inPacket->readInt32();
		strName		= inPacket->readUTF8( true );
		nLevel		= inPacket->readInt32();
		nExp		= inPacket->readInt64();
		nPotential	= inPacket->readInt32();
		nRein		= inPacket->readInt32();
		nAdjustRein	= inPacket->readInt32();
		nNumber		= inPacket->readInt32();
		nPhase		= inPacket->readInt8();
		nLucky		= inPacket->readInt32();
		nBornFlag	= inPacket->readInt8();
		nGrowTimes	= inPacket->readInt32();
		for ( int32_t j = 0; j < PET_ATTR_COUNT; ++j )
		{
			vAttr[j]		= inPacket->readInt32();
		}
		records			= inPacket->readUTF8( true );
		skills			= inPacket->readUTF8( true );

		nFlag		= inPacket->readInt32();
		nBattle		= inPacket->readInt32();
		nPoints		= inPacket->readInt32();
		nFromWay	= inPacket->readInt8();
	}

public:
	CharId_t		nOwner;
	PetId_t			nPetId;
	int32_t			nBaseId;
	std::string		strName;
	int32_t			nLevel;
	int64_t			nExp;
	int32_t			nPotential;
	int32_t			nRein;
	int32_t			nAdjustRein;
	int32_t			nNumber;
	int8_t			nPhase;
	int32_t			nLucky;
	int32_t			vAttr[PET_ATTR_COUNT];
	int8_t			nBornFlag;
	int32_t			nGrowTimes;
	std::string		records;
	std::string		skills;

	int32_t			nFlag;
	int32_t			nBattle;
	int32_t			nPoints;
	int8_t			nFromWay;
};
//========================================================================================================================================

//========================================================================================================================================
typedef std::list<DBPet> DBPetList;
typedef std::list<DBPetEgg> DBPetEggList;
class PetDBData : public IDataStruct
{
public:
	PetDBData(){ CleanUp(); }
	virtual ~PetDBData(){}

	void CleanUp()
	{
		charPets.clear();
		charPetEggs.clear();
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "DELETE FROM `mem_chr_pet` WHERE `cid` = %lld", nCid );
		sqls.push_back( szSQL );

		{
			DBPetList::iterator iter = charPets.begin();
			DBPetList::iterator eiter = charPets.end();
			for ( ; iter != eiter; ++iter )
			{
				bzero( szSQL, sizeof( szSQL ) );
				snprintf( szSQL, sizeof( szSQL ) - 1,
					"INSERT `mem_chr_pet`( cid,pid,hp,alive,soul,state,bag,knight,family_id,qi_shi ) VALUES (%lld,%lld,%d,%d,%d,%d,%d,%d,%lld,%d )",
					nCid,
					iter->nPetId,
					iter->nHP,
					iter->bAlive,
					iter->nSoul,
					iter->nState,
					iter->nBag,
					iter->nKnight,
					iter->nFamilyId,
					iter->nQiShi
					);
				sqls.push_back( szSQL );
			}
		}

		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "DELETE FROM `mem_chr_pet_egg` WHERE `cid` = %lld", nCid );
		sqls.push_back( szSQL );

		{
			DBPetEggList::iterator iter = charPetEggs.begin();
			DBPetEggList::iterator eiter = charPetEggs.end();
			for ( ; iter != eiter; ++iter )
			{
				bzero( szSQL, sizeof( szSQL ) );
				snprintf( szSQL, sizeof( szSQL ) - 1,
					"INSERT `mem_chr_pet_egg`( cid,baseid,bag,slot,state,starttime,pid ) VALUES (%lld,%d,%d,%d,%d,%d,%lld)",
					nCid,
					iter->nBaseId,
					iter->nBag,
					iter->nSlot,
					iter->nState,
					iter->nStartTime,
					static_cast<int64_t>( iter->nPetId )
					);
				sqls.push_back( szSQL );
			}
		}
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_pet` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );

		DBPet pet;
		while(!result.eof())
		{
			bzero( &pet, sizeof( pet ) );
			pet.nPetId			= result.getInt64Value("pid");
			pet.nHP				= result.getIntValue("hp");
			pet.bAlive			= result.getIntValue("alive");
			pet.nSoul			= result.getIntValue("soul");
			pet.nState			= result.getIntValue("state");
			pet.nBag			= result.getIntValue("bag");
			pet.nKnight			= result.getIntValue("knight");
			pet.nFamilyId		= result.getInt64Value("family_id");
			pet.nQiShi			= result.getIntValue("qi_shi");

			charPets.push_back( pet );

			result.nextRow();
		}

		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_pet_egg` WHERE `cid`=%lld", nCid );
		result = db.query( szSQL );

		DBPetEgg egg;
		while(!result.eof())
		{
			bzero( &egg, sizeof( egg ) );
			egg.nBaseId			= result.getIntValue("baseid");
			egg.nBag		= result.getIntValue("bag");
			egg.nSlot		= result.getIntValue("slot");
			egg.nState		= result.getIntValue("state");
			egg.nStartTime	= result.getIntValue("starttime");
			egg.nPetId		= static_cast<PetId_t>( result.getInt64Value("pid") );

			charPetEggs.push_back( egg );

			result.nextRow();
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		writePetsInfo( packet );
		writePetsEggsInfo( packet );
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		readPetsInfo( inPacket );
		readPetEggsInfo( inPacket );
	}


private:
	void writePetsInfo( Answer::NetPacket* packet )
	{
		if ( NULL == packet )
		{
			return;
		}

		int32_t nCount = charPets.size();
		packet->writeInt32( nCount );
		DBPetList::iterator iter = charPets.begin();
		DBPetList::iterator eiter = charPets.end();
		for ( ; iter != eiter; ++iter )
		{
			packet->writeInt64( iter->nPetId );
			packet->writeInt32( iter->nHP );
			packet->writeInt8( iter->bAlive );
			packet->writeInt32( iter->nSoul );
			packet->writeInt8( iter->nState );
			packet->writeInt8( iter->nBag );
			packet->writeInt8( iter->nKnight );
			packet->writeInt64( iter->nFamilyId );
			packet->writeInt8( iter->nQiShi );
		}
	}

	void readPetsInfo( Answer::NetPacket* inPacket )
	{
		if ( NULL == inPacket )
		{
			return;
		}

		DBPet pet = {};
		int32_t nCount = inPacket->readInt32();
		for ( int32_t i = 0; i < nCount; ++i )
		{
			bzero( &pet, sizeof( pet ) );
			pet.nPetId			= inPacket->readInt64();
			pet.nHP				= inPacket->readInt32();
			pet.bAlive			= inPacket->readInt8();
			pet.nSoul			= inPacket->readInt32();
			pet.nState			= inPacket->readInt8();
			pet.nBag			= inPacket->readInt8();
			pet.nKnight			= inPacket->readInt8();
			pet.nFamilyId		= inPacket->readInt64();
			pet.nQiShi			= inPacket->readInt8();

			charPets.push_back( pet );
		}
	}

	void writePetsEggsInfo( Answer::NetPacket* packet )
	{
		if ( NULL == packet )
		{
			return;
		}

		int32_t nCount = charPetEggs.size();
		packet->writeInt32( nCount );
		DBPetEggList::iterator iter = charPetEggs.begin();
		DBPetEggList::iterator eiter = charPetEggs.end();
		for ( ; iter != eiter; ++iter )
		{
			packet->writeInt8( iter->nBag );
			packet->writeInt32( iter->nSlot );
			packet->writeInt32( iter->nBaseId );
			packet->writeInt8( iter->nState );
			packet->writeInt32( iter->nStartTime );
			packet->writeInt64( iter->nPetId );
		}
	}

	void readPetEggsInfo( Answer::NetPacket* inPacket )
	{
		if ( NULL == inPacket )
		{
			return;
		}

		DBPetEgg egg = {};
		int32_t nCount = inPacket->readInt32();
		for ( int32_t i = 0; i < nCount; ++i )
		{
			bzero( &egg, sizeof( egg ) );
			egg.nBag		= inPacket->readInt8();
			egg.nSlot		= inPacket->readInt32();
			egg.nBaseId			= inPacket->readInt32();
			egg.nState		= inPacket->readInt8();
			egg.nStartTime	= inPacket->readInt32();
			egg.nPetId		= static_cast<PetId_t>( inPacket->readInt64() );
			charPetEggs.push_back( egg );
		}
	}
public:
	DBPetList		charPets;
	DBPetEggList	charPetEggs;
};
//========================================================================================================================================

//========================================================================================================================================
class InsidePetDBData : public IDataStruct
{
public:
	InsidePetDBData(){ CleanUp(); }
	virtual ~InsidePetDBData(){}

	void CleanUp()
	{
		nPetId			= 0;
		bStartIllusion	= 0;
		nAttrType		= 0;
		nAttrValue		= 0;
		nOnlineTime		= 0;
		nLeftTime		= 0;
		nBuyExpTimes	= 0;
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof(szSQL)-1, 
			"INSERT INTO `mem_char_inside_pet` (cid,pid,start_flag,attr_type,attr_val,online_time,left_time,buy_exp_times) VALUES (%lld,%lld,%d,%d,%d,%d,%d,%d) on duplicate key update pid=%lld,start_flag=%d,attr_type=%d,attr_val=%d,online_time=%d,left_time=%d,buy_exp_times=%d",
			nCid,
			nPetId,
			bStartIllusion,
			nAttrType,
			nAttrValue,
			nOnlineTime,
			nLeftTime,
			nBuyExpTimes,
			nPetId,
			bStartIllusion,
			nAttrType,
			nAttrValue,
			nOnlineTime,
			nLeftTime,
			nBuyExpTimes
			);
		sqls.push_back( szSQL );
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_char_inside_pet` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );

		while(!result.eof())
		{
			nPetId			= result.getInt64Value("pid");
			bStartIllusion	= result.getIntValue("start_flag");
			nAttrType		= result.getIntValue("attr_type");
			nAttrValue		= result.getIntValue("attr_val");
			nOnlineTime		= result.getIntValue("online_time");
			nLeftTime		= result.getIntValue("left_time");
			nBuyExpTimes	= result.getIntValue("buy_exp_times");

			return true;
		}
		return false;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		if ( NULL == packet )
		{
			return;
		}

		packet->writeInt64( nPetId );
		packet->writeInt8( bStartIllusion );
		packet->writeInt8( nAttrType );
		packet->writeInt32( nAttrValue );
		packet->writeInt32( nOnlineTime );
		packet->writeInt32( nLeftTime );
		packet->writeInt16( nBuyExpTimes );
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		if ( NULL == inPacket )
		{
			return;
		}

		nPetId			= inPacket->readInt64();
		bStartIllusion	= inPacket->readInt8();
		nAttrType		= inPacket->readInt8();
		nAttrValue		= inPacket->readInt32();
		nOnlineTime		= inPacket->readInt32();
		nLeftTime		= inPacket->readInt32();
		nBuyExpTimes	= inPacket->readInt16();
	}

public:
	PetId_t		nPetId;							// ����ID
	int8_t		bStartIllusion;					// ��������״̬
	int8_t		nAttrType;						// ��������
	int32_t		nAttrValue;						// ����ֵ
	int32_t		nOnlineTime;					// ����ʱ��
	int32_t		nLeftTime;						// ������������ʱ�䣨���ٻ��Ƶ�����Ҫ����ʣ��ʱ�䣩
	int16_t		nBuyExpTimes;					// ���չ�����ֵ����
};
//========================================================================================================================================

//========================================================================================================================================
class WorshipDBData : public IDataStruct
{
public:
	WorshipDBData(){ CleanUp(); }
	virtual ~WorshipDBData(){}

	void CleanUp()
	{
		nTimes		= 0;
		strCharList	= "";
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof(szSQL)-1, 
			"INSERT INTO `mem_char_worship` (cid,times,charlist) VALUES (%lld,%d,'%s') on duplicate key update times=%d,charlist='%s'",
			nCid,
			nTimes,
			strCharList.c_str(),
			nTimes,
			strCharList.c_str()
			);
		sqls.push_back( szSQL );
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_char_worship` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );

		while(!result.eof())
		{
			nTimes			= result.getIntValue("times");
			strCharList		= result.getStringValue("charlist");

			return true;
		}
		return false;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		if ( NULL == packet )
		{
			return;
		}

		packet->writeInt32( nTimes );
		packet->writeUTF8( strCharList );
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		if ( NULL == inPacket )
		{
			return;
		}

		nTimes		= inPacket->readInt32();
		strCharList	= inPacket->readUTF8(true);
	}

public:
	int32_t		nTimes;						// Ĥ�ݴ���
	std::string	strCharList;				// Ĥ���б�
};
//========================================================================================================================================

//========================================================================================================================================
class SoulDBData : public IDataStruct
{
public:
	SoulDBData(){ CleanUp(); }
	virtual ~SoulDBData(){}

	void CleanUp()
	{
		nLevel	= 0;
		nSoul	= 0;
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof(szSQL)-1, 
			"INSERT INTO `mem_char_soul` (cid,level,soul) VALUES (%lld,%d,%lld) on duplicate key update level=%d,soul=%lld",
			nCid,
			nLevel,
			nSoul,
			nLevel,
			nSoul
			);
		sqls.push_back( szSQL );
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_char_soul` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );

		while(!result.eof())
		{
			nLevel	= result.getIntValue("level");
			nSoul	= result.getInt64Value("soul");

			return true;
		}
		return false;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		if ( NULL == packet )
		{
			return;
		}

		packet->writeInt32( nLevel );
		packet->writeInt64( nSoul );
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		if ( NULL == inPacket )
		{
			return;
		}

		nLevel	= inPacket->readInt32();
		nSoul	= inPacket->readInt64();
	}

public:
	int32_t	nLevel;				// �ȼ�
	int64_t	nSoul;				// ����
};
//========================================================================================================================================

//========================================================================================================================================
class CharFamilyDBData : public IDataStruct
{
public:
	CharFamilyDBData(){ CleanUp(); }
	virtual ~CharFamilyDBData(){}

	void CleanUp()
	{
		nFamilyId			= 0;
		nPosition			= FP_NONE;
		nContribution		= 0;
		nPetContribution	= 0;
		bzero( vRegFlag, sizeof( vRegFlag ) );
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1,
			"INSERT `mem_chr_family`( cid,family_id,position,contribute,pet_contribute,regflag ) VALUES (%lld,%lld,%d,%d,%d,'%s') ON DUPLICATE KEY UPDATE family_id=%lld,position=%d,contribute=%d,pet_contribute=%d,regflag='%s'",
			nCid,
			nFamilyId,
			nPosition,
			nContribution,
			nPetContribution,
			vRegFlag,
			nFamilyId,
			nPosition,
			nContribution,
			nPetContribution,
			vRegFlag
			);
		sqls.push_back( szSQL );
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_family` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );

		while(!result.eof())
		{
			nFamilyId			= result.getInt64Value("family_id");
			nPosition			= result.getIntValue("position");
			nContribution		= result.getIntValue("contribute");
			nPetContribution	= result.getIntValue("pet_contribute");
			snprintf( vRegFlag, MAX_PET_ID, result.getStringValue("regflag") );

			return true;
		}
		return false;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		if ( NULL == packet )
		{
			return;
		}

		packet->writeInt64( nFamilyId );
		packet->writeInt8( nPosition );
		packet->writeInt32( nContribution );
		packet->writeInt32( nPetContribution );
		packet->writeUTF8( vRegFlag );
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		if ( NULL == inPacket )
		{
			return;
		}

		nFamilyId			= inPacket->readInt64();
		nPosition			= inPacket->readInt8();
		nContribution		= inPacket->readInt32();
		nPetContribution	= inPacket->readInt32();
		snprintf( vRegFlag, MAX_PET_ID, inPacket->readUTF8(true).c_str() );
	}

public:
	FamilyId_t	nFamilyId;
	int8_t		nPosition;
	int32_t		nContribution;
	int32_t		nPetContribution;
	char		vRegFlag[MAX_PET_ID];
};
//========================================================================================================================================

//========================================================================================================================================
class KillMonsterData : public IDataStruct
{
public:
	KillMonsterData(){ CleanUp(); }
	virtual ~KillMonsterData(){}
	void CleanUp()
	{
		m_KillMonsterMap.clear();		
	}
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "DELETE FROM mem_chr_kill_monster WHERE Cid = %d", nCid );
		sqls.push_back( szSQL );
		KillMonsterMap::iterator it = m_KillMonsterMap.begin();
		for ( ; it != m_KillMonsterMap.end(); ++it )
		{
			bzero( szSQL, sizeof( szSQL ) );
			snprintf( szSQL, sizeof( szSQL ) - 1,
				"INSERT INTO `mem_chr_kill_monster` ( Cid, GroupId, MonsterMid, KillCount, BossSign ) VALUES( %lld, %d, %d, %d, %d )",
				nCid,
				it->first,
				it->second.m_MonsterMid,																
				it->second.m_KillCount,								
				it->second.m_BossSign
				);
			sqls.push_back( szSQL );
		}
	}
	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_kill_monster` WHERE `Cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		while(!result.eof())
		{
			KillMonsterInfo stu;
			bzero( &stu, sizeof( stu ) );
			int32_t GroupId  = result.getIntValue("GroupId");
			stu.m_MonsterMid = result.getIntValue("MonsterMid");
			stu.m_KillCount  = result.getIntValue("KillCount");
			stu.m_BossSign	 = (int8_t)result.getIntValue("BossSign");
			m_KillMonsterMap[GroupId] = stu;
			result.nextRow();
		}
		return true;
	}
	virtual void PackageData( Answer::NetPacket* packet )
	{
		int32_t nSize = m_KillMonsterMap.size();
		packet->writeInt32( nSize );
		KillMonsterMap::iterator it = m_KillMonsterMap.begin();
		for ( ; it != m_KillMonsterMap.end(); ++it )
		{
			packet->writeInt32( it->first );
			packet->writeInt32( it->second.m_MonsterMid );
			packet->writeInt32( it->second.m_KillCount );
			packet->writeInt8( it->second.m_BossSign );
		}
	}
	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		m_KillMonsterMap.clear();
		int32_t nSize			= inPacket->readInt32();
		for ( int32_t i = 0; i < nSize; ++i )
		{
			KillMonsterInfo stu;
			bzero( &stu, sizeof( stu ) );
			int32_t GroupId	 = inPacket->readInt32();
			stu.m_MonsterMid = inPacket->readInt32();
			stu.m_KillCount  = inPacket->readInt32();
			stu.m_BossSign	 = inPacket->readInt8();
			m_KillMonsterMap[GroupId] = stu;
		}
	}
public:
	KillMonsterMap m_KillMonsterMap;
};
//=======================================================================================================================================
class ScoreShopData : public IDataStruct
{
public:
	ScoreShopData(){ CleanUp(); }
	virtual ~ScoreShopData(){}
	void CleanUp()
	{
		m_LiMitMap.clear();		
	}
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "DELETE FROM mem_chr_shop_limit WHERE Cid = %d AND ShopType=%d", nCid, ST_SCORE_SHOP );
		sqls.push_back( szSQL );
		ItemLimitMap::iterator it = m_LiMitMap.begin();
		for ( ; it != m_LiMitMap.end(); ++it )
		{
			bzero( szSQL, sizeof( szSQL ) );
			snprintf( szSQL, sizeof( szSQL ) - 1,
				"INSERT INTO `mem_chr_shop_limit` ( Cid,ShopType, ShopId, LimitCount ) VALUES( %lld,%d, %d, %d)",
				nCid,
				ST_SCORE_SHOP,
				it->first,
				it->second
				);
			sqls.push_back( szSQL );
		}
	}
	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_shop_limit` WHERE `Cid`=%lld AND ShopType=%d", nCid,ST_SCORE_SHOP );
		Answer::MySqlQuery result = db.query( szSQL );
		while(!result.eof())
		{
			KillMonsterInfo stu;
			bzero( &stu, sizeof( stu ) );
			int32_t ShopId		 = result.getIntValue("ShopId");
			int32_t LimitCount	 = result.getIntValue("LimitCount");
			m_LiMitMap[ShopId] = LimitCount;
			result.nextRow();
		}
		return true;
	}
	virtual void PackageData( Answer::NetPacket* packet )
	{
		int32_t nSize = m_LiMitMap.size();
		packet->writeInt32( nSize );
		ItemLimitMap::iterator it = m_LiMitMap.begin();
		for ( ; it != m_LiMitMap.end(); ++it )
		{
			packet->writeInt32( it->first );
			packet->writeInt32( it->second );
		}
	}
	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		m_LiMitMap.clear();
		int32_t nSize			= inPacket->readInt32();
		for ( int32_t i = 0; i < nSize; ++i )
		{
			int32_t ShopId		= inPacket->readInt32();
			int32_t LimitCount  = inPacket->readInt32();
			m_LiMitMap[ShopId]  = LimitCount;
		}
	}
public:
	ItemLimitMap m_LiMitMap;
};
//========================================================================================================================================
class CJueWeiData : public IDataStruct
{
public:
	CJueWeiData(){ CleanUp(); }
	virtual ~CJueWeiData(){}
	void CleanUp()
	{
		m_DonateMoney = 0;		
	}
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1,
			"INSERT `mem_chr_jue_wei`(CharId,DonateMoney) VALUES (%lld,%lld) ON DUPLICATE KEY UPDATE DonateMoney=%lld",
			nCid,
			m_DonateMoney,
			m_DonateMoney															
			);
		sqls.push_back( szSQL );
	}
	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_jue_wei` WHERE `CharId`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		while(!result.eof())
		{
			m_DonateMoney= result.getInt64Value("DonateMoney");
			result.nextRow();
		}
		return true;
	}
	virtual void PackageData( Answer::NetPacket* packet )
	{
		packet->writeInt64( m_DonateMoney );
	}
	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		m_DonateMoney = inPacket->readInt64();
	}
public:
	int64_t m_DonateMoney;
};

//========================================================================================================================================
class CHuoYueDuData : public IDataStruct
{
public:
	CHuoYueDuData(){ CleanUp(); }
	virtual ~CHuoYueDuData(){}
	void CleanUp()
	{
		m_HuoYueDuRecord.clear();		
	}
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1,
			"INSERT `mem_chr_huo_yue_du`(Cid,HuoYueDuInfo) VALUES (%lld,'%s') ON DUPLICATE KEY UPDATE HuoYueDuInfo='%s'",
			nCid,
			GetHuoYueDuString().c_str(),
			GetHuoYueDuString().c_str()															
			);
		sqls.push_back( szSQL );
	}
	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_huo_yue_du` WHERE `Cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		while(!result.eof())
		{
			std::string HuoYueDuString = result.getStringValue("HuoYueDuInfo");
			InitHuoYueDu( HuoYueDuString );
			result.nextRow();
		}
		return true;
	}
	virtual void PackageData( Answer::NetPacket* packet )
	{
		int32_t nSize = m_HuoYueDuRecord.size();
		packet->writeInt32( nSize );
		HuoYueDuRecordMap::iterator it = m_HuoYueDuRecord.begin();
		for ( ; it != m_HuoYueDuRecord.end(); ++it )
		{
			packet->writeInt32( it->first );
			packet->writeInt32( it->second.FinishTimes );
			packet->writeInt8( it->second.IsSec );
		}
	}
	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		int32_t nSize = inPacket->readInt32();
		for ( int32_t i = 0; i < nSize ; i++ )
		{
			HuoYueDuRecord stu = {};
			int32_t Index		= inPacket->readInt32();
			stu.FinishTimes		= inPacket->readInt32();
			stu.IsSec			= inPacket->readInt8();
			m_HuoYueDuRecord[Index] = stu;
		}
	}

	void	InitHuoYueDu( std::string HuoYueDuString )
	{
		if ( HuoYueDuString == "" )
		{
			return;
		}
		StringVector mineString = Answer::StringUtility::split( HuoYueDuString, "|");
		StringVector::iterator it = mineString.begin();
		for ( ; it != mineString.end(); ++it )
		{
			StringVector StringVt = Answer::StringUtility::split( *it, ":");
			if ( StringVt.size() == 3 )
			{
				int32_t Index		= atoi( StringVt[0].c_str());
				HuoYueDuRecord stu  = {};
				stu.FinishTimes		= atoi( StringVt[1].c_str());
				stu.IsSec			= atoi( StringVt[2].c_str());
				m_HuoYueDuRecord[Index] =  stu;
			}
		}
	}
	std::string GetHuoYueDuString()
	{
		std::stringstream ss;
		HuoYueDuRecordMap::iterator  it = m_HuoYueDuRecord.begin();
		for ( ; it != m_HuoYueDuRecord.end(); ++it )
		{
			ss	<< it->first
				<< ":" 
				<< it->second.FinishTimes
				<< ":"
				<< (int32_t)it->second.IsSec
				<< "|" ;
		}
		return ss.str();
	}
public:
	HuoYueDuRecordMap m_HuoYueDuRecord;
};
//========================================================================================================================================

class CExpBallData : public IDataStruct
{
public:
	CExpBallData(){ CleanUp(); }
	virtual ~CExpBallData(){}
	void CleanUp()
	{
		m_ItemId	= 0;
		m_CurExp	= 0;
	}
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1,
			"INSERT `mem_chr_exp_ball`(Cid,ItemId,CurExp) VALUES (%lld,%d,%d) ON DUPLICATE KEY UPDATE ItemId=%d,CurExp=%d",
			nCid,
			m_ItemId,															
			m_CurExp,
			m_ItemId,
			m_CurExp
			);
		sqls.push_back( szSQL );
	}
	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_exp_ball` WHERE `Cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		while(!result.eof())
		{
			m_ItemId = result.getIntValue("ItemId");
			m_CurExp = result.getIntValue("CurExp"); 
			result.nextRow();
		}
		return true;
	}
	virtual void PackageData( Answer::NetPacket* packet )
	{
		packet->writeInt32( m_ItemId );
		packet->writeInt32( m_CurExp );
	}
	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		m_ItemId = inPacket->readInt32();
		m_CurExp = inPacket->readInt32();
	}
public:
	int32_t		m_ItemId;
	int32_t		m_CurExp;
};
//========================================================================================================================================
class ShangChengData : public IDataStruct
{
public:
	ShangChengData(){ CleanUp(); }
	virtual ~ShangChengData(){}
	void CleanUp()
	{
		m_LiMitMap.clear();		
	}
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "DELETE FROM mem_chr_shop_limit WHERE Cid = %lld AND ShopType=%d", nCid, ST_GAME_SHOP );
		sqls.push_back( szSQL );
		ItemLimitMap::iterator it = m_LiMitMap.begin();
		for ( ; it != m_LiMitMap.end(); ++it )
		{
			bzero( szSQL, sizeof( szSQL ) );
			snprintf( szSQL, sizeof( szSQL ) - 1,
				"INSERT INTO `mem_chr_shop_limit` ( Cid,ShopType, ShopId, LimitCount ) VALUES( %lld,%d, %d, %d)",
				nCid,
				ST_GAME_SHOP,
				it->first,
				it->second
				);
			sqls.push_back( szSQL );
		}
	}
	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_shop_limit` WHERE `Cid`=%lld AND ShopType=%d", nCid,ST_GAME_SHOP );
		Answer::MySqlQuery result = db.query( szSQL );
		while(!result.eof())
		{
			int32_t ShopId		 = result.getIntValue("ShopId");
			int32_t LimitCount	 = result.getIntValue("LimitCount");
			m_LiMitMap[ShopId] = LimitCount;
			result.nextRow();
		}
		return true;
	}
	virtual void PackageData( Answer::NetPacket* packet )
	{
		int32_t nSize = m_LiMitMap.size();
		packet->writeInt32( nSize );
		ItemLimitMap::iterator it = m_LiMitMap.begin();
		for ( ; it != m_LiMitMap.end(); ++it )
		{
			packet->writeInt32( it->first );
			packet->writeInt32( it->second );
		}
	}
	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		m_LiMitMap.clear();
		int32_t nSize			= inPacket->readInt32();
		for ( int32_t i = 0; i < nSize; ++i )
		{
			int32_t ShopId		= inPacket->readInt32();
			int32_t LimitCount  = inPacket->readInt32();
			m_LiMitMap[ShopId]  = LimitCount;
		}
	}
public:
	ItemLimitMap m_LiMitMap;
};
//========================================================================================================================================
class AchievementData : public IDataStruct
{
public:
	AchievementData(){ CleanUp(); }
	virtual ~AchievementData(){}
	void CleanUp()
	{
		m_AchievementMap.clear();		
	}
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "DELETE FROM mem_achievement WHERE Cid = %lld ", nCid );
		sqls.push_back( szSQL );
		AchievementMap::iterator it = m_AchievementMap.begin();
		for ( ; it != m_AchievementMap.end(); ++it )
		{
			bzero( szSQL, sizeof( szSQL ) );
			snprintf( szSQL, sizeof( szSQL ) - 1,
				"INSERT INTO `mem_achievement` ( Cid,Id, FinishCount, AcState,Time ) VALUES( %lld,%d, %d, %d, %d)",
				nCid,
				it->first,
				it->second.FinishCount,
				it->second.AcState,
				it->second.Time
				);
			sqls.push_back( szSQL );
		}
	}
	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_achievement` WHERE `Cid`=%lld ", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		while(!result.eof())
		{
			AchievementState stu = {};
			stu.Index			 = result.getIntValue("Id");
			stu.FinishCount		 = result.getIntValue("FinishCount");
			stu.AcState			 = (int8_t)result.getIntValue("AcState");
			stu.Time			 = result.getIntValue("Time");
			m_AchievementMap[stu.Index] = stu;
			result.nextRow();
		}
		return true;
	}
	virtual void PackageData( Answer::NetPacket* packet )
	{
		int32_t nSize = m_AchievementMap.size();
		packet->writeInt32( nSize );
		AchievementMap::iterator it = m_AchievementMap.begin();
		for ( ; it != m_AchievementMap.end(); ++it )
		{
			packet->writeInt32( it->first );
			packet->writeInt32( it->second.FinishCount );
			packet->writeInt8( it->second.AcState );
			packet->writeInt32( it->second.Time );
		}
	}
	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		m_AchievementMap.clear();
		int32_t nSize			= inPacket->readInt32();
		for ( int32_t i = 0; i < nSize; ++i )
		{
			AchievementState stu = {};
			stu.Index			 = inPacket->readInt32();
			stu.FinishCount		 = inPacket->readInt32();
			stu.AcState			 = inPacket->readInt8();
			stu.Time			 = inPacket->readInt32();
			m_AchievementMap[stu.Index] = stu;
		}
	}
public:
	AchievementMap	m_AchievementMap;
};
//========================================================================================================================================
//========================================================================================================================================
class MemChrTitleData : public IDataStruct
{
public:
	MemChrTitleData(){ CleanUp(); }
	virtual ~MemChrTitleData(){}

	void CleanUp()
	{
		m_vTitles.clear();
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "DELETE FROM `mem_chr_title` WHERE `cid`=%lld", nCid );
		sqls.push_back( szSQL );

		int32_t vtSize = m_vTitles.size();
		for ( int32_t i = 0; i < vtSize; ++i )
		{
			bzero( szSQL, sizeof( szSQL ) );
			snprintf( szSQL, sizeof( szSQL ) - 1,
				"INSERT INTO `mem_chr_title` (`cid`, `type`, `value`, `expire_time`) VALUES (%lld, %d, %d, %d)",
				nCid,
				m_vTitles[i].nType,
				m_vTitles[i].nValue,
				m_vTitles[i].nExpireTime );
			sqls.push_back( szSQL );
		}
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof( szSQL ) - 1, "SELECT * FROM `mem_chr_title` WHERE `cid`=%lld", nCid );
		Answer::MySqlQuery result = db.query( szSQL );
		m_vTitles.clear();
		m_vTitles.reserve( result.getRowCount() );
		while ( !result.eof() )
		{
			MemChrTitle title = {};
			title.nType		= result.getIntValue("type");
			title.nValue		= result.getIntValue("value");
			title.nExpireTime	= result.getIntValue("expire_time");
			m_vTitles.push_back( title );
			result.nextRow();
		}
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		int32_t vtSize = m_vTitles.size();
		packet->writeInt32( vtSize );
		for ( int32_t i = 0; i < vtSize; ++i )
		{
			packet->writeInt32( m_vTitles[i].nType );
			packet->writeInt32( m_vTitles[i].nValue );
			packet->writeInt32( m_vTitles[i].nExpireTime );
		}
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{
		int32_t vtSize = inPacket->readInt32();
		m_vTitles.clear();
		m_vTitles.reserve( vtSize );
		for ( int32_t i = 0; i < vtSize; ++i )
		{
			MemChrTitle title = {};
			title.nType		= inPacket->readInt32();
			title.nValue		= inPacket->readInt32();
			title.nExpireTime	= inPacket->readInt32();
			m_vTitles.push_back( title );
		}
	}

public:
	struct MemChrTitle { int32_t nType; int32_t nValue; int32_t nExpireTime; };
	std::vector<MemChrTitle>	m_vTitles;
};
class PlayerDBData : public IDataStruct
{
public:
	PlayerDBData(){ CleanUp(); }
	virtual ~PlayerDBData(){}

	void CleanUp()
	{
		cgindex					= 0;
		loadTime				= 0;
		todayGoldCharge			= 0;
		firstFiveDayGoldCharge	= 0;
		saveRefCount			= 0;

		chr.CleanUp();
		sysUser.CleanUp();
		signInfo.CleanUp();
		equipData.CleanUp();
		gemData.CleanUp();
		bagData.CleanUp();
		skillData.CleanUp();
		taskData.CleanUp();
		taskCycleData.CleanUp();
		actionData.CleanUp();
		autoFight.CleanUp();
		systemSetting.CleanUp();
		buffData.CleanUp();
		gambleDepot.CleanUp();
		mysVector.CleanUp();
		sysUserPreventWallow.CleanUp();

		m_CurrencyData.CleanUp();
		m_OperateLimit.CleanUp();
		m_CharPets.CleanUp();
		m_CharInsidePet.CleanUp();
		m_MountDBData.CleanUp();
		m_KillMonsterData.CleanUp();
		m_FriendExpReward.CleanUp();
		m_MailDBData.CleanUp();
		m_ShangChengData.CleanUp();
		m_FaBaoData.CleanUp();
		m_FamilyData.CleanUp();
		m_WorshipData.CleanUp();
		m_JueWeiData.CleanUp();
		m_ChouJinagData.CleanUp();
		m_SoulData.CleanUp();
		m_ScoreShopData.CleanUp();
		m_ExpBallData.CleanUp();
		m_TouZiData.CleanUp();
		m_HuoYueDuData.CleanUp();
		m_AchievementData.CleanUp();
		m_TitleData.CleanUp();
	}

public:
	virtual void SaveToSqlString( SqlStringList& sqls, char (&szSQL)[MAX_SQL_LENGTH], CharId_t nCid = 0 )
	{
		chr.SaveToSqlString( sqls, szSQL, nCid );
		attrData.SaveToSqlString( sqls, szSQL, nCid );
		sysUser.SaveToSqlString( sqls, szSQL, nCid );
		sysUserPreventWallow.SaveToSqlString( sqls, szSQL, nCid );
		signInfo.SaveToSqlString( sqls, szSQL, nCid );
		equipData.SaveToSqlString( sqls, szSQL, nCid );
		gemData.SaveToSqlString( sqls, szSQL, nCid );
		bagData.SaveToSqlString( sqls, szSQL, nCid );
		gambleDepot.SaveToSqlString( sqls, szSQL, nCid );
		skillData.SaveToSqlString( sqls, szSQL, nCid );
		taskData.SaveToSqlString( sqls, szSQL, nCid );
		taskCycleData.SaveToSqlString( sqls, szSQL, nCid );
		actionData.SaveToSqlString( sqls, szSQL, nCid );
		autoFight.SaveToSqlString( sqls, szSQL, nCid );
		systemSetting.SaveToSqlString( sqls, szSQL, nCid );
		buffData.SaveToSqlString( sqls, szSQL, nCid );

		m_CurrencyData.SaveToSqlString( sqls, szSQL, nCid );
		m_OperateLimit.SaveToSqlString( sqls, szSQL, nCid );
		m_CharPets.SaveToSqlString( sqls, szSQL, nCid );
		m_CharInsidePet.SaveToSqlString( sqls, szSQL, nCid );
		m_MountDBData.SaveToSqlString( sqls, szSQL, nCid );
		m_KillMonsterData.SaveToSqlString( sqls, szSQL, nCid );
		m_FriendExpReward.SaveToSqlString( sqls, szSQL, nCid );
		m_MailDBData.SaveToSqlString( sqls, szSQL, nCid );
		m_ShangChengData.SaveToSqlString(sqls,szSQL,nCid );
		m_FaBaoData.SaveToSqlString( sqls, szSQL, nCid );
		m_FamilyData.SaveToSqlString( sqls, szSQL, nCid );
		m_WorshipData.SaveToSqlString( sqls, szSQL, nCid );
		m_JueWeiData.SaveToSqlString( sqls, szSQL, nCid );
		m_ChouJinagData.SaveToSqlString( sqls, szSQL, nCid );
		m_SoulData.SaveToSqlString( sqls, szSQL, nCid );
		m_ScoreShopData.SaveToSqlString(sqls,szSQL,nCid );
		m_ExpBallData.SaveToSqlString(sqls,szSQL,nCid );
		m_TouZiData.SaveToSqlString(sqls,szSQL,nCid );
		m_HuoYueDuData.SaveToSqlString(sqls,szSQL,nCid);
		m_AchievementData.SaveToSqlString( sqls, szSQL, nCid );
		m_TitleData.SaveToSqlString( sqls, szSQL, nCid );
	}

	virtual bool LoadFromDB( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		loadTime				= Answer::DayTime::now();
		todayGoldCharge			= 0;
		firstFiveDayGoldCharge	= 0;

		if ( !chr.LoadFromDB( db, szSQL, nUid, nSid ) )
		{
			return false;
		}
		nCid = chr.data.cid;

		if ( sysUser.LoadFromDB( db, szSQL, nUid, nSid ) )
		{
			sysUserPreventWallow.LoadFromDB( db, szSQL, nUid, nSid );
		}
		LoadPayLog( db, szSQL, nUid, nSid );

		gambleDepot.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		signInfo.LoadFromDB( db, szSQL, nUid, nSid, nCid );

		equipData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		gemData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		bagData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		skillData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		taskData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		taskCycleData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		actionData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		autoFight.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		systemSetting.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		buffData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		//equips.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		mysVector.LoadFromDB( db, szSQL, nUid, nSid, nCid );

		m_CurrencyData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		m_OperateLimit.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		m_CharPets.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		m_CharInsidePet.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		m_MountDBData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		m_KillMonsterData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		m_FriendExpReward.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		m_MailDBData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		m_ShangChengData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		m_FaBaoData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		m_FamilyData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		m_WorshipData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		m_JueWeiData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		m_ChouJinagData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		m_SoulData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		m_ScoreShopData.LoadFromDB(db,szSQL,nUid,nSid,nCid);
		m_ExpBallData.LoadFromDB(db,szSQL,nUid,nSid,nCid);
		m_TouZiData.LoadFromDB(db,szSQL,nUid,nSid,nCid);
		m_HuoYueDuData.LoadFromDB(db,szSQL,nUid,nSid,nCid);
		m_AchievementData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		m_TitleData.LoadFromDB( db, szSQL, nUid, nSid, nCid );
		return true;
	}

	virtual void PackageData( Answer::NetPacket* packet )
	{
		packet->writeInt16( cgindex );
		packet->writeInt32( loadTime );
		packet->writeInt32( todayGoldCharge );
		packet->writeInt32( firstFiveDayGoldCharge );

		chr.PackageData( packet );

		attrData.PackageData( packet );
		gambleDepot.PackageData( packet );
		sysUser.PackageData( packet );
		sysUserPreventWallow.PackageData( packet );
		signInfo.PackageData( packet );
		equipData.PackageData( packet );
		gemData.PackageData( packet );
		bagData.PackageData( packet );
		skillData.PackageData( packet );
		taskData.PackageData( packet );
		taskCycleData.PackageData( packet );
		actionData.PackageData( packet );
		autoFight.PackageData( packet );
		systemSetting.PackageData( packet );
		buffData.PackageData( packet );
		//equips.PackageData( packet );
		mysVector.PackageData( packet );

		m_CurrencyData.PackageData( packet );
		m_OperateLimit.PackageData( packet );
		m_CharPets.PackageData( packet );
		m_CharInsidePet.PackageData( packet );
		m_MountDBData.PackageData( packet );
		m_KillMonsterData.PackageData( packet );
		m_FriendExpReward.PackageData( packet );
		m_MailDBData.PackageData( packet );
		m_ShangChengData.PackageData( packet );
		m_FaBaoData.PackageData( packet );
		m_FamilyData.PackageData( packet );
		m_WorshipData.PackageData( packet );
		m_JueWeiData.PackageData( packet );
		m_ChouJinagData.PackageData( packet );
		m_SoulData.PackageData( packet );
		m_ScoreShopData.PackageData( packet);
		m_ExpBallData.PackageData( packet );
		m_TouZiData.PackageData( packet );
		m_HuoYueDuData.PackageData(packet);
		m_AchievementData.PackageData(packet);
		m_TitleData.PackageData( packet );
	}

	virtual void UnPackageData( Answer::NetPacket* inPacket, CharId_t nCid = 0 )
	{

		cgindex					= inPacket->readInt16();
		loadTime				= inPacket->readInt32();
		todayGoldCharge			= inPacket->readInt32();
		firstFiveDayGoldCharge	= inPacket->readInt32();

		chr.UnPackageData( inPacket );
		nCid = chr.data.cid;

		attrData.UnPackageData( inPacket, nCid );
		gambleDepot.UnPackageData( inPacket, nCid );
		sysUser.UnPackageData( inPacket, nCid );
		sysUserPreventWallow.UnPackageData( inPacket, nCid );
		signInfo.UnPackageData( inPacket, nCid );
		equipData.UnPackageData( inPacket, nCid );
		gemData.UnPackageData( inPacket, nCid );
		bagData.UnPackageData( inPacket, nCid );
		skillData.UnPackageData( inPacket, nCid );
		taskData.UnPackageData( inPacket, nCid );
		taskCycleData.UnPackageData( inPacket, nCid );
		actionData.UnPackageData( inPacket, nCid );
		autoFight.UnPackageData( inPacket, nCid );
		systemSetting.UnPackageData( inPacket, nCid );
		buffData.UnPackageData( inPacket, nCid );
		//equips.UnPackageData( inPacket, charId );
		mysVector.UnPackageData( inPacket, nCid );

		m_CurrencyData.UnPackageData( inPacket, nCid );
		m_OperateLimit.UnPackageData( inPacket, nCid );
		m_CharPets.UnPackageData( inPacket, nCid );
		m_CharInsidePet.UnPackageData( inPacket, nCid );
		m_MountDBData.UnPackageData( inPacket, nCid );
		m_KillMonsterData.UnPackageData( inPacket, nCid );
		m_FriendExpReward.UnPackageData( inPacket, nCid );
		m_MailDBData.UnPackageData( inPacket, nCid );
		m_ShangChengData.UnPackageData( inPacket, nCid );
		m_FaBaoData.UnPackageData( inPacket, nCid );
		m_FamilyData.UnPackageData( inPacket, nCid );
		m_WorshipData.UnPackageData( inPacket, nCid );
		m_JueWeiData.UnPackageData( inPacket, nCid );
		m_ChouJinagData.UnPackageData( inPacket, nCid );
		m_SoulData.UnPackageData( inPacket, nCid );
		m_ScoreShopData.UnPackageData( inPacket, nCid );
		m_ExpBallData.UnPackageData( inPacket, nCid );
		m_TouZiData.UnPackageData( inPacket, nCid );
		m_HuoYueDuData.UnPackageData(inPacket,nCid );
		m_AchievementData.UnPackageData(inPacket,nCid);
		m_TitleData.UnPackageData(inPacket, nCid);
	}

	void LoadUseSpecialInfo( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid, CharId_t nCid = 0 )
	{
		sysUser.LoadFromDB( db, szSQL, nUid, nSid );
	}
private:
	bool LoadPayLog( Answer::MySqlDBGuard& db, char (&szSQL)[MAX_SQL_LENGTH], int32_t nUid, int32_t nSid )
	{
		bzero( szSQL, sizeof( szSQL ) );
		snprintf( szSQL, sizeof(szSQL)-1, "%s", "SELECT `value` FROM `sys_server_config` WHERE `name`='startGame_time'" );
		Answer::MySqlQuery result = db.query( szSQL );
		if (!result.eof())
		{
			int32_t serverStartDay = Answer::DayTime::dayzero(atoi(result.getStringValue("value")));
			bzero( szSQL, sizeof(szSQL) );
			snprintf( szSQL, sizeof(szSQL)-1, "SELECT sum(`amount`) FROM `pay_log` WHERE `uid`=%d AND `sid`=%d AND `time`<=%d", nUid, nSid, serverStartDay + 5 * 24 * 60 * 60);
			result = db.query(szSQL);
			if (!result.eof())
			{
				firstFiveDayGoldCharge = result.getIntValue(0);
			}

			int32_t todayZero = Answer::DayTime::dayzero( loadTime );
			bzero( szSQL, sizeof(szSQL) );
			snprintf( szSQL, sizeof(szSQL)-1, "SELECT sum(`amount`) FROM `pay_log` WHERE `uid`=%d AND `sid`=%d AND `time`>=%d AND `time`<=%d", nUid, nSid, todayZero, todayZero + 24 * 60 * 60 );
			result = db.query( szSQL );
			if ( !result.eof() )
			{
				todayGoldCharge = result.getIntValue(0);
			}
			return true;
		}
		return false;
	}

public:
	int16_t						cgindex;
	int32_t						loadTime;
	int32_t						todayGoldCharge;
	int32_t						firstFiveDayGoldCharge;
	int8_t						saveRefCount;

	MemCharacterData			chr;
	SysUserData					sysUser;
	DailyActivityData			signInfo;
	MemChrEquipData				equipData; 
	MemChrGemData				gemData; 
	MemChrBagData				bagData;
	MemChrSkillData				skillData;
	MemChrTaskData				taskData;
	MemChrTaskCycleData			taskCycleData;
	MemChrActionData			actionData;
	MemChrAutoFightData			autoFight;
	MemChrSystemSettingData		systemSetting;
	MemChrBuffData				buffData;
	//MemEquipData				equips;
	MemChrDepotData				gambleDepot;
	MemYellowStoneData			mysVector;
	SysUserPreventWallowData	sysUserPreventWallow;
	MemAttrData					attrData;
	CurrencyDBData				m_CurrencyData;
	OperateLimitDBData			m_OperateLimit;		
	PetDBData					m_CharPets;	
	InsidePetDBData				m_CharInsidePet;
	MountDBData					m_MountDBData;
	KillMonsterData				m_KillMonsterData;
	FriendExpReward				m_FriendExpReward;
	MailDBData					m_MailDBData;
	ShangChengData			    m_ShangChengData;
	CFaBaoData					m_FaBaoData;
	CharFamilyDBData			m_FamilyData;
	WorshipDBData				m_WorshipData;
	CJueWeiData					m_JueWeiData;
	ChouJiangData				m_ChouJinagData;
	SoulDBData					m_SoulData;
	ScoreShopData				m_ScoreShopData;
	CExpBallData			    m_ExpBallData;
	TouZiData					m_TouZiData;
	CHuoYueDuData				m_HuoYueDuData;
	AchievementData				m_AchievementData;
	MemChrTitleData				m_TitleData;
};

struct PlayerDBSql 
{
	PlayerDBSql( CharId_t cid )
		: backId( cid )
	{
	}
	SqlStringList	sqls;
	CharId_t		backId;
};

#endif	//__DATA_STRUCT_H__
