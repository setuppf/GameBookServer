#include "update_system.h"
#include "entity.h"
#include "update_component.h"

void UpdateSystem::Update(EntitySystem* pEntities)
{
	auto pCollections = pEntities->GetComponentCollections<UpdateComponent>();
	if (pCollections == nullptr)
		return;

	pCollections->Swap();

	auto lists = pCollections->GetAll();
	for (const auto one : lists)
	{
		const auto pComponent = one.second;
		const auto pUpdateComponent = static_cast<UpdateComponent*>(pComponent);
		pUpdateComponent->UpdataFunction();
	}
}
