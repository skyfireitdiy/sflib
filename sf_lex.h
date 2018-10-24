#pragma once

#include "sf_grammer_utils.h"
#include <vector>

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
         * 解析
         * @param content 内容
         * @param result 结果
         * @return 是否解析成功
         */
        bool parse(std::string content, std::vector<sf_lex_result_t> &result);
    };


}