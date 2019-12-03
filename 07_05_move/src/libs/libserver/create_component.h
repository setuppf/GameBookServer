#pragma once

#include "entity.h"
#include "system.h"
#include "message_system.h"

class Packet;
class CreateComponentC :public Entity<CreateComponentC>, public IAwakeSystem<>
{
public:
    void Awake() override;
    void BackToPool() override;

private:
    void HandleCreateComponent(Packet* pPacket) const;
    void HandleRemoveComponent(Packet* pPacket);
    void HandleCreateSystem(Packet* pPacket);
};

