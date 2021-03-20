
#pragma once
#include "sf_compiler_utils.h"
#include "sf_stdc++.h"
namespace skyfire
{
class yacc
{
    std::unordered_set<std::string> terminate_ids__;
    std::vector<
        std::pair<std::pair<std::string, std::string>, yacc_state_node_t>>
                                    dfa__;
    std::unordered_set<std::string> term_words__;
    static std::vector<
        std::pair<std::pair<std::set<yacc_state_node_t>, std::string>,
                  std::set<yacc_state_node_t>>>
    nfa_to_dfa(const std::vector<
                   std::pair<std::pair<yacc_state_node_t, std::string>,
                             yacc_state_node_t>>&     old_machine,
               const std::unordered_set<std::string>& term_words);
    static std::vector<
        std::pair<std::pair<std::string, std::string>, yacc_state_node_t>>
    dfa_optimize(
        const std::vector<
            std::pair<std::pair<std::set<yacc_state_node_t>, std::string>,
                      std::set<yacc_state_node_t>>>& dfa);
    static std::string state_to_string(
        const std::set<yacc_state_node_t>& state);
    static std::vector<std::pair<std::pair<yacc_state_node_t, std::string>,
                                 yacc_state_node_t>>
                                           make_nfa(const std::vector<yacc_rule>& rules);
    static std::unordered_set<std::string> make_term_words(
        const std::vector<yacc_rule>& rules);
    static std::vector<std::shared_ptr<yacc_result_t>>
    make_yacc_result_from_lex_result(
        const std::vector<lex_result_t>& lex_result);
    static bool self_reduce_two(
        std::vector<std::shared_ptr<yacc_result_t>>&     result,
        const std::vector<std::pair<std::pair<std::string, std::string>,
                                    yacc_state_node_t>>& dfa,
        const std::unordered_set<std::string>&           term_words);
    static bool self_reduce_one(
        std::vector<std::shared_ptr<yacc_result_t>>&     result,
        const std::vector<std::pair<std::pair<std::string, std::string>,
                                    yacc_state_node_t>>& dfa);
    static bool reduce_new_node(
        std::vector<std::shared_ptr<yacc_result_t>>&     result,
        const std::shared_ptr<yacc_result_t>&            r_node,
        const std::vector<std::pair<std::pair<std::string, std::string>,
                                    yacc_state_node_t>>& dfa,
        const std::unordered_set<std::string>&           term_words);
    static bool add_new_node(
        std::vector<std::shared_ptr<yacc_result_t>>&     result,
        const std::shared_ptr<yacc_result_t>&            r_node,
        const std::vector<std::pair<std::pair<std::string, std::string>,
                                    yacc_state_node_t>>& dfa);

public:
    void set_rules(const std::vector<yacc_rule>& rules);
    void add_terminate_ids(const std::unordered_set<std::string>& ids);
    bool parse(
        const std::vector<lex_result_t>&             lex_result,
        std::vector<std::shared_ptr<yacc_result_t>>& yacc_result) const;
};
} // namespace skyfire