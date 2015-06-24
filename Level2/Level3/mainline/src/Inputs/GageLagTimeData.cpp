// GageLagTimeData.cpp: implementation of the CGageLagTimeData class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GageLagTimeData.h"
#include "probab.h"
#include "../Common/termfile.h"
#include "../Common/ProbabilityDistribution.h"
#include "in_term.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGageLagTimeData::CGageLagTimeData()
{
}

CGageLagTimeData::CGageLagTimeData(const ProcessorID& id)
 : ProcessorDataElement(id)
{
}

CGageLagTimeData::~CGageLagTimeData()
{
}

void CGageLagTimeData::WriteToFile(ArctermFile& _file)const
{
	procID.writeProcessorID( _file );
	_file.writeLine();

	m_pDist->writeDistribution(_file);
	_file.writeLine();

}
void CGageLagTimeData::ReadFromFile(ArctermFile& _file,InputTerminal* _pInTerm)
{
	_file.getLine();
	procID.SetStrDict( _pInTerm->inStrDict );
	procID.readProcessorID(_file);

	_file.getLine();
	m_pDist = GetTerminalRelateProbDistribution (_file,_pInTerm);
}
