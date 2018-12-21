
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_server_win.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include "sf_tcp_server_win.h"
#include "sf_tcp_server_interface.hpp"
#include "sf_net_utils.hpp"
#include "sf_logger.hpp"

namespace skyfire {

    inline SOCKET sf_tcp_server::get_raw_socket() {
        return listen_sock__;
    }

    inline void sf_tcp_server::server_work_thread__(const LPVOID completion_port_id) {
	    const auto completion_port = static_cast<HANDLE>(completion_port_id);
        DWORD bytesTransferred;
        sf_per_handle_data_t *p_handle_data = nullptr;
        sf_per_io_operation_data_t *p_io_data = nullptr;

        DWORD tmp_int = 0;
        DWORD flags = 0;

        while (true) {
	        const auto result = GetQueuedCompletionStatus(completion_port, &bytesTransferred, reinterpret_cast<PULONG_PTR>(&p_handle_data),
                                                   reinterpret_cast<LPOVERLAPPED *>(&p_io_data), WSA_INFINITE);
			// 监听socket出错响应
			auto handle_listen_sock_error = [&]
			{
				disconnect_sock_filter__(p_handle_data->socket);
				close(p_handle_data->socket);
				closed(p_handle_data->socket);
				handle_data__.erase(p_handle_data->socket);
				io_data__.erase(p_io_data->req_id);
			};

			// 其他socket出错响应
			auto handle_client_sock_error = [&]
			{
				if (p_handle_data)
				{
					disconnect_sock_filter__(p_handle_data->socket);
					close(p_handle_data->socket);
					closed(p_handle_data->socket);
					handle_data__.erase(p_handle_data->socket);
				}
				if (p_io_data)
				{
					io_data__.erase(p_io_data->req_id);
				}
			};

			// 退出标记
            if (exit_flag__) {
                if (p_handle_data != nullptr) {
					handle_listen_sock_error();
                }
                break;
            }

			// result为0，请求出错
            if (result == 0) {
                if (p_handle_data != nullptr) {
					handle_listen_sock_error();
                }
                continue;
            }

			// 出错
            if (p_handle_data == nullptr || p_io_data == nullptr) {
				handle_client_sock_error();
                continue;
            }
			// 关闭
            if (bytesTransferred == 0) {
				handle_client_sock_error();
                continue;
            }

			// 发送
            if (p_io_data->is_send) {
                p_io_data->data_trans_count += bytesTransferred;

				// 没发完
                if (p_io_data->data_trans_count != p_io_data->buffer.size()) {
                    ZeroMemory(&p_io_data->overlapped, sizeof(p_io_data->overlapped));
                    p_io_data->wsa_buffer.buf = p_io_data->buffer.data() + p_io_data->data_trans_count;
                    p_io_data->wsa_buffer.len = p_io_data->buffer.size() - p_io_data->data_trans_count;
                    tmp_int = 0;
                    if (WSASend(p_handle_data->socket, &(p_io_data->wsa_buffer), 1, &tmp_int, 0,
                                &(p_io_data->overlapped),
                                nullptr) == SOCKET_ERROR) {
                        if (WSAGetLastError() != ERROR_IO_PENDING) {
							write_error(p_handle_data->socket);
							handle_client_sock_error();
							sf_debug("write error");
                        }
                    }
                }
                else
                {
                    // 释放资源，否则会有内存泄露
					io_data__.erase(p_io_data->req_id);
					write_finished(p_handle_data->socket);

					std::unique_lock<std::mutex> lck(*p_handle_data->mu_out_buffer);
					if(!p_handle_data->data_buffer_out.empty())
                    {
					    p_handle_data->data_buffer_out.pop_front();
                    }
					if(!p_handle_data->data_buffer_out.empty())
                    {
					    DWORD sendBytes;
                        ZeroMemory(&(p_io_data->overlapped), sizeof(p_io_data->overlapped));
                        p_io_data->buffer = p_handle_data->data_buffer_out.front();
                        p_io_data->data_trans_count = 0;
                        p_io_data->is_send = true;
                        p_io_data->wsa_buffer.buf = p_io_data->buffer.data();
                        p_io_data->wsa_buffer.len = p_io_data->buffer.size();
                        if (WSASend(p_handle_data->socket, &(p_io_data->wsa_buffer), 1, &sendBytes, 0, &(p_io_data->overlapped),
                                    nullptr) == SOCKET_ERROR)
                        {
                            if (WSAGetLastError() != ERROR_IO_PENDING)
                            {
                                sf_debug("write error");
                                disconnect_sock_filter__(p_handle_data->socket);
                                close(p_handle_data->socket);
                                closed(p_handle_data->socket);
                                io_data__.erase(p_io_data->req_id);
                                handle_data__.erase(p_handle_data->socket);
                            }
                        }
                    } else {
					    std::unique_lock<std::mutex> lck(p_handle_data->mu_sending);
					    p_handle_data->sending = false;
					}
                }
            } else {
                std::unique_lock<std::mutex> lck(*p_handle_data->read_lock);
				// 接收
                p_io_data->data_trans_count = bytesTransferred;
                p_io_data->buffer.resize(bytesTransferred);

                if (raw__) {
                    sf_debug("recv raw",p_io_data->buffer.size());
                    auto tmp_data = p_io_data->buffer;
                    after_raw_recv_filter__(p_handle_data->socket,tmp_data);
                    raw_data_coming(p_handle_data->socket, tmp_data);
                } else {
                    sf_pkg_header_t header{};
                    p_handle_data->sock_data_buffer.insert(
                            p_handle_data->sock_data_buffer.end(),
                            p_io_data->buffer.begin(),
                            p_io_data->buffer.end());
                    size_t read_pos = 0;
                    auto need_close = false;
					// 循环解析
                    while (p_handle_data->sock_data_buffer.size() - read_pos >= sizeof(sf_pkg_header_t)) {
                        memmove_s(&header, sizeof(header),
							p_handle_data->sock_data_buffer.data() + read_pos,
                                  sizeof(header));
                        if (!check_header_checksum(header)) {
							handle_client_sock_error();
							need_close = true;
                            break;
                        }
                        if (p_handle_data->sock_data_buffer.size() - read_pos - sizeof(header) >=
                            header.length) {
                            auto data = byte_array(
								p_handle_data->sock_data_buffer.begin() + read_pos +
                                    sizeof(header),
								p_handle_data->sock_data_buffer.begin() + read_pos +
                                    sizeof(header) + header.length);
                            after_recv_filter__(p_handle_data->socket, header, data);
                            data_coming(
                                    p_handle_data->socket,
                                    header,
                                    data
                            );
                            read_pos += sizeof(header) + header.length;
                        } else {
                            break;
                        }
                    }
					if(need_close)
					{
						continue;
					}

                    if (read_pos != 0) {
						p_handle_data->sock_data_buffer.erase(
							p_handle_data->sock_data_buffer.begin(),
							p_handle_data->sock_data_buffer.begin() + read_pos);
                    }
                }

                ZeroMemory(&(p_io_data->overlapped), sizeof(p_io_data->overlapped));
                p_io_data->buffer.resize(SF_DEFAULT_BUFFER_SIZE);
                p_io_data->data_trans_count = 0;
                p_io_data->is_send = false;
                p_io_data->wsa_buffer.buf = p_io_data->buffer.data();
                p_io_data->wsa_buffer.len = SF_DEFAULT_BUFFER_SIZE;

                flags = 0;

				// 重新发一个读取请求
                if (WSARecv(p_handle_data->socket, &(p_io_data->wsa_buffer), 1, &tmp_int, &flags,
                            &(p_io_data->overlapped), nullptr) == SOCKET_ERROR) {
                    if (WSAGetLastError() != ERROR_IO_PENDING) {
						handle_client_sock_error();
                    }
                }
            }
        }
    }

    inline sf_tcp_server::sf_tcp_server(bool raw) {
        // 初始化WinSock，线程数量等
	    WSADATA wsa_data{};

        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
            return;
        }

        SYSTEM_INFO sys_info{};
        GetSystemInfo(&sys_info);
        thread_count__ = static_cast<int>(sys_info.dwNumberOfProcessors * 2 + 2);
        inited__ = true;
        raw__ = raw;
    }

    inline bool sf_tcp_server::listen(const std::string &ip, unsigned short port) {
        if (!inited__) {
            return false;
        }

        exit_flag__ = false;;

		// 创建完成端口
        completion_port__ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
        if (completion_port__ == nullptr) {
            close();
            return false;
        }

		// 启动工作线程
		for (auto i = 0; i < thread_count__;++i) {
            std::thread([=] { server_work_thread__(completion_port__); }).detach();
        }

		// 创建监听套接字
        listen_sock__ = WSASocketW(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
        if (listen_sock__ == INVALID_SOCKET) {
            close();
            return false;
        }

        listen_sock_filter__(listen_sock__);

		// 设置地址复用
        int op = 1;
        if(SOCKET_ERROR == setsockopt(listen_sock__,
                                      SOL_SOCKET,
                                      SO_REUSEADDR,
                                      reinterpret_cast<char*>(&op),
                                      sizeof(op))){
            return false;
        }

		// 创建地址信息
        sockaddr_in internet_addr{};
        internet_addr.sin_family = AF_INET;
		sf_safe_inet_addr(AF_INET, ip.c_str(), internet_addr.sin_addr.s_addr);
        internet_addr.sin_port = htons(port);

		// 绑定端口
        if (SOCKET_ERROR ==
            ::bind(listen_sock__, reinterpret_cast<sockaddr *>(&internet_addr), sizeof(internet_addr))) {
            close();
            return false;
        }

		// 监听
        if (::listen(listen_sock__, max_tcp_connection) == SOCKET_ERROR) {
            close();
            return false;
        }

		// 启动线程accept
        std::thread([=]() {
            while (true) {
                SOCKET accept_socket;
                sockaddr_in addr;
                int addr_len = sizeof(addr);
                accept_socket = WSAAccept(listen_sock__, nullptr, nullptr, nullptr, 0);
                if (accept_socket == INVALID_SOCKET) {
                    break;
                }

				// 成功等待到新连接
                new_connection_filter__(accept_socket);
                new_connection(accept_socket);

				handle_data__[accept_socket] = sf_per_handle_data_t();

				auto *p_handle_data = &handle_data__[accept_socket];

                p_handle_data->socket = accept_socket;
                if (CreateIoCompletionPort((HANDLE) accept_socket, completion_port__,
                                           (ULONG_PTR) p_handle_data, 0) == nullptr) {
                    break;
                }

				// 生成新的请求数据

				auto p_io_data = make_req__();

				// 填充请求数据
            	ZeroMemory(&(p_io_data->overlapped), sizeof(p_io_data->overlapped));
                p_io_data->data_trans_count = 0;
                p_io_data->buffer.resize(SF_DEFAULT_BUFFER_SIZE);
                p_io_data->is_send = false;
                p_io_data->wsa_buffer.buf = p_io_data->buffer.data();
                p_io_data->wsa_buffer.len = SF_DEFAULT_BUFFER_SIZE;
                DWORD tmp_int = 0;
                DWORD flags = 0;

				// 投递一个接收请求
                if (WSARecv(accept_socket, &(p_io_data->wsa_buffer), 1, &tmp_int, &flags, &(p_io_data->overlapped),
                            nullptr) ==
                    SOCKET_ERROR) {
					if (WSAGetLastError() != ERROR_IO_PENDING) {
						if (p_handle_data)
						{
							disconnect_sock_filter__(p_handle_data->socket);
							close(p_handle_data->socket);
							closed(p_handle_data->socket);
							handle_data__.erase(p_handle_data->socket);
						}
						if (p_io_data)
						{
							io_data__.erase(p_io_data->req_id);
						}
					}
                }
            }
        }).detach();
        return true;
    }

	inline sf_per_io_operation_data_t* sf_tcp_server::make_req__()
	{
		const auto req_id = req_num__++;
		io_data__[req_id] = sf_per_io_operation_data_t{};
		io_data__[req_id].req_id = req_id;
		return &io_data__[req_id];
	}

    inline std::shared_ptr<sf_tcp_server> sf_tcp_server::make_server(bool raw) {
        return std::make_shared<sf_tcp_server>(raw);
    }

    inline void sf_tcp_server::close() {
        exit_flag__ = true;
		if (completion_port__ != nullptr) {
			for (auto i = 0; i < thread_count__; ++i) {
				PostQueuedCompletionStatus(completion_port__, 0, 0, nullptr);
			}
			CloseHandle(completion_port__);
			completion_port__ = INVALID_HANDLE_VALUE;
		}
        if (listen_sock__ != INVALID_SOCKET) {
            shutdown(listen_sock__,SD_BOTH);
            closesocket(listen_sock__);
            listen_sock__ = INVALID_SOCKET;
        }
		for(auto &p:handle_data__)
		{
			close(p.first);
		}
		handle_data__.clear();
		io_data__.clear();
    }

    inline void sf_tcp_server::close(SOCKET sock) {
        shutdown(sock,SD_BOTH);
        ::closesocket(sock);
    }

    inline bool sf_tcp_server::send(SOCKET sock, int type, const byte_array &data) {

        if(handle_data__.count(sock) == 0)
            return false;

        DWORD sendBytes;
		sf_pkg_header_t header{};
        header.type = type;
        header.length = data.size();
        make_header_checksum(header);
        auto tmp_data = data;
        before_send_filter__(sock,header,tmp_data);
        auto send_data = make_pkg(header) + tmp_data;

        {
            std::unique_lock<std::mutex> lck(*handle_data__[sock].mu_out_buffer);
            handle_data__[sock].data_buffer_out.push_back(send_data);
        }

        if(!handle_data__[sock].sending)
        {
            std::unique_lock<std::mutex> lck(*handle_data__[sock].mu_sending)
            {
                if(!handle_data__[sock].sending)
                {
                    handle_data__[sock].sending = true;
                    auto p_io_data = make_req__();
                    ZeroMemory(&(p_io_data->overlapped), sizeof(p_io_data->overlapped));
                    p_io_data->buffer = send_data;
                    p_io_data->data_trans_count = 0;
                    p_io_data->is_send = true;
                    p_io_data->wsa_buffer.buf = p_io_data->buffer.data();
                    p_io_data->wsa_buffer.len = p_io_data->buffer.size();
                    if (WSASend(sock, &(p_io_data->wsa_buffer), 1, &sendBytes, 0, &(p_io_data->overlapped),
                                nullptr) == SOCKET_ERROR)
                    {
                        if (WSAGetLastError() != ERROR_IO_PENDING)
                        {
                            io_data__.erase(p_io_data->req_id);
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }

    inline bool sf_tcp_server::send(SOCKET sock, const byte_array &data) {
        if(handle_data__.count(sock) == 0)
            return false;

        DWORD sendBytes;

        auto tmp_data = data;
        before_raw_send_filter__(sock,tmp_data);

		auto p_io_data = make_req__();
        ZeroMemory(&(p_io_data->overlapped), sizeof(p_io_data->overlapped));
        p_io_data->buffer = tmp_data;
        p_io_data->data_trans_count = 0;
        p_io_data->is_send = true;
        p_io_data->wsa_buffer.buf = p_io_data->buffer.data();
        p_io_data->wsa_buffer.len = p_io_data->buffer.size();
        if (WSASend(sock, &(p_io_data->wsa_buffer), 1, &sendBytes, 0, &(p_io_data->overlapped),
                    nullptr) == SOCKET_ERROR) {
            if (WSAGetLastError() != ERROR_IO_PENDING) {
				io_data__.erase(p_io_data->req_id);
                return false;
            }
        }
        return true;
    }

    inline sf_tcp_server::~sf_tcp_server() {
	    sf_tcp_server::close();
        WSACleanup();
    }


}