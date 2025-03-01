
/******************************************************
Copyright © Vitaliy Buturlin, Evgeny Danilovich, 2017
See the license in LICENSE
******************************************************/

#ifndef __STRING_H
#define __STRING_H

#define _NO_GTK

#include <stdint.h>
#include <cstring>
#include <cwchar>
#include "types.h"

#ifdef _WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	define snprintf _snprintf
#endif

#include "MB2WC.h"

#if defined(_LINUX) || defined(_MAC)
#	include <wchar.h>
#	include <wctype.h>
#	include <stdlib.h>
#endif

#pragma warning(push)
#pragma warning(disable:4996)
#pragma warning(disable:4522)

static char* xstrcat(char *dest, const char *source)
{
	return(strcat(dest, source));
}

static wchar_t* xstrcat(wchar_t *dest, const wchar_t *source)
{
	return(wcscat(dest, source));
}

static void* xmemmove(char *dest, const char *source, size_t len)
{
	return(memmove(dest, source, len));
}

static void* xmemmove(wchar_t *dest, const wchar_t *source, size_t len)
{
	return(wmemmove(dest, source, len));
}

static char* xstrcpy(char *dest, const char *source)
{
	return(strcpy(dest, source));
}

static wchar_t* xstrcpy(wchar_t *dest, const wchar_t *source)
{
	return(wcscpy(dest, source));
}

static char* xstrncpy(char *dest, const char *source, size_t len)
{
	return(strncpy(dest, source, len));
}

static wchar_t* xstrncpy(wchar_t *dest, const wchar_t *source, size_t len)
{
	return(wcsncpy(dest, source, len));
}

static size_t xstrlen(const char *str)
{
	return(strlen(str));
}

static size_t xstrlen(const wchar_t *str)
{
	return(wcslen(str));
}

static int xstrcmp(const char *left, const char *right)
{
	return(strcmp(left, right));
}

static int xstrcmp(const wchar_t *left, const wchar_t *right)
{
	return(wcscmp(left, right));
}

static char* xstrstr(char *str, const char *substr)
{
	return(strstr(str, substr));
}

static wchar_t* xstrstr(wchar_t *str, const wchar_t *substr)
{
	return(wcsstr(str, substr));
}

static const char* xstrstr(const char *str, const char *substr)
{
	return(strstr(str, substr));
}

static const wchar_t* xstrstr(const wchar_t *str, const wchar_t *substr)
{
	return(wcsstr(str, substr));
}

static int xisspace(char sym)
{
	return(isspace((unsigned char)sym));
}

static int xisspace(wchar_t sym)
{
	return(iswspace(sym));
}

static const char* xstrchr(const char *str, char sym)
{
	return(strchr(str, sym));
}

static const wchar_t* xstrchr(const wchar_t *str, wchar_t sym)
{
	return(wcschr(str, sym));
}

static void* xmemcpy(char *dest, const char *source, size_t len)
{
	return(memcpy(dest, source, len));
}

static const wchar_t* xmemcpy(wchar_t *dest, const wchar_t *source, size_t len)
{
	return(wmemcpy(dest, source, len));
}

static int xstricmp(const char *left, const char *right)
{
	return(strcasecmp(left, right));
}

static int xstricmp(const wchar_t *left, const char *right)
{
	return(wcscasecmp(left, CMB2WC(right)));
}

template <typename... T>
static int xsprintf(char *dest, const char* format, T... args)
{
	return(sprintf(dest, format, args...));
}

template <typename... T>
static int xsprintf(wchar_t *dest, const char* format, T... args)
{
	return(swprintf(dest, CMB2WC(format), args...));
}

template <typename... T>
static int xsscanf(const char *source, const char* format, T... args)
{
	return(sscanf(source, format, args...));
}

template <typename... T>
static int xsscanf(const wchar_t *source, const char* format, T... args)
{
	return(swscanf(source, CMB2WC(format), args...));
}

class String;
class StringW;

template <typename T, typename Derived>
class StringBase
{
public:
	StringBase()
	{
		m_data.stack.size = 0;
		m_data.stack.szStr[0] = 0;
	}

	StringBase(const T *str)
	{
		size_t len = xstrlen(str);
		if(len + 1 <= getStackSize())
		{
			xstrcpy(m_data.stack.szStr, str);
			m_data.stack.size = (byte)len;
		}
		else
		{
			m_isStack = false;
			m_data.heap.capacity = calcCapacity(len);
			m_data.heap.size = len;
			m_data.heap.szStr = new T[capacity()];
			xstrcpy(m_data.heap.szStr, str);
		}
	}

	StringBase(T sym)
	{
		m_data.stack.szStr[0] = sym;
		m_data.stack.szStr[1] = 0;

		m_data.stack.size = 1;
	}

	StringBase(int num)
	{
		m_data.stack.size = (byte)xsprintf(m_data.stack.szStr, "%d", num);
	}

	StringBase(int64_t num)
	{
		initFormat("%lld", num);
	}

	StringBase(uint64_t num)
	{
		initFormat("%llu", num);
	}

	StringBase(UINT num)
	{
		m_data.stack.size = (byte)xsprintf(m_data.stack.szStr, "%u", num);
	}

	StringBase(double num)
	{
		initFormat("%g", num);
	}

	StringBase(float num)
	{
		initFormat("%g", num);
	}

	explicit StringBase(bool bf)
	{
		m_data.stack.size = (byte)xsprintf(m_data.stack.szStr, "%s", (bf ? "true" : "false"));
	}

	StringBase(const Derived &str)
	{
		*this = str;
	}

	StringBase(Derived &&other)
	{
		*this = std::move(other);
	}

	~StringBase()
	{
		release();
	}

	void release()
	{
		if(!m_isStack)
		{
			mem_delete_a(m_data.heap.szStr);
			m_data.heap.capacity = 0;
			m_isStack = true;
		}
		m_data.stack.szStr[0] = 0;
		m_data.stack.size = 0;
	}

	Derived operator+(const Derived &str) const
	{
		return(*this + str.c_str());
	}

	Derived operator+(const T *str) const
	{
		Derived result;
		size_t size = 0;
		const T *szLeft = NULL;
		const T *szRight = NULL;

		size = length() + xstrlen(str);

		if(size + 1 > getStackSize())
		{
			result.m_isStack = false;
			result.m_data.heap.capacity = calcCapacity(size);
			result.m_data.heap.size = size;
			result.m_data.heap.szStr = new T[result.capacity()];

			xstrcpy(result.m_data.heap.szStr, c_str());
			xstrcat(result.m_data.heap.szStr, str);

			return(result);
		}

		result.m_data.stack.size = (byte)size;
		xstrcpy(result.m_data.stack.szStr, c_str());
		xstrcat(result.m_data.stack.szStr, str);

		return(result);
	}

	Derived operator+(T sym) const
	{
		return(*this + Derived(sym));
	}

	Derived operator+(int num) const
	{
		return(*this + Derived(num));
	}

	Derived operator+(int64_t num) const
	{
		return(*this + Derived(num));
	}

	Derived operator+(uint64_t num) const
	{
		return(*this + Derived(num));
	}

	Derived operator+(UINT num) const
	{
		return(*this + Derived(num));
	}

	Derived operator+(double num) const
	{
		return(*this + Derived(num));
	}

	Derived operator+(float num) const
	{
		return(*this + Derived(num));
	}

	Derived operator+(bool bf) const
	{
		return(*this + Derived(bf));
	}

	Derived &operator=(const Derived &str)
	{
		*this = str.c_str();

		return(*(Derived*)this);
	}

	Derived &operator=(Derived &&other)
	{
		if(this != &other)
		{
			std::swap(m_data, other.m_data);
			std::swap(m_isStack, other.m_isStack);
		}

		return(*(Derived*)this);
	}

	Derived &operator=(const T *str)
	{
		if(c_str() != str)
		{
			T *dest = NULL;
			size_t len = xstrlen(str);

			if((m_isStack && len + 1 > getStackSize()) ||
				(capacity() <= len))
			{
				release();

				m_isStack = false;
				m_data.heap.capacity = calcCapacity(len);
				m_data.heap.size = len;
				m_data.heap.szStr = new T[capacity()];
				dest = m_data.heap.szStr;
			}
			else if(!m_isStack && capacity() >= len + 1)
			{
				m_data.heap.size = len;
				dest = m_data.heap.szStr;
			}
			else
			{
				m_data.stack.size = (byte)len;
				dest = m_data.stack.szStr;
			}

			xstrcpy(dest, str);
		}

		return(*(Derived*)this);
	}

	Derived &operator=(T sym)
	{
		*this = Derived(sym);

		return(*(Derived*)this);
	}

	Derived &operator=(int num)
	{
		*this = Derived(num);

		return(*(Derived*)this);
	}

	Derived &operator=(int64_t num)
	{
		*this = Derived(num);

		return(*(Derived*)this);
	}

	Derived &operator=(uint64_t num)
	{
		*this = Derived(num);

		return(*(Derived*)this);
	}

	Derived &operator=(UINT num)
	{
		*this = Derived(num);

		return(*(Derived*)this);
	}

	Derived &operator=(double num)
	{
		*this = Derived(num);

		return(*(Derived*)this);
	}

	Derived &operator=(float num)
	{
		*this = Derived(num);

		return(*(Derived*)this);
	}

	Derived &operator=(bool bf)
	{
		*this = Derived(bf);

		return(*(Derived*)this);
	}

	Derived &operator+=(const Derived &str)
	{
		return(*this += str.c_str());
	}

	Derived &operator+=(const T *str)
	{
		bool selfPlus = c_str() == str;
		size_t len = xstrlen(str) + length();

		if(m_isStack && len + 1 <= getStackSize())
		{
			if(selfPlus)
			{
				xmemcpy(m_data.stack.szStr + length(), m_data.stack.szStr, length());
				m_data.stack.szStr[len] = 0;
			}
			else
			{
				xstrcat(m_data.stack.szStr, str);
			}

			m_data.stack.size = (byte)len;

			return(*(Derived*)this);
		}
		else if(!m_isStack && m_data.heap.capacity >= len + 1)
		{
			if(selfPlus)
			{
				xmemcpy(m_data.heap.szStr + length(), m_data.heap.szStr, length());
				m_data.stack.szStr[len] = 0;
			}
			else
			{
				xstrcat(m_data.heap.szStr, str);
			}

			m_data.heap.size = len;

			return(*(Derived*)this);
		}

		size_t capacity = calcCapacity(len);
		T *szBuff = new T[capacity];
		xsprintf(szBuff, "%s%s", c_str(), str);

		release();

		m_isStack = false;
		m_data.heap.szStr = szBuff;
		m_data.heap.capacity = capacity;
		m_data.heap.size = len;

		return(*(Derived*)this);
	}

	Derived &operator+=(T sym)
	{
		return(*this += Derived(sym));
	}

	Derived &operator+=(int num)
	{
		return(*this += Derived(num));
	}

	Derived &operator+=(int64_t num)
	{
		return(*this += Derived(num));
	}

	Derived &operator+=(uint64_t num)
	{
		return(*this += Derived(num));
	}

	Derived &operator+=(UINT num)
	{
		return(*this += Derived(num));
	}

	Derived &operator+=(double num)
	{
		return(*this += Derived(num));
	}

	Derived &operator+=(float num)
	{
		return(*this += Derived(num));
	}

	Derived &operator+=(bool bf)
	{
		return(*this += Derived(bf));
	}

	Derived operator-(const Derived &str) const
	{
		return(*this - str.c_str());
	}

	Derived operator-(const T *str) const
	{
		if(length() > 0)
		{
			const T *pos = xstrstr(c_str(), str);
			T *szBuf = NULL;

			if(!pos)
			{
				return(*(Derived*)this);
			}

			Derived result;
			size_t strlen = xstrlen(str);
			size_t len = length() - strlen;
			size_t count = pos - c_str();

			if(len + 1 <= getStackSize())
			{
				result.m_data.stack.size = (byte)len;
				szBuf = result.m_data.stack.szStr;

				return(result);
			}
			else
			{
				result.m_isStack = false;
				result.m_data.heap.capacity = calcCapacity(len);
				result.m_data.heap.size = len;
				result.m_data.heap.szStr = new T[result.capacity()];

				szBuf = result.m_data.heap.szStr;
			}


			xstrncpy(szBuf, c_str(), count);
			szBuf[count] = 0;
			xstrcat(szBuf, pos + strlen);

			return(result);
		}

		return(*(Derived*)this);
	}

	Derived operator-(T sym) const
	{
		return(*this - Derived(sym));
	}

	Derived operator-(int num) const
	{
		return(*this - Derived(num));
	}

	Derived operator-(int64_t num) const
	{
		return(*this - Derived(num));
	}

	Derived operator-(uint64_t num) const
	{
		return(*this - Derived(num));
	}

	Derived operator-(UINT num) const
	{
		return(*this - Derived(num));
	}

	Derived operator-(double num)const
	{
		return(*this - Derived(num));
	}

	Derived operator-(float num)const
	{
		return(*this - Derived(num));
	}

	Derived operator-(bool bf) const
	{
		return(*this - Derived(num));
	}

	Derived &operator-=(const Derived &str)
	{
		return(*this -= str.c_str());
	}

	Derived &operator-=(const T *str)
	{
		if(length() > 0)
		{
			size_t len = xstrlen(str);
			T *szData = m_isStack ? m_data.stack.szStr : m_data.heap.szStr;
			T *pos = xstrstr(szData, str);
			size_t size = length() - (pos - szData) - len;

			if(!pos)
			{
				return(*(Derived*)this);
			}

			if(m_isStack)
			{
				m_data.stack.size -= (byte)len;
			}
			else
			{
				m_data.heap.size -= len;
			}

			xmemmove(pos, pos + len, size + 1);
		}
		return(*(Derived*)this);
	}

	Derived &operator-=(T sym)
	{
		return(*this -= Derived(sym));
	}

	Derived &operator-=(int num)
	{
		return(*this -= Derived(num));
	}

	Derived &operator-=(int64_t num)
	{
		return(*this -= Derived(num));
	}

	Derived &operator-=(uint64_t num)
	{
		return(*this -= Derived(num));
	}

	Derived &operator-=(UINT num)
	{
		return(*this -= Derived(num));
	}

	Derived &operator-=(double num)
	{
		return(*this -= Derived(num));
	}

	Derived &operator-=(float num)
	{
		return(*this -= Derived(num));
	}

	Derived &operator-=(bool bf)
	{
		return(*this -= Derived(bf));
	}

	Derived operator/(const Derived &str) const
	{
		return(*this / str.c_str());
	}

	Derived operator/(const T *str) const
	{
		size_t len = length();
		Derived result;

		if(len > 0)
		{
			T *szData = (T*)_malloca(sizeof(T) * (len + 1));
			xstrcpy(szData, c_str());

			T *pos = xstrstr(szData, str);
			size_t delLen = xstrlen(str);
			size_t size = 0;

			while(pos)
			{
				size = len - (pos - szData) - delLen;
				len = len - delLen;
				xmemmove(pos, pos + delLen, size + 1);
				pos = xstrstr(szData, str);
			}

			result = szData;
			_freea(szData);
		}

		return(result);
	}

	Derived operator/(T sym) const
	{
		return(*this / Derived(sym));
	}

	Derived operator/(int num) const
	{
		return(*this / Derived(num));
	}

	Derived operator/(int64_t num) const
	{
		return(*this / Derived(num));
	}

	Derived operator/(uint64_t num) const
	{
		return(*this / Derived(num));
	}

	Derived operator/(UINT num) const
	{
		return(*this / Derived(num));
	}

	Derived operator/(double num) const
	{
		return(*this / Derived(num));
	}

	Derived operator/(float num) const
	{
		return(*this / Derived(num));
	}

	Derived operator/(bool bf) const
	{
		return(*this / Derived(bf));
	}

	Derived &operator/=(const Derived &str)
	{
		return(*this /= str.c_str());
	}

	Derived &operator/=(const T *str)
	{
		size_t len = length();

		if(len > 0)
		{
			T *szData = m_isStack ? m_data.stack.szStr : m_data.heap.szStr;

			T *pos = xstrstr(szData, str);
			size_t delLen = xstrlen(str);
			size_t size = 0;

			while(len > 0 && pos)
			{
				size = len - (pos - szData) - delLen;
				len = len - delLen;
				xmemmove(pos, pos + delLen, size + 1);
				pos = xstrstr(pos, str);
			}

			if(m_isStack)
			{
				m_data.stack.size = (byte)len;
			}
			else
			{
				m_data.heap.size = len;
			}
		}

		return(*(Derived*)this);
	}

	Derived &operator/=(T sym)
	{
		return(*this /= Derived(sym));
	}

	Derived &operator/=(int num)
	{
		return(*this /= Derived(num));
	}

	Derived &operator/=(int64_t num)
	{
		return(*this /= Derived(num));
	}

	Derived &operator/=(uint64_t num)
	{
		return(*this /= Derived(num));
	}

	Derived &operator/=(UINT num)
	{
		return(*this /= Derived(num));
	}

	Derived &operator/=(double num)
	{
		return(*this /= Derived(num));
	}

	Derived &operator/=(float num)
	{
		return(*this /= Derived(num));
	}

	Derived &operator/=(bool bf)
	{
		return(*this /= Derived(bf));
	}

	bool operator==(const Derived &str) const
	{
		if(this != &str)
		{
			if(length() == str.length())
			{
				return(xstrcmp(c_str(), str.c_str()) == 0);
			}
		}
		else
		{
			return(true);
		}

		return(false);
	}

	bool operator==(const T *str) const
	{
		if(c_str() != str)
		{
			if(length() == xstrlen(str))
			{
				return(xstrcmp(c_str(), str) == 0);
			}
		}
		else
		{
			return(true);
		}

		return(false);
	}

	bool operator==(T sym) const
	{
		return(*this == Derived(sym));
	}

	bool operator==(int num) const
	{
		return(*this == Derived(num));
	}

	bool operator==(int64_t num) const
	{
		return(*this == Derived(num));
	}

	bool operator==(uint64_t num) const
	{
		return(*this == Derived(num));
	}

	bool operator==(UINT num) const
	{
		return(*this == Derived(num));
	}

	bool operator==(double num) const
	{
		return(*this == Derived(num));
	}

	bool operator==(float num) const
	{
		return(*this == Derived(num));
	}

	bool operator==(bool bf) const
	{
		return(*this == Derived(bf));
	}

	bool operator!=(const Derived &str) const
	{
		return(!(*this == Derived(str)));
	}

	bool operator!=(const T *str) const
	{
		return(!(*this == Derived(str)));
	}

	bool operator!=(T sym) const
	{
		return(!(*this == Derived(sym)));
	}

	bool operator!=(int num) const
	{
		return(!(*this == Derived(num)));
	}

	bool operator!=(int64_t num) const
	{
		return(!(*this == Derived(num)));
	}

	bool operator!=(uint64_t num) const
	{
		return(!(*this == Derived(num)));
	}

	bool operator!=(UINT num) const
	{
		return(!(*this == Derived(num)));
	}

	bool operator!=(double num) const
	{
		return(!(*this == Derived(num)));
	}

	bool operator!=(float num) const
	{
		return(!(*this == Derived(num)));
	}

	bool operator!=(bool bf) const
	{
		return(!(*this == Derived(bf)));
	}

	T& operator[](size_t index)
	{
		assert(index <= length());
		return(m_isStack ? m_data.stack.szStr[index] : m_data.heap.szStr[index]);
	}

	const T& operator[](size_t index) const
	{
		assert(index <= length());
		return(m_isStack ? m_data.stack.szStr[index] : m_data.heap.szStr[index]);
	}

	size_t length() const
	{
		return(m_isStack ? m_data.stack.size : m_data.heap.size);
	}

	void insert(size_t pos, const T *data)
	{
		size_t dataLen = xstrlen(data);
		size_t newLen = length() + dataLen;

		T *szTmp = (T*)_malloca(sizeof(T) * (newLen + 1));
	
		xmemcpy(szTmp, c_str(), pos);
		szTmp[pos] = 0;
		xstrcat(szTmp, data);
		xstrcat(szTmp, c_str() + pos);

		*this = szTmp;

		_freea(szTmp);
	}

	void insert(size_t pos, const Derived &data)
	{
		insert(pos, data.c_str());
	}

	size_t find(T c, size_t pos = 0) const
	{
		const T *chr = xstrchr(c_str() + pos, c);

		return(chr ? (size_t)(chr - c_str()) : EOS);
	}

	size_t find(const T *str, size_t pos = 0) const
	{
		const T *it = xstrstr(c_str() + pos, str);

		return(it ? (size_t)(it - c_str()) : -1);
	}

	size_t find(const Derived& str, size_t pos = 0) const
	{
		return(find(str.c_str(), pos));
	}

	size_t find_last_of(T c, size_t pos = 0) const
	{
		const T *it = c_str() + pos;
		const T *res = NULL;

		while((it = xstrchr(it, c)))
		{
			res = it;
			++it;
		}

		return(res ? (size_t)(res - c_str()) : EOS);
	}

	size_t find_last_of(const T *str, size_t pos = 0) const
	{
		const T *it = c_str() + pos;
		const T *res = NULL;

		while((it = xstrstr(it, str)))
		{
			res = it;
			it += xstrlen(str);
		}

		return(res ? (size_t)(res - c_str()) : EOS);
	}

	size_t find_last_of(const Derived &str, size_t pos = 0) const
	{
		return(find_last_of(str.c_str(), pos));
	}

	size_t replace(const T *str, const T *replace, size_t pos)
	{
		const size_t res = find(str, pos);

		if(res != EOS)
		{
			size_t len = xstrlen(str);
			size_t replaceLen = xstrlen(replace);
			size_t newLen = length() - len + replaceLen;

			const T *data = c_str();
			T *buff = (T*)_malloca(sizeof(T) * (newLen + 1));

			xstrncpy(buff, data, res);
			buff[res] = 0;
			xstrcat(buff, replace);
			xstrcat(buff, data + res + len);

			*this = buff;

			_freea(buff);

			return(res);
		}

		return(EOS);
	}

	size_t replace(const Derived &str, const Derived &replace, size_t pos)
	{
		return(StringBase::replace(str.c_str(), replace.c_str(), pos));
	}

	size_t replaceAll(const T *str, const T *replace)
	{
		size_t len = xstrlen(str);
		size_t replaceLen = xstrlen(replace);
		size_t result = 0;
		size_t pos = 0;
		const T *it = c_str();

		while((it = xstrstr(it, str)))
		{
			++result;
			it += len;
		}

		if(result != 0)
		{
			if(len < replaceLen)
			{
				size_t size = result * (replaceLen - len);
				size_t capacity = calcCapacity(size);
				T *szStr = new T[capacity];
				xstrcpy(szStr, c_str());

				release();

				m_isStack = false;
				m_data.heap.capacity = capacity;
				m_data.heap.size = size;
				m_data.heap.szStr = szStr;
			}
		}

		do{
			pos = StringBase::replace(str, replace, pos);
		}
		while(pos != EOS);

		return(result);
	}

	size_t replaceAll(const Derived &str, const Derived &replace)
	{
		return(replaceAll(str.c_str(), replace.c_str()));
	}

	Derived substr(size_t pos, size_t len = EOS) const
	{
		if(pos >= length())
		{
			return(Derived());
		}

		if(len == 0)
		{
			return(Derived());
		}

		Derived result;
		const T *it = c_str() + pos;

		if(len == EOS || len > length() - pos)
		{
			len = length() - pos;
		}

		T *str = (T*)_malloca(sizeof(T) * (len + 1));

		xstrncpy(str, it, len);
		str[len] = 0;

		result = str;

		_freea(str);

		return(result);
	}

	size_t remove(size_t pos, size_t size)
	{
		size_t len = length();

		if(pos >= len || size == 0)
		{
			return(0);
		}

		if(size > len - pos)
		{
			size = len - pos;
		}

		T *str = m_isStack ? m_data.stack.szStr : m_data.heap.szStr;
		T *it = str + pos;
		size_t count = len - (it - str) - size;

		xmemmove(it, it + size, count + 1);

		if(m_isStack)
		{
			m_data.stack.size -= size;
		}
		else
		{
			m_data.heap.size -= size;
		}

		return(size);
	}

	Derived trim()
	{
		UINT pos = length();
		UINT len = 0;
		const T *ch = c_str();
		for(UINT i = 0, l = length(); i < l; ++i)
		{
			if(!xisspace(ch[i]))
			{
				if(i < pos)
				{
					pos = i;
				}
				len = i - pos + 1;
			}
		}
		return(substr(pos, len));
	}

	const T* c_str() const
	{
		return(m_isStack ? m_data.stack.szStr : m_data.heap.szStr);
	}

	void resize(size_t len)
	{
		if(len > capacity())
		{
			appendReserve(len - length());
		}

		if(m_isStack)
		{
			m_data.stack.size = (byte)len;
			m_data.stack.szStr[len] = 0;
		}
		else
		{
			m_data.heap.size = len;
			m_data.heap.szStr[len] = 0;
		}
	}

	void appendReserve(size_t len)
	{
		size_t newLen = len + length();
		if((m_isStack && newLen > getStackSize()) ||
			(!m_isStack && newLen > m_data.heap.capacity))
		{
			size_t size = length();
			T *cpy = (T*)_malloca(sizeof(T) * (size + 1));
			xstrcpy(cpy, c_str());

			release();

			m_isStack = false;
			m_data.heap.capacity = calcCapacity(newLen);
			m_data.heap.size = size;
			m_data.heap.szStr = new T[capacity()];

			xstrcpy(m_data.heap.szStr, cpy);

			_freea(cpy);
		}
	}

	int	toInt() const
	{
		int out = 0;
		xsscanf(c_str(), "%d", &out);
		return(out);
	}

	double toDouble() const
	{
		double out = 0;
		xsscanf(c_str(), "%g", &out);
		return(out);
	}

	float toFloat() const
	{
		float out = 0;
		xsscanf(c_str(), "%g", &out);
		return(out);
	}

	bool toBool() const
	{
		return(xstricmp(c_str(), "true") == 0 || toInt() == 1);
	}

	uint64_t toUInt64() const
	{
		uint64_t out = 0;
		xsscanf(c_str(), "%llu", &out);
		return(out);
	}

	UINT toUInt() const
	{
		UINT out = 0;
		xsscanf(c_str(), "%u", &out);
		return(out);
	}

	int64_t toInt64() const
	{
		int64_t out = 0;
		xsscanf(c_str(), "%lld", &out);
		return(out);
	}

	size_t capacity() const
	{
		return(m_isStack ? getStackSize() : m_data.heap.capacity);
	}

	bool operator<(const Derived &str) const
	{
		return(xstrcmp(c_str(), str.c_str()) < 0);
	}

	static const size_t EOS = -1;

private:
	template <typename Type>
	void initFormat(const char *szFormat, Type arg)
	{
		size_t len = snprintf(NULL, 0, szFormat, arg);
		T *szDest = NULL;

		if(len + 1 <= getStackSize())
		{
			szDest = m_data.stack.szStr;
			m_data.stack.size = (byte)len;
		}
		else
		{
			szDest = m_data.heap.szStr;
			appendReserve(len + 1);
			m_data.heap.size = len;
		}

		xsprintf(szDest, szFormat, arg);
	}

	int getStackSize() const
	{
		return(ARRAYSIZE(m_data.stack.szStr));
	}

	size_t calcCapacity(size_t len) const
	{
		return(len + (len % 2 ? (len - 1) / 2 : (len / 2)));
	}

	union
	{
#pragma pack(push, 1)
		struct Heap
		{
			size_t capacity;
			size_t size;
			T *szStr;
		} heap;

		struct
		{
			T szStr[(sizeof(Heap) - 1) / sizeof(T)];
			byte size;
		}stack;
#pragma pack(pop)
	} m_data;
	bool m_isStack = true;
};

class String: public StringBase<char, String>
{
public:

	using StringBase::operator=;
	using StringBase::operator+;
	using StringBase::operator+=;
	using StringBase::operator-=;
	using StringBase::operator-;
	using StringBase::operator/;
	using StringBase::operator/=;

	String():
		StringBase()
	{
	}

	String(const char *str):
		StringBase(str)
	{
	}

	String(char sym):
		StringBase(sym)
	{
	}

	String(int num):
		StringBase(num)
	{
	}

	String(int64_t num):
		StringBase(num)
	{

	}

	String(uint64_t num):
		StringBase(num)
	{
	}

	String(UINT num):
		StringBase(num)
	{
	}

	String(double num):
		StringBase(num)
	{
	}

	String(float num):
		StringBase(num)
	{
	}
	explicit String(bool bf):
		StringBase(bf)
	{
	}

	String(const String &str):
		StringBase(str)
	{
	}

	String& operator=(const String &str)
	{
		return(StringBase::operator=(str));
	}

	inline operator StringW() const;
};

class StringW: public StringBase<wchar_t, StringW>
{
public:

	using StringBase::operator=;
	using StringBase::operator+;
	using StringBase::operator+=;
	using StringBase::operator-=;
	using StringBase::operator-;
	using StringBase::operator/;
	using StringBase::operator/=;

	StringW():
		StringBase()
	{
	}

	StringW(const StringW &str):
		StringBase(str)
	{
	}

	StringW(const wchar_t *str):
		StringBase(str)
	{
	}

	StringW(wchar_t sym):
		StringBase(sym)
	{
	}

	StringW(int num):
		StringBase(num)
	{
	}

	StringW(int64_t num):
		StringBase(num)
	{

	}

	StringW(uint64_t num):
		StringBase(num)
	{
	}

	StringW(UINT num):
		StringBase(num)
	{
	}

	StringW(double num):
		StringBase(num)
	{
	}

	StringW(float num):
		StringBase(num)
	{
	}

	explicit StringW(bool bf):
		StringBase(bf)
	{
	}

	StringW& operator=(const StringW &str)
	{
		return(StringBase::operator=(str));
	}

	inline operator String() const;
};

inline String::operator StringW() const
{
	StringW result;
	size_t len = length() + 1;

	result.resize(len);

#if defined(_WIN32)
	MultiByteToWideChar(CP_UTF8, 0, c_str(), (int)len, &result[0], (int)len);
#else
	TODO("CHECK THIS");
	mbstowcs(&result[0], c_str(), len);
#endif

	return(result);
}

inline StringW::operator String() const
{
	String result;

#if defined(_WIN32)
	size_t size = WideCharToMultiByte(CP_UTF8, 0, c_str(), (int)(length() + 1), NULL, 0, NULL, NULL);
	result.resize(size);
	WideCharToMultiByte(CP_UTF8, 0, c_str(), (int)(length() + 1), &result[0], (int)size, NULL, NULL);
#else
	TODO("CHECK THIS");
	size_t len = length() + 1;
	result.resize(size);
	mbstowcs(&result[0], c_str(), len);
#endif

	return(result);
}

#pragma warning(pop)

#endif
