#pragma once
#include "core/kastd.h"
#include "core/Controller.hpp"
#include "core/data/data.h"
#include "core/entry/entry.u.h"

namespace ka
{
	namespace entry
	{
		class TCPEntry : public core::IModule, public xlib::IDataEntry::Listener
		{
		public:
			virtual Byte id() { return core::IDTCPEntry; };
		private:
			xlib::IDataEntry* entry;
		protected:
			virtual Nil execute(const core::Parameter* p)
			{
				switch (p->method)
				{
				case m::OnBuffer::ID:
				{
					m::OnBuffer onbuf(p);
					data::Node* node = onbuf.node();
					data::IBuffer* buff = onbuf.buffer();
					buff->position(0);
					if (node->sendBuff)
					{
						data::IBuffer* current = static_cast<data::IBuffer*>(node->sendBuff);
						current->next(buff);
					}
					else
					{
						node->sendBuff = buff;
						entry->trySend(node->tag);
					}
					break;
				}
				default:
					break;
				}
			};
		public:
			virtual Nil run(Int numThreads)
			{
				entry = xlib::IDataEntry::create(this);
				entry->start(Config::server()->port, Config::threads()->tcp);
				core::IModule::run(numThreads);
			}
		public:
			virtual Nil onCloseClient(xlib::IDataEntry::UserTag tag)
			{
				data::Node* node = static_cast<data::Node*>(tag);
				xthrow(node == null, error::CorruptData, "Failed to convert xlib::IDataEntry::UserTag to data::Node.");
				
				data::IBuffer* buff = static_cast<data::IBuffer*>(node->sendBuff);
				buff = buff ? buff->next() : null;
				while (buff)
				{
					data::IBuffer* next = buff->next();
					buff->unref();
					buff = next;
				}
				core::u::unref(&node->recvBuff);
				core::u::unref(&node->sendBuff);

				m::OnNodeClosed onClose(id(), core::IDMessage,  node);
				onClose.post();
			}
			virtual Nil onError(xlib::IDataEntry::ErrorCode code, const char* foramt, ...)
			{
			};
			virtual xlib::IDataEntry::UserTag onNewClient(const sockaddr_in* addr, xlib::IDataEntry::EntryTag tag)
			{
				data::Node* node = data::u::newNode();
				node->tag = tag;
				memcpy(node->ip, &addr->sin_addr.S_un.S_addr, 4);
				return node;
			};
			virtual Nil onRecvData(Byte* data, Int numBytes, xlib::IDataEntry::UserTag tag)
			{
				data::Node* node = static_cast<data::Node*>(tag);
				xthrow(node == null, error::CorruptData, "Failed to convert xlib::IDataEntry::UserTag to data::Node.");
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
						onRecvData(data, numBytes, tag);
					return;
				}
				buff->writer()->writeBytes(data, numBytes);

			};
			virtual Nil getSendData(Byte* data, Int lenOfData, Int* len, xlib::IDataEntry::UserTag tag)
			{
				data::Node* node = static_cast<data::Node*>(tag);
				xthrow(node == null, error::CorruptData, "Failed to convert xlib::IDataEntry::UserTag to data::Node.");
				data::IBuffer* buff = static_cast<data::IBuffer*>(node->sendBuff);
				xthrow(buff == null, error::CorruptData, "Failed to get sent buff.");

				Int numSent = buff->length() - buff->position();
				if (numSent > lenOfData)
				{
					*len = buff->reader()->readBytes(data, lenOfData);
				}
				else
				{
					*len = buff->reader()->readBytes(data, numSent);
				}
				if (buff->length() <= buff->position())
				{
					data::IBuffer* next = buff->next();
					node->sendBuff = next;
					buff->next(null);
					buff->unref();
				}
			};
		};
	};
};