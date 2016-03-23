#pragma once
#include "core/kastd.h"
#include "core/Config.hpp"
#include "core/Controller.hpp"
#include "core/msg/Message.hpp"
#include "core/entry/TCPEntry.hpp"
#include "core/entry/HTTPEntry.hpp"

namespace ka
{
	class Application
	{
	private:
		msg::Message* messager;
		entry::HTTPEntry* http;
		entry::TCPEntry* tcp;
	public:
		Application():http(null), tcp(null), messager(null)
		{
			xthread_attach();
			xlib::IDataEntry::load();
		}
		~Application()
		{
			xlib::IDataEntry::unload();
		}
		Nil setup()
		{
			if(messager)return;
			const Config::Alloc* alloc = Config::alloc();
			if (alloc->length > 0)
			{
				xlib::MemoryPool::pool()->clear();
				xlib::allocPool(alloc->size, alloc->num, alloc->initCount, alloc->length);
			}
			messager = new msg::Message();
			http = new entry::HTTPEntry();
			tcp = new entry::TCPEntry();
			
			core::Controller::add(messager);
			core::Controller::add(tcp);
			core::Controller::add(http);

			const Config::Threads* thread = Config::threads();
			messager->run(thread->message);
			tcp->run(1);
			http->run(1);
		};
		Nil exit()
		{
			core::u::unref(&messager);
			core::u::unref(&tcp);
			core::u::unref(&http);
		};
	};
}