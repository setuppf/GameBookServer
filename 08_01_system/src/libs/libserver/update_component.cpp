#include "update_component.h"

void UpdateComponent::AwakeFromPool()
{
}

void UpdateComponent::BackToPool()
{
	UpdataFunction = nullptr;
}
