#pragma once

#include "common.h"
#include "app_type.h"
#include "component.h"
#include "system.h"

#include <yaml-cpp/yaml.h>

struct YamlConfig
{
    virtual ~YamlConfig() {}
};

struct AppConfig :public YamlConfig
{
    int LogicThreadNum{ 0 };
    int MysqlThreadNum{ 0 };
    int ListenThreadNum{ 1 };
    int ConnectThreadNum{ 1 };
};;

struct CommonConfig : public AppConfig
{
    std::string Ip{ "127.0.0.1" };
    int Port{ 6661 };
    int HttpPort{ 5051 };
};

struct AppListForOneConfig : public CommonConfig
{
    int Id{ 0 };
};

struct AppListConfig : public AppConfig
{
    std::vector<AppListForOneConfig> Apps;

    AppListForOneConfig* GetOne(int id)
    {
        for (decltype(Apps.size()) index = 0; index < Apps.size(); index++)
        {
            if (Apps[index].Id == id)
                return &Apps[index];
        }

        return nullptr;
    }
};

struct LoginConfig : public AppListConfig
{
    std::string UrlLogin;
    std::string UrlMethod;
};

struct GameConfig : public AppListConfig
{

};

struct SpaceConfig :public AppListConfig
{

};

struct DBConfig
{
    std::string DBType;
    std::string Ip{ "127.0.0.1" };
    int Port{ 3306 };
    std::string User{ "" };
    std::string Password{ "" };
    std::string DatabaseName{ "" };
    std::string CharacterSet{ "" };
    std::string Collation{ "" };
};

struct DBMgrConfig : public CommonConfig
{
    static std::string DBTypeMysql;
    static std::string DBTypeRedis;

    std::vector<DBConfig> DBs;

    DBConfig* GetDBConfig(const std::string dbType)
    {
        for (decltype(DBs.size()) index = 0; index < DBs.size(); index++)
        {
            if (DBs[index].DBType != dbType)
                continue;

            return &DBs[index];
        }

        return nullptr;
    }
};

struct RobotConfig : public AppConfig
{

};

class Yaml : public Component<Yaml>, public IAwakeSystem<>
{
public:
    void Awake() override;
    void BackToPool() override;

    YamlConfig* GetConfig(APP_TYPE appType);
    CommonConfig* GetIPEndPoint(APP_TYPE appType, int appId = 0);

private:
    void LoadConfig(APP_TYPE appType, YAML::Node& config);
    DBConfig LoadDbConfig(YAML::Node node) const;
    void LoadAppList(AppListConfig* pConfig, YAML::Node node) const;

private:
    std::map<APP_TYPE, YamlConfig*> _configs;
};
