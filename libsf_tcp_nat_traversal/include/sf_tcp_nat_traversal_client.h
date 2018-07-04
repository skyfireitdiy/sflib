#pragma once

#include "sf_nocopy.h"
#include "sf_object.h"
#include "sf_tcpclient.h"
#include "sf_tcpserver.h"
#include "sf_serialize.h"
#include "sf_define.h"
#include "sf_tcp_nat_traversal_utils.h"
#include "sf_random.h"
#include <climits>


namespace skyfire {

    //p2p客户端连接上下文
    struct sf_p2p_connect_context_t__ {
        sf_tcp_nat_traversal_context_t__ tcp_nat_traversal_context;
        std::shared_ptr<sf_tcpclient> point_a_client_1;
        std::shared_ptr<sf_tcpclient> point_a_client_2;
        std::shared_ptr<sf_tcpserver> point_a_server;
        std::shared_ptr<sf_tcpclient> point_b_client_1;
        std::shared_ptr<sf_tcpclient> point_b_client_2;
        std::shared_ptr<sf_tcpserver> point_b_server;
    };

    class sf_tcp_nat_traversal_client : public sf_nocopy<sf_object> {

        // 新连接，返回建立连接成功的tcpserver和已经连接上的socket，可通过这两个与被连接方通信
    SF_REG_SIGNAL(new_nat_connection, std::shared_ptr<sf_tcp_nat_traversal_connection>, int)

    private:
        std::shared_ptr<sf_tcpclient> client__{sf_tcpclient::make_client()};
        std::set<unsigned long long> client_list__;
        unsigned long long self_id__;
        std::map<int, sf_p2p_connect_context_t__> connect_context_map__;
        addr_info_t server_addr__;


        /**
         * 目的，有远程主机请求建立连接
         * @param context
         */
        void on_new_connect_required__(sf_tcp_nat_traversal_context_t__ &context) {
            // 创建连接上下文
            connect_context_map__[context.connect_id] = sf_p2p_connect_context_t__{};
            // 保存连接上下文
            connect_context_map__[context.connect_id].tcp_nat_traversal_context = context;

            // 第2步，接受端尝试连接A的外网端口，如果连上，一切ok，context有效字段：connect_id、dest_id、src_id、src_addr
            connect_context_map__[context.connect_id].tcp_nat_traversal_context.step = 2;

            // 建立客户端1
            connect_context_map__[context.connect_id].point_b_client_1 = sf_tcpclient::make_client();

            addr_info_t server_addr;
            if (!get_local_addr(client__->get_raw_socket(), server_addr)) {
                context.error_code = SF_ERR_DISCONNECT;
                client__->send(TYPE_NAT_TRAVERSAL_ERROR,
                               sf_serialize(connect_context_map__[context.connect_id].tcp_nat_traversal_context));
                return;
            }

            // 生成一个自动端口
            unsigned short auto_port = static_cast<unsigned short>(sf_random::get_instance()->get_int(1025, 65535));

            __logger__->sf_debug("生成本地连接地址",server_addr.ip,auto_port);

            // 绑定
            if (!connect_context_map__[context.connect_id].point_b_client_1->bind(server_addr.ip, auto_port)) {
                context.error_code = SF_ERR_BIND_ERR;
                client__->send(TYPE_NAT_TRAVERSAL_ERROR,
                               sf_serialize(connect_context_map__[context.connect_id].tcp_nat_traversal_context));
                return;
            }


            __logger__->sf_debug("尝试连接a的外网ip端口，留下路由信息",
                                 connect_context_map__[context.connect_id].tcp_nat_traversal_context.src_addr.ip,
                                 connect_context_map__[context.connect_id].tcp_nat_traversal_context.src_addr.port);
            // 尝试连接a的外网ip端口，留下路由信息（一般会失败，但有时会直接成功）
            if (connect_context_map__[context.connect_id].point_b_client_1->connect(
                    connect_context_map__[context.connect_id].tcp_nat_traversal_context.src_addr.ip,
                    connect_context_map__[context.connect_id].tcp_nat_traversal_context.src_addr.port
            )) {
                // TODO 为了维持操作统一，成功暂时也不做处理
                __logger__->sf_debug("成功连接到来源端口");
            }
            // 连接失败，B建立服务器2

            // 第3步：接受端建立socket，发送信息，便于server获取到接受端地址信息，同时监听端口，context有效字段：connect_id、dest_id、src_id、src_addr
            connect_context_map__[context.connect_id].tcp_nat_traversal_context.step = 3;

            connect_context_map__[context.connect_id].point_b_client_2 = sf_tcpclient::make_client();

            // 将客户端2绑定至于客户端1相同的ip端口
            if (!connect_context_map__[context.connect_id].point_b_client_2->bind(
                    server_addr.ip, auto_port
            )) {
                context.error_code = SF_ERR_BIND_ERR;
                client__->send(TYPE_NAT_TRAVERSAL_ERROR,
                               sf_serialize(connect_context_map__[context.connect_id].tcp_nat_traversal_context));
                return;
            }

            // 监听端口，等待连接
            connect_context_map__[context.connect_id].point_b_server = sf_tcpserver::make_server(context.raw);
            if (!connect_context_map__[context.connect_id].point_b_server->listen(server_addr.ip, auto_port)) {
                context.error_code = SF_ERR_DISCONNECT;
                client__->send(TYPE_NAT_TRAVERSAL_ERROR,
                               sf_serialize(connect_context_map__[context.connect_id].tcp_nat_traversal_context));
                return;
            }

            // lambda 里面需要此绑定id，所以需要传递指针
            std::shared_ptr<int> tmp_bind_id = std::make_shared<int>();
            *tmp_bind_id = sf_bind_signal(connect_context_map__[context.connect_id].point_b_server,
                                          new_connection,
                                          [=](SOCKET sock) {
                                              __logger__->sf_debug("B端新连接到来，NAT穿透连接建立成功");
                                              std::shared_ptr<sf_tcp_nat_traversal_connection> connection(
                                                      new sf_tcp_nat_traversal_connection(
                                                              nullptr,
                                                              connect_context_map__[context.connect_id].point_b_server,
                                                              sock,
                                                              context.connect_id,
                                                              sf_tcp_nat_traversal_connection_type::type_server_valid
                                                      )
                                              );
                                              new_nat_connection(connection, context.connect_id);
                                              // 解除绑定
                                              sf_unbind_signal(connect_context_map__[context.connect_id].point_b_server,
                                                               new_connection,
                                                               *tmp_bind_id
                                              );
                                              __logger__->sf_debug("解绑id", *tmp_bind_id);
                                              connect_context_map__.erase(context.connect_id);
                                          },
                                          true
            );

            __logger__->sf_debug("绑定id", *tmp_bind_id);

            // 使用客户端2连接至server，便于server获取目的的外网ip端口
            if (!connect_context_map__[context.connect_id].point_b_client_2->connect(server_addr__.ip,
                                                                                     server_addr__.port)) {
                context.error_code = SF_ERR_DISCONNECT;
                client__->send(TYPE_NAT_TRAVERSAL_ERROR,
                               sf_serialize(connect_context_map__[context.connect_id].tcp_nat_traversal_context));
                return;
            }

            // 向server发送信息，server获取ip端口（此时已在路由器留下记录）
            __logger__->sf_debug("回复服务器");
            if (!connect_context_map__[context.connect_id].point_b_client_2->send(
                    TYPE_NAT_TRAVERSAL_B_REPLY_ADDR,
                    sf_serialize(
                            connect_context_map__[context.connect_id].tcp_nat_traversal_context))
                    ) {
                context.error_code = SF_ERR_DISCONNECT;
                client__->send(TYPE_NAT_TRAVERSAL_ERROR,
                               sf_serialize(connect_context_map__[context.connect_id].tcp_nat_traversal_context));
                return;
            }


        }

        void on_server_reply_b_addr(sf_tcp_nat_traversal_context_t__ &context) {
            // 第5步，a连接b的监听端口,context有效字段：connect_id、dest_id、src_id、src_addr、dest_addr
            context.step = 5;
            if (connect_context_map__.count(context.connect_id) == 0) {
                context.error_code = SF_ERR_NOT_EXIST;
                client__->send(TYPE_NAT_TRAVERSAL_ERROR, sf_serialize(context));
                return;
            }
            connect_context_map__[context.connect_id].tcp_nat_traversal_context = context;
            connect_context_map__[context.connect_id].point_a_client_2 = sf_tcpclient::make_client(context.raw);
            __logger__->sf_debug("连接B的端口");
            if (connect_context_map__[context.connect_id].point_a_client_2->connect(context.dest_addr.ip,
                                                                                    context.dest_addr.port)) {
                std::shared_ptr<sf_tcp_nat_traversal_connection> connection(new sf_tcp_nat_traversal_connection(
                        connect_context_map__[context.connect_id].point_a_client_2,
                        nullptr,
                        -1,
                        context.connect_id,
                        sf_tcp_nat_traversal_connection_type::type_client_valid
                ));
                __logger__->sf_debug("NAT穿透连接成功");
                new_nat_connection(connection, context.connect_id);
                connect_context_map__.erase(context.connect_id);
            } else {
                context.error_code = SF_ERR_DISCONNECT;
                client__->send(TYPE_NAT_TRAVERSAL_ERROR, sf_serialize(context));
                return;
            }
        }


        void on_client_data_coming__(const pkg_header_t &header, const byte_array &data) {
            __logger__->sf_debug(header.type);
            switch (header.type) {
                case TYPE_NAT_TRAVERSAL_LIST:
                    sf_deserialize(data, client_list__, 0);
                    break;
                case TYPE_NAT_TRAVERSAL_SET_ID:
                    sf_deserialize(data, self_id__, 0);
                    break;
                case TYPE_NAT_TRAVERSAL_NEW_CONNECT_REQUIRED: {
                    __logger__->sf_debug("收到连接请求");
                    sf_tcp_nat_traversal_context_t__ context;
                    sf_deserialize(data, context, 0);
                    on_new_connect_required__(context);
                }
                    break;
                case TYPE_NAT_TRAVERSAL_SERVER_REPLY_B_ADDR: {
                    __logger__->sf_debug("收到B的地址");
                    sf_tcp_nat_traversal_context_t__ context;
                    sf_deserialize(data, context, 0);
                    on_server_reply_b_addr(context);
                }
                    break;
                default:
                    break;
            }
        }

        void on_client_close__() {
            client_list__.clear();
        }


    public:

        static std::shared_ptr<sf_tcp_nat_traversal_client> make_client() {
            return std::make_shared<sf_tcp_nat_traversal_client>();
        }

        unsigned long long int get_id(){
            return self_id__;
        }

        sf_tcp_nat_traversal_client() {
            sf_bind_signal(client__, data_coming, [=](const pkg_header_t &header, const byte_array &data) {
                on_client_data_coming__(header, data);
            }, true);
            sf_bind_signal(client__, closed, [=] { close(); }, true);
        }

        /**
         * 连接到穿透服务器，连接成功后会注册
         * @param ip 穿透服务器ip
         * @param port 穿透服务器端口
         * @return 连接结果
         */
        bool connect(const std::string &ip, unsigned short port) {
            server_addr__.ip = ip;
            server_addr__.port = port;
            if (client__->connect(ip, port)) {
                client__->send(TYPE_NAT_TRAVERSAL_REG, byte_array());
                return true;
            }
            return false;
        }

        /**
         * 获取终端集合
         * @return 终端集合
         */
        std::set<unsigned long long> get_clients() {
            return client_list__;
        }

        /**
         * 来源，连接远程peer端
         * @param peer_id 远端id
         * @return -1表示失败，其他表示此次连接的id
         */
        int connect_to_peer(unsigned long long peer_id, bool raw) {
            // 生成一个连接上下文
            sf_p2p_connect_context_t__ tmp_p2p_conn_context;
            // 第0步：发起端主动连接server，context有效字段：connect_id、dest_id、src_id
            tmp_p2p_conn_context.tcp_nat_traversal_context.step = 0;
            // 生成随机连接id
            tmp_p2p_conn_context.tcp_nat_traversal_context.connect_id = sf_random::get_instance()->get_int(0, INT_MAX);
            // 指定对方id
            tmp_p2p_conn_context.tcp_nat_traversal_context.dest_id = peer_id;
            // 指定自身id
            tmp_p2p_conn_context.tcp_nat_traversal_context.src_id = self_id__;
            // 原始数据传输
            tmp_p2p_conn_context.tcp_nat_traversal_context.raw = raw;
            // 生成连接a客户端
            tmp_p2p_conn_context.point_a_client_1 = sf_tcpclient::make_client();
            // 尝试连接服务器
            if (tmp_p2p_conn_context.point_a_client_1->connect(server_addr__.ip, server_addr__.port)) {

                addr_info_t addr;
                // 获取到本机要连接外网的ip端口，保存至连接上下文
                if (!get_local_addr(tmp_p2p_conn_context.point_a_client_1->get_raw_socket(), addr)) {
                    __logger__->sf_debug("获取本地ip端口失败");
                    return -1;
                }

                tmp_p2p_conn_context.point_a_server = sf_tcpserver::make_server();
                // 复用刚才断开的连接1端口号
                if (!tmp_p2p_conn_context.point_a_server->listen(addr.ip, addr.port)) {
                    __logger__->sf_debug("监听本地ip端口失败", addr.ip, addr.port);
                    return -1;
                }


                __logger__->sf_debug("发起连接请求");
                // 发起连接请求
                if (tmp_p2p_conn_context.point_a_client_1->send(TYPE_NAT_TRAVERSAL_REQUIRE_CONNECT_PEER,
                                                                sf_serialize(
                                                                        tmp_p2p_conn_context.tcp_nat_traversal_context))) {
                    // 保存连接上下文
                    connect_context_map__[tmp_p2p_conn_context.tcp_nat_traversal_context.connect_id] = tmp_p2p_conn_context;
                    return tmp_p2p_conn_context.tcp_nat_traversal_context.connect_id;
                }
            }
            return -1;
        }

        void close() {
            client__->close();
        }
    };
}