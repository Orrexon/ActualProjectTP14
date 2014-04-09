#pragma once
#include <string>
#include <vector>

namespace FileSystem
{
	std::vector<std::string> getFilesFromDirectory(const std::string &directory, const std::string &refcstrExtension);
	int countFiles(const std::string &refcstrRootDirectory, const std::string &refcstrExtension, bool bSubdirectories = true);
}