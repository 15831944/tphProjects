// Flight.cpp: implementation of the CFlight class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "Flight.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFlight::CFlight()
{
	m_sAirline = _T("");
	m_nArrID = -1;
	m_nDepID = -1;
	m_pACType = NULL;
	//m_pOrigin = NULL;
	//m_pDest = NULL;
}

CFlight::~CFlight()
{

}

IMPLEMENT_SERIAL(CFlight, CObject, 1);

void CFlight::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if(ar.IsStoring()) {
		ar << m_sAirline;
		ar << m_nArrID;
		ar << m_nDepID;
		ar << m_tArrTime;
		ar << m_tDepTime;
		//TODO: Serialize ACType, origin and dest
	}
	else {
		ar >> m_sAirline;
		ar << m_nArrID;
		ar << m_nDepID;
		ar << m_tArrTime;
		ar << m_tDepTime;
		//TODO: Serialize ACType, origin and dest
	}
}