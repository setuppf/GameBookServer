#pragma once
#include "entity.h"
#include "system.h"

class TestEntityWithUpdate :public Entity, public IUpdateSystem
{
public:
    virtual void Update() override;

private:
    bool _isShow{ false };
};

class TestEntityWithInitAndUpdate:public Entity, public IInitializeSystem, public IUpdateSystem
{
public:
    virtual void Initialize() override;
    virtual void Update() override;

private:
    bool _isShow{ false };
};

