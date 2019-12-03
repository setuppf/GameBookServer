/*
Navicat MySQL Data Transfer

Source Server         : 192.168.0.120
Source Server Version : 50560
Source Host           : 192.168.0.120:3306
Source Database       : account

Target Server Type    : MYSQL
Target Server Version : 50560
File Encoding         : 65001

*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for account
-- ----------------------------
DROP TABLE IF EXISTS `account`;
CREATE TABLE `account` (
  `id` bigint(64) NOT NULL AUTO_INCREMENT,
  `account` varchar(20) NOT NULL,
  `password` varchar(64) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `account_index` (`account`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=1000 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of account
-- ----------------------------
INSERT INTO `account` VALUES (1000, 'test', 'e10adc3949ba59abbe56e057f20f883e');

-- ----------------------------
-- Procedure structure for gen_account
-- ----------------------------
DROP PROCEDURE IF EXISTS `gen_account`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `gen_account`()
BEGIN

DECLARE i INT DEFAULT 1;
while i <= 1000 do
insert into account (account, password) values (CONCAT('test',i), 'e10adc3949ba59abbe56e057f20f883e'); 
set i = i +1;
end while;

END
;;
DELIMITER ;
