#define SF_DEBUG
#include <sf_argv>
#include <sf_test>

using namespace skyfire;
using namespace std;

struct test_argv_param {
    std::vector<std::string> args;
    sf_json result;
};

bool test_parser_none_postion(const test_argv_param& p)
{
    auto parser = sf_argparser::make_parser();
    parser->add_argument("-t", "--test", sf_json_type::string);
    auto result = parser->parse_argv(p.args, false);
    return sf_json(result) == p.result;
}

bool test_other_name(const test_argv_param& p)
{
    auto parser = sf_argparser::make_parser();
    parser->add_argument("-t", "--test", sf_json_type::string, true, {}, "other");
    auto result = parser->parse_argv(p.args, false);
    return sf_json(result) == p.result;
}

bool test_param_type(const test_argv_param& p)
{
    auto parser = sf_argparser::make_parser();
    parser->add_argument("-s", "--string", sf_json_type::string);
    parser->add_argument("-n", "--number", sf_json_type::number);
    parser->add_argument("-a", "--array", sf_json_type::array);
    parser->add_argument("-b", "--boolean", sf_json_type::boolean);

    auto result = parser->parse_argv(p.args, false);
    return sf_json(result) == p.result;
}

bool test_bool_value(const test_argv_param& p)
{
    auto parser = sf_argparser::make_parser();
    parser->add_argument("-b", "--boolean", sf_json_type::boolean);

    auto result = parser->parse_argv(p.args, false);

    return sf_json(result) == p.result;
}

bool test_array(const test_argv_param& p)
{
    auto parser = sf_argparser::make_parser();
    parser->add_argument("-a", "--array", sf_json_type::array);

    auto result = parser->parse_argv(p.args, false);

    return sf_json(result) == p.result;
}

bool test_not_required(const test_argv_param& p)
{
    auto parser = sf_argparser::make_parser();
    parser->add_argument("-s", "--string", sf_json_type::string, false);

    auto result = parser->parse_argv(p.args, false);
    return sf_json(result) == p.result;
}

bool test_save_bool(const test_argv_param& p)
{
    auto parser = sf_argparser::make_parser();
    parser->add_argument("-t", "--true", sf_json_type::string, true, {}, "", sf_argv_action::store_true);
    parser->add_argument("-f", "--false", sf_json_type::string, true, {}, "", sf_argv_action::store_false);

    auto result = parser->parse_argv(p.args, false);

    return sf_json(result) == p.result;
}

bool test_default_value(const test_argv_param& p)
{
    auto parser = sf_argparser::make_parser();
    parser->add_argument("-s", "--string", sf_json_type::string, true, sf_json("default value"));

    auto result = parser->parse_argv(p.args, false);

    return sf_json(result) == p.result;
}

bool test_subparser(const test_argv_param& p)
{
    auto parser = sf_argparser::make_parser();
    parser->add_argument("-s", "--string", sf_json_type::string, true, sf_json("default value"));

    auto sub_parser1 = sf_argparser::make_parser();
    parser->add_sub_parser("sub1", sub_parser1);

    auto sub_parser2 = sf_argparser::make_parser();
    parser->add_sub_parser("sub2", sub_parser2);

    sub_parser1->add_argument("-o", "--one");
    sub_parser2->add_argument("-t", "--two");

    auto result = parser->parse_argv(p.args, false);

    return sf_json(result) == p.result;
}

int main()
{
    sf_test(test_parser_none_postion,
        { { { "-t", "hello" },
              R"({"--test":"hello"})"_json },
            { { "--test", "hello" },
                R"({"--test":"hello"})"_json } });

    sf_test(test_other_name,
        { { { "-t", "hello" },
              R"({"other":"hello"})"_json },
            { { "--test", "hello" },
                R"({"other":"hello"})"_json } });

    sf_test(test_param_type,
        { { { "-s", "string value", "-a", "elem1", "-a", "elem2", "-a", "elem3", "-b", "true", "-n", "26.4" },
            R"({"--string":"string value", "--array":["elem1","elem2","elem3"], "--boolean":true, "--number":26.4})"_json } });

    sf_test(test_bool_value,
        { { { "-b", "hello" }, R"({"--boolean":true})"_json },
            { { "-b", "0" }, R"({"--boolean":false})"_json },
            { { "-b", "false" }, R"({"--boolean":false})"_json },
            { { "-b", "true" }, R"({"--boolean":true})"_json },
            { { "-b", "1" }, R"({"--boolean":true})"_json } });

    sf_test(test_array,
        {
            { {}, R"({"--array":[]})"_json },
            { { "-a", "data1" }, R"({"--array":["data1"]})"_json },
            { { "-a", "data1", "-a", "data2", "-a", "data3" }, R"({"--array":["data1", "data2", "data3"]})"_json },
        });

    sf_test(test_not_required,
        {
            { {}, "{}"_json },
            { { "-s", "string value" }, R"({"--string": "string value"})"_json },
        });

    sf_test(test_save_bool,
        {
            { {}, R"({"--true":false, "--false":true})"_json },
            { { "-t" }, R"({"--true":true, "--false":true})"_json },
            { { "-f" }, R"({"--true":false, "--false":false})"_json },
            { { "-t", "-f" }, R"({"--true":true, "--false":false})"_json },
        });

    sf_test(test_default_value,
        {
            { {}, R"({"--string":"default value"})"_json },
            { { "-s", "my value" }, R"({"--string":"my value"})"_json },
        });

    sf_test(test_subparser,
        { { { "sub1", "-s", "string", "-o", "one" }, R"({"sub1":{"--string":"string", "--one": "one"}})"_json },
            { { "sub2", "-t", "two", "-s", "string" }, R"({"sub2":{"--string":"string", "--two": "two"}})"_json } });

    return sf_test_run();
}