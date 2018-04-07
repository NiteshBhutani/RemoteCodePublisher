//////////////////////////////////////////////////////////////////////
// Persistence.h - Package to store/retireve DB to/from XML			//
// Version 1.0														//
// Application: NoSQL Database - CSE 687 Project 1					//
// Platform:    Asus R558U, Win 10 Student Edition, Visual studio 15//
// Author:	Nitesh Bhutani, CSE687, nibhutan@syr.edu				//
//////////////////////////////////////////////////////////////////////


/*
Package Operations:
==================

This package defines Persistence class which defines function to store in-memory DB into XML
and used to retrieve data from xml and store it into DB

	Public Interface :
	================
	std::string toXMLString(); // function to convert DB into XMl
	void fromXMLString(const std::string& xml,bool augument); // function to read from Xml and store into DB
	std::string readFile(const std::string& filename); // function to read xml file
	bool writeFile(const std::string& fileName, const std::string& content, bool append_); // function to write into xml file

Build Process:
==============
Required files
- CppProperties.h,CppProperties.cpp , Utilities.h, Utilities.cpp, XMLDocument.h , XMLElement.h, XMLDoxument.cpp, XMLElement.cpp
  Convert.h, Convert.cpp, NoSqlDB.h, NoSqlDb.cpp
Build commands (either one)
- devenv NoSQLDB-Project1.sln
- cl NoSqlDb.cpp

Maintainence History :
====================
- Version 1.0 : 4th Jan 2017
  First Release
*/
#pragma once
#include "../XmlDocument/XmlDocument/XmlDocument.h"
#include "../XmlDocument/XmlElement/XmlElement.h"
#include "../Convert/Convert.h"
#include "../NoSQLDB/NoSqlDb.h"
#include "../StrHelper.h"
#include <string>
#include <fstream>

using namespace XmlProcessing;

template<typename Data>
class Persistence {
public:
	using Key = std::string;
	using Keys = std::vector<Key>;
	using sPtr = std::shared_ptr<AbstractXmlElement>;
	//constructor
	Persistence(NoSqlDb<Data> &db_) : db(db_){
	}

	std::string toXMLString();
	void fromXMLString(const std::string& xml,bool augument);
	std::string readFile(const std::string& filename);
	bool writeFile(const std::string& fileName, const std::string& content, bool append_);
	
private:
	NoSqlDb<Data>& db;//Db which is used to store data into XML / retrieve data from xml
};
/*----------------<Function to write string into DB> - Param ==> bool append_ - if true then write file in append mode-----------------------*/
template<typename Data>
bool Persistence<Data>::writeFile(const std::string& filename, const std::string& content, bool append_) {
	std::ofstream myfile;

	if(append_)//if true write file in append mode
		myfile.open(filename, std::ios::app);
	else
		myfile.open(filename);

	if (myfile.is_open())
	{
		myfile << content;
		myfile.close();
	}
	else return false;

	return true;
}
/*---------<Function to read xml and onvert into string>-------------------*/
template<typename Data>
std::string Persistence<Data>::readFile(const std::string& filename) {
	std::string filestring;
	std::string line;
	std::ifstream myfile(filename);
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			filestring.append(line);
		
		}
		myfile.close();
	}
	
	return filestring;
}

/*----------<function to convert in-memory DB into XMl string>--------------*/
template<typename Data>
std::string Persistence<Data>::toXMLString() {
	std::string xml;
	XmlDocument doc;
	sPtr pRoot = makeTaggedElement("DB");
	doc.docElement() = pRoot;
	for (Key key: db.keys()) {
		sPtr pElementtag = makeTaggedElement("Element");//make Element<Data> name tag and add to child
		pRoot->addChild(pElementtag);
		//sPtr pNameTag = makeTaggedElement("Name");//adding Name metadata
		//pElementtag->addChild(pNameTag);
		//sPtr pNameText = makeTextElement(db.value(key).name.getValue());
		//pNameTag->addChild(pNameText);
		//sPtr pCatTag = makeTaggedElement("Category");//adding category
		//pElementtag->addChild(pCatTag);
		//sPtr pCatText = makeTextElement(db.value(key).category.getValue());
		//pCatTag->addChild(pCatText);
		//sPtr ptextDescTag = makeTaggedElement("TextDesc");//adding text Desc
		//ptextDescTag->addChild(ptextDescText);
		//sPtr ptimeDateTag = makeTaggedElement("TimeDate");//adding time data
		//pElementtag->addChild(ptimeDateTag);
		//sPtr ptimeDateText = makeTextElement(db.value(key).timeDate.getValue());
		//ptimeDateTag->addChild(ptimeDateText);
		sPtr ptimeDataTag = makeTaggedElement("Data");//adding data 
		pElementtag->addChild(ptimeDataTag);
		std::string dataString = Convert<Data>::toString(db.value(key).data.getValue());
		sPtr ptimeDataText = makeTextElement(dataString);
		ptimeDataTag->addChild(ptimeDataText);
		sPtr pChildrenTag = makeTaggedElement("Children");//adding children
		pElementtag->addChild(pChildrenTag);
		for (Key key : db.value(key).getChildren()) {//Loop to add child
			sPtr pChildTag = makeTaggedElement("Child");
			pChildrenTag->addChild(pChildTag);
			sPtr pChildText = makeTextElement(key);
			pChildTag->addChild(pChildText);
		}
	}
	xml = doc.toString();
	return xml;
}
/*-------------<Function to read XML string and load it into in-memory DB>----------------
	Param - bool append - if ture add elements into DB in appended / augument mode
*/
template<typename Data>
void Persistence<Data>::fromXMLString(const std::string& xml,bool augument) {
	if (!augument) {
		db.clearDB();
	}
	XmlDocument doc(xml, XmlDocument::str);
		
	//Getting all name/Category/TextDesc/timeDate/Data/Children
	std::vector<sPtr> descName = doc.descendents("Name").select();
	std::vector<sPtr> descCategory = doc.descendents("Category").select();
	std::vector<sPtr> descTextDesc = doc.descendents("TextDesc").select();
	std::vector<sPtr> descDate = doc.descendents("TimeDate").select();
	std::vector<sPtr> descData = doc.descendents("Data").select();
	std::vector<sPtr> descChildren = doc.descendents("Children").select();

	for (unsigned i = 0; i < descName.size(); i++) {
		Element<Data> e1;
		e1.name = trim(descName[i]->children()[0]->value());
		e1.category = trim(descCategory[i]->children()[0]->value());
		e1.textDesc = trim(descTextDesc[i]->children()[0]->value());
		e1.timeDate = trim(descDate[i]->children()[0]->value());
		std::string data = trim(descData[i]->children()[0]->value());
		e1.data = Convert<Data>::fromString(data);//to Convert string back in typename <Data> type
		
		(db).save(e1.name, e1);
	}
	for (unsigned i = 0; i < descName.size(); i++) {
		Element<Data> e1;
		e1.name = trim(descName[i]->children()[0]->value());
		for (sPtr child : descChildren[i]->children()) {
			std::string child_ = trim(child->children()[0]->value());
			db.addChild(e1.name, child_);
		}
	}
}