#pragma once

#include "sf_http_multipart.hpp"

namespace skyfire
{

    class sf_http_multipart_form : public sf_http_multipart
    {
    public:
        byte_array append_data(byte_array data) override ;
        sf_http_multipart_type get_type() override ;
    };

}