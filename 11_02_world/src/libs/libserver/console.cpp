#include "console.h"

#include <iostream>
#include <thread>

#include "util_string.h"
#include "log4_help.h"
#include "update_component.h"
#include "component_help.h"
#include "global.h"

void ConsoleCmd::OnRegisterHandler(std::string key, HandleConsole handler)
{
    _handles[key] = handler;
}

bool ConsoleCmd::CheckParamCnt(std::vector<std::string>& params, const size_t count)
{
    if (params.size() == count)
        return true;

    std::cout << "input param size is error. see: -help" << std::endl;
    return false;
}

void ConsoleCmd::Dispose()
{
    _handles.clear();
}

void ConsoleCmd::Process(std::vector<std::string>& params)
{
    if (params.size() <= 1)
        return;

    const std::string key = params[1];
    const auto iter = _handles.find(key);

    if (iter == _handles.end())
    {
        if (key == "-help")
        {
            HandleHelp();
            return;
        }

        LOG_ERROR("input error. can't find cmd:" << key.c_str());
        return;
    }

    params.erase(params.begin(), params.begin() + 2);
    iter->second(params);
}

///////////////////////////////////////////////////////////////////

void Console::Awake()
{
    _thread = std::thread([this]()
        {
            char _buffer[ConsoleMaxBuffer];
            do
            {
                std::cin.getline(_buffer, ConsoleMaxBuffer);

                _lock.lock();
                _commands.push(std::string(_buffer));
                _lock.unlock();

                std::string cmdMsg = _buffer;
                auto iter = cmdMsg.find("-exit");
                if (iter != std::string::npos)
                {
                    Global::GetInstance()->IsStop = true;
                    _isRun = false;
                }

            } while (_isRun);
        });

    AddComponent<UpdateComponent>(BindFunP0(this, &Console::Update));
}

void Console::BackToPool()
{
    for (auto& pair : _handles)
    {
        pair.second->Dispose();
        delete pair.second;
    }
    _handles.clear();

    if (_isRun)
        _thread.detach();
    else
        _thread.join();
}

void Console::Update()
{
    _lock.lock();
    if (_commands.empty())
    {
        _lock.unlock();
        return;
    }

    const std::string cmd = _commands.front();
    _commands.pop();
    _lock.unlock();

    std::vector<std::string> params;
    strutil::split(cmd, ' ', params);
    if (params.empty())
        return;

    const std::string key = params[0];
    auto iter = _handles.find(key);
    if (iter == _handles.end())
    {
        if (key == "help")
        {
            iter = _handles.begin();
            while (iter != _handles.end())
            {
                std::cout << "\r\n" << iter->first.c_str() << std::endl;
                iter->second->HandleHelp();
                ++iter;
            }
            return;
        }
        LOG_ERROR("input error. cmd:" << cmd.c_str());
        return;
    }

    iter->second->Process(params);
}

