// ExceedMaxTimeCheckEvent.cpp: implementation of the ExceedMaxTimeCheckEvent class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ExceedMaxTimeCheckEvent.h"
#include "engine\baggage.h"
#include "engine\person.h"
#include "common\CodeTimeTest.h"
#include "engine\terminal.h"
#include "../common/EngineDiagnose.h"
#include "../Inputs/SimParameter.h"
#include "../Common/ARCTracker.h"
#include "Engine/ARCportEngine.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ExceedMaxTimeCheckEvent::ExceedMaxTimeCheckEvent( BaggageProcessor* _pBaggageProc, long _lPersonID ,CString strMobType)
: m_pBaggageProc( _pBaggageProc ), m_lPersonToBeChecked( _lPersonID ),m_strMobType(strMobType)
{
	
}

ExceedMaxTimeCheckEvent::~ExceedMaxTimeCheckEvent()
{

}
int ExceedMaxTimeCheckEvent::process ( CARCportEngine *_pEngine )
{
	PLACE_METHOD_TRACK_STRING();
	if( m_pBaggageProc->IfPersonStillExsit( m_lPersonToBeChecked ) )
	{

		/*
		CString sErrMsg ( "Person exceed max wait time in baggage device.\n");
		sErrMsg += " Person ID : ";
		CString sID;
		sID.Format("%d",m_lPersonToBeChecked);
		sErrMsg += sID;
		sErrMsg += " Baggage device name : ";
		sErrMsg += m_pBaggageProc->getID()->GetIDString();	
		getTime();
		
		/*
		char tempTime[32];
		time.printTime(tempTime );
		sErrMsg +=" Time :";
		sErrMsg += tempTime;


		typedef struct _tagFORMATMSGEX
		{
		CString strTime;
		CString strMsgType;
		CString strProcessorID;
		CString strPaxID;
		CString strFlightID;
		CString strMobileType;
		CString strOther;
		////////////////////////
		DiagnoseEntry diag_entry;
		}FORMATMSGEX;

		typedef struct _tagFORMATMSG
		{
		CString strTime;
		CString strMsgType;
		CString strMobType;
		CString strProcessor; 
		CString strOtherInfo;
		/////////////////////////
		DiagnoseEntry diag_entry;
		}FORMATMSG;

		*/

		//add by hans 2005-10-9
		char str[64];
		ElapsedTime tmptime(getTime().asSeconds() % WholeDay);
		tmptime.printTime ( str );

		CStartDate sDate = _pEngine->getTerminal()->m_pSimParam->GetStartDate();
		bool bAbsDate;
		COleDateTime date;
		int nDtIdx;
		COleDateTime time;
		CString sDateTimeStr;
		sDate.GetDateTime( getTime(), bAbsDate, date, nDtIdx, time );
		if( bAbsDate )
		{
			sDateTimeStr = date.Format(_T("%Y-%m-%d"));
		}
		else
		{
			sDateTimeStr.Format("Day %d", nDtIdx + 1 );
		}
		sDateTimeStr += " ";
		sDateTimeStr += str;
		CString strTime = sDateTimeStr ;

//		CString strTime = getTime().printTime();

		CString strMsgType = "WAIT LONG AT RECLAIM";
		CString strProcessor = m_pBaggageProc->getID()->GetIDString();
//		CString sPaxID;
//		sPaxID.Format("%d",m_lPersonToBeChecked);
//		CString strPaxID = sPaxID;
//		CString strFlightID = "";
		CString strMobType = m_strMobType;
		CString strOtherInfo = "";


		FORMATMSG msgEx;

		msgEx.strTime			= strTime;
		msgEx.strMsgType		= strMsgType;
		msgEx.strProcessor	= strProcessor;
//		msgEx.strPaxID = strPaxID ;
//		msgEx.strFlightID = strFlightID;
		msgEx.strMobType = strMobType ;
		msgEx.strOtherInfo = strOtherInfo ;
		DiagnoseEntry tempDiagnose;
		msgEx.diag_entry = tempDiagnose;

		_pEngine->SendSimFormatMessage(msgEx);		
//		((Terminal*)_pInTerm)->SendSimMessage( sErrMsg );
	}

	return 1;
}