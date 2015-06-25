// UsedProcInfo.cpp: implementation of the CUsedProcInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "UsedProcInfo.h"
#include "inputs\schedule.h"
#include "engine\terminal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUsedProcInfo::CUsedProcInfo(Terminal*_term,ProcessorID*_pProc)
{
	m_pTerm = _term;
	//m_pProc = _pProc; 
	m_ProcID = *_pProc;
}

CUsedProcInfo::~CUsedProcInfo()
{
	
}

char* CUsedProcInfo::getTile(int _baseIndex,char *pTitle)
{
	switch(_baseIndex) 
	{
	case 0:                    //FlightSchedule
		return getFlightScheduleTitle(pTitle);
	case 1:                    //Misdate(Behavor)
		return getMisdateTile(pTitle);
	case 2:                    //peopelMovers
		return getPeopleMoversTitle(pTitle);
	case 3:                    //PassengerFlow
		return getPassengerFlowTitle(pTitle);
	case 4:                    //StationPaxFlow
		return getStationPaxFlowTitle(pTitle);
	case 5:                    //SpecificFlightPaxFlow
		return getSpecificFlightPaxFlowTitle(pTitle);
	case 6:                    //ServiceTime
		return getServiceTimeTitle(pTitle);
	case 7:                    //ProcAssign
		return getProcAssignTile(pTitle);
	case 8:                    //MovingSideWalk
		return getMovingSideWalkTitle(pTitle);
	case 9:                   //MovingSideWalkPaxDist
		return getMovingSideWalkPaxDistTitle(pTitle);
	case 10:
		return getSubFlowTile(pTitle);
	case 11:
		return getPipeSideWalkTitle(pTitle);
	default:
		return "";
	}
}
bool CUsedProcInfo::getInfo(int _baseIndex,std::vector<CString>&_vector)
{
	bool bRet = false;
	switch(_baseIndex) 
	{
	case 0:                    //FlightSchedule
		return getFlightScheduleInfo(_vector);
	case 1:                    //Misdate(Behavor)
		return getMisdateInfo(_vector);
	case 2:                    //peopelMovers
		return getPeopleMoversInfo(_vector);
	case 3:                    //PassengerFlow
		return getPassengerFlowInfo(_vector);
	case 4:                    //StationPaxFlow
		return getStationPaxFlowInfo(_vector);
	case 5:                    //SpecificFlightPaxFlow
		return getSpecificFlightPaxFlowInfo(_vector);
	case 6:                    //ServiceTime
		return getServiceTimeInfo(_vector);
	case 7:                    //ProcAssign
		return getProcAssignInfo(_vector);
	case 8:                    //MovingSideWalk
		return getMovingSideWalkInfo(_vector);
	case 9:                   //MovingSideWalkPaxDist
		return getMovingSideWalkPaxDistInfo(_vector);
	case 10:
		return getSubFlowInfo(_vector);
	case 11:
		return getPipeSideWalkInfo(_vector);
	default:
		break;
	}
	return bRet;
}

bool CUsedProcInfo::getFlightScheduleInfo(std::vector<CString>&_vector)
{
	return m_pTerm->flightSchedule->getUsedProcInfo(m_ProcID,(InputTerminal*)m_pTerm,_vector); 
}

/*
void CUsedProcInfo::addProcToList(const Processor *_pProc)
{
}*/
