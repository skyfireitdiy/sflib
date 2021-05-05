#pragma once

#include "connector.h"
#include "object.h"

namespace skyfire
{
class listener
{
public:
    virtual bool                       listen(const std::string& address) = 0;
    virtual std::shared_ptr<connector> accept()                           = 0;
    virtual void                       close()                            = 0;
};
}