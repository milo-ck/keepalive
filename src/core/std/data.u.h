#pragma once
#include "xlib/xlib.h"
namespace ka
{
	namespace data
	{
		namespace u
		{
			inline UInt getGroupID(id v)
			{
				return (UInt)((v >> 32) & 0xffff);
			};
			inline UInt getDeviceID(id v)
			{
				return (UInt)((v >> 16) & 0xffff);
			};
			inline UInt getNodeID(id v)
			{
				return (UInt)(v & 0xffff);
			};
			inline id makeID(UInt gid, UInt did, UInt aid)
			{
				return (id)(((gid & 0xfff) << 32) | (did & 0xffff) << 16 | (aid & 0xffff));
			};
			inline data::Node* newNode()
			{
				data::Node* node = new data::Node();
				memset(node, 0, sizeof(data::Node));
				return node;
			}
		};
	}
}