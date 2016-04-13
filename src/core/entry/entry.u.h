#pragma once
#include "core/kastd.h"
#include "core/data/IBuffer.h"
namespace ka
{
namespace entry
{
	namespace u
	{
		data::IBuffer* makeBuff(data::Node* node)
		{

		};

		Int readLength(Byte* data)
		{
			return data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
		};
	}
}
}
