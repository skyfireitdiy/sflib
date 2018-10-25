#pragma once

#include "sf_grammer_utils.h"
#include <vector>

namespace skyfire
{
    /**
     * @brief 语法分析器
     */
    class sf_yacc
    {
        std::vector<sf_yacc_rule> rules__;


        static bool rule_matched__(std::vector<std::string>::iterator str_begin,
                                   std::vector<std::string>::iterator str_end,
                                   std::vector<std::shared_ptr<sf_yacc_result_t>>::iterator result_begin,
                                   std::vector<std::shared_ptr<sf_yacc_result_t>>::iterator result_end
        );

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
         * 解析语法
         * @param lex_result 词法分析结果
         * @return 语法分析结果
         */
        std::vector<std::shared_ptr<sf_yacc_result_t>> parse(const std::vector<sf_lex_result_t> &lex_result);

    };


}