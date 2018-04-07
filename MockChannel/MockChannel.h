#ifndef MOCKCHANNEL_H
#define MOCKCHANNEL_H
///////////////////////////////////////////////////////////////////////////////////////
// MockChannel.h - Demo for CSE687 Project #4, Spring 2015                           //
// - build as static Library showing how C++\CLI client can use native code channel  //
// - MockChannel reads from sendQ and writes to recvQ                                //
//                                                                                   //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015  
// Nitesh Bhutani, CSE 687 
///////////////////////////////////////////////////////////////////////////////////////

#ifdef IN_DLL
#define DLL_DECL __declspec(dllexport)
#else
#define DLL_DECL __declspec(dllimport)
#endif


#include <string>
using Message = std::string;

//***********<Struct for sender queue>
struct ISendr
{
  virtual void postMessage(const Message& msg) = 0;
};

//***********<Struct for reciever queue>
struct IRecvr
{
  virtual std::string getMessage() = 0;
};

//***********<Struct for mock channel cotaning both sending and recieving queue>
struct IMockChannel
{
public:
  virtual void start() = 0;
  virtual void stop() = 0;
};

//***********<Object Factory>
extern "C" {
  struct ObjectFactory
  {
     ISendr* createSendr();
    IRecvr* createRecvr();
    IMockChannel* createMockChannel(ISendr* pISendr, IRecvr* pIRecvr, int sPort, int myPort);
  };
}

#endif


