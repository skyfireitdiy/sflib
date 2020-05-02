#define SF_DEBUG
#include <sf_argv>
#include <sf_test>

using namespace skyfire;
using namespace std;

bool test_parser_none_postion(){
    auto parser = sf_argparser::make_parser();
    parser->add_argument("-t", "--test",sf_json_type::string);
    auto result = parser->parse_argv({"-t", "hello"}, false);
    if (result.ec != sf_err_ok){
        sf_error(result.err_string);
        return false;
    }
    sf_debug(result.result);
    return string(result.result["--test"]) == "hello"s;
}

bool test_parser_postion(){
    auto parser = sf_argparser::make_parser();
    parser->add_argument("", "pos1",sf_json_type::string);
    parser->add_argument("", "pos2",sf_json_type::number);
    
    auto result = parser->parse_argv({"hello", "20"}, false);
    if (result.ec != sf_err_ok){
        sf_error(result.err_string);
        return false;
    }
    sf_debug(result.result);
    return string(result.result["pos1"]) == "hello"s && (int)result.result["pos2"] == 20;
}

bool test_parser_array(){
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


int main(){
    sf_test_func(test_parser_none_postion);
    sf_test_func(test_parser_postion);
    sf_test_func(test_parser_array);

    sf_test::run(3);
}