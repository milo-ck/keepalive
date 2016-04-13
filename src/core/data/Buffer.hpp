#pragma once
#include "core/kastd.h"
#include "core/data/IBuffer.h"
#include "core/Config.hpp"
#include <fstream>
namespace ka
{
	namespace data
	{
		class Buffer : public IBuffer
		{
		private:
			class ID
			{
			private:
				xlib::Counter id_;
				static ID* i()
				{
					static ID i;
					return &i;
				}
			public:
				static Int getId()
				{
					ID* id = i();
					id->id_ ++;
					if(id->id_ >= xlib::Counter::Max)
						id->id_ = 0;
					return id->id_;
				}
			};

			UInt fid_;
			IBuffer* next_;
			core::AutoPtr<xlib::IStream>  stream_;
			core::AutoPtr<xlib::StreamWriter> writer_;
			core::AutoPtr<xlib::StreamReader> reader_;
			xlib::IStream* stream() { return stream_; }
		public:
			Buffer(Int len):fid_(0), next_(null)
			{
				if(len < 40960)
					stream_ = xlib::create<xlib::MemoryStream>(len);
				if(stream_.isNull())
				{
					fid_ = ID::getId();
					stream_ = xlib::create<xlib::FileStream>(Config::getTempPath("%d.buf", fid_).c_str());
				}
				stream()->length(len);
			}
		public:
			virtual xlib::StreamReader* reader()
			{
				if(reader_.isNull())
					reader_ = xlib::create<xlib::StreamReader>(stream());
				return reader_;
			};
			virtual xlib::StreamWriter* writer()
			{
				if(writer_.isNull())
					writer_ = xlib::create<xlib::StreamWriter>(stream());
				return writer_;
			};

			
			virtual Int length()				{ return stream()->length(); };
			virtual Int position()				{ return stream()->position(); };
			virtual Boolean position(Int pos)	{ return stream()->position(pos);};
			virtual Nil next(IBuffer* buff) { next_ = buff; };
			virtual IBuffer* next() { return next_; };
		protected:
			virtual Nil disposing()
			{
				if(fid_ > 0)
				{
					unlink(Config::getTempPath("%d.buf", fid_).c_str());
					fid_ = 0;
				}
				next_ = null;
			}
		};

		IBuffer* IBuffer::create(Int len)
		{
			return xlib::create<Buffer>(len);
		}
	};
};
