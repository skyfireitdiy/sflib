#pragma once

#include "sf_grammer_utils.h"
#include <vector>
#include <set>
#include <unordered_set>


namespace skyfire
{
    /**
     * @brief 语法分析器
     */
    class sf_yacc
    {

        std::set<std::string> terminate_ids__;
        std::vector<std::pair<std::pair<std::string, std::string>, sf_yacc_state_node_t>> dfa__;

        static std::vector<std::pair<std::pair<std::set<sf_yacc_state_node_t>, std::string>, std::set<sf_yacc_state_node_t>>> nfa_to_dfa(
                const std::vector<std::pair<std::pair<sf_yacc_state_node_t, std::string>, sf_yacc_state_node_t>> &old_machine,
                const std::unordered_set<std::string> &term_words);

        static std::vector<std::pair<std::pair<std::string, std::string>, sf_yacc_state_node_t>> dfa_optimization(
                const std::vector<std::pair<std::pair<std::set<sf_yacc_state_node_t>, std::string>, std::set<sf_yacc_state_node_t>>> &dfa);

        static std::string state_to_string(const std::set<sf_yacc_state_node_t> &state);

        static std::vector<std::pair<std::pair<sf_yacc_state_node_t,std::string>, sf_yacc_state_node_t>> make_nfa(const std::vector<sf_yacc_rule> &rules);

        static std::unordered_set<std::string> make_term_words(const std::vector<sf_yacc_rule> &rules);

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
        void add_terminate_ids(const std::set <std::string> &ids);

        /**
         * 语法分析
         * @param lex_result 词法分析结果
         * @param yacc_result 语法分析结果
         * @return 是否分析完成
         */
        bool parse(std::vector<sf_lex_result_t> lex_result,
                   std::vector<std::shared_ptr<sf_yacc_result_t>> &yacc_result);

    };


}