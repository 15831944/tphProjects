#include "StdAfx.h"
#include ".\ptlinechartex.h"
#include "common\path2008.h"
#include <math.h>
#include "TermPlanDoc.h"

IMPLEMENT_DYNAMIC(CPtLineChartEx, CStatic)

CPtLineChartEx::CPtLineChartEx(void)
: m_nSeledPointIndex(-1)
, m_ptLastDraged(INT_MIN,INT_MIN)
, m_bDragging(false)
,m_bCursorMove(false)
,m_pPath( NULL )
, m_nNearestYIndex(-1)
, m_dZoomRate(1.0)
, m_bFirstInitControl(TRUE)
{
	m_vVertCoorData.push_back(-10.0);  //min height
	m_vVertCoorData.push_back(10.0);  //max Height

	m_ChartZeroPoint.SetPoint(0, 0);

	double ddd = 13.0/3.0;
}

CPtLineChartEx::~CPtLineChartEx(void)
{
	m_vVertCoorData.clear();
}

BEGIN_MESSAGE_MAP(CPtLineChartEx, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()




void CPtLineChartEx::OnPaint()
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

	if (m_bFirstInitControl)
	{
		GetParent()->SendMessage( WM_UPDATE_ZOOMVALUE, static_cast<WPARAM>(m_dZoomRate*100), static_cast<LPARAM>(m_dZoomRate*100) );
		m_bFirstInitControl = FALSE;
	}
}

// Draw Coordinate
void CPtLineChartEx::DrawCoordinate(CDC* pDC,CRect rectWnd,bool bHorizontal)
{

	CRect rectHori=CalcCoorRect(rectWnd,bHorizontal);
	CPoint ptBegin,ptEnd;
	ptEnd=ptBegin=rectHori.CenterPoint();
	if(bHorizontal)
	{
		ptBegin.x=rectHori.left;
		ptEnd.x=rectHori.right;

		m_ChartZeroPoint.x = ptBegin.x;
		m_ChartZeroPoint.y = ptBegin.y;
	}
	else
	{
		ptBegin.y=rectHori.top;
		ptEnd.y=rectHori.bottom;
	}
	pDC->MoveTo(ptBegin);
	pDC->LineTo(ptEnd);

	if (!bHorizontal)
	{
		DrawHeightRange(pDC, rectHori);
	}

	DrawArrow(pDC,rectHori,bHorizontal);

	DrawTick(pDC,rectHori,bHorizontal);
}

void CPtLineChartEx::DrawHeightRange(CDC* pDC, CRect rectCoor)
{
	ASSERT(NULL != pDC);

	CSize sizeTemp=pDC->GetTextExtent("0000000000000000.00");

	CPen penDash(PS_DOT,1,RGB(192,192,192));

	rectCoor.InflateRect(0,-rectCoor.Height()/20-sizeTemp.cy);
	m_nBottomestPos = rectCoor.bottom;
	m_nTopestPos    = rectCoor.top;

	int nCoorHeight = rectCoor.Height();
	int nCount      = m_vVertCoorData.size();

	CRect rectWnd;
	GetClientRect(rectWnd);
	double fMin=m_vVertCoorData[0] / m_dZoomRate;
	double fMax=m_vVertCoorData[nCount-1] / m_dZoomRate;

	m_fPixelPerUnitV = (double)nCoorHeight/(fMax-fMin);
	int nY;
	int nX0=rectCoor.right; 
	int nX1=nX0-ARROW_WIDTH_EX/2;

	for(int i=0;i<nCount;i++)
	{
		nY=m_nBottomestPos-static_cast<int>((m_vVertCoorData[i]/m_dZoomRate-fMin)*m_fPixelPerUnitV);

		CPen* pOldPen=pDC->SelectObject(&penDash);
		pDC->MoveTo(rectWnd.right,nY);
		pDC->LineTo(nX1,nY);
		pDC->SelectObject(pOldPen);

		pDC->MoveTo(nX0,nY);
		pDC->LineTo(nX1,nY);

		// Draw Tick Text
		CString strTick(_T(""));
		if (i==0)
		{
			strTick.Format("Min Height: %.2f",m_vVertCoorData[i]/m_dZoomRate);
		}
		else if (i==1)
		{
			strTick.Format("Max Height: %.2f",m_vVertCoorData[i]/m_dZoomRate);
		}
		else
		{
			ASSERT(FALSE);
		}		

		CSize sizeStrTick=sizeTemp;
		CRect rectTick;
		rectTick.left=nX0;
		rectTick.right=rectTick.left+sizeStrTick.cx;
		rectTick.bottom=nY+sizeStrTick.cy/2;
		rectTick.top=rectTick.bottom-sizeStrTick.cy;
		pDC->DrawText(strTick,rectTick,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	}	
}

// Set Data
void CPtLineChartEx::SetData(vector<double>& vPointsXPos, vector<double>& vPointsYPos, CPath2008* _pPath, bool bRefresh/* =FALSE */)
{
	// Clear Data
	m_vPointXPos.clear();
	m_vPointYPos.clear();

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

	m_pPath = _pPath;

	if(bRefresh)
		Invalidate();
}

CRect CPtLineChartEx::CalcCoorRect(CRect rectWnd,bool bHorizontal)
{
	// TRACE("ARROW_WIDTH_EX=%d\n",ARROW_WIDTH_EX);
	CRect rectRes=rectWnd;
	if(bHorizontal)
	{
		//rectRes.top=rectRes.bottom-ARROW_WIDTH_EX;
		rectRes.left+=(ARROW_WIDTH_EX)/2;
	}
	else
	{
		rectRes.right=rectRes.left+ARROW_WIDTH_EX;
		rectRes.bottom-=(ARROW_WIDTH_EX)/2;
	}
	return rectRes;
}

BOOL CPtLineChartEx::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

CRect CPtLineChartEx::CalcArrow(CRect rectCoor, bool bHorizontal)
{
	CRect rectRes=rectCoor;
	if(bHorizontal)
	{
		rectRes.left=rectRes	.right-ARROW_LAYER_COUNT_EX;
	}
	else
	{
		rectRes.bottom=rectRes.top+ARROW_LAYER_COUNT_EX;
	}
	return rectRes;
}

void CPtLineChartEx::DrawArrow(CDC* pDC, CRect rectCoor, bool bHorizontal)
{
	CRect rectArrow=CalcArrow(rectCoor,bHorizontal);
	CPoint ptBegin,ptEnd;
	CPoint ptCenter=rectArrow.CenterPoint();
	if(bHorizontal)
	{
		for(int i=0;i<ARROW_LAYER_COUNT_EX;i++)
		{
			ptBegin.x=ptEnd.x=rectArrow.right-i;
			ptBegin.y=ptCenter.y-i;
			ptEnd.y=ptCenter.y+i;
			pDC->MoveTo(ptBegin);
			pDC->LineTo(ptEnd);
		}	
	}
	else
	{
		for(int i=0;i<ARROW_LAYER_COUNT_EX;i++)
		{
			ptBegin.y=ptEnd.y=rectArrow.top+i;
			ptBegin.x=ptCenter.x-i;
			ptEnd.x=ptCenter.x+i;
			pDC->MoveTo(ptBegin);
			pDC->LineTo(ptEnd);
		}	
	}

}

void CPtLineChartEx::DrawTick(CDC* pDC,CRect rectCoor, bool bHorizontal)
{
	CSize sizeTemp=pDC->GetTextExtent("0000000000.00");

	CPen penDash(PS_DOT,1,RGB(192,192,192));
	if(bHorizontal)
	{
		rectCoor.InflateRect(-rectCoor.Width()/20-sizeTemp.cx,0);
		m_nLeftestPos=rectCoor.left;
		int nCoorWidth=rectCoor.Width();
		int nCount=m_vPointXPos.size();
		if(nCount>0)
		{
			double fMin = m_vPointXPos[0];
			double fMax = m_vPointXPos[nCount-1];
			m_fPixelPerUnitH = nCoorWidth/(fMax-fMin);

			int nX;
			int nY0 = rectCoor.bottom;
			int nY1 = nY0 + ARROW_WIDTH_EX/2;

			for(int i=0;i<nCount;i++)
			{
				nX = m_nLeftestPos+static_cast<int>((m_vPointXPos[i]-fMin)*m_fPixelPerUnitH);

				CPen* pOldPen=pDC->SelectObject(&penDash);
				pDC->MoveTo(nX,nY1);
				pDC->LineTo(nX,0);
				pDC->SelectObject(pOldPen);

				//pDC->MoveTo(nX,nY0);
				//pDC->LineTo(nX,nY1);

				// Draw Tick Text
				CString strTick;
				strTick.Format("%.2f",m_vPointXPos[i]);
				CSize sizeStrTick=pDC->GetTextExtent(strTick);
				CRect rectTick;
				rectTick.left=nX-sizeStrTick.cx/2;
				rectTick.right=rectTick.left+sizeStrTick.cx;
				rectTick.bottom=nY0;
				rectTick.top=rectTick.bottom-sizeStrTick.cy;
				pDC->DrawText(strTick,rectTick,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
				// Draw Point Box
				DrawPoint(pDC,nX,i);
			}	
			// Link points in line

			CPen penStretchLine(PS_SOLID,1,RGB(255,0,0));
			CPen* pOldPen=pDC->SelectObject(&penStretchLine);

			int nPointCount=m_vPointRect.size();
			CPoint* pPtTemp=new CPoint[nPointCount];
			for(int i=0;i<nPointCount;i++)
			{
				pPtTemp[i]=m_vPointRect[i].CenterPoint();
			}
			pDC->Polyline(pPtTemp,nPointCount);

			pDC->SelectObject(pOldPen);

			delete []pPtTemp;
		}
	}
}

void CPtLineChartEx::DrawLine(CDC* pDC, double dFromX, double dFromY, double dToX, double dToY)
{
	ASSERT(NULL != pDC);


}

void CPtLineChartEx::DrawPoint(CDC* pDC,int nX,int nIndex)
{
	//int nY=m_nBottomestPos-static_cast<int>((m_vPointYPos[nIndex]-m_vVertCoorData[0])/*fMin*/*m_fPixelPerUnitV);

	int nY = m_ChartZeroPoint.y - (int)(m_vPointYPos[nIndex] * m_fPixelPerUnitV);
	m_vPointRect[nIndex]=DrawPointBox(pDC,nX,nY,nIndex==m_nSeledPointIndex);

}

void CPtLineChartEx::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	Invalidate();
}

void CPtLineChartEx::OnLButtonDown(UINT nFlags, CPoint point)
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
	{
		Invalidate();
		UpdatePointDistanceAndHeight(m_nSeledPointIndex);
	}
		
	CWnd::OnLButtonDown(nFlags, point);
}

void CPtLineChartEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bCursorMove&&m_bDragging&&m_nSeledPointIndex!=-1)
	{
		if(nFlags&MK_CONTROL) // Snap to floor
		{
			CPoint ptTemp=point;
			int nNearestIndex=GetNearestYIndex(ptTemp);
			m_vPointYPos[m_nSeledPointIndex]=m_vVertCoorData[nNearestIndex]/m_dZoomRate;
		}
		else
		{
			CalcYPosition(m_ptLastDraged.y);
		}			

		UpdatePointDistanceAndHeight( m_nSeledPointIndex );
		Invalidate();
	}
	m_ptLastDraged=CPoint(INT_MIN,INT_MIN);
	m_bDragging=false;
	m_bCursorMove=false;
	CWnd::OnLButtonUp(nFlags, point);
}

void CPtLineChartEx::CalcYPosition(int nNewYPosition)
{
	m_vPointYPos[m_nSeledPointIndex]=((double)m_ChartZeroPoint.y - (double)nNewYPosition)/m_fPixelPerUnitV;
}

void CPtLineChartEx::OnMouseMove(UINT nFlags, CPoint point)
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

			CalcYPosition(point.y);
			UpdatePointDistanceAndHeight(m_nSeledPointIndex);
		}
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CPtLineChartEx::OnLButtonDblClk(UINT nFlags, CPoint point)
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
			UpdatePointDistanceAndHeight( m_nSeledPointIndex );
			Invalidate();
		}
	}
	CWnd::OnLButtonDblClk(nFlags,point);
}	

CRect CPtLineChartEx::DrawPointBox(CDC* pDC, int nX, int nY,BOOL bSeled)
{
	CRect rectPoint;
	rectPoint.left=nX-POINT_RECT_SIZE_EX;
	rectPoint.right=nX+POINT_RECT_SIZE_EX;
	rectPoint.top=nY-POINT_RECT_SIZE_EX;
	rectPoint.bottom=nY+POINT_RECT_SIZE_EX;
	CBrush brushGreen(RGB(0,255,0));
	CBrush brushRed(RGB(255,0,0));
	CBrush* pBrushOld=NULL;
	pBrushOld=pDC->SelectObject(bSeled?&brushRed:&brushGreen);
	pDC->Rectangle(rectPoint);
	pDC->SelectObject(pBrushOld);
	return rectPoint;
}

void CPtLineChartEx::UpdatePointDistanceAndHeight(int nIndex)
{
	// set a message to parent window to display the two value
	GetParent()->SendMessage( WM_UPDATE_VALUE_EX, static_cast<WPARAM>(m_vPointXPos[nIndex]*SCALE_FACTOR), static_cast<LPARAM>(m_vPointYPos[nIndex]*SCALE_FACTOR) );
}

int CPtLineChartEx::GetNearestYIndex(CPoint& ptTemp)
{

	int nBetween=-1;
	int nNearestYIndex=-1;
	double fPtValue=((double)m_nBottomestPos-(double)ptTemp.y)/m_fPixelPerUnitV+m_vVertCoorData[0];

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

void CPtLineChartEx::SetVerticalZoomRate(double dZoomRate)
{
	double dOldZoomRate = m_dZoomRate;
	m_dZoomRate = dZoomRate;

	for (int i=0; i<(int)m_vPointYPos.size(); i++)
	{
		if (!IsHeightInRange(m_vPointYPos[i]))
		{
			m_dZoomRate = dOldZoomRate;

			GetParent()->SendMessage( WM_UPDATE_ZOOMVALUE, static_cast<WPARAM>(m_dZoomRate*100), static_cast<LPARAM>(m_dZoomRate*100) );
		}
	}
}

double CPtLineChartEx::GetVerticalZoomRate(void)
{
	return m_dZoomRate;
}

int CPtLineChartEx::GetCurSelectIndex(void)
{
	return m_nSeledPointIndex;
}

void CPtLineChartEx::SetHeight(int nIndex, double dHeight)
{
	if (nIndex < 0
		|| nIndex >= (int)m_vPointYPos.size())
	{
		return;
	}

	if (!IsHeightInRange(dHeight))
	{
		AfxMessageBox(_T("Height is out of range."), MB_OK|MB_ICONWARNING);
		return;
	}

	m_vPointYPos[nIndex] = dHeight;
}

BOOL CPtLineChartEx::IsHeightInRange(double dHeight)
{
	if (m_vVertCoorData[0]/m_dZoomRate - dHeight <= 0.000000001
		&& dHeight - m_vVertCoorData[1]/m_dZoomRate <= 0.000000001)
	{
		return TRUE;
	}

	return FALSE;	
}