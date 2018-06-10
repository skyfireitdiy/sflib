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
    struct sf_p2p_connect_context_t__{
        sf_tcp_nat_traversal_context_t__ tcp_nat_traversal_context;
        std::shared_ptr<sf_tcpclient> point_a_client_1;
        std::shared_ptr<sf_tcpclient> point_a_client_2;
        std::shared_ptr<sf_tcpserver> point_a_server;
        std::shared_ptr<sf_tcpclient> point_b_client_1;
        std::shared_ptr<sf_tcpclient> point_b_client_2;
        std::shared_ptr<sf_tcpserver> point_b_server;
    };

    class sf_tcp_nat_traversal_client : public sf_nocopy<sf_object> {
    private:
        std::shared_ptr<sf_tcpclient> client__ {sf_tcpclient::make_client()};
        std::set<unsigned long long> client_list__;
        unsigned long long self_id__;
        std::map<int, sf_p2p_connect_context_t__> connect_context_map__;
        addr_info_t server_addr__;

        /**
         * 来源，阶段1成功处理
         * @param context
         */
        void on_step_1_ok__(sf_tcp_nat_traversal_context_t__& context)
        {
            // 检测连接是否存在
            if(connect_context_map__.count(context.connect_id) == 0)
            {
                context.error_code = SF_ERR_NOT_EXIST;
                client__->send(TYPE_NAT_TRAVERSAL_ERROR, sf_serialize(context));
                return;
            }
            // 保存上下文
            connect_context_map__[context.connect_id].tcp_nat_traversal_context = context;
            addr_info_t addr;
            // 获取到本机要连接外网的ip端口，保存至连接上下文
            if(!get_local_addr(connect_context_map__[context.connect_id].point_a_client_1->get_raw_socket(), addr))
            {
                context.error_code = SF_ERR_DISCONNECT;
                client__->send(TYPE_NAT_TRAVERSAL_ERROR, sf_serialize(context));
                connect_context_map__.erase(context.connect_id);
                return;
            }
            // 关闭与server的连接1
            connect_context_map__[context.connect_id].point_a_client_1->close();
            // 创建服务器
            connect_context_map__[context.connect_id].point_a_server = sf_tcpserver::make_server();
            // 复用刚才断开的连接1端口号
            if(!connect_context_map__[context.connect_id].point_a_server->listen(addr.ip, addr.port))
            {
                context.error_code = SF_ERR_LISTEN_ERR;
                client__->send(TYPE_NAT_TRAVERSAL_ERROR, sf_serialize(context));
                connect_context_map__.erase(context.connect_id);
                return;
            }
        }

        /**
         * 目的，有远程主机请求建立连接
         * @param context
         */
        void on_new_connect_required__(sf_tcp_nat_traversal_context_t__& context) {
            // 创建连接上下文
            connect_context_map__[context.connect_id] = sf_p2p_connect_context_t__{};
            // 保存连接上下文
            connect_context_map__[context.connect_id].tcp_nat_traversal_context = context;
            // 建立客户端1
            connect_context_map__[context.connect_id].point_b_client_1 = sf_tcpclient::make_client();
            // 使用客户端1连接至server，便于server获取目的的外网ip端口
            if (!connect_context_map__[context.connect_id].point_b_client_1->connect(server_addr__.ip,
                                                                                     server_addr__.port)) {
                context.error_code = SF_ERR_DISCONNECT;
                client__->send(TYPE_NAT_TRAVERSAL_ERROR,
                               sf_serialize(connect_context_map__[context.connect_id].tcp_nat_traversal_context));
                return;
            }
            // 第2步：接受端连接server，发送信息，便于server获取到接受端地址信息，context有效字段：connect_id、dest_id、src_id、src_addr
            connect_context_map__[context.connect_id].tcp_nat_traversal_context.step = 2;
            // 向server发送信息，server获取ip端口
            if (!connect_context_map__[context.connect_id].point_b_client_1->send(TYPE_NAT_TRAVERSAL_B_REPLY_ADDR,
                                                                                  sf_serialize(
                                                                                          connect_context_map__[context.connect_id].tcp_nat_traversal_context))) {
                context.error_code = SF_ERR_DISCONNECT;
                client__->send(TYPE_NAT_TRAVERSAL_ERROR,
                               sf_serialize(connect_context_map__[context.connect_id].tcp_nat_traversal_context));
                return;
            }

            // 建立客户端2
            connect_context_map__[context.connect_id].point_b_client_2 = sf_tcpclient::make_client();
            addr_info_t addr;
            // 获取客户端1的本地ip端口
            if (!get_local_addr(connect_context_map__[context.connect_id].point_b_client_1->get_raw_socket(), addr)) {
                context.error_code = SF_ERR_DISCONNECT;
                client__->send(TYPE_NAT_TRAVERSAL_ERROR,
                               sf_serialize(connect_context_map__[context.connect_id].tcp_nat_traversal_context));
                return;
            }
            // 将客户端2绑定至于客户端1相同的ip端口
            if (!connect_context_map__[context.connect_id].point_b_client_2->bind(
                    addr.ip, addr.port
            )) {
                context.error_code = SF_ERR_BIND_ERR;
                client__->send(TYPE_NAT_TRAVERSAL_ERROR,
                               sf_serialize(connect_context_map__[context.connect_id].tcp_nat_traversal_context));
                return;
            }
            // 尝试连接a的外网ip端口，留下路由信息（一般会失败，不用关心返回结果）
            connect_context_map__[context.connect_id].point_b_client_2->connect(
                    connect_context_map__[context.connect_id].tcp_nat_traversal_context.src_addr.ip,
                    connect_context_map__[context.connect_id].tcp_nat_traversal_context.src_addr.port
                    );
        }


        void on_client_data_coming__(const pkg_header_t &header, const byte_array &data){
            switch (header.type){
                case TYPE_NAT_TRAVERSAL_LIST:
                    sf_deserialize(data, client_list__, 0);
                    break;
                case TYPE_NAT_TRAVERSAL_SET_ID:
                    sf_deserialize(data, self_id__, 0);
                    break;
                case TYPE_NAT_TRAVERSAL_STEP_1_OK:
                {
                    sf_tcp_nat_traversal_context_t__ context;
                    sf_deserialize(data, context, 0);
                    on_step_1_ok__(context);
                }
                break;
                case TYPE_NAT_TRAVERSAL_NEW_CONNECT_REQUIRED:
                {
                    sf_tcp_nat_traversal_context_t__ context;
                    sf_deserialize(data, context, 0);
                    on_new_connect_required__(context);
                }
                break;
                default:
                    break;
            }
        }

        void on_client_close__(){
            client_list__.clear();
        }



    public:

        static std::shared_ptr<sf_tcp_nat_traversal_client> make_client(){
            return std::make_shared<sf_tcp_nat_traversal_client>();
        }


        sf_tcp_nat_traversal_client(){
            sf_bind_signal(client__, data_coming, std::bind(&sf_tcp_nat_traversal_client::on_client_data_coming__,
                                                            this,
                                                            std::placeholders::_1,
                                                            std::placeholders::_2),false);
            sf_bind_signal(client__, closed, std::bind(&sf_tcp_nat_traversal_client::on_client_close__, this), false);
        }

        /**
         * 连接到穿透服务器，连接成功后会注册
         * @param ip 穿透服务器ip
         * @param port 穿透服务器端口
         * @return 连接结果
         */
        bool connect(const std::string& ip, unsigned short port){
            server_addr__.ip = ip;
            server_addr__.port = port;
            if(client__->connect(ip, port))
            {
                client__->send(TYPE_NAT_TRAVERSAL_REG, byte_array());
                return true;
            }
            return false;
        }

        /**
         * 获取终端集合
         * @return 终端集合
         */
        std::set<unsigned long long> get_clients(){
            return client_list__;
        }

        /**
         * 来源，连接远程peer端
         * @param peer_id 远端id
         * @return -1表示失败，其他表示此次连接的id
         */
        int connect_to_peer(unsigned long long peer_id){
            // 生成一个连接上下文
            sf_p2p_connect_context_t__ tmp_p2p_conn_context;
            // 生成随机连接id
            tmp_p2p_conn_context.tcp_nat_traversal_context.connect_id = sf_random::get_instance()->get_int(0, INT_MAX);
            // 指定对方id
            tmp_p2p_conn_context.tcp_nat_traversal_context.dest_id = peer_id;
            // 指定自身id
            tmp_p2p_conn_context.tcp_nat_traversal_context.src_id = self_id__;
            // 第0步：发起端主动连接server，context有效字段：connect_id、dest_id、src_id
            tmp_p2p_conn_context.tcp_nat_traversal_context.step = 0;
            // 生成连接a客户端
            tmp_p2p_conn_context.point_a_client_1 = sf_tcpclient::make_client();
            // 尝试连接服务器
            if(tmp_p2p_conn_context.point_a_client_1->connect(server_addr__.ip,server_addr__.port)) {
                // 发起连接请求
                if (tmp_p2p_conn_context.point_a_client_1->send(TYPE_NAT_TRAVERSAL_REQUIRE_CONNECT_PEER,
                                   sf_serialize(tmp_p2p_conn_context.tcp_nat_traversal_context))) {
                    // 保存连接上下文
                    connect_context_map__[tmp_p2p_conn_context.tcp_nat_traversal_context.connect_id] = tmp_p2p_conn_context;
                    return tmp_p2p_conn_context.tcp_nat_traversal_context.connect_id;
                }
            }
            return  -1;
        }

        void close(){
            client__->close();
        }
    };
}