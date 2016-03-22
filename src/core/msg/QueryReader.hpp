#pragma once
#include "xlib/xlib.h"
#include "core/kastd.h"
#include "core/msg/IMessageReader.h"
namespace ka
{
	namespace msg
	{
		//There have a trap. The device id has same value at diffrent group.
		class QueryReader
		{
		private:
			UShort numGroups_;
			UShort numDevices_;
			UShort* deviceIDs_;
			UShort* groupIDs_;
			UInt node;
			QueryFlags flags_;
			IMessageReader* reader_;

			//
		public:
			QueryReader(IMessageReader* reader): reader_(reader), numGroups_(0), numDevices_(0), deviceIDs_(0), groupIDs_(0), node(0)
			{
			}
			~QueryReader()
			{
				if (groupIDs_ && flags_.has(QueryFlags::IsMultiGroup))delete[]groupIDs_;
				if (deviceIDs_ && flags_.has(QueryFlags::IsMultiDevice))delete[]deviceIDs_;
				deviceIDs_ = 0;
				groupIDs_ = 0;
				reader_ = 0;
			}
		public:
			//If obtian group value is null, please each all groups to get the correct value
			//This method has better performance than the each one group.
			data::Group* obtainGroup() const
			{
				if (flags_.has(QueryFlags::IsAllGroup) || flags_.has(QueryFlags::IsMultiGroup))return 0;
				return reader_->store()->getGroup((UShort)groupIDs_);
			};
			//same as obtianGroup
			data::Device* obtainDevice(data::Group* group) const
			{
				if (flags_.has(QueryFlags::IsAllDevice) || flags_.has(QueryFlags::IsMultiDevice))return 0;
				return reader_->store()->getDevice(group, (UShort)deviceIDs_);
			};

			bool contain(data::Group* group) const
			{
				if (flags_.has(QueryFlags::IsAllGroup))return true;
				if (flags_.has(QueryFlags::IsMultiGroup))
				{
					for (UShort i = 0; i < numGroups_; i++)
					{
						if (groupIDs_[i] == group->id)
							return true;
					}
					return false;
				}
				return (UShort)groupIDs_ == group->id;
			};
			bool contain(data::Device* device) const
			{
				if (flags_.has(QueryFlags::IsAllDevice))return true;
				if (flags_.has(QueryFlags::IsMultiDevice))
				{
					for (UShort i = 0; i < numDevices_; i++)
					{
						if (deviceIDs_[i] == device->id)
							return true;
					}
					return false;
				}
				return (UShort)deviceIDs_ == device->id;
			};
			bool contain(data::Node* node) const
			{
				if (flags_.has(QueryFlags::IsUsingNodeID))return this->node == node->id;
				return (this->node & node->flags) != 0;
			};
			Nil read()
			{
				flags_ = reader_->stream()->readUInt();
				if (flags_.has(QueryFlags::IsMultiGroup))
				{
					numGroups_ = reader_->stream()->readUShort();
					xassert(numGroups_ > 2000, "The numGroup(%d) is so big, maybe it's a corrupt data", numGroups_);
					groupIDs_ = xlib::alloc<UShort>(numGroups_);
					xthrow(groupIDs_ == 0, error::OutOfMemory, "Failed to create groupIDs_");
					
					for (UShort i = 0; i < numGroups_; i++)
					{
						groupIDs_[i] = readGroupID(reader_);
					}
				}
				else if (!flags_.has(QueryFlags::IsAllGroup))
				{
					groupIDs_ = (UShort*)readGroupID(reader_);
				}
				if (flags_.has(QueryFlags::IsMultiDevice))
				{
					numDevices_ = reader_->stream()->readUShort();
					xassert(numDevices_ > 2000, "The numDevices_(%d) is so big, maybe it's a corrupt data", numDevices_);
					deviceIDs_ = new UShort[numDevices_];
					xthrow(deviceIDs_ == 0, error::OutOfMemory, "Failed to create deviceIDs_");
					
					for (UShort i = 0; i < numDevices_; i++)
					{
						deviceIDs_[i] = readDeviceID(reader_);
					}
				}
				else if (!flags_.has(QueryFlags::IsAllDevice))
				{
					deviceIDs_ = (UShort*)readDeviceID(reader_);
				}
				node = reader_->stream()->readUInt();
			};
		private:
			UShort readDeviceID(IMessageReader* reader)
			{
				data::Group* group = obtainGroup();
				if (group)
				{
					return readDeviceID(reader,group);
				}
				data::Iterator<data::Group> it(reader->store()->getGroups());
				UShort id = 0;
				while (it.next())
				{
					if (contain(it.current))
					{
						id = readDeviceID(reader, it.current);
						if (id != 0)
							break;
					}
				}
				return id;
			};

			virtual UShort readGroupID(IMessageReader* reader)
			{
				Byte b = reader->stream()->readByte();
				switch (b)
				{
				case 1:
					return reader->stream()->readUShort();
				case 2:
				{
					data::Iterator<data::Group> g(reader->store()->getGroups());
					String name = reader-> stream()->readString();
					xthrow(name.size() <= 0, error::CorruptData, "Failed to read group name.");
					while (g.next() && name.compare(g.current->name) == 0)
					{
						return g.current->id;
					}
					return 0;
				}
				}
				xthrow(true, error::CorruptData, "Failed to get group id.");
			};
			virtual UShort readDeviceID(IMessageReader* reader, data::Group* group)
			{
				Byte b = reader->stream()->readByte();
				switch (b)
				{
				case 1:
					return reader->stream()->readUShort();
				case 2:
				{
					String name = reader->stream()->readString();
					xthrow(name.size() <= 0, error::CorruptData, "Failed to read device name.");
					data::Iterator<data::Device> d(group->devices);
					while (d.next() && name.compare(d.current->name) == 0)
					{
						return d.current->id;
					}
					return 0;
				}
				}
				xthrow(true, error::CorruptData, "Failed to get device id.");
			};
		};
	};
};
