#pragma once

#include "core/sf_stdc++.h"
#include "sf_grammer_utils.h"

namespace skyfire {
/**
 * @brief 语法分析器
 */
class sf_yacc {
    std::unordered_set<std::string> terminate_ids__;
    std::vector<
        std::pair<std::pair<std::string, std::string>, sf_yacc_state_node_t>>
        dfa__;
    std::unordered_set<std::string> term_words__;

    static std::vector<
        std::pair<std::pair<std::set<sf_yacc_state_node_t>, std::string>,
                  std::set<sf_yacc_state_node_t>>>
    nfa_to_dfa(const std::vector<
                   std::pair<std::pair<sf_yacc_state_node_t, std::string>,
                             sf_yacc_state_node_t>> &old_machine,
               const std::unordered_set<std::string> &term_words);

    static std::vector<
        std::pair<std::pair<std::string, std::string>, sf_yacc_state_node_t>>
    dfa_optimize(
        const std::vector<
            std::pair<std::pair<std::set<sf_yacc_state_node_t>, std::string>,
                      std::set<sf_yacc_state_node_t>>> &dfa);

    static std::string state_to_string(
        const std::set<sf_yacc_state_node_t> &state);

    static std::vector<std::pair<std::pair<sf_yacc_state_node_t, std::string>,
                                 sf_yacc_state_node_t>>
    make_nfa(const std::vector<sf_yacc_rule> &rules);

    static std::unordered_set<std::string> make_term_words(
        const std::vector<sf_yacc_rule> &rules);

    static std::vector<std::shared_ptr<sf_yacc_result_t>>
    make_yacc_result_from_lex_result(
        const std::vector<sf_lex_result_t> &lex_result);

    static bool self_reduce_two(
        std::vector<std::shared_ptr<sf_yacc_result_t>> &result,
        const std::vector<std::pair<std::pair<std::string, std::string>,
                                    sf_yacc_state_node_t>> &dfa,
        const std::unordered_set<std::string> &term_words);

    static bool self_reduce_one(
        std::vector<std::shared_ptr<sf_yacc_result_t>> &result,
        const std::vector<std::pair<std::pair<std::string, std::string>,
                                    sf_yacc_state_node_t>> &dfa);

    static bool reduce_new_node(
        std::vector<std::shared_ptr<sf_yacc_result_t>> &result,
        const std::shared_ptr<sf_yacc_result_t> &r_node,
        const std::vector<std::pair<std::pair<std::string, std::string>,
                                    sf_yacc_state_node_t>> &dfa,
        const std::unordered_set<std::string> &term_words);

    static bool add_new_node(
        std::vector<std::shared_ptr<sf_yacc_result_t>> &result,
        const std::shared_ptr<sf_yacc_result_t> &r_node,
        const std::vector<std::pair<std::pair<std::string, std::string>,
                                    sf_yacc_state_node_t>> &dfa);

   public:
    /**
     * 设置语法规则
     * @param rules 规则集合
     */
    void set_rules(const std::vector<sf_yacc_rule> &rules);

    /**
     * 批量添加终止规则
     * @param ids 终止规则id集合
     */
    void add_terminate_ids(const std::unordered_set<std::string> &ids);

    /**
     * 语法分析
     * @param lex_result 词法分析结果
     * @param yacc_result 语法分析结果
     * @return 是否分析完成
     */
    bool parse(
        const std::vector<sf_lex_result_t> &lex_result,
        std::vector<std::shared_ptr<sf_yacc_result_t>> &yacc_result) const;
};

}    // namespace skyfire