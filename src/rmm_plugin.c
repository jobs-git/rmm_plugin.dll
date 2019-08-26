#include "skse/PluginAPI.h"
#include "skse/skse_version.h"
#include "skse/SafeWrite.h"
#include "skse/ScaleformCallbacks.h"
#include "skse/ScaleformMovie.h"
#include "skse/GameAPI.h"

#include <vector>
#include <stdio.h>
#include <time.h>

IDebugLog	gLog("skse_rmm_plugin.log");

PluginHandle	g_pluginHandle = kPluginHandle_Invalid;

SKSEScaleformInterface	* scaleform = NULL;

#include <string>
#include <iostream>

#include <conio.h>


bool copyDirectory(const std::string &sourceDirectory, const std::string &destinationDirectory)
{
	std::string strSource;				// Source file
	std::string strDestination;			// Destination file
	std::string strPattern;				// Pattern
	HANDLE hFile;						// Handle to file
	WIN32_FIND_DATA fileInformation;	// File information

	strPattern = sourceDirectory + "\\*.*";

	hFile = ::FindFirstFile(strPattern.c_str(), &fileInformation);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(fileInformation.cFileName[0] != '.')
			{
				strSource.erase();
				strDestination.erase();

				strSource = sourceDirectory + "\\" + fileInformation.cFileName;
				strDestination = destinationDirectory + "\\" + fileInformation.cFileName;

				if(fileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					// Copy subdirectory
					if(!copyDirectory(strSource, strDestination))
						return false;
				}
				else
				{
					// Copy file
					if(::CopyFile(strSource.c_str(), strDestination.c_str(), FALSE) == FALSE)
					{
						_MESSAGE(TEXT("Error in file copy from %s to %s - %d"), strSource.c_str(), strDestination.c_str(), ::GetLastError());
						return false;
					}
				}
			}
		} while(::FindNextFile(hFile, &fileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);

		DWORD dwError = ::GetLastError();
		if(dwError != ERROR_NO_MORE_FILES)
		{
			_MESSAGE(TEXT("Error in file copy - %d"), ::GetLastError());
			return false;
		}
	}
	else
	{
		_MESSAGE(TEXT("Error in copyDirectory.findFirstFile - %d"), ::GetLastError());
		return false;
	}

	return true;
}

bool loadRandomWallPaper()
{
	int             iCount          = 0;
	std::string     strFilePath;			// Filepath
	std::string     strPattern;				// Pattern
	HANDLE          hFile;					// Handle to file
	WIN32_FIND_DATA fileInformation;		// File information
	std::vector<std::string> wallpapers;		// wallpaper list
	std::string sourceDirectory;			// source wallpaper directory

	strPattern = ".\\Data\\MainMenuWallpapers\\*";
	hFile = ::FindFirstFile(strPattern.c_str(), &fileInformation);

	if (hFile == INVALID_HANDLE_VALUE) 
	{
		_MESSAGE(TEXT("Data\\MainMenuWallpapers - Wallpaper source Folder error : (%d)"), GetLastError());
		return false;
	}

	do
	{
		if(fileInformation.cFileName[0] != '.' && fileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			wallpapers.push_back(fileInformation.cFileName);
		}
	} while(::FindNextFile(hFile, &fileInformation) == TRUE);

    // Close handle
    ::FindClose(hFile);

	// Get random wallpaper (folder)
	time_t seconds;
	seconds = time(NULL);
	// Copy folder content
	sourceDirectory = ".\\Data\\MainMenuWallpapers\\";
	sourceDirectory.append(wallpapers.at(seconds % wallpapers.size()).c_str());
	return copyDirectory(sourceDirectory, ".\\");
}

extern "C"
{

bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info)
{
	// populate info structure
	info->infoVersion =	PluginInfo::kInfoVersion;
	info->name =		"Random main menu plugin";
	info->version =		1;

	// store plugin handle so we can identify ourselves later
	g_pluginHandle = skse->GetPluginHandle();

	if(skse->isEditor)
	{
		_MESSAGE("loaded in editor, marking as incompatible");

		return false;
	}
	/*else if(skse->runtimeVersion != RUNTIME_VERSION_1_3_10_0)
	{
		_MESSAGE("unsupported runtime version %08X", skse->runtimeVersion);

		return false;
	}*/

	// get the scaleform interface and query its version
	scaleform = (SKSEScaleformInterface *)skse->QueryInterface(kInterface_Scaleform);
	if(!scaleform)
	{
		_MESSAGE("couldn't get scaleform interface");

		return false;
	}

	if(scaleform->interfaceVersion < SKSEScaleformInterface::kInterfaceVersion)
	{
		_MESSAGE("scaleform interface too old (%d expected %d)", scaleform->interfaceVersion, SKSEScaleformInterface::kInterfaceVersion);

		return false;
	}

	// ### do not do anything else in this callback
	// ### only fill out PluginInfo and return true/false

	// supported runtime version
	return true;
}

bool SKSEPlugin_Load(const SKSEInterface * skse)
{
	return loadRandomWallPaper();
}

};
