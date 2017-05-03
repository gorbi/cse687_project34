/////////////////////////////////////////////////////////////////////////
// Server.cpp - Demonstrates simple one-way HTTP messaging             //
//                                                                     //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016           //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Dell XPS 8900, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a client that sends HTTP style messages and
* files to a server that simply displays messages and stores files.
*
* It's purpose is to provide a very simple illustration of how to use
* the Socket Package provided for Project #4.
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
#include "../HttpMessage/HttpMessage.h"
#include "Sockets.h"
#include "FileSystem.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>
#include <thread>
#include "ClientHandler.h"

using Show = Logging::StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;
using EndPoint = std::string;

//----< factory for creating messages >------------------------------
/*
* This function only creates one type of message for this demo.
* - To do that the first argument is 1, e.g., index for the type of message to create.
* - The body may be an empty string.
* - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
*   expects the receiver EndPoint for the toAddr attribute.
*/
HttpMessage makeMessage(size_t n, const std::string& body, const EndPoint& ep)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:8080";  // ToDo: make this a member of the sender
											 // given to its constructor.
	switch (n)
	{
	case 1:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("POST", "Message"));
		msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

		msg.addBody(body);
		if (body.size() > 0)
		{
			attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
			msg.addAttribute(attrib);
		}
		break;
	default:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));
	}
	return msg;
}
//----< send message using socket >----------------------------------

void sendMessage(HttpMessage& msg, Socket& socket)
{
	std::string msgString = msg.toString();
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}
//----< send file using socket >-------------------------------------
/*
* - Sends a message to tell receiver a file is coming.
* - Then sends a stream of bytes until the entire file
*   has been sent.
* - Sends in binary mode which works for either text or binary.
*/
bool sendFile(const std::string& filename, Socket& socket)
{
	// assumes that socket is connected

	std::string fqname = "../TestFiles/" + filename;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;

	HttpMessage msg = makeMessage(1, "", "localhost::8081");
	msg.addAttribute(HttpMessage::Attribute("file", filename));
	msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	sendMessage(msg, socket);
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	while (true)
	{
		FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		socket.send(blk.size(), buffer);
		if (!file.isGood())
			break;
	}
	file.close();
	return true;
}

//----< this defines the behavior of the client >--------------------

void execute(const size_t TimeBetweenMessages, const size_t NumMessages)
{
	// send NumMessages messages
	Show::attach(&std::cout);
	Show::start();

	Show::title(
		"Starting HttpMessage client on thread " + Utilities::Converter<std::thread::id>::toString(std::this_thread::get_id())
	);
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8081))
		{
			Show::write("\n client waiting to connect");
			::Sle2017-05-03 01:46:59, Info                         [svchost.exe] Enter WinReIsWimBootEnabled
2017-05-03 01:46:59, Info                         [svchost.exe] Parameters: pszWinDir: NULL
2017-05-03 01:46:59, Info                         [svchost.exe] Exit WinReIsWimBootEnabled returns 0 with last error: 0x0
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            completed";

		Files allsubfiles = exec.getAllsubFiles();

		//Type Analysis
		TypeAnal ta;
		ta.doTypeAnal();
		//Get Line Number Details
		std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>>& lineMapInst = ta.getLineNumMap();


		//Dependency Analysis
		DepAnal da(exec.getDepXMLPath(), 5);
		for (File file : allsubfiles)
			da.doDepAnal(ta, file);

		//Invoke code publisher
		CodePublisher cPub(da);
		cPub.fileList(allsubfiles, lineMapInst);

		std::cout << "\n\n-------------------------------------------------------------------------------------\n\n";

	}
	catch (std::exception& except)
	{
		exec.flushLogger();
		std::cout << "\n\n  caught exception in Executive::main: " + std::string(except.what()) + "\n\n";
		exec.stopLogger();
		return 1;
	}
	return 0;
}

void processPublishRequest(int category) {
	Show::write("\n\n  server recvd publish request for category: " + std::to_string(category));

	::Sleep(5000);

	try {
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8081))
		{
			Show::write("\n client waiting to connect");
			::Sleep(100);
		}

		// send a set of messages
		HttpMessage res = makeMessage(1, "PUBLISH", "toAddr:localhost:8081");
		res.addAttribute(HttpMessage::attribute("CATEGORY", std::to_string(category)));
		res.addAttribute(HttpMessage::attribute("RESULT", "SUCCESS"));
		sendMessage(res, si);
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}


//----< entry point - runs two clients each on its own thread >------
int main()
{
	::SetConsoleTitle(L"HttpMessage Server - Runs Forever on Port 8080");

	Show::attach(&std::cout);
	Show::start();
	Show::title("\n  HttpMessage Server started");

	Async::BlockingQueue<HttpMessage> msgQ;

	try
	{
		SocketSystem ss;
		SocketListener sl(8080, Socket::IP6);
		ClientHandler cp(msgQ);
		sl.start(cp);
		/*
		* Since this is a server the loop below never terminates.
		* We could easily change that by sending a distinguished
		* message for shutdown.
		*/
		while (true)
		{
			HttpMessage msg = msgQ.deQ();
			if (msg.bodyString()=="PUBLISH") {
				processPublishRequest(std::stoi(msg.findValue("CATEGORY")));
			}
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}
