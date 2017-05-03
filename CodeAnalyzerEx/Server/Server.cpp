#pragma warning (disable: 4503)
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
#include "../Analyzer/Executive.h"

using Show = Logging::StaticLogger<1>;
using namespace Utilities;
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
			::Sleep(100);
		}

		// send a set of messages

		HttpMessage msg;

		for (size_t i = 0; i < NumMessages; ++i)
		{
			std::string msgBody =
				"<msg>Message #" + Converter<size_t>::toString(i + 1) +
				" from client </msg>";
			msg = makeMessage(1, msgBody, "localhost:8081");
			/*
			* Sender class will need to accept messages from an input queue
			* and examine the toAddr attribute to see if a new connection
			* is needed.  If so, it would either close the existing connection
			* or save it in a map[url] = socket, then open a new connection.
			*/
			sendMessage(msg, si);
			Show::write("\n\n  client sent\n" + msg.toIndentedString());
			::Sleep(TimeBetweenMessages);
		}
		//  send all *.cpp files from TestFiles folder

		std::vector<std::string> files = FileSystem::Directory::getFiles("../TestFiles", "*.cpp");
		for (size_t i = 0; i < files.size(); ++i)
		{
			Show::write("\n\n  sending file " + files[i]);
			sendFile(files[i], si);
		}

		// shut down server's client handler

		msg = makeMessage(1, "quit", "toAddr:localhost:8081");
		sendMessage(msg, si);
		Show::write("\n\n  client sent\n" + msg.toIndentedString());

		Show::write("\n");
		Show::write("\n  All done folks");
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}

}

std::string getRemoteCodePublishedDir(int category) {
	switch (category) {
	case 1:
		return "..\\RemoteCodePublished\\Category1\\";
	case 2:
		return "..\\RemoteCodePublished\\Category2\\";
	default:
		return "..\\RemoteCodePublished\\Category3\\";
	}
}

std::string getRemoteCodeDir(int category) {
	switch (category) {
	case 1:
		return "..\\RemoteCode\\Category1";
	case 2:
		return "..\\RemoteCode\\Category2";
	default:
		return "..\\RemoteCode\\Category3";
	}
}

int publishCode(int category) {

	char * argv[7];
	std::string x[] = { "CodeAnalyzer.exe",getRemoteCodeDir(category),"*.h","*.cpp","/m","/f","/r" };
	for (int i = 0; i < 7; i++) {
		const char* xx = x[i].c_str();
		argv[i] = _strdup(xx);
	}

	using namespace CodeAnalysis;

	CodeAnalysisExecutive exec;

	try {
		std::cout << "\n-----------------Requirement 10: Automated unit test suite-------------------\n\n";
		bool succeeded = exec.ProcessCommandLine(7, argv);
		if (!succeeded)
		{
			return 1;
		}
		exec.setDisplayModes();
		exec.startLogger(std::cout);

		std::ostringstream tOut("CodeAnalysis - Version 1.4");
		Utils::sTitle(tOut.str(), 3, 92, tOut, '=');
		Rslt::write(tOut.str());

		//Rslt::write("\n     " + exec.getAnalysisPath());
		Rslt::write("\n     " + exec.systemTime());
		Rslt::flush();
		exec.showCommandLineArguments(7, argv);
		Rslt::write("\n");

		exec.getSourceFiles();
		exec.processSourceCode(true);
		exec.complexityAnalysis();
		exec.dispatchOptionalDisplays();
		exec.flushLogger();
		exec.displayMetricSummary(50, 10);

		exec.flushLogger();
		Rslt::write("\n");
		std::ostringstream out;
		out << "\n  " << std::setw(10) << "searched" << std::setw(6) << exec.numDirs() << " dirs";
		out << "\n  " << std::setw(10) << "processed" << std::setw(6) << exec.numFiles() << " files";
		Rslt::write(out.str());
		Rslt::write("\n");
		exec.stopLogger();
		std::cout << "\n  Code Analysis completed";

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
		cPub.htmlFilePath = getRemoteCodePublishedDir(category);
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

	int result = publishCode(category);

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
		if (result == 0)
			res.addAttribute(HttpMessage::attribute("RESULT", "SUCCESS"));
		else
			res.addAttribute(HttpMessage::attribute("RESULT", "FAILURE"));
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
