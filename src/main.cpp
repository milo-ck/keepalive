#include <SDKDDKVer.h>

#include <stdio.h>
#include <tchar.h>

#include <iostream>
#include "core/Application.hpp"

ka::Application app;

Int main(Int argc, const char * argv[]) 
{
	app.setup();
	String line;
	while(true)
	{
		std::cin >> line;
		if(line.compare("exit"))
		{
			app.exit();
			break;
		}
	}
    return 0;
}
