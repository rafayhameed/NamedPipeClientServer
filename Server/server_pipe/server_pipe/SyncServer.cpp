#include "pch.h"
#include "SyncServer.h"


void SyncServer::clientHandler(HANDLE hPipe)
{
	DWORD dwRead;
	char buffer[1024];
	try {

		HANDLE hHeap = GetProcessHeap();
		char* pRequest = (char*)HeapAlloc(hHeap, 0, BUFSIZE);
		char* pReply = (char*)HeapAlloc(hHeap, 0, BUFSIZE);

		while (true) {
			std::cout << "oo bhai jaan thread started" << std::endl;
			if (hPipe != INVALID_HANDLE_VALUE) {
				ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL);
				while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
				{
					/* add terminating zero */
					buffer[dwRead] = '\0';

					/* do something with data in buffer */
					printf("%s", buffer);
				}

			}
		}
		DisconnectNamedPipe(hPipe);
	}
	catch (...)
	{
		std::cout << "excep";
	}
}

void SyncServer::InitializeServer()
{
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	//std::thread* thread_object;
	try {
		hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\testPipe"),
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			1024 * 16,
			1024 * 16,
			NMPWAIT_USE_DEFAULT_WAIT,
			NULL);
		while (hPipe != INVALID_HANDLE_VALUE)
		{
			if (ConnectNamedPipe(hPipe, NULL) != FALSE)   // wait for someone to connect to the pipe
			{
				std::cout << "Client Connected" << std::endl;

				DWORD dwRead;
				char buffer[1024];
				ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL);
				//thread_object = new std::thread(&SyncServer::clientHandler, hPipe);
			}
		}
		//thread_object->join();
	}
	catch (std::exception e)
	{
		std::cout << e.what();
	}
}

SyncServer::SyncServer()
{
}


SyncServer::~SyncServer()
{
}
