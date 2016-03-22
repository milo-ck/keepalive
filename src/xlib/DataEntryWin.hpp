#pragma once

#include <Windows.h>
#include <winsock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")

#include "xlib/types_.h"
#include "xlib/IDataEntry.hpp"
#include "xlib/Exception.hpp"
#include "xlib/Thread.hpp"

#define DE_MAX_BUFF_SIZE 4096
namespace xlib
{
	class DataEntryWin : public IDataEntry
	{
	public:
		DataEntryWin(IDataEntry::Listener* listener) : sizeOfAddress_(sizeof(SOCKADDR_IN) + 16), listener_(listener), running_(false)
		{
		};
		virtual bool start(short port, short numThreads)
		{
			if (running_)return true;
			iocp_ = createIOCP(numThreads);
			port_ = createListener(port);
			initAcceptExFunctions();
			associateWithIOCP((HANDLE)port_, 0);
			for (Int i = 0; i < numThreads; i++)
			{
				xlib::Thread<DataEntryWin>::start(this, process, 0);
			}
			running_ = true;
			return running_;
		};
		virtual bool stop()
		{
			if (!running_)
				return true;
			running_ = false;
			closesocket(port_);
			port_ = 0;
			CloseHandle(iocp_);
			iocp_ = 0;
			return !running_;
		};
		virtual bool trySend(EntryTag tag)
		{
			ClientContext* client = (ClientContext*)tag;
			if (client->send.buff.len == 0)
			{
				onSent(client, &client->send, 10);
			}
			return true;
		};
	private:
		HANDLE iocp_;
		SOCKET port_;
		LPFN_ACCEPTEX acceptEx_;
		LPFN_GETACCEPTEXSOCKADDRS getAcceptExSockAddrs_;
		IDataEntry::Listener* listener_;
		Int sizeOfAddress_;
		bool running_;

	private:
		struct IOContext
		{
			OVERLAPPED overlapped;
			SOCKET client;
			WSABUF buff;
			char buff_[DE_MAX_BUFF_SIZE];
			IOContext()
			{
				ZeroMemory(&overlapped, sizeof(overlapped));
				client = INVALID_SOCKET;
				buff.len = DE_MAX_BUFF_SIZE;
				buff.buf = buff_;
			}
			Nil resetBuff()
			{
				ZeroMemory(buff_, DE_MAX_BUFF_SIZE);
			}
		};
		struct ClientContext
		{
			IOContext send;
			IOContext recv;
			IDataEntry::UserTag tag; //for custom use data
			ClientContext(SOCKET sock)
			{
				tag = 0;
				send.client = recv.client = sock;
				send.buff.len = 0;
			}

		};
		IDataEntry::Listener* listener() { return listener_; }

		Nil process(Nil* args)
		{
			ClientContext* client = 0;
			OVERLAPPED*	overlapped = NULL;
			while (running_)
			{
				DWORD numBytes = 0;
				
				BOOL ret = GetQueuedCompletionStatus(iocp_, &numBytes, (PULONG_PTR)&client, &overlapped, INFINITE);
				if (!ret)
				{
					DWORD eid = GetLastError();
					if (overlapped != NULL)
					{
						//Process a  failed completed I/O request.
						//eid contains the reason for failure.
					}
					else if (eid == WAIT_TIMEOUT)
					{
						//Time-out while waitting for completed I/O entry.
					}
					else
					{
						//Bad call to GetQueuedCompletionStatus
						//The eid contains the reason for the bad call.
					}
					if (running_)
					{
						listener()->onError(IDataEntry::E_GET_EPOOL_STATUS, "Failed to get queued completion status, eid=%d.", eid);
					}	
					continue;
				}
				IOContext* context = CONTAINING_RECORD(overlapped, IOContext, overlapped);
				if (client)
				{
					if (context == &client->send)
						onSent(client, context, numBytes);
					else if (context == &client->recv)
						onRecv(client, context, numBytes);
				}
				else
				{
					onAccept(context, numBytes);
				}
			}
		}

		Nil onSent(ClientContext* client, IOContext* context, Int numBytes)
		{
			Int len = context->buff.len - numBytes;
			if (len <= 0)
			{
				len = 0;
				listener()->getSendData((Byte*)context->buff_, DE_MAX_BUFF_SIZE,  &len, client->tag);
				context->buff.buf = context->buff_;
				context->buff.len = len;
			}
			else
			{
				context->buff.buf += numBytes;
				context->buff.len = len;
			}
			postSend(client, context);
		}
		Nil postSend(ClientContext* client, IOContext* context)
		{
			if (context->buff.len > 0)
			{
				WSASend(context->client, &context->buff, 1, 0, 0, &context->overlapped, NULL);
			}
		}
		Nil onRecv(ClientContext* client, IOContext* context, DWORD numBytes)
		{
			if (numBytes > 0)
			{
				listener()->onRecvData((Byte*)context->buff.buf, numBytes, client->tag);
			}
			postRecv(context);
		}
		Nil postRecv(IOContext* context)
		{
			context->resetBuff();
			DWORD dwFlags = 0;
			Int ret = WSARecv(context->client, &context->buff, 1, NULL, &dwFlags, &context->overlapped, NULL);
			if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
			{
				listener()->onError(IDataEntry::E_ON_POST_RECV, "Failed to post recv data from client.");
			}
		}
		Nil onAccept(IOContext* context, DWORD numBytes)
		{
			
			sockaddr_in* clientAddr = NULL;
			sockaddr_in* localAddr = NULL;
			Int lenOfLocal  = sizeof(sockaddr_in);
			Int lenOfClient = lenOfLocal;
			getAcceptExSockAddrs_(context->buff.buf, context->buff.len - sizeOfAddress_ * 2, sizeOfAddress_, sizeOfAddress_,(sockaddr**) &localAddr, &lenOfLocal,
				(sockaddr**)&clientAddr, &lenOfClient);
			
			ClientContext* ctx = new ClientContext(context->client);
			ctx->tag = listener()->onNewClient(clientAddr, (IDataEntry::EntryTag)ctx);
			associateWithIOCP((HANDLE)context->client, (ULONG_PTR)ctx);

			CopyMemory(ctx->recv.buff.buf, context->buff.buf, numBytes);
			onRecv(ctx, &ctx->recv, numBytes);
			postAccept(context);
		}
		Nil postAccept(IOContext* context)
		{
			context->resetBuff();
			context->client = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
			if (INVALID_SOCKET == context->client)
			{
				listener()->onError(IDataEntry::E_ON_POST_ACCEPT, "Falied to create socket for accept client.");
				return;
			}
			BOOL bok = acceptEx_(port_, context->client, context->buff.buf, context->buff.len - ((sizeOfAddress_) * 2),
				sizeOfAddress_, sizeOfAddress_, NULL, &context->overlapped);
			if (bok == FALSE && WSAGetLastError() == WSA_IO_PENDING)
			{
				listener()->onError(IDataEntry::E_ON_POST_ACCEPT, "Failed to post accept, the was io is pending.");
			}
		}
		HANDLE createIOCP(short numThreads)
		{
			HANDLE ret = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, numThreads);
			xthrow(ret == NULL, "Failed to create IO completion port.", IDataEntry::E_CREATEEPOLL);
			return ret;
		}
		Nil associateWithIOCP(HANDLE target, ULONG_PTR key)
		{
			xthrow(CreateIoCompletionPort(target, iocp_, key, 0) == NULL, "Failed to associate the handle with the IOCP.", IDataEntry::E_SET_NO_BLOCK);
		}
		SOCKET createListener(short port)
		{
			SOCKET so = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
			xthrow(so == INVALID_SOCKET, "Failed to create socket.", IDataEntry::E_CREATESOCKET);
			sockaddr_in address;
			ZeroMemory(&address, sizeof(sockaddr_in));
			address.sin_family = AF_INET;
			address.sin_addr.s_addr = INADDR_ANY;
			address.sin_port = htons(port);
			xthrow(bind(so, (sockaddr*)&address, sizeof(sockaddr_in)) == SOCKET_ERROR, "Failed to bind the socket.", IDataEntry::E_BINDSOCKET);
			xthrow(listen(so, SOMAXCONN) == SOCKET_ERROR, "Failed to listen the socket.", IDataEntry::E_LISTENSCKET);
			return so;
		}
		Nil initAcceptExFunctions()
		{
			GUID UIDAccpetEx = WSAID_ACCEPTEX;
			GUID UIDGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;

			DWORD bytes = 0;

			INT ret = WSAIoctl(port_, SIO_GET_EXTENSION_FUNCTION_POINTER, &UIDAccpetEx, sizeof(UIDAccpetEx), &acceptEx_, sizeof(acceptEx_), &bytes, NULL, NULL);
			xthrow(ret == SOCKET_ERROR, "Failed to get AcceptEx function ptr.", IDataEntry::E_UNKOWN);
			WSAIoctl(port_, SIO_GET_EXTENSION_FUNCTION_POINTER, &UIDGetAcceptExSockAddrs, sizeof(UIDGetAcceptExSockAddrs), &getAcceptExSockAddrs_, sizeof(getAcceptExSockAddrs_), &bytes, NULL, NULL);
			xthrow(ret == SOCKET_ERROR, "Falied to get GetAcceptExSockAddrs function ptr.", IDataEntry::E_UNKOWN);
			//get the AcceptEx and GetAcceptExSockAddrs ptr, use the ptr calling for better performance.
		}
	};

	IDataEntry* IDataEntry::create(IDataEntry::Listener* listener)
	{
		return new DataEntryWin(listener);
	}
	Nil IDataEntry::load()
	{
		WSADATA data;
		xthrow(NOERROR != WSAStartup(MAKEWORD(2, 2), &data), "Faild to startup to WSA.", IDataEntry::E_LOADSOCKET_LIB);
	}
	Nil IDataEntry::unload()
	{
		WSACleanup();
	}
}