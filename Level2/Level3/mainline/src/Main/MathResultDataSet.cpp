// MathResultDataSet.cpp: implementation of the CMathResultDataSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MathResultDataSet.h"
#include "MathResultManager.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMathResultDataSet::CMathResultDataSet(int p_file)
	: DataSet(p_file)
{
	m_pvstrPros = NULL;
	m_pvResult = NULL;
}

CMathResultDataSet::~CMathResultDataSet()
{

}

void CMathResultDataSet::clear()
{
}

void CMathResultDataSet::readData(ArctermFile& p_file)
{
	if (m_pvResult == NULL || m_pvstrPros == NULL)
		return;

	char buf[256];

	// clear previous data
	m_pvResult->clear();
	m_pvstrPros->clear();

	// get process names from the first line.
	if (p_file.getLine() != 1)
		return;
	if (p_file.getField(buf, 255)) // the first string is "Name"
	{
		while (p_file.getField(buf, 255) != 0)
		{
			m_pvstrPros->push_back(buf);
		}
	}


	// get data
	while (p_file.getLine() == 1)
	{
		MathResultRecord result;
		p_file.getTime(result.time);
		for (int i = 0; i < (int)m_pvstrPros->size(); i++)
		{
			double dResult;
			p_file.getFloat(dResult);
			result.vProcResults.push_back(dResult);
		}
		
		m_pvResult->push_back(result);
	}
}

void CMathResultDataSet::readObsoleteData(ArctermFile& p_file)
{
}

void CMathResultDataSet::writeData(ArctermFile& p_file) const
{
	if (m_pvResult == NULL || m_pvstrPros == NULL)
		return;

	// write the process names on the first line
	p_file.writeField("Time");
	for (vector<string>::const_iterator iter = m_pvstrPros->begin();
		 iter != m_pvstrPros->end();
		 iter++)
	{
		p_file.writeField(iter->c_str());
	}

	p_file.writeLine();

	// TRACE("\n size:%d\n", m_pvResult->size());
	for (vector<MathResultRecord>::const_iterator iterRec = m_pvResult->begin();
		iterRec != m_pvResult->end();
		iterRec++)
	{
		p_file.writeTime(iterRec->time);

		for (vector<double>::const_iterator iterValue = iterRec->vProcResults.begin();
			 iterValue != iterRec->vProcResults.end();
			 iterValue++)
		{
			p_file.writeFloat(static_cast<float>(*iterValue));
		}

		p_file.writeLine();
	}
}