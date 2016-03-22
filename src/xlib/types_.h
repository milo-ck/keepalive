#pragma once
#include <string>
typedef void* Object;
typedef void Nil;
typedef bool Boolean;
typedef char Char;
typedef unsigned char Byte;
typedef short Short;
typedef unsigned short UShort;
typedef int Int;
typedef unsigned int UInt;
typedef long long Int64;
typedef unsigned long long UInt64;
typedef double Number;

typedef std::string String;
#define null 0

namespace xlib
{
	namespace error
	{
		enum ErrorCode	
		{
			OutOfRange,
			OutOfMemory,
			OpenFileFailed,
			InvalidFileStream,
			NullOfObject,
		};
	}
}

