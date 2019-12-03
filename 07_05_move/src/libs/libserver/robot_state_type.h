#pragma once

enum class RobotStateType
{
    None = -1,

    Http_Connecting = 0,   // http请求
    Http_Connected,		// 

    Login_Connecting,	    // 正在链接 Login
    Login_Connected,	    // 链接成功
    Login_Logined,          // 登录成功

    //Login_CreatePlayer,   // 创建角色
    Login_SelectPlayer,      // 选择角色

    Game_Connecting,	    // 正在链接 Game
    Game_Connected,	    // 链接成功
    Game_Logined,           // 登录成功

    Space_EnterWorld,       // 进入地图

    End,
};

const char RobotStateTypeName[][255] = {
    "[Http ] Connecting  :",
    "[Http ] Connected   :",

    "[Login] Connecting  :",
    "[Login] Connected   :",
    "[Login] Logined     :",

    //"[Login] CreatePlayer:",
    "[Login] SelectPlayer:",

    "[Game ] Connecting  :",
    "[Game ] Connected   :",
    "[Game ] Logined     :",

    "[Space] EnterWorld  :",
};

const char RobotStateTypeShortName[][255] = {
    "Http-Connecting",
    "Http-Connected",

    "Login-Connecting",
    "Login-Connected",
    "Login-Logined",
    //"Login-CreatePlayer",
    "Login-SelectPlayer",

    "Game-Connecting",
    "Game-Connected",
    "Game-Logined",

    "Space-EnterWorld",
};

inline const char* GetRobotStateTypeShortName(RobotStateType stateType)
{
    if (stateType <= RobotStateType::None || stateType >= RobotStateType::End)
    {
        return "unknown";
    }

    return RobotStateTypeShortName[(int)stateType];
}


inline const char* GetRobotStateTypeName(RobotStateType stateType)
{
    if (stateType <= RobotStateType::None || stateType >= RobotStateType::End)
    {
        return "unknown";
    }

    return RobotStateTypeName[(int)stateType];
}