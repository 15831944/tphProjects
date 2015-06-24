// DlgOnboardDeckAdjust.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgOnboardDeckAdjust.h"
#include "InputOnBoard/Deck.h"
#include "../common/UnitsManager.h"

#include "common\FloatUtils.h"
#include <limits>

#include "Onboard/AircraftLayout3DView.h"

static const int HEIGHT_PIXELS = 200;
static const int FLOORBARTHICKNESS = 14;
static const int HALFFLOORBARTHICKNESS = 7;
static const int NUMBEROFTICKS = 10;


static const LOGFONT lfHeightText = {
	12, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, "Arial"
};

static const LOGFONT lfFloorText = {
	14, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, "Arial"
};

static const COLORREF floorcolors[] = {
	RGB(20,20,90),
		RGB(90,20,20),
		RGB(20,90,20),
		RGB(90,20,90),
};

static const COLORREF floorcolorsOpaque[] = {
	RGB(20,20,90),
		RGB(90,20,20),
		RGB(20,90,20),
		RGB(90,20,90),
};

// CDlgOnboardDeckAdjust dialog

IMPLEMENT_DYNAMIC(CDlgOnboardDeckAdjust, CDialog)
CDlgOnboardDeckAdjust::CDlgOnboardDeckAdjust(CDeckManager *pDeckManager, CView* pParent /*=NULL*/)
	: CDialog(CDlgOnboardDeckAdjust::IDD, pParent)
	, m_rcClient(0,0,1,1)
	,m_pDeckManager(pDeckManager)
{
	m_nMaxH = 10000; //100 m

	m_nUnitsPerPixel = m_nMaxH / HEIGHT_PIXELS; //50 units per pixel
	m_nSelectedFloor = -1;
	m_ToolTip.Create(this);
	m_bMouseOver=FALSE;
	m_bMovingFloor=FALSE;
	nHStep = m_nMaxH/NUMBEROFTICKS;

}

CDlgOnboardDeckAdjust::~CDlgOnboardDeckAdjust()
{
}

void CDlgOnboardDeckAdjust::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_FAZOOMOUT, m_btnZoomOut);
	DDX_Control(pDX, IDC_BUTTON_FAZOOMIN, m_btnZoomIn);

}


BEGIN_MESSAGE_MAP(CDlgOnboardDeckAdjust, CDialog)
	//{{AFX_MSG_MAP(CDlgFloorAdjust)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_FAZOOMIN, OnButtonZoomIn)
	ON_BN_CLICKED(IDC_BUTTON_FAZOOMOUT, OnButtonZoomOut)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE (WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE (WM_MOUSEHOVER, OnMouseHover)
END_MESSAGE_MAP()


// CDlgOnboardDeckAdjust message handlers

int CDlgOnboardDeckAdjust::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here

	return 0;
}
void CDlgOnboardDeckAdjust::RequestHoverMsg()
{
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof (tme);
	tme.dwFlags = TME_HOVER | TME_LEAVE;
	tme.hwndTrack = GetSafeHwnd();
	tme.dwHoverTime = 0;
	VERIFY(_TrackMouseEvent(&tme));
}

BOOL CDlgOnboardDeckAdjust::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_btnZoomIn.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ZOOMIN));
	m_btnZoomOut.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ZOOMOUT));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgOnboardDeckAdjust::UnitsToPixels(double dInUnits)
{
	//0 is m_rcClient.bottom - FLOORBARTHICKNESS
	return static_cast<int>( (m_rcClient.bottom - FLOORBARTHICKNESS) - static_cast<LONG>(RoundDouble(dInUnits/m_nUnitsPerPixel, 0)) );
}

void CDlgOnboardDeckAdjust::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if(0==m_nUnitsPerPixel)
		return;


	const int c_nMinStep = 10; // add by Benny, to avoid endless loop
	int nPixelStep = nHStep / m_nUnitsPerPixel;
	int nUsedStep = nHStep;
	if (nPixelStep < c_nMinStep)
	{
		nPixelStep = c_nMinStep;
		nUsedStep = c_nMinStep*m_nUnitsPerPixel;
	}


	CPen penGrey(PS_SOLID,1,RGB(128,128,128));

	dc.FillSolidRect(m_rcClient, RGB(255,255,255));

	dc.SetTextColor(RGB(128,128,128));
	CPen* pOldPen = dc.SelectObject(&penGrey);

	CFont fHeightText; 
	fHeightText.CreateFontIndirect(&lfHeightText);
	CFont* pOldFont = dc.SelectObject(&fHeightText);

	int c=0;
	for(int p=FLOORBARTHICKNESS; p<=HEIGHT_PIXELS+FLOORBARTHICKNESS; p+=nPixelStep)
	{
		dc.MoveTo(m_rcClient.left, m_rcClient.bottom-p);
		dc.LineTo(m_rcClient.right, m_rcClient.bottom-p);

		CRect rcText(m_rcClient.left,m_rcClient.bottom-p,m_rcClient.right,m_rcClient.bottom-p);
		CString s;
		s.Format("%.0f %s", RoundDouble(UNITSMANAGER->ConvertLength((double)c*nUsedStep),0), UNITSMANAGER->GetLengthUnitString());
		dc.DrawText(s,&rcText,DT_CENTER|DT_VCENTER|DT_NOCLIP|DT_SINGLELINE );
		c++;
	}

	CFont fFloorText; 
	fFloorText.CreateFontIndirect(&lfFloorText);
	dc.SelectObject(&fFloorText);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,255,255));

	int nDeckCount = m_pDeckManager->getCount();
	CString s;
	int nDeckPixelHeight;

	for(int i=0; i<nDeckCount; i++) 
	{
		CDeck* pDeck = m_pDeckManager->getItem(i);
		if(pDeck == NULL)
			continue;

		nDeckPixelHeight = UnitsToPixels(pDeck->RealAltitude());
		CRect rcFloor(m_rcClient.left, nDeckPixelHeight, m_rcClient.right, nDeckPixelHeight);
		rcFloor.InflateRect(0,HALFFLOORBARTHICKNESS,0,HALFFLOORBARTHICKNESS);
		dc.FillSolidRect(rcFloor, floorcolors[i]);

		s.Format("%d", i+1);
		dc.DrawText(s, rcFloor, DT_CENTER|DT_VCENTER|DT_NOCLIP|DT_SINGLELINE);
	}

	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldFont);

	// Do not call CDialog::OnPaint() for painting messages
}

void CDlgOnboardDeckAdjust::GetBarRect(LPRECT rc)
{
	GetClientRect(rc);
	CWnd* pBtn = GetDlgItem(IDC_BUTTON_FAZOOMIN);
	CRect rcBtn;
	pBtn->GetWindowRect(&rcBtn);
	rc->top+=rcBtn.Height();
}

int CDlgOnboardDeckAdjust::HitTest(CPoint pt)
{

	int nDeckCount = m_pDeckManager->getCount();
	int i;
	for( i=0; i<nDeckCount; i++)
	{
		CDeck* pDeck = m_pDeckManager->getItem(i);
		if(pDeck == NULL)
			continue;

		int nFloorPixelHeight = UnitsToPixels(pDeck->RealAltitude());
		CRect rcFloor(m_rcClient.left, nFloorPixelHeight, m_rcClient.right, nFloorPixelHeight);
		rcFloor.InflateRect(0,HALFFLOORBARTHICKNESS,0,HALFFLOORBARTHICKNESS);
		if(rcFloor.PtInRect(pt))
			return i;
	}
	return -1;
}

void CDlgOnboardDeckAdjust::UpdateLayoutView()
{
	//C3DView* pView = NULL;
	//if(m_pDoc && (pView = m_pDoc->Get3DView())) 
	//{
	//	pView->UseSharpTexture(FALSE);
	//	pView->Invalidate(FALSE);
	//	if(!m_bMovingFloor && pView->IsAutoSharp())
	//		pView->GenerateSharpTexture();
	//}
}

void CDlgOnboardDeckAdjust::OnLButtonDblClk(UINT nFlags, CPoint point) 
{

	//int nDblClickFloor = HitTest(point);
	//if(nDblClickFloor != -1) 
	//{
	//	EnvironmentMode szmode;
	//	int absfloorindex;
	//	szmode = m_pDoc->GetModeByFloorsNumber(nDblClickFloor);
	//	absfloorindex = m_pDoc->GetAbsFloorIndexByMode(nDblClickFloor, szmode);

	//	std::vector<CFloor2*>& floors = m_pDoc->GetFloorByMode(szmode).m_vFloors;

	//	floors[absfloorindex]->IsOpaque(!floors[absfloorindex]->IsOpaque());

	//	Invalidate();

	//	UpdateLayoutView();

	//	m_pDoc->GetCurModeFloor().saveDataSet(m_pDoc->m_ProjInfo.path, FALSE);
	//}


	CDialog::OnLButtonDblClk(nFlags, point);
}

void CDlgOnboardDeckAdjust::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
	m_nSelectedFloor = HitTest(point);
#ifdef _DEBUG
	if(m_nSelectedFloor!=-1)
		// TRACE("Selected floor %d\n", m_nSelectedFloor);
#endif
		m_ptLast = point;
	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgOnboardDeckAdjust::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	m_nSelectedFloor=-1;

	if(m_bMovingFloor) 
	{
		m_bMovingFloor=FALSE;
		//save data
		m_pDeckManager->WriteDeckToDataBase();
	}

	UpdateLayoutView();
	Invalidate();
	CDialog::OnLButtonUp(nFlags, point);
}

void CDlgOnboardDeckAdjust::OnMouseMove(UINT nFlags, CPoint point) 
{
	CPoint ptDiff = point-m_ptLast;

	if (!m_bMouseOver) 
	{
		m_bMouseOver = TRUE;
		RequestHoverMsg();
	}

	if(m_nSelectedFloor>= 0 && m_pDeckManager && m_nSelectedFloor< m_pDeckManager->getCount()) 
	{
		m_ToolTip.Close();

		CRect rcInvalidate;
		int absfloorindex = m_nSelectedFloor;
		CDeck *pDeck = m_pDeckManager->getItem(m_nSelectedFloor);
		if(pDeck)
		{
			int nFloorPixelHeight = UnitsToPixels(pDeck->RealAltitude());
			CRect rcFloor(m_rcClient.left, nFloorPixelHeight - HALFFLOORBARTHICKNESS,
				m_rcClient.right, nFloorPixelHeight + HALFFLOORBARTHICKNESS);

			MoveDeck(-ptDiff.y*m_nUnitsPerPixel);

			if (ptDiff.y>0)
				rcFloor.bottom += ptDiff.y;
			else
				rcFloor.top += ptDiff.y;
			InvalidateRect(rcFloor);

			m_bMovingFloor=TRUE;

			UpdateLayoutView();
		}
	}
	m_ptLast = point;
	CDialog::OnMouseMove(nFlags, point);
}

void CDlgOnboardDeckAdjust::MoveDeck(double delta)
{

	int absfloorindex;

	absfloorindex = m_nSelectedFloor;

	int nDeckCount = m_pDeckManager->getCount();
	CDeck *pDeck = m_pDeckManager->getItem(m_nSelectedFloor);
	if(pDeck == NULL)
		return;

	
	double dAlt = pDeck->RealAltitude();
	double dNewAlt = dAlt+delta;
	double dBuffer = FLOORBARTHICKNESS*m_nUnitsPerPixel;
	if(dNewAlt < 0.0)
		dNewAlt = 0.0;
	if(absfloorindex < nDeckCount-1) 
	{ //there is a floor above
		CDeck *pAboveDeck = m_pDeckManager->getItem(absfloorindex + 1);
		if(pAboveDeck != NULL)
		{
			double dNextAlt = pAboveDeck->RealAltitude()-dBuffer;
			if(dNewAlt > dNextAlt)
				dNewAlt = dNextAlt;
		}
	}
	if(absfloorindex > 0)
	{ //there is a floor below
		CDeck *pBelowDeck = m_pDeckManager->getItem(absfloorindex - 1);
		if(pBelowDeck != NULL)
		{
			double dPrevAlt = pBelowDeck->RealAltitude()+dBuffer;
			if(dNewAlt < dPrevAlt)
				dNewAlt = dPrevAlt;
		}
	}

	pDeck->RealAltitude( dNewAlt );
	pDeck->Altitude(dNewAlt);

	CAircraftLayout3DView *pLayoutView = GetLayoutView();
	if (pLayoutView)
		pLayoutView->UpdateFloorAltitude(pDeck);
	


}


void CDlgOnboardDeckAdjust::ShowToolTip(LPCTSTR string)
{
	m_ToolTip.Close();

	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	point.Offset(5,-10);
	m_ToolTip.SetText(string);	
	m_ToolTip.Show(point);

}

LRESULT CDlgOnboardDeckAdjust::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bMouseOver = FALSE;
	m_ToolTip.Close();

	return 0;
}


LRESULT CDlgOnboardDeckAdjust::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	RequestHoverMsg();
	//
	// Get the nearest item in the list box to the mouse cursor
	//
	CPoint point;
	point.x = LOWORD (lParam);
	point.y	= HIWORD (lParam);
	BOOL	bOutSide = FALSE;

	int nFloorUnderCursor = HitTest(point);

	// Display the tool tip text only if the previous and current
	// item are different.
	if(nFloorUnderCursor >= 0)	
	{
		CString strTip = _T("Tooltip");

		int absfloorindex = nFloorUnderCursor;
		//if(m_pDoc && m_pDoc->m_systemMode == EnvMode_AirSide)
		//	strTip.Format("%s\nAltitude %.2f %s",\
		//	floors[absfloorindex]->FloorName(),\
		//	CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),floors[absfloorindex]->Altitude()),\
		//	CARCLengthUnit::GetLengthUnitString(m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit()));
		//else
		CDeck *pDeck = m_pDeckManager->getItem(absfloorindex);
		if(pDeck == NULL)
			return 0;

		strTip.Format("%s\nAltitude %.2f %s",\
		pDeck->GetName(),\
		UNITSMANAGER->ConvertLength(pDeck->RealAltitude()),\
		UNITSMANAGER->GetLengthUnitString());

		ShowToolTip(strTip);
	}
	else 
	{
		m_ToolTip.Close();
	}

	return 0;
}

void CDlgOnboardDeckAdjust::OnButtonZoomIn() 
{
	if (m_nMaxH>1) // avoid integer overflow
	{
		m_nMaxH /= 2;
		m_nUnitsPerPixel = m_nMaxH / HEIGHT_PIXELS;
	}
	Invalidate();
}

void CDlgOnboardDeckAdjust::OnButtonZoomOut() 
{
	if (m_nMaxH < (std::numeric_limits<int>::max)()/2) // avoid integer overflow
	{
		m_nMaxH *= 2;
		m_nUnitsPerPixel = m_nMaxH / HEIGHT_PIXELS;
	}
	Invalidate();
}

void CDlgOnboardDeckAdjust::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if(GetSafeHwnd())
		GetBarRect(&m_rcClient);
}

CAircraftLayout3DView * CDlgOnboardDeckAdjust::GetLayoutView() const
{
	CView *pView = (CView *)m_pParentWnd;
	if(pView && pView->IsKindOf(RUNTIME_CLASS(CAircraftLayout3DView)))
		return (CAircraftLayout3DView *)pView;

	return NULL;
}
























































