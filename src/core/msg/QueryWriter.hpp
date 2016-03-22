#pragma once
#include "xlib/xlib.h"
#include "core/kastd.h"
#include "core/msg/Header.h"
namespace ka
{
	namespace msg
	{
		//There have a trap. The device id has same value at diffrent group.
		class QueryWriter
		{
		private:
			IMessageWriter* writer_;
			QueryFlags flags_;
			enum  Status
			{
				None,
				Group,
				Device,
				Node,
				Completed
			};
			Status status;
			//
		public:
			QueryWriter(IMessageWriter* writer):status(None), writer_(writer)
			{
			}
			~QueryWriter()
			{
				writer_ = 0;
			}
		public:
			QueryWriter* writeFlags(UInt flags)
			{
				xthrow(status != None, error::IllegalCall, "A illegal calling is to write flags.");
				this->flags_ = flags;
				writer_->stream()->writeUInt(this->flags_);
				status = Group;
				return this;
			}

			QueryWriter* beginGroup(UShort num = 0)
			{
				xthrow(status != Group, error::IllegalCall,"A illegal calling is to begin group.");
				xassert(num > 0 && !flags_.has(QueryFlags::IsMultiGroup), "Can't to write the numGroup, because the flags is not contain the numGroup.");
				if (flags_.has(QueryFlags::IsMultiGroup))
				{
					writer_->stream()->writeUShort(num);
				}
				return this;
			};
			QueryWriter* writeGroup(const Char* name, UShort id)
			{
				xthrow(status != Group, error::IllegalCall, "A illegal calling is to write group.");
				if (!flags_.has(QueryFlags::IsAllGroup))
				{
					writeID(name, id);
				}
				else
				{
					xassert(true, "Can't to write the group, because the flags to defined to used the all group.")
				}
				return this;
			};
			QueryWriter* endGroup()
			{
				xthrow(status != Group, error::IllegalCall, "A illegal calling is to end group.");
				status = Device;
			};

			QueryWriter* beginDevice(UShort num = 0)
			{
				xthrow(status != Device, error::IllegalCall, "A illegal calling is to begin device.");
				xassert(num > 0 && !flags_.has(QueryFlags::IsMultiDevice), "Can't to write the numDevices, because the flags is not contain the numDevices.");
				if (flags_.has(QueryFlags::IsMultiDevice))
				{
					writer_->stream()->writeUShort(num);
				}
			};
			QueryWriter* writeDevice(const Char* name, UShort id)
			{
				xthrow(status != Device, error::IllegalCall, "A illegal calling is to write device.");
				if (!flags_.has(QueryFlags::IsAllDevice))
				{
					writeID(name, id);
				}
				else
				{
					xassert(true, "Can't to write the device, because the flags was defined to used to the all device.")
				}
			};
			QueryWriter* endDevice()
			{
				xthrow(status != Device, error::IllegalCall, "A illegal calling is to end device.");
				status = Node;
			};

			QueryWriter* node(UInt id2flags)
			{
				xthrow(status != Node, error::IllegalCall, "A illegal calling is to write node.");
				if (flags_.has(QueryFlags::IsUsingNodeID))
					id2flags &= 0xffff;
				writer_->stream()->writeUInt(id2flags);
				status = Completed;
			};
			
		private:
			Nil writeID(const Char* name, UShort id)
			{
				if (name == 0)
				{
					xassert(id == 0, "The invalid id.");
					writer_->stream()->writeByte(1);
					writer_->stream()->writeUShort(id);
				}
				else
				{
					writer_->stream()->writeByte(2);
					writer_->stream()->writeString(name);
				}
			};
		};
	};
};
