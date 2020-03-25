#pragma once
#include "libserver/common.h"
#include "libserver/component.h"
#include "libserver/system.h"
#include <curl/curl.h>
#include <json/json.h>  

enum HttpResquestState 
{
    HRS_Send,      // 发送数据
    HRS_Process,   // 等待数据
    HRS_Over,      // 完成
    HRS_NoActive,	  // 完成后的非激活状态，等待线程删除
    HRS_Timeout,	  // 请求超时
};

enum HttpResquestMethod
{
    HRM_Post,
    HRM_Get,
};


enum CURLMRS
{
    CRS_None,
    CRS_OK,
    CRS_CURLM_CALL_MULTI_PERFORM, // call curl_multi_perform
    CRS_TIMEOUT,
    CRS_CURLMError, // curlm 调用出错
    CRS_SelectError,
};

class HttpRequest :public Component<HttpRequest>, public IUpdateSystem
{
public:
    void Update() override;
    void BackToPool() override;

protected:
    bool ProcessSend();
    void ProcessTimeout() const;
    virtual bool ProcessOver();

    virtual bool Process();
    virtual void ProcessMsg();
    virtual void ProcessMsg(Json::Value value) = 0;

protected:
    HttpResquestMethod _method{ HRM_Get };
    CURL* _pCurl{ nullptr };
    CURLM *_pMultiHandle{ nullptr };
    CURLMRS _curlRs;

    std::string _url, _params;
    std::string _responseBuffer;
    HttpResquestState State{ HRS_Send };

    std::string _account;
};

/**
* 使用select函数监听multi curl文件描述符的状态
* 监听成功返回0，监听失败返回-1
*/
inline CURLMRS curl_multi_select(CURLM * curl_m)
{
    struct timeval timeout_tv;
    fd_set  fd_read;
    fd_set  fd_write;
    fd_set  fd_except;
    int     max_fd = -1;

    // 注意这里一定要清空fdset,curl_multi_fdset不会执行fdset的清空操作  //
    FD_ZERO(&fd_read);
    FD_ZERO(&fd_write);
    FD_ZERO(&fd_except);

    // 设置select超时时间
    timeout_tv.tv_sec = 1;
    timeout_tv.tv_usec = 0;

    // 获取multi curl需要监听的文件描述符集合 fd_set //
    curl_multi_fdset(curl_m, &fd_read, &fd_write, &fd_except, &max_fd);

    /**
    * When max_fd returns with -1,
    * you need to wait a while and then proceed and call curl_multi_perform anyway.
    * How long to wait? I would suggest 100 milliseconds at least,
    * but you may want to test it out in your own particular conditions to find a suitable value.
    */
    if (-1 == max_fd)
    {
        return CURLMRS::CRS_CURLM_CALL_MULTI_PERFORM;
    }

    /**
    * 执行监听，当文件描述符状态发生改变的时候返回
    * 返回0，程序调用curl_multi_perform通知curl执行相应操作
    * 返回-1，表示select错误
    * 注意：即使select超时也需要返回0，具体可以去官网看文档说明
    */
    int ret_code = ::select(max_fd + 1, &fd_read, &fd_write, &fd_except, &timeout_tv);
    switch (ret_code)
    {
    case -1:
        /* select error */
        return CRS_SelectError;
    case 0:
        /* select timeout */
        return CRS_TIMEOUT;
    default:
        /* one or more of curl's file descriptors say there's data to read or write*/
        return CRS_OK;
    }
}