
/******************************************************
Copyright © Vitaliy Buturlin, Evgeny Danilovich, 2018
See the license in LICENSE
******************************************************/

#include "file_utils.h"

XDEPRECATED bool FileExistsFile(const char *szPath)
{
	WIN32_FIND_DATA wfd;
	HANDLE hFind = ::FindFirstFile(szPath, &wfd);
	if (INVALID_HANDLE_VALUE != hFind)
	{
		::FindClose(hFind);
		return true;
	}
	return false;
}

int FileGetSizeFile(const char *szPath)
{
	struct stat fi;
	stat(szPath, &fi);
	
	return fi.st_size;
}

XDEPRECATED bool FileExistsDir(const char *szPath)
{
	DWORD dwFileAttributes = GetFileAttributes(szPath);
	if (dwFileAttributes == 0xFFFFFFFF)
		return false;
	return((dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

XDEPRECATED Array<String> FileGetList(const char *szPath, FILE_LIST_TYPE type)
{
	Array<String> aStrings;
	WIN32_FIND_DATA fd;

	String sPath = FileCanonizePathS(szPath);
	if (sPath.length() >= 2 && (sPath.find("*") == -1))
	{
		if (sPath[sPath.length() - 1] != '/')
		{
			sPath += "/";
		}
		sPath += "*";
	}

	HANDLE hFind = ::FindFirstFile(sPath.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(strlen(fd.cFileName) == 1 && fd.cFileName[0] == '.') && !(strlen(fd.cFileName) == 2 && fd.cFileName[0] == '.' && fd.cFileName[1] == '.') &&
				(
					(type == FILE_LIST_TYPE_FILES && !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) ||
					(type == FILE_LIST_TYPE_DIRS && fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
					(type == FILE_LIST_TYPE_ALL)
				)
				)
				aStrings.push_back(fd.cFileName);
		}
		while (::FindNextFile(hFind, &fd));

		::FindClose(hFind);
	}

	return aStrings;
}

XDEPRECATED Array<String> FileGetListRec(const char *szPath, FILE_LIST_TYPE type, const char *szExt)
{
	Array<String> aStrings;
	Array<String> aQueue;
	WIN32_FIND_DATA fd;

	String sRootPath = FileAppendSlash(FileCanonizePathS(szPath).c_str());
	aQueue.push_back("");
	int iCurrDir = 0;

	while((int)aQueue.size() > iCurrDir)
	{
		String sCurrPath = FileCanonizePathS((sRootPath + aQueue[iCurrDir]).c_str());
		if(sCurrPath.length() >= 2 && sCurrPath[sCurrPath.length() - 1] != '*')
			sCurrPath += "*";

		HANDLE hFind = ::FindFirstFile(sCurrPath.c_str(), &fd);
		if(hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if(!(strlen(fd.cFileName) == 1 && fd.cFileName[0] == '.') && !(strlen(fd.cFileName) == 2 && fd.cFileName[0] == '.' && fd.cFileName[1] == '.'))
				{
					if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						String sNewDir = aQueue[iCurrDir] + fd.cFileName;
						aQueue.push_back(FileAppendSlash(sNewDir.c_str()));
					}

					if (
						(type == FILE_LIST_TYPE_FILES && !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (!szExt || (szExt && FileStrIsExt(fd.cFileName, szExt)))) ||
						(type == FILE_LIST_TYPE_DIRS && fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
						(type == FILE_LIST_TYPE_ALL)
						)
						aStrings.push_back(aQueue[iCurrDir] + fd.cFileName);
				}
			}
			while(::FindNextFile(hFind, &fd));

			::FindClose(hFind);
		}

		++iCurrDir;
	}

	return aStrings;
}

XDEPRECATED const char *FileBaseName(const char *szPath)
{
	const char *szPos = szPath;
	while (*szPath)
	{
		if (*szPath == '/' || *szPath == '\\')
		{
			szPos = szPath + 1;
		}
		++szPath;
	}
	return(szPos);
}

XDEPRECATED const char *FileDirName(char *szPath)
{
	char * pos = szPath, *ret = szPath;
	while (*szPath)
	{
		if (*szPath == '/' || *szPath == '\\')
		{
			pos = szPath + 1;
		}
		++szPath;
	}
	*pos = 0;
	return(ret);
}

XDEPRECATED const char *FileCanonizePath(char *szPath)
{
	char * ret = szPath;
	while (*szPath)
	{
		if (*szPath == '\\')
		{
			*szPath = '/';
		}
		++szPath;
	}
	return(ret);
}

XDEPRECATED String FileCanonizePathS(const char *szPath)
{
	String sCanonizePath = szPath;
	for (int i = 0, il = sCanonizePath.length(); i < il; ++i)
	{
		if (sCanonizePath[i] == '\\')
			sCanonizePath[i] = '/';
	}

	return sCanonizePath;
}

XDEPRECATED int FileCountNesting(const char *szPath)
{
	int iCount = 0;

	if (szPath[strlen(szPath)-1] == '/' || szPath[strlen(szPath)-1] == '\\')
		--iCount;

	for (int i = 0, il = (int)strlen(szPath); i < il; ++i)
	{
		if (szPath[i] == '/' || szPath[i] == '\\')
			++iCount;
	}

	return iCount;
}

XDEPRECATED String FileGetPrevDir(const char *szPath)
{
	int iPosDel = 0;

	for(int i = 1, il = (int)strlen(szPath) - 1; i < il; ++i)
	{
		if (szPath[il - i] == '/' || szPath[il - i] == '\\')
		{
			iPosDel = (il - i) + 1;
			break;
		}
	}

	String sStr = szPath;

	if (iPosDel > 0)
		sStr = sStr.substr(0, iPosDel - 1);

	return sStr;
}

XDEPRECATED bool FileExistsEndSlash(const char *szPath)
{
	return (strlen(szPath) > 0 && (szPath[strlen(szPath) - 1] == '\\' || szPath[strlen(szPath) - 1] == '/'));
}

XDEPRECATED String FileAppendSlash(const char *szPath)
{
	String sNewPath = szPath;

	if (!FileExistsEndSlash(szPath))
		sNewPath += "/";

	return sNewPath;
}

XDEPRECATED bool FileExistsInPath(const char *szPath, const char *szSubPath)
{
	String sPath = StrToLower(szPath);

	for (int i = 0, il = sPath.length(); i < il; ++i)
	{
		if (sPath[i] == '\\')
			sPath[i] = '/';
	}

	String sSubPath = StrToLower(szSubPath);

	for (int i = 0, il = sSubPath.length(); i < il; ++i)
	{
		if (sSubPath[i] == '\\')
			sSubPath[i] = '/';
	}

	return(strstr(sPath.c_str(), sSubPath.c_str()) != NULL);
}

XDEPRECATED bool FileCreateDir(const char *szPath)
{
	if (!strstr(szPath, "\\") && !strstr(szPath, "/"))
	{
		return(!!CreateDirectory(szPath, 0));
	}

	String sPath = FileAppendSlash(FileCanonizePathS(szPath).c_str());
	int iPosSlash = 0;
	int iOldPos = 0;

	while ((iPosSlash = sPath.find("/", iOldPos)) >= 0)
	{
		String sDir = sPath.substr(0, iPosSlash);
		iOldPos = iPosSlash + 1;
		if (sDir.length() > 0 && !FileExistsDir(sDir.c_str()))
		{
			if (!CreateDirectory(sDir.c_str(), 0))
				return false;
		}
	}

	return true;
}

XDEPRECATED time_t FileGetTimeLastModify(const char *szPath)
{
	HANDLE hFile = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return 0;

	SYSTEMTIME stUTC;
	FILETIME ftCreate, ftAccess, ftWrite;
	GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite);
	FileTimeToSystemTime(&ftWrite, &stUTC);

	tm tmObj;
	ZeroMemory(&tmObj, sizeof(tm));
	tmObj.tm_year = stUTC.wYear - 1900;
	tmObj.tm_mon = stUTC.wMonth;
	tmObj.tm_mday = stUTC.wDay;
	tmObj.tm_hour = stUTC.wHour;
	tmObj.tm_min = stUTC.wMinute;
	tmObj.tm_sec = stUTC.wSecond;

	time_t tLastModify = mktime(&tmObj);

	CloseHandle(hFile);

	return tLastModify;
}

XDEPRECATED String FileSetStrExt(const char *szPath, const char *szExt)
{
	String sPath = szPath;

	int iPosPoint = -1;

	for (int i = 0, il = sPath.length(); i < il; ++i)
	{
		if (sPath[(il - 1) - i] == '.')
		{
			iPosPoint = (il - 1) - i;
			break;
		}
	}

	if (iPosPoint >= 0)
		sPath = sPath.substr(0, iPosPoint + (szExt[0] != 0 ? 1 : 0))+szExt;
	else if (szExt && szExt[0] != 0)
	{
		if (szExt[0] == '.')
			sPath += szExt;
		else
			sPath += String(".") + szExt;
	}

	return sPath;
}

XDEPRECATED bool FileStrIsExt(const char *szPath, const char *szExt)
{
	if (!szPath || !szExt)
		return false;

	int iPosPoint = -1;

	for (int i = 0, il = (int)strlen(szPath); i < il; ++i)
	{
		if (szPath[(il - 1) - i] == '.')
		{
			iPosPoint = ((il - 1) - i) + 1;
			break;
		}
	}

	return (iPosPoint >= 0 && strcasecmp(szPath + iPosPoint, szExt) == 0);
}