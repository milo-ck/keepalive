#pragma once
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/stat.h>

#include "xlib/IDataEntry.hpp"
#include "xlib/Exception.hpp"
#include "xlib/Thread.hpp"

#define DE_MAX_BUFF_SIZE 1024

namespace xlib
{
	class DataEntryLinux : public IDataEntry
	{
	public:
		struct Buff
		{
			char* buff;
			Int len;
		};
		struct ClientContext
		{
			Int so;
			Buff send;
			char buff_[DE_MAX_BUFF_SIZE];
			UserTag tag;
			ClientContext()
			{
				send.len = 0;
				send.buff = buff_;
			}
		};

		virtual bool start(short port, short numThreads)
		{
			if (running_)return running_;
			fdep_ = createEpoll();
			fdso_ = createListener(port);
			for (Int i = 0; i < numThreads; i++)
			{
				xlib::Thread<DataEntryLinux>::start(this, process, 0);
			}
			running_ = true;
			return running_;
		};
		virtual bool stop()
		{
			if (!running_)
				return true;
			running_ = false;
			close(fdso_);
			fdso_ = 0;
			close(fdep_);
			fdep_ = 0;
			return !running_;
		};
		virtual bool trySend(EntryTag tag)
		{
			struct ClientContext* context = (ClientContext*)tag;
			if (context->send.len == 0)
				onSent(context);
		};
		DataEntryLinux(IDataEntry::Listener* listener) :listener_(listener), running_(false)
		{
		}
	private:
		IDataEntry::Listener* listener_;
		bool running_;
		Int  fdep_;
		Int  fdso_;
		IDataEntry::Listener* listener() { return listener_; }

		
		Nil process()
		{
			struct epoll_event events[1];
			while (running_)
			{
				Int num = epoll_wait(fdep, events, 1, 500);
				//500 is timeout
				if (num <= 0)
					continue;
				Int event = events[0].events;
				Int fd = events[0].data.fd;
				if ((event & EPOLLERR) || (event & EPOLLHUP))
				{
					//on close
					continue;
				}
				if (fd == fdso_)
				{
					onAccept();
				}
				if (event & EPOLLIN)
				{
					onRecv((ClientContext*)events[0].data.ptr);
				}
				if (event & EPOLLOUT)
				{
					onSent((ClientContext*)events[0].data.ptr);
				}
			}

		}
		Nil onAccept()
		{
			struct sockaddr_in addr;
			struct epoll_event ev;
			struct IDataClient client;

			socklen_t len = sizeof(addr);

			while (running_)
			{
				Int so = accept(fdso_, (struct sockaddr*)&addr, &len);
				if (so < 0)
				{
					if ((errno == EAGAIN) || errno == EWOULDBLOCK)
						break;
					break;
				}
				associateUnblock(so);
				ev.data.fd = so;
				ev.events = EPOLLIN | EPOLLET;
				ClientContext* context = new ClientContext();
				context->tag = listener()->onNewClient(&addr, (Nil*)so);
				ev.data.ptr = context;
				//the associated file is available for read operations;
				//sets the edge triggers behavior for the associated file descriptor
				epoll_ctl(fdep, EPOLL_CTL_ADD, client, &ev);
				continue;
			}	
		}
		Nil onRecv(struct ClientContext* context)
		{
			char buff[1024];
			Int numReaded = 0;
			while (running_ && (numReaded = read(context->so, buff, 1024)) > 0)
			{
				listener()->onRecvData((Byte*)buff, numReaded, context->tag);
			}
		}
		bool send(struct ClientContext* context)
		{
			Int len = context->send.len;
			while (running_ && len > 0)
			{
				Int num = write(context->so, context->send.buff, len);
				if (num == -1)
				{
					if (errno != EAGAIN)
					{
						//do close
						return false;
					}
					break;
				}
				len -= num;
				context->send.buff += num;
			}
			if (len <= 0)
			{
				context->send.buff = context->buff_;
				len = 0;
			}
			context->send.len = len;
			return true;
		}
		Nil onSent(struct ClientContext* context)
		{
			if (!send(context))return;
			do
			{
				Int len = 0;
				listener()->getSendData((Byte*)context->buff_, DE_MAX_BUFF_SIZE, &len, context->tag);
				if (len == 0)
				{
					break;
				}
				context->send.len = len;
			} while (running_ && send(context));
		}
		Nil associateUnblock(Int so)
		{
			Int opts = fcntl(so, F_GETFL);//get the socket access mode and the file status flags;
			xthrow(opts < 0, "Bad call to fcntl to get fl", IDataEntry::E_SET_NO_BLOCK);
			opts |= O_NONBLOCK;
			xthrow(fcntl(so, F_SETFL, opts) < 0, "Bad call to fcntl to set fl.", IDataEntry::E_SET_NO_BLOCK);
		}
		Int createListener(short port)
		{
			Int so = socket(AF_INET, SOCK_STREAM, 0);
			xthrow(so == -1, "Falied to create socket.", IDataEntry::E_CREATESOCKET);

			struct epoll_event ev;
			ev.data.fd = so;
			ev.data.ptr = 0;
			ev.events = EPOLLIN | EPOLLET;
			//EPOLLIN The associated file is available for read operations;
			//EPOLLET Set the edge triggered behavior for the associated file descriptor.
			epoll_ctl(fdep, EPOLL_CTL_ADD, so, &ev);

			struct sockaddr_in addr;
			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.addr.s_addr = htonl(INADDR_ANY);
			addr.sin_port = htons(port);
			xthrow(bind(so, (struct sockaddr*)&addr, sizeof(addr)) == -1, "Falied to bind socket.", IDataEntry::E_BINDSOCKET);
			xthrow(listen(so, 20) == -1, "Falied to listen socket.", IDataEntry::E_LISTENSCKET);
			return so;
		}
		Int createEpoll()
		{
			Int fdep = epoll_create(256);
			return fdep;
			//the size argument has been avaliable since the linux 2.6.8 published, but must be greater than zore.
		};
	};

	IDataEntry* IDataEntry::create(IDataEntry::Listener* listener)
	{
		return new DataEntryLinux(listener);
	}
	Nil IDataEntry::load(){}
	Nil IDataEntry::unload(){}
};
