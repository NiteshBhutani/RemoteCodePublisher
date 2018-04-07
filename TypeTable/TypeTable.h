//////////////////////////////////////////////////////////////////////
// TypeTable.h - Package to define the data structure that will		//
//				store the Types information after Type Analysis		//
// Version 1.0														//
// Application: Type Bases Dependecy Analysis - CSE 687 Project 2	//
// Platform:    Asus R558U, Win 10 Student Edition, Visual studio 15//
// Author:	Nitesh Bhutani, CSE687, nibhutan@syr.edu				//
//////////////////////////////////////////////////////////////////////

/*
Package Operations:
==================
1)  Type Info class which stores the type information which is later stored in typetable

2)  TypeTable class which stores vector of TypeInfo class and defines function to add/Retrieve
	type information(Name, type, Location, Fully Qualified Name) in-memory and used to retrieve 
	types based on namespace, location, FQN and type from TypeTable

Public Interface :
================
void add(const Type& type, const Name& name, const Location& location, const Namespace& fqn); // Function to add record into TypeTable
Record& operator[](int n); // Function to access single record/typeinfo/Type in typetable - Read/Write Access
Record operator[](int n) const; // Function to access single record/typeinfo/Type in typetable - Read Access
Records FindName(const Name& name);// Function to find all the types based on Type Name
Records FindType(const Type& type);// Function to find all the types based on Type 
Records FindNamespace(const Namespace& nameSpace);// Function to find all the types based on Namespace Name
std::string containFQN(const Namespace& fqn);// Function to check whether FQN of Type contain particular namespace - If yes return location 
Records FindLocation(const Location& Location);// Function to find all the types based on particular location
Records GetRecords() { return _records; }//Get the Type Table
std::string printTable();//Function to print typetable
Build Process:
==============
Build commands (either one)
- devenv CodeAnalysis.sln
- cl TypeTable.cpp

Maintainence History :
====================
- Version 1.0 : 9th March 2017
First Release
*/

#pragma once
#include <string>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>
///////////////////////////////////
//  Data to be stored in Typetable
//
class typeInfo {

public: 
	using  Type = std::string;
	using Name = std::string;
	using FQN = std::string;
	using Location = std::string;

	Type typeName_;     // TypeName
	Name name_;    // Name
	FQN fQN_;    // Fully Qualified Name
	Location location_;  // Location
	std::string show(); //Function to show single record of type table
	
};

class TypeTable {
	
	using Record = typeInfo;
	using Records = std::vector<Record>;
	using  Type = std::string;
	using Name = std::string;
	using Namespace = std::string;
	using Location = std::string;

public:
	void add(const Type& type, const Name& name, const Location& location, const Namespace& fqn);
	Record& operator[](int n);
	Record operator[](int n) const;
	Records FindName(const Name& name);
	Records FindType(const Type& type);
	Records FindNamespace(const Namespace& nameSpace); 
	std::string containFQN(const Namespace& fqn);
	Records FindLocation(const Location& Location);
	Records GetRecords() { return _records; }
	std::string printTable();
private:
	Records _records;
};