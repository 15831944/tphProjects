#include "StdAfx.h"
#include ".\intermediatestandassign.h"

IntermediateStandAssign::IntermediateStandAssign(): DataSet(IntermediateStandAssignFile)
{
	m_bIntermediateStandAssign = false;
	m_bDifferentStand = false;
}

IntermediateStandAssign::~IntermediateStandAssign(void)
{
}

void IntermediateStandAssign::readData (ArctermFile& p_file)
{
	p_file.reset();
	p_file.skipLines (3);

	if (p_file.getLine())
	{
		if (p_file.isBlankField())
			return;

		int nMark = 0;
		p_file.getInteger(nMark); 
		
		m_bIntermediateStandAssign = nMark >0?true:false;
	}

	if (p_file.getLine())
	{
		if (p_file.isBlankField())
			return;

		int nMark = 0;
		p_file.getInteger(nMark); 

		m_bDifferentStand = nMark >0?true:false;
	}
}

void IntermediateStandAssign::writeData (ArctermFile& p_file) const
{
	int nMark = m_bIntermediateStandAssign ? 1:0;
	p_file.writeInt(nMark);
	p_file.writeLine();

	nMark = m_bDifferentStand ? 1:0;
	p_file.writeInt(nMark);
	p_file.writeLine();

}

void IntermediateStandAssign::clear()
{
}