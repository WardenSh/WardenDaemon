#include "ProcessObserver.h"

#include "Core/PackageObserver.h"

#include <iostream>
#include <print>
#include <ranges>
#include <sys/epoll.h>

ProcessObserver::ProcessObserver()
{
    EpollFd = epoll_create1(0);
    if (EpollFd == -1)
    {
	throw std::runtime_error("ProcessObserver| epoll_create1 failed");
    }

    ObservingThread = std::thread(&ProcessObserver::ObserveLoop, this);
}

ProcessObserver::~ProcessObserver()
{
    Stop = true;
    close(EpollFd);
}

ProcessObserver *ProcessObserver::GetInstance()
{
    static ProcessObserver instance;
    return &instance;
}

void ProcessObserver::AddProcessForObserving(int pid)
{
    int pidfd = pidfd_open(pid, 0);
    if (pidfd == -1)
    {
	std::println(std::cerr, "ProcessObserver: Cannot open pidfd for: {}", pid);
	return;
    }

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = pidfd;

    if (epoll_ctl(EpollFd, EPOLL_CTL_ADD, pidfd, &ev) == -1)
    {
	close(pidfd);
	return;
    }
    FdToPid[pidfd] = pid;
}

int ProcessObserver::pidfd_open(pid_t pid, unsigned int flags)
{
    return syscall(SYS_pidfd_open, pid, flags);
}

void ProcessObserver::ObserveLoop()
{
    constexpr int MAX_EVENTS = 10;
    epoll_event events[MAX_EVENTS];

    while (!Stop) {
        int nfds = epoll_wait(EpollFd, events, MAX_EVENTS, 500);
        if (nfds == -1) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;
            auto it = FdToPid.find(fd);
            if (it != FdToPid.end()) {
                int pid = it->second;
                PackageObserver::GetInstance()->onProcessClose(pid); // Todo: Perhaps this needs to be done differently.
                close(fd);
                FdToPid.erase(it);
            }
        }
    }


    for (const auto &fd : FdToPid | std::views::keys) {
        close(fd);
    }

    FdToPid.clear();
}