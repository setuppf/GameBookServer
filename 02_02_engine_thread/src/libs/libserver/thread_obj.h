#pragma once

#include "disposable.h"

class ThreadObject : public IDisposable {
public:
    virtual ~ThreadObject() {}

    virtual bool Init( ) = 0;
    virtual void RegisterMsgFunction( ) = 0;
    virtual void Update( ) = 0;

    bool IsActive() const;

protected:
    bool _active { true };
};
