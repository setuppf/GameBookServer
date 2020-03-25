#pragma once

#include "libserver/entity.h"
#include "libserver/sync_component.h"

class SpaceSyncHandler :public SyncComponent, public Entity<SpaceSyncHandler>, public IAwakeSystem<>
{
public:
	void Awake() override;
	void BackToPool() override;

	bool GetSpaceApp(AppInfo* pInfo); // 得到一个当前最优的AppId

protected:
    void HandleAppInfoSync(Packet* pPacket);

};

