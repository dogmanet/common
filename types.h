
/******************************************************
Copyright © Vitaliy Buturlin, Evgeny Danilovich, 2017
See the license in LICENSE
******************************************************/

#ifndef __TYPES_H
#define __TYPES_H

#ifdef _WIN64
#	define _X64
#endif

#include <cstdint>
#include <string.h>
#include <memory.h>
#include <ctype.h>
#include <assert.h>
#include <mutex>
#include <atomic>
#include "spinlock.h"
#include "guid.h"

#ifdef __unix
#include <unistd.h>
#include <malloc.h>
#include <stdarg.h>
#include <pthread.h>
#endif

#include "enum_flags.h"

using std::mutex;
typedef std::unique_lock<std::mutex> ScopedLock;
typedef std::unique_lock<SpinLock> ScopedSpinLock;


typedef float float32_t;
static_assert(sizeof(float32_t) == 4, "Please check float32_t on your platform. The size should be exactly 4 bytes");

typedef int32_t ID;
typedef unsigned long DWORD;
typedef unsigned long ULONG;
typedef unsigned short WORD;
//typedef unsigned int size_t;
typedef int BOOL;
typedef unsigned char BYTE, byte, UINT8;
typedef unsigned int UINT;
typedef uint16_t UINT16;
typedef wchar_t WCHAR;
typedef unsigned long ULONG;
typedef void* SXWINDOW;
#define XWINDOW_OS_HANDLE SXWINDOW

#ifdef _X64
typedef uint64_t UINT_PTR;
#else
typedef uint32_t UINT_PTR;
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifdef _MSC_VER
#	define strcasecmp _stricmp

inline const char* strcasestr(const char *haystack, const char *needle)
{
	size_t iLen1 = strlen(haystack);
	size_t iLen2 = strlen(needle);

	if(iLen2 > iLen1)
	{
		return(NULL);
	}
	bool isFound = false;
	for(size_t i = 0, l = iLen1 - iLen2; i <= l; ++i)
	{
		if(tolower(*haystack) == tolower(*needle))
		{
			isFound = true;
			for(size_t j = 1; j < iLen2; ++j)
			{
				if(tolower(haystack[j]) != tolower(needle[j]))
				{
					isFound = false;
					break;
				}
			}
		}
		if(isFound)
		{
			return(haystack);
		}
		++haystack;
	}

	return(NULL);
}
#	define vscprintf _vscprintf
#else
#	define _aligned_malloc(size, align) memalign((align), (size))
#	define _aligned_free(ptr) free(ptr)
#	define __forceinline inline


inline int vscprintf(const char * format, va_list pargs)
{
	int retval;
	va_list argcopy;
	va_copy(argcopy, pargs);
	retval = vsnprintf(NULL, 0, format, argcopy);
	va_end(argcopy);
	return(retval);
}

#endif


#ifndef max
template<typename T>
T max(const T &a, const T &b)
{
	return(a > b ? a : b);
}
// #define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
template<typename T>
T min(const T &a, const T &b)
{
	return(a < b ? a : b);
}
//#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#if defined(_WIN32)
#define dbg_break _asm { int 3 }
#else
#define dbg_break asm("int $3");
#endif

#if !defined(_WIN32)
#define Sleep(x) usleep(x * 1000)

#define _beginthread(ptr, stackSize, arg) pthread_t ptr__thr; pthread_create(&ptr__thr, NULL, [](void*a)->void*{ptr(a); return(NULL);}, arg); pthread_detach(ptr__thr)

#endif

#define DECLARE_CLASS(cls, basecls)		\
	typedef cls ThisClass;				\
	typedef basecls BaseClass;

#define DECLARE_CLASS_NOBASE(cls)		\
	typedef cls ThisClass;

#define mem_delete(obj) if(obj){delete obj;obj=0;}
#define mem_delete_a(obj) if(obj){delete[] obj;obj=0;}
#define mem_release(obj) if(obj){(obj)->Release(); (obj) = 0;}
#define add_ref(obj) if(obj){(obj)->AddRef();}
#define mem_free(a) free(a)
#define mem_alloc(a) malloc(a)

#if defined(_MSC_VER) && (_MSC_VER < 1900)
#	define alignof __alignof
#endif


#if defined(_MSC_VER)
#	define XALIGNED(type, x) __declspec(align(x)) type
#	define XDEPRECATED __declspec(deprecated) 
#	define XMETHODCALLTYPE __stdcall
#	define XINLINE __forceinline
#elif defined(__GNUC__)
#	define XALIGNED(type, x) type __attribute__ ((aligned(x)))
#	define XDEPRECATED __attribute__((deprecated)) 
//#	define XMETHODCALLTYPE __attribute__((stdcall))
#	define XMETHODCALLTYPE
#	define XINLINE inline __attribute__((always_inline))
#else
#	error "unsupported compiler"
#endif

class IXUnknown
{
public:
	virtual void XMETHODCALLTYPE AddRef() = 0;

	virtual void XMETHODCALLTYPE Release() = 0;

	virtual UINT XMETHODCALLTYPE getVersion() = 0;

	virtual void XMETHODCALLTYPE getInternalData(const XGUID *pGUID, void **ppOut) = 0;

protected:
	virtual ~IXUnknown() = default;
};

#ifdef __cplusplus
#define IXUNKNOWN_IMPLEMENTATION(name, ...)                                              \
template <class T>                                                                       \
class name: public __VA_ARGS__ T                                                         \
{                                                                                        \
public:                                                                                  \
	void XMETHODCALLTYPE AddRef() override                                               \
	{                                                                                    \
		++m_uRefCount;                                                                   \
	}                                                                                    \
	void XMETHODCALLTYPE Release() override                                              \
	{                                                                                    \
		if(!--m_uRefCount)                                                               \
		{                                                                                \
			FinalRelease();                                                              \
		}                                                                                \
	}                                                                                    \
	                                                                                     \
	UINT XMETHODCALLTYPE getVersion() override                                           \
	{                                                                                    \
		return(0);                                                                       \
	}                                                                                    \
	                                                                                     \
	void XMETHODCALLTYPE getInternalData(const XGUID *pGUID, void **ppOut) override      \
	{                                                                                    \
		*ppOut = NULL;                                                                   \
	}                                                                                    \
protected:                                                                               \
	name<T>()                                                                            \
	{                                                                                    \
		m_uRefCount.store(1);                                                            \
	}                                                                                    \
private:                                                                                 \
	std::atomic_uint m_uRefCount;                                                        \
	virtual void XMETHODCALLTYPE FinalRelease(){delete this;}                            \
}

IXUNKNOWN_IMPLEMENTATION(IXUnknownImplementation);
IXUNKNOWN_IMPLEMENTATION(IXUnknownVirtualImplementation, virtual);

#define XIMPLEMENT_VERSION(version)                                                      \
virtual UINT XMETHODCALLTYPE getVersion() override                                       \
{                                                                                        \
	return(version);                                                                     \
}
#endif

inline const char* basename(const char *str)
{
	const char * pos = str;
	while(*str)
	{
		if(*str == '/' || *str == '\\')
		{
			pos = str+1;
		}
		++str;
	}
	return(pos);
}

inline const char* dirname(char *str)
{
	char * pos = str, *ret = str;
	while(*str)
	{
		if((*str == '/' || *str == '\\') && *(str + 1))
		{
			pos = str + 1;
		}
		++str;
	}
	*pos = 0;
	return(ret);
}

inline const char* canonize_path(char *str)
{
	char * ret = str;
	while(*str)
	{
		if(*str == '\\')
		{
			*str = '/';
		}
		++str;
	}
	return(ret);
}

inline const char* strip_prefix(const char *str, const char *pref)
{
	size_t len = strlen(pref);
	if(!memcmp(str, pref, len))
	{
		return(str + len);
	}
	return(str);
}

inline int fstrcmp(const char *str1, const char *str2)
{
	return(str1 == str2 ? 0 : strcmp(str1, str2));
}

inline int parse_str(char *str, char **ppOut, int iMaxSize, char delim=',')
{
	//" val ; qwe;asd "
	int c = 0;
	while(*str && (c < iMaxSize || !ppOut))
	{
		while(*str && (*str == delim || isspace(*str))){++str;}
		if(ppOut)
		{
			ppOut[c] = str;
		}
		++c;

		while(*str && *str != delim/* && !isspace(*str)*/){++str;}
		if(ppOut)
		{
			char *tmp = str;
			--tmp;
			while(isspace(*tmp))
			{
				--tmp;
			}
			++tmp;
			if(tmp != str)
			{
				*tmp = 0;
			}
		}
		if(!*str) break;
		if(ppOut)
		{
			*str = 0;
		}
		++str;
		//while(*str && (*str == delim || isspace(*str))){++str;}
	}
	return(c);
}

template<typename T>
T* MovePtr(T *ptr, size_t uOffset)
{
	return((T*)(((byte*)ptr) + uOffset));
}

#define MOVE_PTR(ptr, offset) ptr = MovePtr(ptr, offset)

#endif
