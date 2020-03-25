#include "common.h"
#include "log4.h"

#include <iostream>
#include <log4cplus/configurator.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/logger.h>

#include "util_string.h"
#include "thread_mgr.h"
#include "res_path.h"
#include "app_type.h"
#include "log4_help.h"
#include "component_help.h"

void Log4::Awake(APP_TYPE appType)
{
    _appType = appType;
    auto pResPath = ComponentHelp::GetResPath();

    const std::string filename = strutil::format("/log4/log4_%s.properties", GetAppName(_appType));
    std::string filePath = pResPath->FindResPath(filename);
    if (filePath.empty())
    {
        std::cout << " !!!!! log4 properties not found! filename:" << filename.c_str() << std::endl;
        return;
    }

    const log4cplus::tstring configFile = LOG4CPLUS_STRING_TO_TSTRING(filePath);

    log4cplus::PropertyConfigurator config(configFile);
    config.configure();

    DebugInfo(log4cplus::Logger::getRoot());
    DebugInfo(log4cplus::Logger::getInstance(LOG4CPLUS_C_STR_TO_TSTRING("GM")));
    LOG_DEBUG("Log4::Initialize is Ok.");
}

void Log4::DebugInfo(log4cplus::Logger logger) const
{
    log4cplus::SharedAppenderPtrList appenderList = logger.getAllAppenders();
    auto iter = appenderList.begin();
    while (iter != appenderList.end())
    {
        log4cplus::Appender* pAppender = iter->get();
        //log4cplus::RollingFileAppender* pFileAppender = static_cast<log4cplus::RollingFileAppender*>(pAppender);
        //if (pFileAppender !=nullptr)
        //	pFileAppender->imbue(std::locale("zh_CN"));

        //log4cplus::ConsoleAppender* pConsoleAppender = static_cast<log4cplus::ConsoleAppender*>(pAppender);
        //if (pConsoleAppender != nullptr)
        //	pConsoleAppender->imbue(std::locale("zh_CN"));

        std::cout << "[log4] " << LOG4CPLUS_TSTRING_TO_STRING(logger.getName()) << " appender name:" << LOG4CPLUS_TSTRING_TO_STRING(pAppender->getName()) << std::endl;
        ++iter;
    }
}

void Log4::BackToPool()
{
}
