/*****************************************************
Copyright © DogmaNet Team, 2020
Site: dogmanet.ru
See the license in LICENSE
*****************************************************/

#ifndef __PATH_UTILS_H
#define __PATH_UTILS_H

#include "string.h"
#include "string_utils.h"

//##########################################################################

//! возвращает последний компонент имени из указанного пути
inline const char *PathBaseName(const char *szPath)
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

//**************************************************************************

//! возвращает имя последней директории в пути
inline const char *PathDirName(char *szPath)
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

//**************************************************************************

//! канонизация пути
inline const char *PathCanonizePath(char *szPath)
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

//**************************************************************************

//! канонизация пути
inline void PathCanonizePath(String *sPath)
{
	for (int i = 0, il = sPath->length(); i < il; ++i)
	{
		if ((*sPath)[i] == '\\')
			(*sPath)[i] = '/';
	}
}

//**************************************************************************

//! возвращает количество директорий в пути
inline int PathCountDirs(const char *szPath)
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

//**************************************************************************

//! возвращает предыдущую директорию в пути
inline String PathGetPrevDir(const char *szPath)
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

//**************************************************************************

//! завершен ли путь обратным слэшем
inline bool PathCompleted(const char *szPath)
{
	return (strlen(szPath) > 0 && (szPath[strlen(szPath) - 1] == '\\' || szPath[strlen(szPath) - 1] == '/'));
}

//**************************************************************************

//! завершить путь (добавить обратный слэш в конец пути)
inline String PathComplete(const char *szPath)
{
	String sNewPath = szPath;

	if (!PathCompleted(szPath))
		sNewPath += "/";

	return sNewPath;
}

//**************************************************************************

//! есть ли в szPath путь szSubPath
inline bool PathExistsSubPath(const char *szPath, const char *szSubPath)
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

//**************************************************************************

//! установить в szPath расширение szExt
inline String PathSetExt(const char *szPath, const char *szExt)
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

//**************************************************************************

//! содерижт ли szPath расширение szExt
inline bool PathStrIsExt(const char *szPath, const char *szExt)
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

#endif // __PATH_UTILS_H
