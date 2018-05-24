
/******************************************************
Copyright � Vitaliy Buturlin, Evgeny Danilovich, 2018
See the license in LICENSE
******************************************************/

#ifndef __FILE_UTILS_H
#define __FILE_UTILS_H

#include <windows.h>
#include <sys/stat.h>
#include <time.h>
#include "string.h"
#include "string_utils.h"
#include "array.h"

//! ���������� �� ����
bool FileExistsFile(const char *szPath);

//! ���������� �� ����������
bool FileExistsDir(const char *szPath);

//! ���������� �� ���� (���� ��� ����������)
#define FileExistsPath(szPath)(FileExistsFile(szPath) || FileExistsDir(szPath))

//! ���� ������� ��� #FileGetList
enum FILE_LIST_TYPE
{
	//! �����
	FILE_LIST_TYPE_FILES,

	//! ����������
	FILE_LIST_TYPE_DIRS,

	//! ���
	FILE_LIST_TYPE_ALL,
};

/*! ���������� ������ �� ����� ������� ������������ �� ���� szPath, 
szPath ����� ���� �� ������������� � ������������� ������ ������������� ������, 
szPath ��� ����� ��������� ������ (� ������� C:/*), �� ���� ��� ���, ����� ������� ��������� ����
*/
Array<String> FileGetList(const char *szPath, FILE_LIST_TYPE type);

//! ���������� ������ ������ � ���������� szPath
#define FileGetListFiles(szPath)(FileGetList(szPath, FILE_LIST_TYPE_FILES))

//! ���������� ������ ���������� � ���������� szPath
#define FileGetListDirs(szPath)(FileGetList(szPath, FILE_LIST_TYPE_DIRS))

//! ���������� ������ ������ � ���������� � ���������� szPath
#define FileGetListAll(szPath)(FileGetList(szPath, FILE_LIST_TYPE_ALL))


/*! ���������� ������ ���� ������ ��� ����� (� ����������� �� type), 
szPath �� ������ ��������� ��������, ����� ���� �� ������������� � ��� ���������� �����, 
szExt - ���������� ����� ��� �����
*/
Array<String> FileGetListRec(const char *szPath, FILE_LIST_TYPE type, const char *szExt = 0);


const char *FileBaseName(const char *szPath);

const char *FileDirName(char *szPath);


//! ����������� ������ ���� (���������� ������ � ������� ����), �������� �������� ������ szPath
const char *FileCanonizePath(char *szPath);

//! ����������� ������ ���� (���������� ������ � ������� ����), ���������� ����� ������
String FileCanonizePathS(const char *szPath);

//! ���������� ���������� ����������� �����/�����
int FileCountNesting(const char *szPath);

//! ���������� ���� �� ���� �������� ������, �� ���� �������� ��������� /
String FileGetPrevDir(const char *szPath);

//! ���� �� � ����� ������ ����?
bool FileExistsEndSlash(const char *szPath);

//! ��������� � ����� ������ ����
String FileAppendSlash(const char *szPath);

//! ������ �� szSubPath � szPath
bool FileExistsInPath(const char *szPath, const char *szSubPath);

//! ��������� ����� � ������ szPath (����) ���������� szExt, ���� ���������� ��� ���� �� ����� �������� �� �����
String FileSetStrExt(const char *szPath, const char *szExt);

//! ���������� ���������� ����� � ������ szPath � ����������� szExt (��� �����)
bool FileStrIsExt(const char *szPath, const char *szExt);


//! �������� ����������, � ��� ����� � ���������
bool FileCreateDir(const char *szPath);

//! ���������� ����� ���������� ��������� �����
UINT FileGetTimeLastModify(const char *szPath);

//! ���������� ������ ����� � ������
int FileGetSizeFile(const char *szPath);


#endif