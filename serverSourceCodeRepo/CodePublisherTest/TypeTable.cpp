#include "TypeTable.h"

using Record = typeInfo;
using Records = std::vector<Record>;
using  Type = std::string;
using Name = std::string;
using Namespace = std::string;
using Location = std::string;

void TypeTable::add(const Type& type, const Name& name, const Location& location, const Namespace& fqn) {
	Record r;
	r.name_ = name;
	r.fQN_ = fqn;
	r.location_ = location;
	r.typeName_ = type;

	_records.push_back(r);
}

std::string typeInfo::show() {
	std::ostringstream out;
	out.setf(std::ios::adjustfield, std::ios::left);
	out << "\n    " << std::setw(8) << "Type" << " : " << typeName_;
	out << "\n    " << std::setw(8) << "Name" << " : " << name_;
	out << "\n    " << std::setw(8) << "Fully Qualified Name" << " : " << fQN_;
	out << "\n    " << std::setw(8) << "Location" << " : " << location_;
	out << "\n";
	return out.str();

}
Record& TypeTable::operator[](int n) {
	if (n < 0 || n >= int(_records.size()))  std::cout << "Invalid Arguments";
	return _records[n];

}
Record TypeTable::operator[](int n) const {
	if (n < 0 || n >= int(_records.size()))  std::cout << "Invalid Arguments";
	return _records[n];

}
Records TypeTable::FindName(const Name& name) {
	Records RecordwithName;
	auto iter = _records.begin();
	while (iter != _records.end()) {
		if (iter->name_ == name) {
			RecordwithName.push_back(*(iter));
		}
		iter++;
	}
	return RecordwithName;
}
Records TypeTable::FindType(const Type& type) {
	Records Recordwithtype;
	auto iter = _records.begin();
	while (iter != _records.end()) {
		if (iter->typeName_ == type) {
			Recordwithtype.push_back(*(iter));
		}
		iter++;
	}
	return Recordwithtype;
}

/*******<	Function to spilt string based on Deliminter	>*****/
void split(const std::string& s, std::string c,
	std::vector<std::string>& v) {
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
Records TypeTable::FindNamespace(const Namespace& nameSpace) {
	std::string nameSpace_;
	Records RecordwithNamespace;
	auto iter = _records.begin();
	while (iter != _records.end()) {
		nameSpace_ = "";
		std::vector<std::string> v;
		split(iter->fQN_, "::", v);
		
		for (int i = 0; i < int(v.size())-1; ++i) {
			nameSpace_ += v[i];
		}
		if (nameSpace_ == nameSpace) {
			RecordwithNamespace.push_back(*(iter));
		}
		iter++;
	}
	return RecordwithNamespace;
}
Records TypeTable::FindLocation(const Location& location) {
	Records RecordwithLocation;
	auto iter = _records.begin();
	while (iter != _records.end()) {
		if (iter->location_ == location) {
			RecordwithLocation.push_back(*(iter));
		}
		iter++;
	}
	return RecordwithLocation;
}
std::string TypeTable::containFQN(const Namespace& fqn) {
	auto iter = _records.begin();
	while (iter != _records.end()) {
		std::string typeName;
		if (iter->fQN_.find("::") != std::string::npos) {
			std::vector<std::string> v;
			split(iter->fQN_, "::", v);
			typeName = v.back();
			//std::cout<<
		}
		else {
			typeName = iter->fQN_;
		}
		if (fqn.find(typeName) != std::string::npos) {
			return iter->location_;
		}
		iter++;
	}
	return "";
}


std::string TypeTable::printTable() {

	std::ostringstream out;
	out.setf(std::ios::adjustfield, std::ios::left);
	for (Record rec : _records) {
		out << "\n    " << std::setw(8) << rec.show();
		out << "\n    " << std::setw(8) << "*****************************" ;
	}
	return out.str();
}

//test stub
#ifdef TEST_TYPETABLE
int main()
{
	
	TypeTable* tt = new TypeTable();
	
	tt->add("struct", "contextInfo", "C:\\Users\\Nitesh\\Documents\\Visual Studio 2015\\Projects\\CodeAnalysis\\Analyzer\\DepAnal.h\\DepAnal.h" , "CodeAnalysis::contextInfo ");
	tt->add("class", "TypeAnal", "C:\\Users\\Nitesh\\Documents\\Visual Studio 2015\\Projects\\CodeAnalysis\\Analyzer\\DepAnal.h\\DepAnal.h", "CodeAnalysis::TypeAnal ");
	tt->add("function", "doDisplay", "C:\\Users\\Nitesh\\Documents\\Visual Studio 2015\\Projects\\CodeAnalysis\\Analyzer\\DepAnal.h\\DepAnal.h", "CodeAnalysis::doDisplay ");
	tt->add("alias", "Sptr", "C:\\Users\\Nitesh\\Documents\\Visual Studio 2015\\Projects\\CodeAnalysis\\Analyzer\\DepAnal.h\\DepAnal.h", "CodeAnalysis::Sptr ");
	tt->add("enum", "Enum1", "C:\\Users\\Nitesh\\Documents\\Visual Studio 2015\\Projects\\CodeAnalysis\\Analyzer\\DepAnal.h\\DepAnal.h", "CodeAnalysis::Enum1 ");
	tt->add("function", "func1", "C:\\Users\\Nitesh\\Documents\\Visual Studio 2015\\Projects\\CodeAnalysis\\Analyzer\\Executive.cpp\\Executive.cpp", "AnalFileMgr::func1 ");
	tt->add("function", "func2", "C:\\Users\\Nitesh\\Documents\\Visual Studio 2015\\Projects\\CodeAnalysis\\Analyzer\\Executive.cpp\\Executive.cpp", "AnalFileMgr::func2 ");
	tt->add("function", "func3", "C:\\Users\\Nitesh\\Documents\\Visual Studio 2015\\Projects\\CodeAnalysis\\Analyzer\\Executive.cpp\\Executive.cpp", "AnalFileMgr::func3 ");
	tt->add("function", "main", "C:\\Users\\Nitesh\\Documents\\Visual Studio 2015\\Projects\\CodeAnalysis\\Analyzer\\Executive.h\\Executive.h", "AnalFileMgr::main ");
	tt->add("class", "AnalFileMgr", "C:\\Users\\Nitesh\\Documents\\Visual Studio 2015\\Projects\\CodeAnalysis\\Analyzer\\Executive.h\\Executive.h", "AnalFileMgr::AnalFileMngr ");
	tt->add("struct", "TypeAnal", "C:\\Users\\Nitesh\\Documents\\Visual Studio 2015\\Projects\\CodeAnalysis\\Analyzer\\Executive.h\\Executive.h", "AnalFileMgr::TypeAnal ");

	std::cout << "\n Getting the First Record : " << (*tt)[0].show();
	std::cout << "\n =================================================================================================";

	std::cout << "\n Getting the Records with specific NameSpace : " ;
	Records records = tt->FindNamespace("CodeAnalysis");
	for (Record r : records)
	{
		std::cout << r.show();
		std::cout << "\n -----------------------------------------------";
	}
	std::cout << "\n =================================================================================================";

	
	std::cout << "\n Getting the Records with specific Location : ";
	records = tt->FindLocation("C:\\Users\\Nitesh\\Documents\\Visual Studio 2015\\Projects\\CodeAnalysis\\Analyzer\\DepAnal.h\\DepAnal.h");
	for (Record r : records)
	{
		std::cout << r.show();
		std::cout << "\n -----------------------------------------------";
	}
	std::cout << "\n =================================================================================================";

	std::cout << "\n Getting the Records with specific Name : ";
	records = tt->FindName("contextInfo");
	for (Record r : records)
	{
		std::cout << r.show();
		std::cout << "\n -----------------------------------------------";
	}

	std::cout << "\n =================================================================================================";

	std::cout << "\n Getting the Records with specific Type : ";
	records = tt->FindType("function");
	for (Record r : records)
	{
		std::cout << r.show();
		std::cout << "\n -----------------------------------------------";
	}
	std::cout << "\n =================================================================================================";

	std::cout<< "\n Printing the Table :"<<tt->printTable();
	std::cout << "\n";
	return 0;
}

#endif