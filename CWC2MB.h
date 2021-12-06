#ifndef __CWC2MB__H
#define __CWC2MB__H

template<int STATIC_MAX = 256>
class CWC2MBEx
{
public:
	CWC2MBEx(const wchar_t *szInput)
	{
		init(szInput, CP_UTF8);
	}
	CWC2MBEx(const wchar_t *szInput, UINT nCodePage)
	{
		init(szInput, nCodePage);
	}
	~CWC2MBEx()
	{
		delete[] m_psz;
	}

	operator const char*() const
	{
		return(m_psz ? m_psz : m_szBuffer);
	}

private:
	void init(const wchar_t *szInput, UINT nCodePage)
	{
		m_szBuffer[0] = 0;
		if(!szInput)
		{
			m_psz = NULL;
			return;
		}
		int nLengthA = (int)(wcslen(szInput)) + 1;
		int nLengthW = nLengthA;
		
		bool isFailed = (0 == WideCharToMultiByte(nCodePage, 0, szInput, nLengthA, m_szBuffer, STATIC_MAX, NULL, NULL));
		if(isFailed && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			nLengthW = WideCharToMultiByte(nCodePage, 0, szInput, nLengthA, NULL, 0, NULL, NULL);
			m_psz = new char[nLengthW];
			isFailed = (0 == WideCharToMultiByte(nCodePage, 0, szInput, nLengthA, m_psz, nLengthW, NULL, NULL));
		}
	}
private:
	char *m_psz = NULL;
	char m_szBuffer[STATIC_MAX];

	CWC2MBEx(const CWC2MBEx&) = delete;
	CWC2MBEx& operator=(const CWC2MBEx&) = delete;
};
typedef CWC2MBEx<> CWC2MB;

#endif
