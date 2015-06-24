// ComparativeReportResultList.cpp: implementation of the CComparativeReportResultList class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ComparativeReportResultList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CComparativeReportResultList::CComparativeReportResultList()
{

}

CComparativeReportResultList::~CComparativeReportResultList()
{
	for(std::vector<CComparativeReportResult*>::iterator iter = m_vReport.begin(); iter!=m_vReport.end(); iter++)
	{
		delete (*iter);
	}
	m_vReport.clear();
}

void CComparativeReportResultList::LoadData()
{

}

void CComparativeReportResultList::Save()
{

}

const CmpReportResultVector& CComparativeReportResultList::GetReportResult() const
{
	return m_vReport;
}

void CComparativeReportResultList::GetReportResult( ComparativeReportType repType, std::vector<const CComparativeReportResult*>& _vResult) const
{
	for(CmpReportResultVector::const_iterator iter=m_vReport.begin(); iter!=m_vReport.end(); iter++)
	{
		if( (*iter)->GetReportType() == repType)_vResult.push_back( *iter );
	}
}

const CComparativeReportResult* CComparativeReportResultList::GetReportResult(int _nIdx) const
{
	if( _nIdx<0 || (unsigned)_nIdx >= m_vReport.size()) return NULL;
	return  m_vReport[ _nIdx ] ;
}

void CComparativeReportResultList::ClearReportResult()
{
	for(std::vector<CComparativeReportResult*>::iterator iter = m_vReport.begin(); iter!=m_vReport.end(); iter++)
	{
		delete (*iter);
	}
	m_vReport.clear();
}
