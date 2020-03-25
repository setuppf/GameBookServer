#pragma once
#include "util_time.h"
#include "system.h"
#include "entity.h"

#include <list>
#include <vector>

struct Timer
{
    // 下次调用时间
    timeutil::Time NextTime;

    // 调用函数
    TimerHandleFunction Handler;

    // 首次执行时延迟秒
    int DelaySecond;

    // 间隔时间(秒）
    int DurationSecond;

    // 总调用次数（0为无限）
    int CallCountTotal;

    // 当前调用次数 
    int CallCountCur;

    // 方便删除数据时找到Timer
    uint64 SN;
};

class TimerComponent :public Entity<TimerComponent>, public IAwakeSystem<>
{
public:
    void Awake() override;
    void BackToPool() override;

    uint64 Add(int total, int durations, bool immediateDo, int immediateDoDelaySecond, TimerHandleFunction handler);
    void Remove(std::list<uint64>& timers);

    bool CheckTime();
    Timer PopTimeHeap();

    void Update();

protected:
    void Add(Timer& data);

private:
    std::vector<Timer> _heap;
};