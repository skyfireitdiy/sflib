#pragma once

#include "connector.h"
#include "error.h"
#include "net_type.h"
#include "utils.h"

namespace skyfire
{
class listener : public make_instance_t<listener>
{
private:
    sock_type sock_type__;

public:
    sf_error                   listen(const std::string& address);
    std::shared_ptr<connector> accept();
    void                       close();
};
}