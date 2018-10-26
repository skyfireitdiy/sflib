#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <any>

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
     * @brief 语法树节点
     */
    struct sf_yacc_result_t
    {
        std::string id;
        std::string text;
        std::vector<std::shared_ptr<sf_yacc_result_t>> children;
        std::any user_data;
    };

    /**
     * @brief 语法规则及回调函数
     */
    struct sf_yacc_rule_callback
    {
        std::vector<std::string> rule;
        std::function<std::any(const std::vector<std::shared_ptr<sf_yacc_result_t>> &)> callback;
    };

    /**
     * @brief 语法分析规则
     */
    struct sf_yacc_rule{
        std::string id;
        std::vector<sf_yacc_rule_callback> rules;
    };


}