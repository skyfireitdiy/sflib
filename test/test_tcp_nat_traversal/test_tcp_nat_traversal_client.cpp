#define SF_DEBUG
#include "sf_tcp_nat_traversal_client.h"
#include <iostream>
#include <sf_timer.h>

#include "sf_random.h"
#include "sf_range.h"

using namespace skyfire;
using namespace std;

void connect(shared_ptr<sf_tcp_nat_traversal_client> client){
    string ip;
    unsigned short port;
    cout<<"ip:"<<flush;
    getline(cin,ip);
    cout<<"port:"<<flush;
    cin>>port;
    cin.clear();
    if(client->connect(ip,port)){
        cout<<"连接成功"<<endl;
    }else{
        cout<<"连接失败"<<endl;
    }
}

void ls(shared_ptr<sf_tcp_nat_traversal_client> client){
    auto clients = client->get_clients();
    for(auto &p:clients){
        cout<<p<<endl;
    }
}

void nat_conn(shared_ptr<sf_tcp_nat_traversal_client> client){
    unsigned long long id;
    cout<<"id:"<<flush;
    cin>>id;
    cin.clear();
    client->connect_to_peer(id, false);
}

void send(std::shared_ptr<sf_tcp_nat_traversal_connection> conn) {
    if(!conn){
        cout<<"连接未建立"<<endl;
        return;
    }
    cout<<"msg:"<<flush;
    string msg;
    getline(cin,msg);
    conn->send(TCP_PKG_TYPE_USER + 1,sf_serialize(msg));
}


int main() {

    g_logger->add_level_stream(SF_DEBUG_LEVEL,&cout);
    auto pclient = sf_tcp_nat_traversal_client::make_client();
    std::shared_ptr<sf_tcp_nat_traversal_connection> conn;

    sf_bind_signal(pclient, new_nat_connection, [&](std::shared_ptr<sf_tcp_nat_traversal_connection> conn_t, int){
        cout<<"new connection!"<<endl;
        conn = conn_t;
        sf_bind_signal(conn, data_coming, [](const pkg_header_t &header, const byte_array &data){
            string msg;
            sf_deserialize(data,msg,0);
            cout<<"Recv:"<<msg<<endl;
        }, true);
    },true);

    while(true){
        string cmd;
        cout<<">"<<flush;
        getline(cin,cmd);
        if(cmd == "exit"){
            break;
        }
        if(cmd == "conn"){
            connect(pclient);
        }else if(cmd == "ls"){
            ls(pclient);
        }else if(cmd == "natconn"){
            nat_conn(pclient);
        }else if(cmd == "id"){
            cout<<pclient->get_id()<<endl;
        }else if(cmd == "send"){
            send(conn);
        }
    }
}