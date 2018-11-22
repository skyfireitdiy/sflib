#pragma once

#include "sf_grammer_utils.h"
#include <vector>
#include <unordered_set>

namespace skyfire
{
    /**
     * @brief 词法分析器
     */
    class sf_lex{
    private:
        std::vector<sf_lex_rule_t> rules__;

    public:
        /**
         * 添加规则
         * @param rule 规则
         */
        void add_rule(const sf_lex_rule_t& rule);

        /**
         * 批量添加规则
         * @param rule 规则集合
         */
        void add_rules(const std::vector<sf_lex_rule_t> &rule);

        /**
         * 解析
         * @param content 内容
         * @param result 结果
         * @return 是否解析成功
         */
        bool parse(std::string content, std::vector<sf_lex_result_t> &result);

        /**
         * 获取规则
         * @return 规则
         */
        std::vector<sf_lex_rule_t> get_rules() const;

        /**
         * 获取终结符
         * @return 终结符
         */
        std::unordered_set<std::string> get_real_term() const;
    };


}