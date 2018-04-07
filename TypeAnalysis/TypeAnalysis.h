//////////////////////////////////////////////////////////////////////
// TypeAnalysis.h - Package to do type analysis on set of files 	//
//			and build the type table containing type information	//
// Version 1.0														//
// Application: Type Bases Dependecy Analysis - CSE 687 Project 2	//
// Platform:    Asus R558U, Win 10 Student Edition, Visual studio 15//
// Author:	Nitesh Bhutani, CSE687, nibhutan@syr.edu				//
//////////////////////////////////////////////////////////////////////

/*
Package Operations:
==================
1)  TypeAnalysis class which perform the build the TypeTable by doing Type Analysis on set of files(c++ Files)

2)  Context Info Helper struct used by Type Analysis class DFS function to build Type Table

Public Interface :
================
TypeAnalysis(); //Constructor
void doTypeAnal(); // Function to retrieve type information ( Enum, Global Function, Alias, Struct, class) from AST
	-- doTypeAnal() function DFS(ASTNode *pNode,int treeLevel)  private function to do Depth First Search
void printTypeTable(); // Function to print TypeTable
TypeTable returnTypeTable() // Function to return TypeTable
Build Process:
==============
- Executive.h, Executive.cpp. 
*  - Parser.h, Parser.cpp, ActionsAndRules.h, ActionsAndRules.cpp
*  - ConfigureParser.h, ConfigureParser.cpp
*  - ScopeStack.h, ScopeStack.cpp, AbstrSynTree.h, AbstrSynTree.cpp
*  - ITokenCollection.h, SemiExp.h, SemiExp.cpp, Tokenizer.h, Tokenizer.cpp
*  - IFileMgr.h, FileMgr.h, FileMgr.cpp, FileSystem.h, FileSystem.cpp
*  - Logger.h, Logger.cpp, Utilities.h, Utilities.cpp
*
Build commands (either one)
- devenv CodeAnalysis.sln
- cl TypeAnalysis.cpp

Maintainence History :
====================
- Version 1.0 : 9th March 2017
First Release
*/
#pragma once
#include "../Parser/ActionsAndRules.h"
#include "../TypeTable/TypeTable.h"
#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include <iostream>
#include <functional>
#include <vector>
#pragma warning (disable : 4101)  // disable warning re unused variable x, below

namespace TypeAnal
{
	using namespace CodeAnalysis;
	struct contextInfo {
		std::string _type;
		std::string _typeName;
		int _level;
		contextInfo(std::string type, std::string typeName, int level) : _type(type), _typeName(typeName), _level(level) {}
	};

	class TypeAnalysis
	{
	public:
		using SPtr = std::shared_ptr<ASTNode*>;

		TypeAnalysis();
		void doTypeAnal();
		void printTypeTable();
		TypeTable returnTypeTable() { return typetable_; }
	private:
		void DFS(ASTNode* pNode, int treeLevel);
		AbstrSynTree& ASTref_;
		ScopeStack<ASTNode*> scopeStack_;
		std::vector<contextInfo> contextStack;
		Scanner::Toker& toker_;
		TypeTable typetable_;// instance of Type Table to store Types
	};
	/********<< Contructor - Intialize AST Tree, toker, scopestack>>****************/
	inline TypeAnalysis::TypeAnalysis() :
		ASTref_(Repository::getInstance()->AST()),
		scopeStack_(Repository::getInstance()->scopeStack()),
		toker_(*(Repository::getInstance()->Toker()))
	{
	}

	/********<<Global Function to check whether Type information is lamba,function,class...>>****************/
	inline bool doDisplay_(ASTNode* pNode)
	{
		static std::string toDisplay[] = {
			"function", "lambda", "class", "struct", "enum", "alias", "typedef"
		};
		for (std::string type : toDisplay)
		{
			if (pNode->type_ == type)
				return true;
		}
		return false;
	}

	/********<< Recursive Function to do DFS on AST >>****************/
	inline void TypeAnalysis::DFS(ASTNode* pNode, int treeLevel)
	{
		static std::string path = "";
		std::string fqn = "";
		if (pNode->path_ != path)
		{
			path = pNode->path_;
		}
		while (contextStack.size() > 1 && treeLevel <= contextStack.back()._level) {
			contextStack.pop_back();
		}
		contextInfo cI(pNode->type_, pNode->name_, treeLevel);

		if ((pNode->type_ == "function" || pNode->type_ == "lambda" )&&
			(contextStack.back()._type == "function" || contextStack.back()._type == "class" || contextStack.back()._type == "struct"))
		{	//do Nothing
		}else {
			contextStack.push_back(cI);
			//Getting Fully Qualified Name
			if (contextStack.size() > 1) {
				for (auto it = contextStack.begin() + 1; it != contextStack.end(); ++it) {
					fqn += it->_typeName;
					fqn += "::";
				}
			}
			std::string fqn_ = fqn.substr(0, fqn.length() - 2);
			//Push the element into type table
			if (doDisplay_(pNode)) {
				//std::cout << "\n ( Type: " << pNode->type_ << " , typeName: " << pNode->name_ << " , Fqn " << fqn_ << " )";
				typetable_.add(pNode->type_, pNode->name_, pNode->path_, fqn_);
			}
		}
		for (auto pChild : pNode->children_)
		{
			treeLevel++;
			DFS(pChild, treeLevel);
			treeLevel--;
		}
	}

	/********<<  Function to do Type Analysis on set of C++ files >>****************/
	inline void TypeAnalysis::doTypeAnal()
	{
		//std::cout << "\n  starting type analysis:\n";
		//std::cout << "\n  scanning AST and displaying Type Table:";
		//std::cout << "\n -----------------------------------------------";
		ASTNode* pRoot = ASTref_.root();
		DFS(pRoot, 0);
	}

	/********<<Function to print Type Table >>****************/
	inline void TypeAnalysis::printTypeTable() {
		std::cout << "Printing the Type table ...... ";
		std::cout<<typetable_.printTable();
	}

}