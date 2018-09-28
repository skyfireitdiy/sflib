#pragma once

#include "sf_type.h"
#include "sf_http_utils.h"
#include <fstream>

namespace skyfire
{

    class sf_http_multipart
    {
    protected:
        sf_http_header header__;
        bool end__ = {false};
        bool finish__ = {false};
        std::string  boundary_str__;
        std::shared_ptr<std::ofstream> fp__;
        bool first_block = true;
        std::string filename__;
    public:
        sf_http_multipart(const std::string& boundary_str, const std::string& tmp_path);
        sf_http_header get_header() const;
        bool is_end() const;
        bool is_finished() const;
        std::string get_filename() const;
        bool append_data(const byte_array& data, byte_array &ret);
    };


}