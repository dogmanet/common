
/******************************************************
Copyright © Vitaliy Buturlin, Evgeny Danilovich, 2017
See the license in LICENSE
******************************************************/

#ifndef MEMALLOC_H
#define MEMALLOC_H

#define mem_free(a) free(a)
#define mem_alloc(a) malloc(a)

#if defined(_WINDOWS)
#	include <new.h>
#else
#	include <new>
#endif
#include "types.h"
#include <malloc.h>
#if defined(_WINDOWS)
#	pragma warning(push)
#	pragma warning(disable:4018)
#endif

#if defined(_LINUX) || defined(_MAC)
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wsign-compare"
#endif

struct UsageStats
{
	UINT uAllocCount; // количество занятых элементов
	UINT uFreeCount; // количество свободных элементов
	ULONG ulSysMem; // Количество выделенной у системы памяти
	ULONG ulAllocMem; //Количество занятой элементами памяти
};

template <typename T, int SizeBlock = 256, int SizePage = 16, const int alignBy = alignof(T)>
class MemAlloc
{
	static_assert(alignBy % alignof(T) == 0 && alignBy >= alignof(T), "alignBy should be multiply of alignof(T)");
	
#undef MEMALLOC_DEFAULT_ALIGN
#undef MEMALLOC_DEFAULT_ALIGN_STR

#pragma pack(push, 1)
	struct MemCell
	{
		union
		{
			UINT IsFree;
			byte _padding[alignBy];
		};
		T data;
	};
#pragma pack(pop)
	struct AlignTest
	{
		char start;
		T end;
	};
	struct MemBlock
	{
		MemCell * mem;
		UINT size;
		UINT pos;
		UINT used;
	};
	MemBlock * memblocks;
	int NumCurBlock;
	int NumCurBlockCount;
public:
	
	MemAlloc():memblocks(NULL), NumCurBlock(0), NumCurBlockCount(0)
	{
	//	static_assert((intptr_t)(&(((AlignTest*)0)->end)) - (intptr_t)(((AlignTest*)0)->start) == alignBy, "Invalid align specified!");
		AllocBlock();
	}

	MemAlloc(const MemAlloc & al)
	{
		//printf("MemAlloc()\n");
		this->NumCurBlock = al.NumCurBlock;
		this->NumCurBlockCount = al.NumCurBlockCount;
		this->memblocks = new MemBlock[this->NumCurBlockCount];
		//SX_DBG_NEW(this->NumCurBlockCount * sizeof(MemBlock), this->memblocks);
		for(int i = 0; i < this->NumCurBlockCount; i++)
		{
			this->memblocks[i].size = al.memblocks[i].size;
			this->memblocks[i].pos = al.memblocks[i].pos;
			this->memblocks[i].mem = (MemCell*)_aligned_malloc(al.memblocks[i].size * sizeof(MemCell), alignBy);
			//SX_DBG_NEW(al.memblocks[i].size, this->memblocks[i].mem);
			for(int j = 0; j < al.memblocks[i].size; j++)
			{
				this->memblocks[i].mem[j].IsFree = al.memblocks[i].mem[j].IsFree;
				this->memblocks[i].mem[j].data = al.memblocks[i].mem[j].data;
			}
			//SX_SAFE_DELETE_A(memblocks[i].mem);
		}
	}

	MemAlloc & operator=(const MemAlloc & al)
	{//clear mem
		if(&al != this)
		{
			for(int i = 0; i < this->NumCurBlockCount; i++)
			{
				if(this->memblocks[i].mem)
				{
					for(int j = 0; j < this->memblocks[i].size; j++)
					{
						if(!(this->memblocks[i].mem[j].IsFree & 0x80000000))
						{
							this->memblocks[i].mem[j].data.~T();
						}

					}
					_aligned_free(this->memblocks[i].mem);
				}
				//SX_SAFE_DELETE_A(this->memblocks[i].mem);
			}
			mem_delete_a(this->memblocks);

			this->NumCurBlock = al.NumCurBlock;
			this->NumCurBlockCount = al.NumCurBlockCount;
			this->memblocks = new MemBlock[this->NumCurBlockCount];
			//SX_DBG_NEW(this->NumCurBlockCount * sizeof(MemBlock), this->memblocks);
			for(int i = 0; i < this->NumCurBlockCount; i++)
			{
				this->memblocks[i].size = al.memblocks[i].size;
				this->memblocks[i].pos = al.memblocks[i].pos;

				if(!this->memblocks[i].size)
				{
					this->memblocks[i].mem = NULL;
				}
				else
				{
					this->memblocks[i].mem = (MemCell*)_aligned_malloc(al.memblocks[i].size * sizeof(MemCell), alignBy);
					//SX_DBG_NEW(al.memblocks[i].size, this->memblocks[i].mem);
					for(int j = 0; (unsigned int)j < al.memblocks[i].size; j++)
					{
						this->memblocks[i].mem[j].IsFree = al.memblocks[i].mem[j].IsFree;
						if(!(al.memblocks[i].mem[j].IsFree & 0x80000000))
						{
							this->memblocks[i].mem[j].data = al.memblocks[i].mem[j].data;
						}
					}
				}
				//SX_SAFE_DELETE_A(memblocks[i].mem);
			}
		}
		return(*this);
	}

	~MemAlloc()
	{
		//printf("~MemAlloc()\n");
		for(int i = 0; i < this->NumCurBlockCount; i++)
		{
			if(this->memblocks[i].mem)
			{
				for(int j = 0; j < this->memblocks[i].size; j++)
				{
					if(!(this->memblocks[i].mem[j].IsFree & 0x80000000))
					{
						this->memblocks[i].mem[j].data.~T();
					}

				}
				_aligned_free(this->memblocks[i].mem);
			}
			//SX_SAFE_DELETE_A(this->memblocks[i].mem);
		}
		mem_delete_a(this->memblocks);
		this->NumCurBlockCount = 0;
		this->NumCurBlock = 0;
	}

	void clear()
	{
		for(int i = 0; i < this->NumCurBlockCount; i++)
		{
			if(this->memblocks[i].mem)
			{
				for(int j = 0; j < this->memblocks[i].size; j++)
				{
					if(!(this->memblocks[i].mem[j].IsFree & 0x80000000))
					{
						(&(this->memblocks[i].mem[j].data))->~T();
					}
				}
				_aligned_free(this->memblocks[i].mem);
			}
			//SX_SAFE_DELETE_A(this->memblocks[i].mem);
		}
		mem_delete_a(this->memblocks);
		this->NumCurBlockCount = 0;
		this->NumCurBlock = 0;
		//AllocBlock();
	}

	void clearFast()
	{
		for(int i = 0; i < this->NumCurBlockCount; i++)
		{
			if(this->memblocks[i].mem && this->memblocks[i].used)
			{
				for(int j = 0; j < this->memblocks[i].size; j++)
				{
					if(!(this->memblocks[i].mem[j].IsFree & 0x80000000))
					{
						(&(this->memblocks[i].mem[j].data))->~T();
					}
					this->memblocks[i].mem[j].IsFree = 0x80000000 | (j + 1);
				}
				this->memblocks[i].pos = 0;
				this->memblocks[i].used = 0;
			}
		}
		this->NumCurBlock = 0;
	}

	void AllocBlock(UINT size = SizeBlock)
	{
		MemBlock * tmpMB = this->memblocks;

		NumCurBlockCount += SizePage;

		this->memblocks = new MemBlock[NumCurBlockCount];
		//SX_DBG_NEW(this->NumCurBlockCount * sizeof(MemBlock), this->memblocks);
		if(tmpMB)
		{
			memcpy(this->memblocks, tmpMB, (NumCurBlockCount - SizePage) * sizeof(MemBlock));
		}
		memset(this->memblocks + (NumCurBlockCount - SizePage), 0, SizePage * sizeof(MemBlock));
		mem_delete_a(tmpMB);


		FillCurrentBlock(size);
	}

	__forceinline void FillCurrentBlock(UINT size = SizeBlock)
	{
		if(!this->memblocks[NumCurBlock].mem)
		{
			this->memblocks[NumCurBlock].mem = (MemCell*)_aligned_malloc(size * sizeof(MemCell), alignBy);
			//SX_DBG_NEW(size * sizeof(MemCell), this->memblocks[NumCurBlock].mem);
			for(int i = 0; i < size; i++)
			{
				this->memblocks[NumCurBlock].mem[i].IsFree = 0x80000000 | (i + 1);
			}
			this->memblocks[NumCurBlock].size = size;
		}
	}

	template<typename... Args>
	T * Alloc(Args&&... args)
	{
		T * tmpNewNode = NULL;
		if(!NumCurBlockCount)
		{
			AllocBlock();
		}
		if(this->memblocks[NumCurBlock].pos < this->memblocks[NumCurBlock].size)
		{
			MemCell * mc = &(this->memblocks[NumCurBlock].mem[this->memblocks[NumCurBlock].pos]);
			if(mc->IsFree & 0x80000000)
			{
				this->memblocks[NumCurBlock].pos = mc->IsFree & 0x7FFFFFFF;
				mc->IsFree = NumCurBlock;
				tmpNewNode = &(mc->data);
			}
			else // find next free cell
			{
				while(true)
				{
					++this->memblocks[NumCurBlock].pos;
					if(this->memblocks[NumCurBlock].pos < this->memblocks[NumCurBlock].size)
					{
						mc = &(this->memblocks[NumCurBlock].mem[this->memblocks[NumCurBlock].pos]);
						if(mc->IsFree)
						{
							this->memblocks[NumCurBlock].pos = mc->IsFree & 0x7FFFFFFF;
							mc->IsFree = NumCurBlock;
							tmpNewNode = &(mc->data);
							break;
						}
					}
					else
					{
						//next block
						NumCurBlock++;
						if(NumCurBlock < NumCurBlockCount)
						{
							FillCurrentBlock();
						}
						else
						{
							AllocBlock();
						}
						return(Alloc(args...));
					}
				}
			}
		}
		else
		{
			NumCurBlock++;
			if(NumCurBlock < NumCurBlockCount)
			{
				FillCurrentBlock();
			}
			else
			{
				AllocBlock();
			}
			return(Alloc(args...));
		}
		++this->memblocks[NumCurBlock].used;
		tmpNewNode = new (tmpNewNode)T(args...);
		return(tmpNewNode);
	}

	T * GetAt(int page, int offset)
	{
		if(!(memblocks[page].mem[offset].IsFree & 0x80000000))
		{
			return(&memblocks[page].mem[offset].data);
		}
		return(NULL);
	}

	void Delete(T * pointer)
	{
		//find cell
		//mark cell as free

		UINT * bnum = (UINT*)((intptr_t)pointer - alignBy);
		UINT blockID = *bnum;
		UINT curPos = (UINT)((intptr_t)bnum - (intptr_t)memblocks[blockID].mem) / sizeof(MemCell);
		--this->memblocks[blockID].used;
		pointer->~T();

		if(NumCurBlock > blockID)
		{
			NumCurBlock = blockID;
		}
		if(memblocks[blockID].pos > curPos)
		{
			*bnum = memblocks[blockID].pos | 0x80000000;
			memblocks[blockID].pos = curPos;
		}
		else
		{
			MemCell * mc = &(memblocks[blockID].mem[memblocks[blockID].pos]);
			*bnum = mc->IsFree;
			mc->IsFree = curPos | 0x80000000;
		}
	}

	void Delete(void * ptr)
	{
		Delete((T*)ptr);
	}

	void GetMemUsage(UsageStats * us)
	{
		memset(us, 0, sizeof(UsageStats));
		for(int i = 0; i < this->NumCurBlockCount; i++)
		{
			us->uAllocCount += this->memblocks[i].used;
			us->uFreeCount += this->memblocks[i].size - this->memblocks[i].used;
		}
		us->ulAllocMem = us->uAllocCount * sizeof(T);
		us->ulSysMem = (us->uAllocCount + us->uFreeCount) * sizeof(T) + NumCurBlockCount * sizeof(MemBlock);
	}

	void releaseEmptyPages()
	{
		int iPrevFreeBlock = 0;
		for(int i = 1; i < this->NumCurBlockCount; ++i)
		{
			if(this->memblocks[i].mem)
			{
				bool isFree = true;
				for(int j = 0; j < this->memblocks[i].size; j++)
				{
					if(!(this->memblocks[i].mem[j].IsFree & 0x80000000))
					{
						isFree = false;
						break;
					}
				}
				if(isFree)
				{
					iPrevFreeBlock = i;
					mem_free(this->memblocks[i].mem);
					this->memblocks[i].mem = NULL;
				}
				else if(iPrevFreeBlock >= 0)
				{
					if(NumCurBlock == i)
					{
						NumCurBlock = iPrevFreeBlock;
					}
					this->memblocks[iPrevFreeBlock] = this->memblocks[i];
					memset(&this->memblocks[i], 0, sizeof(this->memblocks[i]));
					++iPrevFreeBlock;
				}
			}
		}
	}
};

#if defined(_WINDOWS)
#	pragma warning(pop)
#endif
#if defined(_LINUX) || defined(_MAC)
#	pragma GCC diagnostic pop
#endif

#endif
