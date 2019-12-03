#pragma once
#include "component.h"
#include "system.h"

class TestCUpdate :public Component, public IUpdateSystem
{
public:
	void Dispose() override;
	void Update() override;

private:
    bool _isShow{ false };
};

