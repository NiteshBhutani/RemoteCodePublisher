//////////////////////////////////////////////////////////////////////
// RemoteCodePublisherServer.cpp - Remote code Publisher class which  //
//								publish web pages of source code	//
// Version 1.0														//
// Application: Remote Code Publisher - CSE 687 Project 4			//
// Platform:    Asus R558U, Win 10 Student Edition, Visual studio 15//
// Author:   Nitesh Bhutani, CSE687, nibhutani@syr.edu				//
//////////////////////////////////////////////////////////////////////
#include "RemoteCodePublisherServer.h"
#include "../Analyzer/Executive.h"
#include <string.h>
using sPtr = std::shared_ptr<AbstractXmlElement>;

/********<<Function to create Folder at specific path >>*******************/
void ServerClientHandler::createFolder(std::string path) {
	if (!FileSystem::Directory::exists(path)) {
		std::string command = "mkdir \"" + path + "\"";
		system(command.c_str());
		system("cls");
	}
}

/********<<Function to remove file at specific path >>*******************/
void ServerClientHandler::removeFile(std::string path) {
	if (FileSystem::File::exists(path)) {
		std::string command = "del /q \"" + path + "\"";
		system(command.c_str());
	}
}

//----< receiver functionality is defined by this function >---------
void ServerClientHandler::operator()(Socket socket)
{
	while (true)
	{
		HttpMessage msg = readMessage(socket);
		if (connectionClosed_ || msg.bodyString() == "quit")
		{
			Show::write("\n\n  clienthandler thread is terminating");
			break;
		}
		msgQ_.enQ(msg);
	}
}

//----< Function to handle Post requests >------------------
bool ServerClientHandler::handlePostRequest(HttpMessage& msg, Socket& socket) {
	std::string filename = msg.findValue("file");
	std::string path = msg.findValue("path");
	if (filename != "")
	{
		size_t contentSize;
		std::string sizeString = msg.findValue("content-length");
		if (sizeString != "")
			contentSize = Converter<size_t>::toValue(sizeString);
		else
			return false;
		readFile(path, filename, contentSize, socket);
	}
	if (filename != "")
	{
		msg.removeAttribute("content-length"); 	// construct message body
		std::string bodyString = "<file>" + filename + "</file>";
		std::string sizeString = Converter<size_t>::toString(bodyString.size());
		msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
		msg.addBody(bodyString);
	}
	else {
		size_t numBytes = 0; // read message body
		size_t pos = msg.findAttribute("content-length");
		if (pos < msg.attributes().size())
		{
			numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
			Socket::byte* buffer = new Socket::byte[numBytes + 1];
			socket.recv(numBytes, buffer);
			buffer[numBytes] = '\0';
			std::string msgBody(buffer);
			msg.addBody(msgBody);
			delete[] buffer;
		}
	}
	return true;
}

//----< this defines processing to frame messages >------------------
HttpMessage ServerClientHandler::readMessage(Socket& socket)
{
	connectionClosed_ = false;
	HttpMessage msg;
	while (true)// read message attributes
	{
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1)
		{
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);
		}
		else { break; }
	}
	if (msg.attributes().size() == 0) // If client is done, connection breaks and recvString returns empty string
	{
		connectionClosed_ = true;
		return msg;
	}

	if (msg.attributes()[0].first == "POST") // read body if POST - all messages in this demo are POSTs
	{	// is this a file message?
		bool result = handlePostRequest(msg, socket);
		if (result == false)
			return msg;
	}
	else if (msg.attributes()[0].first == "GET") {
		msg.addBody(msg.findValue("GET"));
	}
	return msg;
}

//----< read a binary file from socket and save >--------------------
bool ServerClientHandler::readFile(std::string path, const std::string& filename, size_t fileSize, Socket& socket)
{
	std::string fqname;
	if (path == "NoPath") {
		fqname = "../serverSourceCodeRepo/" + filename;
	}
	else {
		createFolder("..\\" + path);
		fqname = "..\\" + path + filename;
	}
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{	/* This error handling is incomplete.  The client will continue  to send bytes, but if the file can't be opened, then the server
		* doesn't gracefully collect and dump them as it should.  That's  an exercise left for students.*/
		Show::write("\n\n  can't open file " + fqname);
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

/*************<<<	Function to create XML structure of directory path sent as arguement >>>>**********************/
std::string Server::createDirectoryXMLStructure(std::string path) {
	std::string xml;
	std::string fpath = FileSystem::Path::getFullFileSpec(path);
	std::vector<std::string> fpathVector;
	StringHelper::splitIntoVector(fpath, "\\", fpathVector);
	xml = "<dir name=\"" + fpathVector[fpathVector.size() -1 ] + "\">";
	std::vector<std::string> files = FileSystem::Directory::getFiles(fpath, "*.*");
	for (auto f : files)
	{
		xml += "<file name=\"" + f + "\" />";
	}

	std::vector<std::string> dirs = FileSystem::Directory::getDirectories(fpath);
	for (auto d : dirs)
	{
		if (d == "." || d == "..")
			continue;
		std::string dpath = fpath + "\\" + d;
		xml += createDirectoryXMLStructure(dpath);
	}

	xml += "</dir>";

	return xml;
}

/*****<< Helper Function for getDependencyForHtmlFile>>**********/
std::string Server::getTheHrefTag(std::string atag) {

	atag.erase(0, 11);
	return atag.substr(0, atag.size() - 2);
}

/****<<Function to get dependecy list from parsing HTML file>>*********/
std::vector<std::string> Server::getDependencyForHtmlFile(std::string fileName) {
	std::vector<std::string> dependecyList;
	std::string html = "";
	std::ifstream file;
	file.open(fileName);
	if (file.is_open())
	{
		std::string line;
		while (getline(file, line))
		{
			html += line;
		}
		file.close();
	}
	int pos = 0;
	std::string aTag = "";
	while (html.find("<a href", pos) != std::string::npos) {
		pos = html.find("<a href", pos);
		aTag = "";
		int lastPos = -1;
		for (unsigned int i = pos; i < html.size(); i++) {
			if (html[i] != '>') {
				aTag += html[i];
			}
			else {
				aTag += html[i];
				lastPos = i;
				break;
			}
		}
		dependecyList.push_back(getTheHrefTag(aTag));
		pos = lastPos;
	}

	return dependecyList;
}

/*****<<Function to handle download command from client>>********/
void Server::processDownloadCommand(HttpMessage msg) {
	std::vector<std::string> msgVector;
	StringHelper::splitIntoVector(msg.bodyString(), "\n", msgVector);
	std::string downloadPath = msgVector[3].substr(msgVector[3].find(":") + 1);
	std::string fileName = msgVector[1].substr(msgVector[1].find(":") + 1);
	std::string path = "..\\" + msgVector[2].substr(msgVector[2].find(":") + 1);
	std::string CSSJSPath = path + "CSS\\";
	std::string fromAddr = msg.findValue("fromAddr");

	//get dependecy files
	std::vector<std::string> moreFileTobeSend = getDependencyForHtmlFile(path + fileName);
	std::unordered_map<std::string, std::string> fileMap;
	fileMap.insert({ { "style.css",CSSJSPath },{ "script.js",CSSJSPath } });
	//build file map
	for (auto i = moreFileTobeSend.begin(); i != moreFileTobeSend.end(); i++) {
		fileMap.insert({ *i,path });
	}
	//looping through all the filemap and send every file
	for (auto i = fileMap.begin(); i != fileMap.end(); i++) {
		sendFile(i->first, i->second, downloadPath, Convert<int>::fromString(fromAddr.substr(fromAddr.find(':') + 1)), "Still Downloading");
		::Sleep(200);
	}
	sendFile(fileName, path, downloadPath, Convert<int>::fromString(fromAddr.substr(fromAddr.find(':') + 1)), "Download Complete");
}

/*****<< Function to send file by server>>******/
bool Server::sendFile(const std::string& filename, std::string path, std::string serverPath, int toAddr, std::string body) {
	SocketSystem ss;
	SocketConnecter si;
	while (!si.connect("localhost", toAddr))
	{
		std::cout << "\n client waiting to connect";
		::Sleep(100);
	}
	std::vector<std::string> filePathVector;
	StringHelper::splitIntoVector(path, "\\", filePathVector);
	serverPath += "\\";
	for (unsigned int i = 2; i < filePathVector.size(); i++) {
		serverPath += filePathVector[i] + "\\";
	}
	serverPath = serverPath.substr(0, serverPath.size() - 1);
	std::string fqname = path + filename;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;
	HttpMessage msg = makeMessage(1, "", "localhost:" + std::to_string(toAddr));
	msg.addAttribute(HttpMessage::Attribute("file", filename));
	msg.addAttribute(HttpMessage::Attribute("path", serverPath));
	msg.addAttribute(HttpMessage::Attribute("downloadstatus", body));
	msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	sendMessage(msg, si);
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	while (true)
	{
		FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		si.send(blk.size(), buffer);
		if (!file.isGood())
			break;
	}
	file.close();
	return true;
}

/****<< Function to send message to client - it makes the message and then call sendMessage() func >>*****/
void Server::sendMsgToClient(std::string body, size_t n, int port) {
	SocketSystem ssc;
	SocketConnecter sc;
	while (!sc.connect("localhost", port))
	{
		Show::write("\n client waiting to connect");
		::Sleep(100);
	}
	HttpMessage msg = makeMessage(n, body, "localhost:" + std::to_string(port));
	sendMessage(msg, sc);
}

/*****<< Function to run Code Publisher >>******/
int Server::executeCodePublishing(std::string path) {
	
	std::string repoPath = "..\\" + path;
	char *repoPath_ = new char[repoPath.length() + 1];
	strcpy_s(repoPath_, repoPath.length() + 1, repoPath.c_str());
		
	int publisherArgc = 4;
	char* publisherArgv[] = {
		executablePath,
		repoPath_,
		"*.h",
		"*.cpp"
	};
	using namespace CodeAnalysis;
	CodeAnalysisExecutive CodePublisher;
	int result = CodePublisher.executeCodePublishing(publisherArgc, publisherArgv);
	delete[] repoPath_;
	return result;
}

/*******<<  Function to start the server client handler which is requiredto handle incoming messages>>>******/
void Server::startListener() {
	SocketSystem ss;
	SocketListener sl(serveraddr, Socket::IP6);
	ServerClientHandler cp(serverReecieveMsgQ);
	sl.start(cp);
	std::cout << "\n Listener started";
	serverSender();
}

/****<<// Function to send message by server>>*****/
void Server::sendMessage(HttpMessage& msg, Socket& socket)
{
	std::string msgString = msg.toString();
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}

/****<<Function to send different messages to client based on request by client>>*******/
void Server::serverSender() {
	while (true) {
		HttpMessage msg = serverReecieveMsgQ.deQ();
		if (msg.bodyString() == "GetSourceCode") {
			std::string fromAddr = msg.findValue("fromAddr");
			std::string xml = "Command:GetSourceCode" + createDirectoryXMLStructure("..\\serverSourceCodeRepo");
			sendMsgToClient(xml, 1, Convert<int>::fromString(fromAddr.substr(fromAddr.find(':') + 1)));
		}
		else if ((msg.bodyString() == "GetPublishCode")) {
			std::string fromAddr = msg.findValue("fromAddr");
			std::string xml = "Command:GetPublishCode" + createDirectoryXMLStructure("..\\Repository");
			sendMsgToClient(xml, 1, Convert<int>::fromString(fromAddr.substr(fromAddr.find(':') + 1)));
		}
		else if (msg.bodyString().find("<file>") != std::string::npos) {
			std::string fromAddr = msg.findValue("fromAddr");
			sendMsgToClient("Success", 1, Convert<int>::fromString(fromAddr.substr(fromAddr.find(':') + 1)));
		}
		else if (msg.bodyString().find("Command:CreateFolder") != std::string::npos) {
			std::string fromAddr = msg.findValue("fromAddr");
			std::vector<std::string> pathVector;
			StringHelper::splitIntoVector(msg.bodyString(), "\n", pathVector);
			std::string path = "..\\" + pathVector[1].substr(pathVector[1].find(":") + 1);
			ServerClientHandler::createFolder(path);
			sendMsgToClient("Success", 1, Convert<int>::fromString(fromAddr.substr(fromAddr.find(':') + 1)));
		}
		else if (msg.bodyString().find("Command:Publish") != std::string::npos) {
			std::string fromAddr = msg.findValue("fromAddr");
			std::vector<std::string> pathVector;
			StringHelper::splitIntoVector(msg.bodyString(), "\n", pathVector);
			std::string path = pathVector[1].substr(pathVector[1].find(":") + 1);
			if (executeCodePublishing(path) == 0)
				sendMsgToClient("Success", 1, Convert<int>::fromString(fromAddr.substr(fromAddr.find(':') + 1)));
			else
				sendMsgToClient("Fail to Publish Code", 1, Convert<int>::fromString(fromAddr.substr(fromAddr.find(':') + 1)));
		}
		else if (msg.bodyString().find("Command:Download") != std::string::npos) {
			processDownloadCommand(msg);
		}
		else if (msg.bodyString().find("Command:RemoveFile") != std::string::npos) {
			std::string fromAddr = msg.findValue("fromAddr");
			std::vector<std::string> pathVector;
			StringHelper::splitIntoVector(msg.bodyString(), "\n", pathVector);
			std::string path = "..\\" + pathVector[1].substr(pathVector[1].find(":") + 1);
			path = path.substr(0, path.size() - 1);
			ServerClientHandler::removeFile(path);
			sendMsgToClient("Success", 1, Convert<int>::fromString(fromAddr.substr(fromAddr.find(':') + 1)));
		}
	}
}

//----< factory for creating messages >------------------------------
HttpMessage Server::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:" + std::to_string(serveraddr);
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



//---------------<< Main Function - Test stub >> ------------
int main(int argc, char* argv[]) {
	std::cout << "\n starting server";
	
	std::string serverAddr = argv[1];
	std::string port = serverAddr.substr(serverAddr.find(':')+1);
	Server s(Convert<int>::fromString(port),argv[0]);//Initializing server
	s.startListener();//starting server
	
	//Test Stub
	//Testing Direcoty XML Structure
	//std::cout << s.createDirectoryXMLStructure("..\\CodeAnalyzer");
	////TestTIng Code Publishing
	//s.executeCodePublishing("..\serverSourceCodeRepo\\CodePublisherTest");
	////Testing message sending
	//s.sendMsgToClient("hi", 1, 8082);
	return 0;
}

