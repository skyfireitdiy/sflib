
#include "sflib/http/http_server_base.h"
#include "sflib/cache/cache.h"
#include "sflib/core/type.h"
#include "sflib/http/http_content_type.h"
#include "sflib/http/http_server_config.h"
#include "sflib/http/http_server_req_multipart.h"
#include "sflib/http/http_session_id_key.h"
#include "sflib/http/http_utils.h"
#include "sflib/http/websocket_utils.h"
#include "sflib/json/json.h"
#include "sflib/object/eventloop.h"
#include "sflib/object/object.h"
#include "sflib/object/timer.h"

#include "sflib/tcp/tcp_server.h"
#include "sflib/tools/finally.h"
#include "sflib/tools/random.h"

using namespace std::string_literals;
namespace skyfire
{
void http_server_base::http_handler__(
    const SOCKET sock, const http_server_request &http_server_request)
{
    bool keep_alive = true;
    http_server_response res(http_server_request);
    res.set_status(200);
    res.set_http_version(http_server_request.request_line().http_version);
    http_cookie_item_t session_cookie;
    session_cookie.key = session_id_key;
    auto session_id = http_server_request.session_id();
    session_cookie.value = session_id;
    keep_session_alive(session_id);
    res.add_cookie(session_cookie);
    request_callback__(http_server_request, res);
    // 运行中间件
    for (auto &m : middleware__)
    {
        if (!m->before(http_server_request, res))
        {
            return;
        }
    }
    if (equal_nocase_string(
            http_server_request.header().header_value("Connection", "Close"),
            "Close"))
    {
        keep_alive = false;
    }
    res.header().set_header("Server", "SkyFire HTTP Server");
    res.header().set_header("Connection", keep_alive ? "Keep-Alive" : "Close");
    // 运行中间件，以相反的顺序执行
    for (auto m = middleware__.rbegin(); m != middleware__.rend(); ++m)
    {
        if (!(*m)->after(http_server_request, res))
        {
            return;
        }
    }
    // http响应的实现
    if (res.type() == http_data_type::file)
    {
        file_response__(sock, res);
    }
    else if (res.type() == http_data_type::multipart)
    {
        multipart_response__(sock, res);
    }
    else if (res.type() == http_data_type::normal)
    {
        normal_response__(sock, res);
    }
    if (!keep_alive)
    {
        close_request__(sock);
    }
}
void http_server_base::add_middleware(std::shared_ptr<http_middleware> m)
{
    middleware__.push_back(m);
}
void http_server_base::raw_data_coming__(SOCKET sock,
                                         const byte_array &data)
{

    // 过滤websocket消息
    {
        std::lock_guard lck(mu_websocket_context__);
        if (websocket_context__.count(sock) != 0)
        {

            websocket_data_coming__(sock, data);
            return;
        }
    }
    // 普通请求
    {
        std::lock_guard lck(mu_request_context__);
        //
        if (request_context__.count(sock) == 0)
        {

            request_context__[sock] = request_context_t();
        }
        request_context__[sock].buffer += data;
        if (request_context__[sock].buffer.empty())
        {
            return;
        }
    }
    // multipart消息
    {
        std::lock_guard lck(mu_multipart_data_context__);
        if (multipart_data_context__.count(sock) != 0)
        {
            std::lock_guard lck2(mu_request_context__);
            request_context__[sock].buffer = append_multipart_data__(
                multipart_data_context__[sock], request_context__[sock].buffer);
            if (multipart_data_context__[sock].multipart.back().is_end())
            {

                http_handler__(sock, http_server_request(multipart_data_context__[sock], sock));
                multipart_data_context__.erase(sock);
            }
            request_context__[sock].buffer.clear();
            return;
        }
    }
    std::unique_lock lck(mu_request_context__);
    http_server_request request(request_context__[sock].buffer, sock);
    if (request.is_valid())
    {
        request_context__[sock].new_req = true;
        request_context__[sock].buffer.clear();
        lck.unlock();
        auto req_header = request.header();
        if (request.is_multipart_data())
        {

            build_boundary_context_data(sock, request);
            return;
        }
        const auto connection_header = req_header.header_value("Connection");
        auto connection_header_list = split_string(connection_header, ",");
        // 过滤Connection为Upgrade的请求
        if (std::find_if(connection_header_list.begin(),
                         connection_header_list.end(),
                         [](const std::string &str) {
                             return equal_nocase_string(string_trim(str),
                                                        "Upgrade");
                         })
            != connection_header_list.end())
        {
            // 筛选Websocket请求
            if (equal_nocase_string(req_header.header_value("Upgrade"),
                                    "websocket"))
            {

                build_websocket_context_data__(sock, request);
            }
            return;
        }

        if (request_callback__)
        {
            http_handler__(sock, request);
        }
    }
    else if (request.is_error())
    {

        server__->close(sock);
    }
}
void http_server_base::build_websocket_context_data__(
    SOCKET sock, const http_server_request &request)
{
    // NOTE 删除记录，防止超时检测线程关闭连接
    std::unique_lock lck(mu_request_context__);
    request_context__.erase(sock);
    lck.unlock();
    websocket_context_t ws_data;
    ws_data.url = request.request_line().url;
    if (websocket_request_callback__)
    {
        http_server_response res(request);
        websocket_request_callback__(request, res);
        res.header().set_header("Content-Length", std::to_string(res.length()));

        if (!server__->send(sock, res.to_package()))
        {

            return;
        }
        std::lock_guard lck2(mu_websocket_context__);
        websocket_context__.insert({sock, ws_data});
        websocket_context__[sock].sock = sock;
        websocket_context__[sock].buffer = byte_array();
        if (websocket_open_callback__)
        {
            websocket_open_callback__(sock, ws_data.url);
        }
    }
}
void http_server_base::build_boundary_context_data(
    SOCKET sock, const http_server_request &request)
{

    // 初始化boundary数据
    auto multipart_data = request.multipart_data_context();
    std::unique_lock lck(mu_request_context__);
    request_context__[sock].buffer = append_multipart_data__(multipart_data, request.body());
    lck.unlock();
    if (!multipart_data.multipart.empty() && multipart_data.multipart.back().is_end())
    {

        http_handler__(sock, http_server_request(multipart_data, sock));
    }
    else
    {

        std::lock_guard lck2(mu_multipart_data_context__);
        multipart_data_context__[sock] = multipart_data;
    }
}
void http_server_base::close_request__(SOCKET sock)
{
    {
        std::lock_guard lck(mu_request_context__);
        request_context__.erase(sock);
    }
    {
        std::lock_guard lck(mu_multipart_data_context__);
        multipart_data_context__.erase(sock);
    }
}
void http_server_base::normal_response__(
    SOCKET sock, http_server_response &res) const
{
    res.header().set_header("Content-Length", std::to_string(res.length()));

    if (!server__->send(sock, res.to_package()))
    {
    }
}
void http_server_base::multipart_response__(SOCKET sock,
                                            http_server_response &res)
{
    auto multipart = res.multipart();
    if (!check_analysis_multipart_file__(multipart))
    {
        res.set_status(403);
        res.set_body(to_byte_array("<p>Forbidden</p>"));
        res.header().set_header("Content-Type", "text/html");
        normal_response__(sock, res);
        return;
    }
    auto boundary_str = random::instance()->uuid_str();
    unsigned long long content_length = 0;
    std::vector<std::string> header_vec;
    std::string end_str = "--" + boundary_str + "--";
    for (auto &p : multipart)
    {
        if (p.type == http_multipart_info_t::multipart_info_type::file)
        {
            auto tmp_str = "--" + boundary_str + "\r\n";
            auto suffix = to_lower_string(get_path_ext(p.file_info.filename));
            if (http_content_type.count(suffix) != 0)
            {
                tmp_str += "Content-Type:" + http_content_type[suffix] + "\r\n";
            }
            else
            {
                tmp_str += "Content-Type: application/octet-stream\r\n";
            }
            long long file_size = std::filesystem::file_size(p.file_info.filename);
            tmp_str += "Content-Range: bytes " + std::to_string(p.file_info.begin) + "-" + std::to_string(p.file_info.end) + "/" + std::to_string(file_size) + "\r\n\r\n";
            header_vec.push_back(tmp_str);
            content_length += tmp_str.length() + (p.file_info.end - p.file_info.begin) + 2; // 添加\r\n
        }
        else if (p.type == http_multipart_info_t::multipart_info_type::form)
        {
            auto tmp_str = "--" + boundary_str + "\r\n";
            http_server_res_header tmp_header;
            tmp_header.set_header(p.form_info.header);
            tmp_str += tmp_header.to_string();
            header_vec.push_back(tmp_str);
            content_length += tmp_str.length() + p.form_info.body.size() + 2;
        }
    }
    content_length += end_str.length();
    auto &res_header = res.header();
    res_header.set_header("Content-Type",
                          "multipart/byteranges; boundary=" + boundary_str);
    res_header.set_header("Content-Length", std::to_string(content_length));
    res.set_status(206);
    // 发送header
    if (!server__->send(sock, res.to_header_package()))
    {

        return;
    }
    for (auto i = 0UL; multipart.size() > i; ++i)
    {
        if (!server__->send(sock, to_byte_array(header_vec[i])))
        {

            return;
        }
        if (multipart[i].type == http_multipart_info_t::multipart_info_type::file)
        {
            std::ifstream fi(multipart[i].file_info.filename,
                             std::ios::in | std::ios::binary);
            if (!fi)
            {
                close_request__(sock);
                return;
            }
            send_response_file_part__(sock, multipart[i].file_info, fi);
            fi.close();
            if (!server__->send(sock, to_byte_array("\r\n" s)))
            {

                return;
            }
        }
        else if (multipart[i].type == http_multipart_info_t::multipart_info_type::form)
        {
            if (!server__->send(sock, to_byte_array(header_vec[i])))
            {

                return;
            }
            if (!server__->send(sock, multipart[i].form_info.body))
            {

                return;
            }
            if (!server__->send(sock, to_byte_array("\r\n" s)))
            {

                return;
            }
        }
    }
    if (!server__->send(sock, to_byte_array(end_str)))
    {

        return;
    }
}
bool http_server_base::check_analysis_multipart_file__(
    std::vector<http_multipart_info_t> &multipart_data)
{
    for (auto &p : multipart_data)
    {
        if (p.type == http_multipart_info_t::multipart_info_type::file)
        {
            uintmax_t file_size = 0;
            try
            {
                file_size = std::filesystem::file_size(p.file_info.filename);
            }
            catch (std::filesystem::filesystem_error &e)
            {

                return false;
            }
            if (p.file_info.begin >= static_cast<long long>(file_size))
                return false;
            if (p.file_info.end == -1)
            {
                p.file_info.end = file_size;
            }
            if (p.file_info.end < p.file_info.begin)
                return false;
        }
    }
    return true;
}
http_server_base::file_etag_t http_server_base::make_etag__(const http_file_info_t &file) const
{
    std::error_code sf_error;
    auto modify_time = std::filesystem::last_write_time(file.filename, sf_error);
    if (sf_error)
    {
    }
    return http_server_base::file_etag_t {
        "W/\"" + std::to_string(modify_time.time_since_epoch().count()) + "-" + std::to_string(file.file_size) + "\"",
        make_time_str(modify_time)};
}
void http_server_base::set_file_etag__(http_server_response &res, const file_etag_t &etag) const
{

    res.header().set_header("Etag", etag.etag);
    res.header().set_header("Last-Modified", etag.last_modify);
}
void http_server_base::file_response__(SOCKET sock,
                                       http_server_response &res) const
{
    // 先检测是否可以返回304
    auto file = res.file();
    auto req = res.get_req();
    auto etag = make_etag__(file);
    if (req.header().has_key("If-None-Match") && req.header().has_key("If-Modified-Since"))
    {
        if (etag.etag == req.header().header_value("If-None-Match") && etag.last_modify == req.header().header_value("If-Modified-Since"))
        {

            res.set_status(304);
            normal_response__(sock, res);
            return;
        }
    }
    auto file_size = std::filesystem::file_size(file.filename);

    if (file.begin != 0 || (file.end != static_cast<long long>(file_size) && file.end != -1))
    {
        auto &header = res.header();
        if (file.end == -1)
        {
            file.end = file_size;
        }
        if (file_size != 0 && (file.begin > static_cast<long long>(file_size) || file.end > static_cast<long long>(file_size)))
        {
            res.set_status(416);
            res.set_body(to_byte_array("Requested Range Not Satisfiable" s));
            normal_response__(sock, res);
            return;
        }
        header.set_header("Content-Range", "bytes " + std::to_string(file.begin) + "-" + std::to_string(file.end) + "/" + std::to_string(file_size));
        header.set_header("Content-Length",
                          std::to_string(file.end - file.begin));
        auto suffix = to_lower_string(get_path_ext(file.filename));
        if (http_content_type.count(suffix) != 0)
        {
            header.set_header("Content-Type", http_content_type[suffix]);
        }
        else
        {
            header.set_header("Content-Type", "application/octet-stream");
        }
        std::ifstream fi(file.filename,
                         std::ios_base::binary | std::ios_base::in);
        if (!fi)
        {

            res.set_status(404);
            normal_response__(sock, res);
            return;
        }
        res.set_status(206);
        sf_finally([&fi]() {
            fi.close();
        });
        if (!server__->send(sock, res.to_header_package()))
        {

            return;
        }
        send_response_file_part__(sock, file, fi);
    }
    else
    {
        file.end = file_size;
        auto &header = res.header();
        header.set_header("Content-Length", std::to_string(file_size));
        auto suffix = to_lower_string(get_path_ext(file.filename));
        if (http_content_type.count(suffix) != 0)
        {
            header.set_header("Content-Type", http_content_type[suffix]);
        }
        else
        {
            header.set_header("Content-Type", "application/octet-stream");
        }
        if (file_size > config__.max_cache_file_size || config__.debug)
        {
            std::ifstream fi(file.filename,
                             std::ios_base::binary | std::ios_base::in);
            if (!fi)
            {

                res.set_status(404);
                normal_response__(sock, res);
                return;
            }
            sf_finally([&fi]() {
                fi.close();
            });
            set_file_etag__(res, etag);
            if (!server__->send(sock, res.to_header_package()))
            {

                return;
            }
            send_response_file_part__(sock, file, fi);
        }
        else
        {
            std::error_code sf_error;
            auto modify_time = std::filesystem::last_write_time(file.filename, sf_error);
            if (sf_error)
            {

                res.set_status(404);
                normal_response__(sock, res);
                return;
            }
            auto data = file_cache__->data<file_cache_data_t>(file.filename);
            if (data && data->modify_time == modify_time)
            {
                set_file_etag__(res, etag);
                res.set_body(data->data);
                normal_response__(sock, res);
            }
            else
            {
                byte_array content;
                if (!read_file(file.filename, content))
                {
                    res.set_status(404);
                    normal_response__(sock, res);
                }
                else
                {
                    set_file_etag__(res, etag);
                    file_cache__->set_data(file.filename, file_cache_data_t {content, modify_time});
                    res.set_body(content);
                    normal_response__(sock, res);
                }
            }
        }
    }
}
void http_server_base::send_response_file_part__(
    SOCKET sock, const http_file_info_t &file,
    std::ifstream &fi) const
{
    fi.seekg(file.begin, std::ios_base::beg);
    const auto buffer_size = 4096;
    byte_array buffer(static_cast<unsigned long>(buffer_size));
    auto curr_read_pos = file.begin;
    while (curr_read_pos < file.end - buffer_size)
    {

        fi.read(buffer.data(), buffer_size);
        if (server__->send(sock, buffer))
        {
            curr_read_pos += buffer_size;
        }
        else
        {

            return;
        }
    }
    fi.read(buffer.data(),
            static_cast<std::streamsize>(file.end - curr_read_pos));
    buffer.resize(static_cast<unsigned long>(file.end - curr_read_pos));
    if (!server__->send(sock, buffer))
    {
    }
}

void http_server_base::websocket_data_coming__(
    const SOCKET sock, const byte_array &data)
{
    std::lock_guard lck(mu_websocket_context__);
    websocket_context__[sock].buffer += data;

    auto resolve_pos = 0;
    while (websocket_context__[sock].buffer.size() - resolve_pos >= sizeof(websocket_client_data_1_header_t))
    {
        const auto header1 = reinterpret_cast<const websocket_client_data_1_header_t *>(
            websocket_context__[sock].buffer.data() + resolve_pos);
        auto fin = false;
        auto op_code = -1;
        byte_array body;
        auto len = get_size(*header1);

        if (len == 126)
        {
            if (websocket_context__[sock].buffer.size() - resolve_pos < sizeof(websocket_client_data_2_header_t))
            {
                break;
            }
            const auto header2 = reinterpret_cast<const websocket_client_data_2_header_t *>(
                websocket_context__[sock].buffer.data() + resolve_pos);
            if (!analysis_websocket_pkg__(sock, header2, resolve_pos, len, body,
                                          fin, op_code))
            {
                break;
            }
        }
        else if (len == 127)
        {
            if (websocket_context__[sock].buffer.size() - resolve_pos < sizeof(websocket_client_data_3_header_t))
            {
                break;
            }
            const auto header3 = reinterpret_cast<const websocket_client_data_3_header_t *>(
                websocket_context__[sock].buffer.data() + resolve_pos);
            if (!analysis_websocket_pkg__(sock, header3, resolve_pos, len, body,
                                          fin, op_code))
            {
                break;
            }
        }
        else
        {
            if (!analysis_websocket_pkg__(sock, header1, resolve_pos, len, body,
                                          fin, op_code))
            {
                break;
            }
        }

        websocket_context__[sock].data_buffer += body;
        if (WEBSOCKET_OP_DISCONNECT_PKG == op_code)
        {
            server__->close(sock);
            if (websocket_close_callback__)
            {
                websocket_close_callback__(sock, websocket_context__[sock].url);
            }
            websocket_context__.erase(sock);
            return;
        }
        if (fin)
        {
            if (WEBSOCKET_OP_TEXT_PKG == op_code)
            {
                if (websocket_text_data_callback__)
                {
                    websocket_text_data_callback__(
                        sock, websocket_context__[sock].url,
                        to_string(websocket_context__[sock].data_buffer));
                }
            }
            else if (WEBSOCKET_OP_BINARY_PKG == op_code)
            {
                if (websocket_binary_data_callback__)
                {
                    websocket_binary_data_callback__(
                        sock, websocket_context__[sock].url,
                        websocket_context__[sock].data_buffer);
                }
            }
            else if (WEBSOCKET_OP_PING_PKG == op_code)
            {
                // TODO ping消息相应
            }
            else if (WEBSOCKET_OP_PONG_PKG == op_code)
            {
                // TODO pong消息相应
            }
            else if (WEBSOCKET_OP_MIDDLE_PKG == op_code)
            {
                // TODO middle 消息响应
            }
            else
            {
                // TODO 其他消息响应
            }
            websocket_context__[sock].data_buffer.clear();
        }
    }
    websocket_context__[sock].buffer.erase(
        websocket_context__[sock].buffer.begin(),
        websocket_context__[sock].buffer.begin() + resolve_pos);
}
void http_server_base::build_new_request__(SOCKET sock)
{
}
void http_server_base::on_socket_closed__(SOCKET sock)
{
    std::unique_lock lck(mu_request_context__);
    if (request_context__.count(sock) != 0)
    {
        request_context__.erase(sock);
    }
    lck.unlock();
    if (websocket_context__.count(sock) != 0)
    {
        std::lock_guard lck2(mu_websocket_context__);
        if (websocket_close_callback__)
        {
            websocket_close_callback__(sock, websocket_context__[sock].url);
        }
        websocket_context__.erase(sock);
    }
    std::lock_guard lck2(mu_multipart_data_context__);
    if (multipart_data_context__.count(sock) != 0)
    {
        std::lock_guard lck3(mu_multipart_data_context__);
        multipart_data_context__.erase(sock);
    }
}
http_server_base::http_server_base(
    const http_server_config &config)
    : config__(config)
    , file_cache__(cache::make_instance(config.max_cache_count))
    , server__(tcp_server::make_instance(tcp::raw(true), tcp::thread_count(config.thread_count)))
{

    sf_bind(
        server__, raw_data_coming,
        [this](SOCKET sock, const byte_array &data) {
            raw_data_coming__(sock, data);
        },
        false);
    sf_bind(
        server__, new_connection,
        [this](SOCKET sock) {
            build_new_request__(sock);
        },
        false);
    sf_bind(
        server__, closed, [this](SOCKET sock) {
            on_socket_closed__(sock);
        },
        false);
    sf_bind(
        &session_timer__, timeout, [this] {
            flush_session__();
        },
        false);
    session_timer__.start(1000);
}
void http_server_base::set_request_callback(
    std::function<void(const http_server_request &, http_server_response &)>
        request_callback)
{
    request_callback__ = std::move(request_callback);
}
void http_server_base::set_websocket_request_callback(
    std::function<void(const http_server_request &, http_server_response &)>
        websocket_request_callback)
{
    websocket_request_callback__ = std::move(websocket_request_callback);
}
void http_server_base::set_websocket_binary_data_callback(
    std::function<void(SOCKET, const std::string &url, const byte_array &)>
        websocket_binary_data_callback)
{
    websocket_binary_data_callback__ = std::move(websocket_binary_data_callback);
}
void http_server_base::set_websocket_text_data_callback(
    std::function<void(SOCKET, const std::string &url, const std::string &)>
        websocket_text_data_callback)
{
    websocket_text_data_callback__ = std::move(websocket_text_data_callback);
}
void http_server_base::set_websocket_open_callback(
    std::function<void(SOCKET, const std::string &url)>
        websocket_open_callback)
{
    websocket_open_callback__ = std::move(websocket_open_callback);
}
void http_server_base::set_websocket_close_callback(
    std::function<void(SOCKET, const std::string &url)>
        websocket_close_callback)
{
    websocket_close_callback__ = std::move(websocket_close_callback);
}
void http_server_base::quit()
{
    event_loop__.quit();
}
bool http_server_base::start()
{
    if (!server__->listen(config__.host, config__.port))
    {

        return false;
    }

    std::thread(std::function([this] {
        event_loop__.exec();
    })).join();
    return true;
}
void http_server_base::close_websocket(SOCKET sock)
{
    server__->close(sock);
    std::lock_guard lck(mu_websocket_context__);
    if (websocket_context__.count(sock) != 0)
    {
        websocket_context__.erase(sock);
    }
}
byte_array http_server_base::append_multipart_data__(
    http_server_req_multipart_data_context_t &multipart_data, const byte_array &data) const
{
    auto tmp_data = data;
    while (!tmp_data.empty())
    {
        if (multipart_data.multipart.empty() || multipart_data.multipart.back().is_finished())
        {
            multipart_data.multipart.emplace_back(multipart_data.boundary_str,
                                                  config__.tmp_file_path);
        }
        const auto ret = multipart_data.multipart.back().append_data(tmp_data, tmp_data);
        if (!ret)
        {
            return tmp_data;
        }
        if (multipart_data.multipart.back().is_end())
        {
            return byte_array();
        }
    }
    return byte_array();
}
void http_server_base::flush_session__()
{
    std::lock_guard lck(mu_session__);
    for (auto &p : session_data__)
    {
        p.second->timeout -= 1;
    }
    std::erase_if(session_data__,
                  [](const auto &p) {
                      return p.second->timeout <= 0;
                  });
}
json http_server_base::session(const std::string &session_key)
{
    std::lock_guard lck(mu_session__);
    if (session_data__.count(session_key) == 0)
    {
        return json();
    }
    session_data__[session_key]->timeout = config__.session_timeout;
    return session_data__[session_key]->data;
}
void http_server_base::keep_session_alive(
    const std::string &session_key)
{
    std::lock_guard lck(mu_session__);
    if (session_data__.count(session_key) == 0)
    {
        session_data__[session_key] = std::make_shared<session_data_t>(
            session_data_t {config__.session_timeout, json()});
    }
    session_data__[session_key]->timeout = config__.session_timeout;
}

} // namespace skyfire
#undef SF_SSL
#undef SF_ZLIB
