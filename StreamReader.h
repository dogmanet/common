#ifndef __COMMON_STREAMREADER_H
#define __COMMON_STREAMREADER_H

#include <common/types.h>

class StreamReader
{
public:
	StreamReader(const byte *pData, size_t uSize):
		m_pBuf(pData),
		m_uBufSize(uSize)
	{
	}

	StreamReader(const char *szData):
		m_pBuf((const byte*)szData),
		m_uBufSize(strlen(szData))
	{
	}

	template<typename T>
	size_t read(T *pOut, UINT uCount = 1)
	{
		return(readBytes((byte*)pOut, sizeof(T) * uCount));
	}

	void movR(int i = 0)
	{
		m_uBufPos += i;

		if(m_uBufPos >= m_uBufSize)
		{
			m_uBufPos = m_uBufSize;
		}
	}
	void movL(int i = 0)
	{
		if(m_uBufPos < i)
		{
			m_uBufPos = 0;
		}
		else
		{
			m_uBufPos -= i;
		}
	}
	void movE()
	{
		m_uBufPos = m_uBufSize;
	}
	void movB()
	{
		m_uBufPos = 0;
	}
	bool isEnd()
	{
		return(m_uBufPos >= m_uBufSize);
	}
	bool isEmpty()
	{
		return(m_uBufSize == 0);
	}

	void setCursor(size_t pos = 0)
	{
		if(m_uBufSize < pos)
		{
			pos = m_uBufSize;
		}
		m_uBufPos = pos;
	}

	void readString(char *pBuf, UINT iBufSize)
	{
		if(iBufSize > m_uBufSize - m_uBufPos)
		{
			iBufSize = (UINT)(m_uBufSize - m_uBufPos);
		}

		strncpy(pBuf, (const char*)&(m_pBuf[m_uBufPos]), iBufSize);
		pBuf[iBufSize - 1] = 0;

		while(!isEnd() && readInt8())
		{
		}
	}
	UINT readChar()
	{
		UINT ch = 0;
		BYTE byte1 = readInt8();
		if(byte1 < 0x80)
		{
			ch = byte1;
		}
		else if(byte1 >= 0xC2 && byte1 < 0xE0)
		{
			BYTE byte2 = readInt8();
			ch = (((byte1 & 0x1F) << 6) + (byte2 & 0x3F));
		}
		else if(byte1 >= 0xE0 && byte1 < 0xF0)
		{
			BYTE byte2 = readInt8();
			BYTE byte3 = readInt8();
			ch = (((byte1 & 0xFF) << 12) + ((byte2 & 0x3F) << 6) + (byte3 & 0x3F));
		}
		else if(byte1 >= 0xF0 && byte1 < 0xF5)
		{
			BYTE byte2 = readInt8();
			BYTE byte3 = readInt8();
			BYTE byte4 = readInt8();
			UINT codepoint = ((byte1 & 0x07) << 18) + ((byte2 & 0x3F) << 12) + ((byte3 & 0x3F) << 6) + (byte4 & 0x3F);
			codepoint -= 0x10000;
			ch = (codepoint >> 10) + 0xD800, (codepoint & 0x3FF) + 0xDC00;
		}
		return(ch);
	}
	size_t readBytes(byte *pBuf, size_t uBufLen)
	{
		size_t uZeroBuf = 0;
		if(uBufLen > m_uBufSize - m_uBufPos)
		{
			uZeroBuf = uBufLen - (m_uBufSize - m_uBufPos);
			uBufLen -= uZeroBuf;
		}

		if(uBufLen)
		{
			memcpy(pBuf, &(m_pBuf[m_uBufPos]), uBufLen);
			m_uBufPos += uBufLen;
		}
		if(uZeroBuf)
		{
			memset(pBuf + uBufLen, 0, uZeroBuf);
		}

		return(uBufLen);
	}
	int32_t readInt32()
	{
		int32_t out;
		read(&out);
		return(out);
	}
	uint32_t readUInt32()
	{
		uint32_t out;
		read(&out);
		return(out);
	}
	int64_t readInt64()
	{
		int64_t out;
		read(&out);
		return(out);
	}
	uint64_t readUInt64()
	{
		uint64_t out;
		read(&out);
		return(out);
	}
	int16_t readInt16()
	{
		int16_t out;
		read(&out);
		return(out);
	}
	uint16_t readUInt16()
	{
		uint16_t out;
		read(&out);
		return(out);
	}
	int8_t readInt8()
	{
		int8_t out;
		read(&out);
		return(out);
	}
	byte readUInt8()
	{
		byte out;
		read(&out);
		return(out);
	}
	float readFloat32()
	{
		float out;
		read(&out);
		return(out);
	}

	size_t getCursor()
	{
		return(m_uBufPos);
	}

	const byte* getPointer()
	{
		return(m_pBuf + m_uBufPos);
	}

	size_t getSize()
	{
		return(m_uBufSize);
	}


private:
	size_t m_uBufPos = 0;
	size_t m_uBufSize = 0;
	UINT m_uPosBit = 0;
	const byte *m_pBuf = 0;
};

#endif
