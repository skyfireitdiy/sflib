
#pragma once
#include "compiler_utils.h"
#include "stdc++.h"
namespace skyfire
{
class lex final
{
private:
    std::vector<lex_rule_t> rules__;

public:
    void                            set_rules(const std::vector<lex_rule_t>& rule);
    bool                            parse(std::string content, std::vector<lex_result_t>& result);
    std::vector<lex_rule_t>         rules() const;
    std::unordered_set<std::string> real_term() const;
};
} // namespace skyfire
