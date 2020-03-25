#pragma once

#include "common.h"
#include "singleton.h"
#include "app_type_mgr.h"
#include <yaml-cpp/yaml.h>

struct YamlConfig
{
    virtual ~YamlConfig() {}
};

struct AppConfig :public YamlConfig
{
    int ThreadNum{ 1 };
};;

struct CommonConfig : public AppConfig
{
    std::string Ip{ "127.0.0.1" };
    int Port{ 6661 };
};

struct LoginConfig : public CommonConfig
{
    std::string UrlLogin;
};

struct RobotConfig : public AppConfig
{

};

class Yaml : public Singleton<Yaml>
{
public:
    Yaml();
    YamlConfig* GetConfig(APP_TYPE appType);

private:
    void LoadConfig(APP_TYPE appType, YAML::Node& config);

private:
    std::map<APP_TYPE, YamlConfig*> _configs;
};
