#include "yaml.h"
#include "res_path.h"
#include "app_type_mgr.h"
#include "util_string.h"
#include "entity.h"
#include "entity_system.h"
#include "log4_help.h"

#include <tuple>

std::string DBMgrConfig::DBTypeMysql{ "mysql" };
std::string DBMgrConfig::DBTypeRedis{ "redis" };

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

    LoadConfig(APP_TYPE::APP_ALLINONE, config);
    LoadConfig(APP_TYPE::APP_LOGIN, config);
    LoadConfig(APP_TYPE::APP_ROBOT, config);
    LoadConfig(APP_TYPE::APP_DB_MGR, config);

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

    YamlConfig* pYamlConfig;

    switch (appType)
    {
    case APP_LOGIN:
    {
        auto pConfig = new LoginConfig();
        pConfig->UrlLogin = node["url_login"].as<std::string>();
        pYamlConfig = pConfig;
        break;
    }
	case APP_DB_MGR:
	{
        auto pConfig = new DBMgrConfig();
		YAML::Node node_dbs = node["dbs"];
		const size_t size = node_dbs.size();
		for (size_t i = 0; i < size; i++)
		{
			DBConfig one = LoadDbConfig(node_dbs[i]);
			pConfig->DBs.push_back(one);
		}
        pYamlConfig = pConfig;
		break;
	}
    case APP_ROBOT:
    {
        auto pConfig = new RobotConfig();
        pYamlConfig = pConfig;
        break;
    }
    default:
    {
        pYamlConfig = new CommonConfig();
        break;
    }
    }

    auto pCommon = dynamic_cast<CommonConfig*>(pYamlConfig);
    if (pCommon != nullptr)
    {
        pCommon->Ip = node["ip"].as<std::string>();
        pCommon->Port = node["port"].as<int>();
    }

    const auto pAppConfig = dynamic_cast<AppConfig*>(pYamlConfig);
    if (pAppConfig != nullptr)
    {
        if (node["thread_logic"])
            pAppConfig->LogicThreadNum = node["thread_logic"].as<int>();
        else
            pAppConfig->LogicThreadNum = 0;

        if (node["thread_mysql"])
            pAppConfig->MysqlThreadNum = node["thread_mysql"].as<int>();
        else
            pAppConfig->MysqlThreadNum = 0;
    }

    _configs.insert(std::make_pair(appType, pYamlConfig));
}

DBConfig Yaml::LoadDbConfig(YAML::Node node) const
{
	DBConfig one;
	YAML::Node::iterator iter = node.begin();
	while (iter != node.end())
	{
		const std::string key = iter->first.as<std::string>();
		if (key == "type")
		{
			one.DBType = iter->second.as<std::string>();
			std::transform(one.DBType.begin(), one.DBType.end(), one.DBType.begin(), ::tolower);
		}

		else if (key == "ip")
			one.Ip = iter->second.as<std::string>();
		else if (key == "port")
			one.Port = iter->second.as<int>();
		else if (key == "user")
			one.User = iter->second.as<std::string>();
		else if (key == "password")
			one.Password = iter->second.as<std::string>();
		else if (key == "character_set")
			one.CharacterSet = iter->second.as<std::string>();
		else if (key == "collation")
			one.Collation = iter->second.as<std::string>();
		else if (key == "database_name")
			one.DatabaseName = iter->second.as<std::string>();

		++iter;
	}

	return one;
}

CommonConfig* Yaml::GetIPEndPoint(APP_TYPE appType, int appId)
{
    const auto pConfig = GetConfig(appType);
    const auto pListCfg = dynamic_cast<AppListConfig*>(pConfig);
    if (pListCfg != nullptr)
    {
        auto pOneCfg = pListCfg->GetOne(appId);
        if (pOneCfg == nullptr)
        {
            LOG_ERROR("can't find appType yaml's config. appType:" << appType << " id:" << appId);
            return nullptr;
        }

        return pOneCfg;
    }
    auto pAppConfig = dynamic_cast<CommonConfig*>(pConfig);
    if (pAppConfig == nullptr)
    {
        LOG_ERROR("can't find appType yaml's config. appType:" << appId);
        return nullptr;
    }

    return pAppConfig;
}
