#pragma once

#include <any>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace skyfire {
/**
 * @brief 词法规则
 */
struct sf_lex_rule_t final {
    std::string id;           // 标识
    std::string regex_str;    // 正则表达式
};

/**
 * @brief 词法分析结果
 */
struct sf_lex_result_t final {
    std::string id;             // 标识
    std::string matched_str;    // 匹配到的字符串
};

/**
 * @brief 语法树节点
 */
struct sf_yacc_result_t final {
    std::string id;      // 标识
    std::string text;    // 匹配到的字符串
    std::vector<std::shared_ptr<sf_yacc_result_t>> children;    // 子节点
    std::any user_data;    // 用户数据，从回调函数中返回
};

/**
 * @brief 语法规则及回调函数
 */
struct sf_yacc_rule_callback final {
    std::vector<std::string> rule;    // 规则集（标识符序列）
    std::function<std::any(
        const std::vector<std::shared_ptr<sf_yacc_result_t>>&)>
        callback;    // 回调函数，该函数接收一个推导出该结果的结果集合，返回的数据将被存储在本节点的user_data中
};

/**
 * @brief 语法分析规则
 */
struct sf_yacc_rule final {
    std::string id;                              // 标识
    std::vector<sf_yacc_rule_callback> rules;    // 规则集合
};

struct sf_yacc_state_node_t final {
    std::string id;
    std::function<std::any(
        const std::vector<std::shared_ptr<sf_yacc_result_t>>&)>
        callback;
};

inline bool operator<(const sf_yacc_state_node_t& a,
                      const sf_yacc_state_node_t& b) {
    return a.id < b.id;
}
inline bool operator==(const sf_yacc_state_node_t& a,
                       const sf_yacc_state_node_t& b) {
    return a.id == b.id;
}

constexpr char sf_yacc_end_mark[] = "__SF_YACC_END_MARK__";

}    // namespace skyfire