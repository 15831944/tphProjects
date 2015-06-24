// BagDeliveryTimeReport.cpp: implementation of the CBagDeliveryTimeReport class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "Reports.h"
#include "BagDeliveryTimeReport.h"
#include "common\termfile.h"
#include "rep_pax.h"
#include "results\Paxlog.h"
#include "inputs\schedule.h"
#include "StatisticalTools.h"
#include "engine\terminal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBagDeliveryTimeReport::CBagDeliveryTimeReport(Terminal* _pTerm, const CString& _csProjPath)
	:CBaseReport(_pTerm, _csProjPath)
{

}

CBagDeliveryTimeReport::~CBagDeliveryTimeReport()
{

}


void CBagDeliveryTimeReport::ReadSpecifications(const CString& _csProjPath)
{
	//general operations
    char specFileName[128];
    PROJMANAGER->getSpecFileName( _csProjPath, GetSpecFileType(), specFileName );

    if (PROJMANAGER->fileExists (specFileName))
    {
        ArctermFile specFile;
        specFile.openFile (specFileName, READ);
        specFile.getLine();
        specFile.getTime (m_startTime);
        specFile.getTime (m_endTime);
        if (!specFile.eol() && m_usesInterval)
            specFile.getTime (m_interval);

        specFile.getLine();
        if (!specFile.isBlank())
		{
			ASSERT(m_pTerm);
			m_multiConst.readConstraints(specFile,m_pTerm);
		}
	}
}

void CBagDeliveryTimeReport::GenerateDetailed(ArctermFile& p_file)
{
    p_file.writeField ("Pax #,Bag Count,Delivery Time,Passenger Type");
    p_file.writeLine();

	CProgressBar bar( "Generating Bag Delivery Report", 100, m_pTerm->paxLog->getCount(), TRUE );
    
	ReportPaxEntry bagEntry;
	ReportPaxEntry ownerEntry;
	char str[200];
	
	
	bagEntry.SetOutputTerminal(m_pTerm);
	bagEntry.SetEventLog(m_pTerm->m_pMobEventLog);

	ownerEntry.SetOutputTerminal(m_pTerm);
	ownerEntry.SetEventLog(m_pTerm->m_pMobEventLog);
	
	long paxCount = m_pTerm->paxLog->getCount();
    for (long i = 0; i < paxCount; ++i)
    {
		bar.StepIt();

		m_pTerm->paxLog->getItem(bagEntry, i);
		bagEntry.clearLog();
		//// TRACE("\n id = %d\n", bagEntry.getID() );
		//// TRACE("\n index = %d\n", bagEntry.getIndex() );
		if (bagEntry.GetMobileType() != 2 )
			continue;

		if( bagEntry.getArrFlight() >= 0)
		{
			if (!m_pTerm->paxLog->GetItemByID(ownerEntry,bagEntry.getOwnerID()))
				continue;

			if (ownerEntry.fits(m_multiConst) == 0)
				continue;

			p_file.writeInt(bagEntry.getID());
			ownerEntry.printFullType(str);
			p_file.writeInt(bagEntry.getOwnerID());
			p_file.writeField(str);
		//	int iFlightIdx = bagEntry.getFlightIndex();
			int iFlightIdx = bagEntry.getArrFlight();
			Flight *aFlight = m_pTerm->flightSchedule->getItem (iFlightIdx);
			ASSERT( aFlight );
			ElapsedTime timeLagTime = bagEntry.getEntryTime() - aFlight->getArrTime();
			p_file.writeTime( timeLagTime );
			p_file.writeLine();		
		}
    }
}

void CBagDeliveryTimeReport::GenerateSummary(ArctermFile& p_file)
{
    p_file.writeField ("Passenger Type,Minimum,Maximum,Average,Count");
    p_file.writeLine();

    char str[MAX_PAX_TYPE_LEN];
    const CMobileElemConstraint *aConst;
	ReportPaxEntry bagEntry;
	ReportPaxEntry ownerEntry;
	bagEntry.SetOutputTerminal(m_pTerm);
	bagEntry.SetEventLog(m_pTerm->m_pMobEventLog);
	ownerEntry.SetOutputTerminal(m_pTerm);
	ownerEntry.SetEventLog(m_pTerm->m_pMobEventLog);

	long paxCount = m_pTerm->paxLog->getCount();
	long i = 0, j = 0, k = 0;

	int paxTypeCount = m_multiConst.getCount();
	ElapsedTime **calcTime;
	long *count = new long[paxTypeCount];
	calcTime = new ElapsedTime*[paxTypeCount];

	CStatisticalTools< ElapsedTime >* pTools = new CStatisticalTools< ElapsedTime >[ paxTypeCount ] ;
	for(i = 0; i < paxTypeCount; i++)
	{
		calcTime[i] = new ElapsedTime[3];
		calcTime[i][0] = 8640000l;	//(24 * 60 * 60 * 100)L; minimum	time
		calcTime[i][1] = 0l;		//maximum	time
		calcTime[i][2] = 0l;		//average	time
		count[i] = 0l;
	}

	
	CProgressBar bar( "Generating Bag Delivery Report", 100, m_pTerm->paxLog->getCount(), TRUE );
    for (i = 0; i < m_multiConst.getCount(); i++)
    {	
		aConst = m_multiConst.getConstraint(i);
		for(j = 0; j < paxCount; j++)
		{
			m_pTerm->paxLog->getItem(bagEntry, j);
			bagEntry.clearLog();
			if( bagEntry.GetMobileType() == 2 )// is bag
			{
				for(k = 0; k < paxCount; k++)
				{
					m_pTerm->paxLog->getItem(ownerEntry, k);
					ownerEntry.clearLog();
					if(ownerEntry.getID() == bagEntry.getOwnerID())
						break;
				}
				if(ownerEntry.isDeparture())
					continue;
				if(!ownerEntry.fits(*aConst))
					continue;

				int iFlightIdx = bagEntry.getFlightIndex();
				Flight *aFlight = m_pTerm->flightSchedule->getItem (iFlightIdx);
				ASSERT( aFlight );
				ElapsedTime timeLagTime = bagEntry.getEntryTime() - aFlight->getArrTime();
			
				pTools[ i ].AddNewData( timeLagTime );
				//ElapsedTime tempTime = bagEntry.getEntryTime();
				if(timeLagTime < calcTime[i][0])
					calcTime[i][0] = timeLagTime;
				if(timeLagTime > calcTime[i][1])
					calcTime[i][1] = timeLagTime;
				calcTime[i][2]+=timeLagTime;
				count[i]++;
			}
			
			
		}
		calcTime[i][2] = calcTime[i][2]/count[i];
    }	
	for(i = 0; i < m_multiConst.getCount(); i++)
	{
		pTools[ i ].SortData();

        aConst = m_multiConst.getConstraint(i);
        aConst->screenPrint (str, 0, 37);
        p_file.writeField (str);
        p_file.writeTime (calcTime[i][0], TRUE);
        p_file.writeTime (calcTime[i][1], TRUE);
        p_file.writeTime (calcTime[i][2], TRUE);
        p_file.writeInt (count[i]);

		p_file.writeTime( pTools[ i ].GetPercentile( 25 ) , TRUE );//Q1
		p_file.writeTime( pTools[ i ].GetPercentile( 50 ), TRUE );//Q2
		p_file.writeTime( pTools[ i ].GetPercentile( 75 ), TRUE );//Q3
		
		p_file.writeTime( pTools[ i ].GetPercentile( 1 ), TRUE );//P1
		p_file.writeTime( pTools[ i ].GetPercentile( 5 ), TRUE );//P5
		p_file.writeTime( pTools[ i ].GetPercentile( 10 ), TRUE );//P10
		
		p_file.writeTime( pTools[ i ].GetPercentile( 90 ), TRUE );//P90
		p_file.writeTime( pTools[ i ].GetPercentile( 95 ), TRUE );//P95
		p_file.writeTime( pTools[ i ].GetPercentile( 99 ), TRUE );//P99
		
		p_file.writeTime( ElapsedTime( pTools[ i ].GetSigma() ), TRUE );//sigma


        p_file.writeLine();
		bar.StepIt();
	}
	for(i = 0; i < paxTypeCount; i++)
	{
		delete[] calcTime[i];
	}
	delete calcTime;
	delete[] count;
}