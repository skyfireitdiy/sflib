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

        // 主Server
        std::shared_ptr<sf_tcpserver> server__{sf_tcpserver::make_server()};

        // 帮助连接的Server
        std::shared_ptr<sf_tcpserver> help_connect_server__{sf_tcpserver::make_server()};

        // 当前已运行
        bool running__ = false;

        /**
         * 主Server新连接到来处理
         * @param sock 新连接的socket
         */
        void on_new_connnection(SOCKET sock){
            // TODO 做一些处理
        }

        /**
         * 主Server连接断开处理
         * @param sock 断开的socket
         */
        void on_disconnect(SOCKET sock){
            clients__.erase(sock);
        }

        /**
         * 主Server处理注册请求
         * @param sock 注册的Socket
         */
        void on_client_reg(SOCKET sock){
            clients__.insert(sock);
            on_update_client_list();
        }

        /**
         * 更新客户信息
         * @param sock
         */
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

        /**
         * 消息到来处理
         * @param sock 来源socket
         * @param header 消息头
         * @param data 数据
         */
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
            sf_bind_signal(server__, new_connection, std::bind(&sf_tcp_nat_traversal_server::on_new_connnection,
                                                               this, std::placeholders::_1), false);
            sf_bind_signal(server__, closed, std::bind(&sf_tcp_nat_traversal_server::on_disconnect,
                                                       this, std::placeholders::_1), false);
            sf_bind_signal(server__, data_coming, std::bind(&sf_tcp_nat_traversal_server::on_msg_coming,
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
            // NOTE 辅助服务器随机分配端口
            return server__->listen(ip, port) && help_connect_server__->listen(ip, 0);
        }

        /**
         * 关闭
         */
        void close(){
            server__->close();
            help_connect_server__->close();
            clients__.clear();
        }
    };
}