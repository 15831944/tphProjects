// PassengerDensityReport.cpp: implementation of the CPassengerDensityReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main\resource.h"
#include "Reports.h"
#include "PassengerDensityReport.h"
#include "common\termfile.h"
#include "rep_pax.h"
#include "ReportParameter.h"
#include "main\TermPlanDoc.h"
#include "main/CompareReportDoc.h"
#include "main\CompareReportView.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPassengerDensityReport::CPassengerDensityReport(Terminal* _pTerm, const CString& _csProjPath)
	:CBaseReport(_pTerm, _csProjPath)
{
}

CPassengerDensityReport::~CPassengerDensityReport()
{

}

void CPassengerDensityReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );
	m_usesInterval=1;
	m_nIntervalCount = 1 + (int)((m_endTime - m_startTime) / m_interval);
	std::vector<CString>vArea;
	_pPara->GetAreas( vArea );
	if( vArea.size() >0 )
	{
		//CTermPlanDoc* pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
		CDocument* pDoc = ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
		assert(pDoc);

		Terminal* pTerm = NULL;

		if (_tcscmp(pDoc->GetRuntimeClass()->m_lpszClassName, _T("CTermPlanDoc")) == 0)
		{
			pTerm = &(((CTermPlanDoc*)pDoc)->GetTerminal());
		}
		else if (_tcscmp(pDoc->GetRuntimeClass()->m_lpszClassName, _T("CCompareReportDoc")) == 0)
		{
			//------------------added by frank 11-4
			//POSITION pos = pDoc->GetFirstViewPosition();
			//CView * pTempView =	NULL;
			//CCompareReportView2* pCmpReportView = NULL;
			//while (pos != NULL)
			//{
			//	pTempView = pDoc->GetNextView(pos);

			//	if (pTempView->IsKindOf(RUNTIME_CLASS(CCompareReportView2)))
			//	{
			//		pCmpReportView = (CCompareReportView2 *)pTempView;
			//		break;
			//	}
			//}
			//assert(pCmpReportView);
			pTerm = m_pTerm;
//			pTerm = &(pCmpReportView->GetReportWnd()->GetReportManager()->GetCmpReport()->GetTerminal());
		}

		if(pTerm == NULL)
		{
			m_nIntervalCount = 0;
			return ;
		}

		CString sAreaName = vArea[0];//only one area can be selected
		int iCount = pTerm->m_pAreas->m_vAreas.size();
		for( int i=0; i<iCount; ++i )
		{
			if( pTerm->m_pAreas->m_vAreas[ i ]->name == sAreaName )
			{
				int iPointCount = pTerm->m_pAreas->m_vAreas[ i ]->points.size();
				Point *pPoint = new Point[ iPointCount ];
				for( int j=0; j<iPointCount; ++j )
				{
					pPoint[j].setX( pTerm->m_pAreas->m_vAreas[ i ]->points.at( j )[0] );
					pPoint[j].setY( pTerm->m_pAreas->m_vAreas[ i ]->points.at( j )[1] );
					pPoint[j].setZ( pTerm->m_pAreas->m_vAreas[ i ]->floor * SCALE_FACTOR );
				}
				m_region.init (iPointCount, pPoint);
				m_dArea = m_region.calculateArea();
				delete [] pPoint;
				return;
			}
		}

		m_nIntervalCount = 0;//needless to caculate this report, because the report parameter is invalid	
	}
	else
	{
		m_nIntervalCount = 0;//needless to caculate this report, because the report parameter is invalid
	}
    
}
void CPassengerDensityReport::GenerateSummary( ArctermFile& p_file )
{
}


void CPassengerDensityReport::GenerateDetailed( ArctermFile& p_file )
{
	p_file.writeField ("Passenger Type,Time,Count,Pax/m2,m2/Pax");
    p_file.writeLine();

    int paxTypeCount = m_multiConst.getCount();

    const CMobileElemConstraint *paxType;


	CProgressBar bar( "Generating Passenger Log Report", 100, paxTypeCount, TRUE );

    for (int i = 0; i < paxTypeCount; i++)
    {
		bar.StepIt();
        paxType = m_multiConst.getConstraint(i);
        calculatePassengerDensity (*paxType, p_file);
    }
}


void CPassengerDensityReport::calculatePassengerDensity
    (const CMobileElemConstraint& paxType, ArctermFile& p_file)
{
    char str[80];
	strcpy (str, "Calculating passenger counts, ");

	m_curTime = m_startTime;
    for (long i = 0; i < m_nIntervalCount; i++)
    {
        m_nPaxCount = 0;
        getCountAtTime (paxType);

        paxType.screenPrint(str) ;
        p_file.writeField (str);
        p_file.writeTime (m_curTime);
        p_file.writeInt (m_nPaxCount);
		if(m_dArea==0)
		{
			p_file.writeFloat (0);
		}
		else
		{
			p_file.writeDouble(m_nPaxCount / m_dArea);
		}
        p_file.writeDouble((m_nPaxCount)? m_dArea / m_nPaxCount: m_dArea);
        p_file.writeLine();

        m_curTime += m_interval;
    }
}



void CPassengerDensityReport::getCountAtTime (const CMobileElemConstraint& paxType)
{
    Point tempPoint;
    ReportPaxEntry element;

	element.SetOutputTerminal( m_pTerm );
	element.SetEventLog( m_pTerm->m_pMobEventLog );
    long count = m_pTerm->paxLog->getCount();
    for (long i = 0; i < count; i++)
    {
        m_pTerm->paxLog->getItem (element, i);
		element.clearLog();

        if (element.getEntryTime() > m_curTime)
            break;

        if (element.fits (paxType) && element.alive (m_curTime))
        {
            if( element.getPointAtTime (m_curTime, tempPoint ) && m_region.contains (tempPoint) )
                m_nPaxCount ++;
        }
    }
}


void CPassengerDensityReport::ReadSpecifications(const CString &_csProjPath)
{
	int pointCount;
    Point pointList[32];
    char specFileName[128];
    PROJMANAGER->getSpecFileName( _csProjPath, GetSpecFileType(), specFileName );
//    env->getSpecFileName (getSpecFileType(), specFileName);

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
            

		//?
        specFile.getLine();
        pointCount = specFile.countColumns() / 3;
        for (int i = 0; i < pointCount; i++)
            specFile.getPoint (pointList[i]);
        specFile.closeIn();
    }
    else
    {
        pointCount = 4;
        pointList[0].init (-100l, -100l, 0);
        pointList[1].init (100l, -100l, 0);
        pointList[2].init (100l, 100l, 0);
        pointList[3].init (-100l, 100l, 0);
    }
    m_nIntervalCount = 1 + (int)((m_endTime - m_startTime) / m_interval);
    m_region.init (pointCount, pointList);
    m_dArea = m_region.calculateArea();
}
