//////////////////////////////////////////////////////////////////////
// DepAnal.cpp - Package to do type based Dependency Analysis for	//
//				a given set of files based on TypeTable				//
// Version 1.0														//
// Application: Type Bases Dependecy Analysis - CSE 687 Project 2	//
// Platform:    Asus R558U, Win 10 Student Edition, Visual studio 15//
// Author:	Nitesh Bhutani, CSE687, nibhutan@syr.edu				//
//////////////////////////////////////////////////////////////////////

#include "DepAnal.h"
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <exception>

#include "../Parser/Parser.h"
#include "../FileSystem/FileSystem.h"
#include "../FileMgr/FileMgr.h"
#include "../Parser/ActionsAndRules.h"
#include "../Parser/ConfigureParser.h"
#include "../Tokenizer/Tokenizer.h"
#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../Analyzer/Executive.h"
#include "../TypeAnalysis/TypeAnalysis.h"
#include "../Parser/ActionsAndRules.h"
#include "../Persistence/Persistence.h"
#include "../Code Publisher/CodePublisher.h"
using namespace DependencyAnalysis;

using Path = std::string;
using File = std::string;
using Files = std::vector<File>;
using Pattern = std::string;
using Ext = std::string;
using FileMap = std::unordered_map<Pattern, Files>;
using Record = typeInfo;
using Records = std::vector<Record>;
using Key = std::string;
using Keys = std::vector<Key>;

///*********<		DepAnal Class Definations	>*******************/
//----< report number of files processed >-----------------------
size_t DepAnal::numFiles()
{
	return numFiles_;
}

//----< report number of directories searched >------------------
size_t DepAnal::numDirs()
{
	return numDirs_;
}

//----< returns reference to FileMap >---------------------------
/*
* Supports quickly finding all the files found with a give pattern
*/
FileMap& DepAnal::getFileMap()
{
	return fileMap_;
}

/*****<<	Constructor		>>********/
DepAnal::DepAnal(const FileMap& fm, const size_t& nmd, const size_t& nmf, const TypeTable& ta) : 
	fileMap_(fm), numDirs_(nmd), numFiles_(nmf), typeTable_(ta), ASTref_(CodeAnalysis::Repository::getInstance()->AST())
	{
	//Do nothing
	}

/******<<	Destructor	>>********/
DepAnal::~DepAnal(){//do nothing
}

/*******<< Function to remove preprocessor tokens from set of token >>************/
std::vector<std::string> DepAnal::removePreProcessorStatements(std::vector<std::string>& tok) {
	bool detectHash = false;
	std::vector<std::string> Token;
	for (auto it = tok.begin(); it != tok.end(); it++) {
		if (*(it) == "#")
			detectHash = true;
		if (detectHash == true && (*it) == "\n")
			detectHash = false;
		if (!detectHash) {
			Token.push_back(*it);
		}
	}
	return Token;
}

/*******<< Function to remove newLine and operators from vector of tokens >>************/
std::vector<std::string> DepAnal::removeNewLineAndOperators(std::vector<std::string>& tok) {
	std::vector<std::string> Token;
	std::vector<std::string> _oneCharTokens =
	{
		"\n", "<", ">", "{", "}", "[", "]", "(", ")", ":", ";", " = ", " + ", " - ", "*", ".", ",", "@"
	};
	std::vector<std::string> _twoCharTokens =
	{
		"<<", ">>", "::"  ,"++", "--", "==", "+=", "-=", "*=", "/="
	};
	auto it = tok.begin();
	while(it!= tok.end()) {
		bool _oneCharFound = std::any_of(_oneCharTokens.begin(), _oneCharTokens.end(), [=](std::string const& s) {return s == *it ;});
		bool _twoCharFound = std::any_of(_twoCharTokens.begin(), _twoCharTokens.end(), [=](std::string const& s) {return s == *it;});
		if (!_oneCharFound && !_twoCharFound)
			Token.push_back(*it);
		it++;
	}
	return Token;
}

/*******<<  Global helper Function for string manipulation to join ~ and destructor name >>************/
std::vector<std::string> joinDestructor(std::vector<std::string> token_) {
	unsigned  int it = 0;
	unsigned int size = token_.size();
	while (it < (size - 1))
	{
		if (token_[it] == "~") {
			token_[it] += token_[it + 1];
			token_.erase(token_.begin() + it +1 );
			size -= 1;
			continue;
		}
		else
			it++;
	}
	return token_;
}

/*******<< Global helper function for string manipulation to join namespace based ::>>************/
std::vector<std::string> joinNamespace(std::vector<std::string> token_) {
	unsigned  int it = 0;
	unsigned int size = token_.size();
	while (it < (size - 1))
	{
		if (token_[it] == "::") {
			token_[it - 1] += token_[it] + token_[it + 1];
			token_.erase(token_.begin() + it, token_.begin() + it +2);
			size -= 2;
			continue;
		}
		else
			it++;
	}
	return token_;
}

/*******<< Global helper function for string maniplation tp extract namespace based on deliminter>>************/
std::string extractNamespace(const std::string& s, std::string c){
	std::string returnString;
	std::vector<std::string> v;
	std::string::size_type i = 0;
	std::string::size_type j = s.find(c);
	while (j != std::string::npos) {
		v.push_back(s.substr(i, j - i));
		i = ++j;
		j = s.find(c, j);

		if (j == std::string::npos)
			v.push_back(s.substr(i, s.length()));
	}

	for (unsigned i = 0; i <= v.size() -2 ;i++) {
		returnString += v[i];
	}
	return returnString;
}

/*******<< Function to return set of dependency for a particular file >>************/
std::set<File> DepAnal::buildDependencyForFile(File file) {
	std::set<File> F;
	std::list<std::string> namespaceList_ = {}; // to store the list of namespaces used
	for (auto it = tokenVector_[file].begin(); it != tokenVector_[file].end();it++)//Building the list of namespaces used in the file checking
	{	if (*it == "namespace")
		{	namespaceList_.push_back(*(it + 1));}
	}
	for (auto it = tokenVector_[file].begin(); it != tokenVector_[file].end();it++)
	{
		if (it->find("::") != std::string::npos) { // check if namesapce/class i.e type is given with it
			if (typeTable_.containFQN(*it) != file && typeTable_.containFQN(*it) != "") {
				//std::cout << " \n token is : " << *it << " :::::";	std::cout << " File :" << typeTable_.containFQN(*it);
				F.insert(typeTable_.containFQN(*it));
			}
		}
		else {
			Records newRec;
			if (*it != "main") 
				newRec = typeTable_.FindName(*it);
			for (auto i = newRec.begin(); i != newRec.end(); i++) {
				std::string tokenNamespace;
				if (i->fQN_.find("::") != std::string::npos) {
					tokenNamespace = extractNamespace(i->fQN_, "::");
				}
				for (auto j = namespaceList_.begin(); j != namespaceList_.end(); j++) {		//First check whether that token present in the namespacelist_
					if(tokenNamespace.find(*j) != std::string::npos && i->location_ != file) {
						//std::cout << " \n token is : " << *it << " :::::";
						//std::cout << " File :" << i->location_;
						F.insert(i->location_);
					}
				}
				if (i->fQN_ == i->name_ && i->location_ != file) {	// Last check that token present in global namespace
					//std::cout << " \n token is : " << *it << " :::::";
					//std::cout << " File :" << i->location_;
					F.insert(i->location_);
					break;
				}
			}
		}
	}
	return F;
}

/*-------<Global function to show all elements of DB>-------------*/
void showDB(NoSqlDb<std::string> db) {
	std::cout << "\n  size of db = " << db.count() << "\n";
	Keys keys = db.keys();
	for (Key key : keys)
	{
		std::cout << "\n  " << key << ":";
		std::cout << db.value(key).show();
	}
}

/*******<< Function to build token Vector for a particular File >>************/
void DepAnal::buildTokenVector(File file) {
	std::ifstream in(file);
	if (!in.good())
	{
		std::cout << "\n  can't open " << file << "\n\n";
		throw std::exception("Cannot Open File for Tokenizig. ");
	}

	Scanner::Toker toker;
	toker.returnComments();
	toker.attach(&in);
	std::vector<std::string> toker_list;
	do
	{
		std::string tok = toker.getTok();
		if (tok.substr(0, 2) != "//" && tok.substr(0, 2) != "/*") //removing comments
			toker_list.push_back(tok);
	} while (in.good());
	
	toker_list=removePreProcessorStatements(toker_list); 	//cleaning pre-processor directive
	toker_list = joinDestructor(toker_list); 	//Joining the toker List
	toker_list = joinNamespace(toker_list); 	//Joining the toker List
	toker_list = removeNewLineAndOperators(toker_list);		//remove Newline and operator
	
	for (auto it = toker_list.begin(); it != toker_list.end(); it++) {
		tokenVector_[file].push_back(*it);
	}
}

/*******<< Function to check whether member function of class in header file is defined in cpp file or not>>************/
void DepAnal::checkHeaderDependecyonCpp(CodeAnalysis::ASTNode* pNode, int treelevel) {
	static int classLevel = 0;
	static std::string classLocation = "";
	if (pNode->type_ == "class" || pNode->type_ == "struct") {
		classLevel = treelevel;
		classLocation = pNode->path_;
	}
	if (pNode->type_ == "function" && treelevel > classLevel) {
		if (pNode->parentType_ == "namespace" && classLocation != pNode->path_)
			filePairVector.insert({ classLocation,pNode->path_ });
	}

	for (auto pChild : pNode->children_)
	{
		treelevel++;
		checkHeaderDependecyonCpp(pChild, treelevel);
		treelevel--;
	}

}

/*******<< Function to build Dependency Table Entries based on Filename>>************/
void DepAnal::buildTokenVectoAndDependecyTable() {
	 int i = 0;
	 for (auto it = fileMap_.begin(); it != fileMap_.end(); it++) {
		 for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			 buildTokenVector(*it2);
			 //Creating Dependecy Table
			 Element<std::string> e;
			 e.name = "File" + std::to_string(i);
			 e.category = "File";
			 e.data = *it2;
			 e.textDesc = "File " + std::to_string(i) + " Used for Analysis";
			 dependencytable.save(e.data, e);
			 i++;
		 }
	 }
 }

/*******<<Function to build Dependecy Table - call buildDependencyForFile() for each File >>************/
void DepAnal::buildDependencyTable() {
	std::set<File> files;
	for (auto it = fileMap_.begin(); it != fileMap_.end(); it++) {
		for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			files = buildDependencyForFile(*it2);
			for (auto i = files.begin(); i != files.end(); i++)
			{
				dependencytable.addChild(*it2, *i);
			}
		}
	}
	CodeAnalysis::ASTNode* pRoot = ASTref_.root();
	checkHeaderDependecyonCpp(pRoot, 0);
	for (auto i = filePairVector.begin(); i != filePairVector.end(); i++) {
		dependencytable.addChild(i->first,i->second);
	}
}

/*******<< Function to return Depdency Table >>************/
NoSqlDb<std::string> DepAnal::getDependecyTable() {
	return dependencytable;
}
using Rslt = Logging::StaticLogger<0>;  // use for application results
using Demo = Logging::StaticLogger<1>;  // use for demonstrations of processing
using Dbug = Logging::StaticLogger<2>;  // use for debug output

/***********<<		Test Stub		>>**************************/
#ifdef TEST_DEPANAL
#include <fstream>

 int main(int argc, char* argv[])
{	using namespace CodeAnalysis;
	CodeAnalysisExecutive exec_;
	try {bool succeeded = exec_.ProcessCommandLine(argc, argv);
		if (!succeeded)		{return 1;}
		exec_.setDisplayModes();
		exec_.startLogger(std::cout);
		exec_.getSourceFiles();
		exec_.processSourceCode(true);
		exec_.flushLogger();
		Rslt::write("\n");
		std::ostringstream out_;
		Rslt::write(out_.str());
		Rslt::write("\n");
		exec_.stopLogger();
		using namespace TypeAnal; ////////////////////////////Test Requirement 3
		TypeAnalysis ta;
		ta.doTypeAnal();
		std::cout << "\n ***********Requirement 4 - Type Analysis completed. **********";
		std::cout << "\n ================================================================ \n";
		ta.printTypeTable();
		//Test Requirement 4
		DepAnal depAnal(exec_.getFileMap(), exec_.numDirs(),exec_.numFiles(),ta.returnTypeTable());
		depAnal.buildTokenVectoAndDependecyTable();
		depAnal.buildDependencyTable();	
		NoSqlDb<std::string> db_ = depAnal.getDependecyTable(); 
		std::cout << "\n ***********Requirement 5 - Dependency Analysis Completed **********";
		std::cout << "\n ======================Printing Dependecy Table==========================================";
		showDB(db_);
		Persistence<std::string> pe(db_);
		std::string xmlString = pe.toXMLString();
		pe.writeFile("../DepAnal.xml", xmlString, false);
		//Test Requirement 6
		std::cout << "\n ***********Requirement 6 - Strong components **********";
		std::cout << "\n ================================================================";
		//Test Requirement 7
		std::cout << "\n \n ***********Requirement 7 - Analysis Result **********";
		std::cout << "\n Dependecy Analysis result and Strong Components Result are written in file " << argv[argc - 1] << ".";
		//Test Requirement 8
		std::cout << "\n\n ***********Requirement 8 - Command Line Arguemnts **********";
		std::cout << "\n Command line arguments are taken in format - TestExecutive.exe [Folder] [List of Patterns] [File to store analysis result]";
		
	}catch (std::exception& except)
	{	exec_.flushLogger();
		std::cout << "\n\n  caught exception in Executive::main: " + std::string(except.what()) + "\n\n";
		exec_.stopLogger();
		return 1;}
	return 0;}

#endif