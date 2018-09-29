#pragma once

#include "sf_http_response.h"

namespace skyfire
{

    inline void sf_http_response::set_status(int status) {
        status__ = status;
    }

    inline void sf_http_response::set_http_version(const std::string &http_version) {
        http_version__ = http_version;
    }

    inline void sf_http_response::set_reason(const std::string &reason) {
        reason__ = reason;
    }

    inline void sf_http_response::set_header(const sf_http_header &header) {
        header__ = header;
    }

    inline void sf_http_response::set_body(const byte_array &body) {
        type__ = response_type ::normal;
        body__ = body;
    }


    inline void sf_http_response::set_file(const sf_http_response::response_file_info_t &file_info)
    {
        type__ = response_type ::file;
        file_info__ = file_info;
    }

    inline void sf_http_response::set_multipart(const std::vector<multipart_info_t> &multipart_info_vec)
    {
        type__ = response_type ::multipart;
        multipart_info_vec__ = multipart_info_vec;
    }

    inline unsigned long long sf_http_response::get_length() const {
        return body__.size();
    }

    inline sf_http_header &sf_http_response::get_header() {
        return header__;
    }

    inline byte_array sf_http_response::to_package() const {
        if(type__ != response_type::normal)
            return byte_array();
        auto pkg = to_header_package();
        pkg.insert(pkg.end(), body__.begin(),body__.end());
        return pkg;
    }

    inline byte_array sf_http_response::to_header_package() const
    {
        std::string response_head;
        response_head += http_version__ + " " + std::to_string(status__) + " " + reason__ + "\r\n";
        for(auto &p:header__.get_key_list())
        {
            response_head += p + ":" + header__.get_header_value(p) + "\r\n";
        }
        response_head += "\r\n";
        return {response_head.begin(),response_head.end()};
    }

    inline sf_http_response::response_type sf_http_response::get_type() const
    {
        return type__;
    }

    inline sf_http_response::response_file_info_t sf_http_response::get_file() const
    {
        return file_info__;
    }

    inline std::vector<sf_http_response::multipart_info_t> sf_http_response::get_multipart() const
    {
        return multipart_info_vec__;
    }
}