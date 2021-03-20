
#pragma once
#include "error.h"
#include "json.hpp"
#include "multi_value.h"
#include "option.h"
#include "stdc++.h"
namespace skyfire
{
enum class argv_action
{
    store,       //保存值
    store_true,  //保存为true
    store_false, // 保存为false
    count        // 保存数量
};
struct argv_opt_t
{
    std::string short_name {};                 // 短名称，如:-i
    std::string long_name {};                  // 长名称，如:--id
    std::string json_name {};                  // 保存在json中的键名称，如：id
    json_type   type     = json_type::string;  // 参数类型（参见json_type）
    bool        required = false;              // 是否必须
    argv_action action { argv_action::store }; // 动作
    json        default_value {};              // 默认值
};
using argv_result_t             = multi_value<sf_error, json>;
using argv_opt_option           = option<argv_opt_t>;
using argv_opt_option_func_type = argv_opt_option::OptionFuncType;
namespace argv
{
    namespace
    {
        auto short_name    = argv_opt_option::make_option(std::function([](argv_opt_t& opt, const std::string& name) {
            opt.short_name = name;
        }));
        auto json_name     = argv_opt_option::make_option(std::function([](argv_opt_t& opt, const std::string& name) {
            opt.json_name = name;
        }));
        auto type          = argv_opt_option::make_option(std::function([](argv_opt_t& opt, const json_type tp) {
            opt.type = tp;
        }));
        auto required      = argv_opt_option::make_option(std::function([](argv_opt_t& opt) {
            opt.required = true;
        }));
        auto action        = argv_opt_option::make_option(std::function([](argv_opt_t& opt, const argv_action& ac) {
            opt.action = ac;
        }));
        auto default_value = argv_opt_option::make_option(std::function([](argv_opt_t& opt, const json& v) {
            opt.default_value = v;
        }));
    }
}
class argparser : public std::enable_shared_from_this<argparser>
{
private:
    std::unordered_map<std::string, std::shared_ptr<argparser>> sub_parsers_;
    std::vector<argv_opt_t>                                     position_arg_;
    std::vector<argv_opt_t>                                     none_position_arg__;
    std::pair<std::string, std::string>                         prefix_ { "-", "--" };
    std::string                                                 help_;
    argv_result_t                                               parse_argv__(const std::vector<std::string>& argv);
    argparser(const std::string& help = "");

public:
    static std::shared_ptr<argparser> make_parser(
        const std::string& help = "");
    bool          add_argument(argv_opt_t opt);
    bool          add_argument(std::string short_name, std::string long_name,
                               json_type type = json_type::string,
                               bool required = false, json default_value = json(),
                               std::string json_name = "",
                               argv_action action    = argv_action::store);
    bool          add_argument(const std::string& long_name, std::initializer_list<argv_opt_option_func_type> options);
    void          add_sub_parser(const std::string&         name,
                                 std::shared_ptr<argparser> sub_parser);
    argv_result_t parse_argv(int argc, const char** argv, bool skip0 = true);
    argv_result_t parse_argv(const std::vector<std::string>& args, bool skip0 = true);
    friend void   prepare_parser__(std::shared_ptr<argparser>& parser);
};
} // namespace skyfire