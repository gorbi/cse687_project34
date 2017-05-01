#pragma once
//////////////////////////////////////////////////////////////////////////
// TypeAnalysis.h - Finds all the types and global functions defined in //
//                  each of a collection of C++ source files            //
// Ver 1.0                                                              //
// Platform:    Apple Mac Mini Late 2014, Win 10 Education,             //
//              Visual Studio 2015 Enterprise                           //
// Author:      Nagaprasad Natarajaurs, 418910728                       //
//              nnataraj@syr.edu                                        //
//////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* It is used to find all the types and global functions defined in
* each of a collection of C++ source files. It does this by building
* rules to detect: 
*       type definitions - classes, structs, enums, typedefs, & aliases. 
*       global function definitions
*       global data definitions
* and capture their fully qualified names and the files where they are
* defined. It uses the TypeTable (unordered map std c++ library) to
* store that information. 
*
* Required Files:
* ---------------
*   - ActionsAndRules.h, ActionsAndRules.cpp,
*     TypeAnalysis.h, TypeAnalysis.cpp
*
* Build Process:
* --------------
*   devenv TypeAnalysis.sln /rebuild debug
*
* Maintenance History:
* --------------------
* Ver 1.0 : 13 Mar 2017
* - first release
*
*/

#pragma warning (disable: 4503)
#include "../Parser/ActionsAndRules.h"
#include <iostream>
#include <stack>
#include <functional>

#pragma warning (disable : 4101)  // disable warning re unused variable x, below

namespace CodeAnalysis
{
	class TypeAnal
	{
	public:
		using SPtr = std::shared_ptr<ASTNode*>;

		TypeAnal();
		void doTypeAnal();
		void showTypeTable();
		std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>>& getGlobFuncMap();
		std::vector<std::string> returnDependentFiles(std::string);
		void displayLineNoInfo();
		std::unordered_map<std::string, std::vector<std::unordered_map <std::string, std::string>>>& getLineNumMap();
	private:
		void DFS(ASTNode*);
		void getGlobalFunctionInfo(ASTNode *);
		void storeGlobalFunction(ASTNode*);
		std::unordered_map<std::string, std::vector<std::unordered_map <std::string, std::string>>> typeTable;
		std::unordered_map<std::string, std::vector<std::unordered_map <std::string, std::string>>> lineDataMap;
		std::unordered_map<std::string, std::vector<std::unordered_map <std::string, std::string>>> funcDataMap;
		std::stack<std::string> namespaceStack;
		AbstrSynTree& ASTref_;
		ScopeStack<ASTNode*> scopeStack_;
		Scanner::Toker& toker_;
	};

	// Default constructor also initialises AST, ScopeStack and Toker
	inline TypeAnal::TypeAnal() :
		ASTref_(Repository::getInstance()->AST()),
		scopeStack_(Repository::getInstance()->scopeStack()),
		toker_(*(Repository::getInstance()->Toker()))
	{
		std::cout << "\n-----------------Requirements 3 & 4: Created TypeAnalysis package for Type Analysis-------------------\n\n";
		std::function<void()> test = [] { int x; };  // This is here to test detection of lambdas.
	}                                              // It doesn't do anything useful for dep anal.

	//Checks if node is either of class,struct or enum
	inline bool doDisplay(ASTNode* pNode)
	{
		static std::string toDisplay[] = {
			"class","function","struct", "enum"
		};
		for (std::string type : toDisplay)
		{
			if (pNode->type_ == type)
				return true;
		}
		return false;
	}

	inline bool checkIfLineStEnEqual(std::string start, std::string end)
	{
		return (std::stoi(start) == std::stoi(end));
	}

	//Depth first search over the abstract syntax tree & builds type table
	inline void TypeAnal::DFS(ASTNode* pNode)
	{
		static std::string path = "";
		if (pNode->path_ != path)
		{
			std::cout << "\n    -- " << pNode->path_ << "\\" << pNode->package_;
			path = pNode->path_;
		}
		if (doDisplay(pNode))
		{
			std::unordered_map<std::string, std::string> data;
			std::unordered_map<std::string, std::string> lineData;
			data["type"] = pNode->type_;
			data["filename"] = pNode->path_;
			data["namespace"] = namespaceStack.top();
			std::ostringstream ost;
			ost << pNode->startLineCount_;
			std::string lineNumber = ost.str();
			ost.str("");
			ost.clear();
			data["lineNum"] = lineNumber;
			ost << pNode->endLineCount_;
			std::string lineNumberEnd = ost.str();
			data["lineNumEnd"] = lineNumberEnd;
			bool eqCheck = checkIfLineStEnEqual(lineNumber, lineNumberEnd);
			lineData["startLine"] = lineNumber;
			lineData["endLine"] = lineNumberEnd;
			std::vector<std::unordered_map<std::string, std::string>> temp;
			if (!eqCheck) {
				if (lineDataMap.find(pNode->path_) == lineDataMap.end()) {
					temp.push_back(lineData);
				}
				else {
					temp = lineDataMap[pNode->path_];
					temp.push_back(lineData);
				}
				lineDataMap[pNode->path_] = temp;
			}
			temp.clear();
			if (typeTable.find(pNode->name_) == typeTable.end()) {
				temp.push_back(data);
			}
			else {
				temp = typeTable[pNode->name_];
				temp.push_back(data);
			}
			typeTable[pNode->name_] = temp;
		}
		if (pNode->type_ == "namespace") {
			namespaceStack.push(pNode->name_);
		}
		for (auto pChild : pNode->children_) {
			DFS(pChild);
		}
		if (pNode->type_ == "namespace") {
			namespaceStack.pop();
		}
	}

	//Returns list of all files from  type table
	//by checking to match the token against each entry in type table
	std::vector<std::string> TypeAnal::returnDependentFiles(std::string token) {
		std::vector<std::string> depFiles;
		if (typeTable.find(token) != typeTable.end()) {
			std::vector<std::unordered_map<std::string, std::string>> mapVect = typeTable[token];
			for (std::vector<std::unordered_map<std::string, std::string>>::iterator it = mapVect.begin(); it != mapVect.end(); it++) {
				std::unordered_map<std::string, std::string> mapForTok = *it;
				depFiles.push_back(mapForTok["filename"]);
			}
		}
		return depFiles;
	}

	//Saves global function info
	void TypeAnal::storeGlobalFunction(ASTNode* pNode)
	{
		std::vector<std::unordered_map<std::string, std::string>> globalFuncList;
		std::unordered_map<std::string, std::string> globalFuncMap;
		if (funcDataMap.find(pNode->name_) == funcDataMap.end()) {
			globalFuncMap["file"] = pNode->path_;
			std::ostringstream stringBuff;
			stringBuff << pNode->startLineCount_;
			std::string lineNum = stringBuff.str();
			stringBuff.str();
			stringBuff.clear();
			globalFuncMap["lineNumber"] = lineNum;
			stringBuff << pNode->endLineCount_;
			std::string endLine = stringBuff.str();
			globalFuncMap["lineNumberEnd"] = endLine;
			globalFuncList.push_back(globalFuncMap);
		}
		else {
			globalFuncList = funcDataMap[pNode->name_];
			globalFuncMap["file"] = pNode->path_;
			std::ostringstream ost;
			ost << pNode->startLineCount_;
			std::string linNum = ost.str();
			ost.str("");
			ost.clear();
			globalFuncMap["lineNumber"] = linNum;
			ost << pNode->endLineCount_;
			std::string endLine = ost.str();
			globalFuncMap["lineNumberEnd"] = endLine;
			globalFuncList.push_back(globalFuncMap);
		}
		funcDataMap[pNode->name_] = globalFuncList;
	}

	//Retrieve global function info
	void TypeAnal::getGlobalFunctionInfo(ASTNode * globalNode) {
		std::cout << "\n---------------------------------------\n";
		for (auto node : globalNode->children_) {
			if (node->type_ == "function") {
				if (node->name_ != "void")
					storeGlobalFunction(node);
			}
		}
		std::cout << "\n---------------------------------------\n";
	}

	//Retrieve the function mappings
	std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>>& TypeAnal::getGlobFuncMap()
	{
		return funcDataMap;
	}

	//Perform or start type analysis
	void TypeAnal::doTypeAnal()
	{
		std::cout << "\n  starting type analysis:\n";
		std::cout << "\n  scanning AST and displaying important things:";
		std::cout << "\n -----------------------------------------------";
		ASTNode* pRoot = ASTref_.root();
		DFS(pRoot);
		getGlobalFunctionInfo(Repository::getInstance()->getGlobalScope());
	}

	//Method to display line numbers for classes,methods and functions
	void TypeAnal::displayLineNoInfo()
	{
		for (auto it : lineDataMap) {
			std::cout << std::endl;
			std::cout << it.first;
			std::cout << std::endl;
			std::cout << "----------------------------------" << std::endl;
			std::vector<std::unordered_map<std::string, std::string>> valueVect = it.second;
			for (std::vector<std::unordered_map <std::string, std::string>>::iterator it = valueVect.begin(); it != valueVect.end(); it++) {
				std::unordered_map <std::string, std::string> inst = *it;
				for (auto item : inst) {
					std::cout << std::setw(20) << item.first << std::setw(20) << item.second << std::endl;
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
			std::cout << std::endl;
			std::cout << "-------------------------------------" << std::endl;
		}
	}

	std::unordered_map<std::string, std::vector<std::unordered_map <std::string, std::string>>>& TypeAnal::getLineNumMap()
	{
		return lineDataMap;
	}


}