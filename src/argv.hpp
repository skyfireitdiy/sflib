
#pragma once
#include "argv.h"
#include "error.h"
#include "utils.h"
namespace skyfire
{
inline argparser::argparser(
    const std::string& help)
    : help__(help)
{
}
inline std::shared_ptr<argparser> argparser::make_parser(
    const std::string& help)
{
    return std::shared_ptr<argparser>(new argparser(help));
}
inline void argparser::add_sub_parser(
    const std::string&                  name,
    std::shared_ptr<skyfire::argparser> sub_parser)
{
    sub_parsers__[name] = sub_parser;
}
inline bool argparser::add_argument(const std::string&                               long_name,
                                    std::initializer_list<argv_opt_option_func_type> options)
{
    argv_opt_t opt;
    opt.long_name = long_name;
    for (auto& f : options)
    {
        f(opt);
    }
    return add_argument(opt);
}

inline bool argparser::check_json_name_duplicate__(const std::string& json_name) const
{
    for (const argv_opt_t& p : position_arg__)
    {
        if (json_name == p.json_name)
        {
            return true;
        }
    }
    for (const argv_opt_t& p : none_position_arg__)
    {
        if (json_name == p.json_name)
        {
            return true;
        }
    }
    return false;
}

inline bool argparser::check_none_position_arg_short_or_long_name_duplicate__(const std::string& name) const
{
    for (auto& p : none_position_arg__)
    {
        if (p.short_name == name || p.long_name == name)
        {
            return true;
        }
    }
    return false;
}

inline bool argparser::short_name_matched__(const std::string& short_name) const
{
    return string_start_with(short_name, prefix__.first);
}

inline bool argparser::long_name_matched__(const std::string& long_name) const
{
    return string_start_with(long_name, prefix__.second);
}

inline bool argparser::check_position_arg_long_name_duplicate__(const std::string& name) const
{
    for (auto& p : position_arg__)
    {
        if (p.long_name == name)
        {
            return true;
        }
    }
    return false;
}

inline bool argparser::gen_json_name__(argv_opt_t& opt)
{
    if (opt.json_name.empty())
    {
        opt.json_name = opt.long_name;
    }
    if (opt.json_name.empty())
    {
        opt.json_name = opt.short_name;
    }
    return !opt.json_name.empty();
}

inline bool argparser::add_none_position_arg__(const argv_opt_t& opt)
{
    if (check_none_position_arg_short_or_long_name_duplicate__(opt.short_name))
    {
        return false;
    }
    if (check_none_position_arg_short_or_long_name_duplicate__(opt.long_name))
    {
        return false;
    }
    none_position_arg__.push_back(opt);
    return true;
}

inline bool argparser::add_position_arg__(const argv_opt_t& opt)
{
    if (check_position_arg_long_name_duplicate__(opt.long_name))
    {
        return false;
    }
    position_arg__.push_back(opt);
    return true;
}

inline bool argparser::add_argument(argv_opt_t opt)
{
    if (!gen_json_name__(opt))
    {
        return false;
    }
    if (check_json_name_duplicate__(opt.json_name))
    {
        return false;
    }
    if (short_name_matched__(opt.short_name) && long_name_matched__(opt.long_name))
    {
        return add_none_position_arg__(opt);
    }
    return add_position_arg__(opt);
}

inline void argparser::init_none_position_arg_value__(json& ret)
{
    for (auto& p : none_position_arg__)
    {
        if (p.type == json_type::array)
        {
            ret[p.json_name].convert_to_array();
        }
        if (!p.default_value.is_null())
        {
            ret[p.json_name] = p.default_value;
        }
        if (p.action == argv_action::count)
        {
            ret[p.json_name] = 0;
        }
        if (p.action == argv_action::store_false)
        {
            ret[p.json_name] = true;
        }
        if (p.action == argv_action::store_true)
        {
            ret[p.json_name] = false;
        }
    }
}

inline bool argparser::parse_none_position_arg__(json& ret, const std::string& p, std::shared_ptr<argv_opt_t>& switch_opt)
{
    for (const argv_opt_t& nopt : none_position_arg__)
    {
        if (nopt.long_name == p || nopt.short_name == p)
        {
            switch (nopt.action)
            {
            case argv_action::store_true:
                ret[nopt.json_name] = true;
                break;
            case argv_action::store_false:
                ret[nopt.json_name] = false;
                break;
            case argv_action::count:
                ret[nopt.json_name] = static_cast<int>(ret[nopt.json_name]) + 1;
                break;
            default:
                switch_opt = std::shared_ptr<argv_opt_t>(new argv_opt_t(nopt));
            }

            return true;
        }
    }
    return false;
}

inline argv_result_t argparser::parse_argv_value__(const std::string& p, const argv_opt_t& position_opt)
{
    auto d = json();
    switch (position_opt.type)
    {
    case json_type::string:
    case json_type::array:
        return { sf_error {}, json(p) };
    case json_type::boolean:
        return { sf_error {}, json(!(p == "0" || p == "false")) };

        break;
    case json_type::number:
        return { sf_error {}, json(string_to_long_double(p)) };
        break;
    default:
        return argv_result_t {
            sf_error { { err_unsupport_type, "unsupport:" + std::to_string(static_cast<int>(position_opt.type)) } }, json {}
        };
    }
}

inline argv_result_t argparser::parse_argv__(
    const std::vector<std::string>& argv)
{
    json ret;
    ret.convert_to_object();
    std::shared_ptr<argv_opt_t> switch_opt;
    size_t                      pos = 0;

    init_none_position_arg_value__(ret);

    for (const std::string& p : argv)
    {
        if (parse_none_position_arg__(ret, p, switch_opt))
        {
            continue;
        }

        argv_opt_t position_opt;

        if (!switch_opt)
        {
            if (pos >= position_arg__.size())
            {
                return {
                    sf_error { { err_parse,
                                 "too many postion argv" } },
                    json()
                };
            }
            position_opt = position_arg__[pos];
            ++pos;
        }
        else
        {
            position_opt = *switch_opt;
            switch_opt = nullptr;
        }

        auto postion_arg_result = parse_argv_value__(p, position_opt);

        if (sf_error(postion_arg_result))
        {
            if (position_opt.type == json_type::array)
            {
                ret[position_opt.json_name].append(postion_arg_result);
            }
            else
            {
                ret[position_opt.json_name] = json(postion_arg_result);
            }
        }
        else
        {
            return postion_arg_result;
        }
    }
    for (auto& p : none_position_arg__)
    {
        if (p.required)
        {
            if (!ret.has(p.json_name))
            {
                return {
                    sf_error { { err_not_enough,
                                 p.short_name + "/" + p.long_name + " is required" } },
                    json()
                };
            }
        }
    }
    for (auto& p : position_arg__)
    {
        if (p.required)
        {
            if (!ret.has(p.json_name))
            {
                return {
                    sf_error { { err_not_enough,
                                 p.short_name + "/" + p.long_name + " is required" } },
                    json()
                };
            }
        }
    }
    return { sf_error { { err_ok, "" } }, ret };
}
inline argv_result_t argparser::parse_argv(int argc, const char** argv, bool skip0)
{
    return parse_argv(std::vector<std::string>({ argv, argv + argc }), skip0);
}
inline void argparser::prepare_parser__(std::shared_ptr<argparser>& parser)
{
    if (parser->sub_parsers__.empty())
    {
        return;
    }
    for (auto& p : parser->sub_parsers__)
    {
        p.second->position_arg__.insert(p.second->position_arg__.end(), parser->position_arg__.begin(), parser->position_arg__.end());
        p.second->none_position_arg__.insert(p.second->none_position_arg__.end(), parser->none_position_arg__.begin(), parser->none_position_arg__.end());
        prepare_parser__(p.second);
    }
    parser->none_position_arg__.clear();
    parser->position_arg__.clear();
}
inline argv_result_t argparser::parse_argv(const std::vector<std::string>& args, bool skip0)
{
    std::vector<std::string> data({ args.begin() + skip0, args.end() });
    json                     ret;
    json                     curr = ret;
    curr.convert_to_object();
    auto parser = shared_from_this();
    argparser::prepare_parser__(parser);
    // NOTE 首先处理默认参数
    for (auto& p : none_position_arg__)
    {
        if (p.type == json_type::array)
        {
            ret[p.json_name].convert_to_array();
        }
        if (!p.default_value.is_null())
        {
            ret[p.json_name] = p.default_value;
        }
        if (p.action == argv_action::count)
        {
            ret[p.json_name] = 0;
        }
        if (p.action == argv_action::store_false)
        {
            ret[p.json_name] = true;
        }
        if (p.action == argv_action::store_true)
        {
            ret[p.json_name] = false;
        }
    }
    for (size_t i = 0; i < data.size(); ++i)
    {
        bool find_flag = false;
        for (auto& p : parser->sub_parsers__)
        {
            if (p.first == data[i])
            {
                find_flag     = true;
                curr[p.first] = json();
                curr.copy(curr[p.first]);
                curr.convert_to_object();
                parser = p.second;
                break;
            }
        }
        if (!find_flag)
        {
            auto result = parser->parse_argv__({ data.begin() + i, data.end() });
            if (sf_error(result).exp().code() != err_ok)
            {
                return result;
            }
            curr.join(json(result));
            return { sf_error { { err_ok, "" } }, ret };
        }
    }
    return { sf_error { { err_ok, "" } }, ret };
}
bool argparser::add_argument(std::string short_name, std::string long_name,
                             json_type type, bool required,
                             json default_value, std::string json_name,
                             argv_action action)
{
    argv_opt_t opt;
    opt.short_name    = short_name;
    opt.long_name     = long_name;
    opt.json_name     = json_name;
    opt.type          = type;
    opt.action        = action;
    opt.default_value = default_value;
    opt.required      = required;
    return add_argument(opt);
}
} // namespace skyfire