#pragma once
#include <unordered_set>

namespace config
{
static const std::filesystem::path pacmanPackagesDir = "/var/lib/pacman/local";
static const std::filesystem::path wardenDir = "/var/lib/warden";
static const std::filesystem::path packagesDir = "/var/lib/warden/packages";
static const std::unordered_set<std::string> listOfIgnoredPackagesForObserving = {
    "bash",
    "git"
};
} // namespace config