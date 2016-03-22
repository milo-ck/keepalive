#pragma once
#include "core/kastd.h"

namespace ka
{
	namespace msg
	{
		namespace def
		{
			enum SystemType
			{
				QueryNodes,
				QueryStatus,
				QueryID,
				KeepAlive,
				TargetOffline,
			};
			enum BodyType
			{
				Text,
				Xml,
				Jpg,
				Png,
				Zip,
				Bin,
			};
		}

		class QueryFlags
		{
		private:
			UInt flags;
		public:
			enum Flags
			{
				IsAllGroup = 1,
				IsMultiGroup = 2,
				IsAllDevice = 4,
				IsMultiDevice = 8,
				IsUsingNodeID = 16,
			};
			QueryFlags(UInt flags):flags(flags)
			{
			};
			QueryFlags() : flags(0)
			{
			};
			operator UInt()
			{
				return flags;
			}
			QueryFlags& operator = (UInt flags)
			{
				this->flags = flags;
				return *this;
			}
			bool has(Flags f) const { return (flags & f) == f; }
			QueryFlags* set(Flags f)
			{
				flags |= f;
				return this;
			}

		};
		class HeaderFlags
		{
		private:
			UInt flags;
		public:
			enum Flags
			{
				IsSingleTarget,
				IsMultiTarget,
				IsUnlogin,
				IsSystem,
				IsRequest,
				IsResponse,
			};
			HeaderFlags(UInt flags) :flags(flags)
			{
			};
			HeaderFlags() : flags(0)
			{
			};
			operator UInt() const
			{
				return flags;
			}
			HeaderFlags& operator = (UInt flags)
			{
				this->flags = flags;
				return *this;
			}
			bool has(Flags f) const { return (flags & f) == f; }
			HeaderFlags* set(Flags f)
			{
				flags |= f;
				return this;
			}
		};
		struct Authentication
		{
			String username;
			String password;
			String group;
			String device;
			String node;
			UInt nodeFlags;
		};
		
		struct Header
		{
			ka::id id;
			Byte type;
			Byte majorVersion;
			Byte minorVersion;
			Byte bodyType;
			HeaderFlags flags;
			mutable ka::id from;
			ka::id to;
			Nil setFrom(ka::id fid) const
			{
				from = fid;
			}
			UInt bodyLength;
		};

		namespace utils
		{
			inline Nil copy(Header* to, const Header* from)
			{
				to->id = from->id;

				to->type = from->type;
				to->majorVersion = from->majorVersion;
				to->minorVersion = from->minorVersion;
				to->bodyType = from->bodyType;
				to->bodyLength = from->bodyLength;
				to->flags = (UInt)from->flags;
				to->from = from->from;
				to->to = from->to;
			};
			inline Nil zore(Header* h)
			{
				h->id = 0;

				h->type = 0;
				h->majorVersion = 0;
				h->minorVersion = 0;
				h->bodyType = def::Bin;
				h->bodyLength = 0;
				h->flags = 0;
				h->from = 0;
				h->to = 0;
			}
			inline Nil zore(Authentication* auth)
			{
				auth->username = "";
				auth->password = "";
				auth->group = "";
				auth->device = "";
				auth->node = "";
				auth->nodeFlags = 0;
			}
		};

	};
	
};
