#pragma once
class IWorld
{
public:
    int GetWorldId() const;

protected:
    int _worldId{ 0 };
};

