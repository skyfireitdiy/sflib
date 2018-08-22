#pragma once

#include "sf_http_response.h"

namespace skyfire
{

    void sf_http_response::set_status(int status) {
        status__ = status;
    }

    void sf_http_response::set_http_version(const std::string &http_version) {
        http_version__ = http_version;
    }

    void sf_http_response::set_reason(const std::string &reason) {
        reason__ = reason;
    }

    void sf_http_response::set_header(const sf_http_header &header) {
        header__ = header;
    }

    void sf_http_response::set_body(const byte_array &body) {
        body__ = body;
    }

    unsigned long long sf_http_response::get_length() const {
        return body__.size();
    }

    sf_http_header &sf_http_response::get_header() {
        return header__;
    }

    byte_array sf_http_response::to_package() const {
        std::string response_head;
        response_head += http_version__ + " " + std::to_string(status__) + " " + reason__ + "\r\n";
        for(auto &p:header__.get_key_list())
        {
            response_head += p + ":" + header__.get_header_value(p) + "\r\n";
        }
        response_head += "\r\n";
        byte_array pkg(response_head.begin(),response_head.end());
        pkg.insert(pkg.end(), body__.begin(),body__.end());
        return pkg;
    }
}