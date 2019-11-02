// ServerPipe.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <windows.h> 
#include <thread> 
#include "AsyncServer.h"


int main()
{
	std::cout << "Initializing Async Server with default 4 instances" << std::endl;
	AsyncServer server = AsyncServer(4);
	if (!server.Initialize())
	{
		std::cout << "Error in Initializing Async Server" << std::endl;
	}
	return 0;
}


