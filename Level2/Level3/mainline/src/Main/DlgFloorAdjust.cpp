// DlgFloorAdjust.cpp : implementation file
//

#include "stdafx.h"
#include "DlgFloorAdjust.h"
#include "TermPlanDoc.h"
#include "ViewMsg.h"
#include "3DView.h"
#include "Floor2.h"
#include "../common/UnitsManager.h"
#include "common\FloatUtils.h"
#include "FloorAdjustmentData.h"
#include <InputAirside\ARCUnitManager.h>
#include <common\ViewMsgInfo.h>

#include <limits>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


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

/////////////////////////////////////////////////////////////////////////////
// CDlgFloorAdjust dialog


CDlgFloorAdjust::CDlgFloorAdjust(CTermPlanDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFloorAdjust::IDD, pParent), m_rcClient(0,0,1,1)
{
	//{{AFX_DATA_INIT(CDlgFloorAdjust)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	VERIFY(m_pDoc=pDoc);
	if (m_pDoc->m_systemMode == EnvMode_AirSide)
	{
		m_nMaxH = 1000;   // X10
	}
	else
	{
		m_nMaxH = 10000; //100 m
	}
	m_nUnitsPerPixel = m_nMaxH / HEIGHT_PIXELS; //50 units per pixel
	m_nSelectedFloor = -1;
	m_bMouseOver=FALSE;
	m_bMovingFloor=FALSE;
	nHStep = m_nMaxH/NUMBEROFTICKS;
	m_scale = pDoc->m_iScale*nHStep;
}


void CDlgFloorAdjust::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFloorAdjust)
	DDX_Control(pDX, IDC_BUTTON_FAZOOMOUT, m_btnZoomOut);
	DDX_Control(pDX, IDC_BUTTON_FAZOOMIN, m_btnZoomIn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFloorAdjust, CDialog)
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

/////////////////////////////////////////////////////////////////////////////
// CDlgFloorAdjust message handlers

int CDlgFloorAdjust::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_ToolTip.Create(this);

	return 0;
}

void CDlgFloorAdjust::RequestHoverMsg()
{
	TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof (tme);
    tme.dwFlags = TME_HOVER | TME_LEAVE;
    tme.hwndTrack = GetSafeHwnd();
	tme.dwHoverTime = 0;
    VERIFY(_TrackMouseEvent(&tme));
}

BOOL CDlgFloorAdjust::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_btnZoomIn.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ZOOMIN));
	m_btnZoomOut.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ZOOMOUT));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgFloorAdjust::UnitsToPixels(double dInUnits)
{
	//0 is m_rcClient.bottom - FLOORBARTHICKNESS
	return static_cast<int>( (m_rcClient.bottom - FLOORBARTHICKNESS) - static_cast<LONG>(RoundDouble(dInUnits/m_nUnitsPerPixel, 0)) );
}

void CDlgFloorAdjust::OnPaint() 
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
	for(int p=FLOORBARTHICKNESS; p<=HEIGHT_PIXELS+FLOORBARTHICKNESS; p+=nPixelStep) {
		dc.MoveTo(m_rcClient.left, m_rcClient.bottom-p);
		dc.LineTo(m_rcClient.right, m_rcClient.bottom-p);

		CRect rcText(m_rcClient.left,m_rcClient.bottom-p,m_rcClient.right,m_rcClient.bottom-p);
		CString s;
		if(m_pDoc )
		{
			if (m_pDoc->m_systemMode == EnvMode_AirSide || m_pDoc->m_systemMode == EnvMode_LandSide)
			{
				s.Format("%s %.0f",  "X",RoundDouble(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),(double)c*nUsedStep),0));
			}			
			else
			{
				s.Format("%.0f %s", RoundDouble(UNITSMANAGER->ConvertLength((double)c*nUsedStep),0), UNITSMANAGER->GetLengthUnitString());
			}
		}
		dc.DrawText(s,&rcText,DT_CENTER|DT_VCENTER|DT_NOCLIP|DT_SINGLELINE );		
		c++;
	}

	CFont fFloorText; 
    fFloorText.CreateFontIndirect(&lfFloorText);
	dc.SelectObject(&fFloorText);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,255,255));

	CFloors& floors =m_pDoc->GetCurModeFloor(); //m_pDoc->GetFloorByMode(EnvMode_Terminal);
	int nFloorCount = floors.m_vFloors.size();
	CString s;
	int nFloorPixelHeight;

	if (m_pDoc->m_systemMode == EnvMode_AirSide)
	{
		int PixelHeight = UnitsToPixels(m_scale);
		CRect rcScale(m_rcClient.left,PixelHeight,m_rcClient.right,PixelHeight);
		rcScale.InflateRect(0,HALFFLOORBARTHICKNESS,0,HALFFLOORBARTHICKNESS);
		dc.FillSolidRect(rcScale,RGB(0,255,0));
		s.Format("Scale");
		dc.DrawText(s,rcScale,DT_CENTER|DT_VCENTER|DT_NOCLIP|DT_SINGLELINE);
	}
	else
	{
		for(int i=0; i<nFloorCount; i++) 
		{
			nFloorPixelHeight = UnitsToPixels(floors.m_vFloors[i]->Altitude());
			CRect rcFloor(m_rcClient.left, nFloorPixelHeight, m_rcClient.right, nFloorPixelHeight);
			rcFloor.InflateRect(0,HALFFLOORBARTHICKNESS,0,HALFFLOORBARTHICKNESS);
			if(floors.m_vFloors[i]->IsOpaque()) 
				dc.FillSolidRect(rcFloor, RGB(64,64,64));	
			else
				dc.FillSolidRect(rcFloor, floorcolors[i]);
			s.Format("%d", i+1);
			dc.DrawText(s, rcFloor, DT_CENTER|DT_VCENTER|DT_NOCLIP|DT_SINGLELINE);
		}

	
		{
			// draw airside floor mark.
			/*CFloors& floors2 = m_pDoc->GetFloorByMode(EnvMode_AirSide);
			nFloorCount = floors2.m_vFloors.size();
			nFloorPixelHeight = UnitsToPixels(floors2.m_vFloors[0]->Altitude());
			CRect rcFloor(m_rcClient.left, nFloorPixelHeight, m_rcClient.right, nFloorPixelHeight);
			rcFloor.InflateRect(0,HALFFLOORBARTHICKNESS,0,HALFFLOORBARTHICKNESS);
			if(floors2.m_vFloors[0]->IsOpaque()) 
				dc.FillSolidRect(rcFloor, RGB(0,255,0));	
			else
				dc.FillSolidRect(rcFloor, RGB(0,255,0));
			s.Format("Ground");
			dc.DrawText(s, rcFloor, DT_CENTER|DT_VCENTER|DT_NOCLIP|DT_SINGLELINE);*/
		}
	}


	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldFont);

	// Do not call CDialog::OnPaint() for painting messages
}

void CDlgFloorAdjust::GetBarRect(LPRECT rc)
{
	GetClientRect(rc);
	CWnd* pBtn = GetDlgItem(IDC_BUTTON_FAZOOMIN);
	CRect rcBtn;
	pBtn->GetWindowRect(&rcBtn);
	rc->top+=rcBtn.Height();
}

int CDlgFloorAdjust::HitTest(CPoint pt)
{
	if (m_pDoc->m_systemMode == EnvMode_AirSide)
	{
		return 1;
	}
	else
	{
		CFloors& floors = m_pDoc->GetCurModeFloor();
		int nFloorCount = floors.m_vFloors.size();
		
		for(int i=0; i<nFloorCount; i++)
		{
			int nFloorPixelHeight = UnitsToPixels(floors.m_vFloors[i]->Altitude());
			CRect rcFloor(m_rcClient.left, nFloorPixelHeight, m_rcClient.right, nFloorPixelHeight);
			rcFloor.InflateRect(0,HALFFLOORBARTHICKNESS,0,HALFFLOORBARTHICKNESS);
			if(rcFloor.PtInRect(pt))
				return i;
		}
		
		//floor
		/*CFloors& airsidefloors = m_pDoc->GetFloorByMode(EnvMode_AirSide);
		nFloorCount = airsidefloors.m_vFloors.size();
		for( i=0; i<nFloorCount; i++)
		{
			int nFloorPixelHeight = UnitsToPixels(airsidefloors.m_vFloors[i]->Altitude());
			CRect rcFloor(m_rcClient.left, nFloorPixelHeight, m_rcClient.right, nFloorPixelHeight);
			rcFloor.InflateRect(0,HALFFLOORBARTHICKNESS,0,HALFFLOORBARTHICKNESS);
			if(rcFloor.PtInRect(pt))
				return m_pDoc->GetDrawFloorsNumberByMode(EnvMode_AirSide)+i;
		}*/
	}


	return -1;
}

void CDlgFloorAdjust::Update3DView()
{
	C3DView* pView = NULL;
	if(m_pDoc && (pView = m_pDoc->Get3DView())) {
		pView->UseSharpTexture(FALSE);
		pView->Invalidate(FALSE);
		if(!m_bMovingFloor && pView->IsAutoSharp())
			pView->GenerateSharpTexture();
	}
}

void CDlgFloorAdjust::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (m_pDoc->m_systemMode == EnvMode_AirSide)
		return;

	int nDblClickFloor = HitTest(point);
	if(nDblClickFloor != -1) 
	{	
		CFloors& floors = m_pDoc->GetCurModeFloor();
		int nFloorCount = floors.m_vFloors.size();
		if( CRenderFloor*  pFloor = floors.GetFloor(nDblClickFloor) )
		{
			pFloor->IsOpaque( !pFloor->IsOpaque() );
			Invalidate();
			Update3DView();
			m_pDoc->GetCurModeFloor().saveDataSet(m_pDoc->m_ProjInfo.path, FALSE);
		}

		/*EnvironmentMode szmode;
		int absfloorindex;
		szmode = m_pDoc->GetModeByFloorsNumber(nDblClickFloor);
		absfloorindex = m_pDoc->GetAbsFloorIndexByMode(nDblClickFloor, szmode);

		std::vector<CFloor2*>& floors = m_pDoc->GetFloorByMode(szmode).m_vFloors;*/

	//	floors[absfloorindex]->IsOpaque(!floors[absfloorindex]->IsOpaque());

		
	}

	
	CDialog::OnLButtonDblClk(nFlags, point);
}

void CDlgFloorAdjust::OnLButtonDown(UINT nFlags, CPoint point) 
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

void CDlgFloorAdjust::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	if(m_nSelectedFloor!=-1)
	{
		m_nSelectedFloor=-1;
		if (m_pDoc->m_systemMode == EnvMode_AirSide)
		{
			OnEndModifyScale();
		}
		else
		{
			if(m_bMovingFloor) {
				m_bMovingFloor=FALSE;
				m_pDoc->GetCurModeFloor().saveDataSet(m_pDoc->m_ProjInfo.path, FALSE);
			}
		}
		Update3DView();
		Invalidate();
	}

	
	CDialog::OnLButtonUp(nFlags, point);
}

void CDlgFloorAdjust::OnMouseMove(UINT nFlags, CPoint point) 
{
	CPoint ptDiff = point-m_ptLast;

	if (!m_bMouseOver) {
		m_bMouseOver = TRUE;
		RequestHoverMsg();
	}

	if(m_nSelectedFloor!=-1) 
	{
		m_ToolTip.Close();

		CRect rcInvalidate;

		if (m_pDoc->m_systemMode == EnvMode_AirSide)
		{
			int PixelHeight = UnitsToPixels(m_scale);
			CRect rcScale(m_rcClient.left, PixelHeight, m_rcClient.right, PixelHeight);
			rcScale.InflateRect(0,HALFFLOORBARTHICKNESS,0,HALFFLOORBARTHICKNESS);

			ModifyScale(-ptDiff.y*m_nUnitsPerPixel);

			InvalidateRect(rcScale);
			rcScale.OffsetRect(0,-ptDiff.y);
			InvalidateRect(rcScale);

		}
		else
		{
			CFloors& floors = m_pDoc->GetCurModeFloor();
			
			if( CRenderFloor*  pFloor = floors.GetFloor(m_nSelectedFloor) )
			{
				int nFloorPixelHeight = UnitsToPixels(pFloor->Altitude());
				CRect rcFloor(m_rcClient.left, nFloorPixelHeight - HALFFLOORBARTHICKNESS,
					m_rcClient.right, nFloorPixelHeight + HALFFLOORBARTHICKNESS);

				MoveFloor(-ptDiff.y*m_nUnitsPerPixel);
				if (ptDiff.y>0)
					rcFloor.bottom += ptDiff.y;
				else
					rcFloor.top += ptDiff.y;
				InvalidateRect(rcFloor);
			}

			/*EnvironmentMode szmode;
			int absfloorindex;
			szmode = m_pDoc->GetModeByFloorsNumber(m_nSelectedFloor);
			absfloorindex = m_pDoc->GetAbsFloorIndexByMode(m_nSelectedFloor, szmode);
			CFloors& floors = m_pDoc->GetFloorByMode(szmode);

			int nFloorPixelHeight = UnitsToPixels(floors.m_vFloors[absfloorindex]->Altitude());
			CRect rcFloor(m_rcClient.left, nFloorPixelHeight - HALFFLOORBARTHICKNESS,
				m_rcClient.right, nFloorPixelHeight + HALFFLOORBARTHICKNESS);*/		

			
			
		}	
		//CString stooltip = GetToolTip(m_nSelectedFloor);
		//ShowToolTip(stooltip);
		m_bMovingFloor=TRUE;

		Update3DView();
	}
	m_ptLast = point;
	CDialog::OnMouseMove(nFlags, point);
}

void CDlgFloorAdjust::MoveFloor(double delta)
{
	//??? waiting.
	/*EnvironmentMode szmode;
	int absfloorindex;
	szmode = m_pDoc->GetModeByFloorsNumber(m_nSelectedFloor);
	absfloorindex = m_pDoc->GetAbsFloorIndexByMode(m_nSelectedFloor, szmode);
	std::vector<CFloor2*>& floors = m_pDoc->GetFloorByMode(szmode).m_vFloors;*/
	//std::vector<CFloor2*>& floors = m_pDoc->GetCurModeFloor().m_vFloors;
	CFloors& floors = m_pDoc->GetCurModeFloor();
	int nFloorCount = floors.GetCount();
	CRenderFloor* pFloor = floors.GetFloor(m_nSelectedFloor);
	if(!pFloor)
		return;
	
	double dNewAlt = pFloor->Altitude()  + delta;	
	double dBuffer = FLOORBARTHICKNESS*m_nUnitsPerPixel;

	if(dNewAlt < 0.0)
		dNewAlt = 0.0;

	//there is a floor above
	if(CRenderFloor* pAboveFloor = floors.GetFloor(m_nSelectedFloor+1))
	{
		double dNextAlt = pAboveFloor->Altitude() -dBuffer;
		dNewAlt = MIN(dNewAlt,dNextAlt);
	}
	//there is a floor below
	if(CRenderFloor* pbelowFloor = floors.GetFloor(m_nSelectedFloor-1) )
	{
		double dPreAlt = pbelowFloor->Altitude() + dBuffer;
		dNewAlt = MAX(dPreAlt, dNewAlt);
	}

	pFloor->Altitude(dNewAlt);
	
	FloorAltChangeInfo info;
	info.mode = m_pDoc->GetCurrentMode();
	info.pFloor = pFloor;
	m_pDoc->UpdateAllViews(NULL, VM_UPDATE_FLOOR_ALTITUDE, (CObject*)&info );

	if (m_pDoc->m_systemMode == EnvMode_LandSide)
	{
		int nTermanalFloor = pFloor->linkToTerminalFloor();
		if (nTermanalFloor >= 0)
		{
			const CFloors& pTerminalFloors = m_pDoc->GetFloorByMode(EnvMode_Terminal);
			CRenderFloor* terminalfloor = (CRenderFloor*)pTerminalFloors.GetFloor2(nTermanalFloor);
			terminalfloor->Altitude(dNewAlt);
			terminalfloor->RealAltitude(pFloor->RealAltitude());
			m_pDoc->GetFloorByMode(EnvMode_Terminal).SaveData( m_pDoc->m_ProjInfo.path, true );
			if(!m_pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)))
				return ;
			m_pDoc->UpdateAllViews(NULL,VM_UPDATE_ONE_FLOOR,(CObject*)terminalfloor);
		}
	}
	if (m_pDoc->m_systemMode == EnvMode_Terminal)
	{
		const CFloors& pLandsideFloors = m_pDoc->GetFloorByMode(EnvMode_LandSide);
		int nLandsidefloorCount = pLandsideFloors.GetCount();
		for (int i = 0; i < nLandsidefloorCount; i++)
		{
			CRenderFloor* Landsidefloor = (CRenderFloor*)pLandsideFloors.GetFloor2(i);
			int nTermanalFloor = Landsidefloor->linkToTerminalFloor();
			if (nTermanalFloor == m_nSelectedFloor)
			{
				Landsidefloor->Altitude(dNewAlt);
				Landsidefloor->RealAltitude(pFloor->RealAltitude());
				m_pDoc->GetFloorByMode(EnvMode_LandSide).SaveData( m_pDoc->m_ProjInfo.path, true );
				if(!m_pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)))
					return ;
				m_pDoc->UpdateAllViews(NULL,VM_UPDATE_ONE_FLOOR,(CObject*)Landsidefloor);
			}
		}
		
	}
}

void CDlgFloorAdjust::ModifyScale(double delta)
{
	double dAltScale = m_scale+delta;
    if (dAltScale<0.0)
		dAltScale = 0.0;
	m_scale = dAltScale;
	m_pDoc->m_iScale = m_scale/nHStep;
	Update3DView();
}

void CDlgFloorAdjust::ShowToolTip(LPCTSTR string)
{
	m_ToolTip.Close();
	
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	point.Offset(5,-10);
	m_ToolTip.SetText(string);	
	m_ToolTip.Show(point);

	//m_ToolTip.Invalidate(FALSE);
	
}

LRESULT CDlgFloorAdjust::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bMouseOver = FALSE;
	//m_pKeyFrameHotSeled=NULL;
	m_ToolTip.Close();
	
    return 0;
}


LRESULT CDlgFloorAdjust::OnMouseHover(WPARAM wParam, LPARAM lParam)
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
	if(nFloorUnderCursor>=0)
	{
		CString strTip = _T("Tooltip");

		EnvironmentMode szmode;
		szmode = m_pDoc->GetModeByFloorsNumber( nFloorUnderCursor );
		std::vector<CFloor2*>& floors = m_pDoc->GetFloorByMode(szmode).m_vFloors;
		int absfloorindex = m_pDoc->GetAbsFloorIndexByMode(nFloorUnderCursor, szmode);

		if(m_pDoc )
		{
			if (m_pDoc->m_systemMode == EnvMode_AirSide || m_pDoc->m_systemMode == EnvMode_LandSide)
			{
				strTip.Format("%s\nAltitude %.2f %s",\
				floors[absfloorindex]->FloorName(),\
				CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),floors[absfloorindex]->Altitude()),\
				CARCLengthUnit::GetLengthUnitString(m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit()));
			}			
			else
			{
				strTip.Format("%s\nAltitude %.2f %s",\
				floors[absfloorindex]->FloorName(),\
				UNITSMANAGER->ConvertLength(floors[absfloorindex]->Altitude()),\
				UNITSMANAGER->GetLengthUnitString());
			}			
		}
		if(floors[absfloorindex]->IsOpaque())
			strTip += "\nOpaque";
		if (m_pDoc->m_systemMode != EnvMode_AirSide)
		{
			ShowToolTip(strTip);
		}

	}
	return 0;
}

void CDlgFloorAdjust::OnButtonZoomIn() 
{
	if (m_nMaxH>1) // avoid integer overflow
	{
		m_nMaxH /= 2;
		m_nUnitsPerPixel = m_nMaxH / HEIGHT_PIXELS;
	}
	Invalidate();
}

void CDlgFloorAdjust::OnButtonZoomOut() 
{
	if (m_nMaxH < (std::numeric_limits<int>::max)()/2) // avoid integer overflow
	{
		m_nMaxH *= 2;
		m_nUnitsPerPixel = m_nMaxH / HEIGHT_PIXELS;
	}
	Invalidate();
}

void CDlgFloorAdjust::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if(GetSafeHwnd())
		GetBarRect(&m_rcClient);
}

void CDlgFloorAdjust::OnEndModifyScale()
{
	CFloorAdjustmentData::WriteDataToDataBase(m_pDoc->m_iScale) ;
}

CString CDlgFloorAdjust::GetToolTip( int nFloor )
{
	if(nFloor!=-1)	
	{
		CString strTip = _T("Tooltip");

		EnvironmentMode szmode;
		szmode = m_pDoc->GetModeByFloorsNumber( nFloor );
		std::vector<CFloor2*>& floors = m_pDoc->GetFloorByMode(szmode).m_vFloors;
		int absfloorindex = m_pDoc->GetAbsFloorIndexByMode(nFloor, szmode);

		if(m_pDoc && m_pDoc->m_systemMode == EnvMode_AirSide)
			strTip.Format("%s\nAltitude %.2f %s",\
			floors[absfloorindex]->FloorName(),\
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),floors[absfloorindex]->Altitude()),\
			CARCLengthUnit::GetLengthUnitString(m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit()));
		else
			strTip.Format("%s\nAltitude %.2f %s",\
			floors[absfloorindex]->FloorName(),\
			UNITSMANAGER->ConvertLength(floors[absfloorindex]->Altitude()),\
			UNITSMANAGER->GetLengthUnitString());

		if(floors[absfloorindex]->IsOpaque())
			strTip += "\nOpaque";
		
		return strTip;
	}
	return "";
}
