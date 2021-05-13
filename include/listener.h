#pragma once

#include "connection.h"
#include "error.h"
#include "net_type.h"
#include "utils.h"

namespace skyfire
{
class listener
{
private:
public:
    virtual sf_error                    listen() = 0;
    virtual std::shared_ptr<connection> accept() = 0;
    virtual void                        close()  = 0;
};
}