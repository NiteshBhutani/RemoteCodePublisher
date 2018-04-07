//////////////////////////////////////////////////////////////////////
// CodePublisher.cpp - Package to publish C++ source code in HTML format
// Version 1.0														//
// Application: Code Publisher - CSE 687 Project 3					//
// Platform:    Asus R558U, Win 10 Student Edition, Visual studio 15//
// Author:	Nitesh Bhutani, CSE687, nibhutan@syr.edu				//
//////////////////////////////////////////////////////////////////////
/******
Maintainence History:-

1) Version 1.1 -

Changes made due to project 4 compilications :-
1) Changed buildCSSStyleNHeading() function Line 195 and 198 commented out and added line 196 and 199. Using CSS\\style.css or CSS\script.js for all published files
2) Changed buildHrefTagForDependecy() function Line 285 replaced with 286 to use only name of files in <a> tag as all files will be stored in outputRootPath
3) Changed storeHTMLFile() function line 212 relaced with 210 to store html file at root
4) Changes buildDirectoryStructure() function - Built HTML files Directory commented out

**********/

#pragma once

#include "CodePublisher.h"
#include<string>
#include<vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include "../FileMgr/FileMgr.h"
#include <Windows.h>
#include <algorithm>
#include "../XmlDocument/XmlDocument/XmlDocument.h"
#include "../XmlDocument/XmlElement/XmlElement.h"
#include "../Convert/Convert.h"
#include "../StrHelper.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"
#include "../Parser/Parser.h"
#include "../Parser/ActionsAndRules.h"
#include "../Parser/ConfigureParser.h"


using namespace XmlProcessing;
using namespace CodeAnalysis;
using namespace Scanner;


using sPtr = std::shared_ptr<AbstractXmlElement>;

/*******<	Global Function to spilt string based on Deliminter	>*****/
void split_(const std::string& s, std::string c, std::vector<std::string>& v) {
	std::string::size_type i = 0;
	std::string::size_type j = s.find(c);
	while (j != std::string::npos) {
		v.push_back(s.substr(i, j - i));
		i = j + c.size();
		j = j + 1;
		j = s.find(c, j);
		if (j == std::string::npos)
			v.push_back(s.substr(i, s.length()));
	}
}

/////////////////////////////////////////////////////////////////////
// PublisherFileManager class
// - Derives from FileMgr to make application specific file handler
//   by overriding FileMgr::file(), FileMgr::dir(), and FileMgr::done()

using Path = std::string;
using File = std::string;
using Files = std::vector<File>;
using Pattern = std::string;
using Ext = std::string;
using FileMap = std::unordered_map<Pattern, Files>;
using lineMap = std::unordered_map<std::string, std::pair<int, int>>;

using namespace Publisher;
//----< initialize application specific FileMgr >--------------------
/*
* - Accepts Publisher's path and fileMap by reference
*/
PublisherFileManager::PublisherFileManager(const Path& rootDir,const Path& path, FileMap& fileMap)
	: root_(rootDir),FileMgr(path), fileMap_(fileMap), numOfFiles(0), numOfDirs(0) {}

//----< override of FileMgr::file(...) to store found files >------

void PublisherFileManager::file(const File& f)
{
	File fqf = d_ + "\\" + f;
	Path nonQualifiedPath = d_.substr(root_.size());
	//Ext ext = FileSystem::Path::getExt(fqf);
	//Pattern p = "*." + ext;
	fileMap_["..\\Repository\\"+nonQualifiedPath].push_back(fqf);
	++numOfFiles;
}
//----< override of FileMgr::dir(...) to save current dir >----------

void PublisherFileManager::dir(const Dir& d)
{
	d_ = d;
	++numOfDirs;
}
//----< override of FileMgr::done(), not currently used >------------

void PublisherFileManager::done()
{
}
//----< returns number of matched files from search >----------------

size_t PublisherFileManager::numFiles()
{
	return numOfFiles;
}
//----< returns number of dirs searched >----------------------------

size_t PublisherFileManager::numDirs()
{
	return numOfDirs;
}

//----< Constructor parameters - inputpath and outputpath- taken from command line options >---------------
CodePublisher::CodePublisher(Path inputPath, Path outPath) {

	std::vector<std::string> OutputPathrootVector;
	std::vector<std::string> inputPathRootVector;
	outputRootPath = "";
	inputPath = FileSystem::Path::getFullFileSpec(inputPath);
	outPath = FileSystem::Path::getFullFileSpec(outPath);
	split_(outPath, "\\", OutputPathrootVector);
	split_(inputPath, "\\", inputPathRootVector);
	for (unsigned int i = 0; i < OutputPathrootVector.size() - 2; i++) {
		outputRootPath += OutputPathrootVector[i] + "\\";
	}
	if (inputPathRootVector[inputPathRootVector.size() - 1] == "")
		outputRootPath = outputRootPath + "Repository\\" + inputPathRootVector[inputPathRootVector.size() - 2];
	else
		outputRootPath = outputRootPath + "Repository\\" + inputPathRootVector[inputPathRootVector.size() - 1];
	CSSnJSPath = outputRootPath +  "\\CSS\\";

}
//----< Function to go through all files and build FileMap >---------------
void CodePublisher::buildFileMap()
{
	std::string inputFlePathRoot="";
	std::vector<std::string> inputFilePathVector;
	split_(path_, "\\", inputFilePathVector);
	for (unsigned int i = 0; i < inputFilePathVector.size() - 1; i++) {
		inputFlePathRoot += inputFilePathVector[i]+"\\";
	}
	PublisherFileManager fm(inputFlePathRoot,path_, fileMap_);
	for (auto patt : patterns_)
		fm.addPattern(patt);
	fm.search();
	numOfFiles_ = fm.numFiles();
	numOfDirs_ = fm.numDirs();
}

//----< Function to get HTMLSKeleton.txt and build the HTML around source code >---------------
std::string CodePublisher::getHTMLSkeleton() {
	std::string HTMLSkeleton = "";
	std::string line;
	std::ifstream myfile("HTMLSkeleton.txt");
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			HTMLSkeleton += line;
		}
		myfile.close();
	}

	else std::cout << "Unable to open  HTML skeleton file";

	return HTMLSkeleton;
	
}
//----< Function to create style.css on CSSnJSSpath - output >---------------
void CodePublisher::createCSSFile() {
	//Reading the CSS Style skeleton
	std::string CSSSkeleton = "";
	std::string line;
	std::ifstream myfile("CSSStyle.txt");
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			CSSSkeleton += line;
			CSSSkeleton += '\n';
		}
		myfile.close();
	}
	else std::cout << "Unable to open CSSStyle.txt file";
	//Storing the CSS Style in Output Root
	std::string CSSFileName = CSSnJSPath + "style.css";
	std::ofstream outfile(CSSFileName, std::ios::out);
	if (outfile)
	{
		outfile << CSSSkeleton;
		outfile.close();
	}
	else std::cout << "Unable to Store CSS File";
}
//----< Function to create script.js on CSSnJSSpath - output  >---------------
void CodePublisher::buildCSSStyleNHeading(std::string& HTMLString,std::string filePathName) {
	std::vector<std::string> filePathVector;
	size_t findCSSPATH = HTMLString.find("{{CSSPATH}}");
	//std::cout << "\n Find is" << findCSSPATH;
	//HTMLString.replace(findCSSPATH, 11, CSSnJSPath + "style.css");
	HTMLString.replace(findCSSPATH, 11,  "CSS\\style.css");
	size_t findJSPATH = HTMLString.find("{{JSPATH}}");
	//HTMLString.replace(findJSPATH, 10, CSSnJSPath + "script.js");
	HTMLString.replace(findJSPATH, 10, "CSS\\script.js");
	split_(filePathName, "\\", filePathVector);
	size_t findHEADINGPATH = HTMLString.find("{{FILEHEADING}}");
	HTMLString.replace(findHEADINGPATH, 15, filePathVector[filePathVector.size() - 1]);

}
//----< Function to store HTML on output path >---------------
void CodePublisher::storeHTMLFile(std::string path, std::string HTMLString, std::string filePathName) {
	std::vector<std::string> filePathVector;
	split_(filePathName, "\\", filePathVector);
	//mycode
	std::string HTMLFileName = outputRootPath + "\\" + filePathVector[filePathVector.size() - 1] + ".html";
	//---------------------
	//std::string HTMLFileName = path + "\\" + filePathVector[filePathVector.size() - 1] + ".html";
	std::ofstream myfile(HTMLFileName, std::ios::out);
	if (myfile)
	{
		myfile << HTMLString;
		myfile.close();
	}
	else std::cout << "Unable to Store HTML File";
}
//----< Function to build directory structure similar to input directory structure on Output Path >---------------
bool CodePublisher::buildDirectoryStructure() {

	//Building CSS Directory 
	std::string command = "mkdir \"" + CSSnJSPath + "\"";
	int result = system(command.c_str());
	////Building HTML Files Directory
	//for (auto it = fileMap_.begin(); it != fileMap_.end(); it++) {
	//	std::string command = "mkdir \"" + it->first+ "\"";
	//	//int result = CreateDirectory(command.c_str(),NULL);
	//	int result = system(command.c_str());
	//}
	system("cls");//Clearing the screen
	return true;
}
//----< Function to replace all '<' and '>' with &lt and &gt >---------------
void CodePublisher::replaceAllOccurenceOfCharacter(std::string& str, std::string inputString, std::string outString) {
	size_t found = str.find(inputString);
	if (found != std::string::npos) {
		str.replace(found,inputString.size(),outString);
		replaceAllOccurenceOfCharacter(str, inputString, outString);
	}
	
}
//----< Function to get the source code from input path >---------------
std::string CodePublisher::getSourceCode(File fileName) {
	std::string sourceCode = "";
	std::string line;
	std::ifstream myfile(fileName);
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			sourceCode += line;
			sourceCode += "\n";
		}
		myfile.close();
	}

	else {
		std::cout << "Unable to open Source Code file";
		return sourceCode;
	}
	//Remove '<' and '>' sign
	replaceAllOccurenceOfCharacter(sourceCode, "<", "&lt");
	replaceAllOccurenceOfCharacter(sourceCode, ">", "&gt");

	return sourceCode;

}
//----< Function to HREF tags arund dependecy files >---------------
std::string CodePublisher::buildHrefTagForDependecy(std::vector<std::string> dependecyVector) {
	//std::vector<std::string> dependecyHrefVector;
	std::string depAnchorTag = "";
	//Replace depndecy Vectory with output HTML file pages Link
	for (auto i = dependecyVector.begin(); i != dependecyVector.end(); i++) {
		size_t found = i->find(path_);
		i->replace(found, path_.size(), outputRootPath);
	}
	//Build Anchor tag
	for (auto i = dependecyVector.begin(); i != dependecyVector.end(); i++) {
		std::vector<std::string> sourceFileVector;
		split_(*i, "\\", sourceFileVector);
		*i += ".html";
		//depAnchorTag += "<a href = \"" + *i + "\">" + sourceFileVector[ sourceFileVector.size() -1 ]+"</a> \n";
		depAnchorTag += "<a href = \"" + sourceFileVector[sourceFileVector.size() - 1] +".html"+ "\">" + sourceFileVector[sourceFileVector.size() - 1] + "</a> \n";
	}
	return depAnchorTag;
}
//----< Function to get all the dependecy for each file >---------------
std::string CodePublisher::getDependencyCode(std::string fileName) {
	std::string dependencyCode = "";
	std::string xmlString = "";
	std::string line;
	std::ifstream myfile("../DepAnal.xml");
	if (myfile.is_open())
	{	while (getline(myfile, line))
		{	xmlString += line;		}
		myfile.close();
	}
	else dependencyCode = "Not Able to find Dependecy File genrated for this path. Please check if Dep Analysis ran properly!";
	//Parse XML
	XmlDocument doc(xmlString, XmlDocument::str);
	std::vector<File> depVector;
	//Getting all "Data" and "Children"
	std::vector<sPtr> descData = doc.descendents("Data").select();
	std::vector<sPtr> descChildren = doc.descendents("Children").select();
	unsigned int childrenNo = -1;
	for (unsigned i = 0; i < descData.size(); i++) {
		std::string data = trim(descData[i]->children()[0]->value());
		if (data == fileName) {
			childrenNo = i;
		}
	}
	if (childrenNo != -1) {
		for (sPtr child : descChildren[childrenNo]->children()) {
			std::string child_ = trim(child->children()[0]->value());
			depVector.push_back(child_);
		}
		dependencyCode = buildHrefTagForDependecy(depVector);
		//std::cout << "dependecy Code" << dependencyCode;
	}
	else
	{		dependencyCode = "No children for this File";	}
	return dependencyCode;
}
//----< Function to parse file Paramter - filePath >---------------
lineMap CodePublisher::parseFile(Path filename) {
	lineMap linemap;
	ConfigParseForCodeAnal configure;
	Parser* pParser = configure.Build();
	if (pParser)
	{
		if (!configure.Attach(filename))
		{
			std::cout << "\n  could not open file " << filename << std::endl;
			return linemap;
		}
	}
	else
	{
		std::cout << "\n\n  Parser not built\n\n";
		return linemap;
	}

	while (pParser->next())
		pParser->parse();
	std::cout << "\n";
	// show AST
	Repository* pRepo = Repository::getInstance();
	ASTNode* pGlobalScope = pRepo->getGlobalScope();
	DepthFirstSearch(pGlobalScope, linemap);
	return linemap;
}
//----< Function to check that function is class/Struct/Function >---------------
bool CodePublisher::isFunctionClassStruct(ASTNode* pNode) {
	static std::string toDisplay[] = {
		"function",  "class", "struct"
	};
	for (std::string type : toDisplay)
	{
		if (pNode->type_ == type)
			return true;
	}
	return false;
}
//DFS through AST
void CodePublisher::DepthFirstSearch(ASTNode* pNode, lineMap& lineMap_) {

	if (isFunctionClassStruct(pNode))
	{
		lineMap_[pNode->type_ + "_" + pNode->name_] = std::make_pair(pNode->startLineCount_, pNode->endLineCount_);
	}
	for (auto pChild : pNode->children_)
		DepthFirstSearch(pChild,lineMap_);
}
//----<  insert the div for expanding and collapsing of class/struct/function >---------------
void CodePublisher::insertDiv(std::string& sourceCode, std::string id, int startLineCount, int endLineCount) {
	int lineCount = 1;
	bool startExecuted = true;
	bool endExecuted = true;
	for (unsigned int i =0 ; i < sourceCode.size() ; i++)
	{
		if (lineCount == startLineCount && sourceCode[i] == '{' && startLineCount+1 != endLineCount && startExecuted == true) {
			sourceCode.insert(i-1, "<button onclick=\"toogleDiv('"+id+"');\"> + </button>	<div id = \""+ id +"\"> " );
			startExecuted = false;
		}
		if (lineCount == endLineCount  && sourceCode[i] == '\n' && startLineCount+1 != endLineCount && endExecuted == true) {
			sourceCode.insert(i, "</div>");
			endExecuted = false;

		}
		if (sourceCode[i] == '\n') {
		lineCount++;
		startExecuted = true;
		endExecuted = true;
		}
	}
}
//----< Function to create script.js on CSSnJSSpath - output >---------------
void CodePublisher::createJSFile() {
	//Reading the CSS Style skeleton
	std::string JSSkeleton = "";
	std::string line;
	std::ifstream myfile("JSScript.txt");
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			JSSkeleton += line;
			JSSkeleton += '\n';

		}
		myfile.close();
	}
	else std::cout << "Unable to open JScript.txt file";
	//Storing the CSS Style in Output Root
	std::string JSFileName = CSSnJSPath + "script.js";
	std::ofstream outfile(JSFileName, std::ios::out);
	if (outfile)
	{
		outfile << JSSkeleton;
		outfile.close();
	}
	else std::cout << "Unable to Store JS File";
}

//----< Main Function to publish all Source Files >---------------
void CodePublisher::publishSourceFiles() {
	buildDirectoryStructure();//Build the Directory structure
	createCSSFile();//Build the CSS File
	createJSFile();//Build the JS File
	for (auto it = fileMap_.begin(); it != fileMap_.end(); it++) {
		for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			std::string HTMLSkeleton = getHTMLSkeleton();
			buildCSSStyleNHeading(HTMLSkeleton, *it2);		//Fill HTML heading and CSS File
			lineMap linemap_ = parseFile(*it2);//parse file and get gunction and class defination lines
			std::string sourceCode = getSourceCode(*it2);//get Code
			for (auto i = linemap_.begin(); i != linemap_.end(); i++) {
				if(i->second.first != i->second.second)
					insertDiv(sourceCode,i->first, i->second.first, i->second.second);//Insert div
			}
			size_t findSourceCode = HTMLSkeleton.find("{{SOURCECODE}}");
			HTMLSkeleton.replace(findSourceCode, 14, sourceCode);
			//Fill Dependecy
			std::string dependecyCode = getDependencyCode(*it2);
			size_t findDepCode = HTMLSkeleton.find("{{DEPENDENCYLINKS}}");
			HTMLSkeleton.replace(findDepCode, 19, dependecyCode);
			storeHTMLFile(it->first, HTMLSkeleton,*it2);//Store HTML File
			}
	}

}
//----< Function to return HTML File >---------------
Path CodePublisher::returnHTMLFile() {
	if (HTMLFile.empty()) {
		std::vector<std::string> fileVector;
		split_(fileMap_[fileMap_.begin()->first][0], "\\", fileVector);
		HTMLFile = fileMap_.begin()->first +"\\" + fileVector[fileVector.size() - 1 ]+".html";
	}

	return HTMLFile;
}
//----< Function to handle Command Line Options >---------------
bool CodePublisher::handleCommandLineOption(int argc, char* argv[]) {
	if (argc < 2)
	{
		return false;
	}
	try {
		path_ = FileSystem::Path::getFullFileSpec(argv[1]);
		//std::string projectRoot(argv[0]);
		//buildCSSnJSPath(projectRoot,path_);
		if (!FileSystem::Directory::exists(path_))
		{
			std::cout << "\n\n  path \"" << path_ << "\" does not exist\n\n";
			return false;
		}
		for (int i = 2; i < argc ; ++i)
		{
			std::string sarg(argv[i]);
			if (sarg.find("--") != std::string::npos) {
				HTMLFile = sarg.substr(2);
			} else
				patterns_.push_back(argv[i]);
		}
		if (patterns_.size() == 0)
		{
			return false;
		}
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  command line argument parsing error:";
		std::cout << "\n  " << ex.what() << "\n\n";
		return false;
	}
	return true;
}

//----<<<<<<<<<<<TEST STUB >>>>>>>>>>>>---------------
#ifdef TEST_CODEPUBLISHER
int main(int argc, char* argv[]) {

	CodePublisher cp(argv[1],argv[0]);
	bool succeeded = cp.handleCommandLineOption(argc, argv);
	if (!succeeded) {
		return 1;
	}

	cp.buildFileMap();	
	cp.publishSourceFiles();
 	return 0;
}
#endif