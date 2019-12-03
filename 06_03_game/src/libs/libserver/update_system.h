#pragma once

#include "system.h"

class UpdateSystem : virtual public ISystem
{
public:
	void Update(EntitySystem* pEntities) override;
};
