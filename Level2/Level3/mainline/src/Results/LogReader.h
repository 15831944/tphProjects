#pragma once
#include <vector>

class OutputTerminal;
class Terminal;
class LogReader
{
public:	
	static bool PrintPaxLog(OutputTerminal* pTerm, const std::vector<int>& mobId, const CString& _csProjPath,  const CString& outDir);

	static bool PrintBridgeLog(Terminal* pTerm, const CString& _csProjPath, const CString& outDir);
private:
	static CString getPaxEventFileName(CString initName, int fileIndex);
};

