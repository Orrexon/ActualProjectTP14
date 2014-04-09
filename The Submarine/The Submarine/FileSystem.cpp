#include "FileSystem.h"
#include <Windows.h>
#include <iostream>

namespace FileSystem
{
	std::vector<std::string> getFilesFromDirectory(const std::string &directory, const std::string &refcstrExtension)
	{
		std::string              strPattern;
		std::string              extension;
		std::vector<std::string> files;
		HANDLE                   hFile;
		WIN32_FIND_DATAA         fileInformation;

		strPattern = directory + "\\*.*";
		hFile = FindFirstFileA(strPattern.c_str(), &fileInformation);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (fileInformation.cFileName[0] != '.')
				{
					// Check extension
					extension = fileInformation.cFileName;
					extension = extension.substr(extension.rfind(".") + 1);
					
					if (extension == "*" || extension == refcstrExtension)
					{
						files.push_back(fileInformation.cFileName);
					}
				}
			} while (FindNextFileA(hFile, &fileInformation) == TRUE);
			FindClose(hFile);
		}
		return files;
	}

	int countFiles(const std::string &refcstrRootDirectory, const std::string &refcstrExtension, bool bSubdirectories)
	{
		int              iCount = 0;
		std::string      strFilePath;          // Filepath
		std::string      strPattern;           // Pattern
		std::string      strExtension;         // Extension
		HANDLE           hFile;                // Handle to file
		WIN32_FIND_DATAA FileInformation;      // File information


		strPattern = refcstrRootDirectory + "\\*.*";
		hFile = FindFirstFileA(strPattern.c_str(), &FileInformation);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (FileInformation.cFileName[0] != '.')
				{
					strFilePath.erase();
					strFilePath = refcstrRootDirectory +
						"\\" +
						FileInformation.cFileName;

					if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if (bSubdirectories)
						{
							// Search subdirectory
							int iRC = countFiles(strFilePath,
								refcstrExtension,
								bSubdirectories);
							if (iRC != -1)
								iCount += iRC;
							else
								return -1;
						}
					}
					else
					{
						// Check extension
						strExtension = FileInformation.cFileName;
						strExtension = strExtension.substr(strExtension.rfind(".") + 1);

						if ((refcstrExtension == "*") ||
							(strExtension == refcstrExtension))
						{
							// Increase counter
							++iCount;
						}
					}
				}
			} while (FindNextFileA(hFile, &FileInformation) == TRUE);

			// Close handle
			FindClose(hFile);
		}

		return iCount;
	}
}