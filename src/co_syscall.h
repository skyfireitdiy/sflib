#include <sys/socket.h>
#include <sys/types.h>

namespace skyfire
{
int co_socket(int domain, int type, int protocol);

// helpful func
bool set_fd_nonblock(int fd);
} // namespace skyfire
