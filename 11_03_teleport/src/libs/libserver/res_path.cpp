#include "res_path.h"
#include <ostream>
#include "common.h"
#include "util_string.h"

#if ENGINE_PLATFORM != PLATFORM_WIN32	
#include <unistd.h>
#endif

void ResPath::Awake()
{
    // 获取引擎环境配置
#if ENGINE_PLATFORM != PLATFORM_WIN32		
    char* envValue = getenv("GENGINE_RES_PATH");
    _resPath = envValue == nullptr ? "" : envValue;
#else
    char* envValue = nullptr;
    size_t len;
    if (_dupenv_s(&envValue, &len, "GENGINE_RES_PATH") == 0 && envValue != nullptr)
    {
        _resPath = envValue;
        free(envValue);
    }
#endif

    // 默认路径
    if (_resPath.empty())
    {
#if ENGINE_PLATFORM != PLATFORM_WIN32		
        char name[PATH_MAX];
        int rslt = ::readlink("/proc/self/exe", name, PATH_MAX);
        if (rslt < 0 || rslt >= PATH_MAX)
        {
            std::cout << "ResPath::Initialize. res path failed." << std::endl;
            return;
        }

        name[rslt] = 0;
        _resPath = name;

        for (int i = 0; i < 2; i++)
        {
            const auto index = _resPath.find_last_of("/");
            _resPath = _resPath.substr(0, index);
        }
#else
        // window下的目录必须是全英文
        char name[PATH_MAX];
        ZeroMemory(name, PATH_MAX);
        const DWORD rslt = GetModuleFileNameA(nullptr, name, MAX_PATH);
        if (rslt <= 0 || rslt >= PATH_MAX)
        {
            std::cout << "ResPath::Initialize. res path failed." << std::endl;
            return;
        }

        _resPath = name;

        strutil::replace(_resPath, "\\", "/");
        strutil::replace(_resPath, "//", "/");

        for (int i = 0; i < 3; i++)
        {
            const auto index = _resPath.find_last_of("/");
            _resPath = _resPath.substr(0, index);
        }
#endif

    }

    _resPath += "/res";
    std::cout << "GENGINE_RES_PATH=" << _resPath << std::endl;

}

void ResPath::BackToPool()
{
}

std::string ResPath::FindResPath(const std::string& res)
{
    return FindResPath(res.c_str());
}

std::string ResPath::FindResPath(const char* res)
{
    auto fpath = _resPath + res;

    strutil::replace(fpath, "\\", "/");
    strutil::replace(fpath, "//", "/");

    return fpath;
}
