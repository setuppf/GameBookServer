#pragma once

#include <string>

#include "resource_base.h"
#include "resource_mgr_template.h"
#include "libserver/vector3.h"

enum class ResourceWorldType
{
    Login = 1,
    Roles = 2, // 角色选择场景
    Public = 3,
    Dungeon = 4,
};

class ResourceWorld : public ResourceBase
{
public:
    explicit ResourceWorld(std::map<std::string, int>& head);
    bool Check() override;

    std::string GetName() const;
    ResourceWorldType GetType() const;
    bool IsType(ResourceWorldType iType) const;
    bool IsInitMap() const;
    Vector3 GetInitPosition() const;

protected:
    void GenStruct() override;

private:
    std::string _name{ "" };
    bool _isInit{ false };
    ResourceWorldType _worldType{ ResourceWorldType::Dungeon };
    Vector3 _initPosition{ 0,0,0 };
};

class ResourceWorldMgr :public ResourceManagerTemplate<ResourceWorld>
{
public:
    bool AfterInit() override;
    ResourceWorld* GetInitMap();

private:
    int _initMapId{ 0 };
};
