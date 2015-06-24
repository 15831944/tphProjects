// SpaceThroughputReport.cpp: implementation of the CSpaceThroughputReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "SpaceThroughputReport.h"

#include "Reports.h"
#include "ReportParameter.h"
#include "common\termfile.h"
#include "common\line.h"
#include "main\TermPlanDoc.h"
#include "main\resource.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSpaceThroughputReport::CSpaceThroughputReport(Terminal* _pTerm, const CString& _csProjPath)
	:CBaseReport(_pTerm, _csProjPath)
{

}

CSpaceThroughputReport::~CSpaceThroughputReport()
{

}

void CSpaceThroughputReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );
	m_usesInterval=1;
	m_nIntervalCount = 1 + (int)((m_endTime - m_startTime) / m_interval);
	std::vector<CString>vPortals;
	_pPara->GetPortals( vPortals );
	if( vPortals.size() >0 )
	{
		CTermPlanDoc* pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
		assert( pDoc );
		CString sPortalName = vPortals[0];//only one portal can be selected
		int iCount =pDoc->m_portals.m_vPortals.size();
		for( int i=0; i<iCount; ++i )
		{
			if( pDoc->m_portals.m_vPortals[ i ]->name == sPortalName )
			{
				for( int j=0; j<2; ++j )
				{
					m_pointList[j].setX( pDoc->m_portals.m_vPortals[ i ]->points.at( j )[0] );
					m_pointList[j].setY( pDoc->m_portals.m_vPortals[ i ]->points.at( j )[1] );
					m_pointList[j].setZ( pDoc->m_portals.m_vPortals[ i ]->floor * SCALE_FACTOR );
				}
				
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
void CSpaceThroughputReport::GenerateDetailed(ArctermFile& p_file)
{
	p_file.writeField ("Passenger Type, Start Time, End Time, Count");
    p_file.writeLine();

    int paxTypeCount = m_multiConst.getCount();

    const CMobileElemConstraint *paxType;


	CProgressBar bar( "Generating Space Throughtout Report", 100, paxTypeCount, TRUE );

//	// TRACE("\n paxTypeCount= %d\n",paxTypeCount);
    for (int i = 0; i < paxTypeCount; i++)	//calculate the space through report with the param "Pax Type"
    {
		bar.StepIt();
        paxType = m_multiConst.getConstraint(i);
        calculateSpaceThroughReport(*paxType, p_file);
    }
}

void CSpaceThroughputReport::calculateSpaceThroughReport (const CMobileElemConstraint& paxType, ArctermFile& p_file)
{
    char str[1024];
	strcpy (str, "Calculating passenger counts, ");

//	PaxTrack();
	m_curTime = m_startTime;
	CSpaceThroughtputResult tempResult;
	tempResult.InitItem( m_startTime , m_interval , m_nIntervalCount );
	GetResult( tempResult,  paxType );
	int iResultItemCount = tempResult.GetResultItemCount();
	for( int i=0 ; i< iResultItemCount ; ++i )
	{
		paxType.screenPrint (str, 0, 39);
		p_file.writeField (str);
		THROUGHTOUTITEM tempItem = tempResult[ i ];
		p_file.writeTime (tempItem.m_startTime);
		p_file.writeTime(tempItem.m_endTime);
        p_file.writeInt (tempItem.m_lThroughtout);
        p_file.writeLine();


	}
	/*
    for (long i = 0; i < m_nIntervalCount - 1; i++)
    {
        m_nPaxCount = 0;
        getCountAtTime (paxType);
        paxType.screenPrint (str, 39);
        p_file.writeField (str);
        p_file.writeTime (m_curTime);
		m_curTime += m_interval;
		p_file.writeTime(m_curTime);
        p_file.writeInt (m_nPaxCount);
        p_file.writeLine();
    }
	*/
}


void CSpaceThroughputReport::ReadSpecifications(const CString& _csProjPath)
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
            
	
	//specific for space thruput, to get the point of the portal.
		specFile.getLine();
        for (int i = 0; i < 2; i++)
			specFile.getPoint(m_pointList[i]);
		specFile.closeIn();

    m_nIntervalCount = 1 + (int)((m_endTime - m_startTime) / m_interval);
	}
}
void CSpaceThroughputReport::GetResult( CSpaceThroughtputResult& _result ,const CMobileElemConstraint& paxType)
{
	ReportPaxEntry element;
	element.SetOutputTerminal( m_pTerm );
	element.SetEventLog( m_pTerm->m_pMobEventLog );
    long count = m_pTerm->paxLog->getCount();
	CProgressBar bar( "Calculating Space Throughtput :", 100, count, TRUE, 1 );
	for(long i = 0; i < count; i++)
	{
		bar.StepIt();
		m_pTerm->paxLog->getItem (element, i);
		element.clearLog();
		if( !element.fits(paxType) || element.getEntryTime() > m_endTime || element.getExitTime() < m_startTime   )
			continue;
		ComputerSinglePaxResult( _result ,element);
	}

	
}

void CSpaceThroughputReport::ComputerSinglePaxResult( CSpaceThroughtputResult& _result ,ReportPaxEntry& _element )
{
	
	int iEventCount  = _element.getCount();
	
	PointVector vPoint;
	double portalZ = m_pointList[0].getZ();
	
	//CProgressBar bar( "B", 100, iEventCount, TRUE, 1 );
	bool bChangeFloor = false;
	//int iChangeCount = 0;
	for(int i = 0; i < iEventCount ; ++i )
	{
		//bar.StepIt();
		PaxEvent track;
		track.init(_element.getEvent(i));
		if ( track.getPoint().getZ() == portalZ && track.getTime() >= m_startTime && track.getTime() <= m_endTime   ) 
		{
			vPoint.push_back( track );
			if( !bChangeFloor )
			{
				bChangeFloor = true;
	//			if( iChangeCount != 0 )
	//				iChangeCount++;
			}
		}
		else
		{
			if( bChangeFloor )
			{
				bChangeFloor = false;
				//iChangeCount++;
				ComputerSinglePaxCrossPortal( _result, vPoint );
				vPoint.clear();
			}
				

		}
	}	
	if( bChangeFloor )
	{
		ComputerSinglePaxCrossPortal( _result, vPoint );
	}
	//// TRACE("\n iChangeCount=%d\n",iChangeCount);

	//ComputerSinglePaxCrossPortal( _result, vPoint );
}

void CSpaceThroughputReport::ComputerSinglePaxCrossPortal( CSpaceThroughtputResult& _result , PointVector& _trackVector )
{
	int iPointCount = _trackVector.size();
	if( iPointCount <2 )
		return;
	
	Point prevPoint, curPoint;
	Line linePortal, lineMiniTrack;
	linePortal.init(m_pointList[0], m_pointList[1]);
	double dDistance1=0;
	double dDistance2=0;
	ElapsedTime preTime;
	ElapsedTime curTime;
	ElapsedTime tempTime;
	ElapsedTime pntTime;
	//ElapsedTime printTime;
	//char timeStr[ 16 ];
	
	for( int i=1 ,j=2; j< iPointCount ; ++i, ++j )
	{
		
		prevPoint = _trackVector.at( i ).getPoint();
		curPoint = _trackVector.at( j ).getPoint();
		preTime = _trackVector.at( i ).getTime();
		curTime = _trackVector.at( j ).getTime();

		lineMiniTrack.init(prevPoint, curPoint);
		if (linePortal.intersects (lineMiniTrack))
		{
			Point tempPnt (linePortal.intersection (lineMiniTrack));
			dDistance1 =  tempPnt.distance( prevPoint );
			dDistance2 = tempPnt.distance( curPoint );
			pntTime =preTime;
			tempTime = curTime - preTime ;
			pntTime += (long) (tempTime.asSeconds() * dDistance1 /( dDistance1 + dDistance2 ) );

/*
			// TRACE("*************\n");
			printTime = preTime;
			printTime.printTime( timeStr );
			// TRACE(" pre time = %s\n ",timeStr  );

			printTime = curTime;
			printTime.printTime( timeStr );
			// TRACE(" curTime time = %s\n ",timeStr  );

			printTime = pntTime;
			printTime.printTime( timeStr );
			// TRACE(" pntTime time = %s\n ",timeStr  );
			
			// TRACE(" x = %.2f\n ", tempPnt.getX() );
			// TRACE(" y = %.2f\n ", tempPnt.getY() );
*/
			

			_result.IncreaseCount( pntTime );			
		}
	}
}
/*
void CSpaceThroughputReport::getCountAtTime (const PassengerConstraint& paxType)
{
    ReportPaxEntry element;
	element.SetOutputTerminal( m_pTerm );
	element.SetEventLog( m_pTerm->m_pPaxEventLog );
    long count = m_pTerm->paxLog->getCount();
	Point *PointList;
	long num;
	long trackCount;
	for(long i = 0; i < count; i++)
	{
		m_pTerm->paxLog->getItem (element, i);
		if(element.getEntryTime() > m_endTime || element.getExitTime() < m_startTime)
			continue;
		trackCount = element.getCount();
		if(trackCount == 0) continue;
		if(element.fits(paxType) && element.alive(m_curTime, m_curTime + m_interval))
		{
			//element.g
			PointList = new Point[trackCount];
			ElapsedTime tempTime;
			char timeStr[ 16 ];
			for(num = 0; num < trackCount; num++ )
			{
				PaxEvent track;
				track.init(element.getEvent(num));

				// TRACE("*************\n");
				// TRACE(" proc num = %d\n ", element.getEvent( num ).procNumber );
				// TRACE(" state = %d\n ", element.getEvent( num ).state );
				tempTime.set( element.getEvent( num ).time );
				tempTime.printTime( timeStr );
				// TRACE(" time = %s\n ",timeStr  );
				// TRACE(" x = %.2f\n ", element.getEvent( num ).x );
				 // TRACE(" y = %.2f\n ", element.getEvent( num ).y );
				PointList[num] = track.getPoint();
			}
			m_nPaxCount = m_nPaxCount + CrossPortalNum(PointList, trackCount);
			delete[] PointList;
		}
	}
}

void CSpaceThroughputReport::PaxTrack()
{
}


int CSpaceThroughputReport::CrossPortalNum(Point *PointList, long trackCount)
{
	Point prevPoint, curPoint;
	Line linePortal, lineMiniTrack;
	linePortal.init(m_pointList[0], m_pointList[1]);
	long crossNum = 0;;

	if(trackCount <= 0 || trackCount == 1)
		return 0;
	/*
	for(int i = 0; i < trackCount; i++)
	{
		curPoint = PointList[i];
		if(i==0) continue;
		lineMiniTrack.init(prevPoint, curPoint);
		if(linePortal.intersects(lineMiniTrack))
			crossNum++;		
		prevPoint = curPoint;
	}

	for( int i=0 ,j=1; j< trackCount ; ++i, ++j )
	{
		prevPoint = PointList[i];
		curPoint = PointList[j];
		lineMiniTrack.init(prevPoint, curPoint);
		if(linePortal.intersects(lineMiniTrack))
			crossNum++;	

	}
	
	return crossNum;
}
*/