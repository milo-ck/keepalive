#pragma once
#include "xlib/types_.h"
#include "xlib/allocator.hpp"
#include "xlib/stream_.h"
#include "xlib/Exception.hpp"
#include <stdio.h>
#include <io.h>

namespace xlib
{
	class FileStream  : public IStream
	{
	private:
		Int position_;
		Int length_;
		FILE* file_;

	public:
		FileStream(const Char* path)
		{
			file_ = fopen(path, "wb+");
			xthrow(file_ == null, error::OpenFileFailed, "Can not open the file=%s.", path);
			fseek(file_, 0, SEEK_END);
			length_ = (Int)ftell(file_);
			fseek(file_, 0, SEEK_SET);
			position_ = 0;
		}
		virtual ~FileStream()
		{
			if(file_)fclose(file_);
			file_ = null;
		}

		virtual Int		position()			{ return position_;				}
		virtual Boolean position(Int pos)	
		{ 
			xthrow(file_ == null, error::InvalidFileStream, "Failed to seek stream.");
			if(position_ != pos)
			{
				position_ = pos;	
				fseek(file_, position_, SEEK_SET);
			}
			return true;
		}
		virtual Int		length()			{ return length_;				}
		virtual Boolean eos()				{ return position_ >= length_;	}
		virtual Nil		length(Int v)
		{
			xthrow(file_ == null, error::InvalidFileStream, "Failed to set length to file stream.");
			Int h = fileno(file_);
			chsize(h, v);
			length_ = v;
			
		}

		virtual Int		read(Byte* data, Int len)
		{
			xthrow(file_ == null, error::InvalidFileStream, "Failed to read stream.");
			Int ret = static_cast<Int>(fread((Object)data, 1, len, file_));
			if (ret > 0)
				position_ += ret;
			return ret;
		}
		virtual Int		write(const Byte* data, Int len)
		{
			xthrow(file_ == null, error::InvalidFileStream, "Failed to write stream.");
			Int ret = static_cast<Int>(fwrite(data, 1, len, file_));
			position_ += ret;
			length_ = position_ < length_ ? length_ : position_;
			return ret;
		}
	};
}