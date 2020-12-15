#ifndef __COMMON_GUID_H
#define __COMMON_GUID_H

#include <memory.h>

typedef struct _XGUID
{
	_XGUID()
	{
	}
	_XGUID(unsigned long l, unsigned short w1, unsigned short w2,
		unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4,
		unsigned char b5, unsigned char b6, unsigned char b7, unsigned char b8):
		Data1(l), Data2(w1), Data3(w2), Data40(b1), Data41(b2), Data42(b3), Data43(b4), Data44(b5),
		Data45(b6), Data46(b7), Data47(b8)
	{
	}
	unsigned long  Data1 = 0;
	unsigned short Data2 = 0;
	unsigned short Data3 = 0;
	unsigned char  Data40 = 0;
	unsigned char  Data41 = 0;
	unsigned char  Data42 = 0;
	unsigned char  Data43 = 0;
	unsigned char  Data44 = 0;
	unsigned char  Data45 = 0;
	unsigned char  Data46 = 0;
	unsigned char  Data47 = 0;
} XGUID;

#define DEFINE_XGUID(l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    XGUID(l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)


inline bool operator<(const XGUID &a, const XGUID &b)
{
	return(memcmp(&a, &b, sizeof(XGUID)) < 0);
}

inline bool operator==(const XGUID &a, const XGUID &b)
{
	return(memcmp(&a, &b, sizeof(XGUID)) == 0);
}

void XCreateGUID(XGUID *pOut);

void XGUIDToSting(const XGUID &guid, char *dst, int nBufSize);
bool XGUIDFromString(XGUID *pGUID, const char *szGUID);

#endif
