#pragma once
namespace xlib
{
	class IDataEntry
	{
	public:
		typedef Nil* EntryTag;
		typedef Nil* UserTag;
		enum ErrorCode
		{
			E_LOADSOCKET_LIB,
			E_CREATESOCKET,
			E_BINDSOCKET,
			E_LISTENSCKET,
			E_CREATEEPOLL,
			E_SET_NO_BLOCK,
			E_UNKOWN,
			E_ON_POST_ACCEPT,
			E_ON_POST_RECV,
			E_GET_EPOOL_STATUS
		};
		class Listener
		{
		public:
			virtual Nil onError(ErrorCode code, const char* foramt, ...) = 0;
			virtual UserTag onNewClient(const struct sockaddr_in* addr, EntryTag tag) = 0;
			virtual Nil onRecvData(Byte* data, Int numBytes, UserTag tag) = 0;
			virtual Nil getSendData(Byte* data, Int lenOfData, Int* len, UserTag tag) = 0;
			virtual Nil onCloseClient(UserTag tag) = 0;
		};

		static IDataEntry* create(IDataEntry::Listener* listener);
		static Nil load();
		static Nil unload();

		virtual bool start(short port, short numThreads) = 0;
		virtual bool stop() = 0;
		virtual bool trySend(EntryTag tag) = 0;
	};
}