
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcpclient_win.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#include <winsock2.h>
#include <string>
#include <functional>
#include <memory>
#include <thread>
#include "sf_tcpclient_interface.h"

namespace skyfire
{
    class sf_tcpclient : public sf_tcpclient_interface
    {

    private:
        bool raw__ = false;
        bool inited__ = false;
        SOCKET sock__ = INVALID_SOCKET;
    public:

        SOCKET get_raw_socket() override;

        bool bind(const std::string& ip, unsigned short port) override;

        sf_tcpclient(bool raw = false);

        static std::shared_ptr <sf_tcpclient> make_client(bool raw = false);

        ~sf_tcpclient();

        bool connect_to_server(const std::string &ip, unsigned short port) override;


        bool send(int type, const byte_array &data) override;

        bool send(const byte_array & data) override;

        void close();
    };

}
