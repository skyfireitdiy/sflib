
#include <iostream>
#include "sf_lex.hpp"
#include "sf_yacc.hpp"

using namespace skyfire;

int main()
{
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
    sf_lex lex;
    lex.add_rule({"str",R"("([^\\"]|(\\["\\/bnrt]|(u[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F])))*")"});
    lex.add_rule({"ls",R"(\[)"});
    lex.add_rule({"rs",R"(\])"});
    lex.add_rule({"lb",R"(\})"});
    lex.add_rule({"rb",R"(\{)"});
    lex.add_rule({"comma",R"(,)"});
    lex.add_rule({"colon",R"(:)"});
    lex.add_rule({"ws",R"([\r\n\t ]+)"});
    lex.add_rule({"number",R"(-?(0|[1-9]\d*)(\.\d+)?(e|E(\+|-)?0|[1-9]\d*)?)"});
    std::vector<sf_lex_result_t> result;
    if(lex.parse(json_str,result))
    {
        for(auto& p:result)
        {
            std::cout<<p.matched_str<<"("<<p.id<<")"<<std::flush;
        }
    }
}