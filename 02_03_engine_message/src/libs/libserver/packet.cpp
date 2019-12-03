#include "packet.h"

//Packet::Packet()
//{
//    _socket = INVALID_SOCKET;
//    _msgId = 0;
//    CleanBuffer();
//
//    _bufferSize = DEFAULT_PACKET_BUFFER_SIZE;
//    _beginIndex = 0;
//    _endIndex = 0;
//    _buffer = new char[_bufferSize];
//}

Packet::Packet(const int msgId, SOCKET socket)
{
    _socket = socket;
    _msgId = msgId;
    CleanBuffer();

    _bufferSize = DEFAULT_PACKET_BUFFER_SIZE;
    _beginIndex = 0;
    _endIndex = 0;
    _buffer = new char[_bufferSize];
}

Packet::~Packet()
{
    CleanBuffer();
}

void Packet::Dispose()
{
    _msgId = 0;
    _beginIndex = 0;
    _endIndex = 0;
}

void Packet::CleanBuffer()
{
    if (_buffer != nullptr)
        delete[] _buffer;

    _beginIndex = 0;
    _endIndex = 0;
    _bufferSize = 0;
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
