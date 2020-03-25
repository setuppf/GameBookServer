#include "mysql_table_update.h"

#include "libserver/log4_help.h"
#include "libserver/util_string.h"

#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#include <thread>

MysqlTableUpdate::MysqlTableUpdate()
{
    // 注册更新函数，按下标执行，注意顺序
    _update_func.push_back(BindFunP0(this, &MysqlTableUpdate::Update00)); 
}

MysqlTableUpdate::~MysqlTableUpdate()
{
    Disconnect();
}

void MysqlTableUpdate::Check()
{
    if (!ConnectInit())
        return;

    auto pYaml = Yaml::GetInstance();
    auto pDbMgrCfig = dynamic_cast<DBMgrConfig*>(pYaml->GetConfig(APP_DB_MGR));
    _pDbCfg = pDbMgrCfig->GetDBConfig(DBMgrConfig::DBTypeMysql);
    if (_pDbCfg == nullptr)
    {
        LOG_ERROR("Init failed. get mysql config is failed.");
        return;
    }

    LOG_DEBUG("Mysql update connect. " << _pDbCfg->Ip.c_str() << ":" << _pDbCfg->Port << " starting... id:" << std::this_thread::get_id());

    if (mysql_real_connect(_pMysql, _pDbCfg->Ip.c_str(), _pDbCfg->User.c_str(), _pDbCfg->Password.c_str(), nullptr, _pDbCfg->Port, nullptr, CLIENT_FOUND_ROWS))
    {
        mysql_select_db(_pMysql, _pDbCfg->DatabaseName.c_str());
    }

    int mysqlerrno = CheckMysqlError();
    if (mysqlerrno == ER_BAD_DB_ERROR)
    {
        LOG_DEBUG("Mysql. try create database:" << _pDbCfg->DatabaseName.c_str());

        // 1049: Unknown database。 没有找到数据库，就新建一个
        if (!CreateDatabaseIfNotExist())
        {
            Disconnect();
            return;
        }

        mysql_select_db(_pMysql, _pDbCfg->DatabaseName.c_str());
        mysqlerrno = CheckMysqlError();
    }

    if (mysqlerrno > 0)
    {
        Disconnect();
        return;
    }

    // 检查版本，自动更新
    if (!UpdateToVersion())
    {
        LOG_ERROR("!!!Failed. Mysql update. UpdateToVersion");
        return;
    }

    mysql_ping(_pMysql);
    mysqlerrno = CheckMysqlError();
    if (mysqlerrno > 0)
    {
        Disconnect();
        return;
    }

    LOG_DEBUG("Mysql Update successfully! addr:" << _pDbCfg->Ip.c_str() << ":" << _pDbCfg->Port);
}

bool MysqlTableUpdate::CreateDatabaseIfNotExist()
{
    // 是否存在数据库，如果不存在，则创建
    std::string querycmd = strutil::format("CREATE DATABASE IF NOT EXISTS %s;", _pDbCfg->DatabaseName.c_str());

    my_ulonglong affected_rows;
    if (!Query(querycmd.c_str(), affected_rows)) {
        LOG_ERROR("!!! Failed. MysqlConnector::CreateDatabaseIfNotExist. cmd:" << querycmd.c_str());
        return false;
    }

    // 链接上的DB之后，选择指定的数据库
    if (mysql_select_db(_pMysql, _pDbCfg->DatabaseName.c_str()) != 0) {
        LOG_ERROR("!!! Failed. MysqlConnector::CreateDatabaseIfNotExist: mysql_select_db:" << LOG4CPLUS_STRING_TO_TSTRING(_pDbCfg->DatabaseName));
        return false;
    }

    // 设置数据为的字符集，从yaml中配置中读取我们需要的字符集
    if (mysql_set_character_set(_pMysql, _pDbCfg->CharacterSet.c_str()) != 0) {
        LOG_ERROR("!!! Failed. MysqlConnector::CreateDatabaseIfNotExist: Could not set client connection character set to " << LOG4CPLUS_STRING_TO_TSTRING(_pDbCfg->CharacterSet));
        return false;
    }

    // 设置了数据库大小写敏感，配置文件中为 utf8_general_ci
    querycmd = strutil::format("ALTER DATABASE CHARACTER SET %s COLLATE %s", _pDbCfg->CharacterSet.c_str(), _pDbCfg->Collation.c_str());
    if (!Query(querycmd.c_str(), affected_rows)) {
        LOG_ERROR("!!! Failed. MysqlConnector::CreateDatabaseIfNotExist. cmd:" << LOG4CPLUS_STRING_TO_TSTRING(querycmd.c_str()));
        return false;
    }

    // 创建一个version 表，使用了 InnoDB 方式
    std::string create_version =
        "CREATE TABLE IF NOT EXISTS `version` (" \
        "`version` int(11) NOT NULL," \
        "PRIMARY KEY (`version`)" \
        ") ENGINE=%s DEFAULT CHARSET=%s;";

    std::string cmd = strutil::format(create_version.c_str(), "InnoDB", _pDbCfg->CharacterSet.c_str());
    if (!Query(cmd.c_str(), affected_rows)) {
        LOG_ERROR("!!! Failed. MysqlConnector::CreateTable. " << LOG4CPLUS_STRING_TO_TSTRING(cmd));
        return false;
    }

    // 创建一个 player 表
    std::string create_player =
        "CREATE TABLE IF NOT EXISTS `player` (" \
        "`sn` bigint(20) NOT NULL," \
        "`name` char(32) NOT NULL," \
        "`account` char(64) NOT NULL," \
        "`base` blob," \
        "`item` blob,"  \
        "`misc` blob,"  \
        "`savetime` datetime default NULL," \
        "`createtime` datetime default NULL," \
        "PRIMARY KEY  (`sn`),"  \
        "UNIQUE KEY `NAME` (`name`),"  \
        "KEY `ACCOUNT` (`account`)"  \
        ") ENGINE=%s DEFAULT CHARSET=%s;";

    cmd = strutil::format(create_player.c_str(), "InnoDB", _pDbCfg->CharacterSet.c_str());
    if (!Query(cmd.c_str(), affected_rows)) {
        LOG_ERROR("!!! Failed. MysqlConnector::CreateTable" << LOG4CPLUS_STRING_TO_TSTRING(cmd));
        return false;
    }

    // 最后创建完了，修改 version表的中version 字段，设为初始的0号版本
    cmd = "insert into `version` VALUES ('0')";
    if (!Query(cmd.c_str(), affected_rows)) {
        LOG_ERROR("!!! Failed. MysqlConnector::CreateTable." << LOG4CPLUS_STRING_TO_TSTRING(cmd));
        return false;
    }

    return true;
}

bool MysqlTableUpdate::UpdateToVersion()
{
    my_ulonglong affected_rows;
    std::string sql = "select version from `version`";
    if (!Query(sql.c_str(), affected_rows))
        return false;

    MYSQL_ROW row = Fetch();
    if (row == nullptr)
        return false;

    int version = GetInt(row, 0);
    if (version == _version)
        return true;

    // 如果DB版本不匹配，升级DB
    for (int i = version + 1; i <= _version; i++) {
        if (_update_func[i] == nullptr)
            continue;

        if (!_update_func[i]()) {
            LOG_ERROR("UpdateToVersion failed!!!!!, version=" << i);
            return false;
        }

        LOG_INFO("update db to version:" << i);

        // 成功之后，更改DB的version
        std::string cmd = strutil::format("update `version` set version = %d", i);
        if (!Query(cmd.c_str(), affected_rows)) {
            LOG_ERROR("UpdateToVersion failed!!!!!, change version failed. version=" << i);
            return false;
        }
    }

    return true;
}

bool MysqlTableUpdate::Update00()
{
    return true;
}

//bool MysqlTableUpdate::Update01()
//{
//    std::string sql = "ALTER TABLE `player` ADD COLUMN `testa`  blob NULL AFTER `misc`;";
//    my_ulonglong affected_rows;
//    if (!Query(sql.c_str(), affected_rows)) {
//        LOG_ERROR("!!! Failed. MysqlTableUpdate::Update01. " << sql.c_str());
//        return false;
//    }
//
//    return true;
//}
