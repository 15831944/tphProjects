#include "StdAfx.h"
#include "MyStackWalker.h"

#ifdef _DEBUG
#define SKIPSTACK 3
#else 
#define SKIPSTACK 2
#endif

void MyStackWalker::PrintStack( CFileOutput& outfile,int nDepth )
{
	nCurDepth =0;
	m_nPrintDepth = nDepth;
	m_pOutFile = &outfile;
	ShowCallstack();
}

MyStackWalker::MyStackWalker() : StackWalker(RetrieveSymbol||RetrieveLine) 
	,nCurDepth(0)
{
	m_pOutFile = NULL;
}

bool MyStackWalker::OnCallstackEntry( CallstackEntryType eType, CallstackEntry &entry )
{
	nCurDepth++;
	if(nCurDepth>SKIPSTACK + m_nPrintDepth)
		return false;

	if ( (eType != lastEntry) && (entry.offset != 0) && m_pOutFile && nCurDepth>SKIPSTACK )
	{				
		m_pOutFile->LogItem(nCurDepth - SKIPSTACK );
		m_pOutFile->LogSourceLine(entry.lineFileName,entry.lineNumber);
		m_pOutFile->Line();
	}	
	return true;
}
