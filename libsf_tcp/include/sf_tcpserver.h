#pragma once

#include <winsock2.h>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <map>
#include "sf_tcputils.h"
#include "sf_nocopy.h"
#include "sf_object.h"
#include "sf_type.h"
#include "sf_range.h"
#include "sf_serialize.h"

#include <iostream>

using namespace std;

namespace skyfire
{


    static constexpr size_t __buffer_size__ = 8 * 1024;

    struct per_io_operation_data_t
    {
        OVERLAPPED overlapped{};
        WSABUF wsa_buffer{};
        byte_array buffer;
        DWORD data_trans_count{};
        bool is_send{};
    };

    struct per_handle_data_t
    {
        SOCKET socket;
    };


    class sf_tcpserver : public sf_nocopy<sf_object<>>, public std::enable_shared_from_this<sf_tcpserver>
    {
    SF_REG_SIGNAL(new_connection, SOCKET);
    SF_REG_SIGNAL(data_coming, SOCKET, const pkg_header_t&, const byte_array&);
    SF_REG_SIGNAL(closed, SOCKET);

    private:
        bool inited__ = false;

        SOCKET listen_sock__ = INVALID_SOCKET;
        HANDLE completion_port__ = INVALID_HANDLE_VALUE;
        std::map<SOCKET, byte_array> sock_data_buffer__;

    public:



        sf_tcpserver()
        {
            DWORD ret;
            WSADATA wsa_data{};

            if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
            {
                return;
            }
            completion_port__ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
            if (completion_port__ == nullptr)
            {
                return;
            }

            SYSTEM_INFO sys_info{};
            GetSystemInfo(&sys_info);

//            for (auto i : sf_range(sys_info.dwNumberOfProcessors * 2 + 1))
//            {
//                std::thread(std::bind(server_work_thread__, this, std::placeholders::_1), completion_port__).detach();
//            }

            std::thread(std::bind(server_work_thread__, this, std::placeholders::_1), completion_port__).detach();


            listen_sock__ = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
            if (listen_sock__ == INVALID_SOCKET)
            {
                return;
            }

            inited__ = true;
        }


        static std::shared_ptr<sf_tcpserver> make_server()
        {
            return std::make_shared<sf_tcpserver>();
        }

        bool listen(const std::string &ip, unsigned short port)
        {
            if (!inited__)
            {
                return false;
            }

            DWORD ret;


            sockaddr_in internet_addr{};
            internet_addr.sin_family = AF_INET;
            internet_addr.sin_addr.s_addr = inet_addr(ip.c_str());
            internet_addr.sin_port = htons(port);

            if (SOCKET_ERROR == ::bind(listen_sock__, reinterpret_cast<sockaddr*>(&internet_addr), sizeof(internet_addr)))
            {
                return false;
            }

            if (::listen(listen_sock__, SOMAXCONN) == SOCKET_ERROR)
            {
                return false;
            }

            std::thread([=]()
                        {
                            while (true)
                            {
                                SOCKET accept_socket;
                                sockaddr_in addr;
                                int addr_len = sizeof(addr);
                                accept_socket = ::accept(listen_sock__,
                                                         reinterpret_cast<sockaddr*>(&addr),
                                                         &addr_len);
                                if (accept_socket == INVALID_SOCKET)
                                {
                                    cout<<__LINE__<<endl;
                                    break;
                                }

                                std::thread([=]
                                            {
                                                new_connection(accept_socket);
                                            }).detach();

                                auto *p_handle_data = reinterpret_cast<per_handle_data_t*>(GlobalAlloc(GPTR, sizeof(per_handle_data_t)));
                                p_handle_data->socket = accept_socket;
                                if (CreateIoCompletionPort((HANDLE) accept_socket, completion_port__,
                                                           (ULONG_PTR) p_handle_data, 0) == nullptr)
                                {
                                    cout<<__LINE__<<endl;
                                    break;
                                }
                                auto *p_io_data = reinterpret_cast<per_io_operation_data_t*>(GlobalAlloc(GPTR, sizeof(per_io_operation_data_t)));;
                                ZeroMemory(&(p_io_data->overlapped), sizeof(p_io_data->overlapped));
                                p_io_data->data_trans_count = 0;
                                p_io_data->buffer.resize(__buffer_size__);
                                p_io_data->is_send = false;
                                p_io_data->wsa_buffer.buf = p_io_data->buffer.data();
                                p_io_data->wsa_buffer.len = __buffer_size__;
                                DWORD tmp_int = 0;
                                DWORD flags = 0;
                                if (WSARecv(accept_socket, &(p_io_data->wsa_buffer), 1, &tmp_int, &flags, &(p_io_data->overlapped),
                                            nullptr) ==
                                    SOCKET_ERROR)
                                {
//                                    ::GlobalFree(p_handle_data);
//                                    ::GlobalFree(p_io_data);
//                                    cout<<"error"<<WSAGetLastError()<<endl;
//                                    continue;
                                }
                            }
                        }).detach();
            return true;
        }

        void close()
        {
            if (!inited__)
                return;
            closesocket(listen_sock__);
            listen_sock__ = INVALID_SOCKET;
        }

        bool send(SOCKET sock, int type, const byte_array &data)
        {
            DWORD sendBytes;

            pkg_header_t header;
            header.type = type;
            header.length = data.size();

            auto p_io_data = reinterpret_cast<per_io_operation_data_t*>(GlobalAlloc(GPTR, sizeof(per_io_operation_data_t)));;
            //ZeroMemory(&(p_io_data->overlapped), sizeof(p_io_data->overlapped));
            p_io_data->buffer = make_pkg(header) + data;
            p_io_data->data_trans_count = 0;
            p_io_data->is_send = true;
            p_io_data->wsa_buffer.buf = p_io_data->buffer.data();
            p_io_data->wsa_buffer.len = p_io_data->buffer.size();
            if (WSASend(sock, &(p_io_data->wsa_buffer), 1, &sendBytes, 0, &(p_io_data->overlapped),
                        nullptr) == SOCKET_ERROR)
            {
//                ::GlobalFree(p_io_data);
//                return false;
            }
            return  true;
        }


        ~sf_tcpserver() override
        {
            close();
            WSACleanup();
        }


        void server_work_thread__(LPVOID completion_port_id)
        {
            auto completion_port = (HANDLE) completion_port_id;
            DWORD bytesTransferred;
            per_handle_data_t *p_handle_data = nullptr;
            per_io_operation_data_t *p_io_data = nullptr;

            DWORD tmp_int = 0;
            DWORD flags = 0;

            while (true)
            {
                if (GetQueuedCompletionStatus(completion_port, &bytesTransferred, (PULONG_PTR) &p_handle_data,
                                              (LPOVERLAPPED *) &p_io_data, WSA_INFINITE) == 0)
                {
                    break;
                }
                if(p_handle_data == nullptr || p_io_data== nullptr)
                {
                    CloseHandle((HANDLE) p_handle_data->socket);
                    std::thread([=]()
                                {
                                    closed(p_handle_data->socket);
                                }).detach();
                    ::GlobalFree(p_handle_data);
                    ::GlobalFree(p_io_data);
                    cout<<__LINE__<<endl;
                    continue;
                }
                if (bytesTransferred == 0)
                {
                    CloseHandle((HANDLE) p_handle_data->socket);
                    std::thread([=]()
                                {
                                    closed(p_handle_data->socket);
                                }).detach();
                    ::GlobalFree(p_handle_data);
                    ::GlobalFree(p_io_data);
                    cout<<__LINE__<<endl;
                    continue;
                }
                if (p_io_data->is_send)
                {
                    p_io_data->data_trans_count += bytesTransferred;

                    if (p_io_data->data_trans_count != p_io_data->buffer.size())
                    {
                        p_io_data->wsa_buffer.buf = p_io_data->buffer.data() + p_io_data->data_trans_count;
                        p_io_data->wsa_buffer.len = p_io_data->buffer.size() - p_io_data->data_trans_count;
                        tmp_int = 0;
                        if (WSASend(p_handle_data->socket, &(p_io_data->wsa_buffer), 1, &tmp_int, 0,
                                    &(p_io_data->overlapped),
                                    nullptr) == SOCKET_ERROR)
                        {
                            CloseHandle((HANDLE) p_handle_data->socket);
                            std::thread([=]()
                                        {
                                            closed(p_handle_data->socket);
                                        }).detach();
                            ::GlobalFree(p_handle_data);
                            ::GlobalFree(p_io_data);
                            cout<<__LINE__<<endl;
                            continue;
                        }
                    }
                }
                else
                {
                    p_io_data->data_trans_count = bytesTransferred;
                    p_io_data->buffer.resize(bytesTransferred);

                    pkg_header_t header{};
                    sock_data_buffer__[p_handle_data->socket].insert(sock_data_buffer__[p_handle_data->socket].end(),
                                                                     p_io_data->buffer.begin(),
                                                                     p_io_data->buffer.end());
                    size_t read_pos = 0;
                    while (sock_data_buffer__.size() - read_pos >= sizeof(pkg_header_t))
                    {
                        memmove_s(&header, sizeof(header), sock_data_buffer__[p_handle_data->socket].data() + read_pos,
                                  sizeof(header));
                        if (sock_data_buffer__.size() - read_pos - sizeof(header) >= header.length)
                        {
                            std::thread([=]()
                                        {
                                            data_coming(p_handle_data->socket, header, byte_array(
                                                    sock_data_buffer__[p_handle_data->socket].begin() + read_pos +
                                                    sizeof(header),
                                                    sock_data_buffer__[p_handle_data->socket].begin() + read_pos +
                                                    sizeof(header) + header.length));
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
                        sock_data_buffer__[p_handle_data->socket].erase(
                                sock_data_buffer__[p_handle_data->socket].begin(),
                                sock_data_buffer__[p_handle_data->socket].begin() + read_pos);
                    }

                    ZeroMemory(&(p_io_data->overlapped), sizeof(p_io_data->overlapped));
                    p_io_data->buffer.resize(__buffer_size__);
                    p_io_data->data_trans_count = 0;
                    p_io_data->is_send = false;
                    p_io_data->wsa_buffer.buf = p_io_data->buffer.data();
                    p_io_data->wsa_buffer.len = __buffer_size__;

                    flags = 0;

                    if (WSARecv(p_handle_data->socket, &(p_io_data->wsa_buffer), 1, &tmp_int, &flags,
                                &(p_io_data->overlapped), nullptr) == SOCKET_ERROR)
                    {
//                        CloseHandle((HANDLE) p_handle_data->socket);
//                        std::thread([=]()
//                                    {
//                                        closed(p_handle_data->socket);
//                                    }).detach();
//                        ::GlobalFree(p_handle_data);
//                        ::GlobalFree(p_io_data);
//                        cout<<"error"<<WSAGetLastError()<<endl;
//                        cout<<__LINE__<<endl;
                        continue;
                    }
                }
            }
        }


    };





}