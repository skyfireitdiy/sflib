
#pragma once
#include "sf_argv.h"
#include "sf_error.h"
#include "sf_utils.hpp"
namespace skyfire
{
inline argparser::argparser(
    const std::string& help)
    : help_(help)
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
    sub_parsers_[name] = sub_parser;
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
inline bool argparser::add_argument(argv_opt_t opt)
{
    if (opt.json_name.empty())
    {
        opt.json_name = opt.long_name;
    }
    if (opt.json_name.empty())
    {
        opt.json_name = opt.short_name;
    }
    if (opt.json_name.empty())
    {
        return false;
    }
    for (argv_opt_t& p : none_position_arg__)
    {
        if (opt.json_name == p.json_name)
        {
            return false;
        }
    }
    for (argv_opt_t& p : position_arg_)
    {
        if (opt.json_name == p.json_name)
        {
            return false;
        }
    }
    if (string_start_with(opt.short_name, prefix_.first) && string_start_with(opt.long_name, prefix_.second))
    {
        for (argv_opt_t& p : none_position_arg__)
        {
            if (p.short_name == opt.short_name)
            {
                return false;
            }
            else if (p.long_name == opt.short_name)
            {
                return false;
            }
            else if (p.short_name == opt.long_name)
            {
                return false;
            }
            else if (p.long_name == opt.long_name)
            {
                return false;
            }
        }
        none_position_arg__.push_back(opt);
    }
    else
    {
        for (argv_opt_t& p : position_arg_)
        {
            if (p.long_name == opt.long_name)
            {
                return false;
            }
        }
        position_arg_.push_back(opt);
    }
    return true;
}
inline argv_result_t argparser::parse_argv__(
    const std::vector<std::string>& argv)
{
    json ret;
    ret.convert_to_object();
    std::shared_ptr<argv_opt_t> last_opt;
    size_t                      pos = 0;
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
    for (const std::string& p : argv)
    {
        if (!last_opt)
        {
            bool find_flag = false;
            for (const argv_opt_t& nopt : none_position_arg__)
            {
                if (nopt.long_name == p || nopt.short_name == p)
                {
                    find_flag = true;
                    if (nopt.action == argv_action::store_true)
                    {
                        ret[nopt.json_name] = true;
                        break;
                    }
                    else if (nopt.action == argv_action::store_false)
                    {
                        ret[nopt.json_name] = false;
                        break;
                    }
                    else if (nopt.action == argv_action::count)
                    {
                        ret[nopt.json_name] = static_cast<int>(ret[nopt.json_name]) + 1;
                        break;
                    }
                    last_opt = std::shared_ptr<argv_opt_t>(new argv_opt_t(nopt));
                    break;
                }
            }
            if (find_flag)
            {
                continue;
            }
            if (pos >= position_arg_.size())
            {
                return {
                    sf_error { { err_parse,
                                 "too many postion argv" } },
                    json()
                };
            }
            auto d   = json();
            last_opt = std::shared_ptr<argv_opt_t>(
                new argv_opt_t(position_arg_[pos]));
            ++pos;
            switch (last_opt->type)
            {
            case json_type::string:
                d                        = p;
                ret[last_opt->json_name] = d;
                last_opt                 = nullptr;
                break;
            case json_type::array:
                d = p;
                ret[last_opt->json_name].append(json(p));
                break;
            case json_type::boolean:
                d                        = !(p == "0" || p == "false");
                ret[last_opt->json_name] = d;
                last_opt                 = nullptr;
                break;
            case json_type::number:
                d                        = string_to_long_double(p);
                ret[last_opt->json_name] = d;
                last_opt                 = nullptr;
                break;
            default:
                return argv_result_t {
                    sf_error { { err_unsupport_type, "unsupport:" + std::to_string(static_cast<int>(last_opt->type)) } }, json {}
                };
            }
        }
        else
        {
            auto d = json();
            if (last_opt->type == json_type::array)
            {
                auto find_flag = false;
                for (const argv_opt_t& nopt : none_position_arg__)
                {
                    if (nopt.long_name == p || nopt.short_name == p)
                    {
                        find_flag = true;
                        if (nopt.action == argv_action::store_true)
                        {
                            ret[nopt.json_name] = true;
                            break;
                        }
                        else if (nopt.action == argv_action::store_false)
                        {
                            ret[nopt.json_name] = false;
                            break;
                        }
                        else if (nopt.action == argv_action::count)
                        {
                            ret[nopt.json_name] = static_cast<int>(ret[nopt.json_name]) + 1;
                            break;
                        }
                        last_opt = std::shared_ptr<argv_opt_t>(
                            new argv_opt_t(nopt));
                        break;
                    }
                }
                if (find_flag)
                {
                    continue;
                }
            }
            switch (last_opt->type)
            {
            case json_type::string:
                d                        = p;
                ret[last_opt->json_name] = d;
                last_opt                 = nullptr;
                break;
            case json_type::array:
                d = p;
                ret[last_opt->json_name].append(d);
                break;
            case json_type::boolean:
                d                        = !(p == "0" || p == "false");
                ret[last_opt->json_name] = d;
                last_opt                 = nullptr;
                break;
            case json_type::number:
                d                        = string_to_long_double(p);
                ret[last_opt->json_name] = d;
                last_opt                 = nullptr;
                break;
            default:
                return argv_result_t { sf_error { { err_unsupport_type, "unsupport:" + std::to_string(static_cast<int>(last_opt->type)) } }, json {} };
            }
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
    for (auto& p : position_arg_)
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
inline void prepare_parser__(std::shared_ptr<argparser>& parser)
{
    if (parser->sub_parsers_.empty())
    {
        return;
    }
    for (auto& p : parser->sub_parsers_)
    {
        p.second->position_arg_.insert(p.second->position_arg_.end(), parser->position_arg_.begin(), parser->position_arg_.end());
        p.second->none_position_arg__.insert(p.second->none_position_arg__.end(), parser->none_position_arg__.begin(), parser->none_position_arg__.end());
        prepare_parser__(p.second);
    }
    parser->none_position_arg__.clear();
    parser->position_arg_.clear();
}
inline argv_result_t argparser::parse_argv(const std::vector<std::string>& args, bool skip0)
{
    std::vector<std::string> data({ args.begin() + skip0, args.end() });
    json                     ret;
    json                     curr = ret;
    curr.convert_to_object();
    auto parser = shared_from_this();
    prepare_parser__(parser);
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
        for (auto& p : parser->sub_parsers_)
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