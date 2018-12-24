#pragma once

#include "sf_server_ssl_socket_filter.h"
#include "sf_logger.hpp"

namespace skyfire
{
    sf_ssl_server_socket_filter::sf_ssl_server_socket_filter(const std::string &cert, const std::string &key) {
        SSL_load_error_strings();
        if(SSL_library_init() != 0)
        {
            sf_debug("ssl init error");
            return ;
        }
        error_bio__ = BIO_new_fd(2, BIO_NOCLOSE);
        ctx__ = SSL_CTX_new(SSLv23_method());
        if(!ctx__)
        {
            sf_debug("create ssl context error");
            return;
        }
        if(SSL_CTX_use_certificate_file(ctx__,cert.c_str(),SSL_FILETYPE_PEM) != 0)
        {
            sf_debug("use certificate file cert error", cert);
            return ;
        }
        if(SSL_CTX_use_PrivateKey_file(ctx__,key.c_str(),SSL_FILETYPE_PEM)!=0)
        {
            sf_debug("use certificate file key error", key);
            return ;
        }
        if(SSL_CTX_check_private_key(ctx__) != 0)
        {
            sf_debug("check private key error");
            return;
        }
        check_ok__ = true;
    }

    void sf_ssl_server_socket_filter::listen_sock_filter(SOCKET &sock) {}

    void sf_ssl_server_socket_filter::new_connection_filter(SOCKET &sock) {

    }
}