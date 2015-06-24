// Sector.cpp: implementation of the CSector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "Sector.h"
#include "AirportsManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSector::CSector()
{
}

CSector::~CSector()
{
}

IMPLEMENT_SERIAL(CSector, CObject, 1);

void CSector::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if(ar.IsStoring()) {
	}
	else {
	}
}

void AddToList(CAIRPORTLIST& APList, CAirport* pAP)
{
	for(int i=0; i<APList.size(); i++) {
		if(APList[i]->m_sCode.CompareNoCase(pAP->m_sCode) == 0) { //found the airport
			return;
		}
	}
	APList.push_back(pAP);
}

void CSector::GetAirportList(CAIRPORTLIST& APList)
{
	for(int i=0; i<m_vAirportFilters.size(); i++) {
		CAIRPORTFILTER airportFilter = m_vAirportFilters[i];
		CAIRPORTLIST TempList;
		AIRPORTSMANAGER->GetAirportsByFilter(airportFilter, TempList);
		for(int j=0; j<TempList.size(); j++)
			AddToList(APList, TempList[j]);
		TempList.clear();
	}
}

void CSector::AddFilter(CAIRPORTFILTER airportFilter)
{
	m_vAirportFilters.push_back(airportFilter);
}