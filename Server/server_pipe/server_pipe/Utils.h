#pragma once
#include <windows.h>

#define SAVE_OBJ 1
#define REPL_OBJ 2
#define ADD_CLIENT 3
#define SERVICE_REQ 4

#define MSG_SIZE 58

#define CONNECTED 0 
#define READING 1 
#define WRITING 2  
#define capabArrLength 2;


enum Capabilities {
	MathOps = 1,
	StringOps = 2
};

enum MathOps {
	AddFun = 1,
	SubFun = 2
};

typedef struct {
	HANDLE handler;
	DWORD state;
	DWORD dRead;
	OVERLAPPED overLap;
	bool checkDisc;
	char buffer[1024];
	char resultBuff[1024];
}PipeObj;

typedef struct {
	int msgType = SAVE_OBJ;
	int id = 0;
	char buff[50];
}MessageSave;

typedef struct {
	int msgType = REPL_OBJ;
	int id = 0;
}MessageFetch;

struct QueueMsg {
	int pipeID;
	char* buff;
	QueueMsg() {

	}
	QueueMsg(int a, char* b) {
		pipeID = a;
		buff = b;
	}
};

typedef struct {
	int msgType = ADD_CLIENT;
	int capabilities = 0;
	int clientID;
}MessageAddCapability;

typedef struct {
	int msgType = SERVICE_REQ;
	int capabilityID;
	int functionID;
	int clientID;
	int param1;
	int param2;
	int param3;
	char param4[10];
}MessageServiceReq;
