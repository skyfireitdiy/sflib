#pragma once

#include <winsock2.h>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <map>
#include "sf_tcputils.hpp"
#include "sf_nocopy.hpp"
#include "sf_object.hpp"
#include "sf_type.hpp"
#include "sf_range.hpp"
#include "sf_serialize_binary.hpp"
#include "sf_tcpserver_interface.h"


namespace skyfire {
    struct per_io_operation_data_t {
        OVERLAPPED overlapped{};
        WSABUF wsa_buffer{};
        byte_array buffer;
        DWORD data_trans_count{};
        bool is_send{};
    };

    struct per_handle_data_t {
        SOCKET socket;
    };


    class sf_tcpserver : public sf_tcp_server_interface {
    private:
        bool inited__ = false;
        bool exit_flag__ = false;
        bool raw__ = false;

        SOCKET listen_sock__ = INVALID_SOCKET;
        HANDLE completion_port__ = INVALID_HANDLE_VALUE;
        int thread_count__ = 4;
        std::map<SOCKET, byte_array> sock_data_buffer__;
    public:

        SOCKET get_raw_socket() override;

        sf_tcpserver(bool raw = false);


        static std::shared_ptr<sf_tcpserver> make_server(bool raw = false);

        bool listen(const std::string &ip, unsigned short port) override;

        void close() override;

        void close(SOCKET sock) override;

        bool send(SOCKET sock, int type, const byte_array &data) override;

        bool send(SOCKET sock, const byte_array &data) override;


        ~sf_tcpserver() override;


        void server_work_thread__(LPVOID completion_port_id);


    };
}