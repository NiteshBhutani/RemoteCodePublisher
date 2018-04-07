//////////////////////////////////////////////////////////////////////
// MockChannel.cpp - Mock Channel/Communication						//
//								channel between server & client		//
// Version 1.0														//
// Application: Remote Code Publisher - CSE 687 Project 4			//
// Platform:    Asus R558U, Win 10 Student Edition, Visual studio 15//
// Author:   Nitesh Bhutani, CSE687, nibhutani@syr.edu				//
//////////////////////////////////////////////////////////////////////
/*
Package Operations:
==================

-Client Class - this is backend communication channel used by GUI for communication with server
-it supports HTTP style Messages


Public Interface:
================
void start(); // To start the communication channel for listening messages from client
void stop(); // To stop communication channel from listening
bool sendFile(const std::string& fqname, std::string path, std::string serverPath); // Function to send file to server
void sendMsgToServer(const std::string& body, size_t n); //Function to send message to server
HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep); // Factory function to make HTTP style message
void processMessagefromGUI(Message msg); // Function to process msgs from client/GUI and then communicate further with server
void sendMessage(HttpMessage& msg, Socket& socket);  // Function to send messages to server
void Listener(); // Function to start listener for client for listening reply back from server


-  ClientHandler class is used for
defining function required to process/listen incoming messages from server

Public Interface :
================
void operator()(Socket socket); // Function which make lientHandler class a callable ObjectS
static void createFolder(std::string path); //Function to create folder at particular path

* Sendr class
 - accepts messages from client for consumption by MockChannel
 Public Interface -
 ================
 void postMessage(const Message& msg); // Function to post messages to queue
  BQueue& queue(); // function to return queue
 
*Recvr class
accept message from MockChannel for consumption client
Public Interface -
================
void getMessage(const Message& msg); // Function to get messages from queue
BQueue& queue(); // function to return queue

Build Process:
==============
Required files
*   - Executive.h, Executive.cpp, CodePublisher.cpp , CodePublisher.h, Sockets.h, Sockets.cpp,
*   - WindowsHelper.h , WindowsHelper.cpp
*  - Parser.h, Parser.cpp, ActionsAndRules.h, ActionsAndRules.cpp
*  - ConfigureParser.h, ConfigureParser.cpp
*  - ScopeStack.h, ScopeStack.cpp, AbstrSynTree.h, AbstrSynTree.cpp
*  - ITokenCollection.h, SemiExp.h, SemiExp.cpp, Tokenizer.h, Tokenizer.cpp
*  - IFileMgr.h, FileMgr.h, FileMgr.cpp, FileSystem.h, FileSystem.cpp
*  - Logger.h, Logger.cpp, Utilities.h, Utilities.cpp
Build commands (either one)
- devenv RemoteCodePublisher.sln
- //cl RemoteCodePublisherServer.cpp

Maintainence History :
====================
- Version 1.0 : 3rd May 2017


*/
#define IN_DLL
#include "MockChannel.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include <string>
#include <thread>
#include <iostream>
#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>
#include <thread>
#include <functional>
#include <algorithm>
#include <exception>
#include <iostream>

using Show = Logging::StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;
using BQueue = Async::BlockingQueue < Message >;

/////////////////////////////////////////////////////////////////////////////
// Sendr class
// - accepts messages from client for consumption by MockChannel
//
class Sendr : public ISendr
{
public:
  void postMessage(const Message& msg);
  BQueue& queue();
private:
  BQueue sendQ_;
};

/*******<<Function to post messages to queue>>*********/
void Sendr::postMessage(const Message& msg)
{
  sendQ_.enQ(msg);
}

/*******<<Function to return queue>>*********/
BQueue& Sendr::queue() { return sendQ_; }

/////////////////////////////////////////////////////////////////////////////
// Recvr class
// - accepts messages from MockChanel for consumption by client
//
class Recvr : public IRecvr
{
public:
  Message getMessage();
  BQueue& queue();
private:
  BQueue recvQ_;
};


/*******<<Function to get messages from queue>>*********/
Message Recvr::getMessage()
{
  return recvQ_.deQ();
}

/*******<<Function to return queue>>*********/
BQueue& Recvr::queue()
{
  return recvQ_;
}

/*******<<Global Function to split string based on particular string>>*********/
void splitClient(const std::string& s, std::string c, std::vector<std::string>& v) {
	std::string::size_type i = 0;
	std::string::size_type j = s.find(c);
	while (j != std::string::npos) {
		v.push_back(s.substr(i, j - i));
		i = j + c.size();
		j = j + 1;
		j = s.find(c, j);
		if (j == std::string::npos)
			v.push_back(s.substr(i, s.length()));
	}
}

// Client ClientHandler class
/////////////////////////////////////////////////////////////////////
// Class to handle incoming messages from server
class ClientHandler
{
public:
	ClientHandler(Async::BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
	static void createFolder(std::string path);
private:
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(std::string path,const std::string& filename, size_t fileSize, Socket& socket);
	Async::BlockingQueue<HttpMessage>& msgQ_;
};

/*******<<Function to create Folder at specific Path>>*********/
void ClientHandler::createFolder(std::string path) {
	if (!FileSystem::Directory::exists(path)) {
		std::string command = "mkdir \"" + path + "\"";
		system(command.c_str());
		system("cls");
	}
}

//----< this defines processing to frame messages >------------------
HttpMessage ClientHandler::readMessage(Socket& socket)
{	connectionClosed_ = false;
	HttpMessage msg;
	while (true)	// read message attributes
	{	std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1)
		{	HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);}
		else
		{	break;
		}
	}
	if (msg.attributes().size() == 0) // If client is done, connection breaks and recvString returns empty string
	{	connectionClosed_ = true;
		return msg;}
	if (msg.attributes()[0].first == "POST") // read body if POST - all messages in this demo are POSTs
	{	std::string filename = msg.findValue("file");// is this a file message?
		std::string path = msg.findValue("path");
		if (filename != "")
		{	size_t contentSize;
			std::string sizeString = msg.findValue("content-length");
			if (sizeString != "")
				contentSize = Converter<size_t>::toValue(sizeString);
			else
				return msg;
			readFile(path,filename, contentSize, socket);
		}
		if (filename != "")// construct message body if this is file read
		{	msg.removeAttribute("content-length");
			std::string bodyString = msg.findValue("downloadstatus");
			std::string sizeString = Converter<size_t>::toString(bodyString.size());
			msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
			msg.addBody(bodyString);
		}else// read message body
		{	size_t numBytes = 0;
			size_t pos = msg.findAttribute("content-length");
			if (pos < msg.attributes().size())
			{	numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
				Socket::byte* buffer = new Socket::byte[numBytes + 1];
				socket.recv(numBytes, buffer);
				buffer[numBytes] = '\0';
				std::string msgBody(buffer);
				msg.addBody(msgBody);
				delete[] buffer;}
		}	}
	return msg;
}

//----< read a binary file from socket and save >--------------------
bool ClientHandler::readFile(std::string path,const std::string& filename, size_t fileSize, Socket& socket)
{	std::string fqname;
	createFolder(path);
	fqname =  path + filename;
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		/*
		* This error handling is incomplete.  The client will continue
		* to send bytes, but if the file can't be opened, then the server
		* doesn't gracefully collect and dump them as it should.  That's
		* an exercise left for students.
		*/
		std::cout << ("\n\n  can't open file " + fqname);
		std::cout << "\n File Reading Fail at mock channel";
		return false;
	}

	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];

	size_t bytesToRead;
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;

		socket.recv(bytesToRead, buffer);

		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);

		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close();
	return true;
	
}

//----< receiver functionality is defined by this function >---------
void ClientHandler::operator()(Socket socket)
{
	while (true)
	{
		HttpMessage msg = readMessage(socket);
		if (connectionClosed_ || msg.bodyString() == "quit")
		{
			Show::write("\n\nClient side : clienthandler  thread is terminating");
			break;
		}
		msgQ_.enQ(msg);
		
	}
	
}


/////////////////////////////////////////////////////////////////////////////
// Client class
// - reads messages from Sendr and communicate with Server and writes messages to Recvr
//
class Client : public IMockChannel
{
	using EndPoint = std::string;
public:
  Client(ISendr* pSendr, IRecvr* pRecvr, int sPort, int madd);
  void start();
  void stop();
  bool sendFile(const std::string& fqname, std::string path, std::string serverPath);
  void sendMsgToServer(const std::string& body, size_t n);
  HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep);
  void processMessagefromGUI(Message msg);
  void sendMessage(HttpMessage& msg, Socket& socket);
  void Listener();

private:
  int serverPort;
  int myPort;
  std::thread thread_;
  ISendr* pISendr_;
  IRecvr* pIRecvr_;
  bool stop_ = false;
  Async::BlockingQueue<HttpMessage> ClientRecieveQ;
};

/*******<<Function to listen messages from Server>>*********/
void Client::Listener() {

	SocketSystem ss;
	SocketListener sl(myPort, Socket::IP6);
	ClientHandler cp(ClientRecieveQ);
	sl.start(cp);
	std::cout << "\nClient Side: Listener started";

	Recvr* pRecvr = dynamic_cast<Recvr*>(pIRecvr_);
	BQueue& recvQ = pRecvr->queue();
	HttpMessage httpmsg = ClientRecieveQ.deQ();
	Message msg = httpmsg.bodyString();
	if (httpmsg.findValue("downloadstatus") == "Download Complete")
		msg += "\nPath:" + httpmsg.findValue("path") +"\nFile:"+ httpmsg.findValue("file");
	if(msg != "Success")
		recvQ.enQ(msg);
	
}

//----< send message using socket >----------------------------------
void Client::sendMessage(HttpMessage& msg, Socket& socket)
{
	std::string msgString = msg.toString();
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}

/*******<<Function to send messages to server>>*********/
void Client::sendMsgToServer(const std::string& body, size_t n) {

	SocketSystem ss;
	SocketConnecter si;
	while (!si.connect("localhost", serverPort))
	{
		std::cout << ("\n client waiting to connect");
		::Sleep(100);
	}

	HttpMessage msg;

	msg = makeMessage(n, body, "localhost:" + std::to_string(serverPort));
	sendMessage(msg, si);
}

//----< factory for creating messages >------------------------------
HttpMessage Client::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:" + std::to_string(myPort);  // ToDo: make this a member of the sender
																  // given to its constructor.
	switch (n)
	{
	case 1: //Post
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
	case 2://Get
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("GET", body));
		msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));
		break;
	default:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));
	}
	return msg;
}

//----< send file using socket - in binary format>-------------------------------------
bool Client::sendFile(const std::string& filename, std::string path, std::string serverPath)
{	SocketSystem ss;
	SocketConnecter si;
	while (!si.connect("localhost", serverPort))
	{
		std::cout << "\n client waiting to connect";
		::Sleep(100);
	}
	// assumes that socket is connected
	std::string fqname = path + filename;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;

	HttpMessage msg = makeMessage(1, "", "localhost:"+ std::to_string(serverPort));
	msg.addAttribute(HttpMessage::Attribute("file", filename));
	if (serverPath == "") {
		msg.addAttribute(HttpMessage::Attribute("path", "NoPath"));
	}
	else {
		msg.addAttribute(HttpMessage::Attribute("path", serverPath));
	}
	msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	std::cout << "\n Msg sent is : \n" + msg.toString();
	sendMessage(msg, si);
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	while (true)
	{	FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		si.send(blk.size(), buffer);
		if (!file.isGood())
			break;}
	file.close();
	return true;
}

/*******<<Function to process messages fromclient and forward it to server in HTTP style messages>>*********/
void Client::processMessagefromGUI(Message msg) {

	std::vector<std::string> msgVector;
	splitClient(msg, "\n",msgVector);
	if (msgVector[0].find("Upload") != std::string::npos) {//Command is Upload
		std::string serverPath = msgVector[3].substr( msgVector[3].find(":")+1);
		std::string path = msgVector[2].substr(msgVector[2].find(":") + 1);
		std::string filename = msgVector[1].substr(msgVector[1].find(":") + 1);
		sendFile(filename,path,serverPath);
	}
	else if (msgVector[0].find("GetSourceCode") != std::string::npos) {
		sendMsgToServer("GetSourceCode",2);
	}
	else if (msgVector[0].find("GetPublishCode") != std::string::npos) {
		sendMsgToServer(msgVector[0].substr(msgVector[0].find(":") + 1), 2);
	}
	else if (msgVector[0].find("CreateFolder") != std::string::npos) {
		sendMsgToServer(msg, 1);
	}
	else if (msgVector[0].find("Publish") != std::string::npos) {
			sendMsgToServer(msg, 1);
	}
	else if (msgVector[0].find("Download") != std::string::npos) {
		sendMsgToServer(msg, 1);
	}
	else if (msgVector[0].find("RemoveFile") != std::string::npos) {
		sendMsgToServer(msg, 1);
	}
}

//----< pass pointers to Sender and Receiver >-------------------------------
Client::Client(ISendr* pSendr, IRecvr* pRecvr,int sPort, int madd) : pISendr_(pSendr), pIRecvr_(pRecvr), serverPort(sPort), myPort(madd) {}

//----< creates thread to read from sendQ and echo back to the recvQ >-------
void Client::start()
{
  std::cout << "\n  MockChannel starting up";
  thread_ = std::thread(
    [this] {
    Sendr* pSendr = dynamic_cast<Sendr*>(pISendr_);
    Recvr* pRecvr = dynamic_cast<Recvr*>(pIRecvr_);
    if (pSendr == nullptr || pRecvr == nullptr)
    {
      std::cout << "\n  failed to start Mock Channel\n\n";
      return;
    }
    BQueue& sendQ = pSendr->queue();
    BQueue& recvQ = pRecvr->queue();
    while (!stop_)
    {
      std::cout << "\n  channel deQing message";
      Message msg = sendQ.deQ();  // will block here so send quit message when stopping
	  if(msg != "Still Downloading")
		processMessagefromGUI(msg);
      std::cout << "\n  channel enQing message";
	  Listener();
	  //recvQ.enQ(ClientRecieveQ.deQ().bodyString());
    }
    std::cout << "\n  Server stopping\n\n";
  });
}

//----< signal server thread to stop >---------------------------------------
void Client::stop() { stop_ = true; }

//----< factory functions >--------------------------------------------------
ISendr* ObjectFactory::createSendr() { return new Sendr; }

IRecvr* ObjectFactory::createRecvr() { return new Recvr; }

IMockChannel* ObjectFactory::createMockChannel(ISendr* pISendr, IRecvr* pIRecvr, int sPort, int myPort) 
{ 
  return new Client(pISendr, pIRecvr,sPort, myPort); 
}

#ifdef TEST_MOCKCHANNEL

//----< test stub >----------------------------------------------------------

int main()
{
  ObjectFactory objFact;
  ISendr* pSendr = objFact.createSendr();
  IRecvr* pRecvr = objFact.createRecvr();
  IMockChannel* pMockChannel = objFact.createMockChannel(pSendr, pRecvr);
  pMockChannel->start();
  pSendr->postMessage("Hello World");
  pSendr->postMessage("CSE687 - Object Oriented Design");
  Message msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg << "\"";
  msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg << "\"";
  pSendr->postMessage("stopping");
  msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg << "\"";
  pMockChannel->stop();
  pSendr->postMessage("quit");
  std::cin.get();
}
#endif
