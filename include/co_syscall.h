#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>

namespace skyfire
{
int co_socket(int domain, int type, int protocol);

int co_epoll_wait(int __epfd, epoll_event* __events,
                  int __maxevents, int __timeout);

// helpful func
bool set_fd_nonblock(int fd);
} // namespace skyfire
