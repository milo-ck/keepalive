#pragma once
namespace ka
{
	namespace entry
	{
		class HTTPBuffer
		{
		public:
			static data::IBuffer* toMessageBuffer(String url)
			{

				//http://server/token/group/action/params/?auth=username+password
				///company/push/group/g1,g2,g3/d1,d2,d3/flags/
				///company/push/id/gid-did-nid/
				///company/request/gid-did-nid/
				///company/query/g1,g2,g3/d1,d2,d3/flags/
				///company/status/
			};
			static data::IBuffer* toResponseBuffer(data::IBuffer* buff)
			{
			};
		};
	}
}
