#pragma once

#include <string>
#include <ctime>
#include <map>
#include <memory>

#include "sf_tcputils.h"
#include "sf_type.h"
#include "sf_http_request_line.h"
#include "sf_serialize_binary.hpp"



namespace skyfire
{
    class sf_http_multipart;

    using sf_http_header_t = std::map<std::string,std::string>;

    namespace {
        std::string websocket_sha1_append_str = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    }

    struct request_context_t
    {
        bool new_req = false;
        byte_array buffer;
    };

    SF_MAKE_SERIALIZABLE_BINARY(request_context_t,new_req,buffer)


    struct websocket_context_t
    {
        std::string url;
        SOCKET sock;
        byte_array buffer;
        byte_array data_buffer;
    };

    SF_MAKE_SERIALIZABLE_BINARY(websocket_context_t, url, sock, buffer, data_buffer)

    struct boundary_data_context_t{
        SOCKET sock;
        std::string boundary_str;
        sf_http_header_t header;
        sf_http_request_line request_line;
        std::vector<sf_http_multipart> multipart;
    };


    enum class cookie_life_type
    {
        session = 0,
        time_point = 1
    };

    struct sf_http_cookie_t{
        std::string key;
        std::string value;
        cookie_life_type life_type = cookie_life_type ::session;
        std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now();
        std::string path = "/";
        bool secure = true;
        bool http_only = true;
    };


    inline unsigned char sf_to_hex(unsigned char x);

    inline unsigned char sf_from_hex(unsigned char x);

    inline std::string sf_url_encode(const std::string &str);

    inline std::string sf_url_decode(const std::string &str);

    void sf_parse_param(std::map<std::string, std::string> param,std::string &param_str) ;

    inline void sf_parse_url(const std::string &raw_url, std::string &url, std::map<std::string,std::string>& param,
                             std::string frame);

    inline std::string sf_make_http_time_str();

    inline std::string sf_make_http_time_str(const std::chrono::system_clock::time_point& tp);

    inline std::string sf_to_header_key_format(std::string key);

}