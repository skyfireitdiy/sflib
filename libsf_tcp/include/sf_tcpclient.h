#pragma once

#include <winsock2.h>
#include <string>
#include <functional>
#include <memory>
#include <thread>
#include "sf_tcputils.h"
#include "sf_object.h"
#include "sf_nocopy.h"

namespace skyfire
{
    class sf_tcpclient : public sf_nocopy<sf_object<>>, public std::enable_shared_from_this<sf_tcpclient>
    {
        SF_REG_SIGNAL(connected);
        
        
        //SF_REG_SIGNAL(data_coming, const pkg_header_t &, const byte_array &);

        public:                                                                                     
        std::mutex __mu_data_coming__;                                                                 
        std::vector<std::tuple<std::function<void( const pkg_header_t &, const byte_array &)>, bool, int>> __data_coming_func_vec__;
        template<typename...__SF_OBJECT_ARGS__>                                                     
        void data_coming(__SF_OBJECT_ARGS__... args) {
            std::lock_guard<std::mutex> lck(__mu_data_coming__);
            std::cout<<"size:"<<__data_coming_func_vec__.size()<<std::endl;
            for (auto &p : __data_coming_func_vec__)                                                   
            {
                std::cout<<"num:"<<0<<std::endl;
                if (std::get<1>(p))                                                                 
                {                                                                                   
                    std::thread([=]() {                                                             
                        std::get<0>(p)(args...);
                    }).detach();                                                                    
                }                                                                                   
                else                                                                                
                {                                                                                   
                    std::thread([=](){                                                              
                        __p_msg_queue__->add_msg(this, std::bind(std::get<0>(p),args...));          
                        __p_eventloop__->wake();                                                    
                    }).detach();                                                                    
                }                                                                                   
            }                                                                                       
        }                                                                                           
        
        
        SF_REG_SIGNAL(closed);
    private:
        bool inited__ = false;
        SOCKET sock__ = INVALID_SOCKET;
    public:
        sf_tcpclient()
        {
            WSADATA wsa_data{};
            if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
            {
                inited__ = false;
                return;
            }

            sock__ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (sock__ == INVALID_SOCKET)
            {
                inited__ = false;
                return;
            }
            inited__ = true;
        }


        static std::shared_ptr <sf_tcpclient> make_client()
        {
            return std::make_shared<sf_tcpclient>();
        }

        ~sf_tcpclient()
        {
            close();
            WSACleanup();
        }

        bool connect(const std::string &ip,
                     unsigned short port)
        {
            if (!inited__)
                return false;
            sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
            address.sin_port = htons(port);
            if (::connect(sock__, reinterpret_cast<const sockaddr *>(&address), sizeof(address)) != 0)
            {
                return false;
            }
            std::thread([=]
                        {
                            byte_array recv_buffer(BUFFER_SIZE);
                            byte_array data;
                            pkg_header_t header;
                            while (true)
                            {
                                auto len = ::recv(sock__, recv_buffer.data(), BUFFER_SIZE, 0);
                                if (len <= 0)
                                {
                                    std::thread([=]
                                                {
                                                    closed();
                                                }).detach();
                                    break;
                                }
                                data.insert(data.end(), recv_buffer.begin(), recv_buffer.begin() + len);
                                size_t read_pos = 0;
                                while (data.size() - read_pos >= sizeof(pkg_header_t))
                                {
                                    memmove_s(&header, sizeof(header), data.data() + read_pos, sizeof(header));
                                    if (data.size() - read_pos - sizeof(header) >= header.length)
                                    {
                                        std::thread([=]()
                                                    {
                                                        data_coming(header, byte_array(
                                                                data.begin() + read_pos + sizeof(header),
                                                                data.begin() + read_pos + sizeof(header) +
                                                                header.length));
                                                    }).detach();
                                        read_pos += sizeof(header) + header.length;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                                if (read_pos != 0)
                                {
                                    data.erase(data.begin(), data.begin() + read_pos);
                                }
                            }
                        }).detach();
            return true;
        }


        bool send(int type, const byte_array &data)
        {
            if (!inited__)
                return false;
            pkg_header_t header;
            header.type = type;
            header.length = data.size();
            auto ret = ::send(sock__, make_pkg(header).data(), sizeof(header), 0);
            if (ret != sizeof(header))
                return false;
            return ::send(sock__, data.data(), data.size(), 0) == data.size();
        }

        void close()
        {
            if (!inited__)
                return;
            closesocket(sock__);
            sock__ = INVALID_SOCKET;
        }
    };

}
