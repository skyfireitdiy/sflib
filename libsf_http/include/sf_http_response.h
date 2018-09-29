#pragma once

#include <string>
#include "sf_type.hpp"

namespace skyfire
{
    class sf_http_response
    {
    public:
        enum class response_type
        {
            normal,
            file,
            multipart
        };

        struct response_file_info_t
        {
            std::string filename;
            long long begin;
            long long end;
        };

        struct multipart_info_t
        {
            enum class multipart_info_type
            {
                form,
                file
            };
            struct form_info_t
            {
                sf_http_header_t header;
                sf_http_header_t body;
            };

            multipart_info_type type;
            form_info_t form_info;
            response_file_info_t file_info;
        };

    private:
        int status__ = 200;
        std::string http_version__ = "HTTP/1.1";
        std::string reason__ = "OK";
        sf_http_header header__;
        byte_array body__;
        response_type type__ = {response_type ::normal};

        response_file_info_t file_info__;
        std::vector<multipart_info_t> multipart_info_vec__;
    public:
        void set_status(int status);
        void set_http_version(const std::string& http_version);
        void set_reason(const std::string& reason);
        void set_header(const sf_http_header& header);
        void set_body(const byte_array & body);
        void set_file(const response_file_info_t &file_info);
        void set_multipart(const std::vector<multipart_info_t>& multipart_info_vec);
        unsigned long long get_length() const;

        sf_http_header& get_header();

        byte_array to_package() const;
    };
}