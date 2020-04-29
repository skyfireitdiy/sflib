#define SF_DEBUG
#include <sf_argv>
#include <sf_test>

using namespace skyfire;
using namespace std;

bool test_parser(){
    auto parser = sf_argparser::make_parser();
    parser->add_argument("-t", "--test",sf_json_type::string);
    auto result = parser->parse_argv({"-t", "hello"}, false);
    sf_debug(result);
    return string(result["--test"]) == "hello"s;
}

int main(){
    sf_test_func(test_parser);

    sf_test::run();
}