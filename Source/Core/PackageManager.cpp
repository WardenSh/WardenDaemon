#include "PackageManager.h"

#include "PackageObserver.h"
#include "Utils/Utils.h"
#include "Utils/config.h"

#include <fstream>
#include <iostream>

#include <print>

PackageManager::PackageManager()
{
    std::println("[INFO] PackageManager instance created");
}

void PackageManager::Init()
{
    for ( auto i : FindPacmanPackages())
    {
        AddPackage(i);
    }

    std::println("PackageManager| Found {} packages", PackagesByName.size());
}

std::vector<sPackage> PackageManager::FindPacmanPackages()
{
    std::vector<sPackage> pacmanPackages;

    const std::filesystem::path packagesDir(config::pacmanPackagesDir);
    if (!std::filesystem::exists(packagesDir)) // Todo: In the future, make validation of config values at startup
    {
	std::println(std::cerr, "[FATAL ERROR] PackageManager| pacman local directory {} not exist",
		     packagesDir.string());
	abort();
    }

    for (auto const &i : std::filesystem::directory_iterator{packagesDir})
    {
        if (!is_directory(i.path())) continue;

	std::string packageName = Utils::GetPackageName(i.path()/"desc");
	auto binFiles = Utils::FindPackageBinFiles(i.path());

	pacmanPackages.push_back({packageName, binFiles});
    }

    return pacmanPackages;
}

void PackageManager::AddPackage(sPackage package)
{
    PackagesByName.insert({package.name, package});
    if (package.isCanBeRunning())
    {
        for (auto i : package.binFiles)
        {
            PackagesByBinPath.insert({i, &PackagesByName[package.name]});
        }
    }
}

PackageManager *PackageManager::GetInstance()
{
    static PackageManager instance;
    return &instance;
}

sPackage *PackageManager::GetPackageByBinPath(const std::filesystem::path &binPath)
{
    return PackagesByBinPath[binPath.string()];
}

std::vector<sPackage *> PackageManager::GetAllPackages()
{
    std::vector<sPackage *> allPackages;
    for (auto &i : PackagesByName)
    {
	allPackages.push_back(&i.second);
    }

    return allPackages;
}