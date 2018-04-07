//////////////////////////////////////////////////////////////////////
// NoSqlDb.cpp - key/value pair in-memory database					//
// Version 1.0														//
// Application: NoSQL Database - CSE 687 Project 1					//
// Platform:    Asus R558U, Win 10 Student Edition, Visual studio 15//
// Author: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017//
//		   Nitesh Bhutani, CSE687, nibhutani@syr.edu				//
//////////////////////////////////////////////////////////////////////

#include "NoSqlDb.h"
#include "../Utilities/Utilities.h"
#include<string>
using StrData = std::string;
using intData = int;
using Key = NoSqlDb<StrData>::Key;
using Keys = NoSqlDb<StrData>::Keys;

//Widget structure/class data structure needed to test whether NoSql db can store widgets or not
class X {
public:
	StrData name;
	double doubleValue;
	std::vector<int> vectorValue;
};

/*------<Global Function - To output Class 'X' object stored in DB to console>---------*/
using obj = X;
std::string objShow(Element<X>& ObjElem)
{
	std::ostringstream out;
	int i = 1;
	out.setf(std::ios::adjustfield, std::ios::left);
	out << "\n    " << std::setw(8) << "name" << " : " << ObjElem.name;
	out << "\n    " << std::setw(8) << "category" << " : " << ObjElem.category;
	out << "\n    " << std::setw(8) << "textDesc" << " : " << ObjElem.textDesc;
	out << "\n    " << std::setw(8) << "timeDate" << " : " << ObjElem.timeDate;
	for (const auto& elem : ObjElem.getChildren()) {
		out << "\n    " << std::setw(8) << "children " << i << " : " << elem;
		i++;
	}
	out << "\n    " << std::setw(8) << "data.name " << " : " << ObjElem.data.getValue().name;
	out << "\n    " << std::setw(8) << "data.doubleValue " << " : " << ObjElem.data.getValue().doubleValue;
	i = 1;
	for (int elem : ObjElem.data.getValue().vectorValue) {

		out << "\n    " << std::setw(8) << "data.vectorValue " << i << " : " << elem;
		i++;
	}
	out << "\n";
	return out.str();
}

/////////////////////////////////////////////////////////////////////////////
// Test Functions

/*------<Test Function for creation DB>---------*/
NoSqlDb<StrData> createDB() {

	NoSqlDb<StrData> db;
	
	Element<StrData> elem1;
	elem1.name = "elem1";
	elem1.category = "test";
	elem1.data = "elem1's StrData";
	elem1.textDesc = "elem1 text desc";
	db.save(elem1.name, elem1);
	std::cout<<"\n db.addChild(\"elem1\", \"elem4\")";
	std::cout << "\n Trying to add elem2 , elem3, elem4 as child before adding these element to DB- This will fail as add child check \n element is present in db or not ";
	db.addChild("elem1", "elem4");
	for (int i = 2; i < 5; i++) {
		Element<StrData> elem2;
		elem2.name = "elem"+std::to_string(i);
		elem2.category = "test";
		elem2.data = "elem"+ std::to_string(i) +"'s StrData";
		elem2.textDesc = "elem"+ std::to_string(i) +" text desc";
		db.save(elem2.name, elem2);
	} 
	db.addChildren("elem1", { "elem2","elem3"});
	db.addChild("elem1", "elem4");
	return db;
}

/*-------<Global function to show all elements of DB>-------------*/
void showDB(NoSqlDb<StrData> db) {
	std::cout << "\n  size of db = " << db.count() << "\n";
	Keys keys = db.keys();
	for (Key key : keys)
	{
		std::cout << "\n  " << key << ":";
		std::cout << db.value(key).show();
	}
}

/*----------<Function to create DB with Object- class X object data record >---------*/
void createObjDb() {

	NoSqlDb<obj> odb;

	for (int i = 0;i < 2;i++) {
		
		Element<obj> oelem1;
		obj temp;
		temp.name = "Dr. Fawcett"+std::to_string(i);
		temp.doubleValue = 3.14 + 2*i;
		temp.vectorValue = { i,i+3,i+5 };

		oelem1.name = "oelem"+std::to_string(i);
		oelem1.category = "otest";
		oelem1.textDesc = " object element text desc";
		oelem1.timeDate = "3rd Feb 2017 element - "+std::to_string(i);
		oelem1.data = temp;

		odb.save(oelem1.name, oelem1);
	}
	
	odb.addChildren("oelem2", { "elem4","oelem1" });
	
	//Utilities::StringHelper::title("\n  Retrieving elements from NoSqlDb<X>");
	std::cout << "\n  size of odb = " << odb.count() << "\n";
	Keys okeys = odb.keys();
	for (Key key : okeys)
	{
		std::cout << "\n  " << key << ":";
		std::cout << objShow(odb.value(key));
	}
	std::cout << "\n\n";

}

//----< test stub >----------------------------------------------------------
int main()
{
	
	Utilities::StringHelper::Title("\n  Creating and saving NoSqlDb elements with string data");
	NoSqlDb<StrData> db = createDB();
	showDB(db);
	
	Utilities::StringHelper::Title("Removing children and Deleting the Element in DB of string");
	db.removeChild("elem1", "elem3");//First need to remove it from parents
	db.del("elem3"); 
	showDB(db);
	
	Utilities::StringHelper::Title("Updating the Element value in DB of string");
	db.updateValue("elem4", "elem4 Update Value");
	showDB(db);
	
	Utilities::StringHelper::Title("Updating the Element metaData in DB of string");
	db.updateMetadata("elem4", "category" ,"elem4 Update category");
	db.updateMetadata("elem4", "text description", "elem4 Update category");
	db.updateMetadata("elem4", "timedate", "14th Dec 2017");
	showDB(db);
	
	Utilities::StringHelper::Title( "Replacing existing element with another");
	Element<StrData> elem5;
	elem5.name = "elem5";
	elem5.category = "test5";
	elem5.data = "elem5's StrData";
	elem5.textDesc = "elem5 text desc";
	elem5.timeDate = "7th Feb 2017";
	db.updateElement("elem2",elem5);
	showDB(db);
	
	Utilities::StringHelper::Title("Creating and saving NoSqlDb elements with object data");
	createObjDb();
}