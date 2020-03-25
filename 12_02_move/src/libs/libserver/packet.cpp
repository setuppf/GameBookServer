#include "packet.h"

Packet::Packet()
{
    _bufferSize = DEFAULT_PACKET_BUFFER_SIZE;
    _beginIndex = 0;
    _endIndex = 0;
    _buffer = new char[_bufferSize];

    _ref = 0;
    _isRefOpen = false;
    _socketKey.Clear();
    _tagKey.Clear();
}

Packet::~Packet()
{
    delete[] _buffer;
}

void Packet::Awake(Proto::MsgId msgId, NetIdentify* pIdentify)
{
    if (pIdentify != nullptr)
    {
        _socketKey.Clear();
        _tagKey.Clear();

        _socketKey.CopyFrom(pIdentify->GetSocketKey());
        _tagKey.CopyFrom(pIdentify->GetTagKey());
    }
    else
    {
        _socketKey.Clear();
        _tagKey.Clear();
    }

    _msgId = msgId;

    _beginIndex = 0;
    _endIndex = 0;
    _ref = 0;
    _isRefOpen = false;
}

void Packet::BackToPool()
{
    _msgId = Proto::MsgId::None;
    _socketKey.Clear();
    _tagKey.Clear();

    _beginIndex = 0;
    _endIndex = 0;
    _ref = 0;
    _isRefOpen = false;
}

void Packet::CopyFrom(Packet* pPacket)
{
    const auto total = pPacket->GetDataLength();
    while (GetEmptySize() < total)
    {
        ReAllocBuffer();
    }

    _beginIndex = 0;
    _endIndex = total;
    memcpy(_buffer, pPacket->GetBuffer(), _endIndex);
}

char* Packet::GetBuffer() const
{
    return _buffer;
}

unsigned short Packet::GetDataLength() const
{
    return _endIndex - _beginIndex;
}

int Packet::GetMsgId() const
{
    return _msgId;
}

void Packet::FillData(const unsigned int size)
{
    _endIndex += size;
}

void Packet::ReAllocBuffer()
{
    Buffer::ReAllocBuffer(_endIndex - _beginIndex);
}

void Packet::AddRef()
{
    ++_ref;
}

void Packet::RemoveRef()
{
    --_ref;
    if (_ref < 0)
    {
        const google::protobuf::EnumDescriptor* descriptor = Proto::MsgId_descriptor();
        const auto name = descriptor->FindValueByNumber(_msgId)->name();
        LOG_ERROR("packet ref < 0. ref:" << _ref << " msgId:" << name.c_str());
    }
}

void Packet::OpenRef()
{
    _isRefOpen = true;
}

bool Packet::CanBack2Pool() const
{
    if (!_isRefOpen)
        return false;

    if (_ref == 0)
        return true;

    return false;
}
