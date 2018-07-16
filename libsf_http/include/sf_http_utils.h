#pragma once

#include <string>
#include <ctime>

namespace skyfire
{
    inline unsigned char sf_to_hex(unsigned char x)
    {
        return  x > 9 ? x + 55 : x + 48;
    }

    inline unsigned char sf_from_hex(unsigned char x)
    {
        unsigned char y;
        if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
        else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
        else if (x >= '0' && x <= '9') y = x - '0';
        else assert(0);
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
                assert(i + 2 < length);
                unsigned char high = sf_from_hex((unsigned char)str[++i]);
                unsigned char low = sf_from_hex((unsigned char)str[++i]);
                strTemp += high*16 + low;
            }
            else strTemp += str[i];
        }
        return strTemp;
    }


    inline void sf_parse_url(const std::string &raw_url, std::string &url, std::map<std::string,std::string>& param)
    {
        auto pos = raw_url.find('?');
        param.clear();
        if(pos == -1){
            url = raw_url;
            return;
        }
        url = std::string(raw_url.begin(),raw_url.begin()+pos);
        auto param_str = std::string(raw_url.begin()+pos+1,raw_url.end());
        while((pos = param_str.find('&'))!=-1)
        {
            auto tmp_param = std::string(param_str.begin(),param_str.begin()+pos);
            param_str = std::string(param_str.begin()+pos+1,param_str.end());
            if(tmp_param.empty())
                continue;
            if(pos = tmp_param.find('=') == -1)
                continue;
            auto key = sf_url_decode(std::string(tmp_param.begin(),tmp_param.begin()+pos));
            auto value = sf_url_decode(std::string(tmp_param.begin()+pos+1,tmp_param.end()));
            param[key] = value;
        }
        if(param_str.empty())
            return;
        if(pos = param_str.find('=') == -1)
            return;
        auto key = sf_url_decode(std::string(param_str.begin(),param_str.begin()+pos));
        auto value = sf_url_decode(std::string(param_str.begin()+pos+1,param_str.end()));
        param[key] = value;
    }

    inline std::string sf_make_http_time_str()
    {
        std::time_t raw_time;
        std::time(&raw_time);
        std::tm *time_info = std::localtime(&raw_time);
        std::string ret(128,'\0');
        strftime(ret.data(),128,"%a, %d %b %Y %T GMT",time_info);
        return ret;
    }
}