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
struct typeInfo {

public: 
	using  Type = std::string;
	using Name = std::string;
	using FQN = std::string;
	using Location = std::string;

	Type typeName_;     // TypeName
	Name name_;    // Name
	FQN fQN_;    // Fully Qualified Name
	Location location_;  // Location
	std::string show();

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