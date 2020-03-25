#include "resource_world.h"
#include "libserver/log4_help.h"

ResourceWorld::ResourceWorld(std::map<std::string, int>& head) : ResourceBase(head)
{

}

std::string ResourceWorld::GetName() const
{
    return _name;
}

ResourceWorldType ResourceWorld::GetType() const
{
    return _worldType;
}


bool ResourceWorld::IsInitMap() const
{
    return _isInit;
}

bool ResourceWorld::IsType(ResourceWorldType iType) const
{
    return _worldType == iType;
}

Vector3 ResourceWorld::GetInitPosition() const
{
    return _initPosition;
}

void ResourceWorld::GenStruct()
{
    _name = GetString("name");
    _isInit = GetBool("init");
    _worldType = static_cast<ResourceWorldType>(GetInt("type"));

    std::string value = GetString("playerinitpos");
    std::vector<std::string> params;
    strutil::split(value, ',', params);
    if (params.size() == 3)
    {
        _initPosition.X = std::stof(params[0]);
        _initPosition.Y = std::stof(params[1]);
        _initPosition.Z = std::stof(params[2]);
    }
}

bool ResourceWorld::Check()
{
    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////


bool ResourceWorldMgr::AfterInit()
{
    auto iter = _refs.begin();
    while (iter != _refs.end())
    {
        auto pRef = iter->second;

        if (pRef->IsInitMap())
        {
            if (_initMapId != 0)
            {
                LOG_ERROR("map has tow init Map. id1:" << _initMapId << " id2:" << pRef->GetId());
            }
            _initMapId = pRef->GetId();
        }

        if (pRef->IsType(ResourceWorldType::Roles))
        {
            _rolesMapId = pRef->GetId();
        }

        ++iter;
    }

    return true;
}

ResourceWorld* ResourceWorldMgr::GetInitMap()
{
    if (_initMapId > 0)
        return GetResource(_initMapId);

    return nullptr;
}

ResourceWorld* ResourceWorldMgr::GetRolesMap()
{
    if (_rolesMapId > 0)
        return GetResource(_rolesMapId);

    return nullptr;
}
