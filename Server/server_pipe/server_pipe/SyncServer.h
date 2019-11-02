#pragma once
#include <windows.h> 
#include <iostream>
#include <thread> 

class SyncServer
{
public:
#define BUFSIZE 1024*1024*10

	void clientHandler(HANDLE hPipe);
	void InitializeServer();
	SyncServer();
	~SyncServer();
};

