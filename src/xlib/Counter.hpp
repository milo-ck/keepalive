#pragma once
#ifdef XPLATFORM_WIN
#include <Windows.h>
#endif 

namespace xlib
{
	class Counter
	{
	private:
#ifdef XPLATFORM_WIN
		volatile Int value_;
		Int value() 
		{ 
			return value_; 
		}
		Nil value(Int i) 
		{
			InterlockedExchange((unsigned Int *)&value_, (unsigned Int)i);
		}
		Nil increment() 
		{
			InterlockedIncrement((unsigned Int*)&value_);
		}
		Nil decrement()
		{
			InterlockedDecrement((unsigned Int*)&value_);
		}
#else
		atomic_t v_;
		Int value()
		{
			return v_.counter;
		}
		Nil value(Int i)
		{
			atomic_set(&v_, i);
		}
		Nil increment()
		{
			atomic_inc(&v);
		}
		Nil decrement()
		{
			atomic_dec(&v_);
		}
#endif // XPLATFORM_WIN

		
	public:
		enum Value
		{
			Max = 2147483640,
			Min = -2147483640,
		};
		Counter()
		{
			
		}
		Counter& operator = (Int i)
		{
			value(i);
			return *this;
		}
		Counter& operator ++ ()
		{
			increment();
			return *this;
		}
		Counter& operator -- ()
		{
			decrement();
			return *this;
		}
		operator Int()
		{
			return value();
		}
		operator unsigned Int()
		{
			return (unsigned Int)value();
		}
		bool operator >= (Int i)
		{
			return value() >= i;
		}
		bool operator <= (Int i)
		{
			return value() <= i;
		}
		bool operator == (Int i)
		{
			return value() == i;
		}
		bool operator > (Int i)
		{
			return value() > i;
		}
		bool operator < (Int i)
		{
			return value() < i;
		}

	};	
};
