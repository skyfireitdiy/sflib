#pragma once

#include <string>
#include "sf_type.hpp"
#include "sf_http_utils.h"



namespace skyfire
{
    class sf_http_base_server;

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
                byte_array body;
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
        std::map<std::string,sf_http_cookie_t> cookies__;
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
        void add_cookie(const sf_http_cookie_t &cookie_data);
        void remove_cookie(const std::string& key);
        std::map<std::string,sf_http_cookie_t> get_cookies() const;

        response_type get_type() const ;
        response_file_info_t get_file() const;
        std::vector<multipart_info_t> get_multipart() const;
        unsigned long long get_length() const;

        sf_http_header& get_header();
        byte_array to_header_package() const;
        byte_array to_package() const;

        friend sf_http_base_server;
    };
}