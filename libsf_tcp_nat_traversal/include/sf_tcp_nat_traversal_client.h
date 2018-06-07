#pragma once

#include "sf_nocopy.h"
#include "sf_object.h"
#include "sf_tcpclient.h"
#include "sf_serialize.h"
#include "sf_define.h"
#include "sf_tcp_nat_traversal_utils.h"
#include "sf_random.h"
#include <climits>
#include <sf_tcpserver.h>

namespace skyfire {
    class sf_tcp_nat_traversal_client : public sf_nocopy<sf_object> {
    private:
        std::shared_ptr<sf_tcpclient> client__ {sf_tcpclient::make_client()};
        std::set<unsigned long long> client_list__;

        void on_data_coming(const pkg_header_t & header, const byte_array & data){
            switch (header.type){
                case TYPE_NAT_TRAVERSAL_LIST:
                    sf_deserialize(data, client_list__, 0);
                    break;
                default:
                    break;
            }
        }

        void on_close(){
            client_list__.clear();
        }

    public:
        static std::shared_ptr<sf_tcp_nat_traversal_client> make_client(){
            return std::make_shared<sf_tcp_nat_traversal_client>();
        }

        sf_tcp_nat_traversal_client(){
            sf_bind_signal(client__, data_coming, std::bind(&sf_tcp_nat_traversal_client::on_data_coming,
                                                            this,
                                                            std::placeholders::_1,
                                                            std::placeholders::_2),false);
            sf_bind_signal(client__, closed, std::bind(&sf_tcp_nat_traversal_client::on_close, this), false);
        }

        bool connect(const std::string& ip, unsigned short port){
            if(client__->connect(ip, port))
            {
                client__->send(TYPE_NAT_TRAVERSAL_REG, byte_array());
                return true;
            }
            return false;
        }

        std::set<unsigned long long> get_clients(){
            return client_list__;
        }

        /**
         * 连接远程peer端
         * @param peer_id 远端id
         * @return -1表示失败，其他表示此次连接的id
         */
        int connect_to_peer(unsigned long long peer_id){
            sf_tcp_nat_traversal_context_t__ context;
            context.connect_id = sf_random::get_instance()->get_int(0, INT_MAX);
            context.dest_id = peer_id;
            // TODO 本机需要做一些准备
            if(client__->send(TYPE_NAT_TRAVERSAL_REQUIRE_CONNECT_PEER, sf_serialize(context))) {
                return context.connect_id;
            }
            return  -1;
        }

        void close(){
            client__->close();
        }
    };
}