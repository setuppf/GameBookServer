#pragma once

class ISystem
{
protected:
	ISystem() = default;

public:
	virtual ~ISystem() = default;
};

class IInitializeSystem : virtual public ISystem
{
protected:
	IInitializeSystem() = default;

public:
	virtual ~IInitializeSystem() = default;

	virtual void Initialize() = 0;
};

class IUpdateSystem : virtual public ISystem
{
protected:
	IUpdateSystem() = default;

public:
	virtual ~IUpdateSystem() = default;

	virtual void Update() = 0;
};