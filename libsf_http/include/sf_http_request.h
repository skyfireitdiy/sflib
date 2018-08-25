
#pragma once

#include <sstream>
#include <utility>
#include "sf_type.hpp"
#include "sf_http_request_line.hpp"
#include "sf_http_header.hpp"
#include "sf_utils.hpp"
#include "sf_logger.hpp"

namespace skyfire{
    class sf_http_request
    {
    private:
        const byte_array raw__;
        bool valid__ = false;
        sf_http_request_line request_line__;
        sf_http_header header__;
        byte_array body__;

        static bool split_request__(const byte_array &raw,std::string &request_line, std::vector<std::string> &header_lines, byte_array &body);

        static bool parse_request_line(const std::string& request_line,sf_http_request_line& request_line_para);

        static bool parse_header(const std::vector<std::string> header_lines, sf_http_header& header);

        bool parse_request__();

    public:
        explicit sf_http_request(byte_array raw);

        bool is_valid() const;

        sf_http_request_line get_request_line() const;

        sf_http_header get_header() const;

        byte_array get_body() const;
    };
}