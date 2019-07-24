#pragma once

#include "sf_template.h"
#include "sf_utils.hpp"
#include "sf_json.hpp"

#include "sf_stdc++.h"
#include "sf_logger.hpp"

#include "sf_lex.hpp"

namespace skyfire
{
    inline std::string sf_render(std::string content, const sf_json &js, const std::pair<std::string,std::string> &sep) {
        auto back_up = content;

        auto extract_result = sf_extract__(content, sep);
        for (auto p = extract_result.rbegin(); p != extract_result.rend(); ++p) {
            sf_template_parse__(p->data, js);
            // TODO replace text
        }

        return content;
    }

    inline std::vector<sf_template_info_t> sf_extract__(const std::string &content,
                                                        const std::pair<std::string, std::string> &sep)
    {
        std::vector<sf_template_info_t> ret;
        auto start = sep.first;
        auto end = sep.second;
        int pos = 0;
        while(true)
        {
            pos = content.find(start, pos);
            if(pos == -1)
            {
                break;
            }
            auto s_pos = pos + start.length();
            pos = content.find(end, s_pos);
            if(pos == -1)
            {
                break;
            }
            auto e_pos = pos;
            pos += end.length();
            sf_template_info_t tmp;
            tmp.data = {content.substr(s_pos, e_pos-s_pos)};
            tmp.pos = s_pos;
            tmp.len = e_pos-s_pos;
            ret.push_back(tmp);
        }
        return ret;
    }

    inline std::string sf_template_parse__(std::string content, const sf_json& js) {
        sf_lex lex;
        lex.set_rules({
                              {"if",       R"(if)"},
                              {"sp",       R"(\s+)"},
                              {"for",      R"(for)"},
                              {"in",       R"(in)"},
                              {"print",    R"(print)"},
                              {"string",   R"("([^\\"]|(\\["\\/bnrt]|(u[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F])))*")"},
                              {"number",   R"(-?(0|[1-9]\d*)(\.\d+)?(e|E(\+|-)?0|[1-9]\d*)?)"},
                              {"true",     R"(true)"},
                              {"false",    R"(false)"},
                              {"null",     R"(null)"},
                              {":",        R"(:)"},
                              {"(",        R"(\()"},
                              {")",        R"(\))"},
                              {"{",        R"(\{)"},
                              {"}",        R"(\})"},
                              {"[",        R"(\[)"},
                              {"]",        R"(\])"},
                              {"\"",       R"(")"},
                              {";",        R"(;)"},
                              {",",        R"(,)"},
                              {".",        R"(\.)"},
                              {"=",        R"(=)"},
                              {"!",        R"(\!)"},
                              {"!=",       R"(\!=)"},
                              {"==",       R"(==)"},
                              {"&&",       R"(\&\&)"},
                              {"||",       R"(\|\|)"},
                              {"key_word", R"(\w+)"},
                      });

        sf_yacc yacc;
        yacc.set_rules(
                {
                        {
                                "value",
                                {
                                        {
                                                {"true"},
                                                nullptr
                                        },
                                        {
                                                {"false"},
                                                nullptr
                                        },
                                        {
                                                {"null"},
                                                nullptr
                                        },
                                        {
                                                {"key_word"},
                                                nullptr
                                        },
                                        {
                                                {"string"},
                                                nullptr
                                        },
                                        {
                                                {"number"},
                                                nullptr
                                        },
                                        {
                                                {"int"},
                                                nullptr
                                        },
                                        {
                                                {"(", "value", ")"},
                                                nullptr
                                        },
                                        {
                                                {"value", "[", "string", "]"},
                                                nullptr
                                        },
                                        {
                                                {"value", "+", "value"},
                                                nullptr
                                        },
                                        {
                                                {"!", "value"},
                                                nullptr
                                        },
                                        {
                                                {"value", "!=", "value"},
                                                nullptr
                                        },
                                        {
                                                {"value", "==", "value"},
                                                nullptr
                                        },
                                        {
                                                {"value", ".", "key_word"},
                                                nullptr
                                        }
                                }
                        },
                        {
                                "for",
                                {
                                        {
                                                {"for", "key_word", "in", "value", "block"},
                                                nullptr
                                        }
                                }
                        },
                        {
                                "block",
                                {
                                        {
                                                {"{",   "}"},
                                                nullptr
                                        },
                                        {
                                                {"{", "sentences", "}"},
                                                nullptr
                                        }
                                }
                        },
                        {
                                "sentence",
                                {
                                        {
                                                {"value"},
                                                nullptr
                                        },
                                        {
                                                {"for"},
                                                nullptr
                                        },
                                        {
                                                {"block"},
                                                nullptr
                                        },
                                        {
                                            { "sentence", "value" },
                                            nullptr
                                        }
                                }
                        }
                }
        );


        std::vector<sf_lex_result_t> lex_result;
        auto lex_res = lex.parse(content, lex_result);
        if (!lex_res) {
            return "";
        }
        lex_result.erase(std::remove_if(lex_result.begin(), lex_result.end(), [](const sf_lex_result_t &t) {
            return t.id == "sp";
        }), lex_result.end());

//            for(auto &p:lex_result)
//            {
//                std::cout<<p.id<<"【"<<p.matched_str<<"】";
//            }
        std::cout << std::endl;

    }
}