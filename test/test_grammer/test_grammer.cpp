
#include <iostream>
#include "sf_lex.hpp"
#include "sf_yacc.hpp"

using namespace skyfire;

int main()
{
#if 1
    std::string json_str = R"({
    "name": "王尼玛",
    "fans": [
        {
            "name": "小王",
            "age": 7
        },
        {
            "name": "小尼玛",
            "age": 10,
        }
    ]
})";
#else
    std::string json_str = R"({"number": 1234, "data":[1,2,3,true]})";
#endif
    sf_lex lex;
    lex.add_rules({
                          {
                                  "string",
                                  R"("([^\\"]|(\\["\\/bnrt]|(u[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F])))*")"
                          },
                          {
                                  "left_square_bracket",
                                  R"(\[)"
                          },
                          {
                                  "right_square_bracket",
                                  R"(\])"
                          },
                          {
                                  "left_curly_bracket",
                                  R"(\{)"
                          },
                          {
                                  "right_curly_bracket",
                                  R"(\})"
                          },
                          {
                                  "comma",
                                  R"(,)"
                          },
                          {
                                  "colon",
                                  R"(:)"
                          },
                          {
                                  "ws",
                                  R"([\r\n\t ]+)"
                          },
                          {
                                  "number",
                                  R"(-?(0|[1-9]\d*)(\.\d+)?(e|E(\+|-)?0|[1-9]\d*)?)"
                          },
                          {
                                  "true",
                                  R"(true)"
                          },
                          {
                                  "false",
                                  R"(false)"
                          },
                          {
                                  "null",
                                  R"(null)"
                          }
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
                                                       {"members",             "comma",    "member"},
                                                       nullptr
                                               }
                                       }
                               },
                               {
                                       "member",
                                       {
                                               {
                                                       {"string",              "colon", "element"},
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

        auto yacc_result = yacc.parse(lex_result);

        for (auto &p:yacc_result)
        {
            std::cout << p->text << "(" << p->id << ")" << std::endl;
        }
    }


}