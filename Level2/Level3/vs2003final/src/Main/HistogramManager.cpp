// HistogramManager.cpp: implementation of the CHistogramManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "HistogramManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CHistogramManager::CHistogramManager() : DataSet(HistogramInputFile)
{
	clear();
}

CHistogramManager::~CHistogramManager()
{
	clear();
}

	
void CHistogramManager::clear()
{
	m_vdbValue.clear();
}

void CHistogramManager::readData(ArctermFile& p_file)
{
	while (p_file.getLine() == 1)
	{
		// read the name
		double dbValue = 0.0;
		if (p_file.getFloat(dbValue) != 0 )
		{
			m_vdbValue.push_back(dbValue);
		}
	}
}

void CHistogramManager::readObsoleteData(ArctermFile& p_file)
{

}

void CHistogramManager::writeData(ArctermFile& p_file) const
{
	int iSize = m_vdbValue.size();
	if(iSize <= 0)
	{
		p_file.writeLine();
		return ;
	}

	for(int i = 0; i < iSize; i++)
	{
		p_file.writeFloat(m_vdbValue[i]);
		p_file.writeLine();
	}
}

void CHistogramManager::AddElement(const double& dbValue)
{
	m_vdbValue.push_back(dbValue);
}

double CHistogramManager::DeleteElement(int iIndex)
{
	ASSERT(iIndex >= 0 && iIndex < m_vdbValue.size());
	double d = m_vdbValue[iIndex];
	m_vdbValue.erase(m_vdbValue.begin()+iIndex);
	return d;
}

double CHistogramManager::ModifyElement(int iIndex, const double& dbValue)
{
	ASSERT(iIndex >= 0 && iIndex < m_vdbValue.size());
	double d = m_vdbValue[iIndex];
	m_vdbValue[iIndex] = dbValue;
	return d;
}
