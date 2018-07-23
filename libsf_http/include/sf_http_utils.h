#pragma once

#include <string>
#include <ctime>
#include <map>

namespace skyfire
{

    namespace {
        std::string websocket_sha1_append_str = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    }

    struct request_context_t
    {
        bool new_req = false;
        byte_array buffer;
    };

    struct websocket_context_t
    {
        std::string url;
        SOCKET sock;
        byte_array buffer;
        // WARNING 会不会数据量太大导致内存不足？
        std::string text_msg;
        byte_array binary_msg;
    };

    inline unsigned char sf_to_hex(unsigned char x)
    {
        return static_cast<unsigned char>(x > 9 ? x + 55 : x + 48);
    }

    inline unsigned char sf_from_hex(unsigned char x)
    {
        unsigned char y = 0;
        if (x >= 'A' && x <= 'Z') y = static_cast<unsigned char>(x - 'A' + 10);
        else if (x >= 'a' && x <= 'z') y = static_cast<unsigned char>(x - 'a' + 10);
        else if (x >= '0' && x <= '9') y = x - '0';
        return y;
    }

    inline std::string sf_url_encode(const std::string &str)
    {
        std::string strTemp = "";
        size_t length = str.length();
        for (size_t i = 0; i < length; i++)
        {
            if (isalnum((unsigned char)str[i]) ||
                (str[i] == '-') ||
                (str[i] == '_') ||
                (str[i] == '.') ||
                (str[i] == '~'))
                strTemp += str[i];
            else if (str[i] == ' ')
                strTemp += "+";
            else
            {
                strTemp += '%';
                strTemp += sf_to_hex((unsigned char)str[i] >> 4);
                strTemp += sf_to_hex((unsigned char)str[i] % 16);
            }
        }
        return strTemp;
    }

    inline std::string sf_url_decode(const std::string &str)
    {
        std::string strTemp = "";
        size_t length = str.length();
        for (size_t i = 0; i < length; i++)
        {
            if (str[i] == '+') strTemp += ' ';
            else if (str[i] == '%')
            {
                unsigned char high = sf_from_hex((unsigned char)str[++i]);
                unsigned char low = sf_from_hex((unsigned char)str[++i]);
                strTemp += high*16 + low;
            }
            else strTemp += str[i];
        }
        return strTemp;
    }


    void sf_parse_param(std::map<std::string, std::string> param,std::string &param_str) {
        param.clear();
        unsigned long url_pos;
        while((url_pos = param_str.find('&')) != std::string::npos)
        {
            auto tmp_param = std::string(param_str.begin(),param_str.begin()+url_pos);
            param_str = std::string(param_str.begin()+url_pos+1,param_str.end());
            if(tmp_param.empty())
                continue;
            if((url_pos = tmp_param.find('=')) == std::string::npos)
                continue;
            auto key = sf_url_decode(std::string(tmp_param.begin(), tmp_param.begin() + url_pos));
            auto value = sf_url_decode(std::string(tmp_param.begin() + url_pos + 1, tmp_param.end()));
            param[key] = value;
        }
        if(param_str.empty())
            return;
        if((url_pos = param_str.find('=')) == std::string::npos)
            return;
        auto key = sf_url_decode(std::string(param_str.begin(), param_str.begin() + url_pos));
        auto value = sf_url_decode(std::string(param_str.begin() + url_pos + 1, param_str.end()));
        param[key] = value;
    }


    inline void sf_parse_url(const std::string &raw_url, std::string &url, std::map<std::string,std::string>& param,
                             std::string frame)
    {
        auto frame_pos = raw_url.find('#');
        std::string raw_url_without_frame;
        if(frame_pos == std::string::npos) {
            raw_url_without_frame = raw_url;
            frame = "";
        }
        else{
            raw_url_without_frame = std::string(raw_url.begin(),raw_url.begin()+frame_pos);
            frame = std::string(raw_url.begin()+frame_pos+1,raw_url.end());
        }
        auto url_pos = raw_url_without_frame.find('?');
        if(url_pos == std::string::npos){
            url = raw_url_without_frame;
            return;
        }
        url = std::string(raw_url_without_frame.begin(),raw_url_without_frame.begin()+url_pos);
        auto param_str = std::string(raw_url_without_frame.begin()+url_pos+1,raw_url_without_frame.end());
        sf_parse_param(param, param_str);
    }



    inline std::string sf_make_http_time_str()
    {
        std::time_t raw_time;
        std::time(&raw_time);
        std::tm *time_info = std::localtime(&raw_time);
        std::string ret(128,'\0');
        strftime(ret.data(),128,"%a, %d %b %Y %T GMT",time_info);
        ret.resize(strlen(ret.c_str()));
        return ret;
    }

    inline std::string sf_to_header_key_format(std::string key) {
        bool flag = false;
        for (auto &k:key) {
            if (0 != isalnum(k)) {
                if (!flag)
                    k = static_cast<char>(toupper(k));
                flag = true;
            } else {
                flag = false;
            }
        }
        return key;
    }
}