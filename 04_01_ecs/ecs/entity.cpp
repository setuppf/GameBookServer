#include "entity.h"

void Entity::AddComponent(Component* pObj ) {
    pObj->SetParent(this);
	_components.insert( std::make_pair(pObj->GetSN( ), pObj) );
}

void Entity::Dispose( ) {
	for ( const auto &one : _components ) {
		one.second->Dispose( );
	}
}
