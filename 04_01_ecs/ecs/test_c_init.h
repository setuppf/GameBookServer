#pragma once
#include "system.h"
#include "component.h"

class TestCInit :public Component, public IInitializeSystem
{
public:
	void Initialize() override;
	void Dispose() override;
};

