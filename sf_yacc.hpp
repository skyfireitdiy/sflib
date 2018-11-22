#pragma once

#include "sf_yacc.h"
#include "sf_range.hpp"

#include <iostream>

using namespace std;

namespace skyfire
{
    inline void sf_yacc::add_rule(sf_yacc_rule rule)
    {
        for (auto &p:rule.rules)
        {
            if (!p.rule.empty())
            {
                p.rule.emplace_back(sf_yacc_end_mark);
            }
        }
        for (auto i:sf_range(rule.rules.size()))
        {
            if (rule.rules[i].rule.empty())
                continue;
            state_machine__.push_back({{{sf_yacc_begin_mark,nullptr},                       rule.rules[i].rule[0]},
                                       {rule.id + "_" + std::to_string(i) + "_0", nullptr}});
            for (auto j:sf_range(rule.rules[i].rule.size() - 1))
            {
                state_machine__.push_back(
                        {{{rule.id + "_" + std::to_string(i) + "_" + std::to_string(j),nullptr},     rule.rules[i].rule[j + 1]},
                         {rule.id + "_" + std::to_string(i) + "_" + std::to_string(j + 1), nullptr}});
            }
            state_machine__.back() = {{{rule.id + "_" + std::to_string(i) + "_" +
                                       std::to_string(rule.rules[i].rule.size() - 2), nullptr}, rule.rules[i].rule[
                                                                                              rule.rules[i].rule.size() -
                                                                                              1]},
                                      {rule.id,                                       rule.rules[i].callback}};
        }

        for (auto &p:rule.rules)
        {
            for (auto &q:p.rule)
            {
                term_words__.insert(q);
            }
        }
    }

    void sf_yacc::add_rules(const std::vector<sf_yacc_rule> &rules)
    {
        for (auto &p: rules)
        {
            add_rule(p);
        }
    }


    inline bool sf_yacc::parse(std::vector<sf_lex_result_t> lex_result,
                               std::vector<std::shared_ptr<sf_yacc_result_t>> &yacc_result)
    {
        auto new_machine = nfa_to_dfa(state_machine__, term_words__);

        cout << new_machine.size() << endl;

        auto dfa_str = dfa_to_string(new_machine);


        for (auto &p:state_machine__)
        {
            cout << p.first.first.id << "," << p.first.second << "-->" << p.second.id << endl;
        }

        cout << "--------graph-------" << endl;

        for (auto &p:state_machine__)
        {
            cout << "\"" << p.first.first.id << "\"->\"" << p.second.id << "\"[label=\"" << p.first.second << "\"]"
                 << endl;
        }

        cout << "=======================" << endl;
        for (auto &p:dfa_str)
        {
            cout << p.first.first << "," << p.first.second << "-->" << p.second.id << endl;
        }

        cout << "--------graph-------" << endl;

        for (auto &p:dfa_str)
        {
            cout << "\"" << p.first.first << "\"->\"" << p.second.id << "\"[label=\"" << p.first.second << "\"]"
                 << endl;
        }
    }

    inline void sf_yacc::add_terminate_id(const std::string &id)
    {
        terminate_ids__.insert(id);
    }

    inline void sf_yacc::add_terminate_ids(const std::set<std::string> &ids)
    {
        terminate_ids__.insert(ids.begin(), ids.end());
    }

    inline std::string sf_yacc::state_to_string(const std::set<sf_yacc_state_node_t> &state)
    {
        if (state.size() == 1)
        {
            return state.begin()->id;
        }
        std::string ret;
        for (auto &p:state)
        {
            ret += "_|" + p.id + "|_";
        }
        return ret;
    }


    inline std::vector<std::pair<std::pair<std::string, std::string>, sf_yacc_state_node_t>> sf_yacc::dfa_to_string(
            const std::vector<std::pair<std::pair<std::set<sf_yacc_state_node_t>, std::string>, std::set<sf_yacc_state_node_t>>> &dfa)
    {
        std::vector<std::pair<std::pair<std::string, std::string>, sf_yacc_state_node_t>> ret;
        for (auto &p:dfa)
        {
            auto src_str = state_to_string(p.first.first);
            auto des_str = state_to_string(p.second);
            std::pair<std::pair<std::string, std::string>, sf_yacc_state_node_t> new_record = {{src_str, p.first.second},
                                                                                               {des_str, nullptr}};
            if (p.second.size() == 1)
            {
                new_record.second.callback = p.second.begin()->callback;
            }
            ret.push_back(new_record);
        }
        return ret;
    }

    std::vector<std::pair<std::pair<std::set<sf_yacc_state_node_t>, std::string>, std::set<sf_yacc_state_node_t>>>
    sf_yacc::nfa_to_dfa(
            const std::vector<std::pair<std::pair<sf_yacc_state_node_t, std::string>, sf_yacc_state_node_t>> &old_machine,
            const std::unordered_set<std::string> &term_words)
    {
        std::vector<std::pair<std::pair<std::set<sf_yacc_state_node_t>, std::string>, std::set<sf_yacc_state_node_t>>> new_machine;

        std::vector<std::set<sf_yacc_state_node_t>> state;
        state.push_back({{sf_yacc_begin_mark, nullptr}});

        for (int i = 0; i < state.size(); ++i)
        {
            std::set<sf_yacc_state_node_t> state_backup = state[i];
            for (auto &q:state_backup)
            {
                for (auto &p:term_words)
                {
                    std::set<sf_yacc_state_node_t> new_state;
                    auto conv = std::pair < sf_yacc_state_node_t, std::string>(q, p);
                    auto old_iter = old_machine.begin()-1;
                    while ((old_iter = std::find_if(old_iter+1, old_machine.end(), [&](auto item)
                    {
                        return item.first == conv;
                    })) != old_machine.end())
                    {
                        new_state.insert(old_iter->second);
                    }
                    if (new_state.empty())
                        continue;
                    if (std::find(state.begin(), state.end(), new_state) == state.end())
                    {
                        state.push_back(new_state);
                    }
                    new_machine.push_back({{state[i], p}, new_state});
                }
            }
        }

        for(auto &p:term_words)
        {
            decltype(new_machine) new_rule;
            std::pair<std::set<sf_yacc_state_node_t>, std::string> equal_state {{{sf_yacc_begin_mark,nullptr}}, p};
            for(auto &q:new_machine)
            {
                if(q.first == equal_state)
                {
                    auto next_state = q.second;
                    for(auto &r:new_machine)
                    {
                        if(r.first.first == next_state)
                        {
                            new_rule.push_back({{{{p,nullptr}},r.first.second},r.second});
                        }
                    }
                    break;
                }
            }
            new_machine.insert(new_machine.end(),new_rule.begin(),new_rule.end());
        }

        return new_machine;
    }

}