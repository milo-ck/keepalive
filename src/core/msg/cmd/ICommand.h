#pragma once
#include "core/kastd.h"

namespace ka
{
namespace msg
{
	namespace cmd
	{
		class ICommand
		{
		public:
			virtual bool execute() = 0;

		};
	}
}
}

