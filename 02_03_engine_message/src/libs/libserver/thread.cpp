#include "thread.h"
#include "global.h"
#include "packet.h"

#include <iterator>

Thread::Thread()
{
    this->_isRun = true;
    _tmpObjs.clear();
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

void Thread::Update()
{

    _thread_lock.lock();
    std::copy(_objlist.begin(), _objlist.end(), std::back_inserter(_tmpObjs));
    _thread_lock.unlock();

    for (ThreadObject* pTObj : _tmpObjs)
    {
        pTObj->ProcessPacket();
        pTObj->Update();

        // ·Ç¼¤»î×´Ì¬£¬É¾³ý
        if (!pTObj->IsActive())
        {
            _thread_lock.lock();
            _objlist.remove(pTObj);
            _thread_lock.unlock();

            pTObj->Dispose();
            delete pTObj;
        }
    }

    _tmpObjs.clear();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void Thread::AddThreadObj(ThreadObject* obj)
{
    obj->RegisterMsgFunction();

    std::lock_guard<std::mutex> guard(_thread_lock);
    _objlist.push_back(obj);
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

void Thread::AddPacket(Packet* pPacket)
{
    std::lock_guard<std::mutex> guard(_thread_lock);
    for (auto iter = _objlist.begin(); iter != _objlist.end(); ++iter)
    {
        ThreadObject* pObj = *iter;
        if (pObj->IsFollowMsgId(pPacket->GetMsgId()))
        {
            pObj->AddPacket(pPacket);
        }
    }
}
