#ifndef _FLOAT16_H_
#define _FLOAT16_H_

#include <memory>

class float16_t
{
public:
	float16_t()
	{
		m_siValue = 0;
	}
	float16_t(float value)
	{
		m_siValue = (((*(int*)&value) & 0x7fffffff) >> 13) - (0x38000000 >> 13);
		m_siValue |= (((*(int*)&value) & 0x80000000) >> 16);
	}

	float16_t(const float16_t &other)
	{
		m_siValue = other.m_siValue;
	}
	float16_t& operator=(const float16_t &other)
	{
		m_siValue = other.m_siValue;
		return(*this);
	}

	operator float() const
	{
		int fltInt32 = ((m_siValue & 0x8000) << 16);
		fltInt32 |= ((m_siValue & 0x7fff) << 13) + 0x38000000;

		float fRet;
		memcpy(&fRet, &fltInt32, sizeof(float));
		return(fRet);
	}
	
	float16_t operator-() const
	{
		return(-(float)*this);
	}

	float16_t operator+(const float16_t &other) const
	{
		return((float)*this + (float)other);
	}
	float16_t operator-(const float16_t &other) const
	{
		return((float)*this - (float)other);
	}
	float16_t operator*(const float16_t &other) const
	{
		return((float)*this * (float)other);
	}
	float16_t operator/(const float16_t &other) const
	{
		return((float)*this / (float)other);
	}

	float16_t& operator+=(const float16_t &other)
	{
		*this = *this + other;
		return(*this);
	}
	float16_t& operator-=(const float16_t &other)
	{
		*this = *this - other;
		return(*this);
	}
	float16_t& operator*=(const float16_t &other)
	{
		*this = *this * other;
		return(*this);
	}
	float16_t& operator/=(const float16_t &other)
	{
		*this = *this / other;
		return(*this);
	}

private:
	short m_siValue;
};

#endif
