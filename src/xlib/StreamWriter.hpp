#pragma once
#include "xlib/types_.h"
#include "xlib/stream_.h"
#include "xlib/Exception.hpp"

namespace xlib
{
    class StreamWriter
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
		StreamWriter(IStream* buff) : buff_(buff), littleEndian_(u::isLittleEndian())
		{
		};
		~StreamWriter()
		{
			buff_ = 0;
		};
		IStream* stream(){ return buff(); }
		Boolean writeChar(Char b)
		{
			return writeByte((Byte)b);
		}
		Boolean writeByte(Byte b)
		{
			return buff()->write(&b, 1) == 1;
		}
		Boolean writeShort(Short i)
		{
			Byte b[2] = { 0 };
			memcpy(b, &i, 2);
			if (!littleEndian_)u::invertBytes(b, 2);
			return buff()->write(b, 2) == 2;
		}
		Boolean writeUShort(UShort i)
		{
			return writeShort((Short)i);
		}
		Boolean writeInt(Int i)
		{
			Byte b[4] = { 0 };
			memcpy(b, &i, 4);
			if (!littleEndian_)u::invertBytes(b, 4);
			return buff()->write(b, 4) == 4;
		}
		Boolean writeUInt(UInt i)
		{
			return writeInt((Int)i);
		}
		Boolean writeInt64(Int64 i)
		{
			Byte b[8] = { 0 };
			memcpy(b, &i, 8);
			if (!littleEndian_)u::invertBytes(b, 8);
			return buff()->write(b, 8) == 8;
		}
		Boolean writeUInt64(UInt64 i)
		{
			return writeInt64((Int64)i);
		}
		Boolean writeString(const String& s)
		{
			Int len = static_cast<Int>(s.length());
			return buff()->write((Byte*)s.c_str(), len) && writeByte(0);
		}
		Boolean writeBoolean(Boolean b)
		{
			return writeByte((Byte)b);
		}
		Int writeBytes(const Byte* out, Int len)
		{
			return buff()->write(out, len);
		}
    };
};

