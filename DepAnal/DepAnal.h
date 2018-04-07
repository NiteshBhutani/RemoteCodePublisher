//////////////////////////////////////////////////////////////////////
// DepAnal.h - Package to do type based Dependency Analysis for		//
//				a given set of files based on TypeTable				//
// Version 1.0														//
// Application: Type Bases Dependecy Analysis - CSE 687 Project 2	//
// Platform:    Asus R558U, Win 10 Student Edition, Visual studio 15//
// Author:	Nitesh Bhutani, CSE687, nibhutan@syr.edu				//
//////////////////////////////////////////////////////////////////////

/*
Package Operations:
==================
1)  DepAnal class which perform Type Based Dependecy Analysis for a given set of files based on Type Table
	It uses NoSQLDB<std::string> to store Depedency relationship bu using NoSQL child
Public Interface :
================
DepAnal(const FileMap& fm, const size_t& nmd, const size_t& nmf, const TypeTable& ta ); // Constructor
virtual ~DepAnal(); // Destructor
void setFileMap(const FileMap& fm) // Function to set the File Map
void setNumdirs(const size_t& nmd) // Function to set Num of dirs processed
void setNumFiles(const size_t& nmf) // Function to set Num of Files processed
void setTypeTable(const TypeTable& ta) // Function to set Type Table
std::vector<std::string> removePreProcessorStatements(std::vector<std::string>& tok) // Function to remove preprocessor tokens
std::vector<std::string> removeNewLineAndOperators(std::vector<std::string>& tok); // Function to remove newLine and operators from vector of tokens
std::set<File> buildDependencyForFile(File file);// Function to build Dependency for a particular files
void checkHeaderDependecyonCpp(CodeAnalysis::ASTNode* pNode, int treelevel); // Function to check whether cpp file depend on its header or not 
NoSqlDb<std::string> getDependecyTable();// Function to get Dependecy table
FileMap& getFileMap(); // Function to get File map
size_t numFiles(); // Function to get num of files processed
size_t numDirs();// function to get num of dirs processed
void buildDependencyTable();//Function to build dependency - NoSQLDB table - call buildDependencyForFile() function
void buildTokenVector(File file);// Function to build token vector for particular file
void buildTokenVectoAndDependecyTable();//Function to build dependency table enteries for files

Build Process:
==============
- Executive.h, Executive.cpp, TypeTable.h, TypeTable.cpp, TypeAnalysis.h, TypeAnalysis.h
*  - Parser.h, Parser.cpp, ActionsAndRules.h, ActionsAndRules.cpp
*  - ConfigureParser.h, ConfigureParser.cpp
*  - ScopeStack.h, ScopeStack.cpp, AbstrSynTree.h, AbstrSynTree.cpp
*  - ITokenCollection.h, SemiExp.h, SemiExp.cpp, Tokenizer.h, Tokenizer.cpp
*  - IFileMgr.h, FileMgr.h, FileMgr.cpp, FileSystem.h, FileSystem.cpp
*  - Logger.h, Logger.cpp, Utilities.h, Utilities.cpp
*
Build commands (either one)
- devenv CodeAnalysis.sln
- cl DepAnal.cpp

Maintainence History :
====================
- Version 1.0 : 9th March 2017
First Release
*/

#pragma once
#include<string>
#include<vector>
#include<set>
#include <unordered_map>
#include "../FileMgr/FileMgr.h"
#include "../TypeTable/TypeTable.h"
#include "../TypeAnalysis/TypeAnalysis.h"
#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include "../NoSQLDB/NoSqlDb.h"

namespace DependencyAnalysis
{

	///////////////////////////////////////////////////////////////////
	// DepAnal Class Tokenize the files present in the directories

	class DepAnal
	{
	public:
		using Path = std::string;
		using Pattern = std::string;
		using Patterns = std::vector<Pattern>;
		using File = std::string;
		using Files = std::vector<File>;
		using Options = std::vector<char>;
		using FileMap = std::unordered_map<Pattern, Files>;
		using TokenVector = std::unordered_map<File, std::vector<std::string>>;

		DepAnal(const FileMap& fm, const size_t& nmd, const size_t& nmf, const TypeTable& ta );
		virtual ~DepAnal();
		DepAnal(const DepAnal&) = delete;
		DepAnal& operator=(const DepAnal&) = delete;
		void setFileMap(const FileMap& fm) { fileMap_ = fm; }
		void setNumdirs(const size_t& nmd) { numDirs_ = nmd; }
		void setNumFiles(const size_t& nmf) { numFiles_ = nmf; }
		void setTypeTable(const TypeTable& ta) { typeTable_ = ta; }
		std::vector<std::string> removePreProcessorStatements(std::vector<std::string>& tok);
		std::vector<std::string> removeNewLineAndOperators(std::vector<std::string>& tok);
		std::set<File> buildDependencyForFile(File file);
		void checkHeaderDependecyonCpp(CodeAnalysis::ASTNode* pNode, int treelevel);
		NoSqlDb<std::string> getDependecyTable();
		FileMap& getFileMap();
		size_t numFiles();
		size_t numDirs();
		void buildDependencyTable();//Function to build dependency - NoSQLDB table
		void buildTokenVector(File file);
		void buildTokenVectoAndDependecyTable();
	
	private:
		FileMap fileMap_;
		TokenVector tokenVector_;
		std::set<std::pair<File, File>> filePairVector;
		CodeAnalysis::AbstrSynTree& ASTref_;
		size_t numFiles_;
		size_t numDirs_;
		TypeTable typeTable_;
		NoSqlDb<std::string> dependencytable;
	};
}
