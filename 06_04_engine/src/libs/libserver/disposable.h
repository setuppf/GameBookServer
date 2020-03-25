#pragma once

class IDisposable
{
public:
	virtual ~IDisposable()
	{
	}

	virtual void Dispose() = 0;
};