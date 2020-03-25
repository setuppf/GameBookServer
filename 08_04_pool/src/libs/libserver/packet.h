#pragma once

#include "base_buffer.h"
#include "common.h"
#include "entity.h"
#include "system.h"

#pragma pack(push)
#pragma pack(4)

struct PacketHead {
    unsigned short MsgId;
};

struct PacketInnerHead :public PacketHead
{
    unsigned int ThreadType;
    unsigned short ChooseType;
};

#pragma pack(pop)

#if TestNetwork
#define DEFAULT_PACKET_BUFFER_SIZE	10
#else
// Ä¬ÈÏ´óÐ¡ 10KB
#define DEFAULT_PACKET_BUFFER_SIZE	1024 * 10
#endif

class Packet : public Entity<Packet>, public Buffer,
    public IAwakeFromPoolSystem<Proto::MsgId, SOCKET>
{
public:
    Packet();
    ~Packet();
    void Awake(Proto::MsgId msgId, SOCKET socket);

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

    void BackToPool();
    void CleanBuffer();

    char* GetBuffer() const;
    unsigned short GetDataLength() const;
    int GetMsgId() const;
    void FillData(unsigned int size);
    void ReAllocBuffer();
    SOCKET GetSocket() const;
    void SetSocket(SOCKET socket);

private:
    Proto::MsgId _msgId;
    SOCKET _socket;
};
