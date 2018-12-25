#pragma once

#include "sf_server_socket_filter.h"
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace skyfire
{
	struct sf_pkg_header_t;

	class sf_ssl_server_socket_filter final : public sf_server_socket_filter
    {
    private:
        bool check_ok__ { false };

        SSL_CTX * ctx__ {nullptr};

        BIO * error_bio__{nullptr};
    public:
        sf_ssl_server_socket_filter(const std::string &cert, const std::string& key);

        void send_filter(SOCKET sock,sf_pkg_header_t& header, byte_array& data) override;
        void recv_filter(SOCKET sock,sf_pkg_header_t& header, byte_array& data) override;
        void raw_send_filter(SOCKET sock,byte_array& data) override;
        void raw_recv_filter(SOCKET sock,byte_array& data) override;
        void new_connection_filter(SOCKET sock) override;
        void listen_sock_filter(SOCKET sock) override;
        ~sf_ssl_server_socket_filter();
    };
}