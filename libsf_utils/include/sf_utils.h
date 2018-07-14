#pragma once

#include <string>

namespace skyfire
{
    inline std::string sf_string_trim(const std::string& str)
    {
        if(str.length() == 0)
            return "";
        unsigned long begin = 0;
        for(unsigned long i = 0;i<str.length();++i)
        {
            if(!isspace(str[i]))
            {
                begin = i;
                break;
            }
        }
        unsigned long end = begin;
        for(unsigned long i = str.length();i>begin;--i)
        {
            if(!isspace(str[i-1]))
            {
                end = i;
                break;
            }
        }
        return {str.begin()+begin, str.begin()+end};
    }
}