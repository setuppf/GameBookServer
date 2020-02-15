#pragma once

#include "message_list.h"
#include "sn_object.h"

class Thread;
class ThreadObject : public MessageList, public SnObject
{
public:
    virtual bool Init() = 0;
    virtual void RegisterMsgFunction() = 0;
    virtual void Update() = 0;    

    void SetThread(Thread* pThread);
    Thread* GetThread() const;
    bool IsActive() const;
    void Dispose() override;
    
protected:
    bool _active{ true };
    Thread* _pThread{ nullptr };
};
