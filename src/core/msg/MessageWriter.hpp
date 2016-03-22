#pragma once
#include "xlib/xlib.h"
#include "core/kastd.h"
#include "core/msg/IMessageWriter.h"

namespace ka
{
	namespace msg
	{
		class MessageWriter : public IMessageWriter
		{
		private:
			xlib::StreamWriter* writer_;
			core::AutoPtr<QueryWriter> query_;
			core::AutoPtr<Authentication> authentication_;
			core::AutoPtr<Header> header_;
			Int numBytes;
			Int bodyEos_;
			Header* getHeader()
			{
				Header* header = header_;
				xthrow(header == null, error::NullOfObject, "Can't got the header, Beacause it's null.");
				return header;
			}
		public:
			~MessageWriter()
			{
				writer_ = 0;
				bodyEos_ = 0;
			}
			MessageWriter(xlib::StreamWriter* writer)
				: writer_(writer), query_(0), authentication_(0), header_(0), bodyEos_(0)
			{
			};

			virtual Header* header(const Header* from = 0)
			{
				if (header_.isNull())
				{
					Header* header = xlib::create<Header>();
					header_ = header;
					if (from)
						utils::copy(header, from);
					else
						utils::zore(header);
				}
			};
			virtual Authentication* authentication()
			{
				if (authentication_.isNull())
				{
					Authentication* auth = xlib::create<Authentication>();
					authentication_ = auth;
					utils::zore(auth);
				}
				return authentication_;
			};


			virtual Int writeHeader() 
			{
				Header* header = getHeader();
				Int ret = writer_->writeUInt64(header->id);

				ret += writer_->writeByte(header->type);
				ret += writer_->writeByte(header->majorVersion);
				ret += writer_->writeByte(header->minorVersion);
				ret += writer_->writeByte(header->bodyType);

				ret += writer_->writeUInt(header->flags);
				ret += writer_->writeUInt64(header->from);
				ret += writer_->writeUInt64(header->to);
				ret += writer_->writeUInt(header->bodyLength);
				bodyEos_ = writer_->stream()->position();

				return ret;
			};
			virtual Int setBodyLength(UInt len)
			{
				xlib::IStream* stream = writer_->stream();
				Int pos = stream->position();
				stream->position(bodyEos_ - 4);
				Int ret = writer_->writeUInt(len);
				stream->position(pos);
				return ret;
			}
			virtual Int writeAuthentication()
			{
				Int ret = 0;
				Header* header = getHeader();
				Authentication* auth = authentication_;
				xthrow(auth == null, error::NullOfObject, "Can't got the authentication, Beacause it's null.");
				if (header->flags.has(HeaderFlags::IsUnlogin))
				{
					ret += writer_->writeString(auth->username.c_str());
					ret += writer_->writeString(auth->password.c_str());
					ret += writer_->writeString(auth->group.c_str());
					ret += writer_->writeString(auth->device.c_str());
					ret += writer_->writeString(auth->node.c_str());
					ret += writer_->writeUInt(auth->nodeFlags);
				}
				else
				{
					xthrow(true, error::IllegalCall, "Can't write the authentication, Beacause the header flags is invalid.");
				}
				return ret;
			};


			virtual const QueryWriter* query()
			{
				if (query_.isNull())
				{
					QueryWriter* writer = xlib::create<QueryWriter>(this);
					query_ = writer;
				}
				return query_;
			};
			virtual Int write(Byte buff[], Int len)
			{
				return writer_->writeBytes(buff, len);
			};

			virtual xlib::StreamWriter* stream() { return writer_; };
		};

		IMessageWriter* IMessageWriter::create(xlib::StreamWriter* writer)
		{
			return xlib::create<MessageWriter>(writer);
		}
	};
};


