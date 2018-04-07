//////////////////////////////////////////////////////////////////////
// Persistence.cpp - To provide Test Stub for  Persistence package //
// Version 1.0														//
// Application: NoSQL Database - CSE 687 Project 1					//
// Platform:    Asus R558U, Win 10 Student Edition, Visual studio 15//
// Author:	Nitesh Bhutani, CSE687, nibhutan@syr.edu				//
//////////////////////////////////////////////////////////////////////

#include "Persistence.h"
#include "../Utilities/Utilities.h"

using StrData = std::string;
using intData = int;
using Key = Persistence<StrData>::Key;
using Keys = Persistence<StrData>::Keys;

/*------< Function for creation DB>---------*/
NoSqlDb<StrData> createDB() {

	NoSqlDb<StrData> db;

	Element<StrData> elem1;
	elem1.name = "elem1";
	elem1.category = "test";
	elem1.data = "elem1's language - c++";
	elem1.textDesc = "elem1 text desc";
	elem1.timeDate = "01/04/2017";
	db.save(elem1.name, elem1);

	for (int i = 2; i < 5; i++) {
		Element<StrData> elem2;
		elem2.name = "elem" + std::to_string(i);
		elem2.category = "test" + std::to_string(i);
		elem2.data = "elem" + std::to_string(i) + "'s language - Java";
		elem2.textDesc = "elem" + std::to_string(i) + " text desc";
		elem2.timeDate = "01/23/2017";
		db.save(elem2.name, elem2);
	}
	db.addChildren("elem1", { "elem2","elem3" ,"elem4" });
	db.addChildren("elem2", { "elem4" });
	db.addChildren("elem4", { "elem3","elem1" });
	db.addChildren("elem3", { "elem4","elem1","elem2" });

	db.updateMetadata("elem3", "timedate", "01/30/2017");
	db.updateMetadata("elem4", "timedate", "02/04/2017");
	db.updateValue("elem3", "elem3's language - c++");
	db.updateValue("elem4", "elem4's language - Java");

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

#ifdef TEST_PERSISTENCE
/*----------------<<<<Test Stub>>>>---------------------*/
int main() {


	Utilities::StringHelper::Title("\n  Creating and saving NoSqlDb elements with string data");
	NoSqlDb<StrData> db = createDB();
	showDB(db);
	
	//Persistence engine
	Persistence<StrData> pe(db);

	Utilities::StringHelper::Title("Creating the XML String of in-memory DB and storing in XML name 'example1.xml'");
	std::string xmlString = pe.toXMLString();
	
	std::cout<<"\n    "<<"Writing File  in non-append mode.......";
	pe.writeFile("../example1.xml", xmlString, false);
	std::cout << "\n    " << "Again Writing File in append mode.......";
	pe.writeFile("../example1.xml", xmlString, true);

	Utilities::StringHelper::Title("Reading XML-'example.xml' and storing it into in-memory DB");
	std::string xml = pe.readFile("../ReadXmlString.xml");
	
	std::cout << "\n    " << "Storing XML in DB in append mode.......";
	pe.fromXMLString(xml,true);
	showDB(db);
	
	std::cout << "\n    " << "Storing XML in DB in non-append mode.......";
	pe.fromXMLString(xml, false);
	showDB(db);

	std::cout << "\n\n";

}

#endif