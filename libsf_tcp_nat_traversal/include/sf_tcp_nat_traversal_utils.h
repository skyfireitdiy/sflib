#pragma once

#include <memory>
#include <utility>
#include "sf_tcpclient.h"
#include "sf_tcpserver.h"
#include "sf_type.h"
#include "sf_serialize.h"
#include "sf_netutils.h"
#include "sf_object.h"
#include "sf_nocopy.h"


#include "sf_logger.h"
static auto __logger__ = skyfire::sf_logger::get_instance();


namespace skyfire {

    // nat穿透连接上下文
    struct sf_tcp_nat_traversal_context_t__ {
        int connect_id;
        unsigned long long src_id;
        addr_info_t src_addr;
        unsigned long long dest_id;
        addr_info_t dest_addr;
        int error_code;
        int step;
        bool raw;
    };

    SF_MAKE_SERIALIZABLE(sf_tcp_nat_traversal_context_t__, connect_id, src_id, src_addr, dest_id, dest_addr, error_code,
                         step, raw)

    enum class sf_tcp_nat_traversal_connection_type {
        type_server_valid,
        type_client_valid
    };


    class sf_tcp_nat_traversal_client;

    class sf_tcp_nat_traversal_connection : public sf_nocopy<sf_object> {

    SF_REG_SIGNAL(data_coming, const pkg_header_t &, const byte_array &)

    SF_REG_SIGNAL(raw_data_coming, const byte_array &)

    SF_REG_SIGNAL(closed)

    private:

        std::shared_ptr<sf_tcpclient> client__;
        std::shared_ptr<sf_tcpserver> server__;
        SOCKET sock__;
        int connect_id__;
        sf_tcp_nat_traversal_connection_type type__;


        sf_tcp_nat_traversal_connection(std::shared_ptr<sf_tcpclient> client,
                                        std::shared_ptr<sf_tcpserver> server,
                                        int sock,
                                        int connect_id,
                                        sf_tcp_nat_traversal_connection_type type) :
                client__(std::move(client)),
                server__(std::move(server)),
                sock__(sock),
                connect_id__(connect_id),
                type__(type) {
            if (type__ == sf_tcp_nat_traversal_connection_type::type_client_valid) {
                sf_bind_signal(client__,
                               data_coming,
                               [=](const pkg_header_t &header, const byte_array &data) {
                                   data_coming(header, data);
                               }, true);
                sf_bind_signal(client__,
                               raw_data_coming,
                               [=](const byte_array &data) {
                                   raw_data_coming(data);
                               }, true);
                sf_bind_signal(client__,
                               closed,
                               [=]() {
                                   closed();
                               },
                               true);
            } else {
                sf_bind_signal(server__,
                               data_coming,
                               [=](SOCKET, const pkg_header_t &header, const byte_array &data) {
                                   data_coming(header, data);
                               }, true);
                sf_bind_signal(server__,
                               raw_data_coming,
                               [=](SOCKET, const byte_array &data) {
                                   raw_data_coming(data);
                               }, true);
                sf_bind_signal(server__,
                               closed,
                               [=](SOCKET) {
                                   closed();
                               },
                               true);
            }
        }

    public:

        bool send(int type, const byte_array &data) {
            if (type__ == sf_tcp_nat_traversal_connection_type::type_client_valid) {
                return client__->send(type, data);
            }
            return server__->send(sock__, type, data);
        }

        bool send(const byte_array &data) {
            if (type__ == sf_tcp_nat_traversal_connection_type::type_client_valid) {
                return client__->send(data);
            }
            return server__->send(sock__, data);
        }


        friend sf_tcp_nat_traversal_client;
    };

}

