#include "sflib/http/http_server_req_multipart.h"
#include "sflib/http/http_server_request.h"
namespace skyfire
{
http_server_req_multipart::http_server_req_multipart(const std::string& boundary_str,
                                                     const std::string& tmp_path)
    : boundary_str__(boundary_str)
    , filename__(std::filesystem::path(tmp_path) / random::instance()->uuid_str())
{
}
http_server_req_header http_server_req_multipart::header() const { return header__; }
bool                   http_server_req_multipart::is_end() const { return end__; }
bool                   http_server_req_multipart::is_finished() const { return finish__; }
std::string            http_server_req_multipart::filename() const { return filename__; }
bool                   http_server_req_multipart::append_data(const byte_array& data,
                                            byte_array&       ret)
{
    const auto new_boundary_str = "----" + boundary_str__;
    if (first_block)
    {
        auto tmp_data = to_string(data);
        if (tmp_data.find(new_boundary_str) != 0)
        {
            ret = data;
            return false;
        }
        std::string              request_line;
        std::vector<std::string> header_lines;
        byte_array               body;
        if (!http_server_request::split_request(data, request_line, header_lines,
                                                body))
        {
            ret = data;
            return false;
        }
        if (!http_server_request::parse_header(header_lines, header__))
        {
            ret = data;
            return false;
        }
        first_block = false;
        fp__        = std::shared_ptr<std::ofstream>(
            new std::ofstream(filename__, std::ios::binary | std::ios::out),
            [](std::ofstream* p) {
                if (p->good())
                {
                    p->close();
                }
                delete p;
            });
        // NOTE 暂时忽略掉打开失败的情况
        auto       body_str   = to_string(body);
        const auto finish_pos = body_str.find(new_boundary_str);
        if (finish_pos == std::string::npos)
        {
            fp__->write(body.data(), body.size());
            ret = byte_array();
            return true;
        }
        else
        {
            fp__->write(body.data(), finish_pos - 2); // 有一个\r\n
            finish__ = true;
            fp__->close();
            if (finish_pos == body_str.find(new_boundary_str + "--"))
            {
                end__ = true;
                ret   = byte_array();
                return true;
            }
            else
            {
                ret = { body.begin() + finish_pos, body.end() };
                return true;
            }
        }
    }
    else
    {
        auto       body       = data;
        auto       body_str   = to_string(body);
        const auto finish_pos = body_str.find(new_boundary_str);
        if (finish_pos == std::string::npos)
        {
            fp__->write(body.data(), body.size());
            ret = byte_array();
            return true;
        }
        else
        {
            fp__->write(body.data(), finish_pos - 2); // 有一个\r\n
            finish__ = true;
            fp__->close();
            if (finish_pos == body_str.find(new_boundary_str + "--"))
            {
                end__ = true;
                ret   = byte_array();
                return true;
            }
            else
            {
                ret = { body.begin() + finish_pos, body.end() };
                return true;
            }
        }
    }
}
} // namespace skyfire