#include "Utils.h"

#include <filesystem>
#include <fstream>

#include <print>

namespace fs = std::filesystem;


std::vector<std::filesystem::path> Utils::FindPackageBinFiles(const fs::path &packageDir)
{
    std::vector<std::filesystem::path> binFiles;

    std::ifstream filesFile(packageDir / "files");

    if (!filesFile.is_open())
	return {};

    std::string line;
    while (getline(filesFile, line))
    {
	const std::string binDir = "usr/bin/";
	if (line.starts_with(binDir) && line.find_last_of('/') + 1 == binDir.length() &&
	    line.length() > binDir.length() && fs::exists("/" + line))
	{
	    fs::path binFile = "/" + line;
	    if (fs::is_symlink(binFile))
	    {
		binFiles.push_back(fs::weakly_canonical(fs::absolute(binFile.parent_path() / fs::read_symlink(binFile))));
	    }
	    else
	    {
	        binFiles.push_back(fs::weakly_canonical(binFile));
	    }
	}
    }

    return binFiles;
}

std::string Utils::GetCurrentDateTime()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Utils::GetPackageName(const std::filesystem::path& file)
{
    std::ifstream infile(file);
    if (!infile.is_open()) {
        throw std::runtime_error("Unable to open file: " + file.string());
    }

    std::string line;
    bool foundNameTag = false;

    while (std::getline(infile, line)) {
        if (foundNameTag) {
            return line;
        }

        if (line == "%NAME%") {
            foundNameTag = true;
        }
    }

    throw std::runtime_error("'%NAME%' tag not found or no line after it in file: " + file.string());
}