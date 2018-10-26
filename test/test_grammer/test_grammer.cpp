
#include <iostream>
#include "sf_lex.hpp"
#include "sf_yacc.hpp"

using namespace skyfire;

int main()
{
#if 1
    std::string json_str = R"({
    "code": 0,
    "message": "交易成功",
    "object": {
        "head": {
            "version": "1.0.0",
            "code": "contract_setting_info",
            "timestamp": 1400138523979
        },
        "body": {
            "contractSettings": [
                {
                    "contractDict": {
                        "code": "YGXAG9995",
                        "name": "粤贵银9995",
                        "market_code": "PMEC",
                        "status": 1,
                        "sort": 0
                    },
                    "contractEx": {
                        "unit_number": 15,
                        "unit_type": "KG",
                        "min_quote_unit": 1,
                        "min_var_unit": 1,
                        "step_unit": 1,
                        "currency": "CNY",
                        "quote_type": 1
                    },
                    "contractProtect": {
                        "spread": 20,
                        "time_interval": 60000
                    },
                    "contractAccess": null
                },
                {
                    "contractDict": {
                        "code": "YGXAG9999",
                        "name": "粤贵银9999",
                        "market_code": "PMEC",
                        "status": 1,
                        "sort": 0
                    },
                    "contractEx": {
                        "unit_number": 1,
                        "unit_type": "KG",
                        "min_quote_unit": 1,
                        "min_var_unit": 1,
                        "step_unit": 1,
                        "currency": "CNY",
                        "quote_type": 1
                    },
                    "contractProtect": {
                        "spread": 20,
                        "time_interval": 600
                    },
                    "contractAccess": null
                },
                {
                    "contractDict": {
                        "code": "YGXAG100G",
                        "name": "粤贵银100G",
                        "market_code": "PMEC",
                        "status": 1,
                        "sort": 0
                    },
                    "contractEx": {
                        "unit_number": 100,
                        "unit_type": "g",
                        "min_quote_unit": 0.001,
                        "min_var_unit": 0.001,
                        "step_unit": 0.001,
                        "currency": "CNY",
                        "quote_type": 1
                    },
                    "contractProtect": {
                        "spread": 20,
                        "time_interval": 600
                    },
                    "contractAccess": null
                }
            ]
        }
    },
    "success": true
})";

#else
    std::string json_str = R"({"hello":1231})";
#endif

    sf_lex lex;
    lex.add_rules({
                          {"string", R"("([^\\"]|(\\["\\/bnrt]|(u[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F])))*")"},
                          {"left_square_bracket", R"(\[)"},
                          {"right_square_bracket", R"(\])"},
                          {"left_curly_bracket", R"(\{)"},
                          {"right_curly_bracket", R"(\})"},
                          {"comma", R"(,)"},
                          {"colon", R"(:)"},
                          {"ws", R"([\r\n\t ]+)"},
                          {"number", R"(-?(0|[1-9]\d*)(\.\d+)?(e|E(\+|-)?0|[1-9]\d*)?)"},
                          {"true", R"(true)"},
                          {"false", R"(false)"},
                          {"null", R"(null)"}
                  });

    std::vector<sf_lex_result_t> lex_result;
    if (lex.parse(json_str, lex_result))
    {
        lex_result.erase(std::remove_if(lex_result.begin(), lex_result.end(), [](const sf_lex_result_t &r)
        {
            return r.id == "ws";
        }), lex_result.end());
        for (auto &p:lex_result)
        {
            std::cout << p.matched_str << "(" << p.id << ")" << std::flush;
        }

        std::cout << std::endl << std::endl;

        sf_yacc yacc;
        yacc.add_rules({
                               {
                                       "value",
                                       {
                                               {
                                                       {"object"},
                                                       nullptr
                                               },
                                               {
                                                       {"array"},
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
                                                       {"true"},
                                                       nullptr
                                               },
                                               {
                                                       {"false"},
                                                       nullptr,
                                               },
                                               {
                                                       {"null"},
                                                       nullptr
                                               }
                                       }
                               },
                               {
                                       "object",
                                       {
                                               {
                                                       {"left_curly_bracket",  "right_curly_bracket"},
                                                       nullptr
                                               },
                                               {
                                                       {"left_curly_bracket",  "members",  "right_curly_bracket"},
                                                       nullptr
                                               }
                                       }
                               },
                               {
                                       "members",
                                       {
                                               {
                                                       {"member"},
                                                       nullptr
                                               },
                                               {
                                                       {"members","comma","member"},
                                                       nullptr
                                               }
                                       }
                               },
                               {
                                       "member",
                                       {
                                               {
                                                       {"string", "colon", "array"},
                                                       nullptr
                                               },
                                               {
                                                       {"string", "colon", "object"},
                                                       nullptr
                                               },
                                               {
                                                       {"string","colon","string"},
                                                       nullptr
                                               },
                                               {
                                                       {"string","colon","number"},
                                                       nullptr
                                               },
                                               {
                                                       {"string","colon","true"},
                                                       nullptr
                                               },
                                               {
                                                       {"string","colon","false"},
                                                       nullptr
                                               },
                                               {
                                                       {"string","colon","null"},
                                                       nullptr
                                               }

                                       }
                               },
                               {
                                       "array",
                                       {
                                               {
                                                       {"left_square_bracket", "right_square_bracket"},
                                                       nullptr
                                               },
                                               {
                                                       {"left_square_bracket", "elements", "right_square_bracket"}
                                               }
                                       }
                               },
                               {
                                       "elements",
                                       {
                                               {
                                                       {"element"},
                                                       nullptr
                                               },
                                               {
                                                       {"elements",            "comma",    "element"},
                                                       nullptr
                                               },
                                               {
                                                       {"elements",            "comma",    "elements"},
                                                       nullptr
                                               }
                                       }
                               },
                               {
                                       "element",
                                       {
                                               {
                                                       {"value"},
                                                       nullptr
                                               }
                                       }
                               }
                       });

        yacc.add_termanate_ids({"value", "array", "object"});
        std::vector<std::shared_ptr<sf_yacc_result_t>> yacc_result;
        if (yacc.parse(lex_result, yacc_result))
        {
            for (auto &p:yacc_result)
            {
                std::cout << "(" << p->id << ")[" << p->text << "]" << std::flush;
            }
        }
    }


}