#pragma once
#include "xlib/types_.h"
#include "xlib/stream_.h"
#include "xlib/Exception.hpp"

namespace xlib
{
    class StreamReader
    {
    private:
		IStream* buff_;
		Boolean littleEndian_;
		IStream* buff()
		{
			xthrow(buff_ == 0, error::NullOfObject, "Can not get the buff.");
			return buff_;
		}

    public:
		StreamReader(IStream* buff) : buff_(buff), littleEndian_(u::isLittleEndian())
        {
        };
        ~StreamReader()
        {
			buff_ = 0;
        };
		IStream* stream(){ return buff(); }
		Char readChar()
		{
			Byte b = 0;
			buff()->read(&b, 1);
			return b;
		}
		Byte readByte()
		{
			return (Byte)readChar();
		}
		Short readShort()
		{
			Byte b[2] = { 0 };
			buff()->read(b, 2);
			Short ret;
			if (!littleEndian_)u::invertBytes(b, 2);
			memcpy(&ret, b, 2);
			return ret;
		}
		UShort readUShort()
		{
			return (UShort)readShort();
		}
		Int readInt()
		{
			Byte b[4] = { 0 };
			buff()->read(b, 4);
			Int ret;
			if (!littleEndian_)u::invertBytes(b, 4);
			memcpy(&ret, b,  4);
			return ret;
		}
		UInt readUInt()
		{
			return (UInt)readInt();
		}
		Int64 readInt64()
		{
			Byte b[8] = { 0 };
			buff()->read(b, 8);
			Int64 ret;
			if (!littleEndian_)u::invertBytes(b, 8);
			memcpy(&ret, b, 8);
			return ret;
		}
		UInt64 readUInt64()
		{
			return (UInt64)readInt64();
		}
		String readString()
		{
			String ret;
			if (!buff()->eos())
			{
				while (!buff_->eos())
				{
					Byte b;
					buff_->read(&b, 1);
					if (b == 0 || b == '\0')
						break;
					ret += (Char)b;
				}
			}
			return ret;
		}
		Boolean readBoolean()
		{
			return readByte() == 1;
		}
		Int readBytes(Byte* out, Int len)
		{
			return buff()->read(out, len);
		}
    };
};

