DROP TABLE IF EXISTS `Arena`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Arena` (
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `timestamp` bigint DEFAULT '0' COMMENT '参与时间',
  `opponents` varchar(256) DEFAULT '' COMMENT '对手列表',
  `lastrank` int DEFAULT '0' COMMENT '昨日排名',
  `gettimestamp` bigint DEFAULT '0' COMMENT '昨日排名获取时间',
  `drawtimestamp` bigint DEFAULT '0' COMMENT '每日奖励领取时间',
  `drawhltimestamp` bigint DEFAULT '0' COMMENT '高光时刻奖励领取时间',
  PRIMARY KEY (`roleid`),
  KEY `idx_roleid` (`roleid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='竞技场';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Attribute_0`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Attribute_0` (
  `id` bigint unsigned NOT NULL COMMENT 'HeroID',
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `formation` int NOT NULL DEFAULT '0' COMMENT '阵容类型',
  `order` int NOT NULL DEFAULT '0' COMMENT '阵容内部序号',
  `level` int NOT NULL DEFAULT '0' COMMENT '等级',
  `star` int DEFAULT '0' COMMENT '星级',
  `grade` int DEFAULT '0' COMMENT '品阶',
  `quality` int DEFAULT '0' COMMENT '品质',
  `rate` int DEFAULT '0' COMMENT '评级',
  `battlepoint` bigint DEFAULT '0' COMMENT '战斗力',
  `baseid` int DEFAULT '0' COMMENT '配置ID',
  `helper` int DEFAULT '0' COMMENT '支援位',
  `skills` varchar(4096) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT '' COMMENT '技能列表',
  `attribute` varchar(1024) DEFAULT '' COMMENT '属性列表',
  PRIMARY KEY (`id`,`formation`,`order`),
  KEY `idx_roleid` (`roleid`),
  KEY `idx_secondary` (`formation`,`order`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='战斗属性镜像';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Attribute_1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Attribute_1` (
  `id` bigint unsigned NOT NULL COMMENT 'HeroID',
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `formation` int NOT NULL DEFAULT '0' COMMENT '阵容类型',
  `order` int NOT NULL DEFAULT '0' COMMENT '阵容内部序号',
  `level` int NOT NULL DEFAULT '0' COMMENT '等级',
  `star` int DEFAULT '0' COMMENT '星级',
  `grade` int DEFAULT '0' COMMENT '品阶',
  `quality` int DEFAULT '0' COMMENT '品质',
  `rate` int DEFAULT '0' COMMENT '评级',
  `battlepoint` bigint DEFAULT '0' COMMENT '战斗力',
  `baseid` int DEFAULT '0' COMMENT '配置ID',
  `helper` int DEFAULT '0' COMMENT '支援位',
  `skills` varchar(4096) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT '' COMMENT '技能列表',
  `attribute` varchar(1024) DEFAULT '' COMMENT '属性列表',
  PRIMARY KEY (`id`,`formation`,`order`),
  KEY `idx_roleid` (`roleid`),
  KEY `idx_secondary` (`formation`,`order`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='战斗属性镜像';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Attribute_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Attribute_2` (
  `id` bigint unsigned NOT NULL COMMENT 'HeroID',
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `formation` int NOT NULL DEFAULT '0' COMMENT '阵容类型',
  `order` int NOT NULL DEFAULT '0' COMMENT '阵容内部序号',
  `level` int NOT NULL DEFAULT '0' COMMENT '等级',
  `star` int DEFAULT '0' COMMENT '星级',
  `grade` int DEFAULT '0' COMMENT '品阶',
  `quality` int DEFAULT '0' COMMENT '品质',
  `rate` int DEFAULT '0' COMMENT '评级',
  `battlepoint` bigint DEFAULT '0' COMMENT '战斗力',
  `baseid` int DEFAULT '0' COMMENT '配置ID',
  `helper` int DEFAULT '0' COMMENT '支援位',
  `skills` varchar(4096) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT '' COMMENT '技能列表',
  `attribute` varchar(1024) DEFAULT '' COMMENT '属性列表',
  PRIMARY KEY (`id`,`formation`,`order`),
  KEY `idx_roleid` (`roleid`),
  KEY `idx_secondary` (`formation`,`order`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='战斗属性镜像';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Attribute_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Attribute_3` (
  `id` bigint unsigned NOT NULL COMMENT 'HeroID',
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `formation` int NOT NULL DEFAULT '0' COMMENT '阵容类型',
  `order` int NOT NULL DEFAULT '0' COMMENT '阵容内部序号',
  `level` int NOT NULL DEFAULT '0' COMMENT '等级',
  `star` int DEFAULT '0' COMMENT '星级',
  `grade` int DEFAULT '0' COMMENT '品阶',
  `quality` int DEFAULT '0' COMMENT '品质',
  `rate` int DEFAULT '0' COMMENT '评级',
  `battlepoint` bigint DEFAULT '0' COMMENT '战斗力',
  `baseid` int DEFAULT '0' COMMENT '配置ID',
  `helper` int DEFAULT '0' COMMENT '支援位',
  `skills` varchar(4096) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT '' COMMENT '技能列表',
  `attribute` varchar(1024) DEFAULT '' COMMENT '属性列表',
  PRIMARY KEY (`id`,`formation`,`order`),
  KEY `idx_roleid` (`roleid`),
  KEY `idx_secondary` (`formation`,`order`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='战斗属性镜像';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Billing`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Billing` (
  `order` varchar(128) NOT NULL COMMENT '订单号',
  `roleid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '角色ID',
  `account` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT '' COMMENT '账号',
  `status` int unsigned NOT NULL DEFAULT '0' COMMENT '订单状态（1，初始；2，支付成功；3，完成订单）',
  `package` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT '' COMMENT '套餐号',
  `number` int unsigned DEFAULT '0' COMMENT '购买数量',
  `timestamp` bigint unsigned DEFAULT '0' COMMENT '时间戳',
  `detail` varchar(1024) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT '' COMMENT '扩展字段',
  PRIMARY KEY (`order`) USING BTREE,
  KEY `idx_order` (`order`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='支付订单';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Chapter`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Chapter` (
  `roleid` bigint unsigned NOT NULL COMMENT '角色id',
  `chapter` int unsigned NOT NULL DEFAULT '0' COMMENT '篇幅id',
  `star` int unsigned NOT NULL DEFAULT '0' COMMENT '完成星数',
  `stages` varchar(512) DEFAULT '' COMMENT '阶段信息列表',
  PRIMARY KEY (`roleid`,`chapter`),
  KEY `idx_roleid` (`roleid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='章节信息';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `CharBase`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `CharBase` (
  `roleid` bigint unsigned NOT NULL,
  `name` varchar(255) NOT NULL,
  `account` varchar(255) NOT NULL COMMENT '账号',
  `zone` smallint unsigned NOT NULL DEFAULT '0' COMMENT '区服ID',
  `createtime` bigint unsigned DEFAULT '0' COMMENT '创角时间',
  `lastlogintime` bigint DEFAULT '0' COMMENT '最近登录时间',
  `lastlogouttime` bigint DEFAULT '0' COMMENT '最近登出时间',
  `exp` bigint DEFAULT '0' COMMENT '角色经验',
  `money` bigint DEFAULT '0' COMMENT '元宝',
  `diamond` bigint DEFAULT '0' COMMENT '钻石(绑银)',
  `gamecoin` bigint DEFAULT '0' COMMENT '金币',
  `arenacoin` bigint DEFAULT '0' COMMENT '竞技场券',
  `strength` bigint DEFAULT '0' COMMENT '体力',
  `strengthtimestamp` bigint DEFAULT '0' COMMENT '体力恢复时间',
  `vipexp` bigint DEFAULT '0' COMMENT 'vip经验',
  `lastcity` int unsigned DEFAULT '0' COMMENT '最近所在城镇',
  `lastposition` varchar(50) DEFAULT '' COMMENT '所在位置',
  `totembagcapacity` int DEFAULT '0' COMMENT '图腾背包扩充的格子',
  `guildcoin` bigint DEFAULT '0' COMMENT '公会币',
  `opensystems` varchar(1024) DEFAULT '' COMMENT '开放的系统列表',
  PRIMARY KEY (`roleid`),
  KEY `idx_roleid` (`roleid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='角色信息';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `CharBrief`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `CharBrief` (
  `roleid` bigint unsigned NOT NULL COMMENT 'roleid',
  `account` varchar(128) NOT NULL COMMENT '账号',
  `storeaccount` varchar(128) NOT NULL DEFAULT '' COMMENT '商店账号',
  `status` tinyint(1) NOT NULL DEFAULT '1' COMMENT '账号状态,0-禁用1-启用',
  `name` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT '' COMMENT '角色名',
  `zone` smallint unsigned NOT NULL DEFAULT '0' COMMENT '区服ID',
  `createtime` bigint NOT NULL DEFAULT '0' COMMENT '创角时间',
  `level` int unsigned DEFAULT '1' COMMENT '角色等级',
  `viplevel` int unsigned DEFAULT '0' COMMENT 'VIP等级',
  `avatar` int unsigned DEFAULT '0' COMMENT '头像',
  `battlepoint` bigint unsigned DEFAULT '0' COMMENT '最强战斗力',
  `lastercity` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT '' COMMENT '最近所在城镇',
  `lastlogintime` bigint DEFAULT '0' COMMENT '最近登录时间',
  `lastlogouttime` bigint DEFAULT '0' COMMENT '最近登出时间',
  `bannedtime` bigint DEFAULT '0' COMMENT '封号过期时间',
  `bannedreason` varchar(512) DEFAULT '' COMMENT '封号原因',
  `shutuptime` bigint DEFAULT '0' COMMENT '禁言过期时间',
  `opensystems` varchar(1024) DEFAULT '' COMMENT '开放的系统列表',
  `mainprogress` varchar(256) DEFAULT '' COMMENT '主线进度',
  `friendscount` int unsigned DEFAULT '0' COMMENT '好友个数',
  `settings` bigint unsigned DEFAULT '0' COMMENT '相关功能设置',
  `arenastatus` tinyint(1) NOT NULL DEFAULT '0' COMMENT '竞技场状态,0-未参与1-已参与',
  PRIMARY KEY (`roleid`),
  KEY `idx_account` (`account`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='角色简要信息';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `CombatJournal`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `CombatJournal` (
  `id` bigint unsigned NOT NULL COMMENT '战斗ID',
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `module` int NOT NULL DEFAULT '0' COMMENT '系统ID',
  `result` int NOT NULL DEFAULT '0' COMMENT '结果',
  `opponent` bigint unsigned NOT NULL DEFAULT '0' COMMENT '对手ID',
  `robotid` int NOT NULL DEFAULT '0' COMMENT '机器人ID',
  `avatar` int unsigned NOT NULL DEFAULT '0' COMMENT '头像',
  `name` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT '' COMMENT '对手名称',
  `level` int NOT NULL DEFAULT '0' COMMENT '对手等级',
  `viplevel` int NOT NULL DEFAULT '0' COMMENT '对手VIP等级',
  `change` int NOT NULL DEFAULT '0' COMMENT '积分变化',
  `timestamp` bigint NOT NULL DEFAULT '0' COMMENT '发生的时间戳',
  `statistics` varchar(2048) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT '' COMMENT '数据统计',
  `extrainfo` varchar(1024) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT '' COMMENT '额外信息',
  PRIMARY KEY (`id`) USING BTREE,
  KEY `idx_roleid` (`roleid`) USING BTREE,
  KEY `idx_secondary` (`module`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='战斗日志';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `CombatVideo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `CombatVideo` (
  `id` bigint unsigned NOT NULL COMMENT '战斗ID',
  `hostid` int unsigned NOT NULL DEFAULT '0' COMMENT '服务器ID',
  `dungeon` int NOT NULL DEFAULT '0' COMMENT '关卡ID',
  `creator` bigint unsigned NOT NULL DEFAULT '0' COMMENT '创建者',
  `timestamp` bigint NOT NULL DEFAULT '0' COMMENT '时间戳',
  `version` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT '' COMMENT '版本号',
  `result` int NOT NULL DEFAULT '0' COMMENT '战斗结果',
  `attacker` varchar(2048) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT '' COMMENT '攻击方信息',
  `defender` varchar(2048) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT '' COMMENT '防御方信息',
  `statistics` varchar(2048) NOT NULL DEFAULT '' COMMENT '数据统计',
  `video` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '回放数据',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='战报记录';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Dungeon`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
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
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Equipment_0`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Equipment_0` (
  `id` bigint unsigned NOT NULL COMMENT '实体ID',
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `baseid` int DEFAULT '0' COMMENT '配置ID',
  `count` bigint DEFAULT '0' COMMENT '数量',
  `place` int DEFAULT '0' COMMENT '存储位置',
  `createtime` bigint DEFAULT '0' COMMENT '创建时间',
  `exp` bigint DEFAULT '0' COMMENT '经验',
  `camp` int DEFAULT '0' COMMENT '阵营标签',
  `heroid` bigint DEFAULT '0' COMMENT '所属英雄',
  PRIMARY KEY (`id`),
  KEY `idx_roleid` (`roleid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='装备表';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Equipment_1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Equipment_1` (
  `id` bigint unsigned NOT NULL COMMENT '实体ID',
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `baseid` int DEFAULT '0' COMMENT '配置ID',
  `count` bigint DEFAULT '0' COMMENT '数量',
  `place` int DEFAULT '0' COMMENT '存储位置',
  `createtime` bigint DEFAULT '0' COMMENT '创建时间',
  `exp` bigint DEFAULT '0' COMMENT '经验',
  `camp` int DEFAULT '0' COMMENT '阵营标签',
  `heroid` bigint DEFAULT '0' COMMENT '所属英雄',
  PRIMARY KEY (`id`),
  KEY `idx_roleid` (`roleid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='装备表';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Equipment_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Equipment_2` (
  `id` bigint unsigned NOT NULL COMMENT '实体ID',
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `baseid` int DEFAULT '0' COMMENT '配置ID',
  `count` bigint DEFAULT '0' COMMENT '数量',
  `place` int DEFAULT '0' COMMENT '存储位置',
  `createtime` bigint DEFAULT '0' COMMENT '创建时间',
  `exp` bigint DEFAULT '0' COMMENT '经验',
  `camp` int DEFAULT '0' COMMENT '阵营标签',
  `heroid` bigint DEFAULT '0' COMMENT '所属英雄',
  PRIMARY KEY (`id`),
  KEY `idx_roleid` (`roleid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='装备表';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Equipment_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Equipment_3` (
  `id` bigint unsigned NOT NULL COMMENT '实体ID',
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `baseid` int DEFAULT '0' COMMENT '配置ID',
  `count` bigint DEFAULT '0' COMMENT '数量',
  `place` int DEFAULT '0' COMMENT '存储位置',
  `createtime` bigint DEFAULT '0' COMMENT '创建时间',
  `exp` bigint DEFAULT '0' COMMENT '经验',
  `camp` int DEFAULT '0' COMMENT '阵营标签',
  `heroid` bigint DEFAULT '0' COMMENT '所属英雄',
  PRIMARY KEY (`id`),
  KEY `idx_roleid` (`roleid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='装备表';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Formation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Formation` (
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `type` int NOT NULL COMMENT '阵容类型',
  `order` int NOT NULL COMMENT '内部顺序',
  `name` varchar(50) DEFAULT '' COMMENT '自定义名称',
  `formation` varchar(256) DEFAULT '' COMMENT '阵容具体信息',
  `helpsite` varchar(50) DEFAULT '' COMMENT '支援位配置',
  `ultimate` varchar(50) DEFAULT '' COMMENT '奥义技能',
  `comboflow` varchar(50) DEFAULT '' COMMENT '连招配置',
  PRIMARY KEY (`roleid`,`type`,`order`),
  KEY `idx_roleid` (`roleid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='阵容数据';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `GMMail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `GMMail` (
  `mailid` int unsigned NOT NULL COMMENT '邮件ID',
  `starttime` bigint DEFAULT '0' COMMENT '邮件开始时间',
  `expiretime` bigint DEFAULT '0' COMMENT '邮件失效时间',
  `condition` varchar(512) DEFAULT '' COMMENT '条件',
  `tag` int unsigned DEFAULT '0' COMMENT '标签',
  `keepdays` int unsigned DEFAULT '0' COMMENT '邮件保留天数',
  `sendername` varchar(255) DEFAULT '' COMMENT '发送者名字',
  `title` varchar(255) DEFAULT '' COMMENT '标题',
  `content` varchar(1024) DEFAULT '' COMMENT '邮件内容',
  `paramlist` varchar(1024) DEFAULT '' COMMENT '参数列表',
  `attachment` varchar(10240) DEFAULT '' COMMENT '附件',
  PRIMARY KEY (`mailid`),
  KEY `idx_mailid` (`mailid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='GM邮件';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `HeroBase`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `HeroBase` (
  `id` bigint unsigned NOT NULL,
  `roleid` bigint unsigned NOT NULL COMMENT '所属角色',
  `baseid` int NOT NULL COMMENT '基础ID',
  `expiretime` bigint DEFAULT '0' COMMENT '试用伙伴的过期时间',
  `exp` bigint DEFAULT '0' COMMENT '伙伴经验',
  `level` int DEFAULT '0' COMMENT '伙伴等级',
  `star` int DEFAULT '0' COMMENT '伙伴星级',
  `grade` int DEFAULT '0' COMMENT '伙伴进阶等级',
  `quality` int DEFAULT '0' COMMENT '伙伴突破等级',
  `rate` int DEFAULT '0' COMMENT '伙伴评级',
  `locking` tinyint(1) DEFAULT '0' COMMENT '锁定状态',
  `skill` varchar(1024) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT '' COMMENT '伙伴技能列表',
  PRIMARY KEY (`id`),
  KEY `idx_roleid` (`roleid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='伙伴表';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `ImportZone`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `ImportZone` (
  `account` varchar(128) NOT NULL COMMENT '账号',
  `viplv` int unsigned DEFAULT '0' COMMENT 'VIP等级',
  `totalpay` int unsigned DEFAULT '0' COMMENT '充值总额',
  `itemlist` varchar(1024) DEFAULT '' COMMENT '初始化的道具列表',
  PRIMARY KEY (`account`),
  KEY `idx_account` (`account`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='区服导入信息';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Item_0`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Item_0` (
  `id` bigint unsigned NOT NULL,
  `roleid` bigint unsigned NOT NULL,
  `baseid` int unsigned NOT NULL DEFAULT '0',
  `count` bigint NOT NULL DEFAULT '0',
  `place` tinyint NOT NULL DEFAULT '0',
  `create_time` bigint NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`) USING BTREE,
  KEY `idx_roleid` (`roleid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='物品表';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Item_1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Item_1` (
  `id` bigint unsigned NOT NULL,
  `roleid` bigint unsigned NOT NULL,
  `baseid` int unsigned NOT NULL DEFAULT '0',
  `count` bigint NOT NULL DEFAULT '0',
  `place` tinyint NOT NULL DEFAULT '0',
  `create_time` bigint NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`) USING BTREE,
  KEY `idx_roleid` (`roleid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='物品表';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Item_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Item_2` (
  `id` bigint unsigned NOT NULL,
  `roleid` bigint unsigned NOT NULL,
  `baseid` int unsigned NOT NULL DEFAULT '0',
  `count` bigint NOT NULL DEFAULT '0',
  `place` tinyint NOT NULL DEFAULT '0',
  `create_time` bigint NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`) USING BTREE,
  KEY `idx_roleid` (`roleid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='物品表';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Item_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Item_3` (
  `id` bigint unsigned NOT NULL,
  `roleid` bigint unsigned NOT NULL,
  `baseid` int unsigned NOT NULL DEFAULT '0',
  `count` bigint NOT NULL DEFAULT '0',
  `place` tinyint NOT NULL DEFAULT '0',
  `create_time` bigint NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`) USING BTREE,
  KEY `idx_roleid` (`roleid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='物品表';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Mail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Mail` (
  `mailid` bigint unsigned NOT NULL COMMENT '邮件id',
  `type` tinyint unsigned DEFAULT '0' COMMENT '邮件类型',
  `sendtime` bigint DEFAULT '0' COMMENT '发件时间',
  `templateid` int unsigned DEFAULT '0' COMMENT '模板ID',
  `status` tinyint unsigned DEFAULT '0' COMMENT '邮件状态',
  `sender` bigint unsigned DEFAULT '0' COMMENT '发件人id',
  `sendername` varchar(255) DEFAULT '' COMMENT '发件人角色名',
  `roleid` bigint unsigned DEFAULT NULL COMMENT '收件人id',
  `tag` int unsigned DEFAULT '0' COMMENT '标签',
  `keepdays` int DEFAULT '0' COMMENT '邮件保留天数',
  `title` varchar(255) DEFAULT '' COMMENT '标题',
  `content` varchar(1024) DEFAULT '' COMMENT '邮件内容',
  `paramlist` varchar(1024) DEFAULT '' COMMENT '参数列表',
  `attachment` varchar(10240) DEFAULT '' COMMENT '附件',
  PRIMARY KEY (`mailid`),
  KEY `idx_roleid` (`roleid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='邮件';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `OfflineMessage`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `OfflineMessage` (
  `id` bigint unsigned NOT NULL COMMENT '消息ID',
  `roleid` bigint unsigned NOT NULL COMMENT '接收者ID',
  `sender` bigint unsigned NOT NULL COMMENT '发送者ID',
  `sendtime` bigint NOT NULL DEFAULT '0' COMMENT '发送时间',
  `style` int unsigned DEFAULT '0' COMMENT '聊天样式',
  `message` varchar(1024) NOT NULL DEFAULT '' COMMENT '消息内容',
  PRIMARY KEY (`id`,`roleid`),
  KEY `idx_roleid` (`roleid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='离线消息库';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Payment`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Payment` (
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `totalpay` int DEFAULT '0' COMMENT '累积支付',
  `records` varchar(1024) DEFAULT '' COMMENT '支付记录',
  `vipstate` int DEFAULT '0' COMMENT 'vip奖励领取记录',
  `vipdiamondstate` int DEFAULT '0' COMMENT '钻石礼包购买记录',
  `viprmbstate` int DEFAULT '0' COMMENT '直购礼包购买记录',
  `pay1state` int DEFAULT '0' COMMENT '首充状态',
  PRIMARY KEY (`roleid`),
  KEY `idx_roleid` (`roleid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='支付相关';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `QuestDetail_0`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `QuestDetail_0` (
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `questid` int unsigned NOT NULL COMMENT '任务ID',
  `type` int NOT NULL DEFAULT '0' COMMENT '任务类型',
  `count` bigint DEFAULT '0',
  `accept_time` bigint DEFAULT '0' COMMENT '接领时间',
  `state` tinyint unsigned DEFAULT '0' COMMENT '任务完成状态',
  PRIMARY KEY (`roleid`,`questid`),
  KEY `idx_roleid` (`roleid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='任务详情';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `QuestDetail_1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `QuestDetail_1` (
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `questid` int unsigned NOT NULL COMMENT '任务ID',
  `type` int NOT NULL DEFAULT '0' COMMENT '任务类型',
  `count` bigint DEFAULT '0',
  `accept_time` bigint DEFAULT '0' COMMENT '接领时间',
  `state` tinyint unsigned DEFAULT '0' COMMENT '任务完成状态',
  PRIMARY KEY (`roleid`,`questid`) USING BTREE,
  KEY `idx_roleid` (`roleid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='任务详情';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `QuestDetail_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `QuestDetail_2` (
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `questid` int unsigned NOT NULL COMMENT '任务ID',
  `type` int NOT NULL DEFAULT '0' COMMENT '任务类型',
  `count` bigint DEFAULT '0',
  `accept_time` bigint DEFAULT '0' COMMENT '接领时间',
  `state` tinyint unsigned DEFAULT '0' COMMENT '任务完成状态',
  PRIMARY KEY (`roleid`,`questid`) USING BTREE,
  KEY `idx_roleid` (`roleid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='任务详情';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `QuestDetail_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `QuestDetail_3` (
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `questid` int unsigned NOT NULL COMMENT '任务ID',
  `type` int NOT NULL DEFAULT '0' COMMENT '任务类型',
  `count` bigint DEFAULT '0',
  `accept_time` bigint DEFAULT '0' COMMENT '接领时间',
  `state` tinyint unsigned DEFAULT '0' COMMENT '任务完成状态',
  PRIMARY KEY (`roleid`,`questid`) USING BTREE,
  KEY `idx_roleid` (`roleid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='任务详情';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `RankList`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `RankList` (
  `roleid` bigint unsigned NOT NULL DEFAULT '0' COMMENT '角色id',
  `arena` varchar(128) DEFAULT '' COMMENT '竞技场',
  PRIMARY KEY (`roleid`),
  KEY `idx_roleid` (`roleid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='排行榜';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Relation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Relation` (
  `roleid` bigint unsigned NOT NULL,
  `friendid` bigint unsigned NOT NULL,
  `type` tinyint unsigned DEFAULT '0' COMMENT '关系: 1-黑名单,2-好友,4-收到的邀请',
  `intimacy` int unsigned DEFAULT '0' COMMENT '亲密度',
  `timestamp` bigint unsigned DEFAULT '0' COMMENT '时间戳',
  PRIMARY KEY (`roleid`,`friendid`),
  KEY `idx_roleid` (`roleid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='关系表';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `RoleDaily`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `RoleDaily` (
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `module` int NOT NULL COMMENT '模块ID',
  `timestamp` bigint NOT NULL DEFAULT '0' COMMENT '时间戳',
  `content` text COMMENT '隔天的数据',
  `extracontent` text COMMENT '不隔天的数据',
  PRIMARY KEY (`roleid`,`module`),
  KEY `idx_roleid` (`roleid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='玩家日常信息表';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Settings`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Settings` (
  `roleid` bigint unsigned NOT NULL,
  `importaccount` varchar(128) DEFAULT '' COMMENT '导入账号',
  `onlinedays` int NOT NULL DEFAULT '0' COMMENT '累计登陆天数',
  `gamecoin` bigint NOT NULL DEFAULT '0' COMMENT '累计获得金币',
  `strength` bigint NOT NULL DEFAULT '0' COMMENT '累计使用体力',
  `towerfloor` int NOT NULL DEFAULT '0' COMMENT '累计爬塔层数',
  `pay` bigint NOT NULL DEFAULT '0' COMMENT '累积支付',
  `paytimes` bigint NOT NULL DEFAULT '0' COMMENT '累积支付次数',
  `gainmoney` bigint NOT NULL DEFAULT '0' COMMENT '累积获得钻石',
  `consumemoney` bigint NOT NULL DEFAULT '0' COMMENT '累积消耗钻石',
  `setting` varchar(1024) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT '' COMMENT '设置内容',
  `gsettings` bigint DEFAULT '0' COMMENT '服务器相关设置',
  `guide` varchar(1024) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT '' COMMENT '新手引导信息',
  `rnregister` int DEFAULT '0' COMMENT '实名认证的奖励',
  `refunds` int DEFAULT '0' COMMENT '累积退款金额',
  PRIMARY KEY (`roleid`),
  KEY `idx_role` (`roleid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='玩家设置信息';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `ShopGoods`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `ShopGoods` (
  `id` int NOT NULL COMMENT '商品ID',
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `shop` int NOT NULL DEFAULT '0' COMMENT '所属商店',
  `times` int NOT NULL DEFAULT '0' COMMENT '购买次数',
  `limittimes` int NOT NULL DEFAULT '0' COMMENT '限购次数',
  `timestamp` bigint NOT NULL DEFAULT '0' COMMENT '限购时间',
  PRIMARY KEY (`id`,`roleid`),
  KEY `idx_roleid` (`roleid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='商品表';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `ShopMall`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `ShopMall` (
  `id` int NOT NULL COMMENT '商店id',
  `roleid` bigint unsigned NOT NULL COMMENT '角色id',
  `createtime` bigint NOT NULL DEFAULT '0' COMMENT '商店的创建时间',
  `refreshtime` bigint NOT NULL DEFAULT '0' COMMENT '自动刷新时间',
  `manualtime` bigint NOT NULL DEFAULT '0' COMMENT '手动刷新时间',
  `manualtimes` int unsigned NOT NULL DEFAULT '0' COMMENT '手动刷新次数',
  PRIMARY KEY (`id`,`roleid`) USING BTREE,
  KEY `idx_roleid` (`roleid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='商城表';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `SystemState`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `SystemState` (
  `id` int unsigned NOT NULL DEFAULT '0' COMMENT '系统id',
  `season` int unsigned NOT NULL DEFAULT '0' COMMENT '赛季',
  `timestamp` bigint NOT NULL DEFAULT '0' COMMENT '开始时间',
  `state` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '系统状态',
  `notes` text NOT NULL COMMENT '备注信息',
  PRIMARY KEY (`id`),
  KEY `idx_id` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='系统状态';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Variable`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Variable` (
  `id` smallint NOT NULL COMMENT '服务器索引',
  `roleid` bigint DEFAULT '0' COMMENT '角色自增ID',
  `mailid` bigint unsigned DEFAULT '0' COMMENT '邮件自增ID',
  `teamid` bigint unsigned DEFAULT '0' COMMENT '组队自增ID',
  `entityid` bigint unsigned DEFAULT '0' COMMENT '道具自增ID',
  `imageid` bigint unsigned DEFAULT '0' COMMENT '镜像自增ID',
  `guildid` int unsigned DEFAULT '0' COMMENT '工会自增ID',
  `heroid` bigint unsigned DEFAULT '0' COMMENT '伙伴id',
  `logid` bigint unsigned DEFAULT '0' COMMENT '伙伴评价自增ID',
  `messageid` bigint unsigned DEFAULT '0' COMMENT '留言ID',
  PRIMARY KEY (`id`),
  KEY `idx_id` (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='服务器参数';
/*!40101 SET character_set_client = @saved_cs_client */;
DROP TABLE IF EXISTS `Zone`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Zone` (
  `roleid` bigint unsigned NOT NULL COMMENT '角色ID',
  `heroid` int unsigned DEFAULT '0' COMMENT '伙伴形象',
  `likes` int unsigned DEFAULT '0' COMMENT '人气值',
  `gifts` int unsigned DEFAULT '0' COMMENT '礼物计数',
  `visits` int unsigned DEFAULT '0' COMMENT '访问计数',
  `bookbio` varchar(512) DEFAULT '' COMMENT '留言板心情',
  `signature` varchar(512) DEFAULT '' COMMENT '签名',
  PRIMARY KEY (`roleid`),
  KEY `idx_roleid` (`roleid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='个人空间';
/*!40101 SET character_set_client = @saved_cs_client */;
