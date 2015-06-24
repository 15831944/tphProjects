#include "StdAfx.h"
#include ".\congestionareagrid.h"
#include "..\Common\Line2008.h"
#include "..\Common\DistanceLineLine.h"

#define  MAX_COUNT 1
CongestionCellCentrePoint::CongestionCellCentrePoint(Point ptt)
{
	x = (int)( (ptt.getX() / 100.0 ) );
	y = (int)( (ptt.getY() / 100.0 ) );
}

CCongestionAreaGrid::CCongestionAreaGrid(void)
{
	m_Curfloor = 0.0;
	m_vCells.clear();
	m_AllPtVec.clear();
	m_AllPolVec.clear();
	m_MaxRect.bottom = m_MaxRect.top = m_MaxRect.left = m_MaxRect.right = 0;
}

CCongestionAreaGrid::~CCongestionAreaGrid(void)
{
	m_vCells.clear();
	m_AllPtVec.clear();
	m_AllPolVec.clear();
	m_MaxRect.bottom = m_MaxRect.top = m_MaxRect.left = m_MaxRect.right = 0;
}


void CCongestionAreaGrid::InitGrid(CAreas* pAreas, const std::vector<CPipe*>& pPipes )
{
	GetAreasInfo( pAreas );
	GetPipeInfo( pPipes );

/*	// assert
#ifdef _DEBUG
	for( int i = 0; i < (int)m_AllPolVec.size(); i++ )
	{
		assert( m_AllPolVec[i].calculateArea() > 0.0 );
	}	
#endif
*/
	GetMaxRectByPoint(m_MaxRect, m_AllPtVec);

	GetUsableCell();
/*
#ifdef _DEBUG
	char debugbuf[255];
	std::map<CongestionCellCentrePoint, CCongestionAreaCell, CellMapCmp>::iterator pcelliter = m_vCells.begin();
	FILE *fwtemp;
	int j;
	static bool flg = false;

//	if(0)
	{
		flg = true;
		if( !g_debugfileflg )
		{
			fwtemp = fopen("c:\\GridCellTest.log", "w");
			g_debugfileflg = true;
		}
		else
			fwtemp = fopen("c:\\GridCellTest.log", "a+");


		fputs("\n[[[	Output cell center point information	]\n", fwtemp);
		for( j=0; pcelliter !=  m_vCells.end(); pcelliter++ , j++)
		{
			sprintf( debugbuf, "UsableCell %d	x:%d y:%d	", j, pcelliter->first.x, pcelliter->first.y );
			fputs( debugbuf, fwtemp);
			if( j%2 == 0)
				fputs( "\n",fwtemp);

		}
		fputs("\n[[[	Output End	]\n", fwtemp);

		for( j=0; j < (int)m_AllPolVec.size(); j++)
		{
			//fprintf(fwtemp, "\nm_AllPolVec: %d:\n", j);
			sprintf(debugbuf, "\nm_AllPolVec: %d:\n", j);
			fputs( debugbuf, fwtemp);

			for( int z=0; z < (int)m_AllPolVec[j].getCount(); z++ )
			{
				sprintf(debugbuf, "\nPoint %d: x=%f, y=%f\n", 
					z, m_AllPolVec[j].getPoint(z).getX(),m_AllPolVec[j].getPoint(z).getY() );
				fputs( debugbuf, fwtemp);
			}
		}

		sprintf(debugbuf, "\nMax Rect Area --- left:%d top:%d right:%d bottom: %d\n", m_MaxRect.left, m_MaxRect.top, m_MaxRect.right, m_MaxRect.bottom);
		fputs( debugbuf, fwtemp);

		fclose( fwtemp );
	}
#endif
*/
}

#ifdef _DEBUG
CCongestionAreaCell* CCongestionAreaGrid::GetCell(const Point pt, Person *pPerson)
#else
CCongestionAreaCell* CCongestionAreaGrid::GetCell(const Point pt)
#endif
{
	CCongestionAreaCell *retcell = NULL;
	if( (int)m_vCells.size()<1 )
		return retcell;

#ifdef _DEBUG
	POINT pm;
	pm.x = (LONG)(pt.getX() / 100.0) ;
	pm.y = (LONG)(pt.getY() / 100.0) ;
//	if( FALSE == PtInRect(&m_MaxRect, pm) )
//		return retcell;
	CRect rc = m_MaxRect;
	rc.NormalizeRect();
	if( FALSE == PtInRect(&rc, pm) )
		Sleep(0);
#endif

	CongestionCellCentrePoint ptemp( pt );

	const int col = ( (int)( ptemp.x - m_MaxRect.left) / CELLWIDTH  );
	const int row = ( (int)( ptemp.y - m_MaxRect.top) / CELLHEIGHT  );
	RECT temprc;
	temprc.left = m_MaxRect.left + col * CELLWIDTH;
//	temprc.right = temprc.left + CELLWIDTH;
	
	temprc.top = m_MaxRect.top + row * CELLHEIGHT;
//	temprc.bottom = temprc.top + CELLHEIGHT;


	int x = temprc.left;// + CELLWIDTH / 2;
	int y = temprc.top ;//+ CELLHEIGHT / 2 ;
	CongestionCellCentrePoint ptindx ;
	ptindx.x = x;
	ptindx.y = y;

	std::map<CongestionCellCentrePoint, CCongestionAreaCell, CellMapCmp>::iterator ptcell;
	ptcell = m_vCells.find(ptindx) ;
/*
#ifdef _DEBUG
	FILE *fwtemp = fopen("c:\\GridCellTest.log", "a+");
//	char debugbuf[255];
	static bool tflg = false;
	if(!tflg)
	{
		fputs("\n---------	GetCell() Test!-------\n",fwtemp);
		tflg = true;
	}

	if( 0 && pPerson && (pPerson->getLogEntry()).getID() == 171 )
	{
		if(  ptcell == m_vCells.end() )
		{	
			fprintf(fwtemp, "\ncurrent Point is:x=%5.1f, y=%5.1f; the Cell left-top point is:left=%d, top=%d; Cell address is:NULL",
				pt.getX(), pt.getY(), ptindx.x, ptindx.y);
		}
		else
		{
			fprintf(fwtemp, "\ncurrent Point is:x=%5.1f,, y=%5.1f,; the Cell left-top point is:left=%d, top=%d; Cell address is: %d",
				pt.getX(), pt.getY(), ptindx.x, ptindx.y, &(ptcell->second));
		}
	}

	fclose(fwtemp);
#endif
*/
	if(  ptcell == m_vCells.end() )
	{
		return NULL;
	}

	return &(ptcell->second);
}

// Get Intersection point from pts,pte, to Pollygon, and it is min distance.
bool CCongestionAreaGrid::GetLineToPollygonIntersection(const Pollygon& pol, 
														const Point pts, const Point pte, Point& intersectpoint)
{
	Point temppoint;
	bool ret = false;
	Line tmpline, lne;
	int i;	
	lne.init(pts, pte);

	int polcount = pol.getCount();
	if( polcount < 2 )
		return ret;


	for( i=0; i+1 < polcount; i++ )
	{
        tmpline.init( pol.getPoint(i), pol.getPoint(i+1));

		if( tmpline.intersects( lne ) )
		{
			temppoint = tmpline.intersection( lne );

			if(ret)
			{
				if( temppoint.distance(pts) < intersectpoint.distance(pts) )
					intersectpoint = temppoint; // get min distance point by pts.
			}
			else
			{
				intersectpoint = temppoint; // get min distance point by pts.
			}

			ret = true;
//			break;
		}
	}

//	if( !ret )
	{

		tmpline.init( pol.getPoint(polcount-1), pol.getPoint(0) );
		if( tmpline.intersects( lne ) )
		{
			temppoint = tmpline.intersection( lne );
			if(ret)
			{
				if( temppoint.distance(pts) < intersectpoint.distance(pts) )
					intersectpoint = temppoint; // get min distace point by pts.
			}
			else
			{
				intersectpoint = temppoint; // get min distace point by pts.
			}
			ret = true;
		}
	}


    return ret;
}

void CCongestionAreaGrid::GetRectByLeftTopPoint(long lf, long tp, RECT& rc)
{	
	rc.left = lf;
	rc.top = tp;
	rc.right = rc.left + CELLWIDTH;
	rc.bottom = rc.top + CELLHEIGHT;
}

// Get InterSectPoint from min cell.
bool CCongestionAreaGrid::GetInterSectPoint(const Point pts, const Point pte, Point& interpoint)
{
/*
#ifdef _DEBUG
	FILE *fwtemp = fopen("c:\\GridCellTest.log", "a+");
	fprintf(fwtemp, "start:x=%5.2f,y=%5.2f; end: x=%5.2f,y=%5.2f\n", pts.getX(), pts.getY(), pte.getX(), pte.getY());
	fclose(fwtemp);
#endif
*/
	bool ret = false;
//	int i;
//	int cellcount = (int)m_vCells.size();
//	int polcount = (int)m_AllPolVec.size();
	std::map<CongestionCellCentrePoint, CCongestionAreaCell, CellMapCmp>::iterator pcelliter;

	// if the first point pts is in the pollygon , then return itselft.
//	for(i =0 ; i<polcount; i++)
//	{
//		if(	m_AllPolVec[i].contains(pts) )
//		{
			// and just it is in usable cell.
			POINT pt;
			pt.x = (long)(pts.getX() / 100.0);
			pt.y = (long)(pts.getY() / 100.0);

			pcelliter = m_vCells.begin();
			for( ; pcelliter !=  m_vCells.end(); pcelliter++ )
			{
				RECT rc;
				GetRectByLeftTopPoint(pcelliter->first.x, pcelliter->first.y, rc);
				if( PtInRect(&rc, pt) )
				{
					ret = true;
					interpoint.setX( (double)pt.x * 100.0);
					interpoint.setY( (double)pt.y * 100.0);
					break;
				}				
			}

//			if( ret )
//				break;
//		}
//	}
	if( ret )
		return ret;
	
	Point temppoint, intersectpoint;

	pcelliter = m_vCells.begin();
	// get intersection point.
	for( ; pcelliter !=  m_vCells.end(); pcelliter++ )
	{
		RECT rc;
		Point ptlist[4];
		Pollygon poltemp;
		GetRectByLeftTopPoint(pcelliter->first.x, pcelliter->first.y, rc);
		ptlist[0].init( (double)rc.left * 100.0, (double)rc.top * 100.0, pts.getZ() );
		ptlist[1].init( (double)rc.left * 100.0, (double)rc.bottom * 100.0, pts.getZ() );
		ptlist[2].init( (double)rc.right * 100.0, (double)rc.bottom * 100.0, pts.getZ() );
		ptlist[3].init( (double)rc.right * 100.0, (double)rc.top * 100.0, pts.getZ() );
		poltemp.init( 4, ptlist);

		if( GetLineToPollygonIntersection( poltemp, pts, pte, temppoint) )
		{				
			if(ret)
			{
				if( temppoint.distance(pts) < intersectpoint.distance(pts) )
				{
//					if( GetCell(temppoint) )
					{
						intersectpoint = temppoint; // get min distance point by pts.
						ret = true;
					}
				}
			}
			else
			{
//				if( GetCell(temppoint) )
				{
					intersectpoint = temppoint; // get min distance point by pts.
					ret = true;
				}
			}
			
		}
	}

	interpoint = intersectpoint;
	interpoint = interpoint.GetDistancePoint(pte, 0.5 * 100.0);

	return ret;
}

void CCongestionAreaGrid::GetAreasInfo(CAreas* pAreas)
{
	Pollygon region;
	int i, j;

	if(pAreas == NULL)
		return ;
	int AreaCount = pAreas->m_vAreas.size();
	for(i=0; i<AreaCount; i++)
	{
		CArea* ptArea = pAreas->m_vAreas[i];		

		ptArea->GetAreaCoveredRegion( region );
		m_AllPolVec.push_back(region);

		int ptcount = ptArea->getPointCount();
		for(j=0; j<ptcount; j++)
		{
			Point ptemp = ptArea->getPointByIndex(j);
			m_AllPtVec.push_back(ptemp);
		}
	}

}

void CCongestionAreaGrid::GetPipeInfo(const std::vector<CPipe*>& pPipes)
{
	std::vector<Pollygon> polvec;
	if(pPipes.empty())
		return ;
	int PipeCount = pPipes.size();
	for(int i=0; i<PipeCount; i++)
	{
		pPipes.at(i)->GetPipeCoveredRegion(polvec);
		int polcount = polvec.size();
		for(int j=0; j<polcount; j++)
		{
			m_AllPolVec.push_back( polvec[j] ); // add a pollygon;

			// add point list;
			int ptcount = polvec[j].getCount();
			for(int m = 0; m<ptcount; m++ )
			{
				Point ptemp = polvec[j].getPoint(m);
				m_AllPtVec.push_back(ptemp);
			}
		}
	}
}

bool CCongestionAreaGrid::GetMaxRectByPoint( RECT& maxrc, const std::vector<Point>& ptvec)
{
	int ptcount = (int)ptvec.size();
	if( ptcount < 3)
		return false;

	double leftx, topy, rightx, bottomy;
	rightx = leftx = ptvec[0].getX();
	topy = bottomy = ptvec[0].getY();
	for( int i = 1; i<ptcount; i++ )
	{
		if( ptvec[i].getX() < leftx )
			leftx = ptvec[i].getX();
		else if( ptvec[i].getX() > rightx )
			rightx = ptvec[i].getX();

		if( ptvec[i].getY() < topy )
			topy = ptvec[i].getY();
		else if( ptvec[i].getY() > bottomy )
			bottomy = ptvec[i].getY();
	}

	const double errorval = 1.0;
	maxrc.left = (LONG)(leftx /100.0 - errorval);
	maxrc.right = (LONG)(rightx /100.0 + errorval);
	maxrc.top = (LONG)(topy /100.0 - errorval);
	maxrc.bottom = (LONG)(bottomy /100.0 + errorval);

	return true;
}

long CCongestionAreaGrid::GetUsableCell( )
{
	int rcWidth = abs(m_MaxRect.right - m_MaxRect.left); // unit is metre.
	int rcHeight = abs(m_MaxRect.bottom - m_MaxRect.top);
	int RowCount, ColumnCount;
	RECT temprc;
	int i, j;

	if(m_AllPtVec.size() <= 0)
		return 0;
	m_Curfloor = m_AllPtVec[0].getZ();

	ColumnCount = rcWidth / CELLWIDTH;
	if( 0 != rcWidth % CELLWIDTH )
		ColumnCount += 1;
	RowCount = rcHeight / CELLHEIGHT;
	if( 0 != rcHeight % CELLHEIGHT )
		RowCount += 1;		

	// make up every min cell.
	for(i=0; i<RowCount; i++)
	{
		temprc.top = m_MaxRect.top + i * CELLHEIGHT ;//+ 1;
		temprc.bottom = temprc.top + CELLHEIGHT ;//- 1;

		for(j=0; j<ColumnCount; j++)
		{
			temprc.left = m_MaxRect.left + j * CELLWIDTH ;
			temprc.right = temprc.left + CELLWIDTH ;
			// Just it is needed.
			if( IsNeedRect(temprc, m_Curfloor) )
			{
				CongestionCellCentrePoint ptt;
				ptt.x = temprc.left;
				ptt.y = temprc.top;
				CCongestionAreaCell aCell; 				
				m_vCells.insert( std::make_pair(ptt, aCell) );				

			}

		}

	}

	return (long)m_vCells.size();
}

bool CCongestionAreaGrid::IsNeedRect(RECT& rc, double curfloor) // just this rc is needed!
{
	bool ret = false;
	Point pt1, pt2, pt3, pt4;
	const double errorval = 0;
	Point LeftTopTemp, RightBottomTemp;
	LeftTopTemp.setX((double)rc.left - errorval);
	LeftTopTemp.setY((double)rc.top - errorval);
	RightBottomTemp.setX((double)rc.right + errorval);
	RightBottomTemp.setY((double)rc.bottom + errorval);

	int polcount = (int)m_AllPolVec.size();

	pt1.init(LeftTopTemp.getX() * 100.0 , LeftTopTemp.getY()  * 100.0 , curfloor);
	pt2.init(LeftTopTemp.getX() * 100.0 , RightBottomTemp.getY()  * 100.0, curfloor);
	pt3.init(RightBottomTemp.getX() * 100.0, RightBottomTemp.getY()* 100.0, curfloor);
	pt4.init(RightBottomTemp.getX() * 100.0, LeftTopTemp.getY() * 100.0 , curfloor);

	for(int i =0 ; i<polcount; i++)
	{

		if(	m_AllPolVec[i].contains(pt1) ||
			m_AllPolVec[i].contains(pt2) ||
			m_AllPolVec[i].contains(pt3) ||
			m_AllPolVec[i].contains(pt4)
			)
		{
			ret = true;
			break;
		}

		if( m_AllPolVec[i].intersects( pt1, pt2) ||
			m_AllPolVec[i].intersects( pt2, pt3) ||
			m_AllPolVec[i].intersects( pt3, pt4) ||
			m_AllPolVec[i].intersects( pt4, pt1)
			)
		{
			ret = true;
			break;
		}
	}

	return ret;
}

bool CCongestionAreaGrid::IsValidCongestionArea()
{
	if( abs(m_MaxRect.bottom - m_MaxRect.top) < 1 ||
		abs(m_MaxRect.right - m_MaxRect.left) < 1 )
		return false;

	return true;
}

Point GetGridRandomPoint(const Point& pCurrentPt)
{
	CongestionCellCentrePoint pt(pCurrentPt);
	if (pCurrentPt.getX() < 0)
	{
		pt.x -= 1l;
	}

	if (pCurrentPt.getY() > 0)
	{
		pt.y += 1l;
	}
	Point point;
	double xProb, yProb;
	xProb = (double)rand() / RAND_MAX;
	yProb = (double)rand() / RAND_MAX;
	point.setX(pt.x * 100 + 100*xProb);
	point.setY(pt.y * 100 - 100*yProb);
	point.setZ(pCurrentPt.getZ());
	return point;
}

bool GetPoint(const Point& nearPt,const Point& pCurrentPt,const Point& pDestPt,Point& resultPt)
{
	CPoint2008 ptList1[2];
	CPoint2008 ptlist2[4];
	CPath2008 path1;
	ptList1[0].init(pCurrentPt.getX(),pCurrentPt.getY(),pCurrentPt.getZ());
	ptList1[1].init(pDestPt.getX(),pDestPt.getY(),pDestPt.getZ());
	path1.init(2,ptList1);

	CPath2008 path2;
	CongestionCellCentrePoint nearCentrerPt(nearPt);
	if (nearPt.getX() < 0)
	{
		nearCentrerPt.x -= 1l;
	}

	if (nearPt.getY()> 0)
	{
		nearCentrerPt.y += 1l;
	}
	ptlist2[0].init( (double)(nearCentrerPt.x) * 100.0, (double)(nearCentrerPt.y) * 100.0,nearPt.getZ());
	ptlist2[1].init( (double)(nearCentrerPt.x) * 100.0, (double)(nearCentrerPt.y -1.0) * 100.0, nearPt.getZ() );
	ptlist2[2].init( (double)(nearCentrerPt.x + 1.0) * 100.0, (double)(nearCentrerPt.y - 1.0) * 100.0, nearPt.getZ() );
	ptlist2[3].init( (double)(nearCentrerPt.x + 1.0) * 100.0, (double)(nearCentrerPt.y) * 100.0, nearPt.getZ() );
	path2.init( 4, ptlist2);

	IntersectPathPolygon2D linePath(path1,path2);
	linePath.Intersects();

	if (linePath.m_vpathParameters.size() > 0)
	{
		if (linePath.m_vpathParameters.size() == 1)
		{
			CPoint2008 pts = path1.GetDistPoint(linePath.m_vpathParameters[0]*path1.GetTotalLength());
			if((fabsl(nearCentrerPt.x) > fabsl(pts.getX())) && (fabsl(nearCentrerPt.y) > fabsl(pts.getY())))
			{
				resultPt.setPoint(pts.getX(),pts.getY(),pts.getZ());
				return true;
			}
		}
		else if (linePath.m_vpathParameters.size() == 2)
		{
			DistanceUnit dAlt = fabsl(linePath.m_vpathParameters[0]-linePath.m_vpathParameters[1])*path1.GetTotalLength();
			dAlt*=((double)rand() / RAND_MAX);
			dAlt += min(linePath.m_vpathParameters[0],linePath.m_vpathParameters[1])*path1.GetTotalLength();
			CPoint2008 pts = path1.GetDistPoint(dAlt);
			resultPt.setPoint(pts.getX(),pts.getY(),pts.getZ());
			return true;
		}

	}

	return false;
}
Point CCongestionAreaGrid::GetNearestPoint(const Point& pCurrentPt,const Point& pDestPt)
{
	//////////////////////////////////////////////////////////
	/*				||
				--		--
				--	pt	--
					||          */
	CongestionCellCentrePoint pt(pCurrentPt);
	if (pCurrentPt.getX() < 0)
	{
		pt.x -= 1l;
	}

	if (pCurrentPt.getY() > 0)
	{
		pt.y += 1l;
	}
	
	Point nearPt;
	Point tempPt;
	long minCount = LONG_MAX;
	std::vector<Point>vDistPoint;
	std::vector<CCongestionAreaCell*>vAreaCell;
	vDistPoint.clear();
	vAreaCell.clear();
	//x
	tempPt.setX((pt.x - 0.5)*100.0);
	tempPt.setY((pt.y + 0.5) * 100.0);
	tempPt.setZ(pCurrentPt.getZ());
	if (GetCell(tempPt))
	{
		vDistPoint.push_back(tempPt);
		vAreaCell.push_back(GetCell(tempPt));
	}

	tempPt.setX((pt.x + 0.5)*100.0);
	tempPt.setY((pt.y + 0.5) * 100.0);
	if (GetCell(tempPt))
	{
		vDistPoint.push_back(tempPt);
		vAreaCell.push_back(GetCell(tempPt));
	}
	
	tempPt.setX((pt.x + 1.5)*100.0);
	tempPt.setY((pt.y+ 0.5) * 100.0);
	if (GetCell(tempPt))
	{
		vDistPoint.push_back(tempPt);
		vAreaCell.push_back(GetCell(tempPt));
	}


	tempPt.setX((pt.x - 0.5)*100.0);
	tempPt.setY((pt.y - 0.5) * 100.0);
	if (GetCell(tempPt))
	{
		vDistPoint.push_back(tempPt);
		vAreaCell.push_back(GetCell(tempPt));
	}

	//
	tempPt.setX((pt.x + 1.5)*100.0);
	tempPt.setY((pt.y - 0.5) * 100.0);
	if (GetCell(tempPt))
	{
		vDistPoint.push_back(tempPt);
		vAreaCell.push_back(GetCell(tempPt));
	}

	tempPt.setX((pt.x - 0.5)*100.0);
	tempPt.setY((pt.y - 1.5) * 100.0);
	if (GetCell(tempPt))
	{
		vDistPoint.push_back(tempPt);
		vAreaCell.push_back(GetCell(tempPt));
	}

	tempPt.setX((pt.x + 0.5)*100.0);
	tempPt.setY((pt.y - 1.5) * 100.0);
	if (GetCell(tempPt))
	{
		vDistPoint.push_back(tempPt);
		vAreaCell.push_back(GetCell(tempPt));
	}

	tempPt.setX((pt.x+1.5)*100.0);
	tempPt.setY((pt.y - 1.5) * 100.0);
	if (GetCell(tempPt))
	{
		vDistPoint.push_back(tempPt);
		vAreaCell.push_back(GetCell(tempPt));
	}

	if (std::find(vAreaCell.begin(),vAreaCell.end(),GetCell(pDestPt)) != vAreaCell.end()\
		|| (pCurrentPt != pDestPt && GetCell(pCurrentPt) == GetCell(pDestPt)) || (pCurrentPt == pDestPt))
	{
		return pDestPt;
	}

	if (vDistPoint.size() == 0)
	{
		return pDestPt;
	}

	for (size_t i = 0; i < vDistPoint.size(); i++)
	{
		CCongestionAreaCell* pCell = GetCell(vDistPoint[i]);
		if (pCell && pCell->GetPersonsCount() <= MAX_COUNT)
		{
			Point resultPt;
			if (GetPoint(vDistPoint[i],pCurrentPt,pDestPt,resultPt))
			{
				return resultPt;
			}
		}
	}

	nearPt = vDistPoint[0];
	for (i = 1; i < vDistPoint.size(); i++)
	{
		CCongestionAreaCell* pCell = GetCell(vDistPoint[i]);
		if (pCell && pDestPt.distance(vDistPoint[i]) < pDestPt.distance(nearPt))
		{
			nearPt = vDistPoint[i];
		}
	}

	tempPt.setX((pt.x + 0.5)*100.0);
	tempPt.setY((pt.y - 0.5) * 100.0);
	if (/*(pDestPt.distance(pCurrentPt) < pDestPt.distance(nearPt)) || */(pDestPt.distance(tempPt) < pDestPt.distance(nearPt)))
	{
		return pDestPt;
	}

	return GetGridRandomPoint(nearPt);

}
