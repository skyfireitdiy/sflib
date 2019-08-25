#pragma once

#include "sf_yacc.h"

namespace skyfire {
inline void sf_yacc::set_rules(const std::vector<sf_yacc_rule> &rules) {
    term_words__ = make_term_words(rules);
    dfa__ = dfa_optimize(nfa_to_dfa(make_nfa(rules), term_words__));
}

inline bool sf_yacc::parse(
    const std::vector<sf_lex_result_t> &lex_result,
    std::vector<std::shared_ptr<sf_yacc_result_t>> &yacc_result) const {
#if 0
		for(auto &p:dfa__)
		{
			printf(R"("%s"->"%s"[label="%s"])" "\n", p.first.first.c_str(), p.second.id.c_str(), p.first.second.c_str());
		}
#endif

    auto prepare_items = make_yacc_result_from_lex_result(lex_result);
    yacc_result.clear();

    // NOTE 这里故意不+1
    for (auto i = 0; i < prepare_items.size();) {
#if 0
			for (auto &p : yacc_result)
			{
				printf(R"(%s[%s])", p->id.c_str(), p->text.c_str());
			}
			printf("\n");
#endif

        if (reduce_new_node(yacc_result, prepare_items[i], dfa__,
                            term_words__)) {
            ++i;
            // while (self_reduce_two(yacc_result, dfa__, term_words__));
            continue;
        }
        if (self_reduce_two(yacc_result, dfa__, term_words__)) {
            // while (self_reduce_two(yacc_result, dfa__, term_words__));
            continue;
        }
        if (self_reduce_one(yacc_result, dfa__)) {
            // while (self_reduce_two(yacc_result, dfa__, term_words__));
            continue;
        }
        if (add_new_node(yacc_result, prepare_items[i], dfa__)) {
            ++i;
            // while (self_reduce_two(yacc_result, dfa__, term_words__));
            continue;
        }
        return false;
    }

#if 0
        for (auto &p : yacc_result)
        {
            printf(R"(%s[%s])", p->id.c_str(), p->text.c_str());
        }
        printf("\n");
#endif

    while (yacc_result.size() != 1) {
#if 0
            for (auto &p : yacc_result)
            {
                printf(R"(%s[%s])", p->id.c_str(), p->text.c_str());
            }
            printf("\n");
#endif
        if (self_reduce_two(yacc_result, dfa__, term_words__)) continue;
        if (self_reduce_one(yacc_result, dfa__)) continue;
        return false;
    }

    while (terminate_ids__.count(yacc_result[0]->id) == 0) {
#if 0
            for (auto &p : yacc_result)
            {
                printf(R"(%s[%s])", p->id.c_str(), p->text.c_str());
            }
            printf("\n");
#endif
        if (!self_reduce_one(yacc_result, dfa__)) return false;
    }
    return true;
}

inline void sf_yacc::add_terminate_ids(
    const std::unordered_set<std::string> &ids) {
    terminate_ids__ = ids;
}

inline std::string sf_yacc::state_to_string(
    const std::set<sf_yacc_state_node_t> &state) {
    if (state.size() == 1) {
        return state.begin()->id;
    }
    std::string ret;
    for (auto &p : state) {
        ret += "_|" + p.id + "|_";
    }
    return ret;
}

inline std::vector<
    std::pair<std::pair<std::string, std::string>, sf_yacc_state_node_t>>
sf_yacc::dfa_optimize(
    const std::vector<
        std::pair<std::pair<std::set<sf_yacc_state_node_t>, std::string>,
                  std::set<sf_yacc_state_node_t>>> &dfa) {
    std::vector<
        std::pair<std::pair<std::string, std::string>, sf_yacc_state_node_t>>
        ret;
    for (auto &p : dfa) {
        auto src_str = state_to_string(p.first.first);
        auto des_str = state_to_string(p.second);
        std::pair<std::pair<std::string, std::string>, sf_yacc_state_node_t>
            new_record = {{src_str, p.first.second}, {des_str, nullptr}};
        if (p.second.size() == 1) {
            new_record.second.callback = p.second.begin()->callback;
        }
        ret.push_back(new_record);
    }
    return ret;
}

inline std::vector<
    std::pair<std::pair<std::set<sf_yacc_state_node_t>, std::string>,
              std::set<sf_yacc_state_node_t>>>
sf_yacc::nfa_to_dfa(
    const std::vector<std::pair<std::pair<sf_yacc_state_node_t, std::string>,
                                sf_yacc_state_node_t>> &old_machine,
    const std::unordered_set<std::string> &term_words) {
    std::vector<
        std::pair<std::pair<std::set<sf_yacc_state_node_t>, std::string>,
                  std::set<sf_yacc_state_node_t>>>
        new_machine;

    std::vector<std::set<sf_yacc_state_node_t>> state;
    state.reserve(term_words.size());
    for (auto &p : term_words) {
        state.push_back({{p, nullptr}});
    }

    for (auto i = 0; i < state.size(); ++i) {
        std::set<sf_yacc_state_node_t> state_backup = state[i];
        for (auto &q : state_backup) {
            for (auto &p : term_words) {
                std::set<sf_yacc_state_node_t> new_state;
                auto conv = std::pair<sf_yacc_state_node_t, std::string>(q, p);
                const auto conv_find = [&](auto item) {
                    return item.first == conv;
                };
                auto old_iter = old_machine.end();
                if ((old_iter = std::find_if(old_machine.begin(),
                                             old_machine.end(), conv_find)) !=
                    old_machine.end()) {
                    new_state.insert(old_iter->second);
                    while ((old_iter = std::find_if(
                                old_iter + 1, old_machine.end(), conv_find)) !=
                           old_machine.end()) {
                        new_state.insert(old_iter->second);
                    }
                }
                if (!new_state.empty()) {
                    if (std::find(state.begin(), state.end(), new_state) ==
                        state.end()) {
                        state.push_back(new_state);
                    }
                    new_machine.push_back({{state[i], p}, new_state});
                }
            }
        }
    }
    return new_machine;
}

inline std::vector<std::pair<std::pair<sf_yacc_state_node_t, std::string>,
                             sf_yacc_state_node_t>>
sf_yacc::make_nfa(const std::vector<sf_yacc_rule> &rules) {
    std::vector<std::pair<std::pair<sf_yacc_state_node_t, std::string>,
                          sf_yacc_state_node_t>>
        nfa;
    for (auto rule : rules) {
        for (auto &p : rule.rules) {
            if (p.rule.size() == 1) {
                p.rule.emplace_back(sf_yacc_end_mark);
            }
        }
        for (auto i = 0; i < rule.rules.size(); ++i) {
            if (rule.rules[i].rule.empty()) continue;
            for (auto j = 0; j < rule.rules[i].rule.size() - 1; ++j) {
                nfa.push_back({{{(j == 0 ? rule.rules[i].rule[0]
                                         : rule.id + "_" + std::to_string(i) +
                                               "_" + std::to_string(j)),
                                 nullptr},
                                rule.rules[i].rule[j + 1]},
                               {rule.id + "_" + std::to_string(i) + "_" +
                                    std::to_string(j + 1),
                                nullptr}});
            }
            nfa.back() = {
                {{(rule.rules[i].rule.size() == 2
                       ? rule.rules[i].rule[0]
                       : rule.id + "_" + std::to_string(i) + "_" +
                             std::to_string(rule.rules[i].rule.size() - 2)),
                  nullptr},
                 rule.rules[i].rule[rule.rules[i].rule.size() - 1]},
                {rule.id, rule.rules[i].callback}};
        }
    }
    return nfa;
}

inline std::unordered_set<std::string> sf_yacc::make_term_words(
    const std::vector<sf_yacc_rule> &rules) {
    std::unordered_set<std::string> ret;
    for (auto &p : rules) {
        for (auto &q : p.rules) {
            for (auto &r : q.rule) {
                ret.insert(r);
            }
        }
    }
    ret.insert(sf_yacc_end_mark);
    return ret;
}

inline std::vector<std::shared_ptr<sf_yacc_result_t>>
sf_yacc::make_yacc_result_from_lex_result(
    const std::vector<sf_lex_result_t> &lex_result) {
    std::vector<std::shared_ptr<sf_yacc_result_t>> ret;
    for (auto &p : lex_result) {
        auto tmp = std::make_shared<sf_yacc_result_t>();
        tmp->id = p.id;
        tmp->text = p.matched_str;
        ret.emplace_back(tmp);
    }
    return ret;
}

inline bool sf_yacc::self_reduce_one(
    std::vector<std::shared_ptr<sf_yacc_result_t>> &result,
    const std::vector<std::pair<std::pair<std::string, std::string>,
                                sf_yacc_state_node_t>> &dfa) {
    if (result.empty()) return false;
    auto left = result.back();
    auto conv = std::make_pair(left->id, std::string(sf_yacc_end_mark));
    auto iter = std::find_if(dfa.begin(), dfa.end(),
                             [&](auto &item) { return item.first == conv; });
    if (iter == dfa.end()) return false;
    auto new_node = std::make_shared<sf_yacc_result_t>();
    new_node->id = iter->second.id;
    new_node->text = left->text;
    new_node->children.push_back(left);
    new_node->user_data = left->user_data;
    if (iter->second.callback) {
        new_node->user_data = iter->second.callback(new_node->children);
    }
    result.resize(result.size() - 1);
    result.push_back(new_node);
    return true;
}

inline bool sf_yacc::self_reduce_two(
    std::vector<std::shared_ptr<sf_yacc_result_t>> &result,
    const std::vector<std::pair<std::pair<std::string, std::string>,
                                sf_yacc_state_node_t>> &dfa,
    const std::unordered_set<std::string> &term_words) {
    if (result.size() < 2) return false;
    auto left = *(result.end() - 2);
    auto right = *(result.end() - 1);
    auto conv = std::make_pair(left->id, right->id);
    auto iter = std::find_if(dfa.begin(), dfa.end(),
                             [&](auto &item) { return item.first == conv; });
    if (iter == dfa.end()) return false;
    auto new_node = std::make_shared<sf_yacc_result_t>();
    new_node->id = iter->second.id;
    new_node->text = left->text + right->text;
    if (term_words.count(left->id) == 0) {
        new_node->children = left->children;
    } else {
        new_node->children.push_back(left);
    }
    new_node->children.push_back(right);
    if (iter->second.callback) {
        new_node->user_data = iter->second.callback(new_node->children);
    }
    result.resize(result.size() - 2);
    result.push_back(new_node);
    return true;
}

inline bool sf_yacc::reduce_new_node(
    std::vector<std::shared_ptr<sf_yacc_result_t>> &result,
    const std::shared_ptr<sf_yacc_result_t> &r_node,
    const std::vector<std::pair<std::pair<std::string, std::string>,
                                sf_yacc_state_node_t>> &dfa,
    const std::unordered_set<std::string> &term_words) {
    if (result.empty()) return false;
    auto left = result.back();
    const auto &right = r_node;
    auto conv = std::make_pair(left->id, right->id);
    auto iter = std::find_if(dfa.begin(), dfa.end(),
                             [&](auto &item) { return item.first == conv; });
    if (iter == dfa.end()) return false;
    auto new_node = std::make_shared<sf_yacc_result_t>();
    new_node->id = iter->second.id;
    new_node->text = left->text + right->text;
    if (term_words.count(left->id) == 0) {
        new_node->children = left->children;
    } else {
        new_node->children.push_back(left);
    }
    new_node->children.push_back(right);
    if (iter->second.callback) {
        new_node->user_data = iter->second.callback(new_node->children);
    }
    result.resize(result.size() - 1);
    result.push_back(new_node);
    return true;
}

inline bool sf_yacc::add_new_node(
    std::vector<std::shared_ptr<sf_yacc_result_t>> &result,
    const std::shared_ptr<sf_yacc_result_t> &r_node,
    const std::vector<std::pair<std::pair<std::string, std::string>,
                                sf_yacc_state_node_t>> &dfa) {
    auto iter = std::find_if(dfa.begin(), dfa.end(), [&](auto &item) {
        return item.first.first == r_node->id;
    });
    if (iter == dfa.end()) return false;
    result.push_back(r_node);
    return true;
}

}    // namespace skyfire