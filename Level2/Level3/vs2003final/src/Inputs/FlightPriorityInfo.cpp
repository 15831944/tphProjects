// FlightPriorityInfo.cpp: implementation of the CFlightPriorityInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FlightPriorityInfo.h"
//#include "termplan.h"
//#include "termplandoc.h"

#include "in_term.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FlightPriorityInfo::FlightPriorityInfo() : DataSet(FlightPriorityInfoFile)
{

}

FlightPriorityInfo::~FlightPriorityInfo()
{

}

//extern CTermPlanApp theApp;
void FlightPriorityInfo::readData (ArctermFile& p_file)
{
	p_file.reset();
	p_file.skipLines (3);

	/*
	POSITION ps = theApp.m_pDocManager->GetFirstDocTemplatePosition();
	POSITION ps2 = theApp.m_pDocManager->GetNextDocTemplate(ps)->GetFirstDocPosition();
	CTermPlanDoc *pDoc = (CTermPlanDoc*)(theApp.m_pDocManager->GetNextDocTemplate(ps)->GetNextDoc(ps2));
    InputTerminal* inputTerminal = (InputTerminal*)&pDoc->GetTerminal();*/

    while (p_file.getLine())
	{
		//flight;
		//FlightConstraint flight;
		//flight.SetAirportDB( m_pInTerm->m_pAirportDB );
		//flight.readConstraint(p_file);

		char str[1024] = ""; 
		p_file.getSubField (str, ';');

		AirsideFlightType flight;
		flight.SetAirportDB(m_pInTerm->m_pAirportDB);
		flight.FormatDBStringToFlightType(str);
		

		//gate id;
		/*ProcessorID newProcID;
		newProcID.SetStrDict(m_pInTerm->inStrDict);*/
		ALTObjectID newObjID;
		newObjID.readALTObjectID(p_file);

		//define new flightGate
		FlightGate flightGate;
		flightGate.flight = flight;
		flightGate.procID = newObjID;

		m_vectFlightGate.push_back(flightGate);
	}
}

void FlightPriorityInfo::writeData (ArctermFile& p_file) const
{
	int nCount = m_vectFlightGate.size();
	for(int i = 0; i < nCount; i++)
	{
		//m_vectFlightGate[i].flight.writeConstraint(p_file);
		AirsideFlightType fltType = m_vectFlightGate[i].flight;
		CString strFlttype = fltType.getDBString();
		 p_file.writeField (strFlttype);

		m_vectFlightGate[i].procID.writeALTObjectID(p_file);
		p_file.writeLine();
	}
}

