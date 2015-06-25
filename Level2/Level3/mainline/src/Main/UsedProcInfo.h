// UsedProcInfo.h: interface for the CUsedProcInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USEDPROCINFO_H__748031EC_4D35_46A4_84B7_7D11508B3E48__INCLUDED_)
#define AFX_USEDPROCINFO_H__748031EC_4D35_46A4_84B7_7D11508B3E48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "inputs\in_term.h"
#include "inputs\SideMoveWalkDataSet.h"
#include "inputs\SideMoveWalkPaxDist.h"
//#include "inputs\schedule.h"
#include "inputs\procdata.h"
#include "inputs\srvctime.h"
#include "inputs\paxflow.h"
#include "Inputs\SubFlowList.h"
#include "inputs\assigndb.h"
#include "inputs\movrdata.h"
#include "inputs\SideMoveWalkPaxDist.h"
#include "inputs\SideMoveWalkDataSet.h"
#include "Inputs\PipeDataSet.h"
#include <vector>

class FlightSchedule;

static int UsedProcCheckCount =12;
class CUsedProcInfo  
{
public:
	CUsedProcInfo(Terminal*_term,ProcessorID*_pProc);
	virtual ~CUsedProcInfo();
private:
    InputTerminal *m_pTerm;
	ProcessorID m_ProcID;
	//std::vector<Processor *> procList;
public:
//	void addProcToList(const Processor * _pProc);
	char* getTile(int _baseIndex,char *_sTitle);
	bool getInfo(int _baseIndex,std::vector<CString>& _vector);
	int getCount(void){return UsedProcCheckCount;}
	ProcessorID& getInfoID(void){return m_ProcID;}
protected:
	char* getFlightScheduleTitle(char *pStr)
	{
		strcpy(pStr,"FlightSchedule :");
	    return pStr;
	}
	bool getFlightScheduleInfo(std::vector<CString>&_vector);
	
	char*getMisdateTile(char *pStr)
	{
		strcpy(pStr,"Behavior :");
		return pStr;
	}
	bool getMisdateInfo(std::vector<CString>& _vector)
	{return m_pTerm->miscData->IfUsedThisProcssor(m_ProcID,(InputTerminal*)m_pTerm,_vector);} 
	//} m_term::Misdate :getUsedProcInfo(vector<String>); }
	
	char* getServiceTimeTitle(char *pStr) {
		strcpy(pStr,"ServiceTime :");
		return pStr;
	}
	bool getServiceTimeInfo(std::vector<CString>& _vector)
	{return m_pTerm->serviceTimeDB->getUsedProInfo(m_ProcID,(InputTerminal*)m_pTerm,_vector);}
	
	char* getPassengerFlowTitle(char *pStr){
		strcpy(pStr,"PassengerFlow :");
		return pStr;
	}
	bool getPassengerFlowInfo(std::vector<CString>& _vector) 
	{return m_pTerm->m_pPassengerFlowDB->getUsedProInfo(m_ProcID,(InputTerminal*)m_pTerm,_vector);}
	
	bool getSubFlowInfo(std::vector<CString>& _vector)
	{
		return m_pTerm->m_pSubFlowList->getUsedProInfo(m_ProcID,(InputTerminal*)m_pTerm,_vector);
	}
	char* getSubFlowTile(char* pStr)
	{
		strcpy(pStr,"PROCESS:");
		return pStr;
	}

	char* getProcAssignTile(char *pStr){
		strcpy(pStr,"Roster :");
		return pStr;
		}
	bool getProcAssignInfo(std::vector<CString>& _vector)
	{return m_pTerm->procAssignDB->getUsedProInfo(m_ProcID,(InputTerminal*)m_pTerm,_vector);}
	
	char* getStationPaxFlowTitle(char *pStr){
		strcpy(pStr,"StationPaxFlow :");
		return pStr;
		}
	bool getStationPaxFlowInfo(std::vector<CString>&_vector)
	{return m_pTerm->m_pStationPaxFlowDB->getUsedProInfo(m_ProcID,(InputTerminal*)m_pTerm,_vector);}
	
	char* getMovingSideWalkPaxDistTitle(char *pStr) {	
		strcpy(pStr,"MovingSideWalkPaxDist :");
		return pStr;
	}
	bool getMovingSideWalkPaxDistInfo(std::vector<CString>&_vector)
	{return m_pTerm->m_pMovingSideWalkPaxDist->getUsedProInfo(m_ProcID,(InputTerminal*)m_pTerm,_vector);}
	
	char* getMovingSideWalkTitle(char *pStr) {	
		strcpy(pStr,"MovingSideWalk :");
		return pStr;
	}
	bool getMovingSideWalkInfo(std::vector<CString>&_vector)
	{return m_pTerm->m_pMovingSideWalk->getUsedProInfo(m_ProcID,(InputTerminal*)m_pTerm,_vector);}
	
	char* getPeopleMoversTitle(char *pStr){
		strcpy(pStr,"PeopleMovers :");
		return pStr;
	}

	//check sidewalk attach to pipe
	char* getPipeSideWalkTitle(char* pStr)
	{
		strcpy(pStr,"Pipe :");
		return pStr;
	}

	bool getPipeSideWalkInfo(std::vector<CString>& _vector)
	{
		return m_pTerm->m_pPipeDataSet->getUsedProInfo(m_ProcID,(InputTerminal*)m_pTerm,_vector);
	}

	bool getPeopleMoversInfo(std::vector<CString>&_vector)
	{return m_pTerm->peopleMovers->getUsedProInfo(m_ProcID,(InputTerminal*)m_pTerm,_vector);}

	char* getSpecificFlightPaxFlowTitle(char *pStr){
		strcpy(pStr,"SpecificFlightPaxFlow :");
		return pStr;
	}
	bool getSpecificFlightPaxFlowInfo(std::vector<CString>&_vector)
	{return m_pTerm->m_pSpecificFlightPaxFlowDB->getUsedProInfo(m_ProcID,(InputTerminal*)m_pTerm,_vector);}
};

#endif // !defined(AFX_USEDPROCINFO_H__748031EC_4D35_46A4_84B7_7D11508B3E48__INCLUDED_)
