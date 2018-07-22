#pragma once

#include <string>

#include "sf_http_header.h"
#include "sf_type.h"

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
        void set_status(int status)
        {
            status__ = status;
        }
        void set_http_version(const std::string& http_version)
        {
            http_version__ = http_version;
        }
        void set_reason(const std::string& reason)
        {
            reason__ = reason;
        }
        void set_header(const sf_http_header& header)
        {
            header__ = header;
        }
        void set_body(const byte_array & body)
        {
            body__ = body;
        }

        unsigned long long get_length() const
        {
            return body__.size();
        }

        sf_http_header& get_header()
        {
            return header__;
        }

        byte_array to_package() const
        {
            std::string response_head;
            response_head += http_version__ + " " + std::to_string(status__) + " " + reason__ + "\r\n";
            for(auto &p:header__.get_key_list())
            {
                response_head += p + ": " + header__.get_header_value(p) + "\r\n";
            }
            response_head += "\r\n";
            byte_array pkg(response_head.begin(),response_head.end());
            pkg.insert(pkg.end(), body__.begin(),body__.end());
            return pkg;
        }
    };
}