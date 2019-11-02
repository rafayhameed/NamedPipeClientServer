

#include "pch.h"
#include "utils.h"
#include <iostream>
#include <string>

HANDLE events[4];
PipesObj pipes[4];
DWORD dWait;

int main()
{
#pragma region VariableDeclaration
	HANDLE hPipe;
	DWORD dwWritten, dwRead;		
	int ind;

	char buffer[1024];
	char frameRecv[8];
	char frame[SEND_MSG_SIZE];
	char newFrwame[SEND_MSG_SIZE];
	char capabalBuff[MessageQueryCapability_MSG_SIZE];
	char servBuff[MessageServiceReq_MSG_SIZE];
	MessageSend msg, msg2;	
#pragma endregion
	
#pragma region MessagesObjectsCreation
	std::string mesgStr = "firstMessage";
	msg.id = 786;
	strncpy_s(msg.buff, mesgStr.c_str(), mesgStr.length());
	std::memcpy(frame, &msg, SEND_MSG_SIZE);

	std::string mesgStr2 = "secondMessage";
	msg2.id = 123;
	strncpy_s(msg2.buff, mesgStr2.c_str(), mesgStr2.length());
	std::memcpy(newFrwame,&msg2, SEND_MSG_SIZE);		

	MessageRecv recv;
	recv.id = 786;	
	std::memcpy(frameRecv, &recv, SEND_MSG_SIZE);


	MessageAddCapability capabalQry;
	capabalQry.clientID = 56;
	capabalQry.capabilities = 1 | 2;
	std::memcpy(capabalBuff, &capabalQry, MessageQueryCapability_MSG_SIZE);

	

	MessageServiceReq servReq;
	servReq.clientID = 56;
	servReq.capabilityID = MathOps;
	servReq.functionID = AddFun;
	servReq.param1 = 8;
	servReq.param2 = 9;	
	std::memcpy(servBuff, &servReq, MessageServiceReq_MSG_SIZE);
#pragma endregion
	
#pragma region PipeCreation
	hPipe = CreateFile(TEXT("\\\\.\\pipe\\testPipe2"),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
#pragma endregion
	if (hPipe != INVALID_HANDLE_VALUE)
	{

#pragma region MessagesSendingRecvPart
		// Sending Message 
		WriteFile(hPipe,
			frame,
			SEND_MSG_SIZE,
			&dwWritten,
			NULL);
		ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL);
		std::cout << "Message rcvd = " << buffer << std::endl;


		WriteFile(hPipe,
			capabalBuff,
			MessageQueryCapability_MSG_SIZE,
			&dwWritten,
			NULL);

		ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL);
		std::cout << "Message rcvd = " << buffer << std::endl;


		WriteFile(hPipe,
			servBuff,
			MessageServiceReq_MSG_SIZE,
			&dwWritten,
			NULL);

		ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL);
		std::cout << "Message rcvd = " << buffer << std::endl;


		WriteFile(hPipe,
			newFrwame,
			SEND_MSG_SIZE,
			&dwWritten,
			NULL);

		ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL);
		std::cout << "Message rcvd = " << buffer << std::endl;
		
		
		
		WriteFile(hPipe,
			frameRecv,
			8,
			&dwWritten,
			NULL);

		ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL);
		std::cout << "Message rcvd = " << buffer << std::endl;
#pragma endregion
		

		CloseHandle(hPipe);
	}

	return (0);
}

#pragma region AsyncClient
void AsyncClient() {
	char frame[SEND_MSG_SIZE];
	int ind;

	try {
		std::cout << "Async Client Started" << std::endl;
		for (int i = 0; i < instances; i++) {
			events[i] = CreateEvent(NULL, TRUE, TRUE, NULL);
			pipes[i].overLap.hEvent = events[i];

			pipes[i].handler = CreateFile(TEXT("\\\\.\\pipe\\testPipe2"),
				GENERIC_READ | GENERIC_WRITE | FILE_FLAG_OVERLAPPED,
				0,
				NULL,
				OPEN_EXISTING,
				0,
				NULL);
			if (pipes[i].handler != INVALID_HANDLE_VALUE)
			{
				std::cout << "Error in CreateFile" << std::endl;
			}
		}


		while (true) {
			dWait = WaitForMultipleObjects(
				instances,    // number of event objects 
				events,      // array of event objects 
				FALSE,        // does not wait for all 
				INFINITE);

			ind = dWait - WAIT_OBJECT_0;




			WriteFile(pipes[ind].handler,
				frame,
				SEND_MSG_SIZE,   // = length of string + terminating '\0' !!!
				&pipes[ind].dWrite,
				&pipes[ind].overLap);


		}
	}
	catch (...) {
		std::cout << "Error in Async Client";
	}	
}
#pragma endregion


