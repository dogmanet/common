#include "guid.h"
#include <cstdio>

#ifdef _WINDOWS
#include <objbase.h>

void XCreateGUID(XGUID *pOut)
{
	GUID newId;
	CoCreateGuid(&newId);

	memcpy(pOut, &newId, sizeof(XGUID));
}

#define snprintf _snprintf

#elif defined(_LINUX)
#include <uuid/uuid.h>

void XCreateGUID(XGUID *pOut)
{
	uuid_generate((unsigned char*)pOut);
}

#elif defined(_MAC)
#include <CoreFoundation/CFUUID.h>

void XCreateGUID(XGUID *pOut)
{
	auto newId = CFUUIDCreate(NULL);
	auto bytes = CFUUIDGetUUIDBytes(newId);
	CFRelease(newId);

	memcpy(pOut, &bytes, sizeof(XGUID));
}
#else
#error Unknown platform!
#endif

void XGUIDToSting(const XGUID &guid, char *dst, int nBufSize)
{
	// {34D5ED67-D3D1-4ACE-9CE7-AAB5C4F9E2FD}
	snprintf(dst, nBufSize, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", guid.Data1, guid.Data2, guid.Data3,
		guid.Data40, guid.Data41, guid.Data42, guid.Data43, guid.Data44, guid.Data45, guid.Data46, guid.Data47);
}

bool XGUIDFromString(XGUID *pGUID, const char *szGUID)
{
	return(11 == sscanf(szGUID, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", &pGUID->Data1, &pGUID->Data2, &pGUID->Data3,
		&pGUID->Data40, &pGUID->Data41, &pGUID->Data42, &pGUID->Data43, &pGUID->Data44, &pGUID->Data45, &pGUID->Data46, &pGUID->Data47));
}
