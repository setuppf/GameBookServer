#pragma once

#include "mysql_base.h"
#include "libserver/singleton.h"

#include <functional>

class MysqlTableUpdate :public MysqlBase, public Singleton<MysqlTableUpdate>
{
public:
    MysqlTableUpdate();
    virtual ~MysqlTableUpdate();

    void Check();

private:
    bool CreateDatabaseIfNotExist();

    // 检查DB数据，更新到最新版本
    bool UpdateToVersion();
    bool Update00();

private:
    // update
    typedef std::function<bool(void)> OnUpdate;
    std::vector<OnUpdate> _update_func;

    int const _version = 0;
};
