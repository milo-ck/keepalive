#pragma once
#include <stdarg.h>
#include <ctime>
#include "xlib/types_.h"

namespace xlib
{
	class Log
	{
	public:
	    Nil static log(const Char* format, ...)
	    {
	    };
	    Nil static error(const Char* issue)
	    {
	    };
		Int static print(const Char* format, ...)
		{
			va_list args;
			va_start(args, format);
			Int cnt = vprintf(format, args);
			va_end(args);
			printf("\r\n");
			return(cnt);
		}
	};

}
#ifdef XDEBUG
namespace xlib
{
	namespace x__
	{
		inline Nil assert_(Boolean bok, const Char* file, Int line, const Char* format, ...)
		{
			if (bok)return;
			printf("[Assert] file %s, line %s \r\n", file, line);
			va_list args;
			va_start(args, format);
			int cnt = vprintf(format, args);
			va_end(args);
			printf("\r\n");
			getchar();
		}
		class Clocker_
		{
		private:
			clock_t time_;
		public:
			Clocker_() :time_(clock())
			{
			}
			void tick()
			{
				clock_t time = clock();
				Number diff = (Number)(time - time_) / (Number)CLOCKS_PER_SEC;
				Log::print("[Clock] %.4lf s.", diff);
				time_ = time;
			}
		};
	}

}

	#define xprint(format, ... ) xlib::Log::print(format, __VA_ARGS__)
	#define xassert(bok, format, ...) xlib::x__::assert_(bok, __FILE__, __LINE__, format, __VA_ARGS__)
	#define xclock_init() xlib::x__::Clocker_ __clocker__
	#define xclock() __clocker__.tick();
#else
	#define xprint(format, ...)
	#define xassert(bok, format, ...)
	#define xclock_init() 
	#define xclock() 
#endif // XDEBUG


