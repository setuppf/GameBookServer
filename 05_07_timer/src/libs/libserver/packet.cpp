#include "packet.h"

Packet::Packet()
{
    _bufferSize = DEFAULT_PACKET_BUFFER_SIZE;
    _beginIndex = 0;
    _endIndex = 0;
    _buffer = new char[_bufferSize];

    _ref = 0;
    _isRefOpen = false;
    _socket = INVALID_SOCKET;
}

Packet::~Packet()
{
    delete[] _buffer;
}

void Packet::Awake(Proto::MsgId msgId, SOCKET socket)
{
    _socket = socket;
    _msgId = msgId;
    _beginIndex = 0;
    _endIndex = 0;
    _ref = 0;
    _isRefOpen = false;
}

void Packet::BackToPool()
{
    _msgId = Proto::MsgId::None;
    _beginIndex = 0;
    _endIndex = 0;
    _ref = 0;
    _isRefOpen = false;
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

SOCKET Packet::GetSocket() const
{
    return _socket;
}

void Packet::SetSocket(SOCKET socket)
{
    _socket = socket;
}

void Packet::AddRef()
{
    _ref++;
}

void Packet::RemoveRef()
{
    _ref--;
}

void Packet::OpenRef()
{
    _isRefOpen = true;
}

bool Packet::CanBack2Pool()
{
    if (!_isRefOpen)
        return false;

    if (_ref == 0)
        return true;

    return false;
}
