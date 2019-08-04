#pragma once

#include "core/sf_stdc++.h"
#include "tools/sf_json.h"

namespace skyfire
{
    struct sf_database_config_t
    {
        std::string host;
        short port;
        std::string user;
        std::string password;
    };

    SF_JSONIFY(sf_database_config_t, host, port, user, password)

    class sf_database_table_interface
    {
    public:
        // TODO 增删改查
    };

    class sf_database_interface{
    public:
        virtual get_table(const std::string& tb_name) = 0;
        // TODO 创建表
    };

    class sf_database_connection{
    public:
        virtual std::shared_ptr<sf_database_interface> get_database(const std::string & db_name) = 0;
        virtual bool create_database(const std::string& db_name) = 0;
    };

    class sf_database_control_interface
    {
    public:
        virtual std::shared_ptr<> get_connection(const sf_database_config_t& config) = 0;
    };
}