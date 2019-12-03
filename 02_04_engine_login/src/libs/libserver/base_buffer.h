#pragma once
#include "disposable.h"

// 追加大小
#define ADDITIONAL_SIZE 10 //1024 * 128

// 最大缓冲
#define MAX_SIZE		1024 * 1024 // 1M

class Buffer :public IDisposable
{
public:
	virtual unsigned int GetEmptySize();
	void ReAllocBuffer(unsigned int dataLength);
	unsigned int GetEndIndex() const
	{
		return _endIndex;
	}

	unsigned int GetBeginIndex() const
	{
		return _beginIndex;
	}

	unsigned int GetTotalSize() const
	{
		return _bufferSize;
	}

protected:
	char* _buffer{ nullptr };
	unsigned int _beginIndex{ 0 }; // buffer数据 开始位与结束位
	unsigned int _endIndex{ 0 };

	unsigned int _bufferSize{ 0 }; // 总长度
};

