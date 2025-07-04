#pragma once
#include "PackageManager.h"

#include <filesystem>
#include <thread>
#include <vector>

namespace notifycpp
{
class InotifyController;
}
namespace notifycpp
{
class Notification;
class FanotifyController;
}


class PackageObserver {
    friend class ProcessObserver;

public:
    void Init();
    void Start();


private:
    std::thread fanotifyThread;
    static inline notifycpp::FanotifyController *fanotifyController;

    std::thread uptimeLoggerThread;

    std::vector<sPackage*> runningPackages;

    std::unordered_map<int, std::vector<sPackage*>> activeProcessPackages; // Pid | Array of packages

    void AddBinFileForObserving(const std::filesystem::path& binPath);
    static void onBinFileOpen(const notifycpp::Notification &notify);

    void AddProcessForObserving(int pid);
    void onProcessClose(int pid);

    void HandlePackageLaunch(sPackage *package, unsigned int pid);
    void HandlePackageStop(sPackage *package);

    static void UpdatePackageUptime(const sPackage &package);
    void UpdatePackageLastLaunch(const sPackage &package);

    void InitializeDirectories();
    static void CreatePackageFolder(const sPackage &package);

    PackageObserver();
    ~PackageObserver();

public:
    // Delete copy and move operations
    PackageObserver(const PackageObserver&) = delete;
    PackageObserver& operator=(const PackageObserver&) = delete;
    PackageObserver(PackageObserver&&) = delete;
    PackageObserver& operator=(PackageObserver&&) = delete;

    static PackageObserver* GetInstance();
};