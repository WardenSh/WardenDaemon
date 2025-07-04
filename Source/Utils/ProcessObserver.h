#pragma once
#include <map>
#include <sys/types.h>
#include <thread>

class ProcessObserver {
    int EpollFd;
    std::map<int, int> FdToPid;
    std::thread ObservingThread;
    bool Stop = false;

    static int pidfd_open(pid_t pid, unsigned int flags);

    void ObserveLoop();

public:
    void AddProcessForObserving(int pid);

private:
    ProcessObserver();
    ~ProcessObserver();
public:
    // Delete copy and move operations
    ProcessObserver(const ProcessObserver&) = delete;
    ProcessObserver& operator=(const ProcessObserver&) = delete;
    ProcessObserver(ProcessObserver&&) = delete;
    ProcessObserver& operator=(ProcessObserver&&) = delete;

    static ProcessObserver* GetInstance();
};
