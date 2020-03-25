#include "create_component.h"
#include "common.h"
#include "component_factory.h"
#include "packet.h"
#include "entity.h"
#include "message_component.h"
#include "system_manager.h"

template <typename... TArgs, size_t... Index>
IComponent* ComponentFactoryEx(EntitySystem* pEntitySystem, std::string className, const std::tuple<TArgs...>& args, std::index_sequence<Index...>)
{
    return pEntitySystem->AddComponentByName(className, std::get<Index>(args)...);
}

template<size_t ICount>
struct DynamicCall
{
    template <typename... TArgs>
    static IComponent* Invoke(EntitySystem* pEntitySystem, const std::string classname, std::tuple<TArgs...> t1, google::protobuf::RepeatedPtrField<::Proto::CreateComponentParam>& params)
    {
        if (params.size() == 0)
        {
            return ComponentFactoryEx(pEntitySystem, classname, t1, std::make_index_sequence<sizeof...(TArgs)>());
        }

        Proto::CreateComponentParam param = (*(params.begin()));
        params.erase(params.begin());

        if (param.type() == Proto::CreateComponentParam::Int)
        {
            auto t2 = std::tuple_cat(t1, std::make_tuple(param.int_param()));
            return DynamicCall<ICount - 1>::Invoke(pEntitySystem, classname, t2, params);
        }

        if (param.type() == Proto::CreateComponentParam::String)
        {
            auto t2 = std::tuple_cat(t1, std::make_tuple(param.string_param()));
            return DynamicCall<ICount - 1>::Invoke(pEntitySystem, classname, t2, params);
        }

        return nullptr;
    }
};

template<>
struct DynamicCall<0>
{
    template <typename... TArgs>
    static IComponent* Invoke(EntitySystem* pEntitySystem, const std::string classname, std::tuple<TArgs...> t1, google::protobuf::RepeatedPtrField<::Proto::CreateComponentParam>& params)
    {
        return nullptr;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CreateComponentC::AwakeFromPool()
{
    auto pMsgCallBack = new MessageCallBackFunction();
    AddComponent<MessageComponent>(pMsgCallBack);

    pMsgCallBack->RegisterFunction(Proto::MsgId::MI_CreateComponent, BindFunP1(this, &CreateComponentC::HandleCreateComponent));
    pMsgCallBack->RegisterFunction(Proto::MsgId::MI_RemoveComponent, BindFunP1(this, &CreateComponentC::HandleRemoveComponent));
}

void CreateComponentC::BackToPool()
{

}

void CreateComponentC::HandleCreateComponent(Packet* pPacket) const
{
    Proto::CreateComponent proto = pPacket->ParseToProto<Proto::CreateComponent>();
    const std::string className = proto.class_name();

    if (proto.params_size() >= 5)
    {
        std::cout << " !!!! CreateComponent failed. className:" << className.c_str() << " params size >= 5" << std::endl;
        return;
    }

    auto params = proto.params();
    const auto pObj = DynamicCall<5>::Invoke(GetSystemManager()->GetEntitySystem(), className, std::make_tuple(), params);
    if (pObj == nullptr)
    {
        std::cout << " !!!! CreateComponent failed. className:" << className.c_str() << std::endl;
    }

    //std::cout << "CreateComponent. name:" << className << std::endl;
}

void CreateComponentC::HandleRemoveComponent(Packet* pPacket)
{
    Proto::RemoveComponent proto = pPacket->ParseToProto<Proto::RemoveComponent>();
    //GetEntitySystem( )->RemoveFromSystem( proto.sn( ) );
}
