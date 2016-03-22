#pragma once
#include "xlib/xlib.h"
#include "core/kastd.h"
#include "core/msg/IMessageReader.h"

namespace ka
{
	namespace msg
	{
		class MessageReader : public IMessageReader
		{
		private:
			xlib::StreamReader* reader_;
			DataStore* store_;
			core::AutoPtr<QueryReader> query_;
			core::AutoPtr<Authentication> authentication_;
			core::AutoPtr<Header> header_;
			Int numBytes;
			Int bodypos;
		public:
			~MessageReader()
			{
				reader_ = 0;
				store_ = 0;
			}
			
			MessageReader(xlib::StreamReader* reader, DataStore* store)
				: reader_(reader), store_(store)
			{
				Header* header = xlib::create<Header>();
				header->id = reader_->readUInt64();

				header->type = reader_->readByte();
				header->majorVersion = reader_->readByte();
				header->minorVersion = reader_->readByte();
				header->bodyType = reader_->readByte();
				header->flags = reader_->readUInt();
				header->from = reader_->readUInt64();
				header->to = reader_->readUInt64();
				header->bodyLength = reader_->readUInt();

				header_ = header;
				if (header->flags.has(HeaderFlags::IsUnlogin))
				{
					Authentication* auth = xlib::create<Authentication>();
					auth->username = reader_->readString();
					auth->password = reader_->readString();
					auth->group = reader_->readString();
					auth->device = reader_->readString();
					auth->node = reader_->readString();
					auth->nodeFlags = reader_->readUInt();
					authentication_ = auth;
				}
				if (header->flags.has(HeaderFlags::IsMultiTarget))
				{
					QueryReader* query =  xlib::create<QueryReader>(this);
					query->read();
					query_ = query;
				}
				numBytes = header->bodyLength;
				bodypos = stream()->stream()->position();
			};
			

			virtual const Header*			header() { return header_; };
			virtual const QueryReader*	query() { return query_; };
			virtual const Authentication*	authentication() { return authentication_; };
			virtual Int read(Byte buff[], Int len)
			{
				xassert(numBytes <= 0, "There is no more bytes to read.");
				Int ret = reader_->readBytes(buff, len);
				if (ret > 0)
					numBytes -= ret;
				return ret;
			};
			virtual void gotoBodyPosition() 
			{
				stream()->stream()->position(bodypos);
				numBytes = header()->bodyLength;
			}
			virtual Int byteAvaliable() { return numBytes; };


			virtual xlib::StreamReader* stream() { return reader_; };
			virtual DataStore* store() { return store_; };
		};

		IMessageReader* IMessageReader::create(xlib::StreamReader* reader, DataStore* store)
		{
			return xlib::create<MessageReader>(reader, store);
		}
	};
};


