
#pragma once

#include <sstream>
#include <utility>
#include <string>
#include "sf_type.h"
#include "sf_http_request_line.h"
#include "sf_utils.h"
#include "sf_http_header.h"
#include "sf_http_utils.h"
#include "sf_logger.h"

namespace skyfire{
    class sf_http_request
    {
    private:
        const byte_array raw__;
        bool valid__ = false;
        sf_http_request_line request_line__;
        sf_http_header header__;
        byte_array body__;
        bool boundary_data__ = false;
        boundary_data_context_t boundary_data_context__;
        bool parse_request__();

    public:
        explicit sf_http_request(byte_array raw);

        explicit sf_http_request(boundary_data_context_t boundary_data);

        bool is_valid() const;

        sf_http_request_line get_request_line() const;

        sf_http_header get_header() const;

        byte_array get_body() const;

        bool is_boundary_data() const;

        boundary_data_context_t get_boundary_data_context() const;

        static bool split_request(const byte_array &raw,std::string &request_line, std::vector<std::string> &header_lines, byte_array &body);

        static bool parse_request_line(const std::string& request_line,sf_http_request_line& request_line_para);

        static bool parse_header(const std::vector<std::string> header_lines, sf_http_header& header);
    };
}