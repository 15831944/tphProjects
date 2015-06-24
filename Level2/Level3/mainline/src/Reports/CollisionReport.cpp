// CollisionReport.cpp: implementation of the CCollisionReport class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "main\resource.h"
#include "Reports.h"
#include "CollisionReport.h"
#include "common\termfile.h"
#include "rep_pax.h"
#include "common\line.h"
#include "common\states.h"
#include "ReportParameter.h"
#include "main\TermPlanDoc.h"
//#include "map"

//#include "CollisionUitl.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define THRESHOLD	200

CCollisionReport::CCollisionReport(Terminal* _pTerm, const CString& _csProjPath,int _iFloorCount)
	:CBaseReport(_pTerm, _csProjPath)
{
	m_iFloorCount = _iFloorCount;
}

CCollisionReport::~CCollisionReport()
{

}
void CCollisionReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );
	m_usesInterval = 1;	

	long lThreashold;
	_pPara->GetThreshold( lThreashold );
	m_fThreshold = (float)lThreashold;
	m_fThreshold *= SCALE_FACTOR;
	m_nIntervalCount = 1 + (int)((m_endTime - m_startTime) / m_interval);

	std::vector<CString>vArea;
	_pPara->GetAreas( vArea );
	if( vArea.size() >0 )
	{
		CTermPlanDoc* pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
		assert( pDoc );
		CString sAreaName = vArea[0];//only one area can be selected
		int iCount =pDoc->GetTerminal().m_pAreas->m_vAreas.size();
		for( int i=0; i<iCount; ++i )
		{
			if( pDoc->GetTerminal().m_pAreas->m_vAreas[ i ]->name == sAreaName )
			{
				int iPointCount = pDoc->GetTerminal().m_pAreas->m_vAreas[ i ]->points.size();
				Point *pPoint = new Point[ iPointCount ];
				for( int j=0; j<iPointCount; ++j )
				{
					pPoint[j].setX( pDoc->GetTerminal().m_pAreas->m_vAreas[ i ]->points.at( j )[0] );
					pPoint[j].setY( pDoc->GetTerminal().m_pAreas->m_vAreas[ i ]->points.at( j )[1] );
					pPoint[j].setZ( pDoc->GetTerminal().m_pAreas->m_vAreas[ i ]->floor * SCALE_FACTOR );
				}
				m_region.init (iPointCount, pPoint);
				
				delete [] pPoint;
				return;
			}
		}
	}
	else
	{
		m_nIntervalCount = 0;//needless to caculate this report, because the report parameter is invalid
	}
}
void CCollisionReport::GenerateDetailed(ArctermFile& p_file)
{
	p_file.writeField ("Pax Type, Time, Pax ID 1, Pax ID 2");
    p_file.writeLine();

    int paxTypeCount = m_multiConst.getCount();

    

	//PaxEvent track;
	//CProgressBar bar( "Generating Collision Log Report", 100, paxTypeCount, TRUE );
	//long trackCount;
	/*
	ReportPaxEntry element,element_tmp;
	element.SetOutputTerminal( m_pTerm );
	element_tmp.SetOutputTerminal(m_pTerm);
	element.SetEventLog( m_pTerm->m_pPaxEventLog );  
	element_tmp.SetEventLog( m_pTerm->m_pPaxEventLog);	
	*/
	long count = m_pTerm->paxLog->getCount();

	//vect_rect.reserve(count);

	const CMobileElemConstraint *paxType;
    for ( int i = 0; i < paxTypeCount; i++)
    {
	//	bar.StepIt();
        paxType = m_multiConst.getConstraint(i);
        calculateCollisionReport(*paxType, p_file);
    }
}

void CCollisionReport::calculateCollisionReport (const CMobileElemConstraint& paxType, ArctermFile& p_file)
{

	float threshold;
	if(m_bThreshold)
		threshold = m_fThreshold;
	else 
		threshold = THRESHOLD;

	ReportPaxEntry element,element_tmp;
	
	element.SetOutputTerminal( m_pTerm );
	element_tmp.SetOutputTerminal(m_pTerm);
	element.SetEventLog( m_pTerm->m_pMobEventLog );  
	element_tmp.SetEventLog( m_pTerm->m_pMobEventLog);

	ElapsedTime entryTime, entryTime_tmp;
	ElapsedTime exitTime, exitTime_tmp;

	m_collisionUitl.Init( m_startTime, m_endTime, m_pTerm );	
	long count = m_pTerm->paxLog->getCount();
	if( m_region.getCount() > 0 )
	{		
		for (long i = 0; i < count; i++)			//loop for the paxLog count
		{
			m_pTerm->paxLog->getItem(element, i);
			element.clearLog();

			if( element.GetMobileType() > 1 )
				continue;
			if( element.getEntryTime() > m_endTime || element.getExitTime() < m_startTime )
				continue;

			long trackCount = element.getCount();
			PaxEvent trackFirst, trackSecond ;
			if( trackCount > 0 )
			{
				trackFirst.init(element.getEvent(0));
				for( long j = 1; j < trackCount ; ++j )
				{
					trackSecond.init(element.getEvent(j));
					if( m_region.IsOverLap( trackFirst.getPoint(),trackSecond.getPoint() ) )
					{
						TRACKPAIR temp;
						temp.m_lFirstTrackIdx = j-1;
						temp.m_lSecondTrackIdx = j;
						m_collisionUitl.AddPersonTrackInfo( i, temp, trackFirst.getTime(), trackSecond.getTime() );
					}
				}
			}
		}

		m_collisionUitl.CaculateAll( paxType, p_file ,threshold , m_region);

		
	}
	else
	{
		CProgressBar bar( "Caculationg Every Floor Collision :", 100, m_iFloorCount, TRUE );	
		for( int iFloor = 0; iFloor< m_iFloorCount; ++iFloor )
		{
			bar.StepIt();
			double dZ= (double)(iFloor*100);
			for (long i = 0; i < count; i++)			//loop for the paxLog count
			{
				m_pTerm->paxLog->getItem(element, i);
				element.clearLog();

				if( element.GetMobileType() > 1 )
					continue;
				
				if( element.getEntryTime() > m_endTime || element.getExitTime() < m_startTime )
					continue;

				long trackCount = element.getCount();
				PaxEvent trackFirst, trackSecond ;
				if( trackCount > 0 )
				{
					
					//trackFirst.init(element.getEvent(0));
					for( long j = 1; j < trackCount ; ++j )
					{
						trackFirst.init(element.getEvent(j-1));
						Point pFirst = trackFirst.getPoint();
						if( pFirst.getZ() != dZ )
							continue;

						trackSecond.init(element.getEvent(j));
						Point pSecond = trackSecond.getPoint();
						if( pSecond.getZ() != dZ )
							continue;
						
						TRACKPAIR temp;
						temp.m_lFirstTrackIdx = j-1;
						temp.m_lSecondTrackIdx = j;
						m_collisionUitl.AddPersonTrackInfo( i, temp, trackFirst.getTime(), trackSecond.getTime() );
						
					}
				}
			}		

			//AfxMessageBox("go");
			m_collisionUitl.CaculateAll( paxType, p_file ,threshold , m_region);
			m_collisionUitl.ClearAll();
		}
	}

	

/*
	PaxEvent track, track_tmp;
	DistanceUnit x_small,y_small,z_small,x_big,y_big,z_big,x1_small,y1_small,z1_small,x1_big,y1_big,z1_big;
	long nContinue = 0;
	long nSuccess = 0;
	long trackCount, trackCount_tmp;
	ElapsedTime PointTime1, PointTime2, PointTime1_tmp, PointTime2_tmp;
	Point Point1, Point2, Point1_tmp, Point2_tmp;
	char str[80];
*/
		
	
//	int id;
	//ComparisonID *comparison = new ComparisonID[count*count];
//	long ComparisonNum = 0;
	//int iPaxType, iPaxType_tmp;
//	long groupMember, groupMember_tmp; //groupSize_tmp, groupSize;
	/**
	PAXIDEXVECTOR vGroups = m_collisionUitl.GetPaxIndexVector();
	 int count;//********************notice nothing use.
	 CProgressBar bar0( "Generating Collision Log Report", 100, vGroups.size(), TRUE );
	for( int iGroup=0; iGroup<vGroups.size() ; ++iGroup )
	{
		bar0.StepIt();
		PAXIDEX vPaxIdx = vGroups[iGroup].GetPaxIdxVector();
		count = vPaxIdx.size();
		CString barStr;
		barStr.Format("Caculating Collision NO%d",iGroup );
		CProgressBar bar( barStr, 100, count, TRUE ,1);	
		
	//for(long i = 0; i < count; i++)			//count: count of the log number in the paxLog file.
		for(long i = 0; i < count; i++)
		{
			bar.StepIt();
		
			x_small = vect_rect[i].point[0].getX() < vect_rect[i].point[1].getX() ? vect_rect[i].point[0].getX() : vect_rect[i].point[1].getX();  
			x_big =  vect_rect[i].point[0].getX() > vect_rect[i].point[1].getX() ? vect_rect[i].point[0].getX() : vect_rect[i].point[1].getX() ;
			y_small = vect_rect[i].point[0].getY() < vect_rect[i].point[1].getY() ? vect_rect[i].point[0].getY() : vect_rect[i].point[1].getY();
			y_big =  vect_rect[i].point[0].getY() > vect_rect[i].point[1].getY() ? vect_rect[i].point[0].getY() : vect_rect[i].point[1].getY();
			z_small = vect_rect[i].point[0].getZ() < vect_rect[i].point[1].getZ() ? vect_rect[i].point[0].getZ() : vect_rect[i].point[1].getZ();
			z_big =  vect_rect[i].point[0].getZ() > vect_rect[i].point[1].getZ() ? vect_rect[i].point[0].getZ() : vect_rect[i].point[1].getZ();
			CRect rect; rect.SetRect(x_small, y_small, x_big, y_big);

			//m_pTerm->paxLog->getItem(element, i);
			m_pTerm->paxLog->getItem(element, vPaxIdx[i] );
			//element.getPaxType(&iPaxType);
	//		groupSize = element.getGroupSize();
			groupMember = element.getGroupMember();
			
			if(element.fits(paxType))				//if element fits paxType
			{										//then compare with all other log entry in the track log file
				entryTime = element.getEntryTime();		//filter0: filter no-way-to-collision pax
				exitTime = element.getExitTime();
			//	for(long j = 0; j < count; j++)
				for(long j = i+1; j < count; j++)
				{				
				//	m_pTerm->paxLog->getItem(element_tmp, j);	
					m_pTerm->paxLog->getItem(element_tmp, vPaxIdx[j] );

			**/

					/*
					element_tmp.getPaxType(&iPaxType_tmp);
					if(i >= j && iPaxType == iPaxType_tmp)		//filter0.5: not to compare repeatedly
					{
						nContinue++;
						continue;
					}
					*/

	/**
					if(element_tmp.isGreeter())					//filter0.9: no greeter/pax collision.
					{
						if(element.getVisitor() == element_tmp.getID())
							continue;
					}
					else
					{
						if(element.isGreeter())
							if(element_tmp.getVisitor() == element.getID())
								continue;
					}
					
	//				groupSize_tmp = element_tmp.getGroupSize();
					groupMember_tmp = element_tmp.getGroupMember();	//filter0.95: no pax collision in a same group
					if(element_tmp.getID() - element.getID() == groupMember_tmp - groupMember)
						continue;

					entryTime_tmp = element_tmp.getEntryTime();
					exitTime_tmp = element_tmp.getExitTime();
					if(entryTime_tmp > exitTime || exitTime_tmp < entryTime) //filter2: time impossible to meet
					{
						nContinue++;
						continue;
					}

					//filter3: no intersection in rectangle scope -- no collision
					
					x1_small = vect_rect[i].point[0].getX() < vect_rect[i].point[1].getX() ? vect_rect[i].point[0].getX() : vect_rect[i].point[1].getX();  
					x1_big =  vect_rect[i].point[0].getX() > vect_rect[i].point[1].getX() ? vect_rect[i].point[0].getX() : vect_rect[i].point[1].getX() ;
					y1_small = vect_rect[i].point[0].getY() < vect_rect[i].point[1].getY() ? vect_rect[i].point[0].getY() : vect_rect[i].point[1].getY();
					y1_big =  vect_rect[i].point[0].getY() > vect_rect[i].point[1].getY() ? vect_rect[i].point[0].getY() : vect_rect[i].point[1].getY();
					z1_small = vect_rect[i].point[0].getZ() < vect_rect[i].point[1].getZ() ? vect_rect[i].point[0].getZ() : vect_rect[i].point[1].getZ();
					z1_big =  vect_rect[i].point[0].getZ() > vect_rect[i].point[1].getZ() ? vect_rect[i].point[0].getZ() : vect_rect[i].point[1].getZ();
					
					if(!(z_small == z1_small && z_big == z1_big))
					{
						nContinue++;
						continue;
					}

					CRect rect1; rect1.SetRect(x1_small, y1_small, x1_big, y1_big);
					if(rect1.IntersectRect(&rect, &rect1) == FALSE)
					{
						nContinue++;
						continue;
					}
					

					//to find out whether there are collisions
					trackCount = element.getCount();
					trackCount_tmp = element_tmp.getCount();
					track.init(element.getEvent(0));
					PointTime1 = track.getTime();
					Point1 = track.getPoint();
					for(long trackNum = 1; trackNum < trackCount; trackNum++)
					{
						track.init(element.getEvent(trackNum));
						PointTime2 = track.getTime();
						Point2 = track.getPoint();

						//filter3.5: check if in queue;
						if(track.getState() == WaitInQueue || track.getState() == WaitForBaggage)
						{
							nContinue++;
							PointTime1 = PointTime2;
							Point1 = Point2;
							continue;
						}

						track_tmp.init(element_tmp.getEvent(0));
						PointTime1_tmp = track_tmp.getTime();
						Point1_tmp = track_tmp.getPoint();
						Line Line1; Line1.init(Point1, Point2);
						for(long trackNum_tmp = 1; trackNum_tmp < trackCount_tmp; trackNum_tmp++)
					//	for(long trackNum_tmp = trackNum+1; trackNum_tmp < trackCount_tmp; trackNum_tmp++)
						{
							track_tmp.init(element_tmp.getEvent(trackNum_tmp));
							PointTime2_tmp = track_tmp.getTime();
							Point2_tmp  = track_tmp.getPoint();

							DistanceUnit dZ = Point1.getZ();
							if( Point2.getZ() != dZ || Point1_tmp.getZ() != dZ || Point2_tmp.getZ() != dZ )
							{
								nContinue++;
								PointTime1_tmp = PointTime2_tmp;
								Point1_tmp = Point2_tmp;
								continue;	
							}
							//filter3.9: check if in queue;
							if(track_tmp.getState() == WaitInQueue || track_tmp.getState() == WaitForBaggage)
							{
								nContinue++;
								PointTime1_tmp = PointTime2_tmp;
								Point1_tmp = Point2_tmp;
								continue;							
							}
							

						//	PointTime2_tmp = track_tmp.getTime();
						//	Point2_tmp = track_tmp.getPoint();
							//filter4: check time overlap of 4 points.
							if(!(PointTime1_tmp <= PointTime1 && PointTime1_tmp >= PointTime2 || PointTime2_tmp <= PointTime1 && PointTime2_tmp >= PointTime2 || PointTime1_tmp >= PointTime1 && PointTime1_tmp <= PointTime2 || PointTime2_tmp >= PointTime1 && PointTime2_tmp <= PointTime2)) //time overlap
							{
								nContinue++;
								PointTime1_tmp = PointTime2_tmp;
								Point1_tmp = Point2_tmp;
								continue;
							}

							//filter5: check line overlap of 4 points.
							Line Line2; Line2.init(Point1_tmp, Point2_tmp);
							if(Line1.intersects(Line2) == FALSE) //line overlap
							{
								PointTime1_tmp = PointTime2_tmp;
								Point1_tmp = Point2_tmp;
								continue;
							}

							//filter6: check if in the THRESHOLD circle.
							Point intersection = Line1.intersection(Line2);
							DistanceUnit distance; 
							
							distance = Point1.distance(intersection);
							ElapsedTime t1 = PointTime1  + (long)( ( distance - threshold ) * ( PointTime2.asSeconds() - PointTime1.asSeconds() ) / Line1.GetLineLength());
							ElapsedTime t2 = PointTime2 - (long)( (Point2.distance( intersection ) - threshold ) * ( PointTime2.asSeconds() - PointTime1.asSeconds() ) / Line1.GetLineLength());
							ElapsedTime t3 = PointTime1_tmp + (long)( (Point1_tmp.distance( intersection ) - threshold ) * ( PointTime2_tmp.asSeconds() - PointTime1_tmp.asSeconds() ) / Line2.GetLineLength());
							ElapsedTime t4 = PointTime2_tmp - (long)( (Point2_tmp.distance( intersection ) - threshold ) * ( PointTime2_tmp.asSeconds() - PointTime1_tmp.asSeconds() ) / Line2.GetLineLength());

							if( !(t3 <= t2 && t4 >= t1 && t1< t2 && t3< t4) )
							{
								PointTime1_tmp = PointTime2_tmp;
								Point1_tmp = Point2_tmp;
								continue;
							}
**/
							
						//	distance*( PointTime2.asSeconds() -  PointTime1.asSeconds() ) * Line2.
	/*
							if(distance > threshold)
							{
								PointTime1_tmp = PointTime2_tmp;
								Point1_tmp = Point2_tmp;
								continue;
							}

							distance = Point2.distance(intersection);
							if(distance > threshold)
							{
								PointTime1_tmp = PointTime2_tmp;
								Point1_tmp = Point2_tmp;
								continue;
							}

							distance = Point1_tmp.distance(intersection);
							if(distance > threshold)
							{
								PointTime1_tmp = PointTime2_tmp;
								Point1_tmp = Point2_tmp;
								continue;
							}

							distance = Point2_tmp.distance(intersection);
							if(distance > threshold)
							{
								PointTime1_tmp = PointTime2_tmp;
								Point1_tmp = Point2_tmp;
							}
	*/						
/**
						    paxType.screenPrint (str, 39);
							p_file.writeField (str);
					        //p_file.writeTime (PointTime1);
							//p_file.writeTime ( t1 + long( (t2.asSeconds() - t1.asSeconds() ) /2) ,TRUE);
							
							p_file.writeTime ( ElapsedTime( (t1.asSeconds() + t2.asSeconds() + t3.asSeconds() + t4.asSeconds() ) /4)  ,TRUE);
							
							id = element.getID();
							CString strID;
							strID.Format("%ld",id);
							//p_file.writeField(strID.GetBuffer(strID.GetLength()));
							p_file.writeField(strID);
							id = element_tmp.getID();
							strID.Format("%ld",id);
							//p_file.writeField(strID.GetBuffer(strID.GetLength()));
							p_file.writeField(strID);
							p_file.writeLine();						
							nSuccess++;
							

							//Point2_tmp = Point1_tmp;
							//PointTime2_tmp = PointTime1_tmp;
							Point1_tmp = Point2_tmp;
							PointTime1_tmp = PointTime2_tmp;

						}
						PointTime1 = PointTime2;
						Point1 = Point2;
					}
				}
			}
		}
	}
**/
	
}

void CCollisionReport::GenerateSummary(ArctermFile& p_file)
{
	return;
}

void CCollisionReport::ReadSpecifications(const CString& _csProjPath)
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

		//specific for collision report only,
	    specFile.getLine();
		specFile.getInteger(m_bThreshold);
		specFile.getFloat(m_fThreshold);
		m_fThreshold = (float)(m_fThreshold * SCALE_FACTOR);
		m_nIntervalCount = 1 + (int)((m_endTime - m_startTime) / m_interval);
		

		specFile.getLine();
		int pointCount = -1;
		specFile.getInteger( pointCount );
		if( pointCount >0 )
		{
			Point* pointList = new Point[pointCount];
			for (int i = 0; i < pointCount; i++)
			{
				specFile.getPoint (pointList[i]);
			}
			m_region.init (pointCount, pointList);
			delete [] pointList;
			
		}

		specFile.closeIn();
		
	}

}
