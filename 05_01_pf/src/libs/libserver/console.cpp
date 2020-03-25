#include "console.h"

#include <iostream>
#include <thread>
#include "thread.h"
#include "util_string.h"

void ConsoleCmd::OnRegisterHandler(std::string key, HandleConsole handler)
{
    _handles[key] = handler;
}

void ConsoleCmd::Dispose()
{
    _handles.clear();
}

void ConsoleCmd::Process(std::vector<std::string> params)
{
    if (params.size() <= 1)
        return;

    const std::string key = params[1];
    const auto iter = _handles.find(key);

    if (iter == _handles.end())
    {
        std::cout << "input error. can't find cmd:" << key.c_str() << std::endl;
        return;
    }

    switch (params.size())
    {
    case 2:
        iter->second("", "");
        break;
    case 3:
        iter->second(params[2], "");
        break;
    case 4:
        iter->second(params[2], params[3]);
        break;
    default:
        std::cout << "input error. -help for help." << std::endl;
        break;;
    }
}

bool Console::Init()
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
        } while (true);
    });

    return true;
}

void Console::RegisterMsgFunction()
{
}

void Console::Update()
{
    _lock.lock();
    if (_commands.size() <= 0)
    {
        _lock.unlock();
        return;
    }

    const std::string cmd = _commands.front();
    _commands.pop();
    _lock.unlock();

    std::vector<std::string> params;
    strutil::split(cmd, ' ', params);
    if (params.size() <= 0)
        return;

    const std::string key = params[0];
    const auto iter = _handles.find(key);

    if (iter == _handles.end())
    {
        std::cout << "input error. cmd:" << cmd.c_str() << std::endl;
        return;
    }

    iter->second->Process(params);
}

void Console::Dispose()
{
    for (auto &pair : _handles)
    {
        pair.second->Dispose();
    }

    _handles.clear();

    ThreadObject::Dispose();
}
