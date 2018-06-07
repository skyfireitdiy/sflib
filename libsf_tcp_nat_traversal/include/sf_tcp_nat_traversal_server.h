#pragma once

#include <set>

#include "sf_nocopy.h"
#include "sf_object.h"
#include "sf_tcpclient.h"
#include "sf_tcpserver.h"
#include "sf_define.h"
#include "sf_tcp_nat_traversal_utils.h"

namespace skyfire{
    class sf_tcp_nat_traversal_server : public sf_nocopy<sf_object>{
    private:
        std::set<SOCKET> clients__;
        std::shared_ptr<sf_tcpserver> server__{sf_tcpserver::make_server()};

        void on_new_connnection(SOCKET sock){
            // TODO 做一些处理
        }

        void on_disconnect(SOCKET sock){
            clients__.erase(sock);
        }

        void on_client_reg(SOCKET sock){
            clients__.insert(sock);
            on_update_client_list();
        }

        void on_update_client_list(SOCKET sock = static_cast<SOCKET>(-1)) {
            std::set<unsigned long long> tmp_data {clients__.begin(), clients__.end()};
            auto data = sf_serialize(tmp_data);
            if (sock == -1) {
                for (auto &p:clients__) {
                    server__->send(p, TYPE_NAT_TRAVERSAL_LIST, data);
                }
            } else {
                server__->send(sock, TYPE_NAT_TRAVERSAL_LIST, data);
            }
        }

        void on_msg_coming(SOCKET sock, const pkg_header_t& header, const byte_array& data){
            switch (header.type){
                case TYPE_NAT_TRAVERSAL_GET_LIST:
                    on_update_client_list(sock);
                    break;
                case TYPE_NAT_TRAVERSAL_REG:
                    on_client_reg(sock);
                    break;
                default:
                    break;
            }
        }

    public:

        static std::shared_ptr<sf_tcp_nat_traversal_server> make_server(){
            return std::make_shared<sf_tcp_nat_traversal_server>();
        }

        sf_tcp_nat_traversal_server(){
            sf_bind_signal(server__, new_connection, std::bind(&sf_tcp_nat_traversal_server::on_new_connnection,
                                                               this, std::placeholders::_1), false);
            sf_bind_signal(server__, closed, std::bind(&sf_tcp_nat_traversal_server::on_disconnect,
                                                       this, std::placeholders::_1), false);
            sf_bind_signal(server__, data_coming, std::bind(&sf_tcp_nat_traversal_server::on_msg_coming,
                                                       this, std::placeholders::_1,
                                                            std::placeholders::_2,
                                                            std::placeholders::_3), false);


        }

        bool listen(const std::string &ip, unsigned short port){
            return server__->listen(ip, port);
        }

        void close(){
            server__->close();
            clients__.clear();
        }
    };
}