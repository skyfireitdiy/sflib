
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_http_base_server.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#include "core/sf_eventloop.h"
#include "core/sf_timer.h"
#include "sf_http_request.h"
#include "sf_http_response.h"
#include "sf_http_server_config.h"
#include "sf_http_utils.h"
#include "sf_tcp_server.h"
#include "sf_websocket_utils.h"
#include "tools/sf_json.h"
#include <functional>
#include <mutex>
#include <tools/sf_cache.h>
#include <utility>
#include <vector>

namespace skyfire {

/**
 *  @brief  HTTP服务器基础框架
 */
class sf_http_base_server {
private:
    const sf_http_server_config config__;

    std::shared_ptr<sf_tcp_server> server__ = std::make_shared<sf_tcp_server>(true);

    sf_eventloop event_loop__;

    std::function<void(const sf_http_request&, sf_http_response&)>
        request_callback__;

    std::function<void(const sf_http_request&, sf_http_response&)>
        websocket_request_callback__;
    std::function<void(SOCKET, const std::string& url, const byte_array& data)>
        websocket_binary_data_callback__;
    std::function<void(SOCKET, const std::string& url, const std::string& data)>
        websocket_text_data_callback__;
    std::function<void(SOCKET, const std::string& url)>
        websocket_open_callback__;
    std::function<void(SOCKET, const std::string& url)>
        websocket_close_callback__;

    std::unordered_map<SOCKET, sf_request_context_t> request_context__;
    std::recursive_mutex mu_request_context__;
    std::unordered_map<SOCKET, sf_websocket_context_t> websocket_context__;
    std::recursive_mutex mu_websocket_context__;
    std::unordered_map<SOCKET, sf_multipart_data_context_t>
        multipart_data_context__;
    std::recursive_mutex mu_multipart_data_context__;

    std::shared_ptr<sf_cache> file_cache__;

    struct session_data_t {
        int timeout;
        sf_json data;
    };

    struct cahce_data_t {
        byte_array data;
        fs::file_time_type modify_time;
    };

    mutable std::recursive_mutex mu_session__;
    std::unordered_map<std::string, std::shared_ptr<session_data_t>>
        session_data__;
    sf_timer session_timer__;

    void flush_session__();

    void raw_data_coming__(SOCKET sock, const byte_array& data);

    template <typename T>
    bool analysis_websocket_pkg__(SOCKET sock, const T* header,
        int& resolve_pos, unsigned long long& len,
        byte_array& body, bool& fin, int& op_code);

    void websocket_data_coming__(SOCKET sock, const byte_array& data);

    void build_new_request__(SOCKET sock);

    void on_socket_closed__(SOCKET sock);

    byte_array append_multipart_data__(
        sf_multipart_data_context_t& multipart_data,
        const byte_array& data) const;

    void file_response__(SOCKET sock, sf_http_response& res) const;

    void normal_response__(SOCKET sock, sf_http_response& res) const;

    void multipart_response__(SOCKET sock, sf_http_response& res);

    static bool check_analysis_multipart_file__(
        std::vector<sf_http_response::multipart_info_t>& multipart_data);

    void close_request__(SOCKET sock);

    void http_handler__(SOCKET sock, const sf_http_request& http_request);

    void build_boundary_context_data(SOCKET sock,
        const sf_http_request& request);

    void build_websocket_context_data__(SOCKET sock,
        const sf_http_request& request);

    void send_response_file_part__(
        SOCKET sock, const sf_http_response::response_file_info_t& file,
        std::ifstream& fi) const;

public:
    /**
     * 获取session
     * @param session_key session key
     * @return session的json对象（注意是浅拷贝）
     */
    sf_json session(const std::string& session_key);
    /**
     * @brief 保持session活动（赋予session生命值）
     *
     * @param session_key seesion key
     */
    void keep_session_alive(const std::string& session_key);

    /**
     * 设置session
     * @tparam T session类型
     * @param session_key session key
     * @param key 键
     * @param value 值
     */
    template <typename T>
    void set_session(const std::string& session_key, const std::string& key,
        const T& value);

    /**
     * 构造函数
     * @param config http配置
     */
    explicit sf_http_base_server(const sf_http_server_config& config);

    /**
     * 设置请求回调函数
     * @param request_callback 请求回调函数，函数接收参数为const
     * sf_http_request&类型参数req和sf_http_response&参数res，处理者根据req来设置res的状态，框架会解析res的状态来返回至客户端
     */
    void set_request_callback(
        std::function<void(const sf_http_request&, sf_http_response&)>
            request_callback);

    /**
     * 设置websocket回调函数
     * @param websocket_request_callback websocket回调函数，函数接收参数为const
     * sf_http_request&类型参数req和sf_http_response&参数res，处理者根据req来设置res的状态，框架会解析res的状态来返回至客户端（websocket握手在此函数中实现）
     */
    void set_websocket_request_callback(
        std::function<void(const sf_http_request&, sf_http_response&)>
            websocket_request_callback);

    /**
     * 设置websocket二进制数据的响应
     * @param websocket_binary_data_callback
     * websocket二进制数据回调函数，函数接收参数为SOCKET（websocket连接，可以在send_websocket_data中使用此标识发送数据）、url（WebSocket地址）、byte_array(接收到的二进制数据)
     */
    void set_websocket_binary_data_callback(
        std::function<void(SOCKET, const std::string& url, const byte_array&)>
            websocket_binary_data_callback);
    /**
     * 设置websocket文本数据的响应
     * @param websocket_text_data_callback
     * websocket文本数据回调函数，函数接收参数为SOCKET（websocket连接，可以在send_websocket_data中使用此标识发送数据）、url（WebSocket地址）、string(接收到的文本数据)
     */
    void set_websocket_text_data_callback(
        std::function<void(SOCKET, const std::string& url, const std::string&)>
            websocket_text_data_callback);

    /**
     * 设置websocket打开响应
     * @param websocket_open_callback
     * websocket打开回调函数，函数接收参数为SOCKET（websocket连接，可以在send_websocket_data中使用此标识发送数据）和url（WebSocket地址）
     */
    void set_websocket_open_callback(
        std::function<void(SOCKET, const std::string& url)>
            websocket_open_callback);

    /**
     * 设置websocket关闭响应
     * @param websocket_close_callback
     * websocket打开回调函数，函数接收参数为SOCKET（websocket连接）和url（WebSocket地址）
     */
    void set_websocket_close_callback(
        std::function<void(SOCKET, const std::string& url)>
            websocket_close_callback);

    /**
     * 退出服务器
     */
    void quit();

    /**
     * 启动服务器，此函数会包含一个事件循环，因此在此处会阻塞，如果有其他逻辑需要处理，可以放置在后台线程
     * @return 启动是否成功
     */
    bool start();

    /**
     * 给特定的websocket发送消息
     * @tparam T
     * 消息类型，仅支持byte_array和string，其中byte_array用于二进制数据发送，string用于文本数据发送
     * @param sock socket标识
     * @param data 数据（二进制或者文本数据）
     * @return 是否发送成功
     */
    template <typename T>
    bool send_websocket_data(SOCKET sock, const T& data);

    /**
     * 给所有的websocket发送消息（广播）,注意：此函数没有返回值，无法判断是否发送成功
     * @tparam T
     * 消息类型，仅支持byte_array和string，其中byte_array用于二进制数据发送，string用于文本数据发送
     * @param data 数据（二进制或者文本数据）
     */
    template <typename T>
    void send_websocket_data(const T& data);

    /**
     * 关闭特定的websocket
     * @param sock websocket标识
     */
    void close_websocket(SOCKET sock);
};
} // namespace skyfire
#pragma clang diagnostic pop
