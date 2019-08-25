
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#include "core/sf_stdc++.h"
#include "sf_grammer_utils.h"

namespace skyfire {
/**
 * @brief 词法分析器
 */
class sf_lex final {
   private:
    std::vector<sf_lex_rule_t> rules__;

   public:
    /**
     * 设置规则
     * @param rule 规则集合
     */
    void set_rules(const std::vector<sf_lex_rule_t> &rule);

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
    std::vector<sf_lex_rule_t> rules() const;

    /**
     * 获取终结符
     * @return 终结符
     */
    std::unordered_set<std::string> real_term() const;
};

}    // namespace skyfire
#pragma clang diagnostic pop