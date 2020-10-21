
/**
* @version 1.0.0
* @author skyfire
* @file sf_argv.h

* 
*/

#pragma once

#include "sf_error.h"
#include "sf_json.hpp"
#include "sf_multi_value.h"
#include "sf_option.h"
#include "sf_stdc++.h"

namespace skyfire {

/**
 * @brief 参数解析时的动作
 *
 */
enum class argv_action {
    store, //保存值
    store_true, //保存为true
    store_false, // 保存为false
    count // 保存数量
};

/**
 * @brief 参数选项
 *
 */
struct argv_opt_t {
    std::string short_name {}; // 短名称，如:-i
    std::string long_name {}; // 长名称，如:--id
    std::string json_name {}; // 保存在json中的键名称，如：id
    json_type type = json_type::string; // 参数类型（参见json_type）
    bool required = false; // 是否必须
    argv_action action { argv_action::store }; // 动作
    json default_value {}; // 默认值
};

using argv_result_t = multi_value<err, json>;

using argv_opt_option = option<argv_opt_t>;
using argv_opt_option_func_type = argv_opt_option::OptionFuncType;

namespace argv {
    namespace {
        auto short_name = argv_opt_option::make_option(std::function([](argv_opt_t& opt, const std::string& name) {
            opt.short_name = name;
        }));
        auto json_name = argv_opt_option::make_option(std::function([](argv_opt_t& opt, const std::string& name) {
            opt.json_name = name;
        }));
        auto type = argv_opt_option::make_option(std::function([](argv_opt_t& opt, const json_type tp) {
            opt.type = tp;
        }));
        auto required = argv_opt_option::make_option(std::function([](argv_opt_t& opt) {
            opt.required = true;
        }));
        auto action = argv_opt_option::make_option(std::function([](argv_opt_t& opt, const argv_action& ac) {
            opt.action = ac;
        }));
        auto default_value = argv_opt_option::make_option(std::function([](argv_opt_t& opt, const json& v) {
            opt.default_value = v;
        }));
    }
}

/**
 * @brief 参数解析类
 *
 */
class argparser : public std::enable_shared_from_this<argparser> {
private:
    std::unordered_map<std::string, std::shared_ptr<argparser>> sub_parsers_;
    std::vector<argv_opt_t> position_arg_;
    std::vector<argv_opt_t> none_position_arg__;
    std::pair<std::string, std::string> prefix_ { "-", "--" };
    std::string help_;

    argv_result_t parse_argv__(const std::vector<std::string>& argv);

    argparser(const std::string& help = "");

public:
    /**
     * 创建一个解析器对象
     * @param help 帮助信息（TODO 输出help信息）
     * @param prefix 前缀
     * @return 解析器对象
     */
    static std::shared_ptr<argparser> make_parser(
        const std::string& help = "");

    /**
     * @brief 添加一个参数
     *
     * @param opt 参数信息
     * @return true 添加成功
     * @return false 添加失败
     */
    bool add_argument(argv_opt_t opt);

    /**
     * @brief 添加一个参数
     *
     * @param short_name 短名称
     * @param long_name 长名称
     * @param type 类型
     * @param required 是否必须
     * @param default_value 默认值
     * @param json_name json键名称
     * @param action 动作
     * @return true 添加成功
     * @return false 添加失败
     */
    bool add_argument(std::string short_name, std::string long_name,
        json_type type = json_type::string,
        bool required = false, json default_value = json(),
        std::string json_name = "",
        argv_action action = argv_action::store);

    /**
     * @brief 增加参数
     * 
     * @param long_name 参数名称
     * @param options 选项
     * @return true 设置失败
     * @return false 设置成功
     */
    bool add_argument(const std::string& long_name, std::initializer_list<argv_opt_option_func_type> options);

    /**
     * @brief 添加子解析器
     *
     * @param name 解析器名称（会被用于键名称）
     * @param sub_parser 子解析器
     */
    void add_sub_parser(const std::string& name,
        std::shared_ptr<argparser> sub_parser);

    /**
     * @brief 解析参数
     *
     * @param argc 参数数量
     * @param argv 参数列表
     * @param skip0 跳过参数0
     * @return argv_result_t 解析结果
     */
    argv_result_t parse_argv(int argc, const char** argv, bool skip0 = true);

    /**
     * @brief 解析参数
     *
     * @param args 参数列表
     * @return argv_result_t 解析结果
     */
    argv_result_t parse_argv(const std::vector<std::string>& args, bool skip0 = true);

    friend void prepare_parser__(std::shared_ptr<argparser>& parser);
};

} // namespace skyfire