--
-- SQL修改脚本
--
-- 注意点:
-- 	1. 大版本发出后,备份并清空 modify_database-{date}.sql
-- 	2. 修改语句在提交后不能合并
--

/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;

CREATE TABLE `Item_0` (
	`id` BIGINT(20) UNSIGNED NOT NULL,
	`roleid` BIGINT(20) UNSIGNED NOT NULL,
	`baseid` INT(10) UNSIGNED NOT NULL DEFAULT '0',
	`count` BIGINT(19) NOT NULL DEFAULT '0',
	`place` TINYINT(3) SIGNED NOT NULL DEFAULT '0',
	`create_time` BIGINT(20) SIGNED NOT NULL DEFAULT '0',
	PRIMARY KEY (`id`) USING BTREE,
	INDEX `idx_roleid` (`roleid`) USING BTREE
) COMMENT='物品表' COLLATE='utf8mb4_0900_ai_ci' ENGINE=InnoDB ;

CREATE TABLE `Item_1` (
	`id` BIGINT(20) UNSIGNED NOT NULL,
	`roleid` BIGINT(20) UNSIGNED NOT NULL,
	`baseid` INT(10) UNSIGNED NOT NULL DEFAULT '0',
	`count` BIGINT(19) NOT NULL DEFAULT '0',
	`place` TINYINT(3) SIGNED NOT NULL DEFAULT '0',
	`create_time` BIGINT(20) SIGNED NOT NULL DEFAULT '0',
	PRIMARY KEY (`id`) USING BTREE,
	INDEX `idx_roleid` (`roleid`) USING BTREE
) COMMENT='物品表' COLLATE='utf8mb4_0900_ai_ci' ENGINE=InnoDB ;

CREATE TABLE `Item_2` (
	`id` BIGINT(20) UNSIGNED NOT NULL,
	`roleid` BIGINT(20) UNSIGNED NOT NULL,
	`baseid` INT(10) UNSIGNED NOT NULL DEFAULT '0',
	`count` BIGINT(19) NOT NULL DEFAULT '0',
	`place` TINYINT(3) SIGNED NOT NULL DEFAULT '0',
	`create_time` BIGINT(20) SIGNED NOT NULL DEFAULT '0',
	PRIMARY KEY (`id`) USING BTREE,
	INDEX `idx_roleid` (`roleid`) USING BTREE
) COMMENT='物品表' COLLATE='utf8mb4_0900_ai_ci' ENGINE=InnoDB ;

CREATE TABLE `Item_3` (
	`id` BIGINT(20) UNSIGNED NOT NULL,
	`roleid` BIGINT(20) UNSIGNED NOT NULL,
	`baseid` INT(10) UNSIGNED NOT NULL DEFAULT '0',
	`count` BIGINT(19) NOT NULL DEFAULT '0',
	`place` TINYINT(3) SIGNED NOT NULL DEFAULT '0',
	`create_time` BIGINT(20) SIGNED NOT NULL DEFAULT '0',
	PRIMARY KEY (`id`) USING BTREE,
	INDEX `idx_roleid` (`roleid`) USING BTREE
) COMMENT='物品表' COLLATE='utf8mb4_0900_ai_ci' ENGINE=InnoDB ;

CREATE TABLE `HeroBase` (
	`id` bigint(20) UNSIGNED NOT NULL,
	`roleid` BIGINT(20) UNSIGNED NOT NULL COMMENT '所属角色',
	`baseid` INT(10) UNSIGNED NOT NULL COMMENT '基础ID',
	`expiretime` BIGINT(19) NULL DEFAULT '0' COMMENT '试用伙伴的过期时间',
	`exp` BIGINT(19) NULL DEFAULT '0' COMMENT '伙伴经验',
	`level` INT(10) SIGNED NULL DEFAULT '0' COMMENT '伙伴等级',
	`star` INT(10) SIGNED NULL DEFAULT '0' COMMENT '伙伴星级',
	`grade` INT(10) SIGNED NULL DEFAULT '0' COMMENT '伙伴进阶等级',
	`quality` INT(10) SIGNED NULL DEFAULT '0' COMMENT '伙伴突破等级',
	`rate` INT(10) SIGNED NULL DEFAULT '0' COMMENT '伙伴评级',
	`skill` VARCHAR(1024) NULL DEFAULT '' COMMENT '伙伴技能列表' COLLATE 'utf8mb4_0900_ai_ci',
	PRIMARY KEY (`id`),
	INDEX `idx_roleid` (`roleid`) USING BTREE
) COMMENT='伙伴表' COLLATE='utf8mb4_0900_ai_ci' ENGINE=InnoDB ;

ALTER TABLE `CharBrief`
	ADD COLUMN `storeaccount` VARCHAR(128) NOT NULL DEFAULT '' COMMENT '商店账号' AFTER `account`;

ALTER TABLE `HeroBase`
	CHANGE COLUMN `baseid` `baseid` INT(10) NOT NULL COMMENT '基础ID' AFTER `roleid`;

ALTER TABLE `CharBase`
	ADD COLUMN `opensystems` VARCHAR(1024) NULL DEFAULT '' COMMENT '开放的系统列表' AFTER `guildcoin`;

CREATE TABLE `RoleDaily` (
	`roleid` BIGINT(20) UNSIGNED NOT NULL COMMENT '角色ID',
	`module` INT(11) NOT NULL COMMENT '模块ID',
	`timestamp` BIGINT(20) NOT NULL DEFAULT 0 COMMENT '时间戳',
	`content` TEXT COMMENT '隔天的数据',
	`extracontent` TEXT COMMENT '不隔天的数据',
	PRIMARY KEY (`roleid`, `module`),
	INDEX `idx_roleid` (`roleid`)
) COMMENT='玩家日常信息表' COLLATE='utf8mb4_0900_ai_ci' ENGINE=InnoDB;

ALTER TABLE `Settings`
	CHANGE COLUMN `roleid` `roleid` BIGINT(20) UNSIGNED NOT NULL FIRST,
	CHANGE COLUMN `onlinedays` `onlinedays` INT(10) NOT NULL DEFAULT 0 COMMENT '累计登陆天数' AFTER `importaccount`,
	CHANGE COLUMN `gamecoin` `gamecoin` BIGINT(20) NOT NULL DEFAULT 0 COMMENT '累计获得金币' AFTER `onlinedays`,
	CHANGE COLUMN `strength` `strength` BIGINT(20) NOT NULL DEFAULT 0 COMMENT '累计使用体力' AFTER `gamecoin`,
	CHANGE COLUMN `towerfloor` `towerfloor` INT(11) NOT NULL DEFAULT 0 COMMENT '累计爬塔层数' AFTER `strength`,
	CHANGE COLUMN `pay` `pay` BIGINT(20) NOT NULL DEFAULT 0 COMMENT '累积支付' AFTER `towerfloor`,
	ADD COLUMN `paytimes` BIGINT(20) NOT NULL DEFAULT 0 COMMENT '累积支付次数' AFTER `pay`,
	CHANGE COLUMN `gainmoney` `gainmoney` BIGINT(20) NOT NULL DEFAULT 0 COMMENT '累积获得钻石' AFTER `paytimes`,
	CHANGE COLUMN `consumemoney` `consumemoney` BIGINT(20) NOT NULL DEFAULT 0 COMMENT '累积消耗钻石' AFTER `gainmoney`,
	CHANGE COLUMN `setting` `setting` VARCHAR(1024) NOT NULL DEFAULT '' COMMENT '设置内容' COLLATE 'utf8mb4_0900_ai_ci' AFTER `consumemoney`,
	CHANGE COLUMN `gsettings` `gsettings` BIGINT(20) NULL DEFAULT 0 COMMENT '服务器相关设置' AFTER `setting`,
	CHANGE COLUMN `guide` `guide` VARCHAR(1024) NOT NULL DEFAULT '' COMMENT '新手引导信息' COLLATE 'utf8mb4_0900_ai_ci' AFTER `gsettings`,
	CHANGE COLUMN `rnregister` `rnregister` INT(10) NULL DEFAULT 0 COMMENT '实名认证的奖励' AFTER `guide`,
	CHANGE COLUMN `refunds` `refunds` INT(10) NULL DEFAULT 0 COMMENT '累积退款金额' AFTER `rnregister`;

CREATE TABLE `Formation` (
	`roleid` BIGINT(20) UNSIGNED NOT NULL COMMENT '角色ID',
	`type` INT(11) NOT NULL COMMENT '阵容类型',
	`order` INT(11) NOT NULL COMMENT '内部顺序',
	`name` VARCHAR(50) NULL DEFAULT '' COMMENT '自定义名称',
	`formation` VARCHAR(256) NULL DEFAULT '' COMMENT '阵容具体信息',
	`helpsite` VARCHAR(50) NULL DEFAULT '' COMMENT '支援位配置',
	`ultimate` VARCHAR(50) NULL DEFAULT '' COMMENT '奥义技能',
	`comboflow` VARCHAR(50) NULL DEFAULT '' COMMENT '连招配置',
	PRIMARY KEY (`roleid`, `type`, `order`),
	INDEX `idx_roleid` (`roleid`)
) COMMENT='阵容数据' COLLATE='utf8mb4_0900_ai_ci' ENGINE=InnoDB ;

ALTER TABLE `Attribute_0`
	ADD COLUMN `id` BIGINT(20) UNSIGNED NOT NULL COMMENT 'HeroID' FIRST,
	ADD COLUMN `order` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT '阵容内部序号' AFTER `formation`,
	CHANGE COLUMN `planet` `baseid` INT(10) UNSIGNED NULL DEFAULT '0' COMMENT '配置ID' AFTER `battlepoint`,
	DROP COLUMN `dimension`,
	DROP COLUMN `buildings`,
	DROP PRIMARY KEY,
	DROP INDEX `idx_roleid`,
	ADD PRIMARY KEY (`id`, `formation`, `order`),
	ADD INDEX `idx_roleid` (`roleid`),
	ADD INDEX `idx_secondary` (`formation`, `order`);

ALTER TABLE `Attribute_1`
	ADD COLUMN `id` BIGINT(20) UNSIGNED NOT NULL COMMENT 'HeroID' FIRST,
	ADD COLUMN `order` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT '阵容内部序号' AFTER `formation`,
	CHANGE COLUMN `planet` `baseid` INT(10) UNSIGNED NULL DEFAULT '0' COMMENT '配置ID' AFTER `battlepoint`,
	DROP COLUMN `dimension`,
	DROP COLUMN `buildings`,
	DROP PRIMARY KEY,
	DROP INDEX `idx_roleid`,
	ADD PRIMARY KEY (`id`, `formation`, `order`),
	ADD INDEX `idx_roleid` (`roleid`),
	ADD INDEX `idx_secondary` (`formation`, `order`);

ALTER TABLE `Attribute_2`
	ADD COLUMN `id` BIGINT(20) UNSIGNED NOT NULL COMMENT 'HeroID' FIRST,
	ADD COLUMN `order` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT '阵容内部序号' AFTER `formation`,
	CHANGE COLUMN `planet` `baseid` INT(10) UNSIGNED NULL DEFAULT '0' COMMENT '配置ID' AFTER `battlepoint`,
	DROP COLUMN `dimension`,
	DROP COLUMN `buildings`,
	DROP PRIMARY KEY,
	DROP INDEX `idx_roleid`,
	ADD PRIMARY KEY (`id`, `formation`, `order`),
	ADD INDEX `idx_roleid` (`roleid`),
	ADD INDEX `idx_secondary` (`formation`, `order`);

ALTER TABLE `Attribute_3`
	ADD COLUMN `id` BIGINT(20) UNSIGNED NOT NULL COMMENT 'HeroID' FIRST,
	ADD COLUMN `order` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT '阵容内部序号' AFTER `formation`,
	CHANGE COLUMN `planet` `baseid` INT(10) UNSIGNED NULL DEFAULT '0' COMMENT '配置ID' AFTER `battlepoint`,
	DROP COLUMN `dimension`,
	DROP COLUMN `buildings`,
	DROP PRIMARY KEY,
	DROP INDEX `idx_roleid`,
	ADD PRIMARY KEY (`id`, `formation`, `order`),
	ADD INDEX `idx_roleid` (`roleid`),
	ADD INDEX `idx_secondary` (`formation`, `order`);

CREATE TABLE `Chapter` (
  `roleid` bigint unsigned NOT NULL COMMENT '角色id',
  `chapter` int unsigned NOT NULL DEFAULT '0' COMMENT '篇幅id',
  `star` int unsigned NOT NULL DEFAULT '0' COMMENT '完成星数',
  `stages` varchar(512) DEFAULT '' COMMENT '阶段信息列表',
  PRIMARY KEY (`roleid`,`chapter`),
  KEY `idx_roleid` (`roleid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='章节信息';

CREATE TABLE `Dungeon` (
  `roleid` bigint unsigned NOT NULL COMMENT '角色id',
  `dungeon` int unsigned NOT NULL COMMENT '关卡id',
  `grade` int unsigned NOT NULL DEFAULT '0' COMMENT '关卡评级',
  `times` int NOT NULL DEFAULT '0' COMMENT '今日通关次数',
  `trytimes` int NOT NULL DEFAULT '0' COMMENT '尝试次数',
  `buytimes` int NOT NULL DEFAULT '0' COMMENT '今日购买次数',
  `record` varchar(512) DEFAULT '' COMMENT '纪录信息',
  PRIMARY KEY (`roleid`,`dungeon`),
  KEY `idx_roleid` (`roleid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='关卡表';

ALTER TABLE `CharBrief`
	CHANGE COLUMN `status` `status` TINYINT(1) NOT NULL DEFAULT 1 COMMENT '账号状态,0-禁用1-启用' AFTER `storeaccount`,
	CHANGE COLUMN `name` `name` VARCHAR(128) NOT NULL DEFAULT '' COMMENT '角色名' COLLATE 'utf8mb4_0900_ai_ci' AFTER `status`,
	CHANGE COLUMN `zone` `zone` SMALLINT(5) UNSIGNED NOT NULL DEFAULT 0 COMMENT '区服ID' AFTER `name`,
	CHANGE COLUMN `createtime` `createtime` BIGINT(19) NOT NULL DEFAULT 0 COMMENT '创角时间' AFTER `zone`,
	CHANGE COLUMN `level` `level` INT(10) UNSIGNED NULL DEFAULT 1 COMMENT '角色等级' AFTER `createtime`,
	CHANGE COLUMN `viplevel` `viplevel` INT(10) UNSIGNED NULL DEFAULT 0 COMMENT 'VIP等级' AFTER `level`,
	CHANGE COLUMN `avatar` `avatar` INT(10) UNSIGNED NULL DEFAULT 0 COMMENT '头像' AFTER `viplevel`,
	CHANGE COLUMN `battlepoint` `battlepoint` BIGINT(20) UNSIGNED NULL DEFAULT 0 COMMENT '最强战斗力' AFTER `avatar`,
	CHANGE COLUMN `lastercity` `lastercity` VARCHAR(64) NULL DEFAULT '' COMMENT '最近所在城镇' COLLATE 'utf8mb4_0900_ai_ci' AFTER `battlepoint`,
	CHANGE COLUMN `lastlogintime` `lastlogintime` BIGINT(19) NULL DEFAULT 0 COMMENT '最近登录时间' AFTER `lastercity`,
	CHANGE COLUMN `lastlogouttime` `lastlogouttime` BIGINT(19) NULL DEFAULT 0 COMMENT '最近登出时间' AFTER `lastlogintime`,
	CHANGE COLUMN `bannedtime` `bannedtime` BIGINT(19) NULL DEFAULT 0 COMMENT '封号过期时间' AFTER `lastlogouttime`,
	CHANGE COLUMN `shutuptime` `shutuptime` BIGINT(19) NULL DEFAULT 0 COMMENT '禁言过期时间' AFTER `bannedreason`,
	ADD COLUMN `mainprogress` VARCHAR(256) NULL DEFAULT '' COMMENT '主线进度' AFTER `opensystems`,
	CHANGE COLUMN `friendscount` `friendscount` INT(10) UNSIGNED NULL DEFAULT 0 COMMENT '好友个数' AFTER `mainprogress`,
	CHANGE COLUMN `settings` `settings` BIGINT(20) UNSIGNED NULL DEFAULT 0 COMMENT '相关功能设置' AFTER `friendscount`,
	DROP COLUMN `gwtimes`,
	DROP COLUMN `gwtotalstat`,
	DROP COLUMN `bossdrawtime`;

CREATE TABLE `CombatVideo` (
	`id` BIGINT(20) UNSIGNED NOT NULL COMMENT '战斗ID',
	`hostid` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT '服务器ID',
	`dungeon` INT(10) NOT NULL DEFAULT '0' COMMENT '关卡ID',
	`roleid` BIGINT(20) UNSIGNED NOT NULL COMMENT '角色ID',
	`timestamp` BIGINT(19) NOT NULL DEFAULT '0' COMMENT '时间戳',
	`version` VARCHAR(50) NOT NULL DEFAULT '' COMMENT '版本号' COLLATE 'utf8mb4_0900_ai_ci',
	`result` INT(10) NOT NULL DEFAULT '0' COMMENT '战斗结果',
	`attacker` VARCHAR(2048) NOT NULL DEFAULT '' COMMENT '攻击方信息' COLLATE 'utf8mb4_0900_ai_ci',
	`defender` VARCHAR(2048) NOT NULL DEFAULT '' COMMENT '防御方信息' COLLATE 'utf8mb4_0900_ai_ci',
	`video` TEXT NOT NULL COMMENT '回放数据' COLLATE 'utf8mb4_0900_ai_ci',
	PRIMARY KEY (`id`) USING BTREE
) COMMENT='战报记录' COLLATE='utf8mb4_0900_ai_ci' ENGINE=InnoDB ;

ALTER TABLE `Attribute_0`
	ADD COLUMN `attribute` VARCHAR(1024) NULL DEFAULT '' COMMENT '属性列表' AFTER `skills`,
	DROP COLUMN `maxhp`,
	DROP COLUMN `attack`,
	DROP COLUMN `defence`,
	DROP COLUMN `strength`,
	DROP COLUMN `energy`,
	DROP COLUMN `hit`,
	DROP COLUMN `dodge`,
	DROP COLUMN `crit`,
	DROP COLUMN `critresist`,
	DROP COLUMN `amplifycritdmg`,
	DROP COLUMN `reducecritdmg`,
	DROP COLUMN `effecthit`,
	DROP COLUMN `effectresist`,
	DROP COLUMN `bonusheal`,
	DROP COLUMN `bonusbehealed`,
	DROP COLUMN `defbreak`,
	DROP COLUMN `parry`,
	DROP COLUMN `amplifydamage`,
	DROP COLUMN `reducedamage`;

ALTER TABLE `Attribute_1`
	ADD COLUMN `attribute` VARCHAR(1024) NULL DEFAULT '' COMMENT '属性列表' AFTER `skills`,
	DROP COLUMN `maxhp`,
	DROP COLUMN `attack`,
	DROP COLUMN `defence`,
	DROP COLUMN `strength`,
	DROP COLUMN `energy`,
	DROP COLUMN `hit`,
	DROP COLUMN `dodge`,
	DROP COLUMN `crit`,
	DROP COLUMN `critresist`,
	DROP COLUMN `amplifycritdmg`,
	DROP COLUMN `reducecritdmg`,
	DROP COLUMN `effecthit`,
	DROP COLUMN `effectresist`,
	DROP COLUMN `bonusheal`,
	DROP COLUMN `bonusbehealed`,
	DROP COLUMN `defbreak`,
	DROP COLUMN `parry`,
	DROP COLUMN `amplifydamage`,
	DROP COLUMN `reducedamage`;

ALTER TABLE `Attribute_2`
	ADD COLUMN `attribute` VARCHAR(1024) NULL DEFAULT '' COMMENT '属性列表' AFTER `skills`,
	DROP COLUMN `maxhp`,
	DROP COLUMN `attack`,
	DROP COLUMN `defence`,
	DROP COLUMN `strength`,
	DROP COLUMN `energy`,
	DROP COLUMN `hit`,
	DROP COLUMN `dodge`,
	DROP COLUMN `crit`,
	DROP COLUMN `critresist`,
	DROP COLUMN `amplifycritdmg`,
	DROP COLUMN `reducecritdmg`,
	DROP COLUMN `effecthit`,
	DROP COLUMN `effectresist`,
	DROP COLUMN `bonusheal`,
	DROP COLUMN `bonusbehealed`,
	DROP COLUMN `defbreak`,
	DROP COLUMN `parry`,
	DROP COLUMN `amplifydamage`,
	DROP COLUMN `reducedamage`;

ALTER TABLE `Attribute_3`
	ADD COLUMN `attribute` VARCHAR(1024) NULL DEFAULT '' COMMENT '属性列表' AFTER `skills`,
	DROP COLUMN `maxhp`,
	DROP COLUMN `attack`,
	DROP COLUMN `defence`,
	DROP COLUMN `strength`,
	DROP COLUMN `energy`,
	DROP COLUMN `hit`,
	DROP COLUMN `dodge`,
	DROP COLUMN `crit`,
	DROP COLUMN `critresist`,
	DROP COLUMN `amplifycritdmg`,
	DROP COLUMN `reducecritdmg`,
	DROP COLUMN `effecthit`,
	DROP COLUMN `effectresist`,
	DROP COLUMN `bonusheal`,
	DROP COLUMN `bonusbehealed`,
	DROP COLUMN `defbreak`,
	DROP COLUMN `parry`,
	DROP COLUMN `amplifydamage`,
	DROP COLUMN `reducedamage`;

ALTER TABLE `HeroBase`
	ADD COLUMN `locking` TINYINT(1) NULL DEFAULT '0' COMMENT '锁定状态' AFTER `rate`;

ALTER TABLE `CombatVideo`
	ADD COLUMN `statistics` VARCHAR(2048) NOT NULL DEFAULT '' COMMENT '数据统计' AFTER `defender`;

ALTER TABLE `RankList`
	ADD COLUMN `arena` VARCHAR(128) NULL DEFAULT '' COMMENT '竞技场' AFTER `roleid`,
	DROP COLUMN `trainscore`,
	DROP COLUMN `traintime`,
	DROP COLUMN `arenascore`,
	DROP COLUMN `arenatime`,
	DROP COLUMN `battlepoint`,
	DROP COLUMN `bptimestamp`,
	DROP COLUMN `arenarank`,
	DROP COLUMN `arenaseason`,
	DROP COLUMN `arenarankhistory`,
	DROP COLUMN `likes`,
	DROP COLUMN `liketimestamp`,
	DROP COLUMN `waves`,
	DROP COLUMN `wavestimestamp`;

CREATE TABLE `CombatJournal` (
	`id` BIGINT(20) UNSIGNED NOT NULL COMMENT '战斗ID',
	`roleid` BIGINT(20) UNSIGNED NOT NULL COMMENT '角色ID',
	`module` INT(10) NOT NULL DEFAULT '0' COMMENT '系统ID',
	`result` INT(10) NOT NULL DEFAULT '0' COMMENT '结果',
	`opponent` BIGINT(20) UNSIGNED NOT NULL DEFAULT '0' COMMENT '对手ID',
	`isrobot` INT(10) NOT NULL DEFAULT '0' COMMENT '是否是机器人',
	`avatar` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT '头像',
	`name` VARCHAR(128) NOT NULL DEFAULT '' COMMENT '对手名称' COLLATE 'utf8mb4_0900_ai_ci',
	`level` INT(10) NOT NULL DEFAULT '0' COMMENT '对手等级',
	`viplevel` INT(10) NOT NULL DEFAULT '0' COMMENT '对手VIP等级',
	`change` INT(10) NOT NULL DEFAULT '0' COMMENT '积分变化',
	`timestamp` BIGINT(19) NOT NULL DEFAULT '0' COMMENT '发生的时间戳',
	`statistics` VARCHAR(2048) NOT NULL DEFAULT '' COMMENT '数据统计' COLLATE 'utf8mb4_0900_ai_ci',
	`extrainfo` VARCHAR(1024) NOT NULL DEFAULT '' COMMENT '额外信息' COLLATE 'utf8mb4_0900_ai_ci',
	PRIMARY KEY (`id`) USING BTREE,
	INDEX `idx_roleid` (`roleid`) USING BTREE,
	INDEX `idx_secondary` (`module`)
) COMMENT='战斗日志' COLLATE='utf8mb4_0900_ai_ci' ENGINE=InnoDB ;

ALTER TABLE `Attribute_0`
	ADD COLUMN `helper` INT(10) UNSIGNED NULL DEFAULT '0' COMMENT '支援位' AFTER `baseid`;
ALTER TABLE `Attribute_1`
	ADD COLUMN `helper` INT(10) UNSIGNED NULL DEFAULT '0' COMMENT '支援位' AFTER `baseid`;
ALTER TABLE `Attribute_2`
	ADD COLUMN `helper` INT(10) UNSIGNED NULL DEFAULT '0' COMMENT '支援位' AFTER `baseid`;
ALTER TABLE `Attribute_3`
	ADD COLUMN `helper` INT(10) UNSIGNED NULL DEFAULT '0' COMMENT '支援位' AFTER `baseid`;

ALTER TABLE `Attribute_0`
	CHANGE COLUMN `formation` `formation` INT(10) NOT NULL DEFAULT '0' COMMENT '阵容类型' AFTER `roleid`,
	CHANGE COLUMN `order` `order` INT(10) NOT NULL DEFAULT '0' COMMENT '阵容内部序号' AFTER `formation`,
	CHANGE COLUMN `level` `level` INT(10) NOT NULL DEFAULT '0' COMMENT '等级' AFTER `order`,
	CHANGE COLUMN `baseid` `baseid` INT(10) NULL DEFAULT '0' COMMENT '配置ID' AFTER `battlepoint`,
	CHANGE COLUMN `helper` `helper` INT(10) NULL DEFAULT '0' COMMENT '支援位' AFTER `baseid`;
ALTER TABLE `Attribute_1`
	CHANGE COLUMN `formation` `formation` INT(10) NOT NULL DEFAULT '0' COMMENT '阵容类型' AFTER `roleid`,
	CHANGE COLUMN `order` `order` INT(10) NOT NULL DEFAULT '0' COMMENT '阵容内部序号' AFTER `formation`,
	CHANGE COLUMN `level` `level` INT(10) NOT NULL DEFAULT '0' COMMENT '等级' AFTER `order`,
	CHANGE COLUMN `baseid` `baseid` INT(10) NULL DEFAULT '0' COMMENT '配置ID' AFTER `battlepoint`,
	CHANGE COLUMN `helper` `helper` INT(10) NULL DEFAULT '0' COMMENT '支援位' AFTER `baseid`;
ALTER TABLE `Attribute_2`
	CHANGE COLUMN `formation` `formation` INT(10) NOT NULL DEFAULT '0' COMMENT '阵容类型' AFTER `roleid`,
	CHANGE COLUMN `order` `order` INT(10) NOT NULL DEFAULT '0' COMMENT '阵容内部序号' AFTER `formation`,
	CHANGE COLUMN `level` `level` INT(10) NOT NULL DEFAULT '0' COMMENT '等级' AFTER `order`,
	CHANGE COLUMN `baseid` `baseid` INT(10) NULL DEFAULT '0' COMMENT '配置ID' AFTER `battlepoint`,
	CHANGE COLUMN `helper` `helper` INT(10) NULL DEFAULT '0' COMMENT '支援位' AFTER `baseid`;
ALTER TABLE `Attribute_3`
	CHANGE COLUMN `formation` `formation` INT(10) NOT NULL DEFAULT '0' COMMENT '阵容类型' AFTER `roleid`,
	CHANGE COLUMN `order` `order` INT(10) NOT NULL DEFAULT '0' COMMENT '阵容内部序号' AFTER `formation`,
	CHANGE COLUMN `level` `level` INT(10) NOT NULL DEFAULT '0' COMMENT '等级' AFTER `order`,
	CHANGE COLUMN `baseid` `baseid` INT(10) NULL DEFAULT '0' COMMENT '配置ID' AFTER `battlepoint`,
	CHANGE COLUMN `helper` `helper` INT(10) NULL DEFAULT '0' COMMENT '支援位' AFTER `baseid`;

ALTER TABLE `Attribute_0`
	CHANGE COLUMN `baseid` `baseid` INT(10) NULL DEFAULT '0' COMMENT '配置ID' AFTER `order`,
	ADD COLUMN `star` INT(10) NULL DEFAULT '0' COMMENT '星级' AFTER `level`,
	ADD COLUMN `grade` INT(10) NULL DEFAULT '0' COMMENT '品阶' AFTER `star`,
	ADD COLUMN `quality` INT(10) NULL DEFAULT '0' COMMENT '品质' AFTER `grade`,
	ADD COLUMN `rate` INT(10) NULL DEFAULT '0' COMMENT '评级' AFTER `quality`,
	CHANGE COLUMN `helper` `helper` INT(10) NULL DEFAULT '0' COMMENT '支援位' AFTER `rate`;
ALTER TABLE `Attribute_1`
	CHANGE COLUMN `baseid` `baseid` INT(10) NULL DEFAULT '0' COMMENT '配置ID' AFTER `order`,
	ADD COLUMN `star` INT(10) NULL DEFAULT '0' COMMENT '星级' AFTER `level`,
	ADD COLUMN `grade` INT(10) NULL DEFAULT '0' COMMENT '品阶' AFTER `star`,
	ADD COLUMN `quality` INT(10) NULL DEFAULT '0' COMMENT '品质' AFTER `grade`,
	ADD COLUMN `rate` INT(10) NULL DEFAULT '0' COMMENT '评级' AFTER `quality`,
	CHANGE COLUMN `helper` `helper` INT(10) NULL DEFAULT '0' COMMENT '支援位' AFTER `rate`;
ALTER TABLE `Attribute_2`
	CHANGE COLUMN `baseid` `baseid` INT(10) NULL DEFAULT '0' COMMENT '配置ID' AFTER `order`,
	ADD COLUMN `star` INT(10) NULL DEFAULT '0' COMMENT '星级' AFTER `level`,
	ADD COLUMN `grade` INT(10) NULL DEFAULT '0' COMMENT '品阶' AFTER `star`,
	ADD COLUMN `quality` INT(10) NULL DEFAULT '0' COMMENT '品质' AFTER `grade`,
	ADD COLUMN `rate` INT(10) NULL DEFAULT '0' COMMENT '评级' AFTER `quality`,
	CHANGE COLUMN `helper` `helper` INT(10) NULL DEFAULT '0' COMMENT '支援位' AFTER `rate`;
ALTER TABLE `Attribute_3`
	CHANGE COLUMN `baseid` `baseid` INT(10) NULL DEFAULT '0' COMMENT '配置ID' AFTER `order`,
	ADD COLUMN `star` INT(10) NULL DEFAULT '0' COMMENT '星级' AFTER `level`,
	ADD COLUMN `grade` INT(10) NULL DEFAULT '0' COMMENT '品阶' AFTER `star`,
	ADD COLUMN `quality` INT(10) NULL DEFAULT '0' COMMENT '品质' AFTER `grade`,
	ADD COLUMN `rate` INT(10) NULL DEFAULT '0' COMMENT '评级' AFTER `quality`,
	CHANGE COLUMN `helper` `helper` INT(10) NULL DEFAULT '0' COMMENT '支援位' AFTER `rate`;

ALTER TABLE `SystemState`
	CHANGE COLUMN `state` `state` TEXT NOT NULL COMMENT '系统状态' COLLATE 'utf8mb4_0900_ai_ci' AFTER `timestamp`;

CREATE TABLE `Arena` (
	`roleid` BIGINT(20) UNSIGNED NOT NULL COMMENT '角色ID',
	`timestamp` BIGINT(20) UNSIGNED NULL DEFAULT 0 COMMENT '参与时间',
	`opponents` VARCHAR(256) NULL DEFAULT '' COMMENT '对手列表',
	`lastrank` INT(11) NULL DEFAULT 0 COMMENT '昨日排名',
	`drawtimestamp` BIGINT(20) NULL DEFAULT 0 COMMENT '每日奖励领取时间',
	PRIMARY KEY (`roleid`),
	INDEX `idx_roleid` (`roleid`)
) COMMENT='竞技场' COLLATE='utf8mb4_0900_ai_ci' ENGINE=InnoDB ;

ALTER TABLE `CharBase`
	CHANGE COLUMN `arenacoin` `arenacoin` BIGINT(19) NULL DEFAULT '0' COMMENT '竞技场券' AFTER `gamecoin`;

ALTER TABLE `SystemState`
	CHANGE COLUMN `state` `state` VARCHAR(1024) NOT NULL DEFAULT '' COMMENT '系统状态' COLLATE 'utf8mb4_0900_ai_ci' AFTER `timestamp`,
	ADD COLUMN `notes` TEXT NOT NULL COMMENT '备注信息' AFTER `state`;

ALTER TABLE `Arena`
	CHANGE COLUMN `timestamp` `timestamp` BIGINT(20) NULL DEFAULT '0' COMMENT '参与时间' AFTER `roleid`;

ALTER TABLE `CombatJournal`
	CHANGE COLUMN `isrobot` `robotid` INT(10) NOT NULL DEFAULT '0' COMMENT '机器人ID' AFTER `opponent`;

ALTER TABLE `Arena`
	ADD COLUMN `gettimestamp` BIGINT(19) NULL DEFAULT '0' COMMENT '昨日排名获取时间' AFTER `lastrank`;

ALTER TABLE `CharBrief`
	ADD COLUMN `arenastatus` TINYINT(1) NOT NULL DEFAULT '0' COMMENT '竞技场状态,0-未参与1-已参与' AFTER `settings`;

ALTER TABLE `Arena`
	ADD COLUMN `drawhltimestamp` BIGINT(19) NULL DEFAULT '0' COMMENT '高光时刻奖励领取时间' AFTER `drawtimestamp`;

ALTER TABLE `CombatVideo`
	CHANGE COLUMN `roleid` `creator` BIGINT(20) UNSIGNED NOT NULL DEFAULT '0' COMMENT '创建者' AFTER `dungeon`;

ALTER TABLE `RoleDaily`
	CHANGE COLUMN `content` `content` TEXT NOT NULL COMMENT '隔天的数据' COLLATE 'utf8mb4_0900_ai_ci' AFTER `timestamp`,
	CHANGE COLUMN `extracontent` `extracontent` TEXT NOT NULL COMMENT '不隔天的数据' COLLATE 'utf8mb4_0900_ai_ci' AFTER `content`;

CREATE TABLE `ShopGoods` (
	`id` INT(11) NOT NULL COMMENT '商品ID',
	`roleid` BIGINT(20) UNSIGNED NOT NULL COMMENT '角色ID',
	`shop` INT(11) NOT NULL DEFAULT 0 COMMENT '所属商店',
	`times` INT(11) NOT NULL DEFAULT 0 COMMENT '购买次数',
	`limittimes` INT(11) NOT NULL DEFAULT 0 COMMENT '限购次数',
	`timestamp` BIGINT(20) NOT NULL DEFAULT 0 COMMENT '限购时间',
	PRIMARY KEY (`id`, `roleid`),
	INDEX `idx_roleid` (`roleid`)
) COMMENT='商品表' COLLATE='utf8mb4_0900_ai_ci' ENGINE=InnoDB ;

CREATE TABLE `ShopMall` (
	`id` INT(10)  NOT NULL COMMENT '商店id',
	`roleid` BIGINT(20) UNSIGNED NOT NULL COMMENT '角色id',
	`createtime` BIGINT(19) NOT NULL DEFAULT '0' COMMENT '商店的创建时间',
	`refreshtime` BIGINT(19) NOT NULL DEFAULT '0' COMMENT '自动刷新时间',
	`manualtime` BIGINT(19) NOT NULL DEFAULT '0' COMMENT '手动刷新时间',
	`manualtimes` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT '手动刷新次数',
	PRIMARY KEY (`id`, `roleid`) USING BTREE,
	INDEX `idx_roleid` (`roleid`) USING BTREE
) COMMENT='商城表' COLLATE='utf8mb4_0900_ai_ci' ENGINE=InnoDB ;

CREATE TABLE `Equipment_0` (
	`id` BIGINT(20) UNSIGNED NOT NULL COMMENT '实体ID',
	`roleid` BIGINT(20) UNSIGNED NOT NULL COMMENT '角色ID',
	`baseid` INT(10) NULL DEFAULT '0' COMMENT '配置ID',
	`count` BIGINT(20) NULL DEFAULT '0' COMMENT '数量',
	`place` INT(10) NULL DEFAULT '0' COMMENT '存储位置',
	`createtime` BIGINT(20) NULL DEFAULT '0' COMMENT '创建时间',
	`exp` BIGINT(20) NULL DEFAULT '0' COMMENT '经验',
	`camp` INT(10) NULL DEFAULT '0' COMMENT '阵营标签',
	`heroid` BIGINT(20) NULL DEFAULT '0' COMMENT '所属英雄',
	PRIMARY KEY (`id`),
	INDEX `idx_roleid` (`roleid`)
)
COMMENT='装备表' COLLATE='utf8mb4_0900_ai_ci' ENGINE=InnoDB;

CREATE TABLE `Equipment_1` (
	`id` BIGINT(20) UNSIGNED NOT NULL COMMENT '实体ID',
	`roleid` BIGINT(20) UNSIGNED NOT NULL COMMENT '角色ID',
	`baseid` INT(10) NULL DEFAULT '0' COMMENT '配置ID',
	`count` BIGINT(20) NULL DEFAULT '0' COMMENT '数量',
	`place` INT(10) NULL DEFAULT '0' COMMENT '存储位置',
	`createtime` BIGINT(20) NULL DEFAULT '0' COMMENT '创建时间',
	`exp` BIGINT(20) NULL DEFAULT '0' COMMENT '经验',
	`camp` INT(10) NULL DEFAULT '0' COMMENT '阵营标签',
	`heroid` BIGINT(20) NULL DEFAULT '0' COMMENT '所属英雄',
	PRIMARY KEY (`id`),
	INDEX `idx_roleid` (`roleid`)
)
COMMENT='装备表' COLLATE='utf8mb4_0900_ai_ci' ENGINE=InnoDB;

CREATE TABLE `Equipment_2` (
	`id` BIGINT(20) UNSIGNED NOT NULL COMMENT '实体ID',
	`roleid` BIGINT(20) UNSIGNED NOT NULL COMMENT '角色ID',
	`baseid` INT(10) NULL DEFAULT '0' COMMENT '配置ID',
	`count` BIGINT(20) NULL DEFAULT '0' COMMENT '数量',
	`place` INT(10) NULL DEFAULT '0' COMMENT '存储位置',
	`createtime` BIGINT(20) NULL DEFAULT '0' COMMENT '创建时间',
	`exp` BIGINT(20) NULL DEFAULT '0' COMMENT '经验',
	`camp` INT(10) NULL DEFAULT '0' COMMENT '阵营标签',
	`heroid` BIGINT(20) NULL DEFAULT '0' COMMENT '所属英雄',
	PRIMARY KEY (`id`),
	INDEX `idx_roleid` (`roleid`)
)
COMMENT='装备表' COLLATE='utf8mb4_0900_ai_ci' ENGINE=InnoDB;

CREATE TABLE `Equipment_3` (
	`id` BIGINT(20) UNSIGNED NOT NULL COMMENT '实体ID',
	`roleid` BIGINT(20) UNSIGNED NOT NULL COMMENT '角色ID',
	`baseid` INT(10) NULL DEFAULT '0' COMMENT '配置ID',
	`count` BIGINT(20) NULL DEFAULT '0' COMMENT '数量',
	`place` INT(10) NULL DEFAULT '0' COMMENT '存储位置',
	`createtime` BIGINT(20) NULL DEFAULT '0' COMMENT '创建时间',
	`exp` BIGINT(20) NULL DEFAULT '0' COMMENT '经验',
	`camp` INT(10) NULL DEFAULT '0' COMMENT '阵营标签',
	`heroid` BIGINT(20) NULL DEFAULT '0' COMMENT '所属英雄',
	PRIMARY KEY (`id`),
	INDEX `idx_roleid` (`roleid`)
)
COMMENT='装备表' COLLATE='utf8mb4_0900_ai_ci' ENGINE=InnoDB;

