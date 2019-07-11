#pragma once


#include "sf_stdc++.h"
#include "sf_json.hpp"


namespace skyfire {

    enum class sf_argv_action {
        store,
        store_true,
        store_false,
        count
    };

    struct sf_argv_opt_t {
        std::string short_name{};
        std::string long_name{};
        std::string json_name{};
        sf_json_type type = sf_json_type::string;
        bool required = true;
        sf_argv_action action{sf_argv_action::store};
        sf_json default_value{};
    };


    class sf_argparser : public std::enable_shared_from_this<sf_argparser> {
    private:
        std::unordered_map<std::string, std::shared_ptr<sf_argparser>> sub_parsers_;
        std::vector<sf_argv_opt_t> position_arg_;
        std::vector<sf_argv_opt_t> none_position_arg__;
        std::pair<std::string, std::string> prefix_;
        std::string help_;

        sf_json parse_argv__(const std::vector<std::string> &argv) const;

        sf_argparser(const std::string &help = "", const std::pair<std::string, std::string> &prefix = {"-", "--"});

    public:
        /**
         * 创建一个解析器对象
         * @param help 帮助信息
         * @param prefix 前缀
         * @return 解析器对象
         */
        static std::shared_ptr<sf_argparser>
        make_parser(const std::string &help = "", const std::pair<std::string, std::string> &prefix = {"-", "--"});

        bool add_argument(sf_argv_opt_t opt);

        bool add_argument(std::string short_name,
                          std::string long_name,
                          sf_json_type type = sf_json_type::string,
                          bool required = true,
                          sf_json default_value = sf_json(),
                          std::string json_name="",
                          sf_argv_action action = sf_argv_action::store);


        void add_sub_parser(const std::string &name, std::shared_ptr<sf_argparser> sub_parser);

        sf_json parse_argv(int argc, const char **argv, bool skip0 = true) const;

    };

}