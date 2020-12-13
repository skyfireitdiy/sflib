#define SF_DEBUG
#include <sf_argv>
#include <sf_test>

using namespace skyfire;
using namespace std;

struct test_argv_param
{
    std::vector<std::string> args;
    json                     result;
};

sf_test_p(test_parser_none_postion, test_argv_param, {
                                                         {
                                                             { "-t", "hello" },
                                                             R"({"--test":"hello"})"_json,
                                                         },
                                                         {
                                                             { "--test", "hello" },
                                                             R"({"--test":"hello"})"_json,
                                                         },
                                                     })
{
    auto parser = argparser::make_parser();
    parser->add_argument("--test", { argv::short_name("-t") });
    auto result = parser->parse_argv(test_param.args, false);

    test_p_eq(json(result), test_param.result);
}

sf_test_p(test_other_name, test_argv_param, {
                                                {
                                                    { "-t", "hello" },
                                                    R"({"other":"hello"})"_json,
                                                },
                                                {
                                                    { "--test", "hello" },
                                                    R"({"other":"hello"})"_json,
                                                },
                                            })
{
    auto parser = argparser::make_parser();
    parser->add_argument("--test", { argv::short_name("-t"), argv::required(), argv::json_name("other") });
    auto result = parser->parse_argv(test_param.args, false);
    test_p_eq(json(result), test_param.result);
}

sf_test_p(test_param_type, test_argv_param, {
                                                {
                                                    { "-s", "string value", "-a", "elem1", "-a", "elem2", "-a", "elem3", "-b", "true", "-n", "26.4" },
                                                    R"({"--string":"string value", "--array":["elem1","elem2","elem3"], "--boolean":true, "--number":26.4})"_json,
                                                },
                                            })
{
    auto parser = argparser::make_parser();
    parser->add_argument("--string", { argv::short_name("-s"), argv::type(json_type::string) });
    parser->add_argument("--number", { argv::short_name("-n"), argv::type(json_type::number) });
    parser->add_argument("--array", { argv::short_name("-a"), argv::type(json_type::array) });
    parser->add_argument("--boolean", { argv::short_name("-b"), argv::type(json_type::boolean) });

    auto result = parser->parse_argv(test_param.args, false);
    test_p_eq(json(result), test_param.result);
}

sf_test_p(test_bool_value, test_argv_param, {
                                                {
                                                    { "-b", "hello" },
                                                    R"({"--boolean":true})"_json,
                                                },
                                                {
                                                    { "-b", "0" },
                                                    R"({"--boolean":false})"_json,
                                                },
                                                {
                                                    { "-b", "false" },
                                                    R"({"--boolean":false})"_json,
                                                },
                                                {
                                                    { "-b", "true" },
                                                    R"({"--boolean":true})"_json,
                                                },
                                                {
                                                    { "-b", "1" },
                                                    R"({"--boolean":true})"_json,
                                                },
                                            })
{
    auto parser = argparser::make_parser();
    parser->add_argument("-b", "--boolean", json_type::boolean);

    auto result = parser->parse_argv(test_param.args, false);

    test_p_eq(json(result), test_param.result);
}

sf_test_p(test_array, test_argv_param, {
                                           {
                                               {},
                                               R"({"--array":[]})"_json,
                                           },
                                           {
                                               { "-a", "data1" },
                                               R"({"--array":["data1"]})"_json,
                                           },
                                           {
                                               { "-a", "data1", "-a", "data2", "-a", "data3" },
                                               R"({"--array":["data1", "data2", "data3"]})"_json,
                                           },
                                       })
{
    auto parser = argparser::make_parser();
    parser->add_argument("-a", "--array", json_type::array);

    auto result = parser->parse_argv(test_param.args, false);

    test_p_eq(json(result), test_param.result);
}

sf_test_p(test_not_required, test_argv_param, {
                                                  {
                                                      {},
                                                      "{}"_json,
                                                  },
                                                  {
                                                      { "-s", "string value" },
                                                      R"({"--string": "string value"})"_json,
                                                  },
                                              })
{
    auto parser = argparser::make_parser();
    parser->add_argument("-s", "--string", json_type::string, false);

    auto result = parser->parse_argv(test_param.args, false);
    test_p_eq(json(result), test_param.result);
}

sf_test_p(test_save_bool, test_argv_param, {
                                               {
                                                   {},
                                                   R"({"--true":false, "--false":true})"_json,
                                               },
                                               {
                                                   { "-t" },
                                                   R"({"--true":true, "--false":true})"_json,
                                               },
                                               {
                                                   { "-f" },
                                                   R"({"--true":false, "--false":false})"_json,
                                               },
                                               {
                                                   { "-t", "-f" },
                                                   R"({"--true":true, "--false":false})"_json,
                                               },
                                           })
{
    auto parser = argparser::make_parser();
    parser->add_argument("-t", "--true", json_type::string, true, {}, "", argv_action::store_true);
    parser->add_argument("-f", "--false", json_type::string, true, {}, "", argv_action::store_false);

    auto result = parser->parse_argv(test_param.args, false);

    test_p_eq(json(result), test_param.result);
}

sf_test_p(test_default_value, test_argv_param, {
                                                   {
                                                       {},
                                                       R"({"--string":"default value"})"_json,
                                                   },
                                                   {
                                                       { "-s", "my value" },
                                                       R"({"--string":"my value"})"_json,
                                                   },
                                               })
{
    auto parser = argparser::make_parser();
    parser->add_argument("-s", "--string", json_type::string, true, json("default value"));

    auto result = parser->parse_argv(test_param.args, false);

    test_p_eq(json(result), test_param.result);
}

sf_test_p(test_subparser, test_argv_param, {
                                               {
                                                   { "sub1", "-s", "string", "-o", "one" },
                                                   R"({"sub1":{"--string":"string", "--one": "one"}})"_json,
                                               },
                                               {
                                                   { "sub2", "-t", "two", "-s", "string" },
                                                   R"({"sub2":{"--string":"string", "--two": "two"}})"_json,
                                               },
                                           })
{
    auto parser = argparser::make_parser();
    parser->add_argument("-s", "--string", json_type::string, true, json("default value"));

    auto sub_parser1 = argparser::make_parser();
    parser->add_sub_parser("sub1", sub_parser1);

    auto sub_parser2 = argparser::make_parser();
    parser->add_sub_parser("sub2", sub_parser2);

    sub_parser1->add_argument("-o", "--one");
    sub_parser2->add_argument("-t", "--two");

    auto result = parser->parse_argv(test_param.args, false);

    test_p_eq(json(result), test_param.result);
}

int main()
{
    return run_test();
}