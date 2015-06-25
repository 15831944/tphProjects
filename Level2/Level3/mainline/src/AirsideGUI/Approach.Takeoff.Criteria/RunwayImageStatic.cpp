// RunwayImageStatic.cpp : implementation file
//

#include "stdafx.h"
#include "RunwayImageStatic.h"
#include "../../InputAirside/InputAirside.h"
#include "../../InputAirside/ALTAirport.h"
#include "../../InputAirside/runway.h"
#include ".\runwayimagestatic.h"

// CRunwayImageStatic

IMPLEMENT_DYNAMIC(CRunwayImageStatic, CStatic)
CRunwayImageStatic::CRunwayImageStatic(int nProjID):m_nProjID(nProjID)
{
	//GetRunway();
}

CRunwayImageStatic::~CRunwayImageStatic()
{
}

BEGIN_MESSAGE_MAP(CRunwayImageStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CRunwayImageStatic::GetRunway(int nAirportID, const ALTObjectList& vRunways)
{
	m_xMin = 1.0;
	m_xMax = 1.0;
	m_yMin = 1.0;
	m_yMax = 1.0;
	m_xOrg = 1.0;
	m_yOrg = 1.0;

	StaticRunwayLine structNewLine;
	//std::vector<int> vAirportIds;
	//InputAirside::GetAirportList(m_nProjID, vAirportIds);
	//for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	//{
		ALTAirport airport;
		airport.ReadAirport(nAirportID);

		m_xOrg = airport.getRefPoint().getX();
		m_yOrg = airport.getRefPoint().getY();
		//std::vector<int> vrRunwayIDs;
		//vrRunwayIDs.clear();
		//ALTAirport::GetRunwaysIDs(*iterAirportID,vrRunwayIDs);
		size_t nCount = vRunways.size();
		//for (std::vector<int>::iterator itrRunwayID = vrRunwayIDs.begin();itrRunwayID != vrRunwayIDs.end();++itrRunwayID) 
		//{
		//	Runway runway;
		//	runway.ReadObject(*itrRunwayID);
		for (size_t i = 0; i < nCount; i++)
		{
			Runway *pRunway =(Runway*) vRunways.at(i).get();

			structNewLine.pt1 = pRunway->GetPath().getPoint(0);
			structNewLine.strName1 = pRunway->GetMarking1().c_str();
			structNewLine.pt2 = pRunway->GetPath().getPoint(pRunway->GetPath().getCount() - 1);
			structNewLine.strName2 = pRunway->GetMarking2().c_str();

			m_vrRunwayLines.push_back(structNewLine);

			//if (vrRunwayIDs.begin() == itrRunwayID) 
			if (i == 0)
			{
				m_xMin = m_xMax = structNewLine.pt1.getX();
				m_yMin = m_yMax = structNewLine.pt1.getY();
			}

			if (structNewLine.pt1.getX() < m_xMin)
				m_xMin = structNewLine.pt1.getX();
			if (structNewLine.pt2.getX() < m_xMin) 
				m_xMin = structNewLine.pt2.getX();

			if (structNewLine.pt1.getX() > m_xMax)
				m_xMax = structNewLine.pt1.getX();
			if (structNewLine.pt2.getX() > m_xMax)
				m_xMax = structNewLine.pt2.getX();

			if (structNewLine.pt1.getY() < m_yMin)
				m_yMin = structNewLine.pt1.getY();
			if (structNewLine.pt2.getY() < m_yMin) 
				m_yMin = structNewLine.pt2.getY();

			if (structNewLine.pt1.getY() > m_yMax)
				m_yMax = structNewLine.pt1.getY();
			if (structNewLine.pt2.getY() > m_yMax)
				m_yMax = structNewLine.pt2.getY();
		}
	//}
}
void CRunwayImageStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectDraw;
	CRect rectComDC;
	GetClientRect(&rectDraw);
	dc.FillSolidRect(rectDraw,RGB(255,255,255));
	if (m_vrRunwayLines.empty())
		return;

	HDC hDC = CreateCompatibleDC(dc.m_hDC);
	CDC * pDC = CDC::FromHandle(hDC);
	if(!pDC)return;

	HBITMAP hMemBM = CreateCompatibleBitmap (dc.m_hDC, rectDraw.Width(), rectDraw.Height());
	SelectObject(hDC, hMemBM);

	pDC->IntersectClipRect(&rectDraw);
	pDC->FillSolidRect(rectDraw,RGB(255,255,255));

	pDC->SetMapMode(MM_LOMETRIC);
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(int(2.5*MAX(fabs(m_xMax),fabs(m_xMin))),-int(2.5*MAX(fabs(m_yMax),fabs(m_yMin))));
	pDC->SetWindowOrg(int(m_xOrg),int(m_yOrg));
	pDC->SetViewportExt(rectDraw.Width(),rectDraw.Height());
	pDC->SetViewportOrg(rectDraw.left + rectDraw.Width()/2,rectDraw.top + rectDraw.Height()/2);

	CString strTitle = _T("Runways");
	CSize sizeText = pDC->GetTextExtent(strTitle);
	pDC->TextOut(-sizeText.cx/2,int(0.5*2.5*MAX(fabs(m_yMax),fabs(m_yMin))),strTitle);
	CPen pen;
	pen.CreatePen(PS_SOLID,8,RGB(0,0,255));
	pDC->SelectObject(&pen); 
	pDC->SetTextColor(RGB(255,0,0));
	for (std::vector <StaticRunwayLine>::iterator itrLine = m_vrRunwayLines.begin();itrLine != m_vrRunwayLines.end();++itrLine) {	
		pDC->MoveTo((int)((*itrLine).pt1.getX()),(int)((*itrLine).pt1.getY()));
		pDC->LineTo((int)((*itrLine).pt2.getX()),(int)((*itrLine).pt2.getY()));

		pDC->TextOut((int)((*itrLine).pt1.getX()),(int)((*itrLine).pt1.getY()),(*itrLine).strName1);
		pDC->TextOut((int)((*itrLine).pt2.getX()),(int)((*itrLine).pt2.getY()),(*itrLine).strName2);
	}

	pDC->GetClipBox(&rectComDC);
	dc.StretchBlt(rectDraw.left,rectDraw.top,rectDraw.Width(),rectDraw.Height(),pDC,rectComDC.left,rectComDC.top,rectComDC.Width(),rectComDC.Height(),SRCCOPY);

	SelectObject(hDC,(HBITMAP)0);
	DeleteObject(hMemBM);
	pDC->Detach();
	DeleteDC(hDC);	
}