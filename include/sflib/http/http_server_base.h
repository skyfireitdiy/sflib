
#pragma once
#include "sflib/cache/cache.h"
#include "sflib/http/http_middleware.h"
#include "sflib/http/http_server_config.h"
#include "sflib/http/http_server_request.h"
#include "sflib/http/http_server_response.h"
#include "sflib/http/http_utils.h"
#include "sflib/http/websocket_utils.h"
#include "sflib/json/json.h"
#include "sflib/object/eventloop.h"
#include "sflib/object/timer.h"
#include "sflib/tcp/tcp_server.h"
#include <functional>
#include <mutex>
#include <utility>
#include <vector>

namespace skyfire
{
class http_server_base
{
private:
    const http_server_config config__;
    std::shared_ptr<cache> file_cache__;
    std::shared_ptr<tcp_server> server__;
    eventloop event_loop__;
    std::function<void(const http_server_request &, http_server_response &)>
        request_callback__;
    std::function<void(const http_server_request &, http_server_response &)>
        websocket_request_callback__;
    std::function<void(SOCKET, const std::string &url, const byte_array &data)>
        websocket_binary_data_callback__;
    std::function<void(SOCKET, const std::string &url, const std::string &data)>
        websocket_text_data_callback__;
    std::function<void(SOCKET, const std::string &url)>
        websocket_open_callback__;
    std::function<void(SOCKET, const std::string &url)>
        websocket_close_callback__;
    std::unordered_map<SOCKET, request_context_t> request_context__;
    std::recursive_mutex mu_request_context__;
    std::unordered_map<SOCKET, websocket_context_t> websocket_context__;
    std::recursive_mutex mu_websocket_context__;
    std::unordered_map<SOCKET, http_server_req_multipart_data_context_t>
        multipart_data_context__;
    std::recursive_mutex mu_multipart_data_context__;
    std::vector<std::shared_ptr<http_middleware>> middleware__;
    struct session_data_t
    {
        size_t timeout;
        json data;
    };
    struct file_cache_data_t
    {
        byte_array data;
        std::filesystem::file_time_type modify_time;
    };
    struct file_etag_t
    {
        std::string etag;
        std::string last_modify;
    };
    mutable std::recursive_mutex mu_session__;
    std::unordered_map<std::string, std::shared_ptr<session_data_t>>
        session_data__;
    timer session_timer__;
    void flush_session__();
    void raw_data_coming__(SOCKET sock, const byte_array &data);
    template <typename T>
    bool analysis_websocket_pkg__(SOCKET sock, const T *header,
                                  int &resolve_pos, unsigned long long &len,
                                  byte_array &body, bool &fin, int &op_code);
    void websocket_data_coming__(SOCKET sock, const byte_array &data);
    void build_new_request__(SOCKET sock);
    void on_socket_closed__(SOCKET sock);
    byte_array append_multipart_data__(
        http_server_req_multipart_data_context_t &multipart_data,
        const byte_array &data) const;
    void file_response__(SOCKET sock, http_server_response &res) const;
    void normal_response__(SOCKET sock, http_server_response &res) const;
    void multipart_response__(SOCKET sock, http_server_response &res);
    static bool check_analysis_multipart_file__(
        std::vector<http_multipart_info_t> &multipart_data);
    void close_request__(SOCKET sock);
    void http_handler__(SOCKET sock, const http_server_request &http_server_request);
    void build_boundary_context_data(SOCKET sock,
                                     const http_server_request &request);
    void build_websocket_context_data__(SOCKET sock,
                                        const http_server_request &request);
    void send_response_file_part__(
        SOCKET sock, const http_file_info_t &file,
        std::ifstream &fi) const;
    void set_file_etag__(http_server_response &res, const file_etag_t &etag) const;
    file_etag_t make_etag__(const http_file_info_t &file) const;

public:
    json
    session(const std::string &session_key);
    void keep_session_alive(const std::string &session_key);
    template <typename T>
    void set_session(const std::string &session_key, const std::string &key,
                     const T &value);
    explicit http_server_base(const http_server_config &config);
    void set_request_callback(
        std::function<void(const http_server_request &, http_server_response &)>
            request_callback);
    void set_websocket_request_callback(
        std::function<void(const http_server_request &, http_server_response &)>
            websocket_request_callback);
    void set_websocket_binary_data_callback(
        std::function<void(SOCKET, const std::string &url, const byte_array &)>
            websocket_binary_data_callback);
    void set_websocket_text_data_callback(
        std::function<void(SOCKET, const std::string &url, const std::string &)>
            websocket_text_data_callback);
    void set_websocket_open_callback(
        std::function<void(SOCKET, const std::string &url)>
            websocket_open_callback);
    void set_websocket_close_callback(
        std::function<void(SOCKET, const std::string &url)>
            websocket_close_callback);
    void quit();
    bool start();
    template <typename T>
    bool send_websocket_data(SOCKET sock, const T &data);
    template <typename T>
    void send_websocket_data(const T &data);
    void close_websocket(SOCKET sock);
    void add_middleware(std::shared_ptr<http_middleware> m);
};

template <typename T>
bool http_server_base::analysis_websocket_pkg__(SOCKET sock, const T *header,
                                                int &resolve_pos,
                                                unsigned long long &len,
                                                byte_array &body, bool &fin,
                                                int &op_code)
{
    std::lock_guard lck(mu_websocket_context__);
    len = get_size(*header);
    if (websocket_context__[sock].buffer.size() - resolve_pos - sizeof(T) < len)
    {
        return false;
    }

    body = byte_array(
        websocket_context__[sock].buffer.begin() + resolve_pos + sizeof(T),
        websocket_context__[sock].buffer.begin() + resolve_pos + sizeof(T) + static_cast<int>(len));
    if (with_mask(*header))
    {
        decode_websocket_pkg(body, header->mask_key);
    }
    fin = is_fin(*header);
    op_code = get_op_code(*header);
    resolve_pos += sizeof(T) + static_cast<int>(len);
    return true;
}

template <typename T>
bool http_server_base::send_websocket_data(SOCKET sock, const T &data)
{
    if constexpr (std::is_same_v<T, std::string>)
    {
    }
    return server__->send(sock, make_server_websocket_data_pkg(data));
}
template <typename T>
void http_server_base::send_websocket_data(const T &data)
{
    if constexpr (std::is_same_v<T, std::string>)
    {
    }
    for (auto &p : websocket_context__)
    {
        server__->send(p.first, make_server_websocket_data_pkg(data));
    }
}

template <typename T>
void http_server_base::set_session(const std::string &session_key,
                                   const std::string &key, const T &value)
{
    std::lock_guard lck(mu_session__);
    keep_session_alive(session_key);
    session_data__[session_key]->data[key] = json(value);
}

} // namespace skyfire