#include "yaml.h"
#include "res_path.h"
#include "app_type_mgr.h"
#include "util_string.h"
#include "entity.h"
#include "entity_system.h"
#include "log4_help.h"

#include <tuple>

Yaml::Yaml()
{
    auto pResPath = ResPath::GetInstance();
    if (pResPath == nullptr)
    {
        std::cout << "yaml awake failed. can't get ResPath." << std::endl;
        return;
    }

    const std::string path = pResPath->FindResPath("/engine.yaml");
    YAML::Node config = YAML::LoadFile(path);

    LoadConfig(APP_TYPE::APP_LOGIN, config);
    LoadConfig(APP_TYPE::APP_ROBOT, config);

    LOG_DEBUG("Yaml awake is Ok.");
}

YamlConfig* Yaml::GetConfig(const APP_TYPE appType)
{
    if (_configs.find(appType) != _configs.end())
    {
        return _configs[appType];
    }

    std::string appTypeName = AppTypeMgr::GetInstance()->GetAppName(appType);
    std::cout << "load config failed. appType:" << appTypeName.c_str() << std::endl;
    return nullptr;
}

void Yaml::LoadConfig(const APP_TYPE appType, YAML::Node& config)
{
    std::string appTypeName = AppTypeMgr::GetInstance()->GetAppName(appType);
    YAML::Node node = config[appTypeName];
    if (node == nullptr)
    {
        std::cout << "load config failed." << appTypeName.c_str() << std::endl;
        return;
    }

    AppConfig* pAppConfig;

    switch (appType)
    {
    case APP_LOGIN:
    {
        auto pConfig = new LoginConfig();
        pConfig->UrlLogin = node["url_login"].as<std::string>();
        pAppConfig = pConfig;
        break;
    }
    case APP_ROBOT:
    {
        auto pConfig = new RobotConfig();
        pAppConfig = pConfig;
        break;
    }
    default:
    {
        pAppConfig = new CommonConfig();
        break;
    }
    }

    auto pCommon = dynamic_cast<CommonConfig*>(pAppConfig);
    if (pCommon != nullptr)
    {
        pCommon->Ip = node["ip"].as<std::string>();
        pCommon->Port = node["port"].as<int>();
    }

    pAppConfig->ThreadNum = node["thread_num"].as<int>();
    _configs.insert(std::make_pair(appType, pAppConfig));
}
