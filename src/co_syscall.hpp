#include "co_syscall.h"
#include <fcntl.h>
#include <unistd.h>

namespace skyfire
{

int co_socket(int domain, int type, int protocol)
{
    int sock = socket(domain, type, protocol);
    if (sock == -1)
    {
        return -1;
    }
    if (!set_fd_nonblock(sock))
    {
        close(sock);
        return -1;
    }
    return sock;
}

bool set_fd_nonblock(int fd)
{
    int flag = fcntl(fd, F_GETFL, NULL);
    if (flag < 0)
    {
        return false;
    }
    return fcntl(fd, F_SETFL, flag | O_NONBLOCK) != -1;
}

} // namespace skyfire
