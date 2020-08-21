
/******************************************************
Copyright © Vitaliy Buturlin, Evgeny Danilovich, 2017
See the license in LICENSE
******************************************************/

#ifndef String_H
#define String_H

#include <string.h>
#define _NO_GTK

#if !defined(_WINNT_)
typedef wchar_t WCHAR;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned int UINT;
#endif

//##########################################################################

class StringW;

class String
{
public:
	friend class StringW;
	String();
	String(const char *	str);
	String(const char *	str, int iCount);
	String(const char		sym);
	String(const int		num);
	String(const unsigned short		num);
	String(const unsigned long	num);
	String(const UINT num);
	String(const long		num);
	String(const double	num);
	explicit String(bool bf);
	String(const String & str);
	String(const String * str);

	String(String && other);

	~String();

	void release();

	////////////////////////////////////
	//операции сложения строк
	//возвращается образуется новый объект String 
	//который и содержит результат сложения строк

	String operator+(const String & str) const;

	String operator+(const char *	str);
	String operator+(const char &	sym);
	String operator+(const int &	num);
	String operator+(const unsigned short &	num);
	String operator+(const unsigned long &num);
	String operator+(const long &	num);
	String operator+(const double &num);
	String operator+(const bool &	bf);

	////////////////////////////////////
	//операции присваивания

	String & operator=(const String & str);

	String & operator=(const char *	str);
	String & operator=(const char &	sym);
	String & operator=(const int &	num);
	String & operator=(const unsigned short &	num);
	String & operator=(const unsigned long &	num);
	String & operator=(const long &	num);
	String & operator=(const double &	num);
	String & operator=(const bool &	bf);

	String & operator=(String && other);

	////////////////////////////////////
	//операции сложения строк объекта к которому применяется сложение и слагаемого
	String & operator+=(const String &str);

	String & operator+=(const char *	str);
	String & operator+=(const char &	sym);
	String & operator+=(const int &	num);
	String & operator+=(const unsigned short &	num);
	String & operator+=(const unsigned long &	num);
	String & operator+=(const long &	num);
	String & operator+=(const double &num);
	String & operator+=(const bool &	bf);

	////////////////////////////////////
	//искоючить первое нахождение из строки
	//возвращается новый объект String, который содержит результат
	String operator-(const String & str);

	String operator-(const char *	str);
	String operator-(const char &	sym);
	String operator-(const int &	num);
	String operator-(const unsigned short &	num);
	String operator-(const unsigned long &	num);
	String operator-(const long &	num);
	String operator-(const double &	num);
	String operator-(const bool &	bf);

	////////////////////////////////////
	//исключает первое нахождение из данного объекта
	String & operator-=(const String &str);

	String & operator-=(const char *	str);
	String & operator-=(const char &	sym);
	String & operator-=(const int &	num);
	String & operator-=(const unsigned short &	num);
	String & operator-=(const unsigned long &	num);
	String & operator-=(const long &	num);
	String & operator-=(const double &num);
	String & operator-=(const bool &	bf);

	//////////////////////////////////////
	//исключить все похожие нахождения
	//возвращается новый объект String
	String operator / (const String &str);

	String operator/(const char *	str);
	String operator/(const char &	sym);
	String operator/(const int &	num);
	String operator/(const unsigned short &	num);
	String operator/(const unsigned long &	num);
	String operator/(const long &	num);
	String operator/(const double &	num);
	String operator/(const bool &	bf);

	////////////////////////////////////
	//исключить все похожие нахождения
	String & operator/=(const String &str);

	String & operator/=(const char *	str);
	String & operator/=(const char &	sym);
	String & operator/=(const int &	num);
	String & operator/=(const unsigned short &	num);
	String & operator/=(const unsigned long &	num);
	String & operator/=(const long &	num);
	String & operator/=(const double &num);
	String & operator/=(const bool &	bf);

	////////////////////////////////////

	bool operator==(const String &str) const;

	bool operator==(const char *	str) const;
	bool operator==(const char &	sym) const;
	bool operator==(const int &		num) const;
	bool operator==(const unsigned short &	num) const;
	bool operator==(const unsigned long &	num) const;
	bool operator==(const long &	num) const;
	bool operator==(const double &	num) const;
	bool operator==(const bool &	bf) const;

	//////////////////////////////////////

	bool operator!= (const String &str) const;

	bool operator!=(const char *	str) const;
	bool operator!=(const char &	sym) const;
	bool operator!=(const int &		num) const;
	bool operator!=(const unsigned short &	num) const;
	bool operator!=(const unsigned long &	num) const;
	bool operator!=(const long &	num) const;
	bool operator!=(const double &	num) const;
	bool operator!=(const bool &	bf) const;

	//////////////////////////////////////
	char & operator[](const unsigned long & num);
	const char & operator[](const unsigned long & num) const;
	//////////////////////////////////////

	unsigned long length() const;
	unsigned long find(const char & c, unsigned long pos = 0) const;
	unsigned long find(const char * str, unsigned long pos = 0) const;
	unsigned long find(const String & str, unsigned long pos = 0) const;

	unsigned long find_last_of(const char & c, unsigned long pos = 0) const;
	unsigned long find_last_of(const char * str, unsigned long pos = 0) const;
	unsigned long find_last_of(const String & str, unsigned long pos = 0) const;

	unsigned long replace(const char * str, const char * replace, unsigned long pos);
	unsigned long replace(const String & str, const String & replace, unsigned long pos);

	unsigned long replaceAll(const char * str, const char * replace);
	unsigned long replaceAll(const String & str, const String & replace);

	//char * SubStr(unsigned long pos, unsigned long lenght); // ��� ������ �� �����
	String substr(unsigned long pos, unsigned long lenght = 0) const;
	unsigned int remove(unsigned long pos, unsigned long lenght);

	String trim();

	const char * c_str() const;

	void reserve(int length);
	void appendReserve(int length);

	int		toInt() const;
	long	toLongInt() const;
	unsigned long	toUnsLongInt() const;
	double	toDouble() const;
	bool	toBool() const;

	operator StringW() const;

	bool operator<(const String & s) const;

protected:
	char * m_szString = NULL;
};

//bool operator<(const String & a, const String & b);



class StringW
{
public:
	friend class String;
	StringW();
	StringW(const WCHAR *	str);
	StringW(const WCHAR		sym);
	StringW(const int		num);
	StringW(const unsigned short		num);
	StringW(const unsigned long	num);
	StringW(const long		num);
	StringW(const double	num);
	explicit StringW(bool bf);
	StringW(const StringW & str);
	StringW(const StringW * str);

	StringW(StringW && other);

	~StringW();

	void release();

	////////////////////////////////////
	//операции сложения строк
	//возвращается образуется новый объект StringW 
	//который и содержит результат сложения строк

	StringW operator+(const StringW & str) const;

	StringW operator+(const WCHAR *	str);
	StringW operator+(const WCHAR &	sym);
	StringW operator+(const int &	num);
	StringW operator+(const unsigned short &	num);
	StringW operator+(const unsigned long &num);
	StringW operator+(const long &	num);
	StringW operator+(const double &num);
	StringW operator+(const bool &	bf);

	////////////////////////////////////
	//операции присвоения

	StringW & operator=(const StringW & str);

	StringW & operator=(const WCHAR *	str);
	StringW & operator=(const WCHAR &	sym);
	StringW & operator=(const int &	num);
	StringW & operator=(const unsigned short &	num);
	StringW & operator=(const unsigned long &	num);
	StringW & operator=(const long &	num);
	StringW & operator=(const double &	num);
	StringW & operator=(const bool &	bf);

	StringW & operator=(StringW && other);

	////////////////////////////////////
	//операции сложения строк объекта к которому применяется сложение и слагаемого
	StringW & operator+=(const StringW &str);

	StringW & operator+=(const WCHAR *	str);
	StringW & operator+=(const WCHAR &	sym);
	StringW & operator+=(const int &	num);
	StringW & operator+=(const unsigned short &	num);
	StringW & operator+=(const unsigned long &	num);
	StringW & operator+=(const long &	num);
	StringW & operator+=(const double &num);
	StringW & operator+=(const bool &	bf);

	////////////////////////////////////
	//искоючить первое нахождение из строки
	//возвращается новый объект String, который содержит результат
	StringW operator-(const StringW & str);

	StringW operator-(const WCHAR *	str);
	StringW operator-(const WCHAR &	sym);
	StringW operator-(const int &	num);
	StringW operator-(const unsigned short &	num);
	StringW operator-(const unsigned long &	num);
	StringW operator-(const long &	num);
	StringW operator-(const double &	num);
	StringW operator-(const bool &	bf);

	////////////////////////////////////
	//исключает первое нахождение из данного объекта
	StringW & operator-=(const StringW &str);

	StringW & operator-=(const WCHAR *	str);
	StringW & operator-=(const WCHAR &	sym);
	StringW & operator-=(const int &	num);
	StringW & operator-=(const unsigned short &	num);
	StringW & operator-=(const unsigned long &	num);
	StringW & operator-=(const long &	num);
	StringW & operator-=(const double &num);
	StringW & operator-=(const bool &	bf);

	//////////////////////////////////////
	//исключить все похожие нахождения
	//возвращается новый объект StringW
	StringW operator / (const StringW &str);

	StringW operator/(const WCHAR *	str);
	StringW operator/(const WCHAR &	sym);
	StringW operator/(const int &	num);
	StringW operator/(const unsigned short &	num);
	StringW operator/(const unsigned long &	num);
	StringW operator/(const long &	num);
	StringW operator/(const double &	num);
	StringW operator/(const bool &	bf);

	////////////////////////////////////
	//исключить все похожие нахождения
	StringW & operator/=(const StringW &str);

	StringW & operator/=(const WCHAR *	str);
	StringW & operator/=(const WCHAR &	sym);
	StringW & operator/=(const int &	num);
	StringW & operator/=(const unsigned short &	num);
	StringW & operator/=(const unsigned long &	num);
	StringW & operator/=(const long &	num);
	StringW & operator/=(const double &num);
	StringW & operator/=(const bool &	bf);

	////////////////////////////////////

	bool operator==(const StringW &str) const;

	bool operator==(const WCHAR *	str) const;
	bool operator==(const WCHAR &	sym) const;
	bool operator==(const int &		num) const;
	bool operator==(const unsigned short &	num) const;
	bool operator==(const unsigned long &	num) const;
	bool operator==(const long &	num) const;
	bool operator==(const double &	num) const;
	bool operator==(const bool &	bf) const;

	//////////////////////////////////////

	bool operator!= (const StringW &str) const;

	bool operator!=(const WCHAR *	str) const;
	bool operator!=(const WCHAR &	sym) const;
	bool operator!=(const int &		num) const;
	bool operator!=(const unsigned short &	num) const;
	bool operator!=(const unsigned long &	num) const;
	bool operator!=(const long &	num) const;
	bool operator!=(const double &	num) const;
	bool operator!=(const bool &	bf) const;

	//////////////////////////////////////
	WCHAR & operator[](const unsigned long & num);
	const WCHAR & operator[](const unsigned long & num) const;
	//////////////////////////////////////

	void insert(unsigned int pos, const WCHAR * data);
	void insert(unsigned int pos, const StringW & data);

	unsigned long length() const;
	unsigned long find(const WCHAR & c, unsigned long pos = 0) const;
	unsigned long find(const WCHAR * str, unsigned long pos = 0) const;
	unsigned long find(const StringW & str, unsigned long pos = 0) const;

	unsigned long find_last_of(const WCHAR & c, unsigned long pos = 0) const;
	unsigned long find_last_of(const WCHAR * str, unsigned long pos = 0) const;
	unsigned long find_last_of(const StringW & str, unsigned long pos = 0) const;

	unsigned long replace(const WCHAR * str, const WCHAR * replace, unsigned long pos);
	unsigned long replace(const StringW & str, const StringW & replace, unsigned long pos);

	unsigned long replaceAll(const WCHAR * str, const WCHAR * replace);
	unsigned long replaceAll(const StringW & str, const StringW & replace);

	StringW substr(unsigned long pos, unsigned long lenght = 0) const;
	unsigned int remove(unsigned long pos, unsigned long lenght);

	StringW trim();

	const WCHAR * c_str() const;

	void reserve(int length);
	void appendReserve(int length);

	int		toInt() const;
	long	toLongInt() const;
	unsigned long	toUnsLongInt() const;
	double	toDouble() const;
	bool	toBool() const;

	operator String() const;

	bool operator<(const StringW & s) const;

protected:
	WCHAR * m_szString;
};

/*bool operator<(const StringW & a, const StringW & b);

StringW operator+(const WCHAR * a, const StringW & b);
StringW operator+(const StringW & a, const WCHAR * b);*/

#endif
