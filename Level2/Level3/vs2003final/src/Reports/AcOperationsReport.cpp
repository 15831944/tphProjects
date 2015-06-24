// AcOperationsReport.cpp: implementation of the CAcOperationsReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reports.h"
#include "common\termfile.h"
#include "rep_pax.h"
#include "AcOperationsReport.h"
#include "Results\FltEntry.h"
#include "Results\FltLog.h"
#include "inputs\schedule.h"
#include "engine\terminal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAcOperationsReport::CAcOperationsReport(Terminal* _pTerm, const CString& _csProjPath)
:CBaseReport(_pTerm, _csProjPath)
{

}

CAcOperationsReport::~CAcOperationsReport()
{

}

void CAcOperationsReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );
	m_usesInterval = 1;

}
void CAcOperationsReport::ReadSpecifications(const CString& _csProjPath)
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
			m_multiConst.readConstraints (specFile,m_pTerm);
		}
	}
}


void CAcOperationsReport::GenerateSummary(ArctermFile& p_file)
{ 
	p_file.writeField("Flight ID, Operation, Aircraft Type, Capacity, Schedule Arr Time,Schedule Dep Time, Load Factor, Load, Actual Arr Time , Actual Dep Time, Delay Arr, Delay Dep, Gate Occupancy, Flight type");
	p_file.writeLine();

	ElapsedTime ActArrTime, ActDepTime;
	ElapsedTime SchArrTime, SchDepTime;
	ElapsedTime testStartTime, testEndTime;

	FlightLogEntry fltEntry;
	FlightDescStruct flightDesc;
	fltEntry.SetOutputTerminal(m_pTerm);
	fltEntry.SetEventLog(m_pTerm->m_pFltEventLog);

	int nEntryCount = m_multiConst.getCount();
	CMultiMobConstraint multiConst;

	for( int i=0; i<nEntryCount; i++ )
	{
		CMobileElemConstraint tempConstr =  *m_multiConst.getConstraint( i );
		int mode = tempConstr.GetFltConstraintMode();
		if( mode == ENUM_FLTCNSTR_MODE_ALL )
		{
			tempConstr.SetFltConstraintMode( ENUM_FLTCNSTR_MODE_ARR );
			multiConst.addConstraint( tempConstr );
			tempConstr.SetFltConstraintMode( ENUM_FLTCNSTR_MODE_DEP );
			multiConst.addConstraint( tempConstr );
		}
		else
		{
			multiConst.addConstraint( tempConstr );
		}
	}

	nEntryCount = multiConst.getCount();
	int nIsTA=0;//0 means not T/A; or is T/A . 1 is arriving ; 2 is departing . 
	for ( int k=0; k<nEntryCount; k++ )
	{
		FlightConstraint tempStraint =  *multiConst.getConstraint( k );
		long count = m_pTerm->flightLog->getCount();
		char str[250];
		nIsTA=0;
		for(long i = 0; i < count; i++)
		{
			m_pTerm->flightLog->getItem(fltEntry, i);
			fltEntry.initStruct(flightDesc);
			
			ASSERT( tempStraint.GetFltConstraintMode() );

			if( !tempStraint.fits( flightDesc ) )
				continue;

			if(fltEntry.isArriving() > 0 /*&& fltEntry.isDeparting() == 0*/&& tempStraint.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ARR)//Arriving
			{
				testStartTime.set(flightDesc.arrTime/100L);
				if( this->FilterFlight(testStartTime,testEndTime, "A" ) == false )
					continue;
				fltEntry.getFlightID(str,'A');
				p_file.writeField(str);
				strcpy(str, "ARR");
			}
			/*else*/ if(/*fltEntry.isArriving() == 0 &&*/ fltEntry.isDeparting() >0 && tempStraint.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_DEP)//Departing
			{
				testEndTime.set(flightDesc.depTime/100L);
				if( this->FilterFlight(testStartTime,testEndTime,"D" )==false )
					continue;
				fltEntry.getFlightID(str,'D');
				p_file.writeField(str);
				strcpy(str, "DEP");
			}
			//else		//T/A					//modify by emily
			//{
			//	FLTCNSTR_MODE enumMode = tempStraint.GetFltConstraintMode();
			//	char buf[10];
			//	switch( enumMode )
			//	{
			//	case ENUM_FLTCNSTR_MODE_ALL:
			//		strcpy( buf, "T" );
			//		break;
			//	case ENUM_FLTCNSTR_MODE_ARR:
			//		strcpy( buf, "A" );
			//		break;
			//	case ENUM_FLTCNSTR_MODE_DEP:
			//		strcpy( buf, "D" );
			//		break;
			//	}
			//	testStartTime.set(flightDesc.arrTime/100L);
			//	testEndTime.set(flightDesc.depTime/100L);
			//	if( this->FilterFlight(testStartTime,testEndTime, buf )==false )
			//		continue;
			//	
			//	switch(buf[0])
			//	{
			//	case 'A':
			//		nIsTA=1;
			//		break;
			//	case 'D':
			//		nIsTA=2;
			//		break;
			//	default:
			//		ASSERT(0);
			//		break;
			//	}

			//	fltEntry.getFlightID(str,'A');
			//	CString strTemp( str );
			//	fltEntry.getFlightID(str,'D');
			//	strTemp += "/";
			//	strTemp += str;
			//	p_file.writeField( strTemp);
			//	strcpy(str, "T/A");
			//}
	//		if(this->FilterFlight(ActArrTime,ActDepTime,"A"))
			p_file.writeField(str);
			p_file.writeField(flightDesc.acType);
			p_file.writeInt(flightDesc.capacity);
			if(fltEntry.isArriving() > 0 /*&& fltEntry.isDeparting() == 0*/&& tempStraint.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ARR)//Arriving
			{
				SchArrTime.set(flightDesc.arrTime/100L);
				SchDepTime.set(0);
				ActArrTime.set(flightDesc.arrTime/100L + (long)flightDesc.arrDelay/100L);
				ActDepTime.set(0);

				p_file.writeTime(SchArrTime);
				p_file.writeField("");
				p_file.writeFloat(flightDesc.arrloadFactor*100);
				p_file.writeInt((int)(flightDesc.arrloadFactor * flightDesc.capacity));
				p_file.writeTime(ActArrTime);
				p_file.writeField("");

				int min = (ActArrTime - SchArrTime).asMinutes();
				p_file.writeInt(min);
			
				p_file.writeField("");
		
			}
			/*else */if(/*fltEntry.isArriving() == 0 &&*/ fltEntry.isDeparting() >0 && tempStraint.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_DEP)//Departing
			{
				SchArrTime.set(0);
				SchDepTime.set(flightDesc.depTime/100L);
				ActArrTime.set(0);
				ActDepTime.set(flightDesc.depTime/100L + (long)flightDesc.depDelay/100l);

				p_file.writeField("");
				p_file.writeTime(SchDepTime);
				p_file.writeFloat(flightDesc.deploadFactor * 100);
				p_file.writeInt((int)(flightDesc.deploadFactor * flightDesc.capacity));
				p_file.writeField("");
				p_file.writeTime(ActDepTime);

				p_file.writeField("");
				int min = (ActDepTime - SchDepTime).asMinutes();
				p_file.writeInt(min);
			}
			//else			//T/A				//modify by emily
			//{
			//	SchArrTime.set(flightDesc.arrTime/100L);
			//	SchDepTime.set(flightDesc.depTime/100L);
			//	ActArrTime.set(flightDesc.arrTime/100L + (long)flightDesc.arrDelay/100L);
			//	ActDepTime.set(flightDesc.depTime/100L + (long)flightDesc.depDelay/100L);

			//	switch(nIsTA)
			//	{
			//	case 1://A
			//		p_file.writeTime(SchArrTime);
			//		p_file.writeField("");//SchDepTime
			//		p_file.writeFloat(flightDesc.loadFactor);
			//		p_file.writeFloat(flightDesc.loadFactor * flightDesc.capacity);
			//		p_file.writeTime(ActArrTime);
			//		p_file.writeField("");//ActDepTime
			//		break;
			//	case 2://D
			//		p_file.writeField("");//SchArrTime
			//		p_file.writeTime(SchDepTime);
			//		p_file.writeFloat(flightDesc.loadFactor);
			//		p_file.writeFloat(flightDesc.loadFactor * flightDesc.capacity);
			//		p_file.writeField("");//ActArrTime
			//		p_file.writeTime(ActDepTime);
			//		break;
			//	default:
			//		ASSERT(0);
			//		break;
			//	}
/**/
				/*
					p_file.writeTime(SchArrTime);
					p_file.writeTime(SchDepTime);
					p_file.writeFloat(flightDesc.loadFactor);
					p_file.writeFloat(flightDesc.loadFactor * flightDesc.capacity);
					p_file.writeTime(ActArrTime);
					p_file.writeTime(ActDepTime);
				*/
			//	int min = (ActArrTime - SchArrTime).asMinutes();
			//	p_file.writeInt(min);
			//
			//	min = (ActDepTime - SchDepTime).asMinutes();
		
			//	p_file.writeInt(min);
			//}

	//		p_file.writeTime(SchArrTime);
	//		p_file.writeTime(SchDepTime);
	//		p_file.writeFloat(flightDesc.loadFactor);
	//		p_file.writeFloat(flightDesc.loadFactor * flightDesc.capacity);
	//		p_file.writeTime(ActArrTime);
	//		p_file.writeTime(ActDepTime);
			
			if(fltEntry.isArriving() && fltEntry.isDeparting())
				p_file.writeTime((flightDesc.depTime/100L + (long)flightDesc.depDelay/100L) - (flightDesc.arrTime/100L + (long)flightDesc.arrDelay/100L));
			else
				p_file.writeTime(flightDesc.gateOccupancy);
			
			//Flight* pFlight = m_pTerm->flightSchedule->getItem( i );
			//ASSERT( pFlight );
			//bool bHasFited = false;
			//char tempChars[512];
			CString tempChars;
			tempStraint.screenPrint( tempChars );
	//		CString strMode((char)tempStraint.getMode());

	//		_tcscat(tempChars,strMode);
			p_file.writeField( tempChars.GetBuffer(0) );
				
			p_file.writeLine();
		}
	}
}

bool CAcOperationsReport::FilterFlight(ElapsedTime& etArrTime, ElapsedTime& etDepTime, char* sType )
{
	ASSERT(sType!=NULL);

	bool bRet=false;
	if(_tcsnicmp(sType,_T("A"),1)==0)
	{
		if(etArrTime>=this->m_startTime && etArrTime<=this->m_endTime)
			bRet= true;
	}
	else if(_tcsnicmp(sType,_T("D"),1)==0)
	{

		if(etDepTime>=this->m_startTime && etDepTime<=this->m_endTime)
			bRet= true;
	}
	else
	{
		if(etDepTime < this->m_startTime || etArrTime> this->m_endTime)
			bRet= false;
		else 
			bRet=true;

	}
	return bRet;
}
