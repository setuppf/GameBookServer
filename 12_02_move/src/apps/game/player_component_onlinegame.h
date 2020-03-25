#pragma once
#include "libserver/system.h"
#include "libserver/component.h"
#include "libplayer/player_component.h"

class PlayerComponentOnlineInGame :public Component<PlayerComponentOnlineInGame>,
    public PlayerComponent,
    virtual public IAwakeFromPoolSystem<std::string, int>,
    virtual public IAwakeFromPoolSystem<std::string>
{
public:
    void Awake(std::string account, int version) override;
    void Awake(std::string account) override;

    void BackToPool() override;
    void SetOnlineFlag() const;

    void ParserFromProto(const Proto::Player& proto) override;
    void SerializeToProto(Proto::Player* pProto) override;

private:
    std::string _account{ "" };
    int _onlineVersion{ 0 };
    bool _isReadFromeDB{ true };
};

