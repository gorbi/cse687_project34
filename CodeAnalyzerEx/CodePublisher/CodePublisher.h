#pragma once
#pragma warning (disable: 4503)
//////////////////////////////////////////////////////////////////////////
// CodePublisher.h - Publishes of source files.                         //
// Ver 1.0                                                              //
// Platform:    Apple Mac Mini Late 2014, Win 10 Education,             //
//              Visual Studio 2015 Enterprise                           //
// Author:      Nagaprasad Natarajaurs, 418910728                       //
//              nnataraj@syr.edu                                        //
//////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* Display source code files as web pages with embedded child links.
* Each link refers to a code file that the displayed code file depends on.
*
* Required Files:
* ---------------
*   - CodePublisher.h, CodePublisher.cpp,
*     NoSqlDb.h, NoSqlDb.cpp,
*     DependencyAnalysis.h, DependencyAnalysis.cpp,
*
* Build Process:
* --------------
*   devenv CodePublisher.sln /rebuild debug
*
* Maintenance History:
* --------------------
* Ver 1.0 : 13 Apr 2017
* - first release
*
*/

#include<string>
#include<fstream>
#include<stack>
#include<sstream>
#include<vector>
#include<iostream>
#include"../DependencyAnalysis/DependencyAnalysis.h"


class CodePublisher {

public:
	std::string htmlFilePath;
	CodePublisher(CodeAnalysis::DepAnal&);
	void fileList(std::vector<std::string>, std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>>&);
private:
	std::stack<std::string> CodePublisher::getFileNameOnly(const std::string, char);
	void createHtml(std::string, std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>>&, std::ofstream&);
	void printHtml(std::string, std::string, std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>>&);
	std::vector<std::string> getLinkedFiles(std::string);
	void showDependency(std::ofstream&, std::string, std::vector<std::string>);
	void printInitialHtmlContent(std::ofstream&, std::string);
	std::vector<int> getStartLineVect(std::vector<std::unordered_map<std::string, std::string>>);
	std::vector<int> getEndLineVect(std::vector<std::unordered_map<std::string, std::string>>);
	void writeButtonAndDiv(std::ofstream&, std::vector<std::string>);
	void writeHtmlEnd(std::ofstream&);
	void writeHtmlPrologue(std::string, std::ofstream&);
	void generateTags(std::ofstream&);
	std::vector<std::unordered_map<std::string, std::string>> removeElementsWithLineDiffOne(std::vector<std::unordered_map<std::string, std::string>>);
	void displayLineNoInfo(std::vector<std::unordered_map<std::string, std::string>>);
	void printIdx(std::string, std::ofstream&);
	void printIdxInitial(std::ofstream&);

	std::string htmlHeader = "<html>\n<head>\n<link rel=\"stylesheet\" type=\"text/css\" href=\"main.css\">\n<script src=\"main.js\">\n</script>\n</head>\n";
	std::vector<std::string> generateUniqueButtonAndDiv();
	int uniqueId = 0;
	CodeAnalysis::DepAnal& dpAnal;
	NoSqlDb<std::string> db;
	std::string indexStart = "<html>\n<head>\n<title>Main Page</title>\n</head>\n<body>\n";

};

//Default constructor also intitializes dependency analyzer
CodePublisher::CodePublisher(CodeAnalysis::DepAnal& dp) :dpAnal(dp) {
	std::cout << "\n\n";
	std::cout << "************************************************************\n";
	std::cout << "HTML files are present in path ..\\RemoteCodePublished\\CategoryX\n";
	std::cout << "************************************************************\n";
	std::cout << "**********************************************************************************************************\n";
	std::cout << "Requirement 5 : CSS file for styling is present in path: main.css\n";
	std::cout << "**********************************************************************************************************\n";
	std::cout << "Requirements 4 & 5 : JavaScript file for hiding and unhiding is present in path: main.js\n";
	std::cout << "**********************************************************************************************************\n";
	std::cout << "Requirement 6 : Linking to CSS and JS files:<head>\n<link rel=\"stylesheet\" type=\"text/css\" href=\"main.css\">\n<script src=\"main.js\">\n</script>\n</head>\n";
	std::cout << "**********************************************************************************************************\n";
	std::cout << "\n\n";
}

//Collects all files for HTML generation
void CodePublisher::fileList(std::vector<std::string> listOfFiles, std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>>& lMap)
{
	try {
		std::ifstream  src("..\\RemoteCodePublished\\css\\main.css", std::ios::binary);
		std::ofstream  dst(htmlFilePath + "main.css", std::ios::binary);
		dst << src.rdbuf();
	}
	catch (std::exception& exc)
	{
		std::cout << "\n  Exception caught: ";
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		std::cout << exMsg;
	}

	try {
		std::ifstream  src("..\\RemoteCodePublished\\js\\main.js", std::ios::binary);
		std::ofstream  dst(htmlFilePath + "main.js", std::ios::binary);
		dst << src.rdbuf();
	}
	catch (std::exception& exc)
	{
		std::cout << "\n  Exception caught: ";
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		std::cout << exMsg;
	}
	std::ofstream strBuff;
	strBuff.open(htmlFilePath + "index.htm");
	std::cout << "*************************************************************************************************\n";
	std::cout << "Requirement 9 : All important packages are published in ..\\RemoteCodePublished\\CategoryX\\index.htm file \n";
	std::cout << "*************************************************************************************************\n";
	printIdxInitial(strBuff);
	strBuff << "<ul style=\"list - style - type:disc\">";
	std::cout << "****************************************************************************************************************************************\n";
	std::cout << "Requirement 3 :Publisher program for creation of web pages that captures the content of a single C++ source code file, e.g., *.h or *.cpp\n";
	std::cout << "****************************************************************************************************************************************\n";
	if (listOfFiles.size() != 0) {
		for (std::string file : listOfFiles) {
			createHtml(file, lMap, strBuff);
		}
		strBuff << "</body>\n";
		strBuff << "</html>";
		strBuff.close();
		//std::string path = "file:///" + FileSystem::Path::getFullFileSpec(htmlFilePath + "index.htm");
		//std::string command("start \"\" \"" + path + "\"");//open start file i.e. home page
		//std::system(command.c_str());
	}
}


//Gets filename from absolute file path
std::stack<std::string> CodePublisher::getFileNameOnly(const std::string s, char delim) {
	std::stack<std::string> elems;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		if (item.length() > 0) {
			elems.push(item);
		}
	}
	return elems;
}

//Writes content as heading
void CodePublisher::printIdxInitial(std::ofstream& ostIndex) {
	ostIndex << indexStart;
	ostIndex << "<hr/>\n";
	ostIndex << "<h3>Contents</h3>\n";
	ostIndex << "<hr/>\n";
}

//Creates HTML
void CodePublisher::createHtml(std::string file, std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>>& lMap, std::ofstream& ostIndex)
{
	std::stack<std::string> fileComponents = getFileNameOnly(file, '\\');
	std::string fileName;
	if (fileComponents.size()) {
		fileName = fileComponents.top();
	}
	printIdx(fileName, ostIndex);
	printHtml(file, fileName, lMap);
}

//Escapes entities from each line and write
inline void writeLineEscapeEntities(std::ofstream& myfile, std::string line) {
	for (char& c : line) {
		switch (c) {
		case '"':
			myfile << "&quot;";
			break;
		case '&':
			myfile << "&amp;";
			break;
		case '\'':
			myfile << "&apos;";
			break;
		case '<':
			myfile << "&lt;";
			break;
		case '>':
			myfile << "&gt;";
			break;
		default:
			myfile << c;
		}
	}
	myfile << "\n";
}

//Writes character from the string to html file in between html 'pre' tags
//Preserves white spaces
void CodePublisher::printHtml(std::string fileToRead, std::string htmlFileName, std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>>& lMap)
{
	int counter = 1;
	std::vector<std::string> dependantFileVect = getLinkedFiles(fileToRead);
	std::vector<std::unordered_map<std::string, std::string>> lineInfoVal;
	std::vector<std::unordered_map<std::string, std::string>> processVect;
	std::vector<int> startLineVect;
	std::vector<int> endLineVect;
	if (lMap.find(fileToRead) != lMap.end()) {
		lineInfoVal = lMap[fileToRead];
		processVect = removeElementsWithLineDiffOne(lineInfoVal);
	}
	startLineVect = getStartLineVect(processVect);
	endLineVect = getEndLineVect(processVect);
	std::ofstream myfile(htmlFilePath + htmlFileName + ".htm");
	if (myfile.is_open())
	{
		myfile << htmlHeader << "<body>\n";
		writeHtmlPrologue(htmlFileName + ".htm", myfile);
		printInitialHtmlContent(myfile, fileToRead);
		std::cout << "Requirement 7 : Including links to dependent HTML files\n";
		showDependency(myfile, htmlFileName, dependantFileVect);
		generateTags(myfile);
		std::ifstream file(fileToRead);
		std::string str;
		myfile << "<pre>";
		myfile << "\n";
		while (std::getline(file, str))
		{
			for (int lineNo : startLineVect)
				if (counter == (lineNo + 1)) {
					std::vector<std::string> elem = generateUniqueButtonAndDiv();
					writeButtonAndDiv(myfile, elem);
				}
			for (int lineNo : endLineVect)
				if (counter == lineNo + 1)
					myfile << "</div>";
			writeLineEscapeEntities(myfile,str);
			counter++;
		}
		writeHtmlEnd(myfile);
	}
}

//Removes elements which differ by line count of 1
std::vector<std::unordered_map<std::string, std::string>> CodePublisher::removeElementsWithLineDiffOne(std::vector<std::unordered_map<std::string, std::string>> refineVect) {
	std::vector<std::unordered_map<std::string, std::string>>::iterator it = refineVect.begin();
	while (it != refineVect.end()) {
		std::unordered_map<std::string, std::string> mapInst = *it;
		if ((std::stoi(mapInst["endLine"]) - std::stoi(mapInst["startLine"])) == 1) {
			it = refineVect.erase(it);
		}
		else {
			it++;
		}
	}
	return refineVect;
}


//Creates an anchor tag for each HTML file linking
//it to its appropriate HTMl web page
void CodePublisher::printIdx(std::string htmlFileName, std::ofstream& ostIndex) {
	ostIndex << "<li>\n";
	ostIndex << "<a href=\"" + htmlFileName + ".htm" + "\">" + htmlFileName + ".htm" + "</a>\n";
	ostIndex << "</li>\n";
}

//Creates unique button and div tags
std::vector<std::string> CodePublisher::generateUniqueButtonAndDiv()
{
	std::vector<std::string> elemVect;
	std::string numInString = std::to_string(uniqueId);
	std::string button = "<button id=\"but" + numInString + "\" type=\"button\" onclick=\"expandCollapse(this)\">+</button>";
	std::string divElement = "<div class=\"noShow\" id=\"but" + numInString + "_div\">";
	elemVect.push_back(button);
	elemVect.push_back(divElement);
	uniqueId++;
	return elemVect;
}

//Returns the list of all files that the given file is dependent on
std::vector<std::string> CodePublisher::getLinkedFiles(std::string file)
{
	db = dpAnal.getDb();
	Element<std::string> elem = db.value(file);
	std::vector<std::string> dependantFileVect = elem.children;
	std::cout << "\n";
	std::cout << "File is: " << file << std::endl;
	std::cout << "\n";
	return dependantFileVect;
}

//Prints anchor tags and links to dependent files
void CodePublisher::showDependency(std::ofstream& ost, std::string fileNameOnly, std::vector<std::string> dependancyVect)
{
	ost << "<ul style=\"list - style - type:disc\">";
	ost << "\n";
	if (ost.is_open()) {
		for (std::string file : dependancyVect) {
			std::stack<std::string> fileStack = getFileNameOnly(file, '\\');
			std::string fNameOnly = fileStack.top();
			std::string depFile = "<li><a href=\"" + fNameOnly + ".htm" + "\">" + fNameOnly + "</a></li>";
			ost << depFile;
			ost << "\n";
		}
		ost << "</ul>";
		ost << "\n";
	}
}

//Prints initial HTML body content
void CodePublisher::printInitialHtmlContent(std::ofstream& ost, std::string file)
{
	std::stack<std::string> fileStack = getFileNameOnly(file, '\\');
	std::string fNameOnly = fileStack.top();
	std::string initBody = "<h3>" + fNameOnly + "</h3>\n<hr/>\n<div class = \"indent\">\n<h4>Dependencies:\n";
	ost << initBody;
	ost << "\n";
}

//Returns the list of all lines after which appropriate
//button and div tags are added
std::vector<int> CodePublisher::getStartLineVect(std::vector<std::unordered_map<std::string, std::string>> valueVect)
{
	std::vector<int> startLineNo;
	for (std::unordered_map<std::string, std::string> mapInst : valueVect) {
		std::string val = mapInst["startLine"];
		int iVal = std::stoi(val);
		startLineNo.push_back(iVal);
	}
	return startLineNo;
}

//Returns the list of all lines after which appropriate
//div end tags are added
std::vector<int> CodePublisher::getEndLineVect(std::vector<std::unordered_map<std::string, std::string>> valueVect)
{
	std::vector<int> endLineNo;
	for (std::unordered_map<std::string, std::string> mapInst : valueVect) {
		std::string val = mapInst["endLine"];
		int iVal = std::stoi(val);
		endLineNo.push_back(iVal);
	}
	return endLineNo;
}

//prints all button and div tags
//Done for classes,methods and global functions
void CodePublisher::writeButtonAndDiv(std::ofstream& ost, std::vector<std::string> elem) {
	for (std::string elemVal : elem) {
		ost << elemVal;
		ost << "\n";
	}
}

//Helper methid for generating tags
void CodePublisher::writeHtmlEnd(std::ofstream& myfile) {
	myfile << "</pre>";
	myfile << "\n";
	myfile << "</body>";
	myfile << "\n";
	myfile << "</html>";
	myfile.close();
}

//Used to print prologues for HTML files
void CodePublisher::writeHtmlPrologue(std::string htmlFileName, std::ofstream& ost)
{
	std::string htmlPro = " !----------------------------------------------------------------------------\n"
		+ htmlFileName + " - Help file for Project #3\n"
		"Published 14 Apr 2017\n"
		"Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017\n\n\n"
		"Note:\n"
		"-Markup characters in the text part, enclosed in &lt;pre&gt;...&lt;/pre&gt; have to be\n"
		"replaced with escape sequences, e.g., < becomes &lt and > becomes &gt\n"
		"-Be careful that you don't replace genuine markup characters with escape\n"
		"sequences, e.g., everything outside of the &lt;pre&gt;...&lt;/pre&gt; section.\n"
		"----------------------------------------------------------------------------->\n ";
	ost << "<pre>\n";
	ost << htmlPro;
	ost << "</pre>";
	ost << "\n";
}

//helper function for generating tags
void CodePublisher::generateTags(std::ofstream& myfile) {
	myfile << "</div>";
	myfile << "\n";
	myfile << "<hr/>";
	myfile << "\n";
}

//displays line information to identify beginning of classes, methods and global functions
void CodePublisher::displayLineNoInfo(std::vector<std::unordered_map<std::string, std::string>> vect)
{
	for (std::unordered_map<std::string, std::string> mapINst : vect) {
		std::cout << std::endl;
		for (auto it : mapINst) {
			std::cout << std::setw(20) << it.first << std::setw(20) << it.second << "\n";
		}
		std::cout << "\n\n";
	}
}