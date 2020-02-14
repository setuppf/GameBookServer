#include "thread.h"
#include "global.h"

#include <iterator>

void ThreadObjectList::AddObject(ThreadObject* obj)
{
    std::lock_guard<std::mutex> guard(_obj_lock);

    // 在加入之前初始化一下
    if (!obj->Init())
    {
        std::cout << "AddObject Failed. ThreadObject init failed." << std::endl;
    }
    else
    {
        obj->RegisterMsgFunction();
        _objlist.GetAddCache()->emplace_back(obj);

        const auto pThread = dynamic_cast<Thread*>(this);
        if (pThread != nullptr)
            obj->SetThread(pThread);
    }
}

void ThreadObjectList::Update()
{
    _obj_lock.lock();
    if (_objlist.CanSwap())
    {
        auto pDelList = _objlist.Swap();
        for (auto pOne : pDelList)
        {
            pOne->Dispose();
            delete pOne;
        }
    }
    _obj_lock.unlock();

    _packet_lock.lock();
    if (_cachePackets.CanSwap())
    {
        _cachePackets.Swap();
    }
    _packet_lock.unlock();

    auto pList = _objlist.GetReaderCache();
    auto pMsgList = _cachePackets.GetReaderCache();

    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        auto pObj = (*iter);
        for (auto itMsg = pMsgList->begin(); itMsg != pMsgList->end(); ++itMsg)
        {
            auto pPacket = (*itMsg);
            if (pObj->IsFollowMsgId(pPacket))
                pObj->ProcessPacket(pPacket);
        }

        pObj->Update();

        // 非激活状态，删除
        if (!pObj->IsActive())
        {
            _objlist.GetRemoveCache()->emplace_back(pObj);
        }
    }

    pMsgList->clear();
    
}

void ThreadObjectList::AddPacketToList(Packet* pPacket)
{
    std::lock_guard<std::mutex> guard(_packet_lock);
    _cachePackets.GetWriterCache()->emplace_back(pPacket);
}

void ThreadObjectList::Dispose()
{
    _objlist.Dispose();
    _cachePackets.Dispose();
}

Thread::Thread()
{
    _state = ThreadState_Init;
}

void Thread::Start()
{
    _thread = std::thread([this]()
        {
            _state = ThreadState_Run;
            while (!Global::GetInstance()->IsStop)
            {
                Update();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

            const auto theadId = _thread.get_id();
            std::cout << "close thread [1/2]. thread sn:" << this->GetSN() << " thread id:" << theadId << std::endl;
            _state = ThreadState_Stoped;
        });
}

bool Thread::IsRun() const
{
    return _state == ThreadState_Run;
}

bool Thread::IsStop() const
{
    return _state == ThreadState_Stoped;
}

bool Thread::IsDispose()
{
    if (_thread.joinable())
    {
        const auto theadId = _thread.get_id();
        _thread.join();
        std::cout << "close thread [2/2]. thread sn:" << this->GetSN() << " thread id:" << theadId << std::endl;
        return true;
    }

    return false;
}

