#pragma once

#include "sf_http_base_server.h"
#include "sf_http_router.h"
#include <set>

#include <iostream>

using namespace std;

namespace skyfire
{
    class sf_http_server: public sf_http_base_server
    {
    private:
        std::multiset<sf_http_router> routers__;
        void default_callback__(const sf_http_request& req,sf_http_response& res)
        {
            auto req_line = req.get_request_line();
            cout<<req_line.url<<endl;
            for(auto &p:routers__)
            {
                if(p.run_route(req,res,req_line.url,req_line.method))
                    return;
            }
        }
    public:
        explicit sf_http_server(const sf_http_server_config& config):sf_http_base_server(config)
        {
            set_request_callback([=](const sf_http_request& req,sf_http_response& res){
                default_callback__(req,res);
            });
        }

        void add_router(sf_http_router router, int priority = 0)
        {
            routers__.insert(router);
        }
    };
}