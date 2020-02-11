#include "thread.h"
#include "global.h"
#include "packet.h"

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
        _objlist.push_back(obj);
    }
}

void ThreadObjectList::Update()
{
    std::list<ThreadObject*> _tmpObjs;
    _obj_lock.lock();
    std::copy(_objlist.begin(), _objlist.end(), std::back_inserter(_tmpObjs));
    _obj_lock.unlock();

    for (ThreadObject* pTObj : _tmpObjs)
    {
        pTObj->ProcessPacket();
        pTObj->Update();

        // 非激活状态，删除
        if (!pTObj->IsActive())
        {
            _obj_lock.lock();
            _objlist.remove(pTObj);
            _obj_lock.unlock();

            pTObj->Dispose();
            delete pTObj;
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void ThreadObjectList::AddPacketToList(Packet* pPacket)
{
    std::lock_guard<std::mutex> guard(_obj_lock);
    for (auto iter = _objlist.begin(); iter != _objlist.end(); ++iter)
    {
        ThreadObject* pObj = *iter;
        if (pObj->IsFollowMsgId(pPacket->GetMsgId()))
        {
            pObj->AddPacket(pPacket);
        }
    }
}

Thread::Thread()
{
    this->_isRun = true;
}

void Thread::Stop()
{
    if (!_isRun)
    {
        _isRun = false;
        if (_thread.joinable()) _thread.join();
    }
}

void Thread::Start()
{
    _isRun = true;
    _thread = std::thread([this]()
    {
        while (_isRun)
        {
            Update();
        }
    });
}

bool Thread::IsRun() const
{
    return _isRun;
}

void Thread::Dispose()
{
    std::list<ThreadObject*>::iterator iter = _objlist.begin();
    while (iter != _objlist.end())
    {
        (*iter)->Dispose();
        delete (*iter);
        iter = _objlist.erase(iter);
    }

    _objlist.clear();
}

