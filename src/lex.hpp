
#pragma once
#include "lex.h"
#include "stdc++.h"
namespace skyfire
{
inline bool lex::parse(std::string                content,
                       std::vector<lex_result_t>& result)
{
    result.clear();
    auto old_len = content.length();
    while (!content.empty())
    {
        auto         flag = false;
        lex_result_t tmp_result {};
        std::smatch  result_sm;
        for (auto& p : rules__)
        {
            std::regex  token_regex(p.regex_str);
            std::smatch sm;
            if (std::regex_search(content, sm, token_regex,
                                  std::regex_constants::match_continuous))
            {
                if (flag)
                {
                    if (tmp_result.matched_str.length() < static_cast<size_t>(sm[0].length()))
                    {
                        tmp_result.id          = p.id;
                        tmp_result.matched_str = sm[0].str();
                        result_sm              = sm;
                    }
                }
                else
                {
                    tmp_result.id          = p.id;
                    tmp_result.matched_str = sm[0].str();
                    result_sm              = sm;
                }
                flag = true;
            }
        }
        if (!flag)
        {
            return false;
        }
        content = result_sm.suffix();
        if (content.length() == old_len)
        {
            return false;
        }
        result.push_back(tmp_result);
    }
    return true;
}
inline void lex::set_rules(const std::vector<lex_rule_t>& rule)
{
    rules__ = rule;
}
inline std::vector<lex_rule_t>         lex::rules() const { return rules__; }
inline std::unordered_set<std::string> lex::real_term() const
{
    std::unordered_set<std::string> ret;
    for (auto& p : rules__)
    {
        ret.insert(p.id);
    }
    return ret;
}
} // namespace skyfire
