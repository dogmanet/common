
/******************************************************
Copyright © Vitaliy Buturlin, Evgeny Danilovich, 2017
See the license in LICENSE
******************************************************/

#ifndef Array_H
#define Array_H

#include <new>
#include "types.h"

/*
	внимание:
		Элемент массива не имеет гарантированного расположения в памяти.
*/

/*#ifdef S4G
template <typename T, int BlockSize = 16>
class s4g_Stack;

#endif*/

template<typename T, int BlockSize=16, bool fully_defined=true>
class Array
{
private:
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4180)
#endif
	template<typename T0>
	struct add_const_to_pointee
	{
		typedef T0 type;
	};

	template <typename T0>
	struct add_const_to_pointee<T0*>
	{
		typedef const T0* type;
	};
#ifdef _MSC_VER
#pragma warning(pop)
#endif
public:
	Array()
	{
		// Alloc();
	}

	Array(const T &val, UINT iCount)
	{
		Realloc(iCount);
		for(UINT i = 0; i < iCount; ++i)
		{
			(*this)[i] = val;
		}
	}

	explicit Array(UINT iCount)
	{
		Realloc(iCount);
	}

	Array(const Array &arr)
	{
		Realloc(arr.Size);
		//this->AllocSize = arr.AllocSize;
		//this->Size = arr.Size;
		for(UINT i = 0; i < arr.Size; ++i)
		{
			(*this)[i] = arr[i];
		}
	}

	void swap(Array &arr)
	{
		UINT tmpS = Size;
		Size = arr.Size;
		arr.Size = tmpS;

		tmpS = AllocSize;
		AllocSize = arr.AllocSize;
		arr.AllocSize = tmpS;

		T* tmpD = Data;
		Data = arr.Data;
		arr.Data = tmpD;
	}

	void resize(UINT NewSize)
	{
		if(NewSize == Size)
		{
			return;
		}
		Realloc(NewSize);
		//ConstructInterval(this->Size, key);
		if(this->Size < NewSize)
		{
			ConstructInterval(this->Size, NewSize - 1);
		}
		this->Size = NewSize;
	}

	void resizeFast(UINT NewSize)
	{
		if(NewSize == Size)
		{
			return;
		}
		if(this->Size < NewSize)
		{
			resize(NewSize);
			return;
		}

		DestructInterval(NewSize, this->Size - 1);

		this->Size = NewSize;
	}

	void reserve(UINT size)
	{
		if(AllocSize < size)
		{
			Realloc(size);
		}
	}

	UINT size() const
	{
		return(Size);
	}

	void push_back(const T & data)
	{
		(*this)[this->Size] = data;
	}

	void erase(UINT key)
	{
		assert(key < this->Size);

		if(key < this->Size)
		{
			(&this->Data[key])->~T();
			if(key < this->Size - 1)
			{
				memcpy(&this->Data[key], &this->Data[key + 1], sizeof(T)* (this->Size - key - 1));
			}
			this->Size--;
		}
	}

	//inline Array & operator=(T* arr)
	//{
	//this->AllocSize = arr.AllocSize;
	//this->Size = arr.Size;
	/*for(int i = arr.Size - 1; i >= 0; i--)
	{
	(*this)[i] = arr[i];
	}*/
	//	return(*this);
	//}

	Array& operator=(const Array<T, BlockSize> & arr)
	{
		//this->AllocSize = arr.AllocSize;
		//this->Size = arr.Size;
		if(arr.Size < this->Size)
		{
			DestructInterval(arr.Size, this->Size - 1);
			this->Size = arr.Size;
		}
		for(int i = arr.Size - 1; i >= 0; i--)
		{
			(*this)[i] = arr[i];
		}
		return(*this);
	}

	operator T*()
	{
		return(Data);
	}
	
	operator const T*() const
	{
		return(Data);
	}

	T& get(UINT key)
	{
		return this->operator[](key);
	}

	T& operator[](int _key)
	{
		assert(_key >= 0);
		UINT key = (UINT)_key;
		if(key >= this->Size)
		{
			if(key >= this->AllocSize)
			{
				Realloc(max(this->AllocSize, key) + BlockSize);
			}
			ConstructInterval(this->Size, key);
			this->Size = key + 1;
		}
		return(Data[key]);
	}


	T& GetKeyOC(UINT key)
	{
		return(Data[key]);
	}

	void SetKeyOC(UINT key, T& val)
	{
		Data[key] = val;
	}

	const T& get(UINT key) const
	{
		return this->operator[](key);
	}

	const T& operator[](int key) const
	{
		return(Data[key]);
	}

	~Array()
	{
		if(Size)
		{
			DestructInterval(0, Size - 1);
		}
		_aligned_free(Data);
	}

	void clear()
	{
		if(Size)
		{
			DestructInterval(0, Size - 1);
		}
		_aligned_free(Data);
		Size = 0;
		AllocSize = 0;
		Data = NULL;
		//Alloc();
	}

	void clearFast()
	{
		if(Size)
		{
			DestructInterval(0, Size - 1);
		}
		Size = 0;
	}

	UINT GetAllocSize()
	{
		return AllocSize;
	}

	void quickSort()
	{
		quickSort([](const T &a, const T &b){
			return(a < b); 
		});
	}

	void insert(const T &data, int index)
	{
		insert(data, (UINT)index);
	}

	void insert(const T &data, UINT index)
	{
		if(index >= this->size())
		{
			(*this)[index] = data;
		}
		else
		{
			bool wasRealloced = Size == AllocSize;
			for(UINT i = Size; i > index; --i)
			{
				(*this)[i] = (*this)[i - 1];
				if(wasRealloced)
				{
					wasRealloced = false;
					(*this)[i] = (*this)[i - 1];
				}
			}
			(*this)[index] = data;
		}
	}

	template <typename L>
	UINT insert(const T &data, const L &CompareFunc = [](const L &a, const L &b){return(a < b); })
	{
		this->push_back(data);
		int item = size() - 2;
		while(item >= 0 && CompareFunc(this->Data[item], data))
		{
			this->Data[item + 1] = this->Data[item];
			this->Data[item] = data;
			--item;
		}

		return(item + 1);
	}

	template <typename L>
	void quickSort(const L& CompareFunc = [](const L &a, const L &b){return(a < b);})
	{
		//don't sort 0 or 1 elements
		if(size() > 1)
		{
			quickSortInternal(CompareFunc, 0, size() - 1);
		}
	}

	int indexOf(typename add_const_to_pointee<T>::type const &other) const
	{
		return(indexOf(other, [](const T &a, typename add_const_to_pointee<T>::type const &b){
			return(a == b);
		}));
	}

	template <typename L, typename O>
	int indexOf(const O &other, const L& CompareFunc) const
	{
		for(int i = 0, l = size(); i < l; ++i) 
		{
			if(CompareFunc(operator[](i), other))
			{
				return(i);
			}
		}
		return(-1);
	}

	template<typename O, int S = BlockSize>
	void append(const Array<O, S> &other)
	{
		reserve(size() + other.size());

		for(UINT i = 0, l = other.size(); i < l; ++i)
		{
			push_back(other[i]);
		}
	}


protected:
	/*
#ifdef S4G
	friend s4g_Stack<T, BlockSize>;
	#endif*/
	void Alloc()
	{
		Realloc(BlockSize);
	}

	void Realloc(UINT NewSize)
	{
		if(this->AllocSize == NewSize)
		{
			return;
		}
		T *tmpData = (T*)_aligned_malloc(sizeof(T) * NewSize, alignof(T));
		assert(tmpData);
		if(!tmpData)
		{
			return;
		}
		memcpy(tmpData, this->Data, min(NewSize, this->Size) * sizeof(T));
		if(this->Size > NewSize)
		{
			DestructInterval(NewSize, this->Size - 1);
			this->Size = NewSize;
		}

		this->AllocSize = NewSize;
		T * tmpDel = this->Data;
		this->Data = tmpData;
		_aligned_free(tmpDel);
	}

	void ConstructInterval(UINT start, UINT end)
	{
		//this->Data + start = new(this->Data + start) T[end - start + 1];
		for(UINT i = start; i <= end; i++)
		{
			new(&this->Data[i]) T;
		}
	}

	void DestructInterval(UINT start, UINT end)
	{
		for(UINT i = start; i <= end; i++)
		{
			(&this->Data[i])->~T();
		}
	}

	template <typename L>
	void quickSortInternal(const L& CompareFunc, int lo, int hi)
	{
		//  lo is the lower index, hi is the upper index
		//  of the region of array a that is to be sorted
		int i = lo, j = hi;
		T x = Data[(lo + hi) / 2];

		//  partition
		do
		{
			while(CompareFunc(Data[i], x))
				i++;
			while(CompareFunc(x, Data[j]))
				j--;
			if(i <= j)
			{
				swap(i, j);
				i++; j--;
			}
		}
		while(i <= j);

		//  recursion
		if(lo < j)
			quickSortInternal(CompareFunc, lo, j);
		if(i < hi)
			quickSortInternal(CompareFunc, i, hi);
	}

	void swap(int index0, int index1)
	{
		std::swap(Data[index0], Data[index1]);
	}

	T *Data = NULL;

	UINT Size = 0;
	UINT AllocSize = 0;
};

template<>
class Array<char>: public Array<char, 1024, false>
{
public:
	void append(const char *szString)
	{
		int len = (int)strlen(szString);
		reserve(size() + len + 1);
		for(int i = 0; i < len; ++i)
		{
			push_back(szString[i]);
		}
	}
};


#endif
