#pragma once

#include <set>

#include "sf_nocopy.h"
#include "sf_object.h"
#include "sf_tcpclient.h"
#include "sf_tcpserver.h"

namespace skyfire{
    class sf_tcp_nat_traversal_server : public sf_nocopy<sf_object>{
    private:
        std::set<SOCKET> clients__;
        std::shared_ptr<sf_tcpserver> server__{sf_tcpserver::make_server()};

        void on_new_connnection(SOCKET sock){
            clients__.insert(sock);
        }

        void on_disconnect(SOCKET sock){
            clients__.erase(sock);
        }

        void on_msg_coming(SOCKET sock, const pkg_header_t& header, const byte_array& data){

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