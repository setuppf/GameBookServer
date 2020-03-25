#pragma once

#include "base_buffer.h"
#include "common.h"
#include "entity.h"
#include "system.h"
#include "socket_object.h"

#pragma pack(push)
#pragma pack(4)

struct PacketHead {
    unsigned short MsgId;
};

#pragma pack(pop)

#if TestNetwork
#define DEFAULT_PACKET_BUFFER_SIZE	10
#else
// Ä¬ÈÏ´óÐ¡ 10KB
#define DEFAULT_PACKET_BUFFER_SIZE	1024 * 10
#endif

class Packet : public Entity<Packet>, public Buffer, public NetworkIdentify, public IAwakeFromPoolSystem<Proto::MsgId, NetworkIdentify*>
{
public:
    Packet();
    ~Packet();
    void Awake(Proto::MsgId msgId, NetworkIdentify* pIdentify) override;

    template<class ProtoClass>
    ProtoClass ParseToProto()
    {
        ProtoClass proto;
        proto.ParsePartialFromArray(GetBuffer(), GetDataLength());
        return proto;
    }

    template<class ProtoClass>
    void SerializeToBuffer(ProtoClass& protoClase)
    {
        auto total = (unsigned int)protoClase.ByteSizeLong();
        while (GetEmptySize() < total)
        {
            ReAllocBuffer();
        }

        protoClase.SerializePartialToArray(GetBuffer(), total);
        FillData(total);
    }

    void SerializeToBuffer(const char* s, unsigned int len)
    {
        while (GetEmptySize() < len)
        {
            ReAllocBuffer();
        }

        ::memcpy(_buffer + _endIndex, s, len);
        FillData(len);
    }

    void BackToPool() override;

    char* GetBuffer() const;
    unsigned short GetDataLength() const;
    int GetMsgId() const;
    void FillData(unsigned int size);
    void ReAllocBuffer();

    // ref
    void AddRef();
    void RemoveRef();
    void OpenRef();
    bool CanBack2Pool();

private:
    Proto::MsgId _msgId;

private:
    std::atomic<int> _ref{ 0 };
    bool _isRefOpen{ false };
};
