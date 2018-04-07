#pragma once
//////////////////////////////////////////////////////////////////////
// RemoteCodePublisherServer.h - Remote code Publisher class which  //
//								publish web pages of source code	//
// Version 1.0														//
// Application: Remote Code Publisher - CSE 687 Project 4			//
// Platform:    Asus R558U, Win 10 Student Edition, Visual studio 15//
// Author:   Nitesh Bhutani, CSE687, nibhutani@syr.edu				//
//////////////////////////////////////////////////////////////////////

/*
Package Operations:
==================

Server is the Remot Code Publisher Program which provide following functionlity :-
1) Upload Source Code Files
2) Downloading Source Code File
3) Folder Creation
4) File Removal
5) Publishing Functionality


- Server Class
Public Interface:
================
HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep); // Function to make HTTP style message
void sendMessage(HttpMessage& msg, Socket& socket); // Function to send message by server
void startListener();// Function to start the server client handler which is requiredto handle incoming messages
void serverSender();// Function to send different messages to client based on request by client
void sendMsgToClient(std::string body, size_t n, int port); //Function to send message to client - it makes the message and then call sendMessage() func
std::string createDirectoryXMLStructure(std::string path); //Function to create XML Directory structure of a specific path
int executeCodePublishing(std::string path); // Function to run Code Publisher
void processDownloadCommand(HttpMessage msg); // Function to handle client download request
bool sendFile(HttpMessage msg); // Function to send file
std::vector<std::string> getDependencyForHtmlFile(std::string filename); // Function to get dependecy information of a HTML file
std::string getTheHrefTag(std::string atag); //Helper function for getDependencyForHtmlFile
bool sendFile(const std::string& filename, std::string path, std::string serverPath, int toAddr, std::string body); //Function send file


-  ServerClientHandler class is used for 
defining function required to process/listen incoming messages from different clients

Public Interface :
================
void operator()(Socket socket); // Function which make ServerClientHandler class a callable ObjectS
static void createFolder(std::string path); //Function to create folder at particular path
static void removeFile(std::string path); // Function to remove certail file

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

#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>
#include <thread>
#include "../XmlDocument/XmlDocument/XmlDocument.h"
#include "../XmlDocument/XmlElement/XmlElement.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../Convert/Convert.h"
#include <unordered_map>

using BQueue = Async::BlockingQueue<std::string>;
using Show = Logging::StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;
using namespace XmlProcessing;
using namespace Async;

/////////////////////////////////////////////////////////////////////
// Server ClientHandler class
/////////////////////////////////////////////////////////////////////
class ServerClientHandler
{
public:
	ServerClientHandler(Async::BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
	static void createFolder(std::string path);
	static void removeFile(std::string path);

private:
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(std::string path,const std::string& filename, size_t fileSize, Socket& socket);
	bool handlePostRequest(HttpMessage& msg, Socket& socket);
	BlockingQueue<HttpMessage>& msgQ_;
};



//Server Class
class Server{
	using EndPoint = std::string;

	public:
		
		Server(int addr,  char* execPath) : serveraddr(addr), executablePath(execPath){}
		HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep); // Function to make message for sending the response back by server
		void sendMessage(HttpMessage& msg, Socket& socket); // Function to send message by server
		void startListener();
		void serverSender();
		void sendMsgToClient(std::string body, size_t n, int port);
		std::string createDirectoryXMLStructure(std::string path);
		int executeCodePublishing(std::string path);
		void processDownloadCommand(HttpMessage msg);
		std::vector<std::string> getDependencyForHtmlFile(std::string filename);
		std::string getTheHrefTag(std::string atag);
		bool sendFile(const std::string& filename, std::string path, std::string serverPath, int toAddr, std::string body);
	private:
		int serveraddr;
		char* executablePath;
		BlockingQueue<HttpMessage> serverReecieveMsgQ;
		bool messageTobeSend = false;
		
};


