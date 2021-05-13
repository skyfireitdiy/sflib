#pragma once

#include "listener.h"
#include "utils.h"

namespace skyfire
{
class tcp_listener : public listener, public instance_maker<tcp_listener>
{
private:
    std::string host__;

    tcp_listener(const std::string& host, unsigned short port);

public:
    sf_error                    listen();
    std::shared_ptr<connection> accept();
    void                        close();
};
} // namespace skyfire
