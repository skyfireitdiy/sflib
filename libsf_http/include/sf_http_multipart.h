#pragma once

#include "sf_http_header.hpp"
#include "sf_type.hpp"
#include "sf_http_utils.hpp"

namespace skyfire
{

    enum class sf_http_multipart_type
    {
        form,
        file
    };

    class sf_http_multipart
    {
    protected:
        sf_http_multipart_type type__;
        sf_http_header_t header__;
        bool end__ = {false};
    public:
        sf_http_multipart(sf_http_multipart_type type);
        sf_http_header_t get_header() const;
        bool is_end() const;
        virtual byte_array append_data(byte_array data) = 0;
        virtual sf_http_multipart_type get_type() = 0;
    };


}