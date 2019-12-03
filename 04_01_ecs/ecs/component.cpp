#include "component.h"

void Component::SetParent(Entity* pObj)
{
	_parent = pObj;
}

Entity* Component::GetParent() const
{
	return _parent;
}

long Component::GetSN() const
{
	return _sn;
}

void Component::SetSN(const long sn)
{
	_sn = sn;
}
