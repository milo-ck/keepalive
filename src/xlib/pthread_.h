#pragma once
#include "xlib/types_.h"
#ifdef XPLATFORM_WIN
#define _TIMESPEC_DEFINED
#define PTW32_STATIC_LIB
#pragma comment(lib, "pthread.lib")
#endif

#include <pthread.h>


#ifdef XPLATFORM_WIN
namespace xlib
{
	namespace x__
	{
		static Nil __cdecl  __pthread_detach(Nil)
		{
			pthread_win32_thread_detach_np();
			pthread_win32_process_detach_np();

		}
		void __pthread_attach()
		{
			pthread_win32_process_attach_np();
			pthread_win32_thread_attach_np();
			atexit(__pthread_detach);
		}
	}
}
#define xthread_attach() xlib::x__::__pthread_attach()
#else
#define xthread_attach()
#endif

