#pragma once
#include <vector>
#include <string>
#include "core/kastd.h"
namespace ka
{
	namespace msg
	{
		class DataStore
		{
		private:
			data::Array* groups;
			xlib::Counter totalNodes;
			xlib::Counter aliveNodes;
		public:
			DataStore()
			{}
			UInt getTotalNodes(){ return totalNodes; }
			UInt getAliveNodes(){ return aliveNodes; }
			data::Array* getGroups() { return groups; }
			data::Group* addGroup(const Char* name)
			{
				data::Iterator<data::Group> g(groups);
				while (g.next() && strcmp(g.current->name, name) == 0)
				{
					return g.current;
				}
				Int size = g.length(), size2;
				if (size2 = g.lock()->length() && size != size2)
				{
					g.reset();
					while (g.next(size) && strcmp(g.current->name, name) == 0)
					{
						g.unlock();
						return g.current;
					}
				}
				data::Group* ng = g.push(newGroup(size2 + 1, name));
				g.unlock();
				return ng;
			};
			data::Device* addDevice(data::Group* group, const Char* name)
			{
				if (group == 0)return 0;
				data::Iterator<data::Device> d(group->devices);
				while (d.next() && strcmp(d.current->name, name) == 0)
				{
					return d.current;
				}
				Int size = d.length(), size2;
				if (size2 = d.lock()->length() && size != size2)
				{
					d.reset();
					while (d.next(size) && strcmp(d.current->name, name) == 0)
					{
						d.unlock();
						return d.current;
					}
				}
				data::Device* nd = d.push(newDevice(size2 + 1, name));
				d.unlock();
				return nd;
			};
			data::Group* getGroup(UShort id)
			{
				data::Iterator<data::Group> it(groups);
				return it.at((Int)id - 1);
			}
			data::Device* getDevice(data::Group* g, UShort id)
			{
				if (g == 0)return 0;
				data::Iterator<data::Device> it(g->devices);
				return it.at((Int)id - 1);
			}
			data::Node* getNode(ka::id id)
			{
				data::Group* g = getGroup(data::u::getGroupID(id));
				if (g)
				{
					data::Device* d = getDevice(g, data::u::getDeviceID(id));
					if (d)
					{
						data::Iterator<data::Node> it(d->nodes);
						return it.at(data::u::getNodeID(id) - 1);
					}
						
				}
				return 0;
			}
			data::Node* addNode(data::Device* device, data::Node* newNode, const Char* name, UInt flags)
			{
				if (device == 0)return 0;
				data::Iterator<data::Node> n(device->nodes);
				while (n.next() && n.current == newNode)
				{
					data::Node* node = n.current;
					if (node->name)xlib::dealloc((Object)node->name);
					node->flags = flags;
					node->name = copyName(name);
					return n.current;
				}
				n.lock();
				if (totalNodes++ > 0xfffff)totalNodes = 0;
				if (aliveNodes++ > 0xfffff)aliveNodes = 0;
				
				newNode->flags = flags;
				newNode->name = copyName(name);
				newNode->id = (UShort)(Int)totalNodes;
				n.push(newNode);
				n.unlock();
				return newNode;
			};
			data::Node* removeNode(data::Node* node)
			{
				data::Group* g = getGroup(data::u::getGroupID(node->id));
				data::Device* device = getDevice(g, data::u::getDeviceID(node->id));
				if(device != 0)
				{
					data::Iterator<data::Node> n(device->nodes);
					n.lock();
					if (aliveNodes-- < 0)
						aliveNodes = 0;
					n.remove(node);
					n.unlock();
				}
				xlib::dealloc((Object)node->name);
				node->name = 0;
				xlib::dealloc(node);
			};
		private:
			inline Char* copyName(const Char* name)
			{
				if (name == 0)return 0;
				Int len = strlen(name);
				Char* ret = xlib::alloc<Char>(len + 1);
				ret[len] = '\0';
				strcpy(ret, name);
				return ret;
			}
			data::Array* newArray(Nil* content)
			{
				data::Array* arr = new data::Array();
				arr->content = content;
				arr->mutex = new xlib::Mutex();
				return arr;
			}
			data::Device* newDevice(UShort id, const Char* name)
			{
				data::Device* nd = xlib::alloc<data::Device>();
				if (nd == 0)
				{
					xlib::Log::error("Failed to create new device.");
					return nd;
				}
				nd->name = copyName(name);
				nd->id = id;
				nd->nodes = newArray(new std::vector<data::Device*>());
				return nd;
			}
			data::Group* newGroup(UShort id, const Char* name)
			{
				data::Group* ng = xlib::alloc<data::Group>();
				if (ng == 0)
				{
					xlib::Log::error("Failed to create new group.");
					return ng;
				}
				ng->name = copyName(name);
				ng->id = id;
				ng->devices = newArray(new std::vector<data::Device*>());
				return ng;
			}
			
		};
	};
};