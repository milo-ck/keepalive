#pragma once
#include "core/kastd.h"
#include "core/Controller.hpp"
#include "core/data/data.h"

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
				/*switch (p->method)
				{
				case m::OnBuffer::ID:
				{
					m::OnBuffer buf(p);
					cmd::BufferParser parser;
					parser.onBuffer(buf, store_);
					break;
				}
				case m::OnNodeClosed::ID:
				{
					m::OnNodeClosed node(p);
					cmd::NodeClosed nodeClosed(&node, store_);
					nodeClosed.execute();
					break;
				}
				}*/
			};
		public:
			virtual Nil run(Int numThreads)
			{
				entry = xlib::IDataEntry::create(this);
				entry->start(Config::server()->port, Config::threads()->tcp);
				core::IModule::run(numThreads);
			}
		public:
		public:
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
					Int numCached = node->data[0];
					Int numCopies = numCached + numBytes;
					numCopies = numCopies > 4 ? numCopies - numCached : numBytes;
					memcpy(&node->data[numCached + 1], data, numCopies);
					numBytes -= numCopies;
					data += numCopies;

					numCopies = numCopies + node->data[0];
					node->data[0] = numCopies;
					if (numCopies < 4)return;
					Byte* userData = node->data;
					int len = userData[0] << 24 | userData[1] << 16 | userData[2] << 8 | userData[3];
					buff = data::IBuffer::create(len);
					node->recvBuff = buff;
				}
				else 
				{
					buff = static_cast<data::IBuffer*>(node->recvBuff);
				}
				buff->writer()->writeBytes(data, numBytes);
			};
			virtual Nil getSendData(Byte* data, Int lenOfData, Int* len, xlib::IDataEntry::UserTag tag)
			{
			};
		};
	};
};