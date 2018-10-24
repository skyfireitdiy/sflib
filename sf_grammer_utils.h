#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace skyfire
{
    /**
     * @brief 词法规则
     */
    struct sf_lex_rule_t{
        std::string id;
        std::string regex_str;
    };

    /**
     * @brief 词法分析结果
     */
    struct sf_lex_result_t{
        std::string id;
        std::string matched_str;
    };

    /**
     * @brief 语法分析规则
     */
    struct sf_yacc_rule{
        std::string id;
        int sub_id;
        std::vector<std::string> rules;
    };

}