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
        // 保存客户端列表
        std::set<SOCKET> clients__;

        // Server
        std::shared_ptr<sf_tcpserver> server__{sf_tcpserver::make_server()};


        // 当前已运行
        bool running__ = false;

        /**
         * 主Server新连接到来处理
         * @param sock 新连接的socket
         */
        void on_new_connnection__(SOCKET sock){
            // TODO 做一些处理
        }

        /**
         * 主Server连接断开处理
         * @param sock 断开的socket
         */
        void on_disconnect__(SOCKET sock){
            clients__.erase(sock);
        }

        /**
         * 主Server处理注册请求
         * @param sock 注册的Socket
         */
        void on_client_reg__(SOCKET sock){
            clients__.insert(sock);
            unsigned long long id = sock;
            server__->send(sock, TYPE_NAT_TRAVERSAL_SET_ID, sf_serialize(id));
            on_update_client_list__();
        }

        /**
         * 更新客户信息
         * @param sock
         */
        void on_update_client_list__(SOCKET sock = static_cast<SOCKET>(-1)) {
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

        /**
         * 消息到来处理
         * @param sock 来源socket
         * @param header 消息头
         * @param data 数据
         */
        void on_msg_coming__(SOCKET sock, const pkg_header_t &header, const byte_array &data){
            switch (header.type){
                case TYPE_NAT_TRAVERSAL_GET_LIST:
                    on_update_client_list__(sock);
                    break;
                case TYPE_NAT_TRAVERSAL_REG:
                    on_client_reg__(sock);
                    break;
                case TYPE_NAT_TRAVERSAL_REQUIRE_CONNECT_PEER: {
                    sf_tcp_nat_traversal_context_t__ context;
                    sf_deserialize(data, context, 0);
                }
                    break;
                default:
                    break;
            }
        }

        /**
         * 处理客户端远程连接请求
         * @param context
         */
        void on_client_require_connect_to_peer_client__(sf_tcp_nat_traversal_context_t__ &context)
        {
            if(clients__.count(context.src_id) == 0)
            {
                return;
            }
            if(clients__.count(context.dest_id) == 0)
            {
                context.error_code = SF_ERR_NOT_EXIST;
                server__->send(context.src_id, TYPE_NAT_TRAVERSAL_ERROR, sf_serialize(context));
                return;
            }
            if(!get_peer_addr(context.src_id,context.src_addr))
            {
                context.error_code = SF_ERR_DISCONNECT;
                server__->send(context.src_id, TYPE_NAT_TRAVERSAL_ERROR, sf_serialize(context));
                return;
            }
            if(!server__->send(context.dest_id,TYPE_NAT_TRAVERSAL_NEW_CONNECT_REQUIRED, sf_serialize(context)))
            {
                context.error_code = SF_ERR_REMOTE_ERR;
                server__->send(context.src_id, TYPE_NAT_TRAVERSAL_ERROR, sf_serialize(context));
                return;
            }
            server__->send(context.src_id, TYPE_NAT_TRAVERSAL_STEP_1_OK, sf_serialize(context));
        }

    public:
        /**
         * 创建NAT穿透服务器
         * @return 服务器smart指针
         */
        static std::shared_ptr<sf_tcp_nat_traversal_server> make_server(){
            return std::make_shared<sf_tcp_nat_traversal_server>();
        }

        /**
         * 构造函数
         */
        sf_tcp_nat_traversal_server(){
            sf_bind_signal(server__, new_connection, std::bind(&sf_tcp_nat_traversal_server::on_new_connnection__,
                                                               this, std::placeholders::_1), false);
            sf_bind_signal(server__, closed, std::bind(&sf_tcp_nat_traversal_server::on_disconnect__,
                                                       this, std::placeholders::_1), false);
            sf_bind_signal(server__, data_coming, std::bind(&sf_tcp_nat_traversal_server::on_msg_coming__,
                                                       this, std::placeholders::_1,
                                                            std::placeholders::_2,
                                                            std::placeholders::_3), false);
        }

        /**
         * 监听端口
         * @param ip 本机IP
         * @param port 本机端口
         * @return 监听结果
         */
        bool listen(const std::string &ip, unsigned short port){
            if(running__)
                return false;
            running__ = true;
        }

        /**
         * 关闭
         */
        void close(){
            server__->close();
            clients__.clear();
        }
    };
}