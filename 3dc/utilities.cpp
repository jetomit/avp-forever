
#include "utilities.h"
#include <string>
#include "configFile.h"
#include <assert.h>

#ifdef WIN32
	#include <shlobj.h>
	#include <shlwapi.h>
#endif

static char saveFolder[MAX_PATH] = {0};

extern "C" {

#ifdef WIN32
	extern HWND	hWndMain;
#endif

char *GetSaveFolderPath()
{
	// check if we've got the path previously and use it again
	if (*saveFolder)
		return saveFolder;

#ifdef _XBOX
	// just blank the char array
	saveFolder[0] = '\0';
	return saveFolder;
#endif
#ifdef WIN32

	if (FAILED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, saveFolder)))
	{
		return 0;
	}

	strcat(saveFolder, "\\My Games");

	// first check if "My Games" folder exists, we try create it..
	if (CreateDirectory(saveFolder, NULL) == 0)
	{
		// something went wrong..
		DWORD error = GetLastError();
		if (error == ERROR_ALREADY_EXISTS)
		{
			// this is fine
		}
		else if (error == ERROR_PATH_NOT_FOUND)
		{
			return NULL;
		}
	}

	strcat(saveFolder, "\\Aliens versus Predator");
	
	// then check Aliens versus Predator
	if (CreateDirectory(saveFolder, NULL) == 0)
	{
		// something went wrong..
		DWORD error = GetLastError();
		if (error == ERROR_ALREADY_EXISTS)
		{
			// this is fine
		}
		else if (error == ERROR_PATH_NOT_FOUND)
		{
			return NULL;
		}
	}

	strcat(saveFolder, "\\");

	// also, create User_Profiles folder if required
	char tempPath[MAX_PATH] = {0};
	strcpy(tempPath, saveFolder);
	strcat(tempPath, "User_Profiles\\");

	if (CreateDirectory(tempPath, NULL) == 0)
	{
		// something went wrong..
		DWORD error = GetLastError();
		if (error == ERROR_ALREADY_EXISTS)
		{
			// this is fine
		}
		else if (error == ERROR_PATH_NOT_FOUND)
		{
			return NULL;
		}
	}

	// we're ok if we're here?
	return saveFolder;
#endif
}

FILE *avp_fopen(const char *fileName, const char *mode)
{
	FILE *theFile = 0;
	std::string finalPath;
#ifdef _XBOX
	finalPath.append("d:\\");
	finalPath.append(fileName);
//	return fopen(finalPath.c_str(), mode);
	theFile = fopen(finalPath.c_str(), mode);

	if (theFile == NULL)
	{
		int i = 0;
	}

	return theFile;
#endif
#ifdef WIN32
	return fopen(fileName, mode);
#endif
}

DWORD avp_GetFileAttributes(LPCTSTR lpFileName)
{
#ifdef _XBOX

	std::string finalPath;

	finalPath.append("d:\\");
	finalPath.append(lpFileName);

	return GetFileAttributes(finalPath.c_str());
#endif
#ifdef WIN32
	return GetFileAttributes(lpFileName);
#endif
}

HANDLE avp_CreateFile(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
#ifdef _XBOX

	std::string finalPath;

	finalPath.append("d:\\");
	finalPath.append(lpFileName);

	HANDLE theHandle = CreateFile(finalPath.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

	if (theHandle == INVALID_HANDLE_VALUE)
	{
		int i = 0;
	}

	return theHandle;

#endif
#ifdef WIN32
	return CreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
#endif
}

HANDLE avp_FindFirstFile(LPCTSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData)
{
#ifdef _XBOX

	std::string finalPath;

	finalPath.append("d:\\");
	finalPath.append(lpFileName);
	return FindFirstFile(finalPath.c_str(), lpFindFileData);
#endif
#ifdef WIN32
	return FindFirstFile(lpFileName, lpFindFileData);
#endif	
}

void avp_GetCommandLineArgs(char *args, int size)
{
	assert (args);
	std::string commandLine = Config_GetString("[Misc]", "CommandLine");
	strcpy(args, commandLine.c_str());
}

void avp_MessageBox(const char* message, int type)
{
#ifdef WIN32
	MessageBox(hWndMain, message, "AvP Error", type);
#endif
}

void avp_exit(int code)
{
#ifdef WIN32
	exit(code);
#endif
}

};