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
		virtual Nil		length(int v) = 0;
		virtual Boolean eos() = 0;
		virtual Int		read(Byte* data, Int len) = 0;
		virtual Int		write(const Byte* data, Int len) = 0;

		virtual Int		leftShift(Int from, Int to) = 0;
		

	};
}
