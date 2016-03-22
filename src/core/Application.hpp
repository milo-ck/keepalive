#pragma once
#include "core/kastd.h"
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
		}
		Nil setup()
		{
			if(messager)return;

			messager = new msg::Message();
			http = new entry::HTTPEntry();
			tcp = new entry::TCPEntry();
			
			core::Controller::add(messager);
			core::Controller::add(tcp);
			core::Controller::add(http);

			messager->run(2);
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