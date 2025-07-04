#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

struct sPackage
{
    std::string name;
    std::vector<std::filesystem::path> binFiles;

    [[nodiscard]] bool isCanBeRunning() const {return !binFiles.empty();};

    unsigned int runningProcesses = 0;

    [[nodiscard]] bool isRunning() const {return runningProcesses;};
};

class PackageManager
{
public:
    void AddPackage(sPackage package);
    sPackage* GetPackageByBinPath(const std::filesystem::path& binPath);
    std::vector<sPackage *> GetAllPackages();

    void Init();
private:
    std::unordered_map<std::string, sPackage> PackagesByName; // name | package
    std::unordered_map<std::string, sPackage*> PackagesByBinPath; // Bin path | package pointer from array PackagesByName

    [[nodiscard]] std::vector<sPackage> FindPacmanPackages();

public:
    // Delete copy and move operations
    PackageManager(const PackageManager&) = delete;
    PackageManager& operator=(const PackageManager&) = delete;
    PackageManager(PackageManager&&) = delete;
    PackageManager& operator=(PackageManager&&) = delete;

    static PackageManager* GetInstance();

private:
    PackageManager();
};