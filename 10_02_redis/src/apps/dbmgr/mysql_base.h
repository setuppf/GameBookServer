#pragma once

#include "libserver/common.h"
#include "libserver/yaml.h"

#include <mysql/mysql.h>

class MysqlBase
{
public:
    bool ConnectInit();
    virtual void Disconnect();

    int CheckMysqlError() const;

    bool Query(const char* sql, my_ulonglong& affected_rows);
    MYSQL_ROW Fetch() const;

    static int GetInt(MYSQL_ROW row, int index);
    static unsigned int GetUint(MYSQL_ROW row, int index);
    static uint64 GetUint64(MYSQL_ROW row, int index);
    static char* GetString(MYSQL_ROW row, int index);
    int GetBlob(MYSQL_ROW row, int index, char* buf, unsigned long size) const;
    void GetBlob(MYSQL_ROW row, int index, std::string& protoStr) const;

protected:
    DBConfig* _pDbCfg{ nullptr };

    // 
    MYSQL* _pMysql{ nullptr };
    MYSQL_RES* _pMysqlRes{ nullptr };
    int _numFields{ 0 };
    MYSQL_FIELD* _pMysqlFields{ nullptr };
};

