#pragma once
#include "libserver/message_system.h"
#include "libserver/component.h"

class PlayerComponentOnlineInLogin :public Component<PlayerComponentOnlineInLogin>, public IAwakeFromPoolSystem<std::string>
{
public:
    void Awake(std::string account) override;
    void BackToPool() override;

    void SetOnlineFlag() const;

private:
    std::string _account{ "" };
};

