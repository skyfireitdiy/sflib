#pragma once

#include "sf_grammer_utils.h"
#include <vector>
#include <set>

namespace skyfire
{
    /**
     * @brief 语法分析器
     */
    class sf_yacc
    {
        std::vector<sf_yacc_rule> rules__;
        std::set<std::string> terminate_ids__;

        static int
        rule_matched__(const std::vector<std::string> &rules,
                       const std::vector<std::shared_ptr<sf_yacc_result_t>> &results);

    public:

        /**
         * 添加语法规则
         * @param rule 语法规则
         */
        void add_rule(const sf_yacc_rule &rule);

        /**
         * 批量添加语法规则
         * @param rules 规则集合
         */
        void add_rules(const std::vector<sf_yacc_rule> &rules);

        /**
         * 添加终止规则
         * @param id 终止规则id
         */
        void add_terminate_id(const std::string &id);

        /**
         * 批量添加终止规则
         * @param ids 终止规则id集合
         */
        void add_termanate_ids(const std::set<std::string> &ids);

        /**
         * 语法分析
         * @param lex_result 词法分析结果
         * @param yacc_result 语法分析结果
         * @return 是否分析完成
         */
        bool parse(const std::vector<sf_lex_result_t> &lex_result,
                   std::vector<std::shared_ptr<sf_yacc_result_t>> &yacc_result);

    };


}