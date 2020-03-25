#pragma once

#include "network.h"

class NetworkListen :public Network,
    virtual public IAwakeSystem<std::string, int>,
    virtual public IAwakeSystem<int, int>
{
public:
    void Awake(std::string ip, int port) override;
    void Awake(int appType, int appId) override;
    void Awake(std::string ip, int port, NetworkType iType);

    void BackToPool() override;

    virtual void Update();
    const char* GetTypeName() override;
    uint64 GetTypeHashCode() override;
    void CmdShow();

private:
    void HandleListenKey(Packet* pPacket);

protected:
    virtual int Accept();
#ifdef EPOLL
    virtual void OnEpoll(SOCKET fd, int index) override;
#endif

private:

#ifdef EPOLL
    int _mainSocketEventIndex{ -1 };
#endif

    SOCKET _masterSocket{ INVALID_SOCKET };
};
