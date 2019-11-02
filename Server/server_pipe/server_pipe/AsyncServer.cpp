#include "pch.h"
#include "AsyncServer.h"


DWORD dWait, cbRet, dWrite;
HANDLE events[4];
PipeObj pipes[4];
bool sendReply = false;
char* replyBuff;
int capabArr[] = {1,2};


Blocking_Queue<QueueMsg> messageQueue;

bool AsyncServer::Initialize()
{
	try {
		std::thread* thread_object = NULL;
		std::cout << "######## Initializing server ##########" << std::endl;	
		for (int i = 0; i < instances; i++)
		{
			events[i] = CreateEvent(NULL, TRUE, TRUE, NULL);			
			pipes[i].overLap.hEvent = events[i];

			pipes[i].handler = CreateNamedPipe(TEXT("\\\\.\\pipe\\testPipe2"),
				PIPE_ACCESS_DUPLEX |     // read/write access 
				FILE_FLAG_OVERLAPPED,    // overlapped mode 
				PIPE_TYPE_MESSAGE |      // message-type pipe 
				PIPE_READMODE_MESSAGE |  // message-read mode 
				PIPE_WAIT,
				instances,
				1024 * sizeof(char),
				1024 * sizeof(char),
				NMPWAIT_USE_DEFAULT_WAIT,
				NULL);
			
			if (pipes[i].handler  == INVALID_HANDLE_VALUE){
				std::cout << "Error in CreateNamedPipe for " << i << std::endl;
				return FALSE;
			}

			LPOVERLAPPED lpo = &pipes[i].overLap;
			if (!ConnectNamedPipe(pipes[i].handler, lpo)) {
				pipes[i].state = CONNECTED;
			}
			else {
				std::cout << "Error in AsyncServer " << i << std::endl;
			}

			if (startProcessorThread) {

				thread_object = new std::thread(&AsyncServer::messageProcessorThread,this);
				startProcessorThread = FALSE;
			}
			std::cout << "Initialize NamedPipe " << i << std::endl;			
		}	
		std::cout << "Created Succesfully. No of instances = " << instances << std::endl;
		messageReceiver();
		std::cout << "Dead code";
		thread_object->join();
	}
	catch (...) {
		std::cout << "Error in AsyncServer::Initialize";
		return FALSE;
	}
}




void AsyncServer::messageReceiver()
{
	
	try{
		char* msgBuff;
		int ind;
		std::cout << "Starting messageProcessor " <<std::endl;
		while (true) {
			dWait = WaitForMultipleObjects(
				instances,    // number of event objects 
				events,      // array of event objects 
				FALSE,        // does not wait for all 
				INFINITE);

			ind = dWait - WAIT_OBJECT_0;  // determines which pipe 
			if (ind < 0 || ind >(instances - 1))
			{
				printf("Index out of range.\n");
			}
			GetOverlappedResult(
			    pipes[ind].handler, // handle to pipe 
			   &pipes[ind].overLap, // OVERLAPPED structure 
			   &cbRet,            // bytes transferred 
			   FALSE);

			if (pipes[ind].state == CONNECTED) {
				pipes[ind].checkDisc = FALSE;
				pipes[ind].state = READING;
				printf("CLIENT connected.\n");
			}

			switch (pipes[ind].state) {			
			case READING:		
				bool status = ReadFile(
					pipes[ind].handler,
					pipes[ind].buffer,
					1024 *sizeof(char),//12,
					&pipes[ind].dRead,
					&pipes[ind].overLap);
				if (!status && pipes[ind].overLap.InternalHigh > 0) {			
					std::cout << "Message Recieved from pipe. Adding in Queue to Process" << ind<< std::endl;	
					messageQueue.Enqueue(QueueMsg(ind, pipes[ind].buffer));	
					pipes[ind].checkDisc = TRUE;
				}
				else if (pipes[ind].checkDisc) {
					DisconnectAndReconnect(ind);
				}
			}
		}
	}
	catch (...) {
		std::cout << "Error in AsyncServer::messageProcessor";
	}
}

bool AsyncServer::DisconnectAndReconnect(DWORD i)
{
	try {
		if (!DisconnectNamedPipe(pipes[i].handler))
		{
			printf("DisconnectNamedPipe failed with %d.\n", GetLastError());
		}
		std::cout << "Client Disconnected.  Reconnecting" << std::endl;
		LPOVERLAPPED lpo = &pipes[i].overLap;
		if (!ConnectNamedPipe(pipes[i].handler, lpo)) {
			pipes[i].state = CONNECTED;
		}
		return TRUE;
	}
	catch (...) {
		std::cout << "Error in AsyncServer::DisconnectAndReconnect";
		return FALSE;
	}		
}


void AsyncServer::messageProcessorThread()
{
	try {
		int ind;
		std::string result;
		std::cout << "Starting messageProcessorThread" << std::endl;
		QueueMsg message;
		while (true) {
			messageQueue.Dequeue(message);		
			ind = message.pipeID;
			result = processMessage(message.buff);
			strncpy_s(pipes[ind].resultBuff, result.c_str(), sizeof(result));
			WriteFile(pipes[ind].handler,
				pipes[ind].resultBuff,
				sizeof(result),
				&dWrite,
				NULL);
		}
		if (sendReply) {
			WriteFile(pipes[ind].handler,
				&(*replyBuff),
				MSG_SIZE,
				&dWrite,
				NULL);
			sendReply = FALSE;
			delete replyBuff;
		}
		FlushFileBuffers(pipes[ind].handler);
		
	}
	catch (...) {
		std::cout << "Error in AsyncServer::messageProcessorThread";
	}
}


std::string AsyncServer::processMessage(char* buffer)
{
	int type;
	MessageSave* msg;
	MessageFetch* msgFtch;
	MessageAddCapability* capab;
	MessageServiceReq* servReq;
	std::map<int, char*>::iterator it;
	std::map<int, std::map<int, CustomClass*>*>::iterator clientItr;
	std::map<int, CustomClass*>::iterator capabItr;
	char* msgSave;
	int result = 0;
	std::map<int, CustomClass*>* capabList;
	MathOperations* obj;
	std::string resMessage;
	
	try {

		std::memcpy(&type, buffer, sizeof(int));	

		switch (type)
		{
		case SAVE_OBJ:
			std::cout << "Message recieved -> SAVE_OBJ" << std::endl;
			msg = (MessageSave*)buffer;		
			msgSave = new char[MSG_SIZE];
			std::memcpy(msgSave, buffer, MSG_SIZE);
			messageMap->insert(std::pair<int, char*>(msg->id, msgSave));
			std::cout << "Saved Message  << ID: " << msg->id << "  Message: " << msg->buff << std::endl;
			resMessage = "Object Saved. ID=  " + std::to_string(msg->id);
			break;
		case REPL_OBJ:
			std::cout << "Message recieved -> REPL_OBJ" << std::endl;
			msgFtch = (MessageFetch*)buffer;
			it = messageMap->find(msgFtch->id);
			if (it != messageMap->end()) {
				sendReply = true;
				replyBuff = it->second;
				std::cout << "Replying Message  << ID: " << it->first << std::endl;
				resMessage = "Sending Reply to ID= " + std::to_string(it->first);
			}
			else {
				std::cout << "Map does not contain ID " << msgFtch->id << std::endl;
			}
			break;
		case ADD_CLIENT:
			std::cout << "Message recieved -> ADD_CLIENT" << std::endl;
			capab = (MessageAddCapability*)buffer;
			capabList = new std::map<int, CustomClass*>();
			for (int i = 0; i < 2; i++) {
				if (capab->capabilities & capabArr[i] == capabArr[i]) {	
					switch (capabArr[i]) {
					case MathOps:
						capabList->insert(std::pair<int, CustomClass*>(capabArr[i], new MathOperations()));
						break;
					case StringOps:
						capabList->insert(std::pair<int, CustomClass*>(capabArr[i], new StringOperations()));
						break;
					}					
				}
			}
			clientsMap.insert(std::pair<int, std::map<int, CustomClass*>*>(capab->clientID, capabList));
			std::cout << "Client " << capab->clientID << " added in clients map" << std::endl;
			resMessage = "Client " + std::to_string(capab->clientID) +"  Added in Server";
			break;
		case SERVICE_REQ:
			std::cout << "Message recieved -> SERVICE_REQ" << std::endl;
			servReq = (MessageServiceReq*)buffer;
			clientItr = clientsMap.find(servReq->clientID);
			if (clientItr != clientsMap.end()) {
				std::map<int, CustomClass*>* capabMap = clientItr->second;
				capabItr = capabMap->find(servReq->capabilityID);
				if (capabItr != capabMap->end())
				{
					switch (capabItr->first) {
					case MathOps:
						obj = (MathOperations*)capabItr->second;
						result = obj->perform(servReq);
						resMessage = "Server Response = " + std::to_string(result);
						break;
					case StringOps:
						break;
					}
				}
			}			
			break;
		default:
		{
			std::cout << "wrong message Type" << std::endl;
			resMessage = "Error in Parsing Message";
		}
		}
	}
	catch (...) {
		std::cout << "Error in AsyncServer::processMessage";
		resMessage = "Error in Parsing Message";		
	}
	return resMessage;
}


AsyncServer::AsyncServer(int inst)
{
	instances = inst;
	messageMap = new std::map<int, char*>();	
	//clientsMap = new std::map<int, std::list<int>*>();
}


AsyncServer::~AsyncServer()
{	
	//delete clientsMap;
	delete messageMap;
	delete events;	
}
