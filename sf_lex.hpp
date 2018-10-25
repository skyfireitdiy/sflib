#pragma once


#include "sf_lex.h"
#include <regex>

namespace skyfire
{
    inline void sf_lex::add_rule(const sf_lex_rule_t &rule)
    {
        rules__.push_back(rule);
    }

    inline bool sf_lex::parse(std::string content, std::vector<sf_lex_result_t> &result)
    {
        result.clear();
        auto old_len = content.length();
        while(!content.empty())
        {
            bool flag = false;
            sf_lex_result_t tmp_result{};
            std::smatch result_sm;
            for(auto &p:rules__)
            {
                std::regex token_regex(p.regex_str);
                std::smatch sm;
                if(std::regex_search(content, sm, token_regex, std::regex_constants::match_continuous))
                {
                    if(flag)
                    {
                        if(tmp_result.matched_str.length()<sm[0].length())
                        {
                            tmp_result.id = p.id;
                            tmp_result.matched_str = sm[0].str();
                            result_sm = sm;
                        }
                    }
                    else
                    {
                        tmp_result.id = p.id;
                        tmp_result.matched_str = sm[0].str();
                        result_sm = sm;
                    }
                    flag = true;
                }
            }
            if(!flag)
            {
                return false;
            }
            content = result_sm.suffix();
            if(content.length() == old_len)
            {
                return false;
            }
            result.push_back(tmp_result);
        }
        return true;
    }

    inline void sf_lex::add_rules(const std::vector<sf_lex_rule_t> &rule)
    {
        rules__.insert(rules__.end(), rule.begin(), rule.end());
    }
}