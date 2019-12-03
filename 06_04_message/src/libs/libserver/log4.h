#pragma once

#include "common.h"
#include "app_type.h"
#include "component.h"
#include "system.h"

#include <log4cplus/logger.h>   

class Log4 : public Component<Log4>, public IAwakeSystem<APP_TYPE>
{
public:
    void Awake(APP_TYPE appType) override;
    void BackToPool() override;

    static std::string GetMsgIdName(Proto::MsgId msgId);

protected:
    void DebugInfo(log4cplus::Logger logger) const;

private:
    APP_TYPE _appType{ APP_TYPE::APP_None };
};
