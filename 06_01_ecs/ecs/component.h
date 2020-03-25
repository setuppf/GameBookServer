#pragma once
#include <memory>

class Entity;
class Component
{
public:
    virtual ~Component() = default;
    virtual void Dispose() = 0;

    void SetParent(Entity* pObj);
    Entity* GetParent() const;

    long GetSN() const;
    void SetSN(const long sn);

private:
    Entity* _parent{ nullptr };
    long _sn{ 0 };
};
