
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_server_win.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include <winsock2.h>
#include "sf_stdc++.h"
#include "sf_tcp_utils.hpp"
#include "sf_nocopy.h"
#include "sf_object.hpp"
#include "sf_type.hpp"
#include "sf_serialize_binary.hpp"
#include "sf_tcp_server_interface.h"


namespace skyfire {
    struct sf_per_io_operation_data_t {
        OVERLAPPED overlapped{};
        WSABUF wsa_buffer{};
        byte_array buffer;
        DWORD data_trans_count{};
        bool is_send{};
		int req_id{};
    };

    struct sf_per_handle_data_t {
        SOCKET socket;
		byte_array sock_data_buffer;
		std::shared_ptr<std::mutex> mu_sending = std::make_shared<std::mutex>();
		bool sending;
		std::shared_ptr<std::mutex> mu_out_buffer = std::make_shared<std::mutex>();
		std::deque<byte_array> data_buffer_out;
		std::shared_ptr<std::mutex> read_lock = std::make_shared<std::mutex>();
    };


    class sf_tcp_server : public sf_tcp_server_interface {
    private:
        bool inited__ = false;
        bool exit_flag__ = false;
        bool raw__ = false;

        SOCKET listen_sock__ = INVALID_SOCKET;
        HANDLE completion_port__ = INVALID_HANDLE_VALUE;
        int thread_count__ = 4;

		std::vector<std::thread> thread_vec__;

		std::atomic<long long> req_num__{ 0 };

		std::map<SOCKET, sf_per_handle_data_t> handle_data__;

		std::map<long long,sf_per_io_operation_data_t> io_data__;

		sf_per_io_operation_data_t* make_req__();

		void server_work_thread__(const LPVOID completion_port_id);

    public:

        SOCKET get_raw_socket() override;

        sf_tcp_server(bool raw = false);

		~sf_tcp_server() override;


        static std::shared_ptr<sf_tcp_server> make_server(bool raw = false);

        bool listen(const std::string &ip, unsigned short port) override;

        void close() override;

        void close(SOCKET sock) override;

        bool send(SOCKET sock, int type, const byte_array &data) override;

        bool send(SOCKET sock, const byte_array &data) override;

    };

}
