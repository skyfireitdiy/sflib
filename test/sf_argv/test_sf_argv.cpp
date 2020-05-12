#define SF_DEBUG
#include <sf_argv>
#include <sf_test>

using namespace skyfire;
using namespace std;


bool test_parser_postion()
{
    auto parser = sf_argparser::make_parser();
    parser->add_argument("", "pos1", sf_json_type::string);
    parser->add_argument("", "pos2", sf_json_type::number);

    auto result = parser->parse_argv({ "hello", "20" }, false);
    if (result.ec != sf_err_ok) {
        sf_error(result.err_string);
        return false;
    }
    sf_debug(result.result);
    return string(result.result["pos1"]) == "hello"s && (int)result.result["pos2"] == 20;
}

bool test_parser_array()
{
    auto parser = sf_argparser::make_parser();
    parser->add_argument("-a", "--arr", sf_json_type::array);

    auto result = parser->parse_argv({ "-a", "20", "--arr", "60", "-a", "98" }, false);
    if (result.ec != sf_err_ok) {
        sf_error(result.err_string);
        return false;
    }
    sf_debug(result.result);
    return result.result == sf_json::from_string(R"({"--arr":["20","60","98"]})");
}

bool test_parser_default_value()
{
    auto parser = sf_argparser::make_parser();
    parser->add_argument("-p", "--param", sf_json_type::string, true, sf_json("hello"));

    auto result = parser->parse_argv({}, false);
    if (result.ec != sf_err_ok) {
        sf_error(result.err_string);
        return false;
    }
    sf_debug(result.result);
    return static_cast<std::string>(result.result["--param"]) == "hello";
}

bool test_parser_default_value_array()
{
    auto parser = sf_argparser::make_parser();
    parser->add_argument("-p", "--param", sf_json_type::array, true, R"(["hello","world"])"_json);

    auto result = parser->parse_argv({}, false);
    if (result.ec != sf_err_ok) {
        sf_error(result.err_string);
        return false;
    }
    sf_debug(result.result);
    return result.result["--param"] == R"(["hello","world"])"_json;
}



bool test_parser_subparser()
{
    auto parser = sf_argparser::make_parser();
    auto sub_parser = sf_argparser::make_parser();
    parser->add_sub_parser("sub", sub_parser);
    sub_parser->add_argument("-a", "--arr");

    auto result = parser->parse_argv({"sub", "-a", "test" }, false);
    if (result.ec != sf_err_ok) {
        sf_error(result.err_string);
        return false;
    }
    sf_debug(result.result);
    return static_cast<string>(result.result["sub"]["--arr"]) == "test";
}

struct test_argv_param{
    std::vector<std::string> args;
    sf_json result;
};

bool test_parser_none_postion(const test_argv_param& p){
    auto parser = sf_argparser::make_parser();
    parser->add_argument("-t", "--test", sf_json_type::string);
    auto result = parser->parse_argv(p.args, false);
    if (result.ec != sf_err_ok) {
        sf_error(result.err_string);
        return false;
    }
    sf_debug(result.result);
    return result.result == p.result;
}

    int main()
{
    sf_test(test_parser_none_postion, 
    {
        {
            {"-t", "hello"},
            R"({"--test":"hello"})"_json
        },
        {
            {"--test", "hello"},
            R"({"--test":"hello"})"_json
        }
    });
    sf_test(test_parser_postion);
    sf_test(test_parser_array);
    sf_test(test_parser_default_value);
    sf_test(test_parser_default_value_array);
    sf_test(test_parser_subparser);

    sf_test_run(1);
}