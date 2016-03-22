#pragma once
#include "xlib/types_.h"
namespace xlib
{
	class IStream
	{
	public:
		virtual Int		position() = 0;
		virtual Boolean position(Int pos) = 0;
		virtual Int		length() = 0;
		virtual Boolean eos() = 0;
		virtual Int		read(Byte* data, Int len) = 0;
		virtual Int		write(const Byte* data, Int len) = 0;
		

	};
	namespace u
	{
		inline Boolean isLittleEndian()
		{
			union 
			{
				Short s;
				Byte b[2];
			}a;
			a.s = 0x0102;
			return (a.b[0] == 2 && a.b[1] == 1);
		}
		inline Nil invertBytes(Byte* src, Int num)
		{
			Int len = num / 2;
			for (Int l = 0, r = num - 1; l < len; l++, r--)
			{
				Byte temp = src[r];
				src[r] = src[l];
				src[l] = temp;
			}
		}
	}
}
