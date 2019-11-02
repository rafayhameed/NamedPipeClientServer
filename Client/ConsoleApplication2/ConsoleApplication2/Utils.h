#pragma once
#include <windows.h> 

#define SEND_OBJ 1
#define RECV_OBJ 2
#define ADD_CLIENT 3
#define SERVICE_REQ 4

#define SEND_MSG_SIZE 58
#define MessageQueryCapability_MSG_SIZE 12
#define MessageServiceReq_MSG_SIZE 38

#define instances 4



enum Capabilities {
	MathOps = 1,
	StringOps = 2
};

enum MathOps {
	AddFun = 1,
	SybFun = 2
};


typedef struct {
	int msgType = SEND_OBJ;
	int id = 0;
	char buff[50];
}MessageSend;


typedef struct {
	int msgType = RECV_OBJ;
	int id = 0;
}MessageRecv;

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

typedef struct {
	HANDLE handler;
	DWORD state;
	DWORD dWrite;
	OVERLAPPED overLap;
	char buffer[1024];
}PipesObj;