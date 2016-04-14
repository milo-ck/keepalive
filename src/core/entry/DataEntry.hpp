#pragma once
#include "core/kastd.h"
#include "core/Controller.hpp"
#include "core/data/data.h"
#include "core/entry/entry.u.h"
#include "core/m/OnBuffer.hpp"
#include "core/m/OnNodeClosed.hpp"

namespace ka
{
	namespace entry
	{
		class DataEntry : public core::IModule, public xlib::IDataEntry::Listener
		{
		public:
			virtual Byte id() { return getId(); };
		protected:
			virtual data::IBuffer* onSend(data::Node* node, data::IBuffer* buff) = 0;
			virtual Nil onSent(data::Node* node, data::IBuffer* buff) = 0;
			virtual Nil onRecv(data::Node* node, Byte* data, Int numBytes) = 0;

			virtual Byte getId() = 0;
			virtual Nil startEntry() = 0;
			xlib::IDataEntry* entry() { return entry_; }
			
		private:
			xlib::IDataEntry* entry_;

			
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
					buff = onSend(node, buff);
					if (node->sendBuff)
					{
						data::IBuffer* current = static_cast<data::IBuffer*>(node->sendBuff);
						current->next(buff);
					}
					else
					{
						node->sendBuff = buff;
						entry_->trySend(node->tag);
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
				entry_ = xlib::IDataEntry::create(this);
				entry_->start(Config::server()->port, Config::threads()->tcp);
				startEntry();
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

				m::OnNodeClosed onClose(id(), core::IDMessage, node);
				onClose.post();
			}
			virtual Nil onError(xlib::IDataEntry::ErrorCode code, const char* format, ...)
			{
				va_list args;
				va_start(args, format);
				xlib::Log::errorv(format, args);
				va_end(args);
			}

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
				onRecv(node, data, numBytes);
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
					onSent(node, buff);
					buff->unref();
				}
			};
		};
	}
}