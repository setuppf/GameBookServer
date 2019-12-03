#pragma once

#include "libserver/log4_help.h"
#include "libserver/res_path.h"
#include "libserver/global.h"

#include "resource_base.h"

template<class T>
class ResourceManagerTemplate {
public:
    virtual ~ResourceManagerTemplate( ) {
        _refs.clear( );
    }

    bool Initialize(std::string table, ResPath* pResPath);
    virtual bool AfterInit( ) { return true; }
    T* GetResource(int id);

protected:
    bool ParserHead(std::string line);
    bool LoadReference(std::string line);

protected:
    std::string _cvsName;
    std::map<std::string, int> _head;
    std::map<int, T*> _refs;
};

template <class T>
bool ResourceManagerTemplate<T>::Initialize(std::string table, ResPath* pResPath)
{
    _cvsName = table;
    std::string path = pResPath->FindResPath("/resource");
    path = strutil::format("%s/%s.csv", path.c_str(), table.c_str());
    std::ifstream reader(path.c_str(), std::ios::in);
    if (!reader)
    {
        LOG_ERROR("can't open file. " << path.c_str( ));
        return false;
    }

    LOG_DEBUG("load file. " << path.c_str( ));

    if (reader.eof())
    {
        LOG_ERROR("read head failed. stream is eof.");
        return false;
    }

    std::string line;
    std::getline(reader, line);
    std::transform(line.begin(), line.end(), line.begin(), ::tolower);

    if (!ParserHead(line))
    {
        LOG_ERROR("parse head failed. " << path.c_str( ));
        return false;
    }

    while (true)
    {
        if (reader.eof())
            break;

        std::getline(reader, line);

        if (line.empty())
            continue;

        std::transform(line.begin(), line.end(), line.begin(), ::tolower);

        LoadReference(line);
    }

    if (!AfterInit())
        return false;

    return true;
}

template <class T>
T* ResourceManagerTemplate<T>::GetResource(int id)
{
    auto iter = _refs.find(id);
    if (iter == _refs.end())
        return nullptr;

    return _refs[id];
}

template <class T>
bool ResourceManagerTemplate<T>::ParserHead(std::string line)
{
    if (line.empty())
        return false;

    std::vector<std::string> propertyList = ResourceBase::ParserLine(line);

    for (size_t i = 0; i < propertyList.size(); i++)
    {
        _head.insert(std::make_pair(propertyList[i], i));
    }

    return true;
}

template <class T>
bool ResourceManagerTemplate<T>::LoadReference(std::string line)
{
    auto pT = new T(_head);
    if (pT->LoadProperty(line) && pT->Check())
    {
        _refs.insert(std::make_pair(pT->GetId(), pT));
        return true;
    }

    return false;
}
