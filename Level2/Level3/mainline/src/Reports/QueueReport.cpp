// QueueReport.cpp: implementation of the CQueueReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reports.h"
#include "QueueReport.h"
#include "common\termfile.h"
#include "results\out_term.h"
#include "QueueElement.h"
#include "engine\terminal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQueueReport::CQueueReport(Terminal* _pTerm, const CString& _csProjPath)
:CBaseReport(_pTerm, _csProjPath)
{
	int i=0;
}

CQueueReport::~CQueueReport()
{

}
void CQueueReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );
	QueueElement::initReport ( this );
}
void CQueueReport::GenerateDetailed(ArctermFile &p_file)
{
	p_file.writeField ("Passenger #,Total Time,Average Time,Processors,Passenger Type");
	p_file.writeLine();

	QueueElement element;
	element.initReport(this);
	element.SetOutputTerminal( m_pTerm );
	element.SetEventLog( m_pTerm->m_pMobEventLog );
	long paxCount = m_pTerm->paxLog->getCount();

	CProgressBar bar( "Generating Passenger Queue Report", 100, paxCount, TRUE );
	try
	{
		for (long i = 0; i < paxCount; i++)
		{
			bar.StepIt();
			m_pTerm->paxLog->getItem (element, i);
			element.clearLog();
			if (element.alive (m_startTime, m_endTime))
				if (element.fits (m_multiConst))
				{
					element.calculateQueueTime (m_pTerm,m_startTime, m_endTime);
					element.writeEntry (p_file);
				}
		}
		//completeBar (progressBar);
	}
	catch(ARCInvalidTimeError* timeError)
	{
		char msg[256];
		timeError->getMessage(msg);
		CString strMsg(msg);
		AfxMessageBox(msg);
		delete timeError;
	}
	catch(...)
	{
		throw;
	}


}

void CQueueReport::GenerateSummary(ArctermFile &p_file)
{
	p_file.writeField ("Passenger Type,Minimum,Average,Maximum,Count");
	p_file.writeLine();

	QueueElement element;
	element.initReport(this);
	int  paxTypeCount = m_multiConst.getCount();


	char str[MAX_PAX_TYPE_LEN];
	const CMobileElemConstraint *aConst;

	CProgressBar bar( "Generating Passenger Queue Report", 100, m_multiConst.getCount(), TRUE );
	try
	{
		for (int i = 0; i < m_multiConst.getCount(); i++)
		{
			bar.StepIt();
			aConst = m_multiConst.getConstraint(i);
			CStatisticalTools<ElapsedTime> tempTools;
			getAverageQueueTime (*aConst, tempTools);

			tempTools.SortData();

			aConst->screenPrint (str, 0, 37);
			p_file.writeField (str);
			p_file.writeTime (minimum, TRUE);
			p_file.writeTime (average, TRUE);
			p_file.writeTime (maximum, TRUE);
			p_file.writeInt (count);

			p_file.writeTime( tempTools.GetPercentile( 25 ),  TRUE );//Q1
			p_file.writeTime( tempTools.GetPercentile( 50 ), TRUE );//Q2
			p_file.writeTime( tempTools.GetPercentile( 75 ), TRUE );//Q3

			p_file.writeTime( tempTools.GetPercentile( 1 ), TRUE );//P1
			p_file.writeTime( tempTools.GetPercentile( 5 ), TRUE );//P5
			p_file.writeTime( tempTools.GetPercentile( 10 ), TRUE );//P10

			p_file.writeTime( tempTools.GetPercentile( 90 ), TRUE );//P90
			p_file.writeTime( tempTools.GetPercentile( 95 ), TRUE );//P95
			p_file.writeTime( tempTools.GetPercentile( 99 ), TRUE );//P99

			p_file.writeTime( ElapsedTime(tempTools.GetSigma()), TRUE );//Sigma

			p_file.writeLine();
		}
	}
	catch(ARCInvalidTimeError* timeError)
	{
		char msg[256];
		timeError->getMessage(msg);
		CString strMsg(msg);
		AfxMessageBox(msg);
		delete timeError;
	}
	catch(...)
	{
		throw;
	}
}

void CQueueReport::getAverageQueueTime(const CMobileElemConstraint &aConst,CStatisticalTools<ElapsedTime>& _tools)
{
	long double totalDuration = 0l;
	minimum = maximum = average = 0l;
	count = 0;

	char title[MAX_PAX_TYPE_LEN] = "Calculating Average Wait Times for Pax Type ";
	aConst.screenPrint (title + strlen (title), 0, 128);
	//int progressBar = initBar (title, (long) outTerm.paxLog->getCount());

	QueueElement element;

	element.SetOutputTerminal( m_pTerm );
	element.SetEventLog( m_pTerm->m_pMobEventLog );
	//element.SetEventLog( m_pTerm->m_pProcEventLog );
	long paxCount = m_pTerm->paxLog->getCount();

	//    outTerm.paxLog->setToFirst();
	//    long paxCount = outTerm.paxLog->getCount();
	for (long i = 0; i < paxCount; i++)
	{
		//updateBar (progressBar, i);
		m_pTerm->paxLog->getItem (element, i);
		element.clearLog();
		if (element.alive (m_startTime, m_endTime))
			if (element.fits (aConst))
			{
				element.calculateQueueTime (m_pTerm,m_startTime, m_endTime);
				if (!element.getProcCount())
					continue;

				_tools.AddNewData( element.getTotalTime() );
				if (element.getTotalTime() < minimum || minimum == 0l)
					minimum = element.getTotalTime();

				if (element.getTotalTime() > maximum)
					maximum = element.getTotalTime();

				totalDuration += (element.getTotalTime()).asSeconds();
				count++;
			}
	}
	if (count)
		average = (ElapsedTime) ((long)(totalDuration / count));
}
