#ifndef _FLOAT16_H_
#define _FLOAT16_H_

#include <memory>

class float16_t
{
private:
	union f32
	{
		struct
		{
			unsigned int m : 23;
			unsigned int e : 8;
			unsigned int s : 1;
		} i;
		float f;
	};


public:
	float16_t()
	{
		m_siValue.m = 0;
		m_siValue.e = 0;
		m_siValue.s = 0;
	}
	float16_t(float value)
	{
		f32 f;
		f.f = value;

		m_siValue.m = f.i.m >> 13;
		m_siValue.e = f.i.e + 15 - 127;
		m_siValue.s = f.i.s;
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
		f32 f;
		f.i.s = m_siValue.s;
		f.i.e = m_siValue.e;
		f.i.m = m_siValue.m;

		return(f.f);
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
	struct
	{
		unsigned short m: 10;
		unsigned short e: 5;
		unsigned short s: 1;
	} m_siValue;
};

#endif
