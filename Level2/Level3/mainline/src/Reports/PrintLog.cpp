// PrintLog.cpp: implementation of the CPrintLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reports.h"
#include "PrintLog.h"
#include "common\termfile.h"
#include "results\out_term.h"
#include "results\proclog.h"
#include "results\paxlog.h"
#include "rep_pax.h"
#include "engine\terminal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPrintLog::CPrintLog(Terminal* _pTerm, const CString& _csProjPath)
	:CBaseReport(_pTerm, _csProjPath)
{
}

CPrintLog::~CPrintLog()
{
}

void CPrintLog::GenerateDetailed( ArctermFile& p_file )
{/*
    ReportPaxEvent track;
    int trackCount;

    int progressBar = initBar ("Generating Passenger Log",
        outTerm.paxLog->getCount());

    ReportPaxEntry element;
    outTerm.paxLog->setToFirst();
    long paxCount = outTerm.paxLog->getCount();
    for (long i = 0; i < paxCount; i++)
    {
        updateBar (progressBar, i);
        outTerm.paxLog->getNextItem (element);
        trackCount = element.getCount();
        if (trackCount && element.alive (startTime, endTime))
            if (element.fits (multiConst))
            {
		 p_file.writeField ("ID,Entry Time,Exit Time,Group Size,Bags,Carts,Passenger Type");
		 p_file.writeLine();
		 element.writeElement (p_file);
		 p_file.writeLine();
		 p_file.writeLine();

                p_file.writeField ("Hit Time,X Position,Y Position,Z Position,State,Processor");
                p_file.writeLine();

                element.setToFirst();
                for (int j = 0; j < trackCount; j++)
                {
                    track.init (element.getNextEvent());
                    track.writeTrack (p_file);
                    p_file.writeLine();
                }
                p_file.writeLine();
            }
    }
    completeBar (progressBar);*/
}


void CPrintLog::GenerateSummary (ArctermFile& p_file)
{
	p_file.writeField ("ID,Entry Time,Exit Time,Group Size,Bags,Carts,Speed,Passenger Type");
	p_file.writeLine();



	ReportPaxEntry element;
	element.SetEventLog( m_pTerm->m_pMobEventLog );
	long paxCount = m_pTerm->paxLog->getCount();
    
	CProgressBar bar( "Generating Passenger Log Report", 100, paxCount, TRUE );
	
	for (long i = 0; i < paxCount; i++)
	{
		bar.StepIt();

		m_pTerm->paxLog->getItem (element, i);
		element.clearLog();
		element.SetOutputTerminal( m_pTerm );

		
		if( element.alive (m_startTime, m_endTime))
		if( element.fits (m_multiConst))
		{
			element.writeElement (p_file);
			p_file.writeLine();
		}
	}
//	wndProgress.DestroyWindow();
//	Sleep(50);
//	AfxGetMainWnd()->Invalidate();
}

