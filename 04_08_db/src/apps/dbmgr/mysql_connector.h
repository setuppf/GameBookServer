#pragma once

#include "libserver/common.h"
#include "libserver/yaml.h"
#include "libserver/util_time.h"
#include "libserver/entity.h"
#include "libserver/system.h"
#include "libserver/message_system.h"
#include "mysql_base.h"

#include <mysql/mysql.h>
#include <vector>

class Packet;

enum DatabaseStmtKey
{
	StmtCreate,
	StmtSave,
};

struct DatabaseStmt
{
	MYSQL_STMT *stmt{ nullptr };
	MYSQL_BIND *bind{ nullptr };
	char *bind_buffer{ nullptr };
	int bind_index;
	int bind_buffer_index;

	void Close() const
	{
		if (bind != nullptr)
			delete[] bind;

		if (bind_buffer != nullptr)
			delete bind_buffer;

		if (stmt != nullptr)
			mysql_stmt_close(stmt);
	}
};

#define MAX_BIND_BUFFER    40960
#define MAX_BIND_STR       30000

class MysqlConnector : public MysqlBase, public Entity<MysqlConnector>, public IMessageSystem, public IAwakeFromPoolSystem<>
{
public:
	void AwakeFromPool() override;
	void BackToPool() override;
    void RegisterMsgFunction() override;

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
    void QueryPlayerList(std::string account, SOCKET socket);

	void HandleQueryPlayer(Packet* pPacket);

	void HandleCreatePlayer(Packet* pPacket);
	void HandleSavePlayer(Packet* pPacket);
	bool OnSavePlayer(DatabaseStmt* stmtSave, Proto::Player& protoPlayer);

protected:
	// stmt
	std::map<DatabaseStmtKey, DatabaseStmt*> _mapStmt;

	bool _isRunning{ false };
};

