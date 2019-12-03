#pragma once

#include "common.h"

#include <log4cplus/logger.h>   

#include "system.h"
#include "entity.h"

class Log4 : public Singleton<Log4>
{
public:
    Log4(int appType);
    ~Log4();

    static std::string GetMsgIdName(Proto::MsgId msgId);

protected:
    void DebugInfo(log4cplus::Logger logger) const;

private:
    APP_TYPE _appType{ APP_TYPE::APP_None };
};
