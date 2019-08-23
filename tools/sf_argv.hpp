#pragma once

#include "sf_argv.h"
#include "sf_utils.hpp"

namespace skyfire {
inline sf_argparser::sf_argparser(
    const std::string &help, const std::pair<std::string, std::string> &prefix)
    : help_(help), prefix_(prefix) {}

inline std::shared_ptr<sf_argparser> sf_argparser::make_parser(
    const std::string &help,
    const std::pair<std::string, std::string> &prefix) {
    return std::shared_ptr<sf_argparser>(new sf_argparser(help, prefix));
}

inline void sf_argparser::add_sub_parser(
    const std::string &name,
    std::shared_ptr<skyfire::sf_argparser> sub_parser) {
    sub_parsers_[name] = sub_parser;
}

inline bool sf_argparser::add_argument(skyfire::sf_argv_opt_t opt) {
    if (opt.json_name.empty()) {
        opt.json_name = opt.long_name;
    }
    if (opt.json_name.empty()) {
        opt.json_name = opt.short_name;
    }
    if (opt.json_name.empty()) {
        return false;
    }
    for (sf_argv_opt_t &p : none_position_arg__) {
        if (opt.json_name == p.json_name) {
            return false;
        }
    }
    for (sf_argv_opt_t &p : position_arg_) {
        if (opt.json_name == p.json_name) {
            return false;
        }
    }
    if (sf_string_start_with(opt.short_name, prefix_.first) &&
        sf_string_start_with(opt.long_name, prefix_.second)) {
        for (sf_argv_opt_t &p : none_position_arg__) {
            if (p.short_name == opt.short_name) {
                return false;
            } else if (p.long_name == opt.short_name) {
                return false;
            } else if (p.short_name == opt.long_name) {
                return false;
            } else if (p.long_name == opt.long_name) {
                return false;
            }
        }
        none_position_arg__.push_back(opt);
    } else {
        for (sf_argv_opt_t &p : position_arg_) {
            if (p.short_name == opt.short_name) {
                return false;
            }
        }
        position_arg_.push_back(opt);
    }
    return true;
}

inline sf_json sf_argparser::parse_argv__(
    const std::vector<std::string> &argv) const {
    sf_json ret;
    ret.convert_to_object();
    std::shared_ptr<sf_argv_opt_t> last_opt;
    int pos = 0;
    for (auto &p : none_position_arg__) {
        if (!p.default_value.is_null()) {
            ret[p.json_name] = p.default_value;
        }
        if (p.action == sf_argv_action::count) {
            ret[p.json_name] = 0;
        }
        if (p.action == sf_argv_action::store_false) {
            ret[p.json_name] = true;
        }
        if (p.action == sf_argv_action::store_true) {
            ret[p.json_name] = false;
        }
    }
    for (const std::string &p : argv) {
        if (!last_opt) {
            bool find_flag = false;
            for (const sf_argv_opt_t &nopt : none_position_arg__) {
                if (nopt.long_name == p || nopt.short_name == p) {
                    find_flag = true;
                    if (nopt.action == sf_argv_action::store_true) {
                        ret[nopt.json_name] = true;
                        break;
                    } else if (nopt.action == sf_argv_action::store_false) {
                        ret[nopt.json_name] = false;
                        break;
                    } else if (nopt.action == sf_argv_action::count) {
                        ret[nopt.json_name] =
                            static_cast<int>(ret[nopt.json_name]) + 1;
                        break;
                    }
                    last_opt =
                        std::shared_ptr<sf_argv_opt_t>(new sf_argv_opt_t(nopt));
                    break;
                }
            }
            if (find_flag) {
                continue;
            }
            if (pos >= position_arg_.size()) {
                sf_debug("parse error");
                return sf_json();
            }
            auto d = sf_json();
            last_opt = std::shared_ptr<sf_argv_opt_t>(
                new sf_argv_opt_t(position_arg_[pos]));
            ++pos;
            switch (last_opt->type) {
                case sf_json_type::string:
                    d = p;
                    ret[last_opt->json_name] = d;
                    last_opt = nullptr;
                    break;
                case sf_json_type::array:
                    d = p;
                    ret[last_opt->json_name].append(sf_json(p));
                    break;
                case sf_json_type::boolean:
                    d = !(p == "0" || p == "false");
                    ret[last_opt->json_name] = d;
                    last_opt = nullptr;
                    break;
                case sf_json_type::number:
                    d = sf_string_to_long_double(p);
                    ret[last_opt->json_name] = d;
                    last_opt = nullptr;
                    break;
                default:
                    sf_debug("unsupport");
                    return sf_json();
            }
        } else {
            auto d = sf_json();

            if (last_opt->type == sf_json_type::array) {
                auto find_flag = false;
                for (const sf_argv_opt_t &nopt : none_position_arg__) {
                    if (nopt.long_name == p || nopt.short_name == p) {
                        find_flag = true;
                        if (nopt.action == sf_argv_action::store_true) {
                            ret[nopt.json_name] = true;
                            break;
                        } else if (nopt.action == sf_argv_action::store_false) {
                            ret[nopt.json_name] = false;
                            break;
                        } else if (nopt.action == sf_argv_action::count) {
                            ret[nopt.json_name] =
                                static_cast<int>(ret[nopt.json_name]) + 1;
                            break;
                        }
                        last_opt = std::shared_ptr<sf_argv_opt_t>(
                            new sf_argv_opt_t(nopt));
                        break;
                    }
                }
                if (find_flag) {
                    continue;
                }
            }

            switch (last_opt->type) {
                case sf_json_type::string:
                    d = p;
                    ret[last_opt->json_name] = d;
                    last_opt = nullptr;
                    break;
                case sf_json_type::array:
                    d = p;
                    ret[last_opt->json_name].append(d);
                    break;
                case sf_json_type::boolean:
                    d = !(p == "0" || p == "false");
                    ret[last_opt->json_name] = d;
                    last_opt = nullptr;
                    break;
                case sf_json_type::number:
                    d = sf_string_to_long_double(p);
                    ret[last_opt->json_name] = d;
                    last_opt = nullptr;
                    break;
                default:
                    sf_debug("unsupport");
                    return sf_json();
            }
        }
    }
    for (auto &p : none_position_arg__) {
        if (p.required) {
            if (!ret.has(p.json_name)) {
                sf_debug(p.short_name + "/" + p.long_name + " is required");
                return sf_json();
            }
        }
    }
    for (auto &p : position_arg_) {
        if (p.required) {
            if (!ret.has(p.json_name)) {
                sf_debug(p.short_name + "/" + p.long_name + " is required");
                return sf_json();
            }
        }
    }
    return ret;
}

inline sf_json sf_argparser::parse_argv(int argc, const char **argv,
                                        bool skip0) const {
    std::vector<std::string> data({argv + skip0, argv + argc});
    sf_json ret;
    sf_json curr = ret;
    curr.convert_to_object();
    auto parser = shared_from_this();
    for (int i = 0; i < data.size(); ++i) {
        bool find_flag = false;
        for (auto &p : parser->sub_parsers_) {
            if (p.first == data[i]) {
                find_flag = true;
                curr[p.first] = sf_json();
                curr.copy(curr[p.first]);
                curr.convert_to_object();
                parser = p.second;
                break;
            }
        }
        if (!find_flag) {
            curr.join(parser->parse_argv__({data.begin() + i, data.end()}));
            return ret;
        }
    }
    return sf_json();
}

bool sf_argparser::add_argument(std::string short_name, std::string long_name,
                                sf_json_type type, bool required,
                                sf_json default_value, std::string json_name,
                                sf_argv_action action) {
    sf_argv_opt_t opt;
    opt.short_name = short_name;
    opt.long_name = long_name;
    opt.json_name = json_name;
    opt.type = type;
    opt.action = action;
    opt.default_value = default_value;
    opt.required = required;
    return add_argument(opt);
}

}    // namespace skyfire