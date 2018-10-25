#pragma once

#include "sf_yacc.h"

namespace skyfire
{
    inline void sf_yacc::add_rule(const sf_yacc_rule &rule)
    {
        rules__.push_back(rule);
    }

    inline std::vector<std::shared_ptr<sf_yacc_result_t>> sf_yacc::parse(const std::vector<sf_lex_result_t> &lex_result)
    {
        std::vector<std::shared_ptr<sf_yacc_result_t>> yacc_result_vec;
        for (auto &p:lex_result)
        {
            auto node = std::make_shared<sf_yacc_result_t>();
            node->id = p.id;
            node->text = p.matched_str;
            yacc_result_vec.push_back(node);
            auto new_result = yacc_result_vec;
            reduct__(yacc_result_vec);
        }
        return yacc_result_vec;
    }

    inline void sf_yacc::reduct__(std::vector<std::shared_ptr<sf_yacc_result_t>> &result)
    {
        bool flag = true;
        while (flag)
        {
            flag = false;
            for (auto &p:rules__)
            {
                for (auto &q:p.rules)
                {
                    if (q.rule.size() > result.size())
                        continue;
                    auto result_begin = result.end() - q.rule.size();
                    auto result_end = result.end();
                    if (rule_matched__(q.rule.begin(), q.rule.end(), result_begin, result_end))
                    {
                        auto node = std::make_shared<sf_yacc_result_t>();
                        node->id = p.id;
                        node->child = {result_begin, result_end};
                        for (auto &k:node->child)
                        {
                            node->text += k->text;
                        }
                        if (q.callback)
                        {
                            node->user_data = q.callback(node->child);
                        }
                        result.erase(result_begin, result_end);
                        result.push_back(node);
                        flag = true;
                    }
                }
            }
        }
    }

    inline bool sf_yacc::rule_matched__(std::vector<std::string>::iterator str_begin,
                                        std::vector<std::string>::iterator str_end,
                                        std::vector<std::shared_ptr<sf_yacc_result_t>>::iterator result_begin,
                                        std::vector<std::shared_ptr<sf_yacc_result_t>>::iterator result_end)
    {
        while (str_begin != str_end && result_begin != result_end)
        {
            if (*str_begin != (*result_begin)->id)
            {
                return false;
            }
            ++str_begin;
            ++result_begin;
        }
        return true;
    }

    void sf_yacc::add_rules(const std::vector<sf_yacc_rule> &rules)
    {
        rules__.insert(rules__.end(), rules.begin(), rules.end());
    }

}