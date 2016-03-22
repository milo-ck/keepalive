#pragma once
#include "xlib/MemoryPool.hpp"
#include "xlib/Exception.hpp"

namespace xlib
{
	inline void allocPool(const Int* sizes, const UShort* allocNumPatch, const UShort* initCount, Int count)
	{
		MemoryPool::pool()->init(sizes, allocNumPatch, initCount, count);
	}
	inline void allocDump()
	{
		MemoryPool::pool()->dump();
	}
	inline Object alloc(Int size)
	{
		return MemoryPool::pool()->alloc(size);
	}
	inline Nil dealloc(Object ptr)
	{
		MemoryPool::pool()->dealloc(ptr);
	}

	template<typename T>
	inline T* alloc(Int count = 1)
	{
		return static_cast<T*>(alloc(count * sizeof(T)));
	}
	template<typename T>
	inline Nil dispose(Object ptr, Int count = 1)
	{
		if (!ptr)return;

		T* t = static_cast<T*>(ptr);
		try{for (Int i = 0; i < count; i++, t++)t->~T();}
		catch(...){}

		dealloc(ptr);
	}
	template<typename T>
	inline T* create(Int count = 1)
	{
		T* ret = alloc<T>(count); T* tmp = ret;
		try{for (Int i = 0; i < count; i++, tmp++)new (tmp) T();}
		catch(Exception& exp)	{ dispose<T>(ret, count); throw exp; }
		return ret;
	}
	template<typename T, typename P1>
	inline T* create(P1 p1, Int count = 1)
	{
		T* ret = alloc<T>(count); T* tmp = ret;
		try{for (Int i = 0; i < count; i++, tmp++)new (tmp) T(p1);}
		catch(Exception& exp)	{ dispose<T>(ret, count); throw exp; }
		return ret;
	}
	template<typename T, typename P1, typename P2>
	inline T* create(P1 p1, P2 p2, Int count = 1)
	{
		T* ret = alloc<T>(count); T* tmp = ret;
		try{for (Int i = 0; i < count; i++, tmp++)new (tmp) T(p1, p2);}
		catch(Exception& exp)	{ dispose<T>(ret, count); throw exp; }
		return ret;
	}
	template<typename T, typename P1, typename P2, typename P3>
	inline T* create(P1 p1, P2 p2, P3 p3, Int count = 1)
	{
		T* ret = alloc<T>(count); T* tmp = ret;
		try{for (Int i = 0; i < count; i++, tmp++)new (tmp) T(p1, p2, p3);}
		catch(Exception& exp)	{ dispose<T>(ret, count); throw exp; }
		return ret;
	}
}
