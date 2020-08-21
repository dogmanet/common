// Copyright (c) 2016 Yakov Litvitskiy <thedsi100@gmail.com>
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef __EnumReflection_H
#define __EnumReflection_H

#include "array.h"

//-------------------------------- Public Interface --------------------------------

// Declare an enumeration inside a class
#define XENUM_CLS(enumName, ...) XENUM_DETAIL_MAKE(class, enumName, NULL, __VA_ARGS__)

// Declare an enumeration inside a namespace
#define XENUM(enumName, ...) XENUM_DETAIL_MAKE(namespace, enumName, NULL, __VA_ARGS__)

#ifdef __cplusplus

// Provides access to information about an enum declared with XENUM or XENUM_NS
class EnumReflector
{
public:
    // Returns a reference to EnumReflector object which can be used
    // to retrieve information about the enumeration declared with XENUM or XENUM_NS
    template<typename EnumType>
    static const EnumReflector& Get(EnumType val = EnumType());

    // Represents an enumerator (value) of an enumeration
    class Enumerator
    {
    public:
        // Returns enumerator name

		const char* getName() const
		{
			return(m_reflector.m_pData->aValues[m_index].szShortName);
		}

		const char* getFullName() const
		{
			return(m_reflector.m_pData->aValues[m_index].szName);
		}

		// Returns enumerator value
		int getValue() const
		{
			return(m_reflector.m_pData->aValues[m_index].iValue);
		}

        // Returns enumerator index
		int getIndex() const
		{
			return(m_index);
		}

        // Returns parent reflector object
		const EnumReflector& getReflector() const
		{
			return(m_reflector);
		}

        // Check if this is an valid Enumerator
		bool isValid() const
		{
			return(m_index < m_reflector.getCount());
		}
		operator bool() const
		{
			return(isValid());
		}

        // Check if two objects are the same
		bool operator!=(const Enumerator &other) const
		{
			return(m_index != other.m_index);
		}

        // Moves on to the next Enumerator in enum
		Enumerator& operator++()
		{
			++m_index;
			return(*this);
		}

        // Provided for compatibility with range-based for construct
		const Enumerator& operator*() const
		{
			return(*this);
		}

    private:
        friend class EnumReflector;
		Enumerator(const EnumReflector &reflector, int iIndex):
			m_reflector(reflector),
			m_index(iIndex)
		{
		}
        const EnumReflector &m_reflector;
        int m_index;
    };

    // Returns Enumerator count
	int getCount() const;

    // Returns an Enumerator with specified name or invalid Enumerator if not found
    Enumerator find(const char *szName) const;

    // Returns an Enumerator with specified value or invalid Enumerator if not found
    Enumerator find(int value) const;

    // Returns the enumeration name
	const char* getName() const;

    // Returns Enumerator at specified index
    Enumerator at(int index) const;
    Enumerator operator[](int index) const;
    
    // In some cases Enumerators can be used as iterators. The following functions
    // are provided e.g. for compatibility with range-based for construct:

    // Returns the first Enumerator
    Enumerator begin() const;

    // Returns an invalid Enumerator
    Enumerator end() const;

public:
    // Constructor. Used internally by XENUM and XENUM_CLS
	EnumReflector(const int *pVals, int iCount, const char *szName, const char *szBody, const char *szStripPrefix);
    EnumReflector(EnumReflector &&other);
    ~EnumReflector();
private:
	struct Private
	{
		struct Enumerator
		{
			char *szName;
			const char *szShortName;
			int iValue;
		};
		Array<Enumerator> aValues;
		const char *szEnumName;
	};
    Private *m_pData;

	bool isIdentChar(char c);
};

//----------------------------- Implementation Details -----------------------------

#define XENUM_DETAIL_SPEC_namespace \
    extern "C"{/* Protection from being used inside a class body */} \
    inline
#define XENUM_DETAIL_SPEC_class friend
#define XENUM_DETAIL_STR(x) #x
#define XENUM_DETAIL_MAKE(spec, enumName, stripPrefix, ...)                                     \
    enum enumName: int                                                                          \
    {                                                                                           \
        __VA_ARGS__                                                                             \
    };                                                                                          \
    XENUM_DETAIL_SPEC_##spec const ::EnumReflector& _enum_detail_reflector_(enumName)           \
    {                                                                                           \
        static const ::EnumReflector _reflector([]{                                             \
            static int _detail_sval;                                                            \
            _detail_sval = 0;                                                                   \
            struct _detail_val_t                                                                \
            {                                                                                   \
                _detail_val_t(const _detail_val_t& rhs): _val(rhs){_detail_sval = _val + 1;}    \
                _detail_val_t(int val): _val(val){_detail_sval = _val + 1;}                     \
                _detail_val_t(): _val(_detail_sval){_detail_sval = _val + 1;}                   \
                                                                                                \
                _detail_val_t& operator=(const _detail_val_t&){return(*this);}                  \
                _detail_val_t& operator=(int){return(*this);}                                   \
                operator int() const{return(_val);}                                             \
                int _val;                                                                       \
            } __VA_ARGS__;                                                                      \
            const int _detail_vals[] = {__VA_ARGS__};                                           \
            return(::EnumReflector(_detail_vals, sizeof(_detail_vals) / sizeof(int),            \
                    #enumName, XENUM_DETAIL_STR((__VA_ARGS__)), stripPrefix));                  \
        }());                                                                                   \
        return(_reflector);                                                                     \
    }

// EnumReflector

inline EnumReflector::EnumReflector(EnumReflector &&other):
	m_pData(other.m_pData)
{
	other.m_pData = NULL;
}

template<typename EnumType>
inline const EnumReflector& EnumReflector::Get(EnumType val)
{
	return(_enum_detail_reflector_(val));
}

inline EnumReflector::Enumerator EnumReflector::at(int index) const
{
    return(Enumerator(*this, index));
}

inline EnumReflector::Enumerator EnumReflector::operator[](int index) const
{
    return(at(index));
}

inline EnumReflector::Enumerator EnumReflector::begin() const
{
    return(at(0));
}

inline EnumReflector::Enumerator EnumReflector::end() const
{
    return(at(getCount()));
}

inline bool EnumReflector::isIdentChar(char c)
{
	return((c >= 'A' && c <= 'Z') ||
		(c >= 'a' && c <= 'z') ||
		(c >= '0' && c <= '9') ||
		(c == '_'));
}

inline EnumReflector::EnumReflector(const int *pVals, int iCount, const char *szName, const char *szBody, const char *szStripPrefix):
	m_pData(new Private)
{
	m_pData->szEnumName = szName;
	m_pData->aValues.resize(iCount);
	enum states
	{
		state_start, // Before identifier
		state_ident, // In identifier
		state_skip, // Looking for separator comma
	} state = state_start;
	assert(*szBody == '(');
	++szBody;
	const char* ident_start = nullptr;
	int value_index = 0;
	int level = 0;
	for(bool isRunning = true; isRunning;)
	{
		assert(*szBody);
		switch(state)
		{
		case state_start:
			if(isIdentChar(*szBody))
			{
				state = state_ident;
				ident_start = szBody;
			}
			++szBody;
			break;
		case state_ident:
			if(!isIdentChar(*szBody))
			{
				state = state_skip;
				assert(value_index < iCount);
				char *tmp = (char*)malloc(sizeof(char) * (szBody - ident_start + 1));
				memcpy(tmp, ident_start, szBody - ident_start);
				tmp[szBody - ident_start] = 0;
				m_pData->aValues[value_index].szName = tmp;
				m_pData->aValues[value_index].iValue = pVals[value_index];
				++value_index;
			}
			else
			{
				++szBody;
			}
			break;
		case state_skip:
			if(*szBody == '(')
			{
				++level;
			}
			else if(*szBody == ')')
			{
				if(level == 0)
				{
					assert(value_index == iCount);
					isRunning = false;
					break;
				}
				--level;
			}
			else if(level == 0 && *szBody == ',')
			{
				state = state_start;
			}
			++szBody;
		}
	}

	// szShortName
	int iCommonPrefixLength = 0;
	if(szStripPrefix)
	{
		iCommonPrefixLength = (int)strlen(szStripPrefix);
	}
	else
	{
		for(UINT i = 1, l = m_pData->aValues.size(); i < l; ++i)
		{
			int iLen0 = (int)strlen(m_pData->aValues[i - 1].szName);
			int iLen1 = (int)strlen(m_pData->aValues[i].szName);
			int iLen = min(iLen0, iLen1);
			if(i > 1)
			{
				iLen = min(iLen, iCommonPrefixLength);
			}

			int j = 0;
			for(; j < iLen; ++j)
			{
				if(m_pData->aValues[i - 1].szName[j] != m_pData->aValues[i].szName[j])
				{
					break;
				}
			}
			iCommonPrefixLength = j;

			if(!iCommonPrefixLength)
			{
				break;
			}
			// m_pData->aValues[value_index].sName
		}
	}

	if(iCommonPrefixLength)
	{
		for(UINT i = 0, l = m_pData->aValues.size(); i < l; ++i)
		{
			if((int)strlen(m_pData->aValues[i].szName) > iCommonPrefixLength)
			{
				m_pData->aValues[i].szShortName = m_pData->aValues[i].szName + iCommonPrefixLength;
			}
			else
			{
				m_pData->aValues[i].szShortName = m_pData->aValues[i].szName;
			}
		}
	}
}

inline EnumReflector::~EnumReflector()
{
	if(m_pData)
	{
		for(int i = 0, l = (int)m_pData->aValues.size(); i < l; ++i)
		{
			free(m_pData->aValues[i].szName);
		}
	}
	mem_delete(m_pData);
}

inline int EnumReflector::getCount() const
{
	return((int)m_pData->aValues.size());
}

inline EnumReflector::Enumerator EnumReflector::find(const char *szName) const
{
	for(int i = 0, l = (int)m_pData->aValues.size(); i < l; ++i)
	{
		if(!fstrcmp(m_pData->aValues[i].szName, szName) || (m_pData->aValues[i].szName != m_pData->aValues[i].szShortName && !fstrcmp(m_pData->aValues[i].szShortName, szName)))
		{
			return(at(i));
		}
	}
	return(end());
}

inline EnumReflector::Enumerator EnumReflector::find(int value) const
{
	for(int i = 0; i < (int)m_pData->aValues.size(); ++i)
	{
		if(m_pData->aValues[i].iValue == value)
		{
			return(at(i));
		}
	}
	return(end());
}

inline const char* EnumReflector::getName() const
{
	return(m_pData->szEnumName);
}

#else

#define XENUM_DETAIL_MAKE(spec, enumName, stripPrefix, ...)   \
    enum enumName: int                                        \
	{                                                         \
		__VA_ARGS__                                           \
	}
#endif

#endif
