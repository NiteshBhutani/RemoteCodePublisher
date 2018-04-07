//////////////////////////////////////////////////////////////////////
//  RemoteCodePublisher.cpp - Test Suite to demostrate				//
//   requirements													//
//  version 1.0                                                     //
//  Language:     C++, Visual Studio 2015                           //
//  Application:  Dependency-Based CodePublisher			        //
//				  CSE687 - Object Oriented Design                   //
//  Author:       Nitesh Bhutani                                    //
/////////////////////////////////////////////////////////////////////

#include "TestExecutive.h"
#include "../Utilities/Utilities.h"

// ----------- < req 1 demo > -----------
void RemoteCodePublisher_TestExecutive::Req1()
{
	std::cout << "This project is developed using Visual Studio 2015 and its C++ Windows Console Projects" << std::endl;
	std::cout << std::endl;
}

// ----------- < req 2 demo > -----------
void RemoteCodePublisher_TestExecutive::Req2()
{
	std::cout << "Uses the C++ standard library's streams for all console I/O and new and delete for all heap-based memory management." << std::endl;
	std::cout << std::endl;
}

// ----------- < req 3 demo > -----------
void RemoteCodePublisher_TestExecutive::Req3()
{
	std::cout << "Shall provide a Repository program that provides functionality to publish, as linked web pages, the contents of a set of C++ source code files. RemoteCodePublisherServer is the project" << std::endl;
	std::cout << "The repository program provide following functionality:- ";
	std::cout << "\n 1)  Code File or Folder Uploading,";
	std::cout << "\n 2) New Folder Creation -- only in server source code repository";
	std::cout << "\n 3 )File Removal -- only in server source code repository not in published repo";
	std::cout << "\n 4) Publishing";
	std::cout << "\n 5) File Downloading using Lazy download strategy \n";
	std::cout << std::endl;
}

// ----------- < req 4 demo > -----------
void RemoteCodePublisher_TestExecutive::Req4()
{
	std::cout << "Shall, for the publishing process, satisfy the requirements of CodePublisher developed in Project #3." << std::endl;
	std::cout << "After uploading the Code Files on server. Click the publish button by selecting the directory to see the publishing functionality \n";
	std::cout << std::endl;
}

// ----------- < req 5 demo > -----------
void RemoteCodePublisher_TestExecutive::Req5()
{
	std::cout << "Shall provide a Client program that can upload files3, and view Repository contents, as described in the Purpose section, above." << std::endl;
	std::cout << "Please see the GUI upload button to see this functionality. You can upload both file or whole folder. For uploading folder check the \"select folder\" checkbox \n ";
	std::cout << std::endl;
}

// ----------- < req 6 demo > -----------
void RemoteCodePublisher_TestExecutive::Req6()
{
	std::cout << "Shall provide a message-passing communication system, based on Sockets, used to access the Repository's functionality from another process or machine." << std::endl;
	std::cout << "Communication between GUI and server takes place through comunication channel based on sockets. \n";
	std::cout << std::endl;
}

// ----------- < req 7 demo > -----------
void RemoteCodePublisher_TestExecutive::Req7()
{
	std::cout << "The communication system shall provide support for passing HTTP style messages using either synchronous request/response or asynchronous one-way messaging." << std::endl;
	std::cout << "The communication through channel is by HTTP style messages. \n";
	std::cout << std::endl;
}

// ----------- < req 8 demo > -----------
void RemoteCodePublisher_TestExecutive::Req8()
{
	std::cout << "The communication system shall also support sending and receiving streams of bytes6. Streams will be established with an initial exchange of messages." << std::endl;
	std::cout << "The transmission of files between server and client takes place by sending and receiving streams of bytes. \n";
	std::cout << std::endl;
}

// ----------- < req 9 demo > -----------
void RemoteCodePublisher_TestExecutive::Req9()
{
	std::cout << "Shall include an automated unit test suite that demonstrates you meet all the requirements of this project4 including the transmission of files." << std::endl;
	std::cout << std::endl;
}

// ----------- < req 10 demo > -----------
void RemoteCodePublisher_TestExecutive::Req10()
{
	std::cout << " Used Lazy download strategy while downloading file from server and opening in browser." << std::endl;
	std::cout << "Look at files downloaded in downloaded folder to see the Lazy Download req.\n ";
	std::cout << std::endl;
}

using namespace Utilities;
//#ifdef test
int main(int argc, char* argv[])
{	//Calling all Functions functions
	StringHelper::Title("Demonstrating Req 1");
	std::cout << "\n";
	RemoteCodePublisher_TestExecutive::Req1();
	std::cout << "\n";
	StringHelper::Title("Demonstrating Req 2");
	std::cout << "\n";
	RemoteCodePublisher_TestExecutive::Req2();
	std::cout << "\n";
	StringHelper::Title("Demonstrating Req 3");
	std::cout << "\n";
	RemoteCodePublisher_TestExecutive::Req3();
	std::cout << "\n";
	StringHelper::Title("Demonstrating Req 4");
	std::cout << "\n";
	RemoteCodePublisher_TestExecutive::Req4();
	std::cout << "\n";
	StringHelper::Title("Demonstrating Req 5");
	std::cout << "\n";
	RemoteCodePublisher_TestExecutive::Req5();
	std::cout << "\n";
	StringHelper::Title("Demonstrating Req 6");
	std::cout << "\n";
	RemoteCodePublisher_TestExecutive::Req6();
	std::cout << "\n";
	StringHelper::Title("Demonstrating Req 7");
	std::cout << "\n";
	RemoteCodePublisher_TestExecutive::Req7();
	std::cout << "\n";
	StringHelper::Title("Demonstrating Req 8");
	std::cout << "\n";
	RemoteCodePublisher_TestExecutive::Req8();
	std::cout << "\n";
	StringHelper::Title("Demonstrating Req 9");
	std::cout << "\n";
	RemoteCodePublisher_TestExecutive::Req9();
	std::cout << "\n";
	StringHelper::Title("Demonstrating Req 10");
	std::cout << "\n";
	RemoteCodePublisher_TestExecutive::Req10();
}