/*
Navicat MySQL Data Transfer

Source Server         : 神龙无双
Source Server Version : 50644
Source Host           : 114.67.93.199:3306
Source Database       : syz_123yx_6

Target Server Type    : MYSQL
Target Server Version : 50644
File Encoding         : 65001

Date: 2019-12-05 16:28:21
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for `gmt_ban_chat`
-- ----------------------------
DROP TABLE IF EXISTS `gmt_ban_chat`;
CREATE TABLE `gmt_ban_chat` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '用户ID',
  `expire_time` int(11) NOT NULL DEFAULT '0' COMMENT '到期时间',
  `operator` varchar(20) NOT NULL,
  `reason` varchar(100) NOT NULL,
  `op_time` int(11) NOT NULL,
  `flag` tinyint(4) NOT NULL DEFAULT '0' COMMENT '标记',
  PRIMARY KEY (`cid`),
  KEY `cid` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of gmt_ban_chat
-- ----------------------------

-- ----------------------------
-- Table structure for `gmt_broadcast`
-- ----------------------------
DROP TABLE IF EXISTS `gmt_broadcast`;
CREATE TABLE `gmt_broadcast` (
  `idx` int(11) NOT NULL AUTO_INCREMENT COMMENT '序列',
  `text` varchar(200) NOT NULL DEFAULT '' COMMENT '内容',
  `type` tinyint(4) NOT NULL DEFAULT '0' COMMENT '类型',
  `start_time` int(11) NOT NULL DEFAULT '0' COMMENT '发送时间',
  `interval` int(11) NOT NULL DEFAULT '0' COMMENT '时间（秒）',
  `times` int(11) NOT NULL DEFAULT '0' COMMENT '次数',
  `creater` varchar(20) NOT NULL DEFAULT '' COMMENT '创建者',
  `create_time` int(11) NOT NULL DEFAULT '0' COMMENT '创建时间',
  `flag` tinyint(4) NOT NULL DEFAULT '0' COMMENT '发送标记',
  PRIMARY KEY (`idx`),
  KEY `idx` (`idx`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of gmt_broadcast
-- ----------------------------

-- ----------------------------
-- Table structure for `gmt_family`
-- ----------------------------
DROP TABLE IF EXISTS `gmt_family`;
CREATE TABLE `gmt_family` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '用户ID',
  `operator_time` int(11) NOT NULL DEFAULT '0' COMMENT '到期时间',
  `operator` tinyint(4) NOT NULL,
  `family_id` bigint(20) NOT NULL,
  `user` varchar(20) NOT NULL,
  `param` varchar(200) NOT NULL,
  `flag` tinyint(4) NOT NULL DEFAULT '0' COMMENT '标记',
  PRIMARY KEY (`id`),
  KEY `cid` (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of gmt_family
-- ----------------------------

-- ----------------------------
-- Table structure for `gmt_key_card`
-- ----------------------------
DROP TABLE IF EXISTS `gmt_key_card`;
CREATE TABLE `gmt_key_card` (
  `key` varchar(40) NOT NULL DEFAULT '' COMMENT '卡号',
  `item_id` int(11) NOT NULL DEFAULT '0' COMMENT '兑换礼包ID',
  `passport` varchar(40) NOT NULL DEFAULT '' COMMENT '领取者ID',
  `sid` int(11) NOT NULL DEFAULT '0' COMMENT '服务器ID',
  `state` tinyint(4) NOT NULL DEFAULT '0' COMMENT '状态 0 未领取 1 领取',
  `get_time` int(11) NOT NULL DEFAULT '0' COMMENT '兑换时间',
  `exc_time` int(11) NOT NULL DEFAULT '0' COMMENT '兑换时间',
  PRIMARY KEY (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of gmt_key_card
-- ----------------------------

-- ----------------------------
-- Table structure for `gmt_kick`
-- ----------------------------
DROP TABLE IF EXISTS `gmt_kick`;
CREATE TABLE `gmt_kick` (
  `index` int(11) NOT NULL AUTO_INCREMENT COMMENT '标记',
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '用户ID',
  `operator` varchar(20) NOT NULL,
  `reason` varchar(100) NOT NULL,
  `op_time` int(11) NOT NULL,
  `flag` tinyint(4) NOT NULL DEFAULT '0' COMMENT '标记',
  PRIMARY KEY (`index`),
  KEY `flag` (`flag`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of gmt_kick
-- ----------------------------

-- ----------------------------
-- Table structure for `gmt_recharge`
-- ----------------------------
DROP TABLE IF EXISTS `gmt_recharge`;
CREATE TABLE `gmt_recharge` (
  `index` int(11) NOT NULL AUTO_INCREMENT,
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '角色ID',
  `gold` int(11) NOT NULL DEFAULT '0' COMMENT '冲值金额',
  `start_time` int(11) NOT NULL DEFAULT '0' COMMENT '开始时间',
  `end_time` int(11) NOT NULL DEFAULT '0' COMMENT '截止时间',
  `flag` int(11) NOT NULL DEFAULT '0' COMMENT '审批',
  PRIMARY KEY (`index`),
  KEY `start` (`start_time`) USING BTREE,
  KEY `end` (`end_time`) USING BTREE,
  KEY `flag` (`flag`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of gmt_recharge
-- ----------------------------

-- ----------------------------
-- Table structure for `gmt_seal`
-- ----------------------------
DROP TABLE IF EXISTS `gmt_seal`;
CREATE TABLE `gmt_seal` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '用户ID',
  `expire_time` int(11) NOT NULL DEFAULT '0' COMMENT '到期时间',
  `operator` varchar(20) NOT NULL,
  `reason` varchar(100) NOT NULL,
  `op_time` int(11) NOT NULL,
  `flag` tinyint(4) NOT NULL DEFAULT '0' COMMENT '标记',
  `seal_type` int(11) NOT NULL,
  `type` tinyint(4) DEFAULT NULL,
  `times` int(11) DEFAULT '1' COMMENT '封禁次数,自动封',
  PRIMARY KEY (`cid`),
  KEY `cid` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of gmt_seal
-- ----------------------------

-- ----------------------------
-- Table structure for `info_for_360`
-- ----------------------------
DROP TABLE IF EXISTS `info_for_360`;
CREATE TABLE `info_for_360` (
  `uid` bigint(20) NOT NULL DEFAULT '0' COMMENT '用户ID',
  `sid` int(11) NOT NULL DEFAULT '0' COMMENT '到期时间',
  `vip_type` tinyint(11) NOT NULL DEFAULT '0' COMMENT '=0非会员，=1月费会员，=2年费会员',
  `vip_level` int(11) NOT NULL DEFAULT '0',
  `sw_vip_level` int(11) NOT NULL DEFAULT '0' COMMENT '顺网vip等级',
  `yy_level` int(11) NOT NULL DEFAULT '0' COMMENT 'yy等级',
  `yy_vip_level` int(11) NOT NULL DEFAULT '0' COMMENT 'yyvip等级',
  `yy_super_level` int(11) NOT NULL DEFAULT '0' COMMENT 'yy超级vip等级',
  `sg_hall_level` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '搜狗大厅等级',
  `sg_pf` varchar(20) NOT NULL DEFAULT '' COMMENT 'pc=微端 web=网页 hz=大厅 skin=皮肤',
  `xl_nx_level` int(11) DEFAULT NULL COMMENT '迅雷牛x等级',
  `platform_vip` int(11) unsigned DEFAULT NULL,
  `platform_super_vip` int(11) DEFAULT NULL,
  PRIMARY KEY (`uid`,`sid`),
  KEY `uid_sid` (`uid`,`sid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of info_for_360
-- ----------------------------

-- ----------------------------
-- Table structure for `log_abnormal`
-- ----------------------------
DROP TABLE IF EXISTS `log_abnormal`;
CREATE TABLE `log_abnormal` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `name` varchar(20) NOT NULL DEFAULT '0',
  `type` int(11) NOT NULL DEFAULT '0' COMMENT '=1单笔获得,=2今天累计获得,=3今日累计消费',
  `value` int(11) NOT NULL DEFAULT '0',
  `opway` int(11) NOT NULL,
  `time` int(11) DEFAULT NULL,
  `flag` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `cidflag` (`cid`,`name`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=130 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_abnormal
-- ----------------------------

-- ----------------------------
-- Table structure for `log_activity`
-- ----------------------------
DROP TABLE IF EXISTS `log_activity`;
CREATE TABLE `log_activity` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '序号',
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `actid` int(11) NOT NULL DEFAULT '0' COMMENT '活动ID',
  `acttype` tinyint(4) NOT NULL DEFAULT '0' COMMENT '活动类型',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '参与时间',
  `param` bigint(20) NOT NULL DEFAULT '0' COMMENT '附加参数\r\n军团战 帮派ID\r\n答题 名次\r\nVIP挂机 地图ID',
  PRIMARY KEY (`id`),
  KEY `id` (`id`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=76 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_activity
-- ----------------------------

-- ----------------------------
-- Table structure for `log_boss`
-- ----------------------------
DROP TABLE IF EXISTS `log_boss`;
CREATE TABLE `log_boss` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '自增长标记',
  `mid` int(11) NOT NULL DEFAULT '0' COMMENT '怪物表ID',
  `type` int(11) NOT NULL DEFAULT '0' COMMENT '类型',
  `killer` bigint(20) NOT NULL DEFAULT '0' COMMENT '击杀者',
  `mapid` int(11) DEFAULT NULL,
  `map_monster_id` int(11) DEFAULT NULL,
  `entity_id` bigint(20) DEFAULT NULL,
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '击杀时间',
  PRIMARY KEY (`id`),
  KEY `id` (`id`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_boss
-- ----------------------------

-- ----------------------------
-- Table structure for `log_change_map`
-- ----------------------------
DROP TABLE IF EXISTS `log_change_map`;
CREATE TABLE `log_change_map` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `name` varchar(20) NOT NULL DEFAULT '0',
  `cur_mapid` int(11) NOT NULL,
  `cur_x` int(11) NOT NULL,
  `cur_y` int(11) NOT NULL,
  `tar_mapid` int(11) NOT NULL,
  `tar_x` int(11) NOT NULL,
  `tar_y` int(11) NOT NULL,
  `time` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `id` (`id`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_change_map
-- ----------------------------

-- ----------------------------
-- Table structure for `log_civil_position_change`
-- ----------------------------
DROP TABLE IF EXISTS `log_civil_position_change`;
CREATE TABLE `log_civil_position_change` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `name` char(20) NOT NULL DEFAULT '',
  `kcid` bigint(20) NOT NULL,
  `kname` char(20) NOT NULL,
  `kingdom` int(11) NOT NULL,
  `old_position` int(11) NOT NULL DEFAULT '0',
  `new_position` int(11) NOT NULL DEFAULT '0',
  `time` int(11) NOT NULL DEFAULT '0',
  KEY `cid` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_civil_position_change
-- ----------------------------

-- ----------------------------
-- Table structure for `log_create_process`
-- ----------------------------
DROP TABLE IF EXISTS `log_create_process`;
CREATE TABLE `log_create_process` (
  `uid` bigint(20) DEFAULT NULL,
  `process` int(11) DEFAULT NULL COMMENT '鎵撶偣浣嶇疆',
  `time` int(11) DEFAULT NULL COMMENT '鎵撶偣鏃堕棿',
  `isWeiduan` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_create_process
-- ----------------------------

-- ----------------------------
-- Table structure for `log_currency`
-- ----------------------------
DROP TABLE IF EXISTS `log_currency`;
CREATE TABLE `log_currency` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '序列',
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '角色id',
  `name` char(12) NOT NULL DEFAULT '' COMMENT '角色名',
  `type` int(11) NOT NULL,
  `opway` int(11) NOT NULL DEFAULT '0' COMMENT '原因',
  `value` int(11) NOT NULL DEFAULT '0' COMMENT '变化值',
  `param` bigint(20) NOT NULL DEFAULT '0',
  `left` bigint(20) NOT NULL,
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '时间',
  `fuli` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `id` (`id`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_currency
-- ----------------------------

-- ----------------------------
-- Table structure for `log_daily_gift`
-- ----------------------------
DROP TABLE IF EXISTS `log_daily_gift`;
CREATE TABLE `log_daily_gift` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '序号',
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `giftid` int(11) NOT NULL DEFAULT '0' COMMENT '礼包ID',
  `type` tinyint(4) NOT NULL DEFAULT '0' COMMENT '礼包类型',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '领取时间',
  PRIMARY KEY (`id`),
  KEY `id` (`id`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_daily_gift
-- ----------------------------

-- ----------------------------
-- Table structure for `log_day_report`
-- ----------------------------
DROP TABLE IF EXISTS `log_day_report`;
CREATE TABLE `log_day_report` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '序列',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '日期',
  `type` int(11) NOT NULL DEFAULT '0' COMMENT '类型',
  `value` int(11) NOT NULL DEFAULT '0' COMMENT '值',
  `ratio` int(11) NOT NULL DEFAULT '0' COMMENT '比率',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_day_report
-- ----------------------------

-- ----------------------------
-- Table structure for `log_die`
-- ----------------------------
DROP TABLE IF EXISTS `log_die`;
CREATE TABLE `log_die` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '自增长标记',
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '角色id',
  `mapid` int(11) NOT NULL DEFAULT '0' COMMENT '地图id',
  `y` int(11) DEFAULT NULL,
  `x` int(11) DEFAULT NULL,
  `killer` bigint(20) NOT NULL DEFAULT '0' COMMENT '击杀者',
  `type` int(11) DEFAULT NULL,
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '击杀时间',
  PRIMARY KEY (`id`),
  KEY `id` (`id`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_die
-- ----------------------------

-- ----------------------------
-- Table structure for `log_dungeon`
-- ----------------------------
DROP TABLE IF EXISTS `log_dungeon`;
CREATE TABLE `log_dungeon` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `did` int(11) NOT NULL DEFAULT '0' COMMENT '副本ID',
  `type` tinyint(4) NOT NULL DEFAULT '0' COMMENT '副本类型',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT '等级',
  `state` tinyint(4) NOT NULL DEFAULT '0' COMMENT '状态\r\n0=未完成\r\n1=完成\r\n2=中途退出',
  `start_time` int(11) NOT NULL DEFAULT '0' COMMENT '开始时间',
  `finish_time` int(11) NOT NULL DEFAULT '0' COMMENT '完成时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_dungeon
-- ----------------------------

-- ----------------------------
-- Table structure for `log_equip`
-- ----------------------------
DROP TABLE IF EXISTS `log_equip`;
CREATE TABLE `log_equip` (
  `index` int(11) NOT NULL AUTO_INCREMENT,
  `id` bigint(20) NOT NULL DEFAULT '0' COMMENT '装备唯一ID',
  `base` int(11) NOT NULL DEFAULT '0' COMMENT '装备基础ID',
  `owner` bigint(20) NOT NULL DEFAULT '0',
  `star` tinyint(4) NOT NULL DEFAULT '0' COMMENT '强化等级',
  `createtime` int(11) NOT NULL,
  `mapid` int(11) NOT NULL,
  `mid` int(11) NOT NULL,
  `name` varchar(30) NOT NULL DEFAULT '0' COMMENT '1 增加 2 更改 3 删除',
  `type` tinyint(4) NOT NULL DEFAULT '0' COMMENT '1 增加 2 更改 3 删除',
  `reason` int(11) NOT NULL DEFAULT '0' COMMENT '更改原因',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '更改时间',
  `lucky` int(11) DEFAULT NULL,
  PRIMARY KEY (`index`),
  KEY `owner` (`owner`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_equip
-- ----------------------------

-- ----------------------------
-- Table structure for `log_equip_drop`
-- ----------------------------
DROP TABLE IF EXISTS `log_equip_drop`;
CREATE TABLE `log_equip_drop` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `name` varchar(20) NOT NULL DEFAULT '0',
  `equip_id` int(11) NOT NULL DEFAULT '0',
  `src_id` bigint(20) NOT NULL DEFAULT '0',
  `map_id` int(11) NOT NULL DEFAULT '0',
  `killer_id` bigint(20) NOT NULL DEFAULT '0',
  `killer_name` varchar(20) NOT NULL DEFAULT '0',
  `time` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_equip_drop
-- ----------------------------

-- ----------------------------
-- Table structure for `log_game_shop`
-- ----------------------------
DROP TABLE IF EXISTS `log_game_shop`;
CREATE TABLE `log_game_shop` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '用户ID',
  `name` varchar(20) NOT NULL,
  `type` int(11) NOT NULL,
  `shop_id` int(11) NOT NULL DEFAULT '0' COMMENT '到期时间',
  `item_class` int(11) NOT NULL,
  `item_id` int(11) NOT NULL,
  `item_count` int(11) NOT NULL,
  `cost_gold` int(11) NOT NULL DEFAULT '0' COMMENT '标记',
  `left_gold` bigint(20) NOT NULL,
  `time` int(11) NOT NULL,
  KEY `cid` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_game_shop
-- ----------------------------

-- ----------------------------
-- Table structure for `log_general`
-- ----------------------------
DROP TABLE IF EXISTS `log_general`;
CREATE TABLE `log_general` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `param` varchar(200) DEFAULT NULL,
  `time` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_general
-- ----------------------------

-- ----------------------------
-- Table structure for `log_gm_gold_change`
-- ----------------------------
DROP TABLE IF EXISTS `log_gm_gold_change`;
CREATE TABLE `log_gm_gold_change` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '序列',
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '角色id',
  `name` char(30) NOT NULL DEFAULT '' COMMENT '角色名',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT '角色等级',
  `nindex` int(11) NOT NULL DEFAULT '0',
  `reason` int(11) NOT NULL DEFAULT '0' COMMENT '变动原因',
  `pid` int(11) NOT NULL DEFAULT '0' COMMENT '商品id',
  `count` int(11) NOT NULL DEFAULT '0' COMMENT '商品数量',
  `value` int(11) NOT NULL DEFAULT '0' COMMENT '元宝增减值',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_gm_gold_change
-- ----------------------------

-- ----------------------------
-- Table structure for `log_gold_change`
-- ----------------------------
DROP TABLE IF EXISTS `log_gold_change`;
CREATE TABLE `log_gold_change` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '序列',
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '角色id',
  `name` char(12) NOT NULL DEFAULT '' COMMENT '角色名',
  `type` int(11) NOT NULL,
  `opway` int(11) NOT NULL DEFAULT '0' COMMENT '原因',
  `value` int(11) NOT NULL DEFAULT '0' COMMENT '变化值',
  `param` bigint(20) NOT NULL DEFAULT '0',
  `left` bigint(20) NOT NULL,
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '时间',
  `fuli` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `id` (`id`) USING BTREE,
  KEY `index_time` (`time`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_gold_change
-- ----------------------------

-- ----------------------------
-- Table structure for `log_item_change`
-- ----------------------------
DROP TABLE IF EXISTS `log_item_change`;
CREATE TABLE `log_item_change` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `name` char(20) NOT NULL DEFAULT '',
  `flag` tinyint(4) NOT NULL DEFAULT '0',
  `reason` int(11) NOT NULL DEFAULT '0',
  `id` int(11) NOT NULL DEFAULT '0',
  `class` tinyint(4) NOT NULL DEFAULT '0',
  `count` int(11) NOT NULL DEFAULT '0',
  `time` int(11) NOT NULL DEFAULT '0',
  `srcid` bigint(20) NOT NULL DEFAULT '0' COMMENT '关联ID',
  `allcount` int(11) NOT NULL,
  KEY `cid` (`cid`) USING BTREE,
  KEY `index_time` (`time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_item_change
-- ----------------------------

-- ----------------------------
-- Table structure for `log_ming_ge`
-- ----------------------------
DROP TABLE IF EXISTS `log_ming_ge`;
CREATE TABLE `log_ming_ge` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '自增长标记',
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '角色id',
  `ming_ge_id` int(11) NOT NULL DEFAULT '0' COMMENT '命格id',
  `flag` int(11) NOT NULL DEFAULT '0' COMMENT '=1获得=2失去',
  `bag_type` int(11) NOT NULL COMMENT '=1 命格背包 =2猎命背包 =3 命格装备栏',
  `reason` int(11) DEFAULT NULL COMMENT '途径',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '时间',
  PRIMARY KEY (`id`),
  KEY `id` (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_ming_ge
-- ----------------------------

-- ----------------------------
-- Table structure for `log_moyushijie_reward`
-- ----------------------------
DROP TABLE IF EXISTS `log_moyushijie_reward`;
CREATE TABLE `log_moyushijie_reward` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '自增长标记',
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `name` varchar(20) NOT NULL,
  `dungeonid` int(11) NOT NULL DEFAULT '0' COMMENT '副本ID',
  `itemid` int(11) NOT NULL,
  `itemclass` int(11) NOT NULL,
  `itemcount` int(11) NOT NULL,
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '时间',
  `special` tinyint(4) NOT NULL DEFAULT '-1' COMMENT '特殊标记',
  PRIMARY KEY (`id`),
  KEY `time` (`time`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_moyushijie_reward
-- ----------------------------

-- ----------------------------
-- Table structure for `log_npc_func`
-- ----------------------------
DROP TABLE IF EXISTS `log_npc_func`;
CREATE TABLE `log_npc_func` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `name` varchar(20) NOT NULL DEFAULT '0',
  `funcid` int(11) NOT NULL DEFAULT '0',
  `mapid` int(11) NOT NULL,
  `x` int(11) NOT NULL,
  `y` int(11) NOT NULL,
  `time` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `id` (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_npc_func
-- ----------------------------

-- ----------------------------
-- Table structure for `log_online`
-- ----------------------------
DROP TABLE IF EXISTS `log_online`;
CREATE TABLE `log_online` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `login` int(11) NOT NULL DEFAULT '0',
  `active` int(11) NOT NULL DEFAULT '0',
  `inactive` int(11) NOT NULL DEFAULT '0',
  `online` int(11) NOT NULL DEFAULT '0',
  `job1` int(11) NOT NULL DEFAULT '0' COMMENT '战士在线人数',
  `job2` int(11) NOT NULL DEFAULT '0' COMMENT '法师在线人数',
  `job3` int(11) NOT NULL DEFAULT '0' COMMENT '刺客在线人数',
  `time` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_online
-- ----------------------------

-- ----------------------------
-- Table structure for `log_online_time`
-- ----------------------------
DROP TABLE IF EXISTS `log_online_time`;
CREATE TABLE `log_online_time` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '角色ID',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '时间',
  `online` int(11) NOT NULL DEFAULT '0' COMMENT '在线时长',
  PRIMARY KEY (`cid`,`time`),
  KEY `time` (`time`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_online_time
-- ----------------------------

-- ----------------------------
-- Table structure for `log_player_level`
-- ----------------------------
DROP TABLE IF EXISTS `log_player_level`;
CREATE TABLE `log_player_level` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '序列',
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '角色id',
  `name` char(12) NOT NULL DEFAULT '' COMMENT '角色名',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT '等级',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_player_level
-- ----------------------------

-- ----------------------------
-- Table structure for `log_player_level_distribution`
-- ----------------------------
DROP TABLE IF EXISTS `log_player_level_distribution`;
CREATE TABLE `log_player_level_distribution` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `level` int(11) NOT NULL DEFAULT '0',
  `kingdoms` varchar(500) NOT NULL DEFAULT '',
  `jobs` varchar(500) NOT NULL DEFAULT '',
  `total` int(11) NOT NULL DEFAULT '0',
  `time` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_player_level_distribution
-- ----------------------------

-- ----------------------------
-- Table structure for `log_player_login`
-- ----------------------------
DROP TABLE IF EXISTS `log_player_login`;
CREATE TABLE `log_player_login` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `action` int(11) NOT NULL DEFAULT '0',
  `time` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_player_login
-- ----------------------------

-- ----------------------------
-- Table structure for `log_player_stay`
-- ----------------------------
DROP TABLE IF EXISTS `log_player_stay`;
CREATE TABLE `log_player_stay` (
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '日期',
  `create` int(11) NOT NULL DEFAULT '0' COMMENT '创建人数',
  `day1` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day2` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day3` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day4` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day5` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day6` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day7` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day8` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day9` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day10` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day11` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day12` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day13` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day14` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day15` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day16` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day17` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day18` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day19` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day20` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day21` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day22` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day23` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day24` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day25` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day26` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day27` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day28` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day29` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  `day30` int(11) NOT NULL DEFAULT '0' COMMENT '留存人数',
  PRIMARY KEY (`time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_player_stay
-- ----------------------------

-- ----------------------------
-- Table structure for `log_proc`
-- ----------------------------
DROP TABLE IF EXISTS `log_proc`;
CREATE TABLE `log_proc` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `name` varchar(20) NOT NULL DEFAULT '0',
  `procId` int(11) NOT NULL DEFAULT '0',
  `time` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `id` (`id`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_proc
-- ----------------------------
-- ----------------------------
-- Table structure for `log_report_daily`
-- ----------------------------
DROP TABLE IF EXISTS `log_report_daily`;
CREATE TABLE `log_report_daily` (
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '日期',
  `regist_new` int(11) NOT NULL DEFAULT '0' COMMENT '注册人数',
  `login_new` int(11) NOT NULL DEFAULT '0' COMMENT '登陆人数',
  `create_new` int(11) NOT NULL DEFAULT '0' COMMENT '创建人数',
  `enter_new` int(11) NOT NULL DEFAULT '0' COMMENT '进入人数',
  `login_count` int(11) NOT NULL DEFAULT '0' COMMENT '登录次数',
  `login_player` int(11) NOT NULL DEFAULT '0' COMMENT '登录人数(总)',
  `online_max` int(11) NOT NULL DEFAULT '0' COMMENT '最高在线',
  `online_min` int(11) NOT NULL DEFAULT '0' COMMENT '最低在线',
  `online_avg` int(11) NOT NULL DEFAULT '0' COMMENT '平均在线',
  `pay_count` int(11) NOT NULL DEFAULT '0' COMMENT '充值',
  `pay_player` int(11) NOT NULL DEFAULT '0' COMMENT '充值人数',
  `pay_times` int(11) NOT NULL DEFAULT '0' COMMENT '充值次数',
  `job1` int(11) NOT NULL DEFAULT '0' COMMENT '战士在线人数',
  `job2` int(11) NOT NULL DEFAULT '0' COMMENT '法师在线人数',
  `job3` int(11) NOT NULL DEFAULT '0' COMMENT '刺客在线人数',
  PRIMARY KEY (`time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_report_daily
-- ----------------------------
INSERT INTO log_report_daily VALUES ('1575475200', '3', '0', '3', '2', '2', '2', '1', '0', '0', '0', '0', '0', '11', '0', '0');

-- ----------------------------
-- Table structure for `log_report_hourly`
-- ----------------------------
DROP TABLE IF EXISTS `log_report_hourly`;
CREATE TABLE `log_report_hourly` (
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '日期',
  `regist_new` int(11) NOT NULL DEFAULT '0' COMMENT '注册人数',
  `login_new` int(11) NOT NULL DEFAULT '0' COMMENT '登陆人数',
  `create_new` int(11) NOT NULL DEFAULT '0' COMMENT '创建人数',
  `enter_new` int(11) NOT NULL DEFAULT '0' COMMENT '进入人数',
  `login_count` int(11) NOT NULL DEFAULT '0' COMMENT '登录次数',
  `login_player` int(11) NOT NULL DEFAULT '0' COMMENT '登录人数(总)',
  `online_max` int(11) NOT NULL DEFAULT '0' COMMENT '最高在线',
  `online_min` int(11) NOT NULL DEFAULT '0' COMMENT '最低在线',
  `online_avg` int(11) NOT NULL DEFAULT '0' COMMENT '平均在线',
  `pay_count` int(11) NOT NULL DEFAULT '0' COMMENT '充值',
  `pay_player` int(11) NOT NULL DEFAULT '0' COMMENT '充值人数',
  `pay_times` int(11) NOT NULL DEFAULT '0' COMMENT '充值次数',
  `job1` int(11) NOT NULL DEFAULT '0' COMMENT '战士在线人数',
  `job2` int(11) NOT NULL DEFAULT '0' COMMENT '法师在线人数',
  `job3` int(11) NOT NULL DEFAULT '0' COMMENT '刺客在线人数',
  PRIMARY KEY (`time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_report_hourly
-- ----------------------------

-- ----------------------------
-- Table structure for `log_report_minutly`
-- ----------------------------
DROP TABLE IF EXISTS `log_report_minutly`;
CREATE TABLE `log_report_minutly` (
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '日期',
  `regist_new` int(11) NOT NULL DEFAULT '0' COMMENT '注册人数',
  `login_new` int(11) NOT NULL DEFAULT '0' COMMENT '登陆人数',
  `create_new` int(11) NOT NULL DEFAULT '0' COMMENT '创建人数',
  `enter_new` int(11) NOT NULL DEFAULT '0' COMMENT '进入人数',
  `login_count` int(11) NOT NULL DEFAULT '0' COMMENT '登录次数',
  `login_player` int(11) NOT NULL DEFAULT '0' COMMENT '登录人数(总)',
  `online_max` int(11) NOT NULL DEFAULT '0' COMMENT '最高在线',
  `online_min` int(11) NOT NULL DEFAULT '0' COMMENT '最低在线',
  `online_avg` int(11) NOT NULL DEFAULT '0' COMMENT '平均在线',
  `pay_count` int(11) NOT NULL DEFAULT '0' COMMENT '充值',
  `pay_player` int(11) NOT NULL DEFAULT '0' COMMENT '充值人数',
  `pay_times` int(11) NOT NULL DEFAULT '0' COMMENT '充值次数',
  `job1` int(11) NOT NULL DEFAULT '0' COMMENT '战士在线人数',
  `job2` int(11) NOT NULL DEFAULT '0' COMMENT '法师在线人数',
  `job3` int(11) NOT NULL DEFAULT '0' COMMENT '刺客在线人数',
  PRIMARY KEY (`time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_report_minutly
-- ----------------------------

-- ----------------------------
-- Table structure for `log_sheng_dan_fu_dai_open`
-- ----------------------------
DROP TABLE IF EXISTS `log_sheng_dan_fu_dai_open`;
CREATE TABLE `log_sheng_dan_fu_dai_open` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '自增长标记',
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `name` varchar(20) NOT NULL,
  `special` int(11) NOT NULL DEFAULT '0' COMMENT '圣诞福袋开出特殊道具',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '时间',
  PRIMARY KEY (`id`),
  KEY `time` (`time`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_sheng_dan_fu_dai_open
-- ----------------------------

-- ----------------------------
-- Table structure for `log_task`
-- ----------------------------
DROP TABLE IF EXISTS `log_task`;
CREATE TABLE `log_task` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `tid` int(11) NOT NULL DEFAULT '0' COMMENT '任务ID',
  `type` tinyint(4) NOT NULL DEFAULT '0' COMMENT '任务类型',
  `receive_time` int(11) NOT NULL DEFAULT '0' COMMENT '接受时间',
  `submit_time` int(11) NOT NULL DEFAULT '0' COMMENT '完成时间',
  `state` int(11) NOT NULL DEFAULT '0' COMMENT '任务状态\r\n',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_task
-- ----------------------------

-- ----------------------------
-- Table structure for `log_up_start`
-- ----------------------------
DROP TABLE IF EXISTS `log_up_start`;
CREATE TABLE `log_up_start` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '自增长标记',
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '角色id',
  `name` varchar(20) NOT NULL DEFAULT '0' COMMENT '名字',
  `start` int(11) NOT NULL DEFAULT '0' COMMENT '等级',
  `equip_id` int(11) DEFAULT NULL COMMENT '装备id',
  `srcid` int(20) DEFAULT NULL COMMENT '装备唯一id',
  `opway` int(11) DEFAULT NULL COMMENT '=1升星,=2转移',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '击杀时间',
  PRIMARY KEY (`id`),
  KEY `id` (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_up_start
-- ----------------------------

-- ----------------------------
-- Table structure for `log_xin_mo_item_change`
-- ----------------------------
DROP TABLE IF EXISTS `log_xin_mo_item_change`;
CREATE TABLE `log_xin_mo_item_change` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `flag` tinyint(4) NOT NULL DEFAULT '0',
  `reason` int(11) NOT NULL DEFAULT '0',
  `id` int(11) NOT NULL DEFAULT '0',
  `class` tinyint(4) NOT NULL DEFAULT '0',
  `count` int(11) NOT NULL DEFAULT '0',
  `time` int(11) NOT NULL DEFAULT '0',
  `srcid` bigint(20) NOT NULL DEFAULT '0' COMMENT '关联ID',
  KEY `cid` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of log_xin_mo_item_change
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_boss_first_killed`
-- ----------------------------
DROP TABLE IF EXISTS `mem_boss_first_killed`;
CREATE TABLE `mem_boss_first_killed` (
  `boss_id` int(11) NOT NULL DEFAULT '0',
  `killer` bigint(20) NOT NULL DEFAULT '0' COMMENT '击杀者',
  `killer_name` varchar(20) DEFAULT NULL,
  `kill_time` int(11) NOT NULL DEFAULT '0' COMMENT '击杀时间',
  PRIMARY KEY (`boss_id`),
  KEY `owner` (`boss_id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_boss_first_killed
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_character`
-- ----------------------------
DROP TABLE IF EXISTS `mem_character`;
CREATE TABLE `mem_character` (
  `cid` bigint(20) NOT NULL COMMENT '角色ID',
  `uid` bigint(20) NOT NULL DEFAULT '0' COMMENT '用户ID',
  `sid` int(11) NOT NULL DEFAULT '0' COMMENT '服务器id',
  `name` char(20) NOT NULL DEFAULT '' COMMENT '角色名',
  `sex` int(11) NOT NULL DEFAULT '0' COMMENT '性别',
  `job` int(11) NOT NULL DEFAULT '0' COMMENT '职业',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT '等级',
  `exp` bigint(20) NOT NULL DEFAULT '0' COMMENT '拥有经验',
  `battle` int(11) NOT NULL DEFAULT '0' COMMENT '战斗力',
  `top_battle` int(11) NOT NULL DEFAULT '0' COMMENT '巅峰战斗力',
  `mapid` int(11) NOT NULL DEFAULT '0' COMMENT '所在地图ID',
  `x` int(11) NOT NULL DEFAULT '0' COMMENT '所在地图x',
  `y` int(11) NOT NULL DEFAULT '0' COMMENT '所在地图y',
  `hp` int(11) NOT NULL DEFAULT '0' COMMENT '血量',
  `mp` int(11) NOT NULL DEFAULT '0' COMMENT '蓝量',
  `kun_li` int(11) NOT NULL,
  `head` int(11) NOT NULL DEFAULT '0',
  `create_time` int(11) NOT NULL DEFAULT '0' COMMENT '创建时间',
  `pk_mode` int(11) NOT NULL DEFAULT '0' COMMENT 'pk模式',
  `pk_value` int(11) NOT NULL DEFAULT '0' COMMENT 'pk值',
  `pk_time` int(11) NOT NULL DEFAULT '0',
  `kill_count` int(11) NOT NULL DEFAULT '0' COMMENT '杀死玩家数目',
  `last_task_id` int(11) NOT NULL DEFAULT '0',
  `logout_count` int(11) NOT NULL DEFAULT '0' COMMENT '登录次数',
  `pay_click_count` int(11) NOT NULL DEFAULT '0' COMMENT '充值按钮点击次数',
  `level_stay_time` int(11) NOT NULL DEFAULT '0',
  `last_login_time` int(11) NOT NULL DEFAULT '0' COMMENT '最后登录时间',
  `last_logout_time` int(11) NOT NULL DEFAULT '0' COMMENT '最后登出时间',
  `delete_time` int(11) NOT NULL DEFAULT '0' COMMENT '删除时间',
  `weapon` int(11) NOT NULL DEFAULT '0' COMMENT '主武器',
  `vice` int(11) NOT NULL DEFAULT '0' COMMENT '副武器',
  `cloth` int(11) NOT NULL DEFAULT '0' COMMENT '当前衣服',
  `wing` int(11) NOT NULL DEFAULT '0' COMMENT '当前翅膀',
  `equip_star` int(11) NOT NULL DEFAULT '0' COMMENT '装备总星数',
  `equip_gem` int(11) NOT NULL DEFAULT '0' COMMENT '装备总宝石等级',
  `atk_suit_effect` int(11) NOT NULL DEFAULT '0' COMMENT '套装特效',
  `def_suit_effect` int(11) NOT NULL DEFAULT '0' COMMENT '套装特效',
  `money_drop_ratio` int(11) NOT NULL DEFAULT '0' COMMENT '金币掉落率',
  PRIMARY KEY (`cid`),
  UNIQUE KEY `name` (`name`) USING BTREE,
  KEY `uidsid` (`uid`,`sid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_character
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_char_card`
-- ----------------------------
DROP TABLE IF EXISTS `mem_char_card`;
CREATE TABLE `mem_char_card` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `id` int(11) NOT NULL DEFAULT '0' COMMENT '卡牌',
  `count` int(11) NOT NULL DEFAULT '0' COMMENT '数量',
  PRIMARY KEY (`cid`,`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_char_card
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_char_currency`
-- ----------------------------
DROP TABLE IF EXISTS `mem_char_currency`;
CREATE TABLE `mem_char_currency` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `money` bigint(20) NOT NULL DEFAULT '0' COMMENT '铜钱',
  `gold` bigint(20) NOT NULL DEFAULT '0' COMMENT '元宝',
  `cash` bigint(20) NOT NULL DEFAULT '0' COMMENT '绑元',
  `vigour` bigint(20) NOT NULL DEFAULT '0' COMMENT '元气',
  `draw_score` bigint(20) NOT NULL DEFAULT '0' COMMENT '血钻碎片',
  `boss_score` bigint(20) NOT NULL DEFAULT '0' COMMENT 'boss积分',
  `bind_money` bigint(20) NOT NULL DEFAULT '0' COMMENT '绑定金币',
  `honor` bigint(20) NOT NULL DEFAULT '0' COMMENT '荣誉',
  `ac_score` bigint(20) NOT NULL DEFAULT '0' COMMENT '积分',
  `contribution` bigint(20) NOT NULL DEFAULT '0' COMMENT '军团贡献',
  `blood` bigint(20) NOT NULL DEFAULT '0' COMMENT '血岩碎片',
  `dust` bigint(20) NOT NULL DEFAULT '0' COMMENT '涅法雷姆经验',
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_char_currency
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_char_equip_rong_he`
-- ----------------------------
DROP TABLE IF EXISTS `mem_char_equip_rong_he`;
CREATE TABLE `mem_char_equip_rong_he` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `equip_rong_he` varchar(500) NOT NULL,
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_char_equip_rong_he
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_char_exchange`
-- ----------------------------
DROP TABLE IF EXISTS `mem_char_exchange`;
CREATE TABLE `mem_char_exchange` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `type` tinyint(4) NOT NULL DEFAULT '0' COMMENT '类型',
  `index` int(11) NOT NULL DEFAULT '0' COMMENT '序列',
  `count` int(11) NOT NULL DEFAULT '0' COMMENT '次数',
  KEY `cid` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_char_exchange
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_char_guo_qing`
-- ----------------------------
DROP TABLE IF EXISTS `mem_char_guo_qing`;
CREATE TABLE `mem_char_guo_qing` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `level` int(11) DEFAULT NULL,
  `exp` int(11) DEFAULT NULL,
  `lock` int(11) DEFAULT NULL,
  `ordinary_reward` int(11) DEFAULT NULL,
  `senior_reward` int(11) DEFAULT NULL,
  `national_info` varchar(300) DEFAULT NULL,
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_char_guo_qing
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_char_jue_wei`
-- ----------------------------
DROP TABLE IF EXISTS `mem_char_jue_wei`;
CREATE TABLE `mem_char_jue_wei` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '主角id',
  `level` tinyint(4) NOT NULL DEFAULT '0' COMMENT '爵位等级',
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_char_jue_wei
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_char_magic_box`
-- ----------------------------
DROP TABLE IF EXISTS `mem_char_magic_box`;
CREATE TABLE `mem_char_magic_box` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '角色ID',
  `combi_points` int(11) NOT NULL DEFAULT '0' COMMENT '合成点数',
  `revive_time` int(11) NOT NULL DEFAULT '0' COMMENT '上次合成点数回复时间',
  `active_scrolls` varchar(200) NOT NULL DEFAULT '0' COMMENT '激活配方列表',
  `success_ids` varchar(200) NOT NULL DEFAULT '0' COMMENT '成功合成列表',
  PRIMARY KEY (`cid`),
  KEY `cid` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_char_magic_box
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_char_mystery_shop`
-- ----------------------------
DROP TABLE IF EXISTS `mem_char_mystery_shop`;
CREATE TABLE `mem_char_mystery_shop` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `type` tinyint(4) NOT NULL DEFAULT '0' COMMENT '类型',
  `buy_flag` int(11) NOT NULL DEFAULT '0' COMMENT '购买标记',
  `refresh_time` int(11) NOT NULL DEFAULT '0' COMMENT '上次刷新时间',
  `goods` varchar(50) NOT NULL DEFAULT '' COMMENT '刷新道具',
  PRIMARY KEY (`cid`,`type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_char_mystery_shop
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_char_operate_limit`
-- ----------------------------
DROP TABLE IF EXISTS `mem_char_operate_limit`;
CREATE TABLE `mem_char_operate_limit` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `LimitId` int(11) NOT NULL DEFAULT '0' COMMENT '操作限制类型',
  `LimitCount` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`cid`,`LimitId`),
  KEY `cidtype` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_char_operate_limit
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_char_portal`
-- ----------------------------
DROP TABLE IF EXISTS `mem_char_portal`;
CREATE TABLE `mem_char_portal` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `id` int(11) NOT NULL DEFAULT '0',
  `map` int(11) NOT NULL DEFAULT '0' COMMENT '地图',
  `x` int(11) NOT NULL DEFAULT '0',
  `y` int(11) NOT NULL DEFAULT '0',
  `dungeon` int(11) NOT NULL DEFAULT '0' COMMENT '副本id',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '开始时间',
  `duration` int(11) NOT NULL DEFAULT '0' COMMENT '持续时间',
  `close` tinyint(4) NOT NULL DEFAULT '0' COMMENT '关闭标记',
  PRIMARY KEY (`cid`,`id`),
  KEY `cid_close` (`cid`,`close`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_char_portal
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_char_shi_zhuang`
-- ----------------------------
DROP TABLE IF EXISTS `mem_char_shi_zhuang`;
CREATE TABLE `mem_char_shi_zhuang` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `type` tinyint(4) NOT NULL DEFAULT '0' COMMENT '类型',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT '等级',
  `exp` int(11) NOT NULL DEFAULT '0' COMMENT '经验',
  `wear` int(11) NOT NULL DEFAULT '0' COMMENT '穿戴',
  `active` varchar(600) NOT NULL DEFAULT '' COMMENT '激活列表',
  PRIMARY KEY (`cid`,`type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_char_shi_zhuang
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_char_shi_zhuang_level`
-- ----------------------------
DROP TABLE IF EXISTS `mem_char_shi_zhuang_level`;
CREATE TABLE `mem_char_shi_zhuang_level` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `shi_zhuang_level` varchar(1000) DEFAULT NULL,
  `effect` int(11) DEFAULT NULL,
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_char_shi_zhuang_level
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_char_union_record`
-- ----------------------------
DROP TABLE IF EXISTS `mem_char_union_record`;
CREATE TABLE `mem_char_union_record` (
  `cid` bigint(20) NOT NULL,
  `pf` varchar(20) NOT NULL DEFAULT '0',
  `id` int(11) NOT NULL DEFAULT '0',
  `new` tinyint(4) NOT NULL DEFAULT '0' COMMENT '新手礼包',
  `daily` tinyint(4) NOT NULL DEFAULT '0' COMMENT '每日礼包',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT '等级礼包',
  PRIMARY KEY (`cid`,`pf`,`id`),
  KEY `cid` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_char_union_record
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_char_wing`
-- ----------------------------
DROP TABLE IF EXISTS `mem_char_wing`;
CREATE TABLE `mem_char_wing` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `Level` int(11) NOT NULL DEFAULT '0' COMMENT '等级',
  `Luck` int(11) NOT NULL DEFAULT '0' COMMENT '幸运值',
  `HuanHua` int(11) DEFAULT NULL,
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_char_wing
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_char_wish`
-- ----------------------------
DROP TABLE IF EXISTS `mem_char_wish`;
CREATE TABLE `mem_char_wish` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `id` int(11) NOT NULL DEFAULT '0',
  `flag` tinyint(4) NOT NULL DEFAULT '0' COMMENT '领取标记',
  `item_id` int(11) NOT NULL DEFAULT '0' COMMENT '道具ID',
  `start_time` int(11) NOT NULL DEFAULT '0' COMMENT '开始时间',
  `info` varchar(500) NOT NULL DEFAULT '' COMMENT '许愿信息',
  PRIMARY KEY (`cid`,`id`),
  KEY `cidflag` (`cid`,`flag`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_char_wish
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_char_worship`
-- ----------------------------
DROP TABLE IF EXISTS `mem_char_worship`;
CREATE TABLE `mem_char_worship` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `times` tinyint(4) NOT NULL DEFAULT '0' COMMENT '今日崇拜次数',
  `charlist` varchar(120) NOT NULL DEFAULT '' COMMENT '今日崇拜过的人列表',
  `record` int(11) NOT NULL DEFAULT '0' COMMENT '崇拜数',
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_char_worship
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_action`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_action`;
CREATE TABLE `mem_chr_action` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '角色id',
  `slot` int(11) NOT NULL DEFAULT '0' COMMENT '位置，1~12',
  `id` int(11) NOT NULL DEFAULT '0' COMMENT '道具或者技能id',
  `type` int(11) NOT NULL DEFAULT '0' COMMENT '类型，1道具，2技能',
  KEY `cid` (`cid`) USING BTREE,
  KEY `slot` (`slot`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_action
-- ----------------------------
-- ----------------------------
-- Table structure for `mem_chr_attr`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_attr`;
CREATE TABLE `mem_chr_attr` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `Attrs` varchar(400) NOT NULL DEFAULT '0' COMMENT '生命值',
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_attr
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_auction`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_auction`;
CREATE TABLE `mem_chr_auction` (
  `aid` int(11) NOT NULL COMMENT '拍卖ID',
  `id` int(11) NOT NULL DEFAULT '0' COMMENT '物品ID',
  `class` tinyint(4) NOT NULL DEFAULT '0' COMMENT '物品类型',
  `count` int(11) NOT NULL DEFAULT '0' COMMENT '数量',
  `srcid` bigint(20) NOT NULL DEFAULT '0' COMMENT '关联ID\r\n例如：装备ID',
  `price` int(11) NOT NULL DEFAULT '0' COMMENT '售价',
  `currtype` int(4) NOT NULL,
  `seller` bigint(20) NOT NULL DEFAULT '0' COMMENT '卖家',
  `buyer` bigint(20) NOT NULL DEFAULT '0' COMMENT '买家',
  `uptime` int(11) NOT NULL DEFAULT '0' COMMENT '上架时间',
  `downtime` int(11) NOT NULL DEFAULT '0' COMMENT '下架时间',
  `gettime` int(11) NOT NULL,
  PRIMARY KEY (`aid`),
  KEY `cidbagslot` (`aid`) USING BTREE,
  KEY `id` (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_auction
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_auto_fight`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_auto_fight`;
CREATE TABLE `mem_chr_auto_fight` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `fight` varchar(600) NOT NULL DEFAULT '',
  `pick` char(1) NOT NULL DEFAULT '',
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_auto_fight
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_bag`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_bag`;
CREATE TABLE `mem_chr_bag` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `bag` tinyint(4) NOT NULL DEFAULT '0' COMMENT '包裹编号\r\n装备栏=0\r\n背包=1\r\n仓库=2\r\n',
  `slot` int(11) NOT NULL DEFAULT '0' COMMENT '格子',
  `id` int(11) NOT NULL DEFAULT '0' COMMENT '物品ID',
  `class` tinyint(4) NOT NULL DEFAULT '0' COMMENT '物品类型',
  `count` int(11) NOT NULL DEFAULT '0' COMMENT '数量',
  `bind` tinyint(4) NOT NULL DEFAULT '0' COMMENT '绑定状态',
  `endtime` int(11) NOT NULL DEFAULT '0' COMMENT '过期时间',
  `srcid` bigint(20) NOT NULL DEFAULT '0' COMMENT '关联ID\r\n例如：装备ID',
  PRIMARY KEY (`cid`,`bag`,`slot`),
  KEY `cidbag` (`cid`,`bag`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_bag
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_boss_killed`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_boss_killed`;
CREATE TABLE `mem_chr_boss_killed` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `reward_info` varchar(600) DEFAULT NULL,
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_boss_killed
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_buff`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_buff`;
CREATE TABLE `mem_chr_buff` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `item` int(11) NOT NULL DEFAULT '0' COMMENT '道具ID',
  `buff` int(11) NOT NULL DEFAULT '0' COMMENT 'BUFF ID',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '到期时间',
  KEY `cid` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_buff
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_chou_jiang_record`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_chou_jiang_record`;
CREATE TABLE `mem_chr_chou_jiang_record` (
  `Cid` bigint(20) DEFAULT NULL,
  `ItemId` int(11) DEFAULT NULL,
  `ItemClass` int(4) DEFAULT NULL,
  `ItemCount` int(11) DEFAULT NULL,
  `Time` int(11) DEFAULT NULL,
  KEY `cid` (`Cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_chou_jiang_record
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_chou_jiang_score`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_chou_jiang_score`;
CREATE TABLE `mem_chr_chou_jiang_score` (
  `Cid` bigint(20) NOT NULL DEFAULT '0',
  `LuckPoint` int(11) DEFAULT NULL COMMENT '抽奖幸运值',
  `Score` int(11) DEFAULT NULL COMMENT '抽奖积分',
  PRIMARY KEY (`Cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_chou_jiang_score
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_depot`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_depot`;
CREATE TABLE `mem_chr_depot` (
  `Cid` bigint(20) NOT NULL DEFAULT '0',
  `CurrencyString` varchar(150) DEFAULT NULL,
  `open_count` int(11) DEFAULT NULL,
  `Password` varchar(30) DEFAULT NULL,
  `second_password` varchar(30) DEFAULT NULL,
  PRIMARY KEY (`Cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_depot
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_die`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_die`;
CREATE TABLE `mem_chr_die` (
  `Cid` bigint(20) DEFAULT NULL,
  `KillTime` int(11) DEFAULT NULL,
  `MapId` int(11) DEFAULT NULL,
  `Mid` int(11) DEFAULT NULL,
  `KillerId` bigint(20) DEFAULT NULL,
  `KillerName` varchar(20) DEFAULT NULL,
  KEY `cid` (`Cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_die
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_fabao`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_fabao`;
CREATE TABLE `mem_chr_fabao` (
  `Cid` bigint(20) NOT NULL DEFAULT '0',
  `FaBaoLevel` varchar(300) NOT NULL DEFAULT '' COMMENT '法宝等级',
  PRIMARY KEY (`Cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_fabao
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_family`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_family`;
CREATE TABLE `mem_chr_family` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `family_id` bigint(20) NOT NULL DEFAULT '0' COMMENT '家族id',
  `position` int(11) NOT NULL DEFAULT '0' COMMENT '职位',
  `daily_contribute` int(11) NOT NULL DEFAULT '0' COMMENT '每日贡献度',
  `contribute` int(11) NOT NULL DEFAULT '0' COMMENT '累计贡献度',
  `skills` varchar(60) NOT NULL DEFAULT '' COMMENT '帮派技能',
  `leave_time` int(11) NOT NULL DEFAULT '0' COMMENT '离开帮派时间',
  `med_level` int(11) DEFAULT NULL,
  `med_res` int(11) DEFAULT NULL,
  `hoe` int(11) DEFAULT NULL,
  `enter_dungeion_family_id` bigint(20) DEFAULT NULL,
  `enter_count` int(11) DEFAULT NULL,
  `enter_time` int(11) DEFAULT NULL,
  PRIMARY KEY (`cid`),
  KEY `family_id` (`family_id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_family
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_friend`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_friend`;
CREATE TABLE `mem_chr_friend` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `tcid` bigint(20) NOT NULL DEFAULT '0',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '记录时间',
  KEY `cid` (`cid`) USING BTREE,
  KEY `tcid` (`tcid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_friend
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_gem`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_gem`;
CREATE TABLE `mem_chr_gem` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `gems` varchar(200) NOT NULL DEFAULT '' COMMENT '宝石',
  `pos_level` varchar(60) NOT NULL DEFAULT '' COMMENT '部位强化',
  `streng_then` varchar(60) DEFAULT NULL,
  `shen_yao` varchar(60) DEFAULT NULL,
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_gem
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_goblin`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_goblin`;
CREATE TABLE `mem_chr_goblin` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `goblin_info` varchar(300) NOT NULL DEFAULT '' COMMENT '灵器信息',
  `shou_hu_info` varchar(300) DEFAULT NULL COMMENT '守护精炼信息',
  `wing_equip_polish` varchar(300) DEFAULT NULL,
  `mo_fu` varchar(300) DEFAULT NULL,
  `wing_refining` varchar(300) DEFAULT NULL,
  `vip_pos` varchar(300) DEFAULT NULL,
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_goblin
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_huo_yue_du`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_huo_yue_du`;
CREATE TABLE `mem_chr_huo_yue_du` (
  `Cid` bigint(20) NOT NULL DEFAULT '0',
  `HuoYueDuInfo` varchar(150) DEFAULT NULL,
  PRIMARY KEY (`Cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_huo_yue_du
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_kun`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_kun`;
CREATE TABLE `mem_chr_kun` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `buy_times` tinyint(4) DEFAULT '0' COMMENT '包裹编号\r\n装备栏=0\r\n背包=1\r\n仓库=2\r\n',
  `lian_hua_time` int(11) DEFAULT '0' COMMENT '格子',
  `kun_level` int(11) DEFAULT '0' COMMENT '物品ID',
  `exp` int(11) DEFAULT '0' COMMENT '是否锁定',
  `dan_tian` varchar(300) DEFAULT NULL,
  `ling_zhu_pos` varchar(400) DEFAULT NULL,
  PRIMARY KEY (`cid`),
  KEY `cidbag` (`cid`,`buy_times`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_kun
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_kun_bag`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_kun_bag`;
CREATE TABLE `mem_chr_kun_bag` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `slot` int(11) NOT NULL DEFAULT '0' COMMENT '包裹编号\r\n装备栏=0\r\n背包=1\r\n仓库=2\r\n',
  `nId` int(11) DEFAULT '0' COMMENT '格子',
  `nCount` int(11) DEFAULT '0' COMMENT '物品ID',
  PRIMARY KEY (`cid`,`slot`),
  KEY `cidbag` (`cid`,`slot`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_kun_bag
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_lately_chat`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_lately_chat`;
CREATE TABLE `mem_chr_lately_chat` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `lcid` bigint(20) NOT NULL DEFAULT '0' COMMENT '聊天对象',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '聊天时间',
  PRIMARY KEY (`cid`,`lcid`),
  KEY `cid` (`cid`) USING BTREE,
  KEY `tcid` (`lcid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_lately_chat
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_ming_ge_bag`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_ming_ge_bag`;
CREATE TABLE `mem_chr_ming_ge_bag` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `bag` tinyint(4) NOT NULL DEFAULT '0' COMMENT '包裹编号\r\n装备栏=0\r\n背包=1\r\n仓库=2\r\n',
  `slot` int(11) NOT NULL DEFAULT '0' COMMENT '格子',
  `id` int(11) NOT NULL DEFAULT '0' COMMENT '物品ID',
  `lock` tinyint(4) NOT NULL DEFAULT '0' COMMENT '是否锁定',
  PRIMARY KEY (`cid`,`bag`,`slot`),
  KEY `cidbag` (`cid`,`bag`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_ming_ge_bag
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_ming_ge_info`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_ming_ge_info`;
CREATE TABLE `mem_chr_ming_ge_info` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `ming_ge_exp` int(4) NOT NULL DEFAULT '0' COMMENT '包裹编号\r\n装备栏=0\r\n背包=1\r\n仓库=2\r\n',
  `ming_ge_chip` int(11) NOT NULL DEFAULT '0' COMMENT '格子',
  `ming_ge_npc` varchar(60) NOT NULL DEFAULT '0' COMMENT '命格npc',
  PRIMARY KEY (`cid`),
  KEY `cidbag` (`cid`,`ming_ge_exp`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_ming_ge_info
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_month_chou_jiang`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_month_chou_jiang`;
CREATE TABLE `mem_chr_month_chou_jiang` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `score` int(11) NOT NULL DEFAULT '0' COMMENT '道具ID',
  `all_score` int(11) NOT NULL DEFAULT '0' COMMENT 'BUFF ID',
  `get_item_list` varchar(200) NOT NULL DEFAULT '0' COMMENT '到期时间',
  `get_score_time` varchar(100) NOT NULL,
  PRIMARY KEY (`cid`),
  UNIQUE KEY `cid` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_month_chou_jiang
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_pet`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_pet`;
CREATE TABLE `mem_chr_pet` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '角色ID',
  `pid` bigint(20) NOT NULL DEFAULT '0' COMMENT '宠物ID',
  `name` varchar(50) NOT NULL DEFAULT '' COMMENT '名称',
  `hp` int(11) NOT NULL DEFAULT '0' COMMENT '生命值',
  `alive` tinyint(4) NOT NULL DEFAULT '0' COMMENT '存活标记',
  `state` tinyint(4) NOT NULL DEFAULT '0' COMMENT '状态',
  `star` int(11) NOT NULL DEFAULT '0' COMMENT '星级',
  `starExp` int(11) NOT NULL DEFAULT '0' COMMENT '升星经验',
  `skin` int(11) NOT NULL DEFAULT '0' COMMENT '当前皮肤',
  `dieTime` int(11) NOT NULL DEFAULT '0' COMMENT '死亡时间',
  `skins` varchar(100) NOT NULL DEFAULT '0' COMMENT '激活的皮肤',
  `equip` varchar(20) NOT NULL DEFAULT '' COMMENT '装备',
  `skill` varchar(30) NOT NULL DEFAULT '' COMMENT '技能',
  `AiState` int(4) DEFAULT NULL,
  `HuanHua` int(11) DEFAULT NULL,
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_pet
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_search_back_info`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_search_back_info`;
CREATE TABLE `mem_chr_search_back_info` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `type` tinyint(4) NOT NULL DEFAULT '0' COMMENT '类型\r\n1 活动\r\n2 副本',
  `sub_type` tinyint(4) NOT NULL DEFAULT '0' COMMENT '活动和副本的类型',
  `times` int(20) NOT NULL DEFAULT '0' COMMENT '次数',
  `param` int(20) NOT NULL DEFAULT '0' COMMENT '参数\r\n活动:ID\r\n副本:爬塔最高层数',
  PRIMARY KEY (`cid`,`type`,`sub_type`),
  KEY `cid` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_search_back_info
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_search_back_record`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_search_back_record`;
CREATE TABLE `mem_chr_search_back_record` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `day` tinyint(4) NOT NULL DEFAULT '0' COMMENT '天数\r\n0 表示今天\r\n1 表示昨天\r\n...',
  `id` int(4) NOT NULL DEFAULT '0',
  `times` int(20) NOT NULL DEFAULT '0' COMMENT '次数',
  `param` int(20) NOT NULL DEFAULT '0' COMMENT '参数\r\n活动:ID\r\n副本:爬塔最高层数',
  PRIMARY KEY (`cid`,`day`,`id`),
  KEY `cid` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_search_back_record
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_seven_task`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_seven_task`;
CREATE TABLE `mem_chr_seven_task` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `open_time` int(11) NOT NULL DEFAULT '0' COMMENT 'VIP',
  `reward_state` varchar(180) NOT NULL DEFAULT '0' COMMENT 'vip等级',
  `sum_reward_state` int(11) NOT NULL DEFAULT '0' COMMENT '永久VIP标志',
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_seven_task
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_shop_limit`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_shop_limit`;
CREATE TABLE `mem_chr_shop_limit` (
  `Cid` bigint(20) NOT NULL,
  `ShopType` int(4) NOT NULL,
  `ShopId` int(11) NOT NULL DEFAULT '0',
  `LimitCount` int(11) NOT NULL,
  KEY `cid` (`Cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_shop_limit
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_sign_info`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_sign_info`;
CREATE TABLE `mem_chr_sign_info` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `sign_record` int(11) NOT NULL,
  `sign_reward` varchar(70) NOT NULL DEFAULT '0',
  `refresh_time` int(11) NOT NULL DEFAULT '0',
  `online_reward` varchar(100) NOT NULL,
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_sign_info
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_skill`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_skill`;
CREATE TABLE `mem_chr_skill` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '角色id',
  `talents` varchar(300) NOT NULL DEFAULT '' COMMENT '天赋加点',
  `active` varchar(300) NOT NULL DEFAULT '' COMMENT '天赋激活',
  `power` int(11) DEFAULT NULL,
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_skill
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_system_setting`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_system_setting`;
CREATE TABLE `mem_chr_system_setting` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `setting` varchar(600) NOT NULL DEFAULT '',
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_system_setting
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_task`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_task`;
CREATE TABLE `mem_chr_task` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '玩家id',
  `tid` int(11) NOT NULL DEFAULT '0' COMMENT '任务id',
  `state` int(11) NOT NULL DEFAULT '0' COMMENT '任务状态',
  `monster` int(11) NOT NULL DEFAULT '0',
  KEY `cidtid` (`cid`,`tid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_task
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_tou_zi`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_tou_zi`;
CREATE TABLE `mem_chr_tou_zi` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `seven_day_time` int(11) NOT NULL DEFAULT '0' COMMENT '七日投资开始时间',
  `seven_day_record` int(11) NOT NULL DEFAULT '0' COMMENT '七日投资记录',
  `month_time` int(11) NOT NULL DEFAULT '0' COMMENT '等级投资金额',
  `month_record` int(11) NOT NULL DEFAULT '0' COMMENT '等级投资记录',
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_tou_zi
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_vip`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_vip`;
CREATE TABLE `mem_chr_vip` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `vipCard` int(11) NOT NULL DEFAULT '0' COMMENT 'VIP',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT 'vip等级',
  `forever` int(11) NOT NULL DEFAULT '0' COMMENT '永久VIP标志',
  `vipEndTime` varchar(60) NOT NULL DEFAULT '' COMMENT 'VIP结束时间',
  `dropString` varchar(180) NOT NULL,
  `club` int(11) NOT NULL,
  `club_7_time` int(11) NOT NULL,
  `club_10_time` int(11) NOT NULL,
  `drop_time` int(11) DEFAULT NULL,
  `buy_time` int(11) DEFAULT NULL,
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_vip
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_chr_vip_join`
-- ----------------------------
DROP TABLE IF EXISTS `mem_chr_vip_join`;
CREATE TABLE `mem_chr_vip_join` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '角色id',
  `code` varchar(40) NOT NULL DEFAULT '0' COMMENT '邀请码',
  `flag` int(11) NOT NULL DEFAULT '0' COMMENT '是否被用',
  `name` varchar(20) NOT NULL DEFAULT '0' COMMENT '名字',
  PRIMARY KEY (`cid`,`code`),
  KEY `slot` (`code`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_chr_vip_join
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_city_war`
-- ----------------------------
DROP TABLE IF EXISTS `mem_city_war`;
CREATE TABLE `mem_city_war` (
  `index` int(11) NOT NULL DEFAULT '0' COMMENT '场次',
  `familyid` bigint(20) DEFAULT '0' COMMENT '胜利帮派ID',
  `familyname` varchar(30) NOT NULL DEFAULT '' COMMENT '帮派名称',
  `leader` bigint(20) NOT NULL DEFAULT '0' COMMENT '帮主ID',
  `leadername` varchar(30) NOT NULL DEFAULT '' COMMENT '帮主名称',
  `wintime` int(11) NOT NULL DEFAULT '0' COMMENT '连胜次数',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '军团战时间',
  `first` bigint(20) NOT NULL,
  `second` bigint(20) NOT NULL,
  `third` bigint(20) NOT NULL,
  `firstfamilyname` varchar(30) NOT NULL,
  `secondfamilyname` varchar(30) DEFAULT NULL,
  `thirdfamilyname` varchar(30) DEFAULT NULL,
  PRIMARY KEY (`index`),
  KEY `owner` (`time`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_city_war
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_city_war_apply`
-- ----------------------------
DROP TABLE IF EXISTS `mem_city_war_apply`;
CREATE TABLE `mem_city_war_apply` (
  `index` int(11) NOT NULL DEFAULT '0' COMMENT '场次',
  `applyinfo` varchar(1000) NOT NULL DEFAULT '0' COMMENT '胜利帮派ID',
  PRIMARY KEY (`index`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_city_war_apply
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_cross_family_war`
-- ----------------------------
DROP TABLE IF EXISTS `mem_cross_family_war`;
CREATE TABLE `mem_cross_family_war` (
  `index` int(11) NOT NULL AUTO_INCREMENT,
  `familyid` bigint(20) NOT NULL DEFAULT '0' COMMENT '胜利帮派ID',
  `killer` bigint(20) NOT NULL DEFAULT '0' COMMENT '杀神',
  `scorer` bigint(20) NOT NULL DEFAULT '0' COMMENT '劳模',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '军团战时间',
  PRIMARY KEY (`index`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_cross_family_war
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_cross_tower`
-- ----------------------------
DROP TABLE IF EXISTS `mem_cross_tower`;
CREATE TABLE `mem_cross_tower` (
  `id` int(11) NOT NULL DEFAULT '0' COMMENT '活动ID',
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '王者id',
  `name` varchar(20) NOT NULL DEFAULT '' COMMENT '王者名字',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '成为王者的时间',
  `battle` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `owner` (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_cross_tower
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_cycle_task`
-- ----------------------------
DROP TABLE IF EXISTS `mem_cycle_task`;
CREATE TABLE `mem_cycle_task` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `finishtimes` int(11) NOT NULL DEFAULT '0' COMMENT '完成次数',
  `taskid` int(11) NOT NULL DEFAULT '0' COMMENT '当前任务ID',
  `state` tinyint(4) NOT NULL DEFAULT '0' COMMENT '任务状态',
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_cycle_task
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_double_eleven_rank`
-- ----------------------------
DROP TABLE IF EXISTS `mem_double_eleven_rank`;
CREATE TABLE `mem_double_eleven_rank` (
  `type` tinyint(4) NOT NULL DEFAULT '0' COMMENT '活动类型',
  `day` int(11) NOT NULL DEFAULT '0' COMMENT '天数',
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `name` varchar(20) NOT NULL DEFAULT '' COMMENT '名称',
  `count` int(11) NOT NULL DEFAULT '0' COMMENT '次数',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '操作时间',
  PRIMARY KEY (`type`,`day`,`cid`),
  KEY `type` (`type`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_double_eleven_rank
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_drop_control`
-- ----------------------------
DROP TABLE IF EXISTS `mem_drop_control`;
CREATE TABLE `mem_drop_control` (
  `group_id` int(11) NOT NULL DEFAULT '0' COMMENT '掉落组',
  `times` int(11) NOT NULL DEFAULT '0' COMMENT '次数',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '时间',
  PRIMARY KEY (`group_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_drop_control
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_drop_control_days`
-- ----------------------------
DROP TABLE IF EXISTS `mem_drop_control_days`;
CREATE TABLE `mem_drop_control_days` (
  `group_id` int(11) NOT NULL DEFAULT '0' COMMENT '掉落组',
  `times` int(11) NOT NULL DEFAULT '0' COMMENT '次数',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '时间',
  PRIMARY KEY (`group_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_drop_control_days
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_drop_record`
-- ----------------------------
DROP TABLE IF EXISTS `mem_drop_record`;
CREATE TABLE `mem_drop_record` (
  `index` int(11) NOT NULL AUTO_INCREMENT COMMENT '自增长标记',
  `name` varchar(20) NOT NULL DEFAULT '' COMMENT '击杀者名称',
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '击杀者ID',
  `mapid` int(11) NOT NULL DEFAULT '0' COMMENT '地图ID',
  `mid` int(11) NOT NULL DEFAULT '0' COMMENT '怪物ID',
  `record` int(11) NOT NULL DEFAULT '0' COMMENT '记录Index',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '时间',
  `special` int(11) NOT NULL DEFAULT '0' COMMENT '置顶标记',
  PRIMARY KEY (`index`),
  KEY `cid` (`cid`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=1252 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_drop_record
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_enemy`
-- ----------------------------
DROP TABLE IF EXISTS `mem_enemy`;
CREATE TABLE `mem_enemy` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `ecid` bigint(20) NOT NULL DEFAULT '0',
  `friend_type` char(8) NOT NULL DEFAULT '',
  KEY `cid` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_enemy
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_equip`
-- ----------------------------
DROP TABLE IF EXISTS `mem_equip`;
CREATE TABLE `mem_equip` (
  `id` bigint(20) NOT NULL DEFAULT '0' COMMENT '装备唯一ID',
  `base` int(11) NOT NULL DEFAULT '0' COMMENT '装备基础ID',
  `owner` bigint(20) NOT NULL DEFAULT '0',
  `star` int(11) NOT NULL DEFAULT '0' COMMENT '强化等级',
  `time` int(11) DEFAULT NULL,
  `mapid` int(11) DEFAULT NULL,
  `mid` int(11) DEFAULT NULL,
  `name` varchar(30) DEFAULT NULL,
  `lucky` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `owner` (`owner`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_equip
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_equip_back_count`
-- ----------------------------
DROP TABLE IF EXISTS `mem_equip_back_count`;
CREATE TABLE `mem_equip_back_count` (
  `id` int(11) NOT NULL COMMENT '自增长标记',
  `count` int(11) NOT NULL DEFAULT '0' COMMENT '怪物表ID',
  PRIMARY KEY (`id`),
  KEY `id` (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_equip_back_count
-- ----------------------------


-- ----------------------------
-- Table structure for `mem_equip_back_depot`
-- ----------------------------
DROP TABLE IF EXISTS `mem_equip_back_depot`;
CREATE TABLE `mem_equip_back_depot` (
  `nId` int(11) NOT NULL DEFAULT '0' COMMENT '角色ID',
  `nType` tinyint(4) NOT NULL DEFAULT '0',
  `nCount` int(11) NOT NULL DEFAULT '0' COMMENT '位置',
  `name` varchar(30) NOT NULL DEFAULT '0' COMMENT '符文孔',
  PRIMARY KEY (`nId`,`nType`),
  KEY `cid` (`nId`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_equip_back_depot
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_equip_back_record`
-- ----------------------------
DROP TABLE IF EXISTS `mem_equip_back_record`;
CREATE TABLE `mem_equip_back_record` (
  `nId` int(11) NOT NULL DEFAULT '0' COMMENT '角色ID',
  `nType` tinyint(4) NOT NULL DEFAULT '0' COMMENT '背包类型：\r\n0、装备栏\r\n1、背包',
  `nTime` int(11) NOT NULL DEFAULT '0' COMMENT '位置',
  `name` varchar(30) NOT NULL DEFAULT '0',
  KEY `cid` (`nId`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_equip_back_record
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_equip_rank`
-- ----------------------------
DROP TABLE IF EXISTS `mem_equip_rank`;
CREATE TABLE `mem_equip_rank` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `name` varchar(20) NOT NULL DEFAULT '0' COMMENT '名字',
  `count` int(11) NOT NULL DEFAULT '0' COMMENT '数量',
  `time` int(11) DEFAULT NULL,
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_equip_rank
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_exchange_code`
-- ----------------------------
DROP TABLE IF EXISTS `mem_exchange_code`;
CREATE TABLE `mem_exchange_code` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '序列',
  `code` char(32) NOT NULL DEFAULT '' COMMENT '兑换码',
  `item_id` int(11) NOT NULL DEFAULT '0' COMMENT '兑换物品',
  `repeat` tinyint(4) NOT NULL DEFAULT '0' COMMENT '是否能重复领取',
  `used` bigint(20) NOT NULL DEFAULT '0' COMMENT '使用情况',
  `name` char(8) NOT NULL DEFAULT '' COMMENT '使用者名',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '生成时间',
  PRIMARY KEY (`id`),
  UNIQUE KEY `code` (`code`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_exchange_code
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_family`
-- ----------------------------
DROP TABLE IF EXISTS `mem_family`;
CREATE TABLE `mem_family` (
  `id` bigint(20) NOT NULL DEFAULT '0' COMMENT '序列',
  `name` varchar(20) NOT NULL DEFAULT '' COMMENT '家族名',
  `leader_cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '家族长',
  `creator_cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '创建者',
  `create_time` int(11) NOT NULL DEFAULT '0' COMMENT '创建时间',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT '等级',
  `exp` int(11) NOT NULL DEFAULT '0' COMMENT '经验值',
  `member_card` int(11) NOT NULL DEFAULT '0' COMMENT '军团人数扩张卡',
  `notice` varchar(100) NOT NULL DEFAULT '' COMMENT '公告',
  `count` int(11) NOT NULL DEFAULT '0' COMMENT '人数',
  `auto_agree` tinyint(4) NOT NULL DEFAULT '0' COMMENT '自动同意申请',
  `task_count` int(11) NOT NULL DEFAULT '0' COMMENT '军团任务完成数量',
  `change_leader_time` int(11) NOT NULL DEFAULT '0' COMMENT '军团长转移时间',
  `boss_point` int(11) NOT NULL DEFAULT '0' COMMENT 'boss培养',
  `boss_state` tinyint(4) NOT NULL DEFAULT '0' COMMENT 'boss召唤',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '存储时间',
  `delflag` tinyint(4) NOT NULL DEFAULT '0' COMMENT '删除标记',
  `del_time` int(11) NOT NULL DEFAULT '0' COMMENT '解散时间',
  `sid` int(11) DEFAULT NULL,
  `territory_open` tinyint(4) NOT NULL DEFAULT '0',
  `medl_level` int(4) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `name` (`name`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_family
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_family_dungeon`
-- ----------------------------
DROP TABLE IF EXISTS `mem_family_dungeon`;
CREATE TABLE `mem_family_dungeon` (
  `id` bigint(20) NOT NULL DEFAULT '0' COMMENT '军团ID',
  `times` int(4) NOT NULL DEFAULT '0' COMMENT '这个星期完成次数',
  `start_time` int(11) NOT NULL DEFAULT '0' COMMENT '开始时间',
  `finish_hard` int(4) unsigned NOT NULL DEFAULT '0' COMMENT '通关难度',
  PRIMARY KEY (`id`),
  KEY `time` (`start_time`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_family_dungeon
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_family_log`
-- ----------------------------
DROP TABLE IF EXISTS `mem_family_log`;
CREATE TABLE `mem_family_log` (
  `id` bigint(20) NOT NULL COMMENT '序列',
  `type` tinyint(4) NOT NULL,
  `param1` bigint(20) NOT NULL DEFAULT '0' COMMENT '参数1',
  `param2` bigint(20) NOT NULL DEFAULT '0' COMMENT '参数2',
  `param3` bigint(20) NOT NULL DEFAULT '0' COMMENT '参数3',
  `param4` bigint(20) NOT NULL DEFAULT '0' COMMENT '参数4',
  `param5` bigint(20) NOT NULL DEFAULT '0' COMMENT '参数5',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '操作时间',
  KEY `time` (`time`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_family_log
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_family_record`
-- ----------------------------
DROP TABLE IF EXISTS `mem_family_record`;
CREATE TABLE `mem_family_record` (
  `id` bigint(20) NOT NULL COMMENT '序列',
  `type` tinyint(4) NOT NULL,
  `cid` bigint(20) DEFAULT NULL,
  `name` varchar(20) DEFAULT NULL,
  `count` int(11) NOT NULL DEFAULT '0' COMMENT '参数1',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '操作时间',
  KEY `time` (`time`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_family_record
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_flop_draw`
-- ----------------------------
DROP TABLE IF EXISTS `mem_flop_draw`;
CREATE TABLE `mem_flop_draw` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `act_type` int(11) NOT NULL DEFAULT '0',
  `flop_id` int(11) NOT NULL DEFAULT '0',
  `result` int(11) DEFAULT NULL,
  PRIMARY KEY (`cid`,`act_type`,`flop_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_flop_draw
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_global_chou_jiang_record`
-- ----------------------------
DROP TABLE IF EXISTS `mem_global_chou_jiang_record`;
CREATE TABLE `mem_global_chou_jiang_record` (
  `Cid` bigint(20) DEFAULT NULL,
  `Name` varchar(50) DEFAULT NULL,
  `ItemClass` int(4) DEFAULT NULL,
  `ItemId` int(11) DEFAULT NULL,
  `ItemCount` int(11) DEFAULT NULL,
  KEY `cid` (`Cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_global_chou_jiang_record
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_global_luck_point`
-- ----------------------------
DROP TABLE IF EXISTS `mem_global_luck_point`;
CREATE TABLE `mem_global_luck_point` (
  `Id` int(11) NOT NULL DEFAULT '0',
  `GlobalLuckPint` int(11) unsigned zerofill NOT NULL DEFAULT '00000000000' COMMENT '抽奖全服幸运值',
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_global_luck_point
-- ----------------------------
INSERT INTO mem_global_luck_point VALUES ('1', '00000000000');

-- ----------------------------
-- Table structure for `mem_gui_gu_dao_ren`
-- ----------------------------
DROP TABLE IF EXISTS `mem_gui_gu_dao_ren`;
CREATE TABLE `mem_gui_gu_dao_ren` (
  `npcid` int(11) NOT NULL AUTO_INCREMENT COMMENT '自增长标记',
  `count` int(11) NOT NULL DEFAULT '0' COMMENT '怪物表ID',
  PRIMARY KEY (`npcid`),
  KEY `id` (`npcid`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=3007 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_gui_gu_dao_ren
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_hall_of_fame`
-- ----------------------------
DROP TABLE IF EXISTS `mem_hall_of_fame`;
CREATE TABLE `mem_hall_of_fame` (
  `fame_index` int(11) NOT NULL DEFAULT '0' COMMENT '名次',
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `last_cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '上次结算',
  `reward` tinyint(4) NOT NULL DEFAULT '0' COMMENT '领取标记',
  PRIMARY KEY (`fame_index`),
  KEY `fame_index` (`fame_index`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_hall_of_fame
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_limit_shop`
-- ----------------------------
DROP TABLE IF EXISTS `mem_limit_shop`;
CREATE TABLE `mem_limit_shop` (
  `ShopId` int(11) NOT NULL DEFAULT '0',
  `LimitCount` int(11) DEFAULT NULL,
  `RefreshTime` int(11) DEFAULT NULL,
  PRIMARY KEY (`ShopId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_limit_shop
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_little_helper`
-- ----------------------------
DROP TABLE IF EXISTS `mem_little_helper`;
CREATE TABLE `mem_little_helper` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '玩家ID',
  `work_id` int(11) NOT NULL DEFAULT '0' COMMENT '包裹编号\r\n装备栏=0\r\n背包=1\r\n仓库=2\r\n',
  `act_info` varchar(100) DEFAULT '0' COMMENT '格子',
  PRIMARY KEY (`cid`),
  KEY `cidbag` (`cid`,`work_id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_little_helper
-- ----------------------------


-- ----------------------------
-- Table structure for `mem_mail`
-- ----------------------------
DROP TABLE IF EXISTS `mem_mail`;
CREATE TABLE `mem_mail` (
  `MailId` int(11) NOT NULL AUTO_INCREMENT COMMENT '邮件id',
  `SysMailId` int(11) NOT NULL DEFAULT '0' COMMENT '系统邮件ID',
  `SenderId` bigint(20) NOT NULL DEFAULT '0' COMMENT '发送者cid',
  `SenderName` varchar(20) NOT NULL DEFAULT '' COMMENT '发送者名称',
  `ReceiveId` bigint(20) NOT NULL DEFAULT '0' COMMENT '接收者cid',
  `ReceiveName` varchar(20) NOT NULL DEFAULT '' COMMENT '接收者名称',
  `SendTime` int(11) NOT NULL DEFAULT '0' COMMENT '发送时间',
  `HasRead` tinyint(4) NOT NULL DEFAULT '0' COMMENT '已读标记',
  `Extract` int(11) NOT NULL DEFAULT '0' COMMENT '领取标记',
  `MailTitle` varchar(30) NOT NULL DEFAULT '' COMMENT '邮件标题',
  `MailContent` varchar(500) NOT NULL DEFAULT '' COMMENT '邮件内容',
  `Item1` varchar(80) NOT NULL DEFAULT '' COMMENT '附件1',
  `Item2` varchar(80) NOT NULL DEFAULT '' COMMENT '附件2',
  `Item3` varchar(80) NOT NULL DEFAULT '' COMMENT '附件3',
  `Item4` varchar(80) NOT NULL DEFAULT '' COMMENT '附件4',
  `Item5` varchar(80) NOT NULL DEFAULT '' COMMENT '附件5',
  `Item6` varchar(80) NOT NULL DEFAULT '' COMMENT '附件6',
  `reason` int(11) NOT NULL DEFAULT '0' COMMENT '道具出处',
  `Param` varchar(300) NOT NULL DEFAULT '' COMMENT '附加参数',
  `LoadFlag` tinyint(4) NOT NULL DEFAULT '0' COMMENT '0 新邮件\r\n1 已加载邮件',
  `DelFlag` tinyint(4) NOT NULL DEFAULT '0' COMMENT '0 未删除\r\n1 玩家删除\r\n2 超时自动删除',
  `DelTime` int(11) NOT NULL DEFAULT '0' COMMENT '删除时间',
  PRIMARY KEY (`MailId`),
  UNIQUE KEY `MailId` (`MailId`) USING BTREE,
  KEY `ReceiveId` (`ReceiveId`) USING BTREE,
  KEY `LoadFlag` (`LoadFlag`) USING BTREE,
  KEY `DelFlag` (`DelFlag`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_mail
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_mobile_phone_gift`
-- ----------------------------
DROP TABLE IF EXISTS `mem_mobile_phone_gift`;
CREATE TABLE `mem_mobile_phone_gift` (
  `qid` varchar(32) NOT NULL DEFAULT '' COMMENT '平台ID',
  `sid` int(11) NOT NULL DEFAULT '0',
  `type` tinyint(4) NOT NULL,
  `state` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`qid`,`sid`,`type`),
  KEY `uid` (`sid`,`qid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_mobile_phone_gift
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_money_reward_task`
-- ----------------------------
DROP TABLE IF EXISTS `mem_money_reward_task`;
CREATE TABLE `mem_money_reward_task` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `finishtimes` int(11) NOT NULL DEFAULT '0' COMMENT '完成次数',
  `getreward` int(11) NOT NULL DEFAULT '0' COMMENT '当前任务ID',
  `taskinfo` varchar(300) NOT NULL DEFAULT '0' COMMENT '任务状态',
  `TaskId` int(11) DEFAULT NULL,
  `Star` int(11) DEFAULT NULL,
  `RandStarTimes` int(11) DEFAULT NULL,
  `PdbfFinishTimes` int(11) DEFAULT NULL,
  `EquipBackTaskId` int(11) DEFAULT NULL,
  `EquipBackTaskFinishTimes` int(11) DEFAULT NULL,
  `RandEquipBackTaskStarTimes` int(11) DEFAULT NULL,
  `XiangYaoTask` varchar(300) DEFAULT NULL,
  `RefreshTimes` int(11) DEFAULT NULL,
  `XiangYaoFinishTimes` int(11) DEFAULT NULL,
  `ShenWeiTask` varchar(300) DEFAULT NULL,
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_money_reward_task
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_open_beta_rank`
-- ----------------------------
DROP TABLE IF EXISTS `mem_open_beta_rank`;
CREATE TABLE `mem_open_beta_rank` (
  `type` tinyint(4) NOT NULL DEFAULT '0' COMMENT '活动类型',
  `day` int(11) NOT NULL DEFAULT '0' COMMENT '天数',
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `name` varchar(20) NOT NULL DEFAULT '' COMMENT '名称',
  `count` int(11) NOT NULL DEFAULT '0' COMMENT '次数',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '操作时间',
  PRIMARY KEY (`type`,`day`,`cid`),
  KEY `type` (`type`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_open_beta_rank
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_peerless_war`
-- ----------------------------
DROP TABLE IF EXISTS `mem_peerless_war`;
CREATE TABLE `mem_peerless_war` (
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '活动时间',
  `winner` bigint(20) NOT NULL DEFAULT '0' COMMENT '胜利者',
  PRIMARY KEY (`time`),
  KEY `time` (`time`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_peerless_war
-- ----------------------------


-- ----------------------------
-- Table structure for `mem_pet`
-- ----------------------------
DROP TABLE IF EXISTS `mem_pet`;
CREATE TABLE `mem_pet` (
  `pid` bigint(20) NOT NULL DEFAULT '0' COMMENT '宠物ID',
  `owner` bigint(20) NOT NULL DEFAULT '0' COMMENT '所有者ID',
  `baseid` int(11) NOT NULL DEFAULT '0' COMMENT '宠物配置ID',
  `name` char(30) NOT NULL DEFAULT '' COMMENT '名称',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT '等级',
  `exp` bigint(20) NOT NULL DEFAULT '0' COMMENT '经验',
  `points` int(11) NOT NULL DEFAULT '0' COMMENT '评分',
  `wash_points` int(11) NOT NULL DEFAULT '0' COMMENT '洗练评分',
  `battle` int(11) NOT NULL DEFAULT '0' COMMENT '战斗力',
  `potential` int(11) NOT NULL DEFAULT '0' COMMENT '潜力值',
  `rein` int(11) NOT NULL DEFAULT '0' COMMENT '转生次数',
  `awaken` int(11) NOT NULL DEFAULT '0' COMMENT '觉醒次数',
  `number` int(11) NOT NULL DEFAULT '0' COMMENT '身份牌',
  `phase` tinyint(4) NOT NULL DEFAULT '0' COMMENT '相性',
  `lucky` int(11) NOT NULL DEFAULT '0' COMMENT '幸运值',
  `start_hp` int(11) NOT NULL DEFAULT '0',
  `start_phy_atk_min` int(11) NOT NULL DEFAULT '0',
  `start_phy_atk_max` int(11) NOT NULL DEFAULT '0',
  `start_phy_def` int(11) NOT NULL DEFAULT '0',
  `start_mag_atk_min` int(11) NOT NULL DEFAULT '0',
  `start_mag_atk_max` int(11) NOT NULL DEFAULT '0',
  `start_mag_def` int(11) NOT NULL DEFAULT '0',
  `start_dodge` int(11) NOT NULL DEFAULT '0',
  `start_hitrate` int(11) NOT NULL DEFAULT '0',
  `start_critrate` int(11) NOT NULL DEFAULT '0',
  `start_tenacity` int(11) NOT NULL DEFAULT '0',
  `grow_hp` int(11) NOT NULL DEFAULT '0',
  `grow_phy_atk_min` int(11) NOT NULL DEFAULT '0',
  `grow_phy_atk_max` int(11) NOT NULL DEFAULT '0',
  `grow_phy_def` int(11) NOT NULL DEFAULT '0',
  `grow_mag_atk_min` int(11) NOT NULL DEFAULT '0',
  `grow_mag_atk_max` int(11) NOT NULL DEFAULT '0',
  `grow_mag_def` int(11) NOT NULL DEFAULT '0',
  `grow_dodge` int(11) NOT NULL DEFAULT '0',
  `grow_hitrate` int(11) NOT NULL DEFAULT '0',
  `grow_critrate` int(11) NOT NULL DEFAULT '0',
  `grow_tenacity` int(11) NOT NULL DEFAULT '0',
  `records` varchar(120) NOT NULL DEFAULT '' COMMENT '记录',
  `skills` varchar(300) NOT NULL DEFAULT '' COMMENT '技能',
  `wash_attr` varchar(60) NOT NULL DEFAULT '' COMMENT '洗练属性',
  `equips` varchar(300) NOT NULL DEFAULT '' COMMENT '装备',
  `hide_title` tinyint(4) NOT NULL DEFAULT '0' COMMENT '隐藏称号',
  `fromway` tinyint(4) NOT NULL DEFAULT '0' COMMENT '来源 0 普通 1 出生携带',
  `bornflag` tinyint(4) NOT NULL DEFAULT '0' COMMENT '出生标记',
  `delflag` tinyint(4) NOT NULL DEFAULT '0' COMMENT '删除标记',
  PRIMARY KEY (`pid`),
  KEY `pid` (`pid`) USING BTREE,
  KEY `cid` (`owner`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_pet
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_red_packet`
-- ----------------------------
DROP TABLE IF EXISTS `mem_red_packet`;
CREATE TABLE `mem_red_packet` (
  `id` int(11) NOT NULL COMMENT '自增长标记',
  `sender` bigint(20) NOT NULL DEFAULT '0' COMMENT '发送者ID',
  `familyid` bigint(20) NOT NULL DEFAULT '0' COMMENT '帮派ID',
  `total` int(11) NOT NULL DEFAULT '0' COMMENT '总魔石',
  `times` int(11) NOT NULL DEFAULT '0' COMMENT '总次数',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '时间',
  `left` int(11) NOT NULL DEFAULT '0' COMMENT '剩余魔石',
  `flag` tinyint(4) NOT NULL DEFAULT '0' COMMENT '发回标记',
  `CurrencyType` int(11) NOT NULL DEFAULT '0' COMMENT '货币类型',
  PRIMARY KEY (`id`),
  KEY `leftflag` (`left`,`flag`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_red_packet
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_red_packet_bind`
-- ----------------------------
DROP TABLE IF EXISTS `mem_red_packet_bind`;
CREATE TABLE `mem_red_packet_bind` (
  `RedId` int(11) NOT NULL AUTO_INCREMENT COMMENT '红包ID',
  `owner` bigint(20) NOT NULL DEFAULT '0' COMMENT '人物ID',
  `Total` int(10) NOT NULL DEFAULT '0' COMMENT '发送金额',
  `times` int(10) NOT NULL DEFAULT '0' COMMENT '红包数量',
  `delFig` int(10) NOT NULL DEFAULT '0' COMMENT '删除标志位',
  PRIMARY KEY (`RedId`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_red_packet_bind
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_red_packet_record`
-- ----------------------------
DROP TABLE IF EXISTS `mem_red_packet_record`;
CREATE TABLE `mem_red_packet_record` (
  `index` int(11) NOT NULL AUTO_INCREMENT COMMENT '自增长标记',
  `type` tinyint(4) NOT NULL DEFAULT '0' COMMENT '类型',
  `id` int(11) NOT NULL DEFAULT '0' COMMENT '红包ID',
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '领取人ID',
  `familyid` bigint(20) NOT NULL DEFAULT '0' COMMENT '军团ID',
  `value` int(11) NOT NULL DEFAULT '0' COMMENT '获得魔石',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '时间',
  `CurrencyType` int(11) NOT NULL DEFAULT '0' COMMENT '货币类型',
  PRIMARY KEY (`index`),
  KEY `familyidtime` (`familyid`,`time`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_red_packet_record
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_rong_he_record`
-- ----------------------------
DROP TABLE IF EXISTS `mem_rong_he_record`;
CREATE TABLE `mem_rong_he_record` (
  `nid` int(11) NOT NULL AUTO_INCREMENT COMMENT '用户ID',
  `cid` bigint(20) DEFAULT NULL,
  `name` varchar(20) DEFAULT NULL,
  `cost_id` int(11) DEFAULT NULL,
  `give_id` int(11) DEFAULT NULL,
  `success` int(11) DEFAULT NULL,
  `time` int(11) DEFAULT NULL,
  PRIMARY KEY (`nid`),
  KEY `cid` (`nid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_rong_he_record
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_score_shop_record`
-- ----------------------------
DROP TABLE IF EXISTS `mem_score_shop_record`;
CREATE TABLE `mem_score_shop_record` (
  `Cid` bigint(20) DEFAULT NULL,
  `Name` varchar(50) DEFAULT NULL,
  `ItemClass` int(4) DEFAULT NULL,
  `ItemId` int(11) DEFAULT NULL,
  `ItemCount` int(11) DEFAULT NULL,
  KEY `cid` (`Cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_score_shop_record
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_tencent_draw_record`
-- ----------------------------
DROP TABLE IF EXISTS `mem_tencent_draw_record`;
CREATE TABLE `mem_tencent_draw_record` (
  `index` int(11) NOT NULL AUTO_INCREMENT COMMENT '自增长标记',
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `name` varchar(20) NOT NULL DEFAULT '' COMMENT '名称',
  `id` int(11) NOT NULL DEFAULT '0' COMMENT '抽中Id',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '时间',
  PRIMARY KEY (`index`),
  KEY `nametime` (`name`,`time`) USING BTREE,
  KEY `time` (`time`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_tencent_draw_record
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_tencent_draw_reward`
-- ----------------------------
DROP TABLE IF EXISTS `mem_tencent_draw_reward`;
CREATE TABLE `mem_tencent_draw_reward` (
  `index` int(11) NOT NULL COMMENT '项目',
  `count` int(11) NOT NULL DEFAULT '0' COMMENT '抽中次数',
  PRIMARY KEY (`index`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_tencent_draw_reward
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_te_quan`
-- ----------------------------
DROP TABLE IF EXISTS `mem_te_quan`;
CREATE TABLE `mem_te_quan` (
  `sid` int(11) NOT NULL AUTO_INCREMENT COMMENT '自增长标记',
  `uid` bigint(20) NOT NULL DEFAULT '0' COMMENT '怪物表ID',
  `type` int(11) NOT NULL DEFAULT '0' COMMENT '类型',
  `level` int(20) NOT NULL DEFAULT '0' COMMENT '击杀者',
  KEY `uid` (`sid`,`uid`,`type`,`level`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_te_quan
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_trailer`
-- ----------------------------
DROP TABLE IF EXISTS `mem_trailer`;
CREATE TABLE `mem_trailer` (
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `JieBiaoTimes` int(11) NOT NULL DEFAULT '0',
  `YaBiaoTimes` int(11) NOT NULL DEFAULT '0' COMMENT '数量',
  `TrailerQuality` int(11) NOT NULL,
  `EndTime` int(11) NOT NULL,
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_trailer
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_world_boss_info`
-- ----------------------------
DROP TABLE IF EXISTS `mem_world_boss_info`;
CREATE TABLE `mem_world_boss_info` (
  `id` int(11) NOT NULL DEFAULT '0' COMMENT 'BOSSID',
  `level` int(11) NOT NULL DEFAULT '0' COMMENT 'BOSS等级',
  `exp` int(11) NOT NULL DEFAULT '0' COMMENT '经验值',
  `killer1` bigint(20) NOT NULL DEFAULT '0' COMMENT '击杀者',
  `killer_name1` varchar(20) NOT NULL DEFAULT '' COMMENT '击杀者名称',
  `kill_time1` int(11) NOT NULL DEFAULT '0' COMMENT '击杀时间',
  `killer2` bigint(20) NOT NULL DEFAULT '0' COMMENT '击杀者',
  `killer_name2` varchar(20) NOT NULL DEFAULT '' COMMENT '击杀者名称',
  `kill_time2` int(11) NOT NULL DEFAULT '0' COMMENT '击杀时间',
  `killer3` bigint(20) NOT NULL DEFAULT '0' COMMENT '击杀者',
  `killer_name3` varchar(20) NOT NULL DEFAULT '' COMMENT '击杀者名称',
  `kill_time3` int(11) NOT NULL DEFAULT '0' COMMENT '击杀时间',
  `killer4` bigint(20) NOT NULL DEFAULT '0' COMMENT '击杀者',
  `killer_name4` varchar(20) NOT NULL DEFAULT '' COMMENT '击杀者名称',
  `kill_time4` int(11) NOT NULL DEFAULT '0' COMMENT '击杀时间',
  `killer5` bigint(20) NOT NULL DEFAULT '0' COMMENT '击杀者',
  `killer_name5` varchar(20) NOT NULL DEFAULT '' COMMENT '击杀者名称',
  `kill_time5` int(11) NOT NULL DEFAULT '0' COMMENT '击杀时间',
  PRIMARY KEY (`id`),
  KEY `owner` (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_world_boss_info
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_wu_hun`
-- ----------------------------
DROP TABLE IF EXISTS `mem_wu_hun`;
CREATE TABLE `mem_wu_hun` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '用户ID',
  `refresh_times` int(11) DEFAULT NULL,
  `last_refresh_time` int(11) DEFAULT NULL,
  `wu_hun` varchar(750) DEFAULT NULL,
  PRIMARY KEY (`cid`),
  KEY `cid` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_wu_hun
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_wu_hun_shop`
-- ----------------------------
DROP TABLE IF EXISTS `mem_wu_hun_shop`;
CREATE TABLE `mem_wu_hun_shop` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '用户ID',
  `shopid` int(11) NOT NULL DEFAULT '0',
  `item_index` varchar(50) DEFAULT NULL,
  `item_state` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`cid`,`shopid`),
  KEY `cid` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_wu_hun_shop
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_xin_mo`
-- ----------------------------
DROP TABLE IF EXISTS `mem_xin_mo`;
CREATE TABLE `mem_xin_mo` (
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '用户ID',
  `exp` int(11) DEFAULT NULL,
  `xin_mo_level` int(11) DEFAULT NULL,
  `xin_mo_act_level` int(11) DEFAULT NULL,
  `xin_qing_info` int(11) DEFAULT NULL,
  `qi_qing_level` varchar(80) DEFAULT NULL,
  PRIMARY KEY (`cid`),
  KEY `cid` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_xin_mo
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_yellow_stone`
-- ----------------------------
DROP TABLE IF EXISTS `mem_yellow_stone`;
CREATE TABLE `mem_yellow_stone` (
  `cid` bigint(20) NOT NULL,
  `id` int(11) NOT NULL COMMENT 'cfg_yellow_stone 的 id',
  `get_time` int(11) NOT NULL COMMENT '领取时间'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_yellow_stone
-- ----------------------------

-- ----------------------------
-- Table structure for `mem_zhyyhd_rank`
-- ----------------------------
DROP TABLE IF EXISTS `mem_zhyyhd_rank`;
CREATE TABLE `mem_zhyyhd_rank` (
  `type` tinyint(4) NOT NULL DEFAULT '0' COMMENT '活动类型',
  `day` int(11) NOT NULL DEFAULT '0' COMMENT '天数',
  `cid` bigint(20) NOT NULL DEFAULT '0',
  `name` varchar(20) NOT NULL DEFAULT '' COMMENT '名称',
  `count` int(11) NOT NULL DEFAULT '0' COMMENT '次数',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '操作时间',
  PRIMARY KEY (`type`,`day`,`cid`),
  KEY `type` (`type`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of mem_zhyyhd_rank
-- ----------------------------

-- ----------------------------
-- Table structure for `pay_key`
-- ----------------------------
DROP TABLE IF EXISTS `pay_key`;
CREATE TABLE `pay_key` (
  `ip` char(32) NOT NULL DEFAULT '',
  `key` char(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`ip`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of pay_key
-- ----------------------------

-- ----------------------------
-- Table structure for `pay_log`
-- ----------------------------
DROP TABLE IF EXISTS `pay_log`;
CREATE TABLE `pay_log` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '序列',
  `oid` char(64) NOT NULL DEFAULT '' COMMENT '订单号',
  `sid` int(11) NOT NULL DEFAULT '0' COMMENT '服务器id',
  `uid` bigint(20) NOT NULL DEFAULT '0' COMMENT '账户id',
  `passport` char(32) NOT NULL DEFAULT '' COMMENT '账户名',
  `cid` bigint(20) NOT NULL DEFAULT '0' COMMENT '角色id',
  `name` char(8) NOT NULL DEFAULT '' COMMENT '角色名',
  `amount` int(11) NOT NULL DEFAULT '0' COMMENT '充值金额',
  `time` int(11) NOT NULL DEFAULT '0' COMMENT '时间',
  `level` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `oid` (`oid`) USING BTREE,
  KEY `cid` (`cid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of pay_log
-- ----------------------------

-- ----------------------------
-- Table structure for `sys_server_config`
-- ----------------------------
DROP TABLE IF EXISTS `sys_server_config`;
CREATE TABLE `sys_server_config` (
  `name` varchar(32) NOT NULL,
  `value` varchar(32) NOT NULL,
  `refresh_time` int(11) NOT NULL,
  PRIMARY KEY (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of sys_server_config
-- ----------------------------
INSERT INTO sys_server_config VALUES ('FESTIVAL_2_VERSION', '2019041800', '1575462307');
INSERT INTO sys_server_config VALUES ('FESTIVAL_VERSION', '2019110700', '1575462307');
INSERT INTO sys_server_config VALUES ('KAI_FU_LIMIT_8', '2', '0');
INSERT INTO sys_server_config VALUES ('KAI_FU_LIMIT_9', '1', '0');

-- ----------------------------
-- Table structure for `sys_server_start`
-- ----------------------------
DROP TABLE IF EXISTS `sys_server_start`;
CREATE TABLE `sys_server_start` (
  `type` tinyint(4) NOT NULL DEFAULT '0',
  `kai_fu_time` int(11) DEFAULT '0' COMMENT '服务器开服时间',
  `he_fu_time` int(11) DEFAULT NULL,
  `combined_service_times` int(11) DEFAULT '0' COMMENT '合服次数',
  PRIMARY KEY (`type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of sys_server_start
-- ----------------------------
INSERT INTO sys_server_start VALUES ('0', '1575457200', '0', '0');

-- ----------------------------
-- Table structure for `sys_unique_id`
-- ----------------------------
DROP TABLE IF EXISTS `sys_unique_id`;
CREATE TABLE `sys_unique_id` (
  `sid` int(11) NOT NULL DEFAULT '0' COMMENT '服务器ID',
  `cid` int(11) NOT NULL DEFAULT '0' COMMENT '人物ID',
  `eid` int(11) NOT NULL DEFAULT '0' COMMENT '装备ID',
  `pid` int(11) NOT NULL DEFAULT '0' COMMENT '宠物ID',
  `fid` int(11) NOT NULL DEFAULT '0' COMMENT '军团ID',
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of sys_unique_id
-- ----------------------------


-- ----------------------------
-- Table structure for `sys_user`
-- ----------------------------
DROP TABLE IF EXISTS `sys_user`;
CREATE TABLE `sys_user` (
  `uid` bigint(20) NOT NULL DEFAULT '0' COMMENT '用户ID',
  `sid` int(11) NOT NULL DEFAULT '1',
  `passport` char(32) NOT NULL DEFAULT '' COMMENT '通行证，不区分大小写',
  `password` char(32) NOT NULL DEFAULT '' COMMENT '密码（测试使用）',
  `gm_gold` int(11) NOT NULL,
  `gold` int(11) NOT NULL DEFAULT '0' COMMENT '元宝',
  `sys_gold` int(11) NOT NULL DEFAULT '0' COMMENT '系统元宝',
  `gold_pay` int(11) NOT NULL DEFAULT '1',
  `gold_pay_total` int(11) NOT NULL DEFAULT '0' COMMENT '充值总数',
  `prepay_gold` int(11) NOT NULL DEFAULT '0' COMMENT '删档服换算到真实服的元宝',
  `gold_cost_total` int(11) NOT NULL DEFAULT '0' COMMENT '消费总数',
  `first_week_cost` int(11) NOT NULL DEFAULT '0',
  `second_week_pay` int(11) NOT NULL DEFAULT '0',
  `later_pay` int(11) NOT NULL DEFAULT '0',
  `last_pay_time` int(11) NOT NULL DEFAULT '0',
  `cash` int(11) NOT NULL DEFAULT '0' COMMENT '礼金',
  `create_time` int(11) NOT NULL DEFAULT '0' COMMENT '创建时间',
  `chr_list_time` int(11) NOT NULL DEFAULT '0',
  `chr_create_time` int(11) NOT NULL DEFAULT '0',
  `map_enter_time` int(11) NOT NULL DEFAULT '0',
  `last_login_time` int(11) NOT NULL DEFAULT '0' COMMENT '上次登陆时间',
  `first_login_ip` char(16) NOT NULL DEFAULT '' COMMENT '首次登陆IP',
  `last_login_ip` char(16) NOT NULL DEFAULT '' COMMENT '上次登陆IP',
  `adult` int(11) NOT NULL DEFAULT '1',
  `type` int(11) NOT NULL DEFAULT '0' COMMENT 'GM等级',
  `continue_login_count` int(11) NOT NULL DEFAULT '0' COMMENT '持续登陆天数',
  `total_login_count` int(11) NOT NULL DEFAULT '0' COMMENT '总登陆天数',
  `last_logout_time` int(11) NOT NULL DEFAULT '0',
  `total_online_time` int(11) NOT NULL DEFAULT '0',
  `total_offline_time` int(11) NOT NULL DEFAULT '0',
  `load_create_chr_ui` int(11) NOT NULL DEFAULT '0' COMMENT '创角ui加载完成标记',
  `start_game` int(11) NOT NULL DEFAULT '0' COMMENT '开始游戏标记',
  `fuli` tinyint(4) NOT NULL DEFAULT '0',
  `iopenid` char(32) NOT NULL DEFAULT '' COMMENT '邀请人id',
  `robot` tinyint(4) NOT NULL DEFAULT '0' COMMENT '机器人标记',
  `mini_client` tinyint(4) NOT NULL DEFAULT '0' COMMENT '微端登录',
  `net_bar` tinyint(4) NOT NULL DEFAULT '0' COMMENT '网吧登录',
  `platform` char(32) NOT NULL DEFAULT '' COMMENT '平台标记',
  `from_way` char(32) DEFAULT NULL,
  `jiankong` tinyint(4) DEFAULT NULL,
  UNIQUE KEY `siduid` (`uid`,`sid`) USING BTREE,
  KEY `passport` (`passport`) USING BTREE,
  KEY `sid` (`sid`) USING BTREE,
  KEY `uid` (`uid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of sys_user
-- ----------------------------

-- ----------------------------
-- Table structure for `sys_user_prevent_wallow`
-- ----------------------------
DROP TABLE IF EXISTS `sys_user_prevent_wallow`;
CREATE TABLE `sys_user_prevent_wallow` (
  `uid` bigint(20) NOT NULL DEFAULT '0' COMMENT '用户id',
  `sid` int(11) NOT NULL DEFAULT '0' COMMENT '服务id',
  `name` char(31) NOT NULL DEFAULT '' COMMENT '姓名',
  `identitycard` char(19) NOT NULL DEFAULT '' COMMENT '身份证号码',
  `isGrowUp` int(11) NOT NULL DEFAULT '0' COMMENT '是否成年: 0未填写 1.成年 2.未成年人',
  PRIMARY KEY (`uid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of sys_user_prevent_wallow
-- ----------------------------


-- ----------------------------
-- Table structure for `sys_user_token`
-- ----------------------------
DROP TABLE IF EXISTS `sys_user_token`;
CREATE TABLE `sys_user_token` (
  `uid` bigint(20) NOT NULL DEFAULT '0' COMMENT '用户id',
  `sid` int(11) NOT NULL DEFAULT '0' COMMENT '服务id',
  `token` char(32) NOT NULL DEFAULT '' COMMENT '随机生成的token',
  `create_time` int(11) NOT NULL DEFAULT '0' COMMENT '生成时间',
  `pf` varchar(100) NOT NULL DEFAULT '' COMMENT '进入标记',
  `is_yellow_vip` int(11) NOT NULL DEFAULT '0' COMMENT '是否是黄钻用户',
  `is_yellow_year_vip` int(11) NOT NULL DEFAULT '0' COMMENT '是否是年费黄钻用户',
  `yellow_vip_level` int(11) NOT NULL DEFAULT '0' COMMENT '黄钻用户等级',
  `is_yellow_high_vip` int(11) NOT NULL DEFAULT '0' COMMENT '是否是豪华黄钻用户',
  `is_blue_vip` int(11) NOT NULL DEFAULT '0' COMMENT '是否是蓝钻用户',
  `is_blue_year_vip` int(11) NOT NULL DEFAULT '0' COMMENT '是否是年费蓝钻用户',
  `blue_vip_level` int(11) NOT NULL DEFAULT '0' COMMENT '蓝钻用户等级',
  `is_blue_high_vip` int(11) NOT NULL DEFAULT '0' COMMENT '是否是豪华蓝钻用户',
  `year_vip_valid_time` int(11) NOT NULL,
  `super_vip_valid_time` int(11) NOT NULL,
  `expend_vip_valid_time` int(11) NOT NULL,
  PRIMARY KEY (`uid`,`sid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of sys_user_token
-- ----------------------------

-- ----------------------------
-- Procedure structure for `GetPayGold`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GetPayGold`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GetPayGold`(in InUid bigint, in InSid int, out OutPayGold int)
    SQL SECURITY INVOKER
BEGIN
	DECLARE pay int default 0;
	DECLARE lock_result int;
	IF get_lock('sp_critical_section_lock',60) THEN
		SELECT sys_user.gold_pay into pay FROM sys_user WHERE sys_user.uid=InUid AND sys_user.sid=InSid LIMIT 1; 
		IF pay>0 THEN
			SET OutPayGold = pay;
			UPDATE sys_user SET sys_user.gold = sys_user.gold+pay, sys_user.gold_pay_total = sys_user.gold_pay_total+pay, sys_user.gold_pay=sys_user.gold_pay-pay WHERE sys_user.uid=InUid AND sys_user.sid=InSid;
			SELECT OutPayGold;
		ELSE
			SET OutPayGold = 0;
			SELECT OutPayGold;
		END IF;
		SET lock_result=release_lock('sp_critical_section_lock'); 
	ELSE
		SET OutPayGold = 0;
		SELECT OutPayGold;
	END IF;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `NewCharId`
-- ----------------------------
DROP PROCEDURE IF EXISTS `NewCharId`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `NewCharId`(in ServerId int, out OutCharId int)
    SQL SECURITY INVOKER
begin
declare id int default 0;
declare lock_result int;
if get_lock('sp_critical_section_lock',60) then
select sys_unique_id.cid into id from sys_unique_id WHERE sys_unique_id.sid=ServerId LIMIT 1; 
if id>0 then
	set OutCharId = id;
	update sys_unique_id set sys_unique_id.cid = id+1 where sys_unique_id.sid=ServerId;
	select OutCharId;
ELSE
	set OutCharId = 1;
	INSERT INTO sys_unique_id(sid,cid,eid,pid,fid) VALUES (ServerId,2,1,1,1) ON DUPLICATE KEY UPDATE cid=2;
	SELECT OutCharId;
end if;
set lock_result=release_lock('sp_critical_section_lock'); 
else
set OutCharId = 0;
select OutCharId;
end if;	
end
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `NewEquipId`
-- ----------------------------
DROP PROCEDURE IF EXISTS `NewEquipId`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `NewEquipId`(in ServerId int, out OutEquipId int)
    SQL SECURITY INVOKER
begin
declare id int default 0;
declare lock_result int;
if get_lock('sp_critical_section_lock',60) then
select sys_unique_id.eid into id from sys_unique_id WHERE sys_unique_id.sid=ServerId LIMIT 1; 
if id>0 then
	set OutEquipId = id;
	update sys_unique_id set sys_unique_id.eid = id+20 where sys_unique_id.sid=ServerId;
	select OutEquipId;
ELSE
	set OutEquipId = 1;
	INSERT INTO sys_unique_id(sid,cid,eid,pid,fid) VALUES (ServerId,1,21,1,1) ON DUPLICATE KEY UPDATE eid=21;
	SELECT OutEquipId;
end if;
set lock_result=release_lock('sp_critical_section_lock'); 
else
set OutEquipId = 0;
select OutEquipId;
end if;	
end
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `NewFamilyId`
-- ----------------------------
DROP PROCEDURE IF EXISTS `NewFamilyId`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `NewFamilyId`(in ServerId int, out OutFamilyId int)
    SQL SECURITY INVOKER
begin
declare id int default 0;
declare lock_result int;
if get_lock('sp_critical_section_lock',60) then
select sys_unique_id.fid into id from sys_unique_id WHERE sys_unique_id.sid=ServerId LIMIT 1; 
if id>0 then
	set OutFamilyId = id;
	update sys_unique_id set sys_unique_id.fid = id+1 where sys_unique_id.sid=ServerId;
	select OutFamilyId;
ELSE
	set OutFamilyId = 1;
	INSERT INTO sys_unique_id(sid,cid,eid,pid,fid) VALUES (ServerId,1,1,1,2) ON DUPLICATE KEY UPDATE fid=2;
	SELECT OutFamilyId;
end if;
set lock_result=release_lock('sp_critical_section_lock'); 
else
set OutFamilyId = 0;
select OutFamilyId;
end if;	
end
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `NewPetId`
-- ----------------------------
DROP PROCEDURE IF EXISTS `NewPetId`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `NewPetId`(in ServerId int, out OutPetId int)
    SQL SECURITY INVOKER
begin
declare id int default 0;
declare lock_result int;
if get_lock('sp_critical_section_lock',60) then
select sys_unique_id.pid into id from sys_unique_id WHERE sys_unique_id.sid=ServerId LIMIT 1; 
if id>0 then
	set OutPetId = id;
	update sys_unique_id set sys_unique_id.pid = id+20 where sys_unique_id.sid=ServerId;
	select OutPetId;
ELSE
	set OutPetId = 1;
	INSERT INTO sys_unique_id(sid,cid,eid,pid,fid) VALUES (ServerId,1,1,21,1) ON DUPLICATE KEY UPDATE pid=21;
	SELECT OutPetId;
end if;
set lock_result=release_lock('sp_critical_section_lock'); 
else
set OutPetId = 0;
select OutPetId;
end if;	
end
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `NewUid`
-- ----------------------------
DROP PROCEDURE IF EXISTS `NewUid`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `NewUid`(in InSid int, out OutUid bigint)
    SQL SECURITY INVOKER
begin
declare id bigint default 0;
declare lock_result int;
if get_lock('sp_critical_section_lock',60) then
	select max(sys_user.uid) into id from sys_user;

	if id>0 then
		set OutUid = id+1;
	else
		set OutUid = 1;
	end if;

	INSERT INTO sys_user(uid,sid) VALUES (OutUid,InSid);
	SELECT OutUid;

	set lock_result=release_lock('sp_critical_section_lock'); 
else
	set OutUid = 0;
	select OutUid;
end if;	
end
;;
DELIMITER ;
