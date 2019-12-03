#include "update_component.h"

void UpdateComponent::Awake(UpdateCallBackFun fun)
{
    _function = fun;
}

void UpdateComponent::BackToPool()
{
    _function = nullptr;
}

void UpdateComponent::Update() const
{
    _function();
}
