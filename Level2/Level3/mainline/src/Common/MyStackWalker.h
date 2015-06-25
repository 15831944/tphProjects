#pragma once
#include "StackWalker.h"
#include "FileOutPut.h"



class MyStackWalker : public StackWalker
{
public:
	MyStackWalker();

	void PrintStack(CFileOutput& outfile,int nDepth);
	virtual bool  OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry);

	int nCurDepth;
	CFileOutput* m_pOutFile;
	int m_nPrintDepth;
};

