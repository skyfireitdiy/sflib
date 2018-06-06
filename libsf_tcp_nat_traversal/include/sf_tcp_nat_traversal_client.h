#pragma once

#include "sf_nocopy.h"
#include "sf_object.h"
#include "sf_tcpclient.h"
#include "sf_serialize.h"
#include "sf_define.h"
#include <string>

using namespace std;

namespace skyfire {
    class sf_tcp_nat_traversal_client : public sf_nocopy<sf_object> {
    private:
        std::shared_ptr<sf_tcpclient> client__ {sf_tcpclient::make_client()};
    public:
        static std::shared_ptr<sf_tcp_nat_traversal_client> make_client(){
            return std::make_shared<sf_tcp_nat_traversal_client>();
        }

        bool connect(const std::string& ip, unsigned short port){
            if(client__->connect(ip, port))
            {
                client__->send(TYPE_NAT_TRAVERSAL_REG, sf_serialize("hello"s));
                return true;
            }else{
                return false;
            }
        }

        void close(){
            client__->close();
        }
    };
}