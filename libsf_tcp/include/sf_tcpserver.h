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

#include <iostream>

namespace skyfire
{
    class sf_tcpserver2 : public sf_nocopy<sf_object<>>, public std::enable_shared_from_this<sf_tcpserver>
    {
        SF_REG_SIGNAL(new_connection,SOCKET);
        SF_REG_SIGNAL(data_coming,SOCKET,const pkg_header_t&,const byte_array&);
        SF_REG_SIGNAL(closed,SOCKET);
    private:
        bool inited__ = false;
        SOCKET server_sock__ = INVALID_SOCKET;

    public:
        sf_tcpserver()
        {
            WSADATA wsa_data{};
            if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
            {
                inited__ = false;
                return;
            }

            server_sock__ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (server_sock__ == INVALID_SOCKET)
            {
                inited__ = false;
                return;
            }
            inited__ = true;
        }

        static std::shared_ptr <sf_tcpserver> make_server()
        {
            return std::make_shared<sf_tcpserver>();
        }


        ~sf_tcpserver()
        {
            close();
            WSACleanup();
        }

        bool listen(const std::string &ip,
                    unsigned short port
        )
        {
            if (!inited__)
                return false;
            sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
            address.sin_port = htons(port);
            if (::bind(server_sock__, reinterpret_cast<const sockaddr *>(&address), sizeof(address)) == SOCKET_ERROR)
            {
                return false;
            }
            if (::listen(server_sock__, SOMAXCONN) != 0)
            {
                return false;
            }

            std::thread([=]()
                        {
                            while (true)
                            {
                                sockaddr_in address;
                                int address_len = sizeof(sockaddr_in);
                                auto sock = ::accept(server_sock__, reinterpret_cast<sockaddr *>(&address),
                                                     &address_len);
                                if (sock == INVALID_SOCKET)
                                {
                                    break;
                                }
                                std::thread([=]
                                            {
                                                new_connection(sock);
                                            }).detach();
                                std::thread([=]
                                            {
                                                byte_array recv_buffer(BUFFER_SIZE);
                                                byte_array data;
                                                pkg_header_t header;
                                                while (true)
                                                {
                                                    auto len = ::recv(sock, recv_buffer.data(), BUFFER_SIZE, 0);
                                                    if (len <= 0)
                                                    {
                                                        std::thread([=]
                                                                    {
                                                                        closed(sock);
                                                                    }).detach();
                                                        break;
                                                    }
                                                    data.insert(data.end(), recv_buffer.begin(),
                                                                recv_buffer.begin() + len);
                                                    size_t read_pos = 0;
                                                    while (data.size() - read_pos >= sizeof(pkg_header_t))
                                                    {
                                                        memmove_s(&header, sizeof(header), data.data() + read_pos,
                                                                  sizeof(header));
                                                        if (data.size() - read_pos - sizeof(header) >= header.length)
                                                        {
                                                            std::thread([=]()
                                                                        {
                                                                            data_coming(sock, header, byte_array(
                                                                                    data.begin() + read_pos +
                                                                                    sizeof(header),
                                                                                    data.begin() + read_pos +
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
                                                        data.erase(data.begin(), data.begin() + read_pos);
                                                    }
                                                }
                                            }).detach();
                            }
                        }).detach();
            return true;
        }

        void close()
        {
            if (!inited__)
                return;
            closesocket(server_sock__);
            server_sock__ = INVALID_SOCKET;
        }

        bool send(SOCKET sock, int type, const byte_array &data)
        {
            if (!inited__)
                return false;
            pkg_header_t header;
            header.type = type;
            header.length = data.size();
            auto ret = ::send(sock, make_pkg(header).data(), sizeof(header), 0);
            if (ret != sizeof(header))
                return false;
            return ::send(sock, data.data(), data.size(), 0) == data.size();
        }
    };





    class sf_tcpserver : public sf_nocopy<sf_object<>>, public std::enable_shared_from_this<sf_tcpserver>
    {


    SF_REG_SIGNAL(new_connection, SOCKET);
    SF_REG_SIGNAL(data_coming, SOCKET, const pkg_header_t&, const byte_array&);
    SF_REG_SIGNAL(closed, SOCKET);

    private:
        bool inited__ = false;


        SOCKET listenSocket = INVALID_SOCKET;
        HANDLE completionPort = INVALID_HANDLE_VALUE;

    public:

        constexpr size_t DataBuffSize = 8 * 1024;

        typedef struct
        {
            OVERLAPPED overlapped;
            WSABUF databuff;
            CHAR buffer[DataBuffSize];
            DWORD bytesSend;
            DWORD bytesRecv;
        } PER_IO_OPERATEION_DATA, *LPPER_IO_OPERATION_DATA;

        typedef struct
        {
            SOCKET socket;
        } PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

        sf_tcpserver()
        {
            DWORD ret;
            WSADATA wsaData;
            if (ret = WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
            {
                std::cout << "WSAStartup failed. Error:" << ret << std::endl;
                return;
            }

            completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
            if (completionPort == NULL)
            {
                std::cout << "CreateIoCompletionPort failed. Error:" << GetLastError() << std::endl;
                return;
            }

            SYSTEM_INFO mySysInfo;
            GetSystemInfo(&mySysInfo);

            DWORD threadID;
            for (DWORD i = 0; i < (mySysInfo.dwNumberOfProcessors * 2 + 1); ++i)
            {
                std::thread(std::bind(ServerWorkThread, this, std::placeholders::_1), completionPort).detach();
            }


            SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
            if (listenSocket == INVALID_SOCKET)
            {
                std::cout << " WSASocket( listenSocket ) failed. Error:" << GetLastError() << std::endl;
                return;
            }

            inited__ = true;
        }

        bool listen(const std::string &ip, unsigned short DefaultPort)
        {
            if(!inited__)
            {
                return false;
            }

            LPPER_HANDLE_DATA pHandleData;
            LPPER_IO_OPERATION_DATA pIoData;
            DWORD recvBytes;
            DWORD flags;
            DWORD ret;


            SOCKADDR_IN internetAddr;
            internetAddr.sin_family = AF_INET;
            internetAddr.sin_addr.s_addr = inet_addr(ip.c_str());
            internetAddr.sin_port = htons(DefaultPort);

            if (bind(listenSocket, (PSOCKADDR) &internetAddr, sizeof(internetAddr)) == SOCKET_ERROR)
            {
                std::cout << "Bind failed. Error:" << GetLastError() << std::endl;
                return false;
            }

            if (::listen(listenSocket, 5) == SOCKET_ERROR)
            {
                std::cout << "listen failed. Error:" << GetLastError() << std::endl;
                return 0;
            }

            // 开始死循环，处理数据
            std::thread([=]()
                        {
                            while (1)
                            {
                                SOCKET acceptSocket = WSAAccept(listenSocket, NULL, NULL, NULL, 0);
                                if (acceptSocket == SOCKET_ERROR)
                                {
                                    std::cout << "WSAAccept failed. Error:" << GetLastError() << std::endl;
                                    return 0;
                                }

                                std::thread([=]
                                            {
                                                new_connection(acceptSocket);
                                            }).detach();

                                pHandleData = (LPPER_HANDLE_DATA) GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));
                                if (pHandleData == NULL)
                                {
                                    std::cout << "GlobalAlloc( HandleData ) failed. Error:" << GetLastError()
                                              << std::endl;
                                    return 0;
                                }

                                pHandleData->socket = acceptSocket;
                                if (CreateIoCompletionPort((HANDLE) acceptSocket, completionPort,
                                                           (ULONG_PTR) pHandleData, 0) == NULL)
                                {
                                    std::cout << "CreateIoCompletionPort failed. Error:" << GetLastError() << std::endl;
                                    return 0;
                                }

                                pIoData = (LPPER_IO_OPERATION_DATA) GlobalAlloc(GPTR, sizeof(PER_IO_OPERATEION_DATA));
                                if (pIoData == NULL)
                                {
                                    std::cout << "GlobalAlloc( IoData ) failed. Error:" << GetLastError() << std::endl;
                                    return 0;
                                }

                                ZeroMemory(&(pIoData->overlapped), sizeof(pIoData->overlapped));
                                pIoData->bytesSend = 0;
                                pIoData->bytesRecv = 0;
                                pIoData->databuff.len = DataBuffSize;
                                pIoData->databuff.buf = pIoData->buffer;

                                flags = 0;
                                if (WSARecv(acceptSocket, &(pIoData->databuff), 1, &recvBytes, &flags,
                                            &(pIoData->overlapped), NULL) ==
                                    SOCKET_ERROR)
                                {
                                    if (WSAGetLastError() != ERROR_IO_PENDING)
                                    {
                                        std::cout << "WSARecv() failed. Error:" << GetLastError() << std::endl;
                                        return 0;
                                    } else
                                    {
                                        std::cout << "WSARecv() io pending" << std::endl;
                                        return 0;
                                    }
                                }
                            }
                        }).detach();
            return true;
        }

        void ServerWorkThread(LPVOID CompletionPortID)
        {
            HANDLE complationPort = (HANDLE) CompletionPortID;
            DWORD bytesTransferred;
            LPPER_HANDLE_DATA pHandleData = NULL;
            LPPER_IO_OPERATION_DATA pIoData = NULL;
            DWORD sendBytes = 0;
            DWORD recvBytes = 0;
            DWORD flags;
            byte_array data;

            while (1)
            {
                if (GetQueuedCompletionStatus(complationPort, &bytesTransferred, (PULONG_PTR) &pHandleData,
                                              (LPOVERLAPPED *) &pIoData, INFINITE) == 0)
                {
                    std::cout << "GetQueuedCompletionStatus failed. Error:" << GetLastError() << std::endl;
                    return;
                }
                // 检查数据是否已经传输完了
                if (bytesTransferred == 0)
                {
                    std::cout << " Start closing socket..." << std::endl;
                    if (CloseHandle((HANDLE) pHandleData->socket) == SOCKET_ERROR)
                    {
                        std::cout << "Close socket failed. Error:" << GetLastError() << std::endl;
                        return;
                    }

                    GlobalFree(pHandleData);
                    GlobalFree(pIoData);
                    continue;
                }

                // 检查管道里是否有数据
                if (pIoData->bytesRecv == 0)
                {
                    pIoData->bytesRecv = bytesTransferred;
                    pIoData->bytesSend = 0;
                } else
                {
                    pIoData->bytesSend += bytesTransferred;
                }

                // 数据没有发完，继续发送
                if (pIoData->bytesRecv > pIoData->bytesSend)
                {
                    ZeroMemory(&(pIoData->overlapped), sizeof(OVERLAPPED));
                    pIoData->databuff.buf = pIoData->buffer + pIoData->bytesSend;
                    pIoData->databuff.len = pIoData->bytesRecv - pIoData->bytesSend;

                    // 发送数据出去
                    if (WSASend(pHandleData->socket, &(pIoData->databuff), 1, &sendBytes, 0, &(pIoData->overlapped),
                                NULL) == SOCKET_ERROR)
                    {
                        if (WSAGetLastError() != ERROR_IO_PENDING)
                        {
                            std::cout << "WSASend() failed. Error:" << GetLastError() << std::endl;
                            return;
                        } else
                        {
                            std::cout << "WSASend() failed. io pending. Error:" << GetLastError() << std::endl;
                            return;
                        }
                    }

                    std::cout << "Send " << pIoData->buffer << std::endl;
                }
                else
                {
                    pkg_header_t header;
                    data.insert(data.end(), pIoData->databuff.buf,
                                pIoData->databuff.buf + pIoData->databuff.length);
                    size_t read_pos = 0;
                    while (data.size() - read_pos >= sizeof(pkg_header_t))
                    {
                        memmove_s(&header, sizeof(header), data.data() + read_pos,
                                  sizeof(header));
                        if (data.size() - read_pos - sizeof(header) >= header.length)
                        {
                            std::thread([=]()
                                        {
                                            data_coming(pIoData->socket, header, byte_array(
                                                    data.begin() + read_pos +
                                                    sizeof(header),
                                                    data.begin() + read_pos +
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
                        data.erase(data.begin(), data.begin() + read_pos);
                    }

                    pIoData->bytesRecv = 0;
                    flags = 0;

                    ZeroMemory(&(pIoData->overlapped), sizeof(OVERLAPPED));
                    pIoData->databuff.len = DataBuffSize;
                    pIoData->databuff.buf = pIoData->buffer;

                    if (WSARecv(pHandleData->socket, &(pIoData->databuff), 1, &recvBytes, &flags,
                                &(pIoData->overlapped), NULL) == SOCKET_ERROR)
                    {
                        if (WSAGetLastError() != ERROR_IO_PENDING)
                        {
                            std::cout << "WSARecv() failed. Error:" << GetLastError() << std::endl;
                            return;
                        } else
                        {
                            std::cout << "WSARecv() io pending" << std::endl;
                            return;
                        }
                    }
                }
            }
        }


    };




}