#pragma once
#include "core/entry/DataEntry.hpp"

namespace ka
{
	namespace entry
	{
		class TCPEntry : public DataEntry
		{
		protected:
			virtual data::IBuffer* onSend(data::Node* node, data::IBuffer* buff)
			{

				return buff; 
			}
			virtual Nil onSent(data::Node* node, data::IBuffer* buff) {};
			virtual Nil onRecv(data::Node* node, Byte* data, Int numBytes)
			{
				//http://server/token/group/action/params/?auth=username+password
				///company/push/group/g1,g2,g3/d1,d2,d3/flags/
				///company/push/id/gid-did-nid/
				///company/request/gid-did-nid/
				///company/query/g1,g2,g3/d1,d2,d3/flags/
				///company/status/
				data::IBuffer* buff = null;
				if (!node->recvBuff)
				{
					Byte* userData = node->data;
					Int num = userData[0];
					Byte* dest = &userData[num + 1];
					while (numBytes > 0)
					{
						*dest = *data;
						dest++;
						data++;
						num++;
						numBytes--;
						if (num >= 4)break;
					}
					userData[0] = num;
					if (num < 4)return;
					Int len = entry::u::readLength(&userData[1]);
					userData[0] = 0;
					buff = data::IBuffer::create(len);
					node->recvBuff = buff;
				}
				else
				{
					buff = static_cast<data::IBuffer*>(node->recvBuff);
				}
				if (buff->position() + numBytes >= buff->length())
				{
					Int numWrite = buff->length() - buff->position();
					buff->writer()->writeBytes(data, numWrite);

					m::OnBuffer onbuf(id(), core::IDMessage, buff, node);
					onbuf.post();

					buff->unref();
					node->recvBuff = null;
					numBytes -= numWrite;
					data += numWrite;
					if (numBytes > 0)
						onRecv(node, data, numBytes);
					return;
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