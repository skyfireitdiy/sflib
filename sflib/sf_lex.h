
/**
* @version 1.0.0
* @author skyfire
* @file sf_lex.h

* 
*/

#pragma once

#include "sf_grammer_utils.h"
#include "sf_stdc++.h"

namespace skyfire
{
/**
 * @brief 词法分析器
 */
class lex final
{
private:
    std::vector<lex_rule_t> rules__;

public:
    /**
     * 设置规则
     * @param rule 规则集合
     */
    void set_rules(const std::vector<lex_rule_t>& rule);

    /**
     * 解析
     * @param content 内容
     * @param result 结果
     * @return 是否解析成功
     */
    bool parse(std::string content, std::vector<lex_result_t>& result);

    /**
     * 获取规则
     * @return 规则
     */
    std::vector<lex_rule_t> rules() const;

    /**
     * 获取终结符
     * @return 终结符
     */
    std::unordered_set<std::string> real_term() const;
};

} // namespace skyfire
