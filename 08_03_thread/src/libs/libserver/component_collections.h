#pragma once
#include "component.h"
#include "disposable.h"

#include <list>
#include <map>

// ComponentCollections 是按组件类型来存的
class ComponentCollections :public IDisposable
{
public:
    ComponentCollections(std::string componentName);
    void Add(IComponent* pObj);
    void Remove(uint64 sn);

    IComponent* Get(uint64 sn = 0);
    std::map<uint64, IComponent*>& GetAll();
       
    void Swap();
    void Dispose() override;

    std::string GetClassType() const;

private:
    std::map<uint64, IComponent*> _objs;
    std::map<uint64, IComponent*> _addObjs;
    std::list<uint64> _removeObjs;

    std::string _componentName;
};
