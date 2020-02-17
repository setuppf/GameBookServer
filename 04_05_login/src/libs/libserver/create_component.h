#pragma once

#include "entity.h"
#include "system.h"
#include "message_system.h"

class Packet;
class CreateComponentC :public Entity<CreateComponentC>, public IMessageSystem, public IAwakeFromPoolSystem<>
{
public:
    void AwakeFromPool() override {}
    void RegisterMsgFunction() override;
    void BackToPool() override;

private:
    void HandleCreateComponent(Packet* pPacket) const;
    void HandleRemoveComponent(Packet* pPacket);
};

