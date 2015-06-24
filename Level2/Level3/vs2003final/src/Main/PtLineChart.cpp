// PtLineChart.cpp : 
//

#include "stdafx.h"
#include "PtLineChart.h"
#include ".\ptlinechart.h"
#include "common\path.h"
#include <math.h>
#include "TermPlanDoc.h"

// CPtLineChart

IMPLEMENT_DYNAMIC(CPtLineChart, CStatic)
CPtLineChart::CPtLineChart()
: m_nSeledPointIndex(-1)
, m_ptLastDraged(INT_MIN,INT_MIN)
, m_bDragging(false)
,m_bCursorMove(false)
,m_pPath( NULL )
, m_nNearestYIndex(-1)
,m_envMode(EnvMode_Terminal)
{
}

CPtLineChart::~CPtLineChart()
{
}


BEGIN_MESSAGE_MAP(CPtLineChart, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()




void CPtLineChart::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CFont* pFontOld=dc.SelectObject(GetFont());

	CRect rectWnd;
	GetClientRect(rectWnd);
	// Draw background in white
	dc.FillSolidRect(rectWnd,RGB(255,255,255));
	rectWnd.InflateRect(-2,-2);
	// Must first draw Vertinal Coor,Because first calc some params.
	// Such as m_nBottomestPos..
	DrawCoordinate(&dc,rectWnd,FALSE);
	DrawCoordinate(&dc,rectWnd,TRUE);
	dc.SelectObject(pFontOld);
}

// Draw Coordinate
void CPtLineChart::DrawCoordinate(CDC* pDC,CRect rectWnd,bool bHorizontal)
{

	CRect rectHori=CalcCoorRect(rectWnd,bHorizontal);
	CPoint ptBegin,ptEnd;
	ptEnd=ptBegin=rectHori.CenterPoint();
	if(bHorizontal)
	{
		ptBegin.x=rectHori.left;
		ptEnd.x=rectHori.right;
		ptBegin.y = ptBegin.y - 8;
		ptEnd.y = ptEnd.y - 8;
	}
	else
	{
		ptBegin.y=rectHori.top;
		ptEnd.y=rectHori.bottom -8;
	}
	pDC->MoveTo(ptBegin);
	pDC->LineTo(ptEnd);
	DrawArrow(pDC,rectHori,bHorizontal);
	DrawTick(pDC,rectHori,bHorizontal);
}

// Set Data
void CPtLineChart::SetData(std::vector<double>& vPointsXPos, std::vector<double>& vPointsYPos,std::vector<double>& vVertCoorData, Path* _pPath,CStringArray* pStrArrFloorName, bool bRefresh,EnvironmentMode envMode)
{
	// Clear Data
	m_vPointXPos.clear();
	m_vPointYPos.clear();
	m_vVertCoorData.clear();
	m_vPointRect.clear();
	m_envMode = envMode;
	int i;
	for(i=0;i<static_cast<int>(vPointsXPos.size());i++)
	{
		m_vPointXPos.push_back(vPointsXPos[i]);
		m_vPointRect.push_back(CRect());
	}

	for(i=0;i<static_cast<int>(vPointsYPos.size());i++)
	{
		m_vPointYPos.push_back(vPointsYPos[i]);
	}

	for(i=0;i<static_cast<int>(vVertCoorData.size());i++)
	{
		m_vVertCoorData.push_back(vVertCoorData[i]);
	}
	
	m_pPath = _pPath;
	m_pStrArrFloorName=pStrArrFloorName;
	if(bRefresh)
		Invalidate();
}

CRect CPtLineChart::CalcCoorRect(CRect rectWnd,bool bHorizontal)
{
	// TRACE("ARROW_WIDTH=%d\n",ARROW_WIDTH);
	CRect rectRes=rectWnd;
	if(bHorizontal)
	{
		rectRes.top=rectRes.bottom-ARROW_WIDTH;
		rectRes.left+=(ARROW_WIDTH)/2;
	}
	else
	{
		rectRes.right=rectRes.left+ARROW_WIDTH;
		rectRes.bottom-=(ARROW_WIDTH)/2;
	}
	return rectRes;
}

BOOL CPtLineChart::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

CRect CPtLineChart::CalcArrow(CRect rectCoor, bool bHorizontal)
{
	CRect rectRes=rectCoor;
	if(bHorizontal)
	{
		rectRes.left=rectRes	.right-ARROW_LAYER_COUNT;
		rectRes.bottom=rectRes.top+ARROW_LAYER_COUNT - 6;
	}
	else
	{
		rectRes.bottom=rectRes.top+ARROW_LAYER_COUNT;
	}
	return rectRes;
}

void CPtLineChart::DrawArrow(CDC* pDC, CRect rectCoor, bool bHorizontal)
{
	CRect rectArrow=CalcArrow(rectCoor,bHorizontal);
	CPoint ptBegin,ptEnd;
	CPoint ptCenter=rectArrow.CenterPoint();
	if(bHorizontal)
	{
		for(int i=0;i<ARROW_LAYER_COUNT;i++)
		{
			ptBegin.x=ptEnd.x=rectArrow.right-i;
			ptBegin.y=ptCenter.y-i - 1;
			ptEnd.y=ptCenter.y+i - 1;
			pDC->MoveTo(ptBegin);
			pDC->LineTo(ptEnd);
		}	
	}
	else
	{
		for(int i=0;i<ARROW_LAYER_COUNT;i++)
		{
			ptBegin.y=ptEnd.y=rectArrow.top+i;
			ptBegin.x=ptCenter.x-i;
			ptEnd.x=ptCenter.x+i;
			pDC->MoveTo(ptBegin);
			pDC->LineTo(ptEnd);
		}	
	}

}

void CPtLineChart::DrawTick(CDC* pDC,CRect rectCoor, bool bHorizontal)
{
	CSize sizeTemp=pDC->GetTextExtent("00000000.00");
	
	CPen penDash(PS_DOT,1,RGB(192,192,192));
	if(bHorizontal)
	{
		rectCoor.InflateRect(-rectCoor.Width()/20-sizeTemp.cx,0);
		m_nLeftestPos=rectCoor.left;
		int nCoorWidth=rectCoor.Width();
		int nCount=m_vPointXPos.size();
		if(nCount>1)
		{
			double fMin=m_vPointXPos[0];
			double fMax=m_vPointXPos[nCount-1];
			m_fPixelPerUnitH=nCoorWidth/(fMax-fMin);
			int nX,nY0=rectCoor.top,nY1=nY0+ARROW_WIDTH/2;
			for(int i=0;i<nCount;i++)
			{
				nX=m_nLeftestPos+static_cast<int>((m_vPointXPos[i]-fMin)*m_fPixelPerUnitH);

				CPen* pOldPen=pDC->SelectObject(&penDash);
				pDC->MoveTo(nX,nY1);
				pDC->LineTo(nX,0);
				pDC->SelectObject(pOldPen);

				pDC->MoveTo(nX,nY0);
				pDC->LineTo(nX,nY1);
				// Draw Tick Text
				CString strTick;
				strTick.Format("%.2f",m_vPointXPos[i]);
				CSize sizeStrTick=pDC->GetTextExtent(strTick);
				CRect rectTick;
				rectTick.left=nX-sizeStrTick.cx/2;
				rectTick.right=rectTick.left+sizeStrTick.cx;
				rectTick.bottom=nY0 - 2;
				rectTick.top=rectTick.bottom-sizeStrTick.cy - 2;
				pDC->DrawText(strTick,rectTick,DT_CENTER|DT_SINGLELINE|DT_VCENTER);

				// Draw number
				CString strNum;
				strNum.Format("%d",i+1);

				CSize sizeStrNum=pDC->GetTextExtent(strNum);
				CRect rectNum;
				rectNum.left=nX-sizeStrNum.cx/2;
				rectNum.right=rectNum.left+sizeStrNum.cx;
				rectNum.bottom=nY0;
				rectNum.top=rectNum.bottom + sizeStrNum.cy;
				pDC->DrawText(strNum,rectNum,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
				// Draw Point Box
				DrawPoint(pDC,nX,i);
			}	
			// Link points in line
			int nPointCount=m_vPointRect.size();
			CPoint* pPtTemp=new CPoint[nPointCount];
			for(i=0;i<nPointCount;i++)
			{
				pPtTemp[i]=m_vPointRect[i].CenterPoint();
			}
			pDC->Polyline(pPtTemp,nPointCount);
			delete []pPtTemp;
		}
	}
	else
	{
		rectCoor.InflateRect(0,-rectCoor.Height()/20-sizeTemp.cy);
		m_nBottomestPos=rectCoor.bottom;
		m_nTopestPos=rectCoor.top;
		int nCoorHeight=rectCoor.Height();
		int nCount=m_vVertCoorData.size();
		if(nCount>1)
		{
			CRect rectWnd;
			GetClientRect(rectWnd);
			double fMin=m_vVertCoorData[0];
			double fMax=m_vVertCoorData[nCount-1];
			 m_fPixelPerUnitV=nCoorHeight/(fMax-fMin);
			int nY,nX0=rectCoor.right,nX1=nX0-ARROW_WIDTH/2;
			for(int i=0;i<nCount;i++)
			{
				nY=m_nBottomestPos-static_cast<int>((m_vVertCoorData[i]-fMin)*m_fPixelPerUnitV);

				CPen* pOldPen=pDC->SelectObject(&penDash);
				pDC->MoveTo(rectWnd.right,nY);
				pDC->LineTo(nX1,nY);
				pDC->SelectObject(pOldPen);

				pDC->MoveTo(nX0,nY);
				pDC->LineTo(nX1,nY);
				// Draw Tick Text
				CString strTick=m_pStrArrFloorName->GetAt(i);
				//strTick.Format("%.0f floor",m_vVertCoorData[i]);
				CSize sizeStrTick=sizeTemp;
				CRect rectTick;
				rectTick.left=nX0;
				rectTick.right=rectTick.left+sizeStrTick.cx;
				rectTick.bottom=nY+sizeStrTick.cy/2;
				rectTick.top=rectTick.bottom-sizeStrTick.cy;
				pDC->DrawText(strTick,rectTick,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

			}	
		}	
	}
}

void CPtLineChart::DrawPoint(CDC* pDC,int nX,int nIndex)
{
	int nY=m_nBottomestPos-static_cast<int>((m_vPointYPos[nIndex]-m_vVertCoorData[0])/*fMin*/*m_fPixelPerUnitV);
	m_vPointRect[nIndex]=DrawPointBox(pDC,nX,nY,nIndex==m_nSeledPointIndex);

}

void CPtLineChart::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	Invalidate();
}

void CPtLineChart::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nOldIndex=m_nSeledPointIndex;
	m_nSeledPointIndex=-1;
	for(int i=0;i<static_cast<int>(m_vPointRect.size());i++)
	{
		if(m_vPointRect[i].PtInRect(point))
		{
			m_nSeledPointIndex=i;
			m_bDragging=true;
			break;
		}
	}
	if(nOldIndex!=m_nSeledPointIndex)
		Invalidate();
	CWnd::OnLButtonDown(nFlags, point);
}

void CPtLineChart::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bCursorMove&&m_bDragging&&m_nSeledPointIndex!=-1)
	{
		if(nFlags&MK_CONTROL) // Snap to floor
		{
			CPoint ptTemp=point;
			int nNearestIndex=GetNearestYIndex(ptTemp);
			m_vPointYPos[m_nSeledPointIndex]=m_vVertCoorData[nNearestIndex];
		}
		else
			m_vPointYPos[m_nSeledPointIndex]=(m_nBottomestPos-m_ptLastDraged.y)/m_fPixelPerUnitV+m_vVertCoorData[0];
		calculateLengthAngle( m_nSeledPointIndex );
		Invalidate();
	}
	m_ptLastDraged=CPoint(INT_MIN,INT_MIN);
	m_bDragging=false;
	m_bCursorMove=false;
	CWnd::OnLButtonUp(nFlags, point);
}

void CPtLineChart::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bDragging&&(m_nSeledPointIndex!=-1))
	{
		if(point.y>=m_nTopestPos&&point.y<=m_nBottomestPos)
		{
			CPoint ptPoint=m_vPointRect[m_nSeledPointIndex].CenterPoint();
			CClientDC dc(this);
			dc.SetROP2(R2_XORPEN);

			int nOffsetY;
			if((m_ptLastDraged.x!=INT_MIN)&&(m_ptLastDraged.y!=INT_MIN))
			{
				nOffsetY=m_ptLastDraged.y-ptPoint.y;
				DrawPointBox(&dc,ptPoint.x,ptPoint.y+nOffsetY,TRUE);
			}
			CPoint ptTemp=point;
			if(nFlags&MK_CONTROL) // Snap to floor
			{
				GetNearestYIndex(ptTemp);
			}
			nOffsetY=ptTemp.y-ptPoint.y;
			DrawPointBox(&dc,ptPoint.x,ptPoint.y+nOffsetY,TRUE);
			m_ptLastDraged=ptTemp;
			m_bCursorMove=TRUE;
		}
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CPtLineChart::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(m_nSeledPointIndex!=-1)
	{
		if(point.y>=m_nTopestPos&&point.y<=m_nBottomestPos)
		{
			CPoint ptPoint=m_vPointRect[m_nSeledPointIndex].CenterPoint();
			
			CPoint ptTemp=point;
			int nNearestIndex=GetNearestYIndex(ptTemp);
			m_ptLastDraged=ptTemp;
			m_vPointYPos[m_nSeledPointIndex]=m_vVertCoorData[nNearestIndex];
			calculateLengthAngle( m_nSeledPointIndex );
			Invalidate();
		}
	}
	CWnd::OnLButtonDblClk(nFlags,point);
}	

CRect CPtLineChart::DrawPointBox(CDC* pDC, int nX, int nY,BOOL bSeled)
{
	CRect rectPoint;
	rectPoint.left=nX-POINT_RECT_SIZE;
	rectPoint.right=nX+POINT_RECT_SIZE;
	rectPoint.top=nY-POINT_RECT_SIZE;
	rectPoint.bottom=nY+POINT_RECT_SIZE;
	CBrush brushGreen(RGB(0,255,0));
	CBrush brushRed(RGB(255,0,0));
	CBrush* pBrushOld=NULL;
	pBrushOld=pDC->SelectObject(bSeled?&brushRed:&brushGreen);
	pDC->Rectangle(rectPoint);
	pDC->SelectObject(pBrushOld);
	return rectPoint;
}


void CPtLineChart::calculateLengthAngle( int _idx )
{
	std::vector<double> vRealAltitude;
	double _dLength = 0.0;
	double _dAngle  = 0.0;	
	if(m_envMode == EnvMode_OnBoard)
	{
		std::vector<double> vRealAltitude = m_vVertCoorData ;
		(*m_pPath)[_idx].setZ( m_vPointYPos[m_nSeledPointIndex]);

		//calculate two point's distance and angle
		if(m_pPath)
		{
			double pi = 3.1415926535;

			double _dMaxAngle = 0.0;

			Point ptCur;
			int nPathCount = m_pPath->getCount();
			if(nPathCount > 1)
				ptCur = m_pPath->getPoint(0);
			for (int i = 1;  i < nPathCount;++i)
			{
				Point ptNext = m_pPath->getPoint(i);
				
				double dCurLength = ptCur.distance3D(ptNext);
				_dLength += dCurLength;
				

				//max angle
				double dVerticalLength = 0.0;
				if(ptCur.getZ() >= ptNext.getZ())
					dVerticalLength = ptCur.getZ()-ptNext.getZ();
				else
					dVerticalLength =ptNext.getZ()- ptCur.getZ();

				if( dCurLength != 0.0 )
				{
					// get angle by asin
					long double _sin_radians = asin( dVerticalLength / dCurLength );
					_sin_radians = _sin_radians * 180.0 / pi;

					if( abs( _sin_radians) > abs( _dMaxAngle ) )
						_dMaxAngle = _sin_radians;
				}


				ptCur = ptNext;
			}

			_dAngle =  _dMaxAngle;
		}

		//angle
		GetParent()->SendMessage( WM_UPDATE_VALUE, static_cast<WPARAM>(_dLength), static_cast<LPARAM>(_dAngle) );


	}
	else
	{
		CTermPlanDoc* _pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
		assert( _pDoc );

		_pDoc->GetCurModeFloorsRealAltitude(vRealAltitude);

		(*m_pPath)[_idx].setZ( m_vPointYPos[m_nSeledPointIndex] * SCALE_FACTOR );

		_dLength = m_pPath->getFactLength(vRealAltitude);
		_dAngle  = m_pPath->getMaxAngle(vRealAltitude);

		// set a message to parent window to display the two value
		GetParent()->SendMessage( WM_UPDATE_VALUE, static_cast<WPARAM>(_dLength*SCALE_FACTOR), static_cast<LPARAM>(_dAngle*SCALE_FACTOR) );
	}
	

}

int CPtLineChart::GetNearestYIndex(CPoint& ptTemp)
{

	int nBetween=-1;
	int nNearestYIndex=-1;
	double fPtValue=(m_nBottomestPos-ptTemp.y)/m_fPixelPerUnitV+m_vVertCoorData[0];
	
	for(int i=0;i<static_cast<int>(m_vVertCoorData.size())-1;i++)
	{
		if(fPtValue>=m_vVertCoorData[i]&&
			fPtValue<=m_vVertCoorData[i+1])
		{
			nBetween=i;
			break;
		}
	}
	if(nBetween==-1)
	{
		if(fPtValue<=m_vVertCoorData[0])
		{
			nNearestYIndex=0;
			ptTemp.y=m_nBottomestPos-static_cast<int>((m_vVertCoorData[nNearestYIndex]-m_vVertCoorData[0])/*fMin*/*m_fPixelPerUnitV);
		}
		else
		{
			nNearestYIndex=m_vVertCoorData.size()-1;
			ptTemp.y=m_nBottomestPos-static_cast<int>((m_vVertCoorData[nNearestYIndex]-m_vVertCoorData[0])/*fMin*/*m_fPixelPerUnitV);
		}
	}
	else
	{
		double fValue1=fabs(fPtValue-m_vVertCoorData[nBetween]);
		double fValue2=fabs(fPtValue-m_vVertCoorData[nBetween+1]);
		nNearestYIndex=fValue1<fValue2?nBetween:nBetween+1;
		ptTemp.y=m_nBottomestPos-static_cast<int>((m_vVertCoorData[nNearestYIndex]-m_vVertCoorData[0])/*fMin*/*m_fPixelPerUnitV);
	}
	return nNearestYIndex;
}

void CPtLineChart::UpdataChart( int nStartIndex, int nEndIndex )
{
	double dYStart = m_vPointYPos[nStartIndex - 1];
	double dYEnd = m_vPointYPos[nEndIndex - 1];
	double dXStart = m_vPointXPos[nStartIndex - 1];
	double dXEnd = m_vPointXPos[nEndIndex - 1];

	for(int i = nStartIndex; i < nEndIndex - 1; i++)
	{
		double dXi = m_vPointXPos[i];
		double dlong = dXEnd-dXStart;
		double drat;
		if (fabs(dlong)<(1.0e-30))
		{
			drat = 0; 
		}
		else
		{
			drat = (dXi-dXStart)/dlong;
		}
		double  dYi = dYStart*(1.0-drat)+dYEnd*drat;
		m_vPointYPos[i] = dYi;
	}
	Invalidate();
}
