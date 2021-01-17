#ifndef __MB2WC__H
#define __MB2WC__H

template<int STATIC_MAX = 256>
class CMB2WCEx
{
public:
	CMB2WCEx(const char *szInput)
	{
		init(szInput, CP_UTF8);
	}
	CMB2WCEx(const char *szInput, UINT nCodePage)
	{
		init(szInput, nCodePage);
	}
	~CMB2WCEx()
	{
		delete[] m_psz;
	}

	operator const wchar_t*() const
	{
		return(m_psz ? m_psz : m_szBuffer);
	}

private:
	void init(const char *szInput, UINT nCodePage)
	{
		m_szBuffer[0] = 0;
		if(!szInput)
		{
			m_psz = NULL;
			return;
		}
		int nLengthA = (int)(strlen(szInput)) + 1;
		int nLengthW = nLengthA;

		bool isFailed = (0 == MultiByteToWideChar(nCodePage, 0, szInput, nLengthA, m_szBuffer, STATIC_MAX));
		if(isFailed && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			nLengthW = MultiByteToWideChar(nCodePage, 0, szInput, nLengthA, NULL, 0);
			m_psz = new wchar_t[nLengthW];
			isFailed = (0 == MultiByteToWideChar(nCodePage, 0, szInput, nLengthA, m_psz, nLengthW));
		}
	}

public:
	wchar_t *m_psz = NULL;
	wchar_t m_szBuffer[STATIC_MAX];

private:
	CMB2WCEx(const CMB2WCEx&) = delete;
	CMB2WCEx& operator=(const CMB2WCEx&) = delete;
};
typedef CMB2WCEx<> CMB2WC;

#endif
