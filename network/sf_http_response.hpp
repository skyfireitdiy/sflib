
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_http_response.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"


#include "sf_http_response.h"
#include "sf_http_status.h"
#include "sf_http_header.hpp"
#include "tools/sf_json.hpp"

namespace skyfire
{

    inline void sf_http_response::set_status(int status) {
        status__ = status;
        if(sf_http_status.count(status)!=0)
        {
            set_status_desc(sf_http_status[status]);
            // set_body(to_byte_array(sf_http_status[status]));
        }else
        {
            set_status_desc("Unknown");
            // set_body(to_byte_array("Unknown"s));
        }
    }

    inline void sf_http_response::set_http_version(const std::string &http_version) {
        http_version__ = http_version;
    }

    inline void sf_http_response::set_status_desc(const std::string &desc) {
        status_desc__ = desc;
    }

    inline void sf_http_response::set_header(const sf_http_header &header) {
        header__ = header;
    }

    inline void sf_http_response::set_body(const byte_array &body) {
        type__ = response_type ::normal;
        body__ = body;
    }

    inline void sf_http_response::set_json(const sf_json &json) {
        type__ = response_type ::normal;
        body__ =  to_byte_array(json.to_string());
        header__.set_header("Content-Type", "application/json");
    }


    inline void sf_http_response::set_file(const sf_http_response::response_file_info_t &file_info)
    {
        type__ = response_type ::file;
        file_info__ = file_info;
    }


    inline void sf_http_response::set_file(const std::string &filename) {
        type__ = response_type ::file;
        file_info__ = response_file_info_t{
            filename,
            0,
            -1
        };
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
        response_head += http_version__ + " " + std::to_string(status__) + " " + status_desc__ + "\r\n";
        response_head += header__.to_string();
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

    inline void sf_http_response::add_cookie(const sf_http_cookie_t &cookie_data)
    {
        cookies__[cookie_data.key] = cookie_data;
    }

    inline void sf_http_response::remove_cookie(const std::string &key)
    {
        cookies__.erase(key);
    }

    inline std::unordered_map<std::string, sf_http_cookie_t> sf_http_response::get_cookies() const
    {
        return cookies__;
    }

    inline void sf_http_response::redirect(const std::string &new_location, int code) {
        set_status(code);
        get_header().set_header("Location", new_location);
    }



}
#pragma clang diagnostic pop