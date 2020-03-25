#pragma once
#include "component.h"
#include "system.h"

class ResPath : public Component<ResPath>, public IAwakeSystem<>
{
public:
    void Awake();
    void BackToPool();

    std::string FindResPath(const std::string& res);
    std::string FindResPath(const char* res);

private:
    std::string _resPath{ "" };
};


