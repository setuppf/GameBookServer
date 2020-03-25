#pragma once

class ISystem
{
protected:
	ISystem() = default;

public:
	virtual ~ISystem() = default;
};

template <typename... TArgs>
class IAwakeSystem : virtual public ISystem
{
protected:
	IAwakeSystem() = default;

public:
	virtual ~IAwakeSystem() = default;
	virtual void Awake(TArgs... args) = 0;
};

template <typename... TArgs>
class IAwakeFromPoolSystem : virtual public ISystem
{
protected:
	IAwakeFromPoolSystem() = default;

public:
	virtual ~IAwakeFromPoolSystem() = default;
	virtual void AwakeFromPool(TArgs... args) = 0;
};

class IUpdateSystem : virtual public ISystem
{
protected:
	IUpdateSystem() = default;

public:
	virtual ~IUpdateSystem() = default;
	virtual void Update() = 0;
};

