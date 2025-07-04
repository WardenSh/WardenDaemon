#include "PackageObserver.h"

#include "PackageManager.h"
#include "Utils/DBusServer.h"
#include "Utils/ProcessObserver.h"
#include "Utils/Utils.h"
#include "Utils/config.h"
#include "notify-cpp/notification.h"
#include "notify-cpp/notify_controller.h"
#include "nlohmann/json.hpp"

#include <fstream>
#include <iostream>
#include <print>

namespace fs = std::filesystem;
using json = nlohmann::json;

PackageObserver::PackageObserver()
{
    std::println("[INFO] PackageObserver instance created");
    fanotifyController = new notifycpp::FanotifyController();
}

PackageObserver::~PackageObserver()
{
    delete fanotifyController;
}

void PackageObserver::Init()
{
    for (auto i : PackageManager::GetInstance()->GetAllPackages())
    {
	if (config::listOfIgnoredPackagesForObserving.contains(i->name) || !i->isCanBeRunning())
	    continue;

        for (auto y : i->binFiles)
        {
            AddBinFileForObserving(y);
        }
    }

    InitializeDirectories();
}

void PackageObserver::Start()
{
    fanotifyThread = std::thread([&]() { fanotifyController->run(); });
    std::println("PackageObserver| Fanotify thread started");

    uptimeLoggerThread = std::thread([&]() {
	while (true)
	{
	    for (auto i : runningPackages)
	    {
		UpdatePackageUptime(*i);
	    }

	    std::this_thread::sleep_for(std::chrono::seconds(5));
	}
    });
    std::println("PackageObserver| uptimeLogger thread started");
}

void PackageObserver::AddBinFileForObserving(const std::filesystem::path &binPath)
{
    fanotifyController->watchFile({binPath, notifycpp::Event::open})
	.onEvent(notifycpp::Event::open, &PackageObserver::onBinFileOpen);
}

void PackageObserver::HandlePackageLaunch(sPackage *package, unsigned int pid)
{
    runningPackages.push_back(package);

    /* Some packages are run using an interpreter, so one pid is shared between the current package and the interpreter package */
    if (activeProcessPackages.contains(pid))
    {
        activeProcessPackages[pid].push_back(package);
    }
    else
    {
        activeProcessPackages.emplace(pid, std::vector({package}));
        ProcessObserver::GetInstance()->AddProcessForObserving(pid);
    }

    package->runningProcesses++;

    UpdatePackageLastLaunch(*package);

    std::println("PackageObserver| Package launched: {} -> PID: {}", package->name, pid);
    DBusServer::GetInstance()->PackageLaunchSignal(package->name);
}

void PackageObserver::HandlePackageStop(sPackage *package)
{

}

void PackageObserver::onBinFileOpen(const notifycpp::Notification &notify)
{
    auto package = PackageManager::GetInstance()->GetPackageByBinPath(notify.getPath());

    if (notify.getEvent() == notifycpp::Event::open && !package->isRunning())
    {
	GetInstance()->HandlePackageLaunch(package, notify.getPid());
    }
}

void PackageObserver::onProcessClose(int pid)
{
    auto packages = activeProcessPackages[pid];
    activeProcessPackages.erase(pid);
    for (auto i : packages)
    {
        i->runningProcesses--;
        if (!i->isRunning())
        {
            std::erase(runningPackages, i);
            std::println("PackageObserver| Package close: {}", i->name);
            DBusServer::GetInstance()->PackageStopSignal(i->name);
        }
    }
}

void PackageObserver::UpdatePackageUptime(const sPackage &package)
{
    CreatePackageFolder(package);
    fs::path uptimePath = config::packagesDir / package.name / "uptime";

    if (!fs::exists(uptimePath))
    {
	std::ofstream createFile(uptimePath);
	createFile << "0";
    }

    int currentUptime = 0;
    {
	std::ifstream inFile(uptimePath);
	if (inFile.is_open())
	{
	    inFile >> currentUptime;
	}
	else
	{
	    std::println(std::cerr, "PackageObserver| Failed to read uptime file for: {}", package.name);
	    return;
	}
    }

    std::ofstream outFile(uptimePath, std::ios::trunc);
    if (outFile.is_open())
    {
	outFile << (currentUptime + 5);
    }
    else
    {
	std::println(std::cerr, "PackageObserver| Failed to write uptime file for: {}", package.name);
    }
}
void PackageObserver::UpdatePackageLastLaunch(const sPackage &package)
{
    CreatePackageFolder(package);

    std::filesystem::path dataFile = config::packagesDir / package.name / "data";
    json j;

    if (fs::exists(dataFile)) {
        std::ifstream inFile(dataFile);
        try {
            inFile >> j;
        } catch (const json::parse_error& e) {
            std::cerr << "Ошибка при разборе JSON: " << e.what() << std::endl;
            j = json::object();
        }
    } else {
        j = json::object();
    }

    j["LastLaunch"] = Utils::GetCurrentDateTime();

    std::ofstream outFile(dataFile);
    outFile << std::setw(4) << j << std::endl;
}

void PackageObserver::InitializeDirectories()
{
    if (!exists(config::wardenDir))
    {
	fs::create_directory(config::wardenDir);
    }

    if (!exists(config::packagesDir))
    {
	fs::create_directory(config::packagesDir);
    }
}

void PackageObserver::CreatePackageFolder(const sPackage &package)
{
    if (!exists(config::packagesDir / package.name))
    {
        fs::create_directory(config::packagesDir / package.name);
    }
}

PackageObserver *PackageObserver::GetInstance()
{
    static PackageObserver instance;
    return &instance;
}