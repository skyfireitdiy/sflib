
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_http_base_server.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#include "sf_http_base_server.h"

#include "core/sf_eventloop.hpp"
#include "core/sf_object.hpp"
#include "core/sf_timer.hpp"
#include "sf_http_content_type.h"
#include "sf_http_server_config.h"
#include "sf_http_utils.hpp"
#include "sf_tcp_server.h"
#include "sf_websocket_utils.hpp"
#include "tools/sf_cache.hpp"
#include "tools/sf_json.hpp"
#include "tools/sf_logger.hpp"
#include "tools/sf_random.hpp"
#include <memory>
#include <utility>

using namespace std::string_literals;

namespace skyfire {

inline void sf_http_base_server::http_handler__(
    const SOCKET sock, const sf_http_request& http_request)
{
    bool keep_alive = true;
    sf_http_response res;
    res.set_status(200);
    res.set_http_version(http_request.request_line().http_version);

    sf_http_cookie_t session_cookie;
    session_cookie.key = sf_session_id_key;
    auto session_id = http_request.session_id();
    session_cookie.value = session_id;
    keep_session_alive(session_id);
    res.add_cookie(session_cookie);

    request_callback__(http_request, res);

    if (sf_equal_nocase_string(
            http_request.header().header_value("Connection", "Close"),
            "Close")) {
        keep_alive = false;
    }
    auto cookies = res.cookies();
    for (auto& p : cookies) {
        std::string value_str = p.second.key + "="s + p.second.value + ";"s;
        if (p.second.life_type == cookie_life_type::time_point) {
            value_str += "Expires=" + sf_make_http_time_str(p.second.time_point) + ";"s;
        }
        if (p.second.path.empty()) {
            p.second.path = "/";
        }
        value_str += "Path=" + p.second.path + ";";
        if (p.second.secure) {
            value_str += "Secure;";
        }
        if (p.second.http_only) {
            value_str += "HttpOnly;";
        }
        res.header().cookie_str_vec__.push_back(value_str);
    }

    res.header().set_header("Server", "SkyFire HTTP Server");
    res.header().set_header("Connection", keep_alive ? "Keep-Alive" : "Close");

    if (res.type() == sf_http_response::response_type::file) {
        file_response__(sock, res);
    } else if (res.type() == sf_http_response::response_type::multipart) {
        multipart_response__(sock, res);
    } else if (res.type() == sf_http_response::response_type::normal) {
        normal_response__(sock, res);
    }
    if (!keep_alive) {
        close_request__(sock);
    }

    auto addr = http_request.addr();
    sf_info(res.status__, addr.ip, addr.port,
        http_request.request_line().method,
        http_request.request_line().http_version,
        http_request.request_line().url);
}

inline void sf_http_base_server::raw_data_coming__(SOCKET sock,
    const byte_array& data)
{
    sf_debug("Socket", sock, "Data size", data.size());
    // 过滤websocket消息
    {
        std::lock_guard<std::recursive_mutex> lck(mu_websocket_context__);
        if (websocket_context__.count(sock) != 0) {
            sf_debug("websocket connection");
            websocket_data_coming__(sock, data);
            return;
        }
    }
    // 普通请求
    {
        std::lock_guard<std::recursive_mutex> lck(mu_request_context__);
        // sf_debug("Request",to_string(data));
        if (request_context__.count(sock) == 0) {
            sf_debug("http connection");
            request_context__[sock] = sf_request_context_t();
        }
        request_context__[sock].buffer += data;
        if (request_context__[sock].buffer.empty()) {
            return;
        }
    }
    // multipart消息
    {
        std::lock_guard<std::recursive_mutex> lck(mu_multipart_data_context__);
        if (multipart_data_context__.count(sock) != 0) {
            std::lock_guard<std::recursive_mutex> lck2(mu_request_context__);
            sf_debug("boundary data append",
                request_context__[sock].buffer.size());

            request_context__[sock].buffer = append_multipart_data__(
                multipart_data_context__[sock], request_context__[sock].buffer);

            if (multipart_data_context__[sock].multipart.back().is_end()) {
                sf_debug("boundary data finished");
                http_handler__(sock, sf_http_request(multipart_data_context__[sock], sock));
                multipart_data_context__.erase(sock);
            }
            request_context__[sock].buffer.clear();
            return;
        }
    }

    std::unique_lock<std::recursive_mutex> lck(mu_request_context__);
    sf_http_request request(request_context__[sock].buffer, sock);
    if (request.is_valid()) {
        request_context__[sock].new_req = true;
        request_context__[sock].buffer.clear();
        lck.unlock();

        auto req_header = request.header();

        if (request.is_multipart_data()) {
            sf_debug("build_boundary_context_data");
            build_boundary_context_data(sock, request);
            return;
        }

        const auto connection_header = req_header.header_value("Connection");

        auto connection_header_list = sf_split_string(connection_header, ",");

        // 过滤Connection为Upgrade的请求
        if (std::find_if(connection_header_list.begin(),
                connection_header_list.end(),
                [](const std::string& str) {
                    return sf_equal_nocase_string(sf_string_trim(str),
                        "Upgrade");
                })
            != connection_header_list.end()) {
            // 筛选Websocket请求
            if (sf_equal_nocase_string(req_header.header_value("Upgrade"),
                    "websocket")) {
                sf_debug("new websocket request");
                build_websocket_context_data__(sock, request);
            }
            return;
        }

        sf_debug("new http request");
        if (request_callback__) {
            http_handler__(sock, request);
        }
    } else if (request.is_error()) {
        sf_debug("invalid request", to_string(request_context__[sock].buffer));
        server__->close(sock);
    }
}

inline void sf_http_base_server::build_websocket_context_data__(
    SOCKET sock, const sf_http_request& request)
{
    // NOTE 删除记录，防止超时检测线程关闭连接
    std::unique_lock<std::recursive_mutex> lck(mu_request_context__);
    request_context__.erase(sock);
    lck.unlock();
    sf_websocket_context_t ws_data;
    ws_data.url = request.request_line().url;

    if (websocket_request_callback__) {
        sf_http_response res;
        websocket_request_callback__(request, res);
        res.header().set_header("Content-Length", std::to_string(res.length()));
        sf_debug("Response", to_string(res.to_package()));
        server__->send(sock, res.to_package());
        std::lock_guard<std::recursive_mutex> lck2(mu_websocket_context__);
        websocket_context__.insert({ sock, ws_data });
        websocket_context__[sock].sock = sock;
        websocket_context__[sock].buffer = byte_array();
        if (websocket_open_callback__) {
            websocket_open_callback__(sock, ws_data.url);
        }
        sf_debug("websocket add to context", sock);
    }
}

inline void sf_http_base_server::build_boundary_context_data(
    SOCKET sock, const sf_http_request& request)
{
    sf_debug("is boundary data");
    // 初始化boundary数据
    auto multipart_data = request.multipart_data_context();
    std::unique_lock<std::recursive_mutex> lck(mu_request_context__);
    request_context__[sock].buffer = append_multipart_data__(multipart_data, request.body());
    lck.unlock();
    if (!multipart_data.multipart.empty() && multipart_data.multipart.back().is_end()) {
        sf_debug("boundary data success one time");
        http_handler__(sock, sf_http_request(multipart_data, sock));
    } else {
        sf_debug("boundary data prepare");
        std::lock_guard<std::recursive_mutex> lck2(mu_multipart_data_context__);
        multipart_data_context__[sock] = multipart_data;
    }
}

inline void sf_http_base_server::close_request__(SOCKET sock)
{
    {
        std::lock_guard<std::recursive_mutex> lck(mu_request_context__);
        request_context__.erase(sock);
    }
    {
        std::lock_guard<std::recursive_mutex> lck(mu_multipart_data_context__);
        multipart_data_context__.erase(sock);
    }
}

inline void sf_http_base_server::normal_response__(
    SOCKET sock, sf_http_response& res) const
{
    res.header().set_header("Content-Length", std::to_string(res.length()));
    server__->send(sock, res.to_package());
    sf_debug("http body length", res.length());
}

inline void sf_http_base_server::multipart_response__(SOCKET sock,
    sf_http_response& res)
{
    auto multipart = res.multipart();
    if (!check_analysis_multipart_file__(multipart)) {
        res.set_status(403);
        res.set_body(to_byte_array("<p>Forbidden</p>"));
        res.header().set_header("Content-Type", "text/html");
        normal_response__(sock, res);
        return;
    }

    auto boundary_str = sf_random::instance()->uuid_str();
    unsigned long long content_length = 0;
    std::vector<std::string> header_vec;
    std::string end_str = "--" + boundary_str + "--";
    for (auto& p : multipart) {
        if (p.type == sf_http_response::multipart_info_t::multipart_info_type::file) {
            auto tmp_str = "--" + boundary_str + "\r\n";
            auto suffix = sf_to_lower_string(sf_get_path_ext(p.file_info.filename));
            if (sf_http_content_type.count(suffix) != 0) {
                tmp_str += "Content-Type:" + sf_http_content_type[suffix] + "\r\n";
            } else {
                tmp_str += "Content-Type: application/octet-stream\r\n";
            }

            static std::unordered_map<std::string, long long> file_size_cache;

            long long file_size = fs::file_size(p.file_info.filename);
            tmp_str += "Content-Range: bytes " + std::to_string(p.file_info.begin) + "-" + std::to_string(p.file_info.end) + "/" + std::to_string(file_size) + "\r\n\r\n";
            header_vec.push_back(tmp_str);
            content_length += tmp_str.length() + (p.file_info.end - p.file_info.begin) + 2; // 添加\r\n
        } else if (p.type == sf_http_response::multipart_info_t::multipart_info_type::form) {
            auto tmp_str = "--" + boundary_str + "\r\n";
            sf_http_header tmp_header;
            tmp_header.set_header(p.form_info.header);
            tmp_str += tmp_header.to_string();
            header_vec.push_back(tmp_str);
            content_length += tmp_str.length() + p.form_info.body.size() + 2;
        }
    }

    content_length += end_str.length();

    auto& res_header = res.header();
    res_header.set_header("Content-Type",
        "multipart/byteranges; boundary=" + boundary_str);
    res_header.set_header("Content-Length", std::to_string(content_length));
    res.set_status(206);

    // 发送header
    server__->send(sock, res.to_header_package());
    for (auto i = 0UL; multipart.size() > i; ++i) {
        server__->send(sock, to_byte_array(header_vec[i]));
        if (multipart[i].type == sf_http_response::multipart_info_t::multipart_info_type::file) {
            std::ifstream fi(multipart[i].file_info.filename,
                std::ios::in | std::ios::binary);
            if (!fi) {
                close_request__(sock);
                return;
            }
            send_response_file_part__(sock, multipart[i].file_info, fi);
            fi.close();
            server__->send(sock, to_byte_array("\r\n"s));
        } else if (multipart[i].type == sf_http_response::multipart_info_t::multipart_info_type::form) {
            server__->send(sock, to_byte_array(header_vec[i]));
            server__->send(sock, multipart[i].form_info.body);
            server__->send(sock, to_byte_array("\r\n"s));
        }
    }
    server__->send(sock, to_byte_array(end_str));
}

inline bool sf_http_base_server::check_analysis_multipart_file__(
    std::vector<sf_http_response::multipart_info_t>& multipart_data)
{
    for (auto& p : multipart_data) {
        if (p.type == sf_http_response::multipart_info_t::multipart_info_type::file) {
            const auto file_size = fs::file_size(p.file_info.filename);
            if (file_size == -1)
                return false;
            if (p.file_info.begin >= file_size)
                return false;
            if (p.file_info.end == -1) {
                p.file_info.end = file_size;
            }
            if (p.file_info.end < p.file_info.begin)
                return false;
        }
    }
    return true;
}

inline void sf_http_base_server::file_response__(SOCKET sock,
    sf_http_response& res) const
{
    auto file = res.file();
    auto file_size = fs::file_size(file.filename);
    sf_debug("file:", file.filename, file.begin, file.end);
    if (file.begin != 0 || (file.end != file_size && file.end != -1)) {
        auto& header = res.header();
        if (file.end == -1) {
            file.end = file_size;
        }
        if (file_size != 0 && (file.begin > file_size || file.end > file_size)) {
            res.set_status(416);
            res.set_body(to_byte_array("Requested Range Not Satisfiable"s));
            normal_response__(sock, res);
            return;
        }

        header.set_header("Content-Range", "bytes " + std::to_string(file.begin) + "-" + std::to_string(file.end) + "/" + std::to_string(file_size));
        header.set_header("Content-Length",
            std::to_string(file.end - file.begin));
        auto suffix = sf_to_lower_string(sf_get_path_ext(file.filename));
        if (sf_http_content_type.count(suffix) != 0) {
            header.set_header("Content-Type", sf_http_content_type[suffix]);
        } else {
            header.set_header("Content-Type", "application/octet-stream");
        }

        std::ifstream fi(file.filename,
            std::ios_base::binary | std::ios_base::in);
        if (!fi) {
            sf_debug("file not found");
            res.set_status(404);
            normal_response__(sock, res);
            return;
        }
        res.set_status(206);
        server__->send(sock, res.to_header_package());
        send_response_file_part__(sock, file, fi);
        fi.close();
    } else {
        file.end = file_size;
        auto& header = res.header();
        header.set_header("Content-Length", std::to_string(file_size));
        auto suffix = sf_to_lower_string(sf_get_path_ext(file.filename));
        if (sf_http_content_type.count(suffix) != 0) {
            header.set_header("Content-Type", sf_http_content_type[suffix]);
        } else {
            header.set_header("Content-Type", "application/octet-stream");
        }

        if (file_size > config__.max_cache_file_size || config__.debug) {
            std::ifstream fi(file.filename,
                std::ios_base::binary | std::ios_base::in);
            if (!fi) {
                sf_debug("file not found");
                res.set_status(404);
                normal_response__(sock, res);
                return;
            }

            server__->send(sock, res.to_header_package());
            send_response_file_part__(sock, file, fi);
            fi.close();
        } else {
            std::error_code err;
            auto modify_time = fs::last_write_time(file.filename, err);
            if (err) {
                sf_debug("get modify time error");
                res.set_status(404);
                normal_response__(sock, res);
                return;
            }
            auto data = file_cache__->data<cahce_data_t>(file.filename);
            if (data && data->modify_time == modify_time) {
                res.set_body(data->data);
                normal_response__(sock, res);
            } else {
                byte_array content;
                if (!sf_read_file(file.filename, content)) {
                    res.set_status(404);
                    normal_response__(sock, res);
                } else {
                    file_cache__->set_data(file.filename, cahce_data_t { content, modify_time });
                    res.set_body(content);
                    normal_response__(sock, res);
                }
            }
        }
    }
}

inline void sf_http_base_server::send_response_file_part__(
    SOCKET sock, const sf_http_response::response_file_info_t& file,
    std::ifstream& fi) const
{
    fi.seekg(file.begin, std::ios_base::beg);
    const auto buffer_size = 4096;
    byte_array buffer(static_cast<unsigned long>(buffer_size));
    auto curr_read_pos = file.begin;
    while (curr_read_pos < file.end - buffer_size) {
        sf_debug("read file", curr_read_pos, file.end);
        fi.read(buffer.data(), buffer_size);
        server__->send(sock, buffer);
        curr_read_pos += buffer_size;
    }
    fi.read(buffer.data(),
        static_cast<std::streamsize>(file.end - curr_read_pos));
    buffer.resize(static_cast<unsigned long>(file.end - curr_read_pos));
    server__->send(sock, buffer);
}

template <typename T>
bool sf_http_base_server::analysis_websocket_pkg__(SOCKET sock, const T* header,
    int& resolve_pos,
    unsigned long long& len,
    byte_array& body, bool& fin,
    int& op_code)
{
    std::lock_guard<std::recursive_mutex> lck(mu_websocket_context__);
    len = sf_get_size(*header);
    if (websocket_context__[sock].buffer.size() - resolve_pos - sizeof(T) < len) {
        return false;
    }
    sf_debug(len, sizeof(T), websocket_context__[sock].buffer.size());

    body = byte_array(
        websocket_context__[sock].buffer.begin() + resolve_pos + sizeof(T),
        websocket_context__[sock].buffer.begin() + resolve_pos + sizeof(T) + static_cast<int>(len));
    if (sf_with_mask(*header)) {
        sf_debug("mask true",
            *reinterpret_cast<const unsigned int*>(header->mask_key));
        sf_decode_websocket_pkg(body, header->mask_key);
    }
    fin = sf_is_fin(*header);
    op_code = sf_get_op_code(*header);
    resolve_pos += sizeof(T) + static_cast<int>(len);
    sf_debug("resolve_pos", resolve_pos, "fin", fin, "op", op_code, "body",
        to_string(body));
    return true;
}

inline void sf_http_base_server::websocket_data_coming__(
    const SOCKET sock, const byte_array& data)
{
    std::lock_guard<std::recursive_mutex> lck(mu_websocket_context__);
    websocket_context__[sock].buffer += data;
    sf_debug("buffer size", websocket_context__[sock].buffer.size());
    auto resolve_pos = 0;

    while (websocket_context__[sock].buffer.size() - resolve_pos >= sizeof(sf_websocket_client_data_1_header_t)) {
        const auto header1 = reinterpret_cast<const sf_websocket_client_data_1_header_t*>(
            websocket_context__[sock].buffer.data() + resolve_pos);

        auto fin = false;
        auto op_code = -1;
        byte_array body;

        // TODO 根据不同的len，获取body
        auto len = sf_get_size(*header1);
        sf_debug("base len", len);
        if (len == 126) {
            if (websocket_context__[sock].buffer.size() - resolve_pos < sizeof(sf_websocket_client_data_2_header_t)) {
                break;
            }
            const auto header2 = reinterpret_cast<const sf_websocket_client_data_2_header_t*>(
                websocket_context__[sock].buffer.data() + resolve_pos);
            if (!analysis_websocket_pkg__(sock, header2, resolve_pos, len, body,
                    fin, op_code)) {
                break;
            }
        } else if (len == 127) {
            if (websocket_context__[sock].buffer.size() - resolve_pos < sizeof(sf_websocket_client_data_3_header_t)) {
                break;
            }
            const auto header3 = reinterpret_cast<const sf_websocket_client_data_3_header_t*>(
                websocket_context__[sock].buffer.data() + resolve_pos);
            if (!analysis_websocket_pkg__(sock, header3, resolve_pos, len, body,
                    fin, op_code)) {
                break;
            }
        } else {
            if (!analysis_websocket_pkg__(sock, header1, resolve_pos, len, body,
                    fin, op_code)) {
                break;
            }
        }
        sf_debug("op", op_code);
        websocket_context__[sock].data_buffer += body;
        if (WEBSOCKET_OP_DISCONNECT_PKG == op_code) {
            server__->close(sock);
            if (websocket_close_callback__) {
                websocket_close_callback__(sock, websocket_context__[sock].url);
            }
            websocket_context__.erase(sock);
            return;
        }
        if (fin) {
            if (WEBSOCKET_OP_TEXT_PKG == op_code) {
                sf_debug("text data",
                    to_string(websocket_context__[sock].data_buffer));
                if (websocket_text_data_callback__) {
                    websocket_text_data_callback__(
                        sock, websocket_context__[sock].url,
                        to_string(websocket_context__[sock].data_buffer));
                }
            } else if (WEBSOCKET_OP_BINARY_PKG == op_code) {
                sf_debug("binary data",
                    websocket_context__[sock].data_buffer.size());
                if (websocket_binary_data_callback__) {
                    websocket_binary_data_callback__(
                        sock, websocket_context__[sock].url,
                        websocket_context__[sock].data_buffer);
                }
            } else if (WEBSOCKET_OP_PING_PKG == op_code) {
                // TODO ping消息相应
            } else if (WEBSOCKET_OP_PONG_PKG == op_code) {
                // TODO pong消息相应
            } else if (WEBSOCKET_OP_MIDDLE_PKG == op_code) {
                // TODO middle 消息响应
            } else {
                // TODO 其他消息响应
            }
            websocket_context__[sock].data_buffer.clear();
        }
    }
    websocket_context__[sock].buffer.erase(
        websocket_context__[sock].buffer.begin(),
        websocket_context__[sock].buffer.begin() + resolve_pos);
}

inline void sf_http_base_server::build_new_request__(SOCKET sock)
{
    sf_debug("new connection");
}

inline void sf_http_base_server::on_socket_closed__(SOCKET sock)
{
    std::unique_lock<std::recursive_mutex> lck(mu_request_context__);
    if (request_context__.count(sock) != 0) {
        request_context__.erase(sock);
    }
    lck.unlock();

    if (websocket_context__.count(sock) != 0) {
        std::lock_guard<std::recursive_mutex> lck2(mu_websocket_context__);
        if (websocket_close_callback__) {
            websocket_close_callback__(sock, websocket_context__[sock].url);
        }
        websocket_context__.erase(sock);
    }
    std::lock_guard<std::recursive_mutex> lck2(mu_multipart_data_context__);
    if (multipart_data_context__.count(sock) != 0) {
        std::lock_guard<std::recursive_mutex> lck3(mu_multipart_data_context__);
        multipart_data_context__.erase(sock);
    }
}

inline sf_http_base_server::sf_http_base_server(
    const sf_http_server_config& config)
    : config__(config)
    , file_cache__(sf_cache::make_instance(config.max_cache_count))
{
    sf_info("server config:", to_json(config));

    sf_bind_signal(
        server__, raw_data_coming,
        [this](SOCKET sock, const byte_array& data) {
            raw_data_coming__(sock, data);
        },
        true);
    sf_bind_signal(
        server__, new_connection,
        [this](SOCKET sock) { build_new_request__(sock); }, true);
    sf_bind_signal(
        server__, closed, [this](SOCKET sock) { on_socket_closed__(sock); },
        true);

    sf_bind_signal(
        &session_timer__, timeout, [this] { flush_session__(); }, true);

    session_timer__.start(1000);
}

inline void sf_http_base_server::set_request_callback(
    std::function<void(const sf_http_request&, sf_http_response&)>
        request_callback)
{
    request_callback__ = std::move(request_callback);
}

inline void sf_http_base_server::set_websocket_request_callback(
    std::function<void(const sf_http_request&, sf_http_response&)>
        websocket_request_callback)
{
    websocket_request_callback__ = std::move(websocket_request_callback);
}

inline void sf_http_base_server::set_websocket_binary_data_callback(
    std::function<void(SOCKET, const std::string& url, const byte_array&)>
        websocket_binary_data_callback)
{
    websocket_binary_data_callback__ = std::move(websocket_binary_data_callback);
}

inline void sf_http_base_server::set_websocket_text_data_callback(
    std::function<void(SOCKET, const std::string& url, const std::string&)>
        websocket_text_data_callback)
{
    websocket_text_data_callback__ = std::move(websocket_text_data_callback);
}

inline void sf_http_base_server::set_websocket_open_callback(
    std::function<void(SOCKET, const std::string& url)>
        websocket_open_callback)
{
    websocket_open_callback__ = std::move(websocket_open_callback);
}

inline void sf_http_base_server::set_websocket_close_callback(
    std::function<void(SOCKET, const std::string& url)>
        websocket_close_callback)
{
    websocket_close_callback__ = std::move(websocket_close_callback);
}

inline void sf_http_base_server::quit() { event_loop__.quit(); }

inline bool sf_http_base_server::start()
{
    if (!server__->listen(config__.host, config__.port)) {
        sf_debug("listen error");
        return false;
    }
    sf_debug("listen succeed!", config__.port);
    std::thread([this] { event_loop__.exec(); }).join();
    return true;
}

template <typename T>
bool sf_http_base_server::send_websocket_data(SOCKET sock, const T& data)
{
    if constexpr (std::is_same_v<T, std::string>) {
        sf_debug("send websocket message", data);
    }
    return server__->send(sock, make_server_websocket_data_pkg(data));
}

template <typename T>
void sf_http_base_server::send_websocket_data(const T& data)
{
    if constexpr (std::is_same_v<T, std::string>) {
        sf_debug("send websocket message", data);
    }
    for (auto& p : websocket_context__) {
        server__->send(p.first, make_server_websocket_data_pkg(data));
    }
}

inline void sf_http_base_server::close_websocket(SOCKET sock)
{
    server__->close(sock);
    std::lock_guard<std::recursive_mutex> lck(mu_websocket_context__);
    if (websocket_context__.count(sock) != 0) {
        websocket_context__.erase(sock);
    }
}

inline byte_array sf_http_base_server::append_multipart_data__(
    sf_multipart_data_context_t& multipart_data, const byte_array& data) const
{
    auto tmp_data = data;
    while (!tmp_data.empty()) {
        if (multipart_data.multipart.empty() || multipart_data.multipart.back().is_finished()) {
            multipart_data.multipart.emplace_back(multipart_data.boundary_str,
                config__.tmp_file_path);
        }
        const auto ret = multipart_data.multipart.back().append_data(tmp_data, tmp_data);
        if (!ret) {
            return tmp_data;
        }
        if (multipart_data.multipart.back().is_end()) {
            return byte_array();
        }
    }
    return byte_array();
}

inline void sf_http_base_server::flush_session__()
{
    std::lock_guard<std::recursive_mutex> lck(mu_session__);
    for (auto& p : session_data__) {
        p.second->timeout -= 1;
    }
    std::erase_if(session_data__,
        [](const auto& p) { return p.second->timeout <= 0; });
}

inline sf_json sf_http_base_server::session(const std::string& session_key)
{
    std::lock_guard<std::recursive_mutex> lck(mu_session__);
    if (session_data__.count(session_key) == 0) {
        return sf_json();
    }
    session_data__[session_key]->timeout = config__.session_timeout;
    return session_data__[session_key]->data;
}

inline void sf_http_base_server::keep_session_alive(
    const std::string& session_key)
{
    std::lock_guard<std::recursive_mutex> lck(mu_session__);
    if (session_data__.count(session_key) == 0) {
        session_data__[session_key] = std::make_shared<session_data_t>(
            session_data_t { config__.session_timeout, sf_json() });
    }
    session_data__[session_key]->timeout = config__.session_timeout;
}

template <typename T>
void sf_http_base_server::set_session(const std::string& session_key,
    const std::string& key, const T& value)
{
    std::lock_guard<std::recursive_mutex> lck(mu_session__);
    keep_session_alive(session_key);
    session_data__[session_key]->data[key] = sf_json(value);
}
} // namespace skyfire
#pragma clang diagnostic pop

#undef SF_SSL
#undef SF_ZLIB
