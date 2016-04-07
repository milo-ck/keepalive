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
				if (node)
				{
					data::IBuffer* buff = null;
					if (!node->recver)
					{
						int len = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
						buff = data::IBuffer::create(len);
						node->recver = buff;
					}
					else
					{
						buff = static_cast<data::IBuffer*>(node->recver);
					}

					buff->writer()->writeBytes(data, numBytes);
				}
			};
			virtual Nil getSendData(Byte* data, Int lenOfData, Int* len, xlib::IDataEntry::UserTag tag)
			{
			};
		};
	};
};