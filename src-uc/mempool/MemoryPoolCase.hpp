#pragma once

#include "xlib/Log.hpp"
#include "xlib/allocator.hpp"
class MemoryPoolCase
{
public:
	class A
	{
	private:
		Int a;
		Int b;
	public:
		A():a(10), b(20)
		{
			xprint("Calling construct A, a=%d, b=%d", a, b);
		}
		virtual ~A()
		{
			xprint("Calling unconstruct A.");
		}
	};
	class B : public A
	{
	private:
		const Char* name_;
		Int e, f, g, h, i, j, k, l;
	public:
		B(const Char* name) : A(), name_(name)
		{
			xlib::Log::print("Calling construct B. name=%s", name_);
		}
		virtual ~B()
		{
			xlib::Log::print("Calling unconstruct B.");
		}
	};
	MemoryPoolCase()
	{
		
	};
	void normal()
	{
		Int* i = xlib::alloc<Int>();
		*i = 100;
		xlib::Log::print("Alloc int value %d.", *i);
		xlib::dealloc(i);
		xlib::Log::print("Dealloc int value.");

		Char* s = xlib::alloc<Char>(1000);
		s[0] = 'a';
		s[1] = 'b';
		s[2] = '\0';
		xlib::Log::print("Alloc char array ptr=%s.", s);
		xlib::dealloc(s);
		xlib::Log::print("Dealloc char array.");
		
		xlib::Log::print("Create 4 class B, The class extends from A.");
		A* a = xlib::create<B>("[B]", 4);
		xlib::dispose<B>(a, 4);
		xlib::Log::print("Dispose 4 class B.");
	}

	void allocNormal()
	{
		xprint("[Alloc Test Normal]");
		xlib::MemoryPool::pool()->clear();
		Int sizes[2] = {32, 64};
		UShort nums[2] = {2,2};
		UShort inits[2] = {1, 1};

		xlib::allocPool(sizes, nums, nums, 2);

		Byte* data1 = xlib::alloc<Byte>(32);
		Byte* data2 = xlib::alloc<Byte>(32);
		Byte* data3 = xlib::alloc<Byte>(64);
		Byte* data4 = xlib::alloc<Byte>(64);
		
		xprint("Alloc many memories. %llx, %llx, %llx, %llx", data1, data2, data3, data4);
		xlib::allocDump();

		xprint("Dealloc many memories.");
		xlib::dealloc(data2);
		xlib::dealloc(data1);
		
		xlib::dealloc(data3);
		xlib::dealloc(data4);
		xlib::allocDump();

		data1 = xlib::alloc<Byte>(32);
		data2 = xlib::alloc<Byte>(32);
		data3 = xlib::alloc<Byte>(64);
		data4 = xlib::alloc<Byte>(64);

		xprint("Alloc many memories. %llx, %llx, %llx, %llx", data1, data2, data3, data4);
		xlib::allocDump();

		xprint("Dealloc many memories.");
		xlib::dealloc(data2);
		xlib::dealloc(data1);

		xlib::dealloc(data3);
		xlib::dealloc(data4);
		xlib::allocDump();
	}
	void allocTime()
	{
		xprint("[Alloc Test Time]");
		xlib::MemoryPool::pool()->clear();
		Int sizes[2] = { 1024, 409600 };
		UShort nums[2] = { 512,20 };
		UShort inits[2] = { 10, 10 };
		xlib::allocPool(sizes, nums, nums, 2);

		xclock_init();
		for (Int i = 0; i < 2; i++)
		{
			Int init = inits[i];
			Int size = sizes[i];
			Int num = nums[i];
			for (Int n = 0; n < init; n++)
			{
				for (Int m = 0; m < num; m++)
				{
					Byte* data = xlib::alloc<Byte>(size);
				}
			}
		}
		xclock();
		for (Int i = 0; i < 2; i++)
		{
			Int init = inits[i];
			Int size = sizes[i];
			Int num = nums[i];
			for (Int n = 0; n < init; n++)
			{
				for (Int m = 0; m < num; m++)
				{
					Byte* data = (Byte*)malloc(size);
				}
			}
		}
		xclock();

	}
	void run()
	{
		allocNormal();
		allocTime();
		return;
		Int patchSizes[7] = { 32,64,128,256,1024,2048,4096 };
		UShort patchNums[7] = { 2,2,2,2,2,2,2 };
		UShort initPatchNums[7] = { 2,2,2,2,2,2,2 };

		xlib::allocPool(patchSizes, patchNums, initPatchNums, 7);
		xlib::allocDump();
		return;
		Byte* data1 = xlib::alloc<Byte>(32);
		Byte* data2 = xlib::alloc<Byte>(32);
		Byte* data3 = xlib::alloc<Byte>(128);
		Byte* data4 = xlib::alloc<Byte>(4096);
		xprint("Alloc many memories. %llx, %llx, %llx, %llx", data1, data2, data3, data4);
		
		xprint("Dealloc many memories.");
		xlib::dealloc(data1);
		xlib::dealloc(data2);
		xlib::dealloc(data3);
		xlib::dealloc(data4);
		xlib::allocDump();
		return;
		xclock_init();
		Int* ptr[10240];
		for (Int i = 0; i < 10240; i++)
		{
			ptr[i] = xlib::alloc<Int>(i+1);
		}
		xclock();
		//xlib::allocDump();
		/*Int sizeI = sizeof(Int);
		for (Int i = 0; i < 10240; i++)
		{
			ptr[i] = (Int *)malloc((i + 1) * sizeI);
		}*/
		xclock();
		
	}
};