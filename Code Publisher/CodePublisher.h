//////////////////////////////////////////////////////////////////////
// CodePublisher.h - Package to publish C++ source code in HTML format
// Version 1.0														//
// Application: Code Publisher - CSE 687 Project 3					//
// Platform:    Asus R558U, Win 10 Student Edition, Visual studio 15//
// Author:	Nitesh Bhutani, CSE687, nibhutan@syr.edu				//
//////////////////////////////////////////////////////////////////////

/*
Package Operations:
==================
1) CodePublisher class which publish the source code in HTML format for a given set of files and also show dependency for each file
It uses fixed CSS n JS for each web page
Public Interface :
================
CodePublisher(Path inputPath, Path outPath); //Constructor parameters - inputpath and outputpath- taken from command line options
bool handleCommandLineOption(int argc, char* argv[]); // Function to handle Command Line Options
void buildFileMap(); // function to go through all files and build FileMap
void publishSourceFiles(); // Main Function to publish all Source Files
std::string getHTMLSkeleton(); // Function to get HTMLSKeleton.txt and build the HTML around source code
void buildCSSStyleNHeading(std::string& HTMLString, std::string filePathName); // Function to fill CSS, JS and Heading of page in <Head> tag
void storeHTMLFile(std::string path, std::string HTMLString, std::string filePathName);// Function to store HTML on output path
bool buildDirectoryStructure();//Function to build directory structure similar to input directory structure on Output Path
void createCSSFile(); // Function to create styl e.css on CSSnJSSpath - output 
void createJSFile(); // Function to create script.js on CSSnJSSpath - output 
std::string getSourceCode(File fileName); // Function to get the source code from input path
void replaceAllOccurenceOfCharacter(std::string& str, std::string inputString, std::string outString); // Function to replace all '<' and '>'
std::string getDependencyCode(std::string fileName); // Function to get the dependecy for each file
lineMap parseFile(Path filename); // Function to parse file Paramter - filePath
bool isFunctionClassStruct(ASTNode* pNode); // Function to check that function is class/Struct/Function
void DepthFirstSearch(ASTNode* pNode, lineMap& lineMap_); // Function to do DFS on AST
void insertDiv(std::string& sourceCode, std::string id, int startLineCount, int endLineCount); // insert the div for expanding and collapsing of class/struct/function
Path returnInputPath() // return inputpath
Path returnOutputPath() // return output path
Path returnCSSandJSPath() //return CS n JSS path
Build Process:
==============
- CodePublisher.h, CodePublisher.cpp
*  - Parser.h, Parser.cpp, ActionsAndRules.h, ActionsAndRules.cpp
*  - ConfigureParser.h, ConfigureParser.cpp
*  - ScopeStack.h, ScopeStack.cpp, AbstrSynTree.h, AbstrSynTree.cpp
*  - ITokenCollection.h, SemiExp.h, SemiExp.cpp, Tokenizer.h, Tokenizer.cpp
*  - IFileMgr.h, FileMgr.h, FileMgr.cpp, FileSystem.h, FileSystem.cpp
*  - Logger.h, Logger.cpp, Utilities.h, Utilities.cpp
*
Build commands (either one)
- devenv "Code Publisher".sln
- cl CodePublisher.cpp

Maintainence History :
====================
- Version 1.0 : 2nd April 2017
First Release
*/

#pragma once

#include<string>
#include<vector>
#include<set>
#include <unordered_map>
#include "../FileMgr/FileMgr.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"
#include "../Parser/Parser.h"
#include "../Parser/ActionsAndRules.h"
#include "../Parser/ConfigureParser.h"

namespace Publisher
{
	using namespace CodeAnalysis;
	using namespace Scanner;

	class PublisherFileManager: public FileManager::FileMgr {

	public:
		using Path = std::string;
		using File = std::string;
		using Files = std::vector<File>;
		using Pattern = std::string;
		using Ext = std::string;
		using FileMap = std::unordered_map<Path, Files>;
		
		PublisherFileManager(const Path& rootDir,const Path& path, FileMap& fileMap);
		virtual void file(const File& f);
		virtual void dir(const Dir& d);
		virtual void done();
		size_t numFiles();
		size_t numDirs();
	private:
		Path d_;
		FileMap& fileMap_;
		bool display_;
		size_t numOfFiles;
		size_t numOfDirs;
		Path root_;
	};

	class CodePublisher
	{
	public:
		using Path = std::string;
		using Pattern = std::string;
		using Patterns = std::vector<Pattern>;
		using File = std::string;
		using Files = std::vector<File>;
		using FileMap = std::unordered_map<Path, Files>;
		using lineMap = std::unordered_map<std::string, std::pair<int, int>>;
		
		CodePublisher(Path inputPath, Path outPath);
		bool handleCommandLineOption(int argc, char* argv[]); // Fnction to handle Command Line Options
		void buildFileMap(); // function to go through all files and build FileMap
		void publishSourceFiles(); // Main Function to publish all Source Files
		std::string getHTMLSkeleton();
		void buildCSSStyleNHeading(std::string& HTMLString, std::string filePathName);
		void storeHTMLFile(std::string path, std::string HTMLString, std::string filePathName);
		bool buildDirectoryStructure();
		void createCSSFile();
		void createJSFile();
		std::string getSourceCode(File fileName);
		void replaceAllOccurenceOfCharacter(std::string& str, std::string inputString, std::string outString);
		std::string getDependencyCode(std::string fileName);
		lineMap parseFile(Path filename);
		bool isFunctionClassStruct(ASTNode* pNode);
		void DepthFirstSearch(ASTNode* pNode, lineMap& lineMap_);
		void insertDiv(std::string& sourceCode, std::string id, int startLineCount, int endLineCount);
		Path returnInputPath() { return path_; }
		Path returnOutputPath() { return outputRootPath; }
		Path returnCSSandJSPath() { return CSSnJSPath; }
		Path returnHTMLFile();
	private:
		std::string buildHrefTagForDependecy(std::vector<std::string> dependecyVector);
		FileMap fileMap_;
		Patterns patterns_;
		Path path_;
		size_t numOfFiles_;
		size_t numOfDirs_;
		Path CSSnJSPath;
		Path outputRootPath;
		Path HTMLFile;
	};

}
