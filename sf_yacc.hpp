#pragma once

#include "sf_yacc.h"

#include <iostream>

namespace skyfire
{
    inline void sf_yacc::add_rule(const sf_yacc_rule &rule)
    {
        rules__.push_back(rule);
    }

    inline std::vector<std::shared_ptr<sf_yacc_result_t>> sf_yacc::parse(const std::vector<sf_lex_result_t> &lex_result)
    {
        std::vector<std::pair<sf_yacc_rule_callback,std::string>> reduct_rules;
        for(auto &p:rules__)
        {
            for(auto &q:p.rules)
            {
                reduct_rules.emplace_back(q,p.id);
            }
        }
        // 长规则排前面
        std::sort(reduct_rules.begin(),reduct_rules.end(),[](const auto &a,const auto &b){
            return a.first.rule.size()>b.first.rule.size();
        });

        // 准备数据

        std::vector<std::shared_ptr<sf_yacc_result_t>> yacc_result;
        for(auto &p:lex_result)
        {
            auto node = std::make_shared<sf_yacc_result_t>();
            node->id = p.id;
            node->text = p.matched_str;
            yacc_result.push_back(node);
        }

        // 归约
        while(true)
        {
            if(yacc_result.size() <= 1)
            {
                break;
            }

            bool flag = false;
            for(auto &p:reduct_rules)
            {
                if(p.first.rule.size()>yacc_result.size())
                {
                    continue;
                }
                for(int i=0;i<yacc_result.size()-p.first.rule.size()+1;++i)
                {
                    auto begin_iter = yacc_result.begin()+i;
                    auto end_iter = begin_iter+p.first.rule.size();
                    if(rule_matched__(p.first.rule.begin(),p.first.rule.end(),begin_iter,end_iter))
                    {
                        flag = true;
                        auto new_node = std::make_shared<sf_yacc_result_t>();
                        new_node->id = p.second;
                        for(auto iter=begin_iter;iter!=end_iter;++iter)
                        {
                            new_node->text+=(*iter)->text;
                        }
                        new_node->child = {begin_iter,end_iter};
                        if(p.first.callback)
                        {
                            new_node->user_data = p.first.callback(new_node->child);
                        }
                        yacc_result.erase(begin_iter,end_iter);
                        yacc_result.insert(yacc_result.begin()+i,new_node);
                        break;
                    }
                }
                if(flag)
                {
                    break;
                }
            }

            if(!flag)
            {
                break;
            }
        }
        return yacc_result;
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