#pragma once
#include <string>
#include <filesystem>
#include <vector>

class Utils {
public:
    /* Function can return an empty path */
    static std::vector<std::filesystem::path> FindPackageBinFiles(const std::filesystem::path& packageDir);

    static std::string GetCurrentDateTime();

    static std::string GetPackageName(const std::filesystem::path& file);
};
