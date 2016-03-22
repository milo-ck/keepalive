#pragma once

#include "xlib/Log.hpp"
#include "xlib/allocator.hpp"
#include "xlib/FileStream.hpp"
#include "xlib/MemoryStream.hpp"
#include "xlib/StreamReader.hpp"
#include "xlib/StreamWriter.hpp"

class StreamCase
{
public:
	Nil initAllocator()
	{
		Int sizes[3]		= {1024, 2048, 4096};
		UShort nums[3]		 = {2,2, 2};
		UShort inits[3]		= {1, 1, 1};
		xlib::allocPool(sizes, nums, nums, 3);
	}
	Nil caseFileStream()
	{
		xlib::FileStream stream("C:\\Users\\Public\\fs.txt");
		xlib::StreamWriter sw(&stream);
		xprint("new file, len=%d, position=%d.", stream.length(), stream.position());
		xprint("Write string 'Hello, I am a farmer of coder.' and Int64 1020000000102 repeat to 10");
		for (Int i = 0; i < 10; i ++)
		{
			sw.writeString("Hello, I am a farmer of coder.");
			sw.writeInt64(1020000000102);
		}
		xprint("Wirte content to file, len=%d, position=%d.", stream.length(), stream.position());
		stream.position(0);
		xlib::StreamReader sr(&stream);
		for (Int i = 0; i < 10; i ++)
		{
			xprint("Read String %s, Read Int64 %lld", sr.readString().c_str(), sr.readInt64());
		}
	}
	Nil caseMemStream()
	{
		xlib::MemoryStream stream;
		xlib::StreamWriter sw(&stream);
		xprint("new file, len=%d, position=%d.", stream.length(), stream.position());
		xprint("Write string 'Hello, I am a farmer of coder.' and Int64 1020000000102 repeat to 10");
		for (Int i = 0; i < 10; i ++)
		{
			sw.writeString("Hello, I am a farmer of coder.Hello, I am a farmer of coder.Hello, I am a farmer of coder.Hello, I am a farmer of coder.Hello, I am a farmer of coder.Hello, I am a farmer of coder.Hello, I am a farmer of coder.Hello, I am a farmer of coder.Hello, I am a farmer of coder.Hello, I am a farmer of coder.Hello, I am a farmer of coder.");
			sw.writeInt64(1020000000102);
		}
		xprint("Wirte content to file, len=%d, position=%d.", stream.length(), stream.position());
		stream.position(0);
		xlib::StreamReader sr(&stream);
		for (Int i = 0; i < 10; i ++)
		{
			xprint("Read String %s, Read Int64 %lld", sr.readString().c_str(), sr.readInt64());
		}
	}
	Nil run()
	{
		initAllocator();
		caseFileStream();
		caseMemStream();
	}
};