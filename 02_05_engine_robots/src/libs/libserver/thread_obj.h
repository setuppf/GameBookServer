#pragma once

#include "message_list.h"

class Thread;
class ThreadObject : public MessageList
{
public:
    virtual bool Init() = 0;
    virtual void RegisterMsgFuntion() = 0;
    virtual void Update() = 0;    

    void SetThread(Thread* pThread);
    Thread* GetThread() const;
    void OnUpdate();
    bool IsActive() const;
    void Dispose() override;
    

protected:
    bool _active{ true };
    Thread* _pThread{ nullptr };
};
