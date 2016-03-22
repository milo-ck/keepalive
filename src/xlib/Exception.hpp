#pragma once
#include <exception>

//todo
//#define	xassert(bok, format)	if(bok){ }
#define xthrow(bok, code, format, ...)	if(bok){throw xlib::Exception(format, code,  __FILE__, __LINE__);}
namespace xlib
{
	class Exception : public std::exception
	{
	private:
		Int code_;
		Int line_;
		const char* file_;
	public:
		Exception(const char* msg, Int code, const char* file, Int line) 
			:std::exception(msg), code_(code), line_(line), file_(file)
		{
		}
		const char* text() { return what(); }
		Int code() { return code_; }
		const char* file() { return file_; }
		Int line() { return line_;  }

	};	
};
