// ComparativeReportResult.cpp: implementation of the CComparativeReportResult class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ComparativeReportResult.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmpBaseReport::CCmpBaseReport()
{
	m_vSampleRepPaths.clear();
}

CCmpBaseReport::~CCmpBaseReport()
{
	m_vSampleRepPaths.clear();
}

void CCmpBaseReport::AddSamplePath(const std::vector<std::string>& _vSamplePaths)
{
	for(std::vector<std::string>::const_iterator iter = _vSamplePaths.begin(); iter!=_vSamplePaths.end(); iter++)
	{
		m_vSampleRepPaths.push_back( *iter );
	}
}

CString CCmpBaseReport::GetModelName() const
{
	CString strModel;
	if (m_vSimName.empty() == false)
	{
		for (unsigned i = 0; i < m_vSimName.size(); i++)
		{
			if (strModel.IsEmpty())
			{
				strModel = m_vSimName.at(i);
				continue;
			}

			strModel = strModel + "," + m_vSimName.at(i);
		}
	}
	return strModel;
}
