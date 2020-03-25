#pragma once
#include "component.h"
#include "system.h"
#include <functional>

class UpdateComponent :public Component<UpdateComponent>, public IAwakeFromPoolSystem<>
{
public:
	void Awake() override;
    void BackToPool() override;

	std::function<void()> UpdataFunction{ nullptr };	
};

