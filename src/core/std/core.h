#pragma once
#include "xlib/xlib.h"
namespace ka
{
	namespace core
	{
		
		class AutoRef
		{
		public:
			virtual Int unref() {};
			virtual Int ref() {};
			virtual ~AutoRef(){}
		protected:
			virtual Nil disposing(){}
		};
		
		template<class T>
		class AutoPtr
		{
		private:
			T* ptr;
		public:
			AutoPtr(T* ptr)
			{
				this->ptr = ptr;
			};
			AutoPtr() : ptr(0) {}
			Nil setPtr()
			{

			}
			virtual ~AutoPtr()
			{
				if (ptr)
					xlib::dispose<T>(ptr);
				ptr = 0;
			};
			AutoPtr& operator =(T* t)
			{
				ptr = t;
				return *this;
			};
			operator T*()
			{
				xlib_assert(ptr == 0, "The ptr is null at AutoPtr.");
				return ptr;
			}
			bool isNull() { return ptr == 0; }
		};
	}
	
};