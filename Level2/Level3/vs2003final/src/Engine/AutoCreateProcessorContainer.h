// AutoCreateProcessorContainer.h: interface for the CAutoCreateProcessorContainer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOCREATEPROCESSORCONTAINER_H__81E4A77C_C903_4E4A_8B9A_EEE2D2A9D8F4__INCLUDED_)
#define AFX_AUTOCREATEPROCESSORCONTAINER_H__81E4A77C_C903_4E4A_8B9A_EEE2D2A9D8F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>

class Processor;
class CTermPlanDoc;
class ProcessorList;

typedef std::pair< Processor*, Processor* > ProcCopyPair;
typedef std::vector< ProcCopyPair > AutoProcList;
class CAutoCreateProcessorContainer  
{
private:
	std::vector<AutoProcList> m_vAllAutoProcList;
	int m_iBeUsedIndex;
public:
	CAutoCreateProcessorContainer();
	virtual ~CAutoCreateProcessorContainer();

	void clear();
	void init();
	void addAutoProc( const ProcessorList* _srcList, const ProcessorList* _copyList );
	int  addAutoProcToProcList( int _idx, CTermPlanDoc* _pDoc );
};

#endif // !defined(AFX_AUTOCREATEPROCESSORCONTAINER_H__81E4A77C_C903_4E4A_8B9A_EEE2D2A9D8F4__INCLUDED_)
