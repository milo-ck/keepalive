// UnitCase.cpp : Defines the entry point for the console application.
//
#include <SDKDDKVer.h>

#include <stdio.h>
#include <tchar.h>

#include <iostream>
#pragma comment(lib, "pthread.lib")
#include "mempool/MemoryPoolCase.hpp"
#include "stream/StreamCase.hpp"

class A
{
public:
	A()
	{
		std::cout << "A()" << std::endl;
	}
	virtual ~A()
	{
		std::cout << "~A()" << std::endl;
	}
};
class B : public A
{
public:
	B()
	{
		std::cout << "B()" << std::endl;
	}
	/*~B()
	{
		std::cout << "~B()" << std::endl;
	}*/
};
class C : public B
{
public:
	C()
	{
		std::cout << "C()" << std::endl;
	}
	~C()
	{
		std::cout << "~C()" << std::endl;
	}
};
int main()
{
	xthread_attach();
	std::cout << "Start unit case test." << std::endl;
	B* aa = new C();
	delete aa;
	/*MemoryPoolCase ucase;
	ucase.run();*/
	//StreamCase scase;
	//scase.run();
	Int a;
	std::cout << "Pause any key to continue." << std::endl;
	std::cin >> a;
    return 0;
}

