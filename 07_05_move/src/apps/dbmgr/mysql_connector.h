#pragma once

#include "libserver/common.h"
#include "libserver/entity.h"
#include "libserver/system.h"
#include "libserver/socket_object.h"

#include "mysql_base.h"

#include <mysql/mysql.h>

class Packet;

enum class DatabaseStmtKey
{
	Create,
	Save,
};

struct DatabaseStmt
{
	MYSQL_STMT *stmt{ nullptr };
	MYSQL_BIND *bind{ nullptr };
	char *bind_buffer{ nullptr };
	int bind_index;
	int bind_buffer_index;

	void Close()
	{
        if (bind != nullptr) 
        {
            delete[] bind;
            bind = nullptr;
        }

        if (bind_buffer != nullptr) 
        {
            delete[] bind_buffer;
            bind_buffer = nullptr;
        }

        if (stmt != nullptr) 
        {
            mysql_stmt_close(stmt);
            stmt = nullptr;
        }
	}
};

#define MAX_BIND_BUFFER    40960
#define MAX_BIND_STR       30000

class MysqlConnector : public MysqlBase, public Entity<MysqlConnector>, public IAwakeSystem<>
{
public:
	void Awake() override;
	void BackToPool() override;
    void InitMessageComponent();

	bool Connect();
	void Disconnect() override;

private:
	void ReConnect();

	void CheckPing();

#pragma region 预处理

	void InitStmts();
	void CleanStmts();
	DatabaseStmt* GetStmt(DatabaseStmtKey stmtKey);

	DatabaseStmt* CreateStmt(const char *sql) const;
	void ClearStmtParam(DatabaseStmt* stmt);
	static void AddParamStr(DatabaseStmt* stmt, const char* value);
	static void AddParamInt(DatabaseStmt* stmt, int val);
	static void AddParamUint64(DatabaseStmt* stmt, uint64 val);
	static void AddParamBlob(DatabaseStmt* stmt, void *val, int size);
	bool ExecuteStmt(DatabaseStmt* stmt, my_ulonglong& affected_rows);
	bool ExecuteStmt(DatabaseStmt* stmt);

#pragma endregion 

	// 协议处理
	void HandleQueryPlayerList(Packet* pPacket);
    void QueryPlayerList(std::string account, NetIdentify* pIdentify);

	void HandleQueryPlayer(Packet* pPacket);

	void HandleCreatePlayer(Packet* pPacket);
	void HandleSavePlayer(Packet* pPacket);
	bool OnSavePlayer(DatabaseStmt* stmtSave, Proto::Player& protoPlayer);

protected:
	// stmt
	std::map<DatabaseStmtKey, DatabaseStmt*> _mapStmt;

	bool _isRunning{ false };
};

