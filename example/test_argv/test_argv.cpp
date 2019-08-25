
#include "tools/sf_argv.hpp"

using namespace skyfire;
using namespace std;

int main() {
    auto parser = sf_argparser::make_parser();
    auto add_parser = sf_argparser::make_parser();
    auto mul_parser = sf_argparser::make_parser();

    auto add_int_parser = sf_argparser::make_parser();

    parser->add_sub_parser("add", add_parser);
    parser->add_sub_parser("mul", mul_parser);

    add_parser->add_sub_parser("int", add_int_parser);

    add_parser->add_argument("plus", "", sf_json_type::array);
    mul_parser->add_argument("-f", "--first", sf_json_type::number, false);
    mul_parser->add_argument("-s", "--second", sf_json_type::number, false);
    mul_parser->add_argument("-a", "--array", sf_json_type::array, false);
    mul_parser->add_argument("-F", "--flag", sf_json_type::boolean, false, {},
                             "flag_name", sf_argv_action::store_true);

    add_int_parser->add_argument("-a", "--a", sf_json_type::number);
    add_int_parser->add_argument("-b", "--b", sf_json_type::number);

    const char* argv1[]{"add", "5", "10"};
    cout << parser->parse_argv(3, argv1, false) << endl;

    const char* argv2[]{"mul", "-f", "5.5", "-s", "1.9"};
    cout << parser->parse_argv(5, argv2, false) << endl;

    const char* argv3[]{"mul", "-f", "5.5", "--array",  "20",
                        "68",  "90", "70",  "--second", "6.5"};
    cout << parser->parse_argv(10, argv3, false) << endl;

    const char* argv4[]{"mul", "-f", "5.5",      "--array", "20", "68",
                        "90",  "70", "--second", "6.5",     "-F"};
    cout << parser->parse_argv(11, argv4, false) << endl;

    const char* argv5[]{"add", "int", "-a", "5", "-b", "10"};
    cout << parser->parse_argv(6, argv5, false);
}