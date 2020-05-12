/*****************************************************
Copyright © DogmaNet Team, 2020
Site: dogmanet.ru
See the license in LICENSE
*****************************************************/

#ifndef __THREADSAFE_H
#define __THREADSAFE_H

#include <memory>

//! тип блокированного мьютекса для ThreadSafe::scoped_lock
typedef std::unique_lock<std::mutex> mulock;

/*! шаблон для организации потокобесзопасности обьекта
	Пример создания:
	ThreadSafe<Array<RawLockSegment>> aRawLockSegments;

	Блокировка на уровне области видимости:
	mulock oMuLock = aRawLockSegments.scoped_lock();

	Обращение к свойствам и методам такого обьекта происходит через указатель:
	aRawLockSegments->size(); 
*/
template<typename T>
class ThreadSafe
{
public:

	template<typename ...Args>
	ThreadSafe(Args ... args) :
		p(std::make_shared<T>(args...))
	{	}

	T* operator -> () { return p.get(); }
	const T* operator -> () const { return p.get(); }

	/*! блокировка доступа к обьекту на уровне области видимости
		@note если не присвоить возвращаемое значение в переменную, то блокировка тут же снимется 
	*/
	mulock scoped_lock() { return std::move(mulock(oMutex)); }

	//! заблокировать доступ 
	void lock() { oMutex.lock(); }

	//! разблокировать доступ 
	void unlock() { oMutex.unlock(); }

protected:
	std::mutex oMutex;
	std::shared_ptr<T> p;
};

#endif