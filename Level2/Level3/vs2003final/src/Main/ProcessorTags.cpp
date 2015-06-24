// ProcessorTags.cpp: implementation of the CProcessorTags class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "ProcessorTags.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const char* CProcessorTags::PROCTAGNAMES[] = {
	"Geographic Properties\n",
	"    Status\n",
	"    Name\n",
	"    Type\n",
	"    Queue\n",
	"    Fixed Queue\n",
	"    In Constraint\n",
	"    Out Constraint\n",
	"    AC Stand\n",
	"    Emergency Exit\n",
	"Behavioral Properies\n",
	"    Service Time\n",
	"    Process\n",
	"    Flows\n",
	"    Roster\n",
	"    Bag Device\n",
	"    Behaviors\n",
	"    Train Schedule\n",
	"    Moving Walkway\n",
	"    Elevator\n",
};

const int CProcessorTags::PROCTAGGEOGRAPHIC = 0;
const int CProcessorTags::PROCTAGBEHAVIORAL = 10;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcessorTags::CProcessorTags() : DataSet(ProcTagsFile)
{
}

CProcessorTags::~CProcessorTags()
{
}

void CProcessorTags::clear()
{
	m_bsOn.reset();
}

void CProcessorTags::initDefaultValues()
{
}

void CProcessorTags::readData (ArctermFile& p_file)
{
	p_file.getLine();

	int n;

	for(int i=0; i<PROCESSORTAGCOUNT; i++) {
		p_file.getInteger(n);
		m_bsOn.set(i, n ? true : false);
	}
}

void CProcessorTags::writeData (ArctermFile& p_file) const
{
	for(int i=0; i<PROCESSORTAGCOUNT; i++) {
		p_file.writeInt((int) m_bsOn.at(i));
	}
	p_file.writeLine();
}
