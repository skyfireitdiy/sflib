
#pragma once

#include <sstream>
#include <utility>
#include "sf_type.h"
#include "sf_http_request_line.h"
#include "sf_http_header.h"
#include "sf_utils.h"

namespace skyfire{
    class sf_http_request
    {
    private:
        const byte_array raw__;
        bool valid__ = false;
        sf_http_request_line request_line__;
        sf_http_header header__;
        byte_array body__;

        static bool split_request__(const byte_array &raw,std::string &request_line, std::vector<std::string> &header_lines, byte_array &body)
        {
            auto raw_string = to_string(raw);
            auto pos = raw_string.find("\r\n\r\n");
            if(pos == std::string::npos)
                return false;
            body = byte_array(raw.begin()+pos+4, raw.end());
            std::istringstream si(std::string(raw_string.begin(), raw_string.begin()+pos));
            getline(si, request_line);
            std::string tmp_str;
            header_lines.clear();
            while(!si.eof()){
                getline(si,tmp_str);
                header_lines.push_back(tmp_str);
            }
            return true;
        }

        static bool parse_request_line(const std::string& request_line,sf_http_request_line& request_line_para)
        {
            std::istringstream si(request_line);
            if(!(si>>request_line_para.method))
                return false;
            if(!(si>>request_line_para.url))
                return false;
            return !!(si >> request_line_para.http_version);
        }

        static bool parse_header(const std::vector<std::string> header_lines, sf_http_header& header)
        {

            for(auto &line:header_lines)
            {
                auto pos = line.find(':');
                if(pos==std::string::npos)
                    return false;
                std::string key(line.begin(),line.begin()+pos);
                std::string value(line.begin()+pos+1, line.end());
                value = sf_string_trim(value);
                header.set_header(sf_to_lower_string(key),value);
            }
            return true;
        }

        bool parse_request__()
        {
            std::string request_line;
            std::vector<std::string> header_lines;
            if(!split_request__(raw__,request_line, header_lines, body__))
                return false;
            if(!parse_request_line(request_line,request_line__))
                return false;
            if(!parse_header(header_lines, header__))
                return false;
            auto content_len = header__.get_header_value("Content-Length","0");
            return body__.size() == std::atoll(content_len.c_str());
        }

    public:
        explicit sf_http_request(byte_array raw): raw__(std::move(raw))
        {
            valid__ = parse_request__();
        }

        bool is_valid() const
        {
            return valid__;
        }

        sf_http_request_line get_request_line() const
        {
            return request_line__;
        }

        sf_http_header get_header() const
        {
            return header__;
        }

        byte_array get_body() const
        {
            return body__;
        }
    };
}