#pragma once

#include <string>

#include "sf_http_header.hpp"
#include "sf_type.hpp"

namespace skyfire
{
    class sf_http_response
    {
    private:
        int status__ = 200;
        std::string http_version__ = "HTTP/1.1";
        std::string reason__ = "OK";
        sf_http_header header__;
        byte_array body__;
    public:
        void set_status(int status);
        void set_http_version(const std::string& http_version);
        void set_reason(const std::string& reason);
        void set_header(const sf_http_header& header);
        void set_body(const byte_array & body);

        unsigned long long get_length() const;

        sf_http_header& get_header();

        byte_array to_package() const;
    };
}