#pragma once
/////////////////////////////////////////////////////////////////////////
// TypeAnalysis.h -  //
//                                                                     //
// Nitesh Bhutani, CSE687 - Object Oriented Design, Spring 2017           //
/////////////////////////////////////////////////////////////////////////
/*
* You need to provide all the manual and maintenance informnation
*/

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

	inline TypeAnalysis::TypeAnalysis() :
		ASTref_(Repository::getInstance()->AST()),
		scopeStack_(Repository::getInstance()->scopeStack()),
		toker_(*(Repository::getInstance()->Toker()))
	{
		//std::function<void()> test = [] { int x; };  // This is here to test detection of lambdas.
	}                                              // It doesn't do anything useful for dep anal.

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

	inline void TypeAnalysis::doTypeAnal()
	{
		std::cout << "\n  starting type analysis:\n";
		//std::cout << "\n  scanning AST and displaying Type Table:";
		std::cout << "\n -----------------------------------------------";
		ASTNode* pRoot = ASTref_.root();
		DFS(pRoot, 0);
	}

	inline void TypeAnalysis::printTypeTable() {
		std::cout<<typetable_.printTable();
	}
}