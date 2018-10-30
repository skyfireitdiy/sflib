#pragma once

#include "sf_yacc.h"

namespace skyfire
{
    inline void sf_yacc::add_rule(const sf_yacc_rule &rule)
    {
        rules__.push_back(rule);
    }

    void sf_yacc::add_rules(const std::vector<sf_yacc_rule> &rules)
    {
        rules__.insert(rules__.end(), rules.begin(), rules.end());
    }

    inline int
    sf_yacc::rule_matched__(const std::vector<std::string> &rules,
                            const std::vector<std::shared_ptr<sf_yacc_result_t>> &results)
    {
        if (rules.size() > results.size())
        {
            return -1;
        }
        std::map<std::string, int> src_data;
        for (auto i = 0; i < rules.size(); ++i)
        {
            src_data[rules[i]] = i;
        }

        auto pos = rules.size() - 1;
        while (pos < results.size())
        {
            if (rules.back() == results[pos]->id)
            {
                bool flag = true;
                for (auto i = 0; i < rules.size(); ++i)
                {
                    if (rules[i] != results[pos + i - rules.size() + 1]->id)
                    {
                        flag = false;
                        break;
                    }
                }
                if (flag)
                {
                    return static_cast<int>(pos + 1 - rules.size());
                }
            }
            while (true)
            {
                ++pos;
                if (pos >= results.size())
                {
                    return -1;
                }
                if (src_data.count(results[pos]->id) != 0)
                {
                    pos += rules.size() - src_data[results[pos]->id] - 1;
                    break;
                }
            }
        }
        return -1;
    }

    inline bool sf_yacc::parse(const std::vector<sf_lex_result_t> &lex_result,
                               std::vector<std::shared_ptr<sf_yacc_result_t>> &yacc_result)
    {
        std::vector<std::pair<sf_yacc_rule_callback, std::string>> reduct_rules;
        for (auto &p:rules__)
        {
            for (auto &q:p.rules)
            {
                reduct_rules.emplace_back(q, p.id);
            }
        }
        // 长规则排前面
        std::sort(reduct_rules.begin(), reduct_rules.end(), [](const auto &a, const auto &b)
        {
            return a.first.rule.size() > b.first.rule.size();
        });

        // 准备数据

        yacc_result.clear();
        for (auto &p:lex_result)
        {
            auto node = std::make_shared<sf_yacc_result_t>();
            node->id = p.id;
            node->text = p.matched_str;
            yacc_result.push_back(node);
        }

        // 归约
        while (true)
        {
            if (yacc_result.size() == 1)
            {
                if (terminate_ids__.count(yacc_result.front()->id) != 0)
                {
                    return true;
                }
            }
            bool flag = false;
            for (auto &p:reduct_rules)
            {
                if (p.first.rule.size() > yacc_result.size())
                {
                    continue;
                }
                auto pos = rule_matched__(p.first.rule, yacc_result);
                if (pos != -1)
                {
                    auto begin_iter = yacc_result.begin() + pos;
                    auto end_iter = yacc_result.begin() + pos + p.first.rule.size();
                    flag = true;
                    auto new_node = std::make_shared<sf_yacc_result_t>();
                    new_node->id = p.second;
                    for (auto iter = begin_iter; iter != end_iter; ++iter)
                    {
                        new_node->text += (*iter)->text;
                    }
                    new_node->children = {begin_iter, end_iter};
                    if (p.first.callback)
                    {
                        new_node->user_data = p.first.callback(new_node->children);
                    }
                    yacc_result.erase(begin_iter, end_iter);
                    yacc_result.insert(yacc_result.begin() + pos, new_node);

                    break;
                }
            }
            if (!flag)
            {
                break;
            }
        }
        return false;
    }

    inline void sf_yacc::add_terminate_id(const std::string &id)
    {
        terminate_ids__.insert(id);
    }

    inline void sf_yacc::add_termanate_ids(const std::set<std::string> &ids)
    {
        terminate_ids__.insert(ids.begin(), ids.end());
    }


}