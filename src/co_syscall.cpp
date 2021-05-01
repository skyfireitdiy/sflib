#include "co_syscall.h"
#include "co_env.h"
#include "logger.h"
#include <bits/stdc++.h>
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

int co_epoll_wait(int __epfd, epoll_event* __events,
                  int __maxevents, int __timeout)
{
    auto until = std::chrono::system_clock::now();
    if (__timeout >= 0)
    {
        until += std::chrono::milliseconds(__timeout);
    }
    int wait_fds = 0;
    sf_debug("start epoll_wait");
    do
    {
        if ((wait_fds = ::epoll_wait(__epfd, __events, __maxevents, 0)) == -1)
        {
            if (errno == EINTR)
            {
                get_co_env()->yield_coroutine();
                continue;
            }
            sf_debug("wait error");
            return -1;
        }
        if (wait_fds == 0)
        {
            get_co_env()->yield_coroutine();
        }
        else
        {
            return wait_fds;
        }
    } while (__timeout >= 0 ? std::chrono::system_clock::now() < until : true);
    sf_debug("timeout");
    return 0;
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
