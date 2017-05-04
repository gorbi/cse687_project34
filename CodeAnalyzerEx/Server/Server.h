#pragma once
/////////////////////////////////////////////////////////////////////////
// Server.cpp - Demonstrates simple one-way HTTP messaging             //
//                                                                     //
// Nagaprasad Natarajaurs, CSE687 - Object Oriented Design, Spring 2016//
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Dell XPS 8900, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a server that received HTTP style messages and
* files from a client that the processes on the oncoming request.
*
* It accepts files from client to the given category
* It publishes the files for the given category
* It cleans up files for the given category
* It sends the list of files in the given category when the client asks for
* it
* Supports Lazy download and sends IIS url when publish happens
* Send all files in the given category when requested
*/
/*
* Required Files:
*   MsgClient.cpp, MsgServer.cpp
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp
*/
/*
* ToDo:
* - pull the sending parts into a new Sender class
* - You should create a Sender like this:
*     Sender sndr(endPoint);  // sender's EndPoint
*     sndr.PostMessage(msg);
*   HttpMessage msg has the sending adddress, e.g., localhost:8080.
*/
#pragma warning (disable: 4503)

#include "../HttpMessage/HttpMessage.h"
#include "Sockets.h"
#include "FileSystem.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>
#include <thread>
#include <set>
#include "ClientHandler.h"
#include "../Analyzer/Executive.h"
#include "Server.h"

using Show = Logging::StaticLogger<1>;
using namespace Utilities;
using EndPoint = std::string;

class Server
{
public:
	void start(int port);
	void stop();
private:
	Async::BlockingQueue<HttpMessage> msgQ;
	void processDownloadRequest(int category, std::string files, std::unordered_map<std::string, std::vector<std::string>>& dependencyMap);
	void processDisplayRequest(int category);
	void processDeleteRequest(int category);
	int deleteCode(int category);
	void showDepdencyMap(std::unordered_map<std::string, std::vector<std::string>>& dependencyMap);
	HttpMessage makeMessage(size_t n, const std::string& body, const EndPoint& ep);
	void cleanup(int category);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& fqname, Socket& socket, int category);
	int publishCode(int category, std::unordered_map<std::string, std::vector<std::string>>& dependencyMap);
	void processPublishRequest(int category, std::unordered_map<std::string, std::vector<std::string>>& dependencyMap);
};