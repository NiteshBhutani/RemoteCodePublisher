// TypeAnal
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <exception>
#include <iomanip>

#include "../Parser/Parser.h"
#include "../FileSystem/FileSystem.h"
#include "../FileMgr/FileMgr.h"
#include "../Parser/ActionsAndRules.h"
#include "../Parser/ConfigureParser.h"
#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../Analyzer/Executive.h"
#include "TypeAnalysis.h"

using Rslt = Logging::StaticLogger<0>;  // use for application results
using Demo = Logging::StaticLogger<1>;  // use for demonstrations of processing
using Dbug = Logging::StaticLogger<2>;  // use for debug output

#ifdef TEST_TYPEANAL

#include <fstream>

int main(int argc, char* argv[])
{
	using namespace CodeAnalysis;

	CodeAnalysisExecutive exec_;

	try {
		bool succeeded = exec_.ProcessCommandLine(argc, argv);
		if (!succeeded)
		{
			return 1;
		}
		exec_.setDisplayModes();
		exec_.startLogger(std::cout);

		exec_.getSourceFiles();
		exec_.processSourceCode(true);
		exec_.flushLogger();
		
		Rslt::write("\n");
		std::ostringstream out_;
		out_ << "\n  " << std::setw(10) << "searched" << std::setw(6) << exec_.numDirs() << " dirs";
		out_ << "\n  " << std::setw(10) << "processed" << std::setw(6) << exec_.numFiles() << " files";
		Rslt::write(out_.str());
		Rslt::write("\n");
		exec_.stopLogger();
		std::cout << "\n  Code Analysis completed";
		using namespace TypeAnal;
		TypeAnalysis ta;
		ta.doTypeAnal();
		ta.printTypeTable();
		
	}
	catch (std::exception& except)
	{
		exec_.flushLogger();
		std::cout << "\n\n  caught exception in Executive::main: " + std::string(except.what()) + "\n\n";
		exec_.stopLogger();
		return 1;
	}
	return 0;
}
#endif