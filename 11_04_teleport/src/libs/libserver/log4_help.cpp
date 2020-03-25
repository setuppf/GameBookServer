#include "log4_help.h"
#include "common.h"

#include <list>

#if ENGINE_PLATFORM == PLATFORM_WIN32

void SetColor(const int colorEx)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorEx);
}

#endif

#if LOG_MSG_OPEN

bool IsLogShowMsg(const int msgId)
{
    std::list<int> lists;

    //lists.push_back((int)Proto::MI_Ping);
    //lists.push_back((int)Proto::MI_AppInfoSync);
    //lists.push_back((int)Proto::MI_WorldInfoSyncToGather);
    //lists.push_back((int)Proto::MI_WorldProxyInfoSyncToGather);

    const auto iter = std::find(lists.begin(), lists.end(), msgId);
    if (iter != lists.end())
        return false;

    return true;
}

#endif

std::string Log4Help::GetMsgIdName(Proto::MsgId msgId)
{
    const google::protobuf::EnumDescriptor* descriptor = Proto::MsgId_descriptor();
    return descriptor->FindValueByNumber(msgId)->name();
}

std::string Log4Help::GetMsgIdName(int msgId)
{
    return Log4Help::GetMsgIdName((Proto::MsgId)msgId);
}
