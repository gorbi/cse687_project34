#pragma once
/////////////////////////////////////////////////////////////////////////
// Client.h - Demonstrates simple one-way HTTP messaging               //
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
#include <sstream>
#include <vector>
#include <iterator>

std::vector<std::string> split(const std::string &s, char delim) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	std::vector<std::string> elems;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

using Show = Logging::StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;

/////////////////////////////////////////////////////////////////////
// ClientCounter creates a sequential number for each client
//
class ClientCounter
{
public:
	ClientCounter() { ++clientCount; }
	size_t count() { return clientCount; }
private:
	static size_t clientCount;
};

size_t ClientCounter::clientCount = 0;

/////////////////////////////////////////////////////////////////////
// MsgClient class
// - was created as a class so more than one instance could be 
//   run on child thread
//
class MsgClient
{
public:
	using EndPoint = std::string;
	HttpMessage startListener(int);
	std::string publish(int);
	std::string remove(int);
	std::string display(int);
	std::string upload(int category, std::string filenames);
private:
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& fqname, Socket& socket, int category);
};

//----< factory for creating messages >------------------------------
/*
* This function only creates one type of message for this demo.
* - To do that the first argument is 1, e.g., index for the type of message to create.
* - The body may be an empty string.
* - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
*   expects the receiver EndPoint for the toAddr attribute.
*/
HttpMessage MsgClient::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:8081";  // ToDo: make this a member of the sender
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

void MsgClient::sendMessage(HttpMessage& msg, Socket& socket)
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
bool MsgClient::sendFile(const std::string& fqname, Socket& socket, int category)
{
	// assumes that socket is connected
	try {

		std::string filename = FileSystem::Path::getName(fqname);
		FileSystem::FileInfo fi(fqname);
		size_t fileSize = fi.size();
		std::string sizeString = Converter<size_t>::toString(fileSize);
		FileSystem::File file(fqname);
		file.open(FileSystem::File::in, FileSystem::File::binary);
		if (!file.isGood())
			return false;

		HttpMessage msg = makeMessage(1, "", "localhost::8080");
		msg.addAttribute(HttpMessage::Attribute("file", filename));
		msg.addAttribute(HttpMessage::attribute("CATEGORY", std::to_string(category)));
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
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
		return false;
	}
}
HttpMessage MsgClient::startListener(int port)
{
	Async::BlockingQueue<HttpMessage> msgQ;
	try
	{
		SocketSystem ss;
		SocketListener sl(port, Socket::IP6);
		ClientHandler cp(msgQ);
		sl.start(cp);
		/*
		* Since this is a server the loop below never terminates.
		* We could easily change that by sending a distinguished
		* message for shutdown.
		*/
		return msgQ.deQ();
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
		HttpMessage empty;
		return empty;
	}
}

std::string MsgClient::upload(int category, std::string filenames) {
	try {
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			Show::write("\n client waiting to connect");
			::Sleep(100);
		}
		// send a set of messages
		std::vector<std::string> files = split(filenames, ',');
		std::cout << "\n  got "<< files.size() <<" files to send";

		int count = 0;
		for (std::string file : files) {
			if (sendFile(file, si, category)) {
				std::cout << "\n  sent file: " << file;
				count++;
			}
			else {
				std::cout << "\n  failed to send file: " << file;
			}
		}
		std::cout << "\n  sent " << count << " files";

		std::string res("");
		res.append(std::to_string(count)); res.append("/"); res.append(std::to_string(files.size()));
		res.append(" files sent to server for category: "); res.append(std::to_string(category));
		return std::string(res);
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
		return std::string("Caught exception");
	}
}


std::string MsgClient::publish(int category)
{
	try {
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			Show::write("\n client waiting to connect");
			::Sleep(100);
		}
		// send a set of messages
		HttpMessage msg;
		msg = makeMessage(1, "PUBLISH", "toAddr:localhost:8080");
		msg.addAttribute(HttpMessage::attribute("CATEGORY",std::to_string(category)));
		sendMessage(msg, si);
		HttpMessage res = startListener(8081);
		if (res.bodyString() == "PUBLISH") {
			if (res.findValue("RESULT")=="SUCCESS")
				return std::string("Server successfully published files in category: "+ res.findValue("CATEGORY"));
			else
				return std::string("Server failed to publish files in category: " + res.findValue("CATEGORY"));
		}
		else {
			return std::string("Server failed to publish files in category: " + res.findValue("CATEGORY"));
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
		return std::string("Caught exception");
	}
}

std::string MsgClient::remove(int category)
{
	try {
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			Show::write("\n client waiting to connect");
			::Sleep(100);
		}
		// send a set of messages
		HttpMessage msg;
		msg = makeMessage(1, "DELETE", "toAddr:localhost:8080");
		msg.addAttribute(HttpMessage::attribute("CATEGORY", std::to_string(category)));
		sendMessage(msg, si);
		HttpMessage res = startListener(8081);
		if (res.bodyString() == "DELETE") {
			if (res.findValue("RESULT") == "SUCCESS")
				return std::string("Server successfully deleted files in category: " + res.findValue("CATEGORY"));
			else
				return std::string("Server failed to delete files in category: " + res.findValue("CATEGORY"));
		}
		else {
			return std::string("Server failed to delete files in category: " + res.findValue("CATEGORY"));
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
		return std::string("Caught exception");
	}
}

std::string MsgClient::display(int category)
{
	try {
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			Show::write("\n client waiting to connect");
			::Sleep(100);
		}
		// send a set of messages
		HttpMessage msg;
		msg = makeMessage(1, "DISPLAY", "toAddr:localhost:8080");
		msg.addAttribute(HttpMessage::attribute("CATEGORY", std::to_string(category)));
		sendMessage(msg, si);
		HttpMessage res = startListener(8081);
		if (res.bodyString() == "DISPLAY") {
			if (res.findValue("RESULT") == "SUCCESS") {
				std::cout << "\n  Received success response for display request";
				std::string r("Server successfully sent/displayed files in category: " + res.findValue("CATEGORY"));
				std::string files = res.findValue("FILES");
				if (files != "NONE")
					r += ","+files;
				std::cout << "\n  Response: " << r;
				return r;
			}
			else
				return std::string("Server failed to send/display files in category: " + res.findValue("CATEGORY"));
		}
		else {
			return std::string("Server failed to send/display files in category: " + res.findValue("CATEGORY"));
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
		return std::string("Caught exception");
	}
}

//----< entry point - runs two clients each on its own thread >------

#ifdef DEBUG

int main()
{
	::SetConsoleTitle(L"Clients Running on Threads");

	Show::title("Demonstrating two HttpMessage Clients each running on a child thread");

	MsgClient c1;

	std::thread tserver(
		[&]() { c1.startListener(8080); } // Start listener on the client c1
	);

	tserver.detach();

	c1.execute(100, 1);

	/*std::thread t1(
	[&]() { c1.execute(100, 20); } // 20 messages 100 millisec apart
	);*/

	/*MsgClient c2;
	std::thread t2(
	[&]() { c2.execute(120, 20); } // 20 messages 120 millisec apart
	);
	t1.join();
	t2.join();*/
}

#endif // DEBUG
