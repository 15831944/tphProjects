// HistogramManager.cpp: implementation of the CHistogramManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HistogramManager.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CHistogramManager::CHistogramManager() : DataSet(HistogramInputFile)
{
	m_pvdbValue = NULL;
}

CHistogramManager::~CHistogramManager()
{
	m_pvdbValue = NULL;
}

	
void CHistogramManager::clear()
{
	if(m_pvdbValue != NULL)
	{
		m_pvdbValue->clear();
	}
}

void CHistogramManager::readData(ArctermFile& p_file)
{
	while (p_file.getLine() == 1)
	{
		// read the name
		double dbValue = 0.0;
		if (p_file.getFloat(dbValue) != 0 )
		{
			m_pvdbValue->push_back(dbValue);
		}
	}
}

void CHistogramManager::readObsoleteData(ArctermFile& p_file)
{

}

void CHistogramManager::writeData(ArctermFile& p_file) const
{
	int iSize = m_pvdbValue->size();
	if(iSize <= 0)
	{
		p_file.writeLine();
		return ;
	}

	for(int i = 0; i < iSize; i++)
	{
		p_file.writeFloat((float)m_pvdbValue->at(i));
		p_file.writeLine();
	}
}
/*
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
*/