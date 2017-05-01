// CodePublisher

#include "CodePublisher.h"
#include "../TypeAnalysis/TypeAnalysis.h"
#include "../Analyzer/Executive.h"

#ifdef TEST_CODEPUBLISHER
int main() {
	CodeAnalysis::CodeAnalysisExecutive exec;
	CodeAnalysis::DepAnal dp(exec.getDepXMLPath(), 50);
	CodePublisher cPub(dp);
	CodeAnalysis::TypeAnal tb;
	std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>>& lineMapInst = tb.getLineNumMap();
	cPub.fileCollector(exec.getAllsubFiles(), lineMapInst);
}
#endif // TEST_CODEPUBLISHER
