#pragma once
//////////////////////////////////////////////////////////////////////
// NoSqlDb.h - key/value pair in-memory database					//
// Version 1.0														//
// Application: NoSQL Database - CSE 687 Project 1					//
// Platform:    Asus R558U, Win 10 Student Edition, Visual studio 15//
// Author: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017//
//		   Nitesh Bhutani, CSE687, nibhutani@syr.edu				//
//////////////////////////////////////////////////////////////////////

/*
Package Operations:
==================

This package defines two classes Element class and NoSql class needed for
key/Value pair in-memory database. 

- Element Class defines the Data structure of elements which are stored
in our Database. Each Element consist of MetaData-string Type and Data. 
Element class is templateized around the type of Data stored in Database.
	
	Public Interface:
	================
	Property<Name> name;            // metadata - name
	Property<Category> category;    // metadata - category
	Property<TextDesc> textDesc;    // metadata - Text Desc
	Property<TimeDate> timeDate;    // metadata - Time data
	Property<Data> data;            // data for storage
	std::string show();             // to show the Element properties on console
	void addChild_(const std::string& child) // function to add single child to unordered set
	bool removeChild_(const std::string& child) //function to remove child from unordered set
	std::vector<std::string> getChildren()  //function to return vector of children in unordered set

-  NoSqlDb class is a key/value pair in-memory database 
which stores elment in the form of unordered map(key, Element<Data>).

	Public Interface :
	================
	Keys keys(); //Gives the list of keys 
	bool save(Key key, Element<Data> elem); // to save key/element pair
	bool del(Key key); // to delete element at key position
	bool updateValue(Key key, Data value); // to update data for Element in DB at key position
	bool updateElement(Key key, Element<Data> elem); //to replace existing value of
													   instance of Element with another
	bool addChild(Key key, const std::string& child); //for adding single children 
	bool addChildren(Key key, std::unordered_set<Key> children); //for adding multiple children 
	bool removeChild(Key key, Key child);//remove specific child 
	bool checkElement(Key key); // function to check whether certain key and its corresponding element is present in DB	
	Element<Data> value(Key key); // return Data at specific key
	size_t count(); // return size of DB
	void clearDB(); // to remove all elements of DB

Build Process:
==============
	Required files
	- CppProperties.h,CppProperties.cpp , Utilities.h, Utilities.cpp
	Build commands (either one)
	- devenv NoSQLDB-Project1.sln
	- //cl NoSqlDb.cpp 

Maintainence History :
====================
 - Version 1.0 : 4th Jan 2017
   First Release
 - Version 0.1 - fixed bug in NoSqlDb::count() by replacing
   unordered_map<key,Value>::count() with
   unordered_map<key,Value>::size();
 - Version 0.0 - Help Code provided by Dr. Fawcett

*/

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include "../CppProperties/CppProperties.h"

/////////////////////////////////////////////////////////////////////
// Element class represents a data record in our NoSql database
template<typename Data>
class Element
{
public:
	using Name = std::string;
	using Category = std::string;
	using TextDesc = std::string;
	using TimeDate = std::string;
	using Children = std::unordered_set<std::string>;


	Property<Name> name;            
	Property<Category> category;    
	Property<TextDesc> textDesc;    
	Property<TimeDate> timeDate;    
	Property<Data> data;            
	std::string show();				
	
	// function to add single child
	void addChild_(const std::string& child) {
		children.insert(child);
	}
	// function to remove single child
	bool removeChild_(const std::string& child) {
		if (children.find(child) != children.end())
		{
			children.erase(child);
			return true;
		}
		return false;
	}
	// Function to return vector of child keys
	std::vector<std::string> getChildren() {
		std::vector<std::string> _children;
		for (std::string child : children)
			_children.push_back(child);
		return _children;
	}
private:
	Children children; 
};

//----------<Show Function Implementation> ---------------------------
template<typename Data>
std::string Element<Data>::show()
{
	std::ostringstream out;
	int i = 1;
	out.setf(std::ios::adjustfield, std::ios::left);
	out << "\n    " << std::setw(8) << "name" << " : " << name;
	out << "\n    " << std::setw(8) << "category" << " : " << category;
	out << "\n    " << std::setw(8) << "textDesc" << " : " << textDesc;
	out << "\n    " << std::setw(8) << "timeDate" << " : " << timeDate;
	for (const auto& elem : children) {
		out << "\n    " << std::setw(8) << "children " << i << " : " << elem;
		i++;
	}
	out << "\n    " << std::setw(8) << "data" << " : " << data;
	out << "\n";
	return out.str();
}

/////////////////////////////////////////////////////////////////////
// NoSqlDb class is a key/value pair in-memory database
template<typename Data>
class NoSqlDb
{
public:
	using Key = std::string;
	using Keys = std::vector<Key>;
	using MetaType = std::string;
	using MetaValue = std::string;

	Keys keys(); 
	bool save(Key key, Element<Data> elem); 
	bool del(Key key); 
	bool updateValue(Key key, Data value); 
	bool updateMetadata(Key key, MetaType metaType, MetaValue metaValue);
	bool updateElement(Key key, Element<Data> elem); 
	bool addChild(Key key, const std::string& child); 
	bool addChildren(Key key, std::unordered_set<Key> children); 
	bool removeChild(Key key, Key child);
	bool checkElement(Key key); 
	Element<Data> value(Key key); 
	size_t count(); 
	void clearDB(); 

private:
	using Item = std::pair<Key, Element<Data>>;

	std::unordered_map<Key, Element<Data>> store; // unoredered map for key/Element in-memory Database
};

/*-------<  Deletes all records of DB >----------*/
template<typename Data>
void NoSqlDb<Data>::clearDB() {
	store.clear();
}
/*-------< For adding single children >----------*/
template<typename Data>
bool NoSqlDb<Data>::addChild(Key key, const std::string& child)
{
	Keys allkeys = keys();
	if (store.find(key) != store.end())
	{
		if (store.find(child) != store.end())//check child present in DB
		{
			store[key].addChild_(child);
			return true;
		}
	}
	return false;
}
/*----------------<Function for adding multiple children >-----------*/
template<typename Data>
bool NoSqlDb<Data>::addChildren(Key key, std::unordered_set<Key> children)
{
	if (store.find(key) != store.end())
	{
		for (Key child : children)
		{	if (store.find(child) != store.end()) {
				store[key].addChild_(child);
			}
		}
		return true;
	}
	return false;
}
/*--------<Remove specific child >---------------------*/
template<typename Data>
bool NoSqlDb<Data>::removeChild(Key key, Key child)
{
	if (store.find(key) != store.end())
	{
		store[key].removeChild_(child);//Element function checks if the child is present in set or not
		return true;
	}
	else { return false; }
}
/*-----------<Gives the list of keys present in DB>-------------------*/
template<typename Data>
typename NoSqlDb<Data>::Keys NoSqlDb<Data>::keys()
{
	Keys keys;
	for (Item item : store)
	{
		keys.push_back(item.first);//adding the keys
	}
	return keys;
}
/*-----------------<save element at key position>-------------*/
template<typename Data>
bool NoSqlDb<Data>::save(Key key, Element<Data> elem)
{
	if (store.find(key) != store.end())
		return false;
	store[key] = elem;
	return true;
}
/*----------------<Delete the element at Key Position>--------------------*/
template<typename Data>
bool NoSqlDb<Data>::del(Key key)
{
	if (store.find(key) != store.end())
	{	//check in rest of keys that this is not the child of any other key; if yes delete it
		Keys allkeys = keys();
		for (Key _key : allkeys) {
			store[_key].removeChild_(key);
		}
		store.erase(key);
		return true;
	}
	else {
		std::cout << "Element Not found!";
		return false;
	}
}
/*---------<Update the Data for element at key postion>-----------------*/
template<typename Data>
bool NoSqlDb<Data>::updateValue(Key key, Data value)
{
	if (store.find(key) != store.end())
	{
		(store[key]).data = value;
		return true;

	}
	else {
		std::cout << "Element Not found!";
		return false;
	}
}

/*---------<Update the metaData for element at key position>-----------------*/
template<typename Data>
bool NoSqlDb<Data>::updateMetadata(Key key, MetaType metaType, MetaValue value)
{
	if (store.find(key) != store.end())
	{
		if (metaType.compare("name") == 0) {
			(store[key]).name = value;
			return true;

		}
		else if (metaType.compare("category") == 0) {
			(store[key]).category = value;
			return true;

		}
		else if (metaType.compare("text description") == 0) {
			(store[key]).textDesc = value;
			return true;

		}
		else if (metaType.compare("timedate") == 0) {
			(store[key]).timeDate = value;
			return true;
			
		}
		else {
			std::cout << "Invalid MetaType !";
			return false;
		}

	}
	else {
		std::cout << "Element Not found!";
		return false;
	}
}

/*---------<This function is to replace existing value of data record instance with another>-----------------*/
template<typename Data>
bool NoSqlDb<Data>::updateElement(Key key, Element<Data> elem)
{
	if (store.find(key) != store.end())
	{
		store[key] = elem;
		return true;

	}
	else {
		std::cout << "Element Not found!";
		return false;
	}
}

/*---------<Return the Element at key position>-----------------*/
template<typename Data>
Element<Data> NoSqlDb<Data>::value(Key key)
{
	if (store.find(key) != store.end())
		return store[key];
	return Element<Data>();//giving an empty element

}
/*-------------<Check whether certain key/Element is present in DB>----------------------*/
template<typename Data>
bool NoSqlDb<Data>::checkElement(Key key)
{
	if (store.find(key) != store.end())
		return true; 
	else
	{
		return false;
	}
}
/*-----------<Return size of DB>--------------*/
template<typename Data>
size_t NoSqlDb<Data>::count()
{
	return store.size();
}

