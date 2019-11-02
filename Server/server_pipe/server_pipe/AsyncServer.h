#pragma once
#include <iostream> 
#include <string> 
#include <thread> 
#include <map> 
#include "Blocking_Queue.h"
#include "StringOperations.h"
#include "MathOperations.h"
#include "utils.h"
#include <stdio.h>
#include <algorithm> 

class AsyncServer
{
private:
	int instances = 4;	
	std::string pipeAddrs;
	bool startProcessorThread = true;
	void messageReceiver();
	std::map<int, char*>* messageMap;
	std::map<int, std::map<int, CustomClass*>*> clientsMap;
	void messageProcessorThread();
	bool DisconnectAndReconnect(DWORD i);
public:
	std::string processMessage(char* buff);
	bool Initialize();
	AsyncServer(int inst);
	~AsyncServer();
};

