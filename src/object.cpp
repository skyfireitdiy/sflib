
#include "object.h"
#include "eventloop.h"
#include "random.h"

namespace skyfire
{
object::~object()
{
    __p_msg_queue__->remove_msg([this](std::pair<void*, std::function<void()>> d) -> bool {
        return d.first == this;
    });
}
} // namespace skyfire
