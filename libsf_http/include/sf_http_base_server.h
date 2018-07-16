#pragma once

#include <functional>
#include "sf_http_server_config.h"
#include "sf_eventloop.h"
#include "sf_tcpserver.h"
#include "sf_logger.h"
#include "sf_object.h"
#include "sf_http_request.h"
#include "sf_http_response.h"

#include <iostream>

using namespace std;


namespace skyfire {

    class sf_http_base_server {
    private:
        const sf_http_server_config config__;
        std::shared_ptr<sf_tcpserver> server__ = std::make_shared<sf_tcpserver>(true);
        std::function<void(const sf_http_request&,sf_http_response&)> request_callback__;

        void raw_data_coming__(SOCKET sock, const byte_array &data)
        {
            cout<<"数据到来：\n"<<to_string(data)<<endl;
            sf_http_request request(data);
            if(request.is_valid())
            {
                if(request_callback__)
                {
                    sf_http_response res;
                    res.set_status(200);
                    res.set_reason("OK");
                    res.set_http_version(request.get_request_line().http_version);
                    request_callback__(request,res);
                    res.get_header().set_header("Content-Length", std::to_string(res.get_length()));
                    server__->send(sock,res.to_package());
                }
            }
            //server__->close(sock);
        }

    public:
        explicit sf_http_base_server(const sf_http_server_config& config):
                config__ (config)
        {
            sf_bind_signal(server__, raw_data_coming, [=](SOCKET sock, const byte_array &data){
                raw_data_coming__(sock,data);
            }, false);
        }

        bool start()
        {
            if(!server__->listen(config__.host,config__.port))
            {
                return false;
            }
            std::vector<std::thread> thread_vec;
            for(auto i=0;i<config__.thread_count;++i)
            {
                thread_vec.emplace_back(thread([=]{
                    sf_eventloop loop;
                    loop.exec();
                }));
            }
            for(auto &p:thread_vec)
            {
                p.join();
            }
            return true;
        }

        void set_request_callback(std::function<void(const sf_http_request&,sf_http_response&)> request_callback)
        {
            request_callback__ = request_callback;
        }
    };


}