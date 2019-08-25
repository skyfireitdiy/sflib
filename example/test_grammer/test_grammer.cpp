#include "compiler/sf_lex.hpp"
#include "compiler/sf_yacc.hpp"

using namespace skyfire;

int main() {
    // 1. 一个测试用的json
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

    // 2. 创建一个词法分析器，并添加词法规则
    sf_lex lex;
    lex.set_rules(
        {{"string",
          R"("([^\\"]|(\\["\\/bnrt]|(u[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F])))*")"},
         {"[", R"(\[)"},
         {"]", R"(\])"},
         {"{", R"(\{)"},
         {"}", R"(\})"},
         {",", R"(,)"},
         {":", R"(:)"},
         {"ws", R"([\r\n\t ]+)"},
         {"number", R"(-?(0|[1-9]\d*)(\.\d+)?(e|E(\+|-)?0|[1-9]\d*)?)"},
         {"true", R"(true)"},
         {"false", R"(false)"},
         {"null", R"(null)"}});

    // 3.词法分析
    std::vector<sf_lex_result_t> lex_result;
    if (lex.parse(json_str, lex_result)) {
        // 4. 移除词法分析结果中的空白(ws)
        lex_result.erase(std::remove_if(lex_result.begin(), lex_result.end(),
                                        [](const sf_lex_result_t &r) {
                                            return r.id == "ws";
                                        }),
                         lex_result.end());
        for (auto &p : lex_result) {
            std::cout << p.matched_str << "(" << p.id << ")" << std::flush;
        }

        std::cout << std::endl << std::endl;

        // 5. 生成语法分析器，添加语法规则
        sf_yacc yacc;
        yacc.set_rules(
            {{"value",
              {{{"object"}, nullptr},
               {{"array"}, nullptr},
               {{"string"}, nullptr},
               {{"number"}, nullptr},
               {{"true"}, nullptr},
               {{"false"}, nullptr},
               {{"null"}, nullptr}}},
             {// 6. 表示 文法 object 可由 "{",  "}" （即{}）或者
              // "{",  "members",  "}"（即{"hello":"world"}）推导出来
              // 用通俗一点的话说，一个json对象，可以是空对象，或者一个或多个键值对构成
              "object",
              {{{"{", "}"}, nullptr}, {{"{", "members", "}"}, nullptr}}},
             {"members",
              {{{"member"}, nullptr}, {{"members", ",", "member"}, nullptr}}},
             {"member", {{{"string", ":", "value"}, nullptr}}},
             {"array",
              {{{"[", "]"}, nullptr}, {{"[", "values", "]"}, nullptr}}},
             {"values",
              {{{"value"}, nullptr}, {{"values", ",", "value"}, nullptr}}}});

        // 7. 添加终结符号，即json最终解析出来的结果应该是value 或者 array 或者
        // object
        yacc.add_terminate_ids({"array", "object"});
        // 8. 语法分析，使用词法分析的结果作为输入
        std::vector<std::shared_ptr<sf_yacc_result_t>> yacc_result;
        yacc.parse(lex_result, yacc_result);
        {
            for (auto &p : yacc_result) {
                std::cout << "(" << p->id << ")[" << p->text << "]"
                          << std::flush;
            }
        }
    }
}