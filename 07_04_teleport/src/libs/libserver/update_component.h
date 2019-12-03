#pragma once
#include "component.h"
#include "system.h"
#include <functional>

using UpdateCallBackFun = std::function<void()>;
class UpdateComponent :public Component<UpdateComponent>, public IAwakeFromPoolSystem<UpdateCallBackFun>
{
public:
	void Awake(UpdateCallBackFun fun) override;
    void BackToPool() override;
    void Update() const;

private:
    UpdateCallBackFun _function{ nullptr };
};

