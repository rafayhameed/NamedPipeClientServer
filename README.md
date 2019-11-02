# NamedPipeClientServer
Git Link:
https://github.com/rafayhameed/NamedPipeClientServer

This is a client Server application to communicate with each other using named pipes. It contains two different projects.
1.	Client Application
2.	Server Application (server_pipe)

Server Application
It is console application named server_pipe. 
1.	ServerPipe.cpp:  contains the main function. 
2.	AsyncServer.cpp: contains implementation of Asynchronous named pipes
3.	SyncServer.cpp:  contains implementation of synchronous named pipes
4.	Blocking_Queue.h: contains wrapper on queue
5.	CustomClass.cpp: custom classes which client can request. 
6.	MathOperations.cpp: Type of custom class inherited from  CustomClass
7.	StringOperations.cpp: Type of custom class inherited from  CustomClass
8.	Utils.h c: contains all structures and enums required for communication protocol

Design
 
1.	Message Format is same between client and server
2.	Client serializes the message object and sends bytes to server
3.	Server receives the message and enqueues the bytes.
4.	Message Processor thread dequeues the bytes.
5.	First 4 bytes contain the message type for all messages.
6.	Depending upon the message type, it deserializes the object and processes.


Messages:

MessageSave:
Clients sends the message object. Server saves this message in bytes form in memory. 
MessageFetch:
Client requests the server and server replies the message buffer from saved MessageSave object.
MessageAddCapability:
Client sends list of IDs for custom classes. All IDs are passed by operation OR and a single ID is generated.
Server Decodes the ID by using AND with custom classes IDs and generates the list of IDs. Instance of respective custom classes are saved against clientID.
MessageServiceReq:
Client can access the functions from its custom class objects saved in server memory. 
