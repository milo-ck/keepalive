#pragma once
#include "core/entry/DataEntry.hpp"
#include "core/entry/HTTPBuffer.hpp"

namespace ka
{
	namespace entry
	{
		class TCPEntry : public DataEntry
		{
		private:
			Boolean obtainURL(data::IBuffer* buff, String& result)
			{
				if (buff->length() < 20)
					return false;
				Int pos = buff->position();
				buff->position(0);
				Int i = 0;
				Byte data[4];
				Int endPos = -1;
				while (i < pos)
				{
					if (buff->reader()->readBytes(data, 4) != 4)
						break;
					if (data[0] == '\r' && data[1] == '\n' && data[2] == '\r' && data[3] == '\n')
					{
						endPos = buff->position();
						break;
					}
					i += 4;
					if (i > pos)break;
				}
				if (endPos == -1)
				{
					buff->position(pos);
					return false;
				}
				result = "";
				String line;
				buff->position(0);
				i = 0;
				while (i > pos)
				{
					Byte b = buff->reader()->readByte();
					if (b == '\r')
					{
						Int hpos = -1;
						if (buff->reader()->readByte() == '\n' && (hpos = line.find(" HTTP")) != -1 && strncmp(line.c_str(), "GET ", 4) == 0)
						{
							result = line.substr(0, 4).substr(0, hpos);
							break;
						}
						line = "";
					}
					line += (Char)b;
					i++;
				}
				buff->leftShift(endPos, 0);
				return true;
			};
			data::IBuffer* createMsgBuff(String& url)
			{
				return HTTPBuffer::toMessageBuffer(url);
			};
			data::IBuffer* createResponseBuff(data::IBuffer* buff)
			{
				return HTTPBuffer::toResponseBuffer(buff);
			};
		protected:
			virtual data::IBuffer* onSend(data::Node* node, data::IBuffer* buff)
			{
				data::IBuffer* sndbuff = createResponseBuff(buff);
				buff->unref();
				return sndbuff;
			}
			virtual Nil onSent(data::Node* node, data::IBuffer* buff)
			{
				entry()->shutdown(node->tag);
			};
			virtual Nil onRecv(data::Node* node, Byte* data, Int numBytes)
			{
				if (!node->recvBuff)
					node->recvBuff = data::Buffer::create(1024);
				data::IBuffer* buff = static_cast<data::IBuffer*>(node->recvBuff);
				Int pos = -1;
				String get;
				while (obtainURL(buff, get))
				{
					data::IBuffer* sndbuf = createMsgBuff(get);
					m::OnBuffer onbuf(id(), core::IDMessage, sndbuf, node);
					onbuf.post();
					sndbuf->unref();
				}
				buff->writer()->writeBytes(data, numBytes);
			}

			virtual Byte getId() { return core::IDHTTPEntry; }
			virtual Nil startEntry()
			{
				DataEntry::entry()->start(Config::server()->httpPort, Config::threads()->http);
			}
		};
	}
}