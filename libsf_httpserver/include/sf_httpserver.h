#pragma once

#include <iostream>
#include "sf_httpserver_config.h"
#include "sf_thread_pool.h"
#include "sf_eventloop.h"
#include "sf_tcpserver.h"
#include "sf_logger.h"
#include "sf_object.h"

#define cout std::cout
#define endl std::endl

namespace skyfire {

    class sf_http_server {
    private:
        const sf_httpserver_config config__;
        std::shared_ptr<sf_tcpserver> server__ = std::make_shared<sf_tcpserver>(true);
        sf_thread_pool pool__;

        void raw_data_coming__(SOCKET sock, const byte_array &data)
        {
            cout<<to_string(data)<<endl;
        }

    public:
        explicit sf_http_server(const sf_httpserver_config& config):
                config__ (config), pool__(config.thread_count)
        {
            sf_bind_signal(server__, raw_data_coming, [=](SOCKET sock, const byte_array &data){
                raw_data_coming__(sock,data);
            }, false);
        }

        bool start()
        {
            if(!server__->listen(config__.host,config__.port))
            {
                cout<<"监听失败"<<endl;
                return false;
            }
            else
            {
                cout<<"监听成功"<<endl;
            }
            for(auto i=0;i<config__.thread_count;++i)
            {
                pool__.add_task([=](){
                    sf_eventloop loop;
                    loop.exec();
                });
            }
            pool__.wait_all_task_finished();
        }
    };


}