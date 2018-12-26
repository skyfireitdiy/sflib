
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_tcp_nat_traversal_client.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#define SF_DEBUG
#include "sf_tcp_nat_traversal_client.hpp"
#include <iostream>
#include <sf_timer.hpp>

using namespace skyfire;

void connect(std::shared_ptr<sf_tcp_nat_traversal_client> client){
    std::string ip;
    unsigned short port;
    std::cout<<"ip:"<<std::flush;
    getline(std::cin,ip);
    std::cout<<"port:"<<std::flush;
    std::cin>>port;
    std::cin.clear();
    if(client->connect_to_server(ip,port)){
        std::cout<<"连接成功"<<std::endl;
    }else{
        std::cout<<"连接失败"<<std::endl;
    }
}

void ls(std::shared_ptr<sf_tcp_nat_traversal_client> client){
    auto clients = client->get_clients();
    for(auto &p:clients){
        std::cout<<p<<std::endl;
    }
}

void nat_conn(std::shared_ptr<sf_tcp_nat_traversal_client> client){
    unsigned long long id;
    std::cout<<"id:"<<std::flush;
    std::cin>>id;
    std::cin.clear();
    client->connect_to_peer(id, false);
}

void send(std::shared_ptr<sf_tcp_nat_traversal_connection> conn) {
    if(!conn){
        std::cout<<"connect error"<<std::endl;
        return;
    }
    std::cout<<"msg:"<<std::flush;
    std::string msg;
    getline(std::cin,msg);
    conn->send(tcp_pkg_type_user + 1,to_json(msg));
}


int main() {

    g_logger->add_level_stream(SF_DEBUG_LEVEL,&std::cout);
    // 1.创建nat穿透客户端
    auto pclient = sf_tcp_nat_traversal_client::make_client();
    std::shared_ptr<sf_tcp_nat_traversal_connection> conn;

    // 2.设新连接到来响应
    sf_bind_signal(pclient, new_nat_connection, [&](std::shared_ptr<sf_tcp_nat_traversal_connection> conn_t, int){
        std::cout<<"new connection!"<<std::endl;
        conn = conn_t;
        sf_bind_signal(conn, data_coming, [](const sf_pkg_header_t &header, const byte_array &data){
            std::string msg;
            sf_deserialize_binary(data,msg,0);
            std::cout<<"Recv:"<<msg<<std::endl;
        }, true);
    },true);

    while(true){
        std::string cmd;
        std::cout<<">"<<std::flush;
        getline(std::cin,cmd);
        if(cmd == "exit"){
            break;
        }
        // 3.连接
        if(cmd == "conn"){
            connect(pclient);
        }else if(cmd == "ls"){ // 4.枚举客户端
            ls(pclient);
        }else if(cmd == "natconn"){ // 5.连接客户端
            nat_conn(pclient);
        }else if(cmd == "id"){ // 6.获取id
            std::cout<<pclient->get_id()<<std::endl;
        }else if(cmd == "send"){ // 7.发送消息
            send(conn);
        }
    }
}