#pragma once
#include "component.h"
#include "system.h"
#include "message_system.h"

#include "global.h"
#include "cache_swap.h"
#include "disposable.h"
#include "component_factory.h"
#include "object_pool.h"

class Packet;
class IEntity;

class EntitySystem : virtual public SnObject, public IDisposable
{
public:
	virtual ~EntitySystem();
    friend class IEntity;
    void InitComponent();

	template <class T, typename... TArgs>
	T* AddComponent(TArgs... args);

	template <typename... TArgs>
	IComponent* AddComponentByName(std::string className, TArgs... args);

	template <class T>
	T* GetComponent();

	virtual void Update();
    void UpdateMessage();

	void Dispose() override;

	void AddPacketToList(Packet* pPacket);

protected:
    void AddToSystem(IComponent* pObj);

protected:
	std::list<IUpdateSystem*> _updateSystems;
    std::list<IMessageSystem*> _messageSystems;

	// 所有对象
	std::map<uint64, IComponent*> _objSystems;

	// 本线程中的所有待处理包
	std::mutex _packet_lock;
	CacheSwap<Packet> _cachePackets;
};

template <class T, typename ... TArgs>
T* EntitySystem::AddComponent(TArgs... args)
{
	auto pComponent = DynamicObjectPool<T>::GetInstance()->MallocObject(std::forward<TArgs>(args)...);
    AddToSystem(pComponent);
	return pComponent;
} 

template<typename ...TArgs>
inline IComponent* EntitySystem::AddComponentByName(std::string className, TArgs ...args)
{
	auto pComponent = ComponentFactory<TArgs...>::GetInstance()->Create(className, std::forward<TArgs>(args)...);
	if (pComponent == nullptr)
		return nullptr;

    AddToSystem(pComponent);
	return pComponent;
}

template <class T>
T* EntitySystem::GetComponent()
{
	auto iter = std::find_if(_objSystems.begin(), _objSystems.end(), [](auto pair)
		{
			if (dynamic_cast<T*>(pair.second) != nullptr)
				return true;

			return false;
		});

	if (iter == _objSystems.end())
		return nullptr;

	return dynamic_cast<T*>(iter->second);
}
