#include "stdafx.h"
#include "Resource.h"
#include "DlgFloorPictureReference.h"

#include <Common/WinMsg.h>
#include <CommonData/Fallback.h>
#include "AirsideGround.h"
#include "TermPlanDoc.h"
#include "MainFrm.h"
#include "3DView.h"

//using namespace Gdiplus;
//get picture info from the pick from map
class GetPicutureTransformInfo
{
public:
	ARCVector2 vOffset;
	DistanceUnit dLength;
	double dOrientation;

	void Get(const ARCVector2& v1Dest, const ARCVector2& v2Dest)
	{
		ARCVector2 vDirDist = v2Dest - v1Dest;
		dLength = vDirDist.Length();
		ARCVector2 vNorth(0,1);
		dOrientation = vDirDist.GetAngleOfTwoVector(vNorth);
		if(dOrientation < 0 ) 
			dOrientation += 360;
		vOffset = ( v1Dest + v2Dest ) * 0.5;
	}
	
};




IMPLEMENT_DYNAMIC(CDlgFloorPictureReference, CDialog)
CDlgFloorPictureReference::CDlgFloorPictureReference(CString strFilePath,int nLevelID,CRenderFloor* pFloor, IRender3DView* pView, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFloorPictureReference::IDD, pParent)
,m_strFilePath(strFilePath)
,m_pFloor(pFloor)
,m_nLevelID(nLevelID)
//,m_dLength(0)
//,m_dRotation(0)
,m_dOffsetX(0)
,m_dOffsetY(0)
,m_bInit(FALSE)
,m_p3DView(pView)
{
	m_dLength=pFloor->m_picInfo.dScale/100;
	m_dRotation=pFloor->m_picInfo.dRotation;

}

CDlgFloorPictureReference::~CDlgFloorPictureReference()
{
}

void CDlgFloorPictureReference::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_dLength);
	DDX_Text(pDX, IDC_EDIT2, m_dRotation);
	DDX_Text(pDX, IDC_EDIT3, m_dOffsetX);
	DDX_Text(pDX, IDC_EDIT4, m_dOffsetY);
	DDX_Control(pDX,IDC_STATIC_FILEPATH,m_wndStaticGroup);
}


BEGIN_MESSAGE_MAP(CDlgFloorPictureReference, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_PICK_REFLINE, OnBnClickedPickReflineFrom3DView)
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
	ON_MESSAGE(PICKFROMMAP,PickOnMapFromPictureEvent)
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipText )
	ON_COMMAND(ID_MAP_PAN,OnPanPicture)
	ON_COMMAND(ID_MAP_ZOOM,OnZoomPicture)
	ON_COMMAND(ID_MAP_MOVEUP, MoveUp)
	ON_COMMAND(ID_MAP_MOVEDOWN, MoveDown)
	ON_COMMAND(ID_MAP_MOVELEFT, MoveLeft)
	ON_COMMAND(ID_MAP_MOVERIGHT, MoveRight)
	ON_COMMAND(ID_MAP_PICK, PickOnMap)
	ON_COMMAND(ID_MAP_EDIT,EidtPicture)

	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT2, OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT3, OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT4, OnEnChangeEdit)
END_MESSAGE_MAP()


// CDlgFloorPictureReference message handlers

int CDlgFloorPictureReference::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP |CBRS_TOOLTIPS)
		|| !m_wndToolbar.LoadToolBar(IDR_TOOLBAR_FLRPIC))
	{
		return -1;
	}

	m_wndPictureCtrl.SetFilePath(m_strFilePath);
	m_wndPictureCtrl.Create(CRect(0,0,0,0),this);
	return 0;
}

BOOL CDlgFloorPictureReference::OnEraseBkgnd(CDC*  pDC)     
{   
	return TRUE;   
} 

void CDlgFloorPictureReference::OnPaint()
{	
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	HWND hwndT=::GetWindow(m_hWnd, GW_CHILD);
	CRect rectCW;
	CRgn rgnCW;
	rgnCW.CreateRectRgn(0,0,0,0);
	while (hwndT != NULL)
	{
		CWnd* pWnd=CWnd::FromHandle(hwndT)  ;
		if(pWnd->IsKindOf(RUNTIME_CLASS(CListCtrl))
			||pWnd == GetDlgItem(IDC_STATIC_FILEPATH))
		{
			if(!pWnd->IsWindowVisible())
			{
				hwndT=::GetWindow(hwndT,GW_HWNDNEXT);
				continue;
			}
			pWnd->GetWindowRect(rectCW);
			ScreenToClient(rectCW);
			CRgn rgnTemp;
			rgnTemp.CreateRectRgnIndirect(rectCW);
			rgnCW.CombineRgn(&rgnCW,&rgnTemp,RGN_OR);
		}
		hwndT=::GetWindow(hwndT,GW_HWNDNEXT);

	}
	CRect rect;
	GetClientRect(&rect);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rect);
	CRgn rgnDraw;
	rgnDraw.CreateRectRgn(0,0,0,0);
	rgnDraw.CombineRgn(&rgn,&rgnCW,RGN_DIFF);
	CBrush br(GetSysColor(COLOR_BTNFACE));
	dc.FillRgn(&rgnDraw,&br);
	CRect rectRight=rect;
	rectRight.left=rectRight.right-10;
	dc.FillRect(rectRight,&br);
	rectRight=rect;
	rectRight.top=rect.bottom-44;
	dc.FillRect(rectRight,&br);

	GetDlgItem(IDC_STATIC1)->Invalidate(FALSE);
	GetDlgItem(IDC_STATIC2)->Invalidate(FALSE);
	GetDlgItem(IDC_STATIC3)->Invalidate(FALSE);
	GetDlgItem(IDC_STATIC4)->Invalidate(FALSE);
	GetDlgItem(IDC_STATIC5)->Invalidate(FALSE);
	GetDlgItem(IDC_EDIT1)->Invalidate(FALSE);
	GetDlgItem(IDC_EDIT2)->Invalidate(FALSE);
	GetDlgItem(IDC_EDIT3)->Invalidate(FALSE);
	GetDlgItem(IDC_EDIT4)->Invalidate(FALSE);
	GetDlgItem(IDC_PICK_REFLINE)->Invalidate(FALSE);
	CDialog::OnPaint();
}

void CDlgFloorPictureReference::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType,cx,cy);
	
	if (m_wndStaticGroup.GetSafeHwnd() != NULL)
	{
		CRect rectClient;
		GetDlgItem(IDC_STATIC_GROUP1)->GetWindowRect(&rectClient);

		CRect rect;
		GetDlgItem(IDC_STATIC_GROUP)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		GetDlgItem(IDC_STATIC_GROUP)->SetWindowPos(NULL,rect.left,rect.top,cx - 2*rect.left,cy-rect.top - rectClient.Height() - 55,NULL);
		GetDlgItem(IDC_STATIC_GROUP)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		m_wndToolbar.SetWindowPos(NULL,rect.left+2,rect.top+8,cx-22,26,NULL);
		m_wndToolbar.GetWindowRect(&rect);
		ScreenToClient(&rect);
		GetDlgItem(IDC_STATIC_FILEPATH)->SetWindowPos(NULL,rect.left-2,rect.bottom+2, cx - 2*rect.left+4, cy - rect.bottom - rectClient.Height() - 57,NULL);
// 		const ARCVector2& pt = m_wndPictureCtrl.GetPoint();
// 		double nRateX = pt.n[VX]/m_wndPictureCtrl.GetWidth();
// 		double nRateY = pt.n[VY]/m_wndPictureCtrl.GetHeight();
// 		int nWidth = cx - 2*rect.left+4;
// 		int nHeight = cy - rect.bottom - rectClient.Height() - 57;
// 		m_wndPictureCtrl.SetPoint(ARCVector2(nWidth*nRateX,nHeight*nRateY));
		m_wndPictureCtrl.SetWindowPos(NULL,rect.left-2,rect.bottom+2, cx - 2*rect.left+4, cy - rect.bottom - rectClient.Height() - 57,NULL);
		GetDlgItem(IDC_STATIC_FILEPATH)->GetWindowRect(&rect);
		ScreenToClient(&rect);

		GetDlgItem(IDC_STATIC_GROUP1)->SetWindowPos(NULL,rect.left,rect.bottom+8,cx - 2*rect.left,rectClient.Height(),NULL);
		GetDlgItem(IDC_STATIC_GROUP1)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		CRect rectPick;
		GetDlgItem(IDC_PICK_REFLINE)->GetWindowRect(&rectPick);
		GetDlgItem(IDC_PICK_REFLINE)->SetWindowPos(NULL,rect.left+8,rect.top+30,rectPick.Width(),rectPick.Height(),NULL);
		CRect rc;
		GetDlgItem(IDC_PICK_REFLINE)->GetWindowRect(&rc);
		ScreenToClient(&rc);
		GetDlgItem(IDC_STATIC1)->GetWindowRect(&rectPick);
		GetDlgItem(IDC_STATIC1)->SetWindowPos(NULL,rc.right+25,rect.top+35,rectPick.Width(),rectPick.Height(),NULL);
		GetDlgItem(IDC_EDIT1)->GetWindowRect(&rectPick);
		GetDlgItem(IDC_STATIC1)->GetWindowRect(&rc);
		ScreenToClient(&rectPick);
		ScreenToClient(&rc);
		GetDlgItem(IDC_EDIT1)->SetWindowPos(NULL,rc.right+6,rect.top+30,rectPick.Width(),rectPick.Height(),NULL);
		GetDlgItem(IDC_STATIC2)->GetWindowRect(&rectPick);
		GetDlgItem(IDC_EDIT1)->GetWindowRect(&rc);
		ScreenToClient(&rectPick);
		ScreenToClient(&rc);
		GetDlgItem(IDC_STATIC2)->SetWindowPos(NULL,rc.right+35,rect.top+30,rectPick.Width(),rectPick.Height(),NULL);
		GetDlgItem(IDC_EDIT2)->GetWindowRect(&rectPick);
		GetDlgItem(IDC_STATIC2)->GetWindowRect(&rc);
		ScreenToClient(&rc);
		ScreenToClient(&rectPick);
		GetDlgItem(IDC_EDIT2)->SetWindowPos(NULL,rc.right+6,rect.top+30,rectPick.Width(),rectPick.Height(),NULL);
		GetDlgItem(IDC_STATIC3)->GetWindowRect(&rectPick);
		GetDlgItem(IDC_EDIT2)->GetWindowRect(&rc);
		ScreenToClient(&rc);
		ScreenToClient(&rectPick);
		GetDlgItem(IDC_STATIC3)->SetWindowPos(NULL,rc.left,rect.bottom+16,rectPick.Width(),rectPick.Height(),NULL);
		GetDlgItem(IDC_STATIC4)->GetWindowRect(&rectPick);
		GetDlgItem(IDC_STATIC3)->GetWindowRect(&rc);
		ScreenToClient(&rc);
		ScreenToClient(&rectPick);
		GetDlgItem(IDC_STATIC4)->SetWindowPos(NULL,rc.right+3,rect.bottom+16,rectPick.Width(),rectPick.Height(),NULL);
		GetDlgItem(IDC_EDIT3)->GetWindowRect(&rectPick);
		GetDlgItem(IDC_STATIC4)->GetWindowRect(&rc);
		ScreenToClient(&rc);
		ScreenToClient(&rectPick);
		GetDlgItem(IDC_EDIT3)->SetWindowPos(NULL,rc.right+2,rect.bottom+16,rectPick.Width(),rectPick.Height(),NULL);
		GetDlgItem(IDC_STATIC5)->GetWindowRect(&rectPick);
		GetDlgItem(IDC_EDIT3)->GetWindowRect(&rc);
		ScreenToClient(&rectPick);
		ScreenToClient(&rc);
		GetDlgItem(IDC_STATIC5)->SetWindowPos(NULL,rc.right+3,rect.bottom+16,rectPick.Width(),rectPick.Height(),NULL);
		GetDlgItem(IDC_EDIT4)->GetWindowRect(&rectPick);
		GetDlgItem(IDC_STATIC5)->GetWindowRect(&rc);
		ScreenToClient(&rectPick);
		ScreenToClient(&rc);
		GetDlgItem(IDC_EDIT4)->SetWindowPos(NULL,rc.right+2,rect.bottom+16,rectPick.Width(),rectPick.Height(),NULL);
		GetDlgItem(IDOK)->GetWindowRect(&rectPick);
		ScreenToClient(&rectPick);
		GetDlgItem(IDOK)->SetWindowPos(NULL,rect.right-2*rectPick.Width()-6,rect.bottom+16,rectPick.Width(),rectPick.Height(),NULL);
		GetDlgItem(IDCANCEL)->GetWindowRect(&rectPick);
		ScreenToClient(&rectPick);
		GetDlgItem(IDCANCEL)->SetWindowPos(NULL,rect.right-rectPick.Width(),rect.bottom+16,rectPick.Width(),rectPick.Height(),NULL);
	}
}

void CDlgFloorPictureReference::UpdateUIState()
{
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MAP_MOVEUP, TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MAP_MOVEDOWN, TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MAP_MOVELEFT, TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MAP_MOVERIGHT, TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MAP_EDIT, TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MAP_PAN,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MAP_ZOOM,TRUE);

	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MAP_PICK, TRUE);

	m_wndToolbar.GetToolBarCtrl().CheckButton(ID_MAP_PAN,m_wndPictureCtrl.GetPan());
	m_wndToolbar.GetToolBarCtrl().CheckButton(ID_MAP_ZOOM,m_wndPictureCtrl.GetZoom());
	

	CPictureInfo& overlay = m_pFloor->m_picInfo;
	BOOL refHasValue = TRUE;
	if(overlay.refLine.getCount() != 2)
		refHasValue = FALSE;
	else
	{
		ASSERT(overlay.refLine.getCount() == 2);
		Point pt1 = overlay.refLine.getPoint( 0 );
		Point pt2 = overlay.refLine.getPoint( 1 );
		refHasValue =  (pt1.getX() != 0.0 && pt1.getY() != 0.0 && pt2.getX() != 0.0 && pt2.getY() != 0.0);
	}

	GetDlgItem(IDC_PICK_REFLINE)->EnableWindow(refHasValue);

	GetDlgItem(IDC_EDIT1)->EnableWindow(refHasValue);
	GetDlgItem(IDC_EDIT2)->EnableWindow(refHasValue);
	//GetDlgItem(IDC_EDIT3)->EnableWindow(refHasValue);
	//GetDlgItem(IDC_EDIT4)->EnableWindow(refHasValue);
	
	GetDlgItem(IDOK)->EnableWindow(refHasValue);
}

BOOL CDlgFloorPictureReference::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rectFltToolbar;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect(&rectFltToolbar);
	ScreenToClient(&rectFltToolbar);
	m_wndToolbar.MoveWindow(&rectFltToolbar, true);

	CRect rect;
	GetDlgItem(IDC_STATIC_FILEPATH)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	m_wndPictureCtrl.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),NULL);
	m_wndPictureCtrl.SetPath(m_pFloor->m_picInfo.refLine);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);


	m_bInit = true;


	UpdateUIState();

	return TRUE;
}

void CDlgFloorPictureReference::HideDialog(CWnd* parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CDlgFloorPictureReference::ShowDialog(CWnd* parentWnd)
{
	while (!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		parentWnd->ShowWindow(SW_SHOW);
		parentWnd->EnableWindow(FALSE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(FALSE);
	ShowWindow(SW_SHOW);	
	EnableWindow();
}


void CDlgFloorPictureReference::OnBnClickedPickReflineFrom3DView()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();


	IRender3DView* p3DView = pDoc->GetIRenderView();
	if( p3DView == NULL )
	{
		IRender3DView* pView = (IRender3DView*)pDoc->GetMainFrame()->CreateOrActiveEnvModeView();
	}
	if (!p3DView)
	{
		ASSERT(FALSE);
		return;
	}

	HideDialog( pDoc->GetMainFrame() );
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();

	enum FallbackReason enumReason = PICK_TWOPOINTS;

	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}
}

LRESULT CDlgFloorPictureReference::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_SHOW);	
	EnableWindow();
	DoTempFallBackFinished(wParam,lParam);
	UpdateTempObjectInfo();
	return true;
}

void CDlgFloorPictureReference::UpdateTempObjectInfo()
{
}

void CDlgFloorPictureReference::DoneEditTempObject()
{
}

bool CDlgFloorPictureReference::DoTempFallBackFinished(WPARAM wParam, LPARAM lParam)
{

	std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);
	size_t nSize = pData->size();
	ASSERT(nSize == 2);

	CPictureInfo& overlay = m_pFloor->m_picInfo;

	ASSERT(overlay.refLine.getCount()>=2);
	Point p1 = overlay.refLine.getPoint(0);
	Point p2 = overlay.refLine.getPoint(1);

	GetPicutureTransformInfo tools;
	tools.Get(ARCVector2(pData->at(0)), ARCVector2(pData->at(1)));
	
	overlay.dRotation = tools.dOrientation;
	overlay.vOffset = tools.vOffset;
	m_dLength  = tools.dLength / 100.0;
	
	m_dRotation = overlay.dRotation;
	m_dOffsetX = overlay.vOffset[0];
	m_dOffsetY = overlay.vOffset[1];
	
	UpdateData(FALSE);
	UpdateUIState();

	CString strTemp;
	strTemp.Format("%.2f", m_dLength);
	GetDlgItem(IDC_EDIT1)->SetWindowText(strTemp);

	strTemp.Format("%.2f", m_dRotation);
	GetDlgItem(IDC_EDIT2)->SetWindowText(strTemp);

	strTemp.Format("%.2f", m_dOffsetX);
	GetDlgItem(IDC_EDIT3)->SetWindowText(strTemp);

	strTemp.Format("%.2f", m_dOffsetY);
	GetDlgItem(IDC_EDIT4)->SetWindowText(strTemp);


	return true;

}

void CDlgFloorPictureReference::OnPanPicture()
{
	m_wndPictureCtrl.SetPan(TRUE);
	m_wndPictureCtrl.SetZoom(FALSE);
	UpdateUIState();
}

void CDlgFloorPictureReference::OnZoomPicture()
{
	m_wndPictureCtrl.SetZoom(TRUE);
	m_wndPictureCtrl.SetPan(FALSE);
	UpdateUIState();
}

void CDlgFloorPictureReference::MoveUp()
{
	const ARCVector2& pt = m_wndPictureCtrl.GetPoint();

	double nOffset = m_wndPictureCtrl.GetImageHeight()/10;
	m_wndPictureCtrl.SetPoint(ARCVector2(pt.x,pt.y-nOffset));
	m_wndPictureCtrl.SetMove(TRUE);
	m_wndPictureCtrl.SetPan(FALSE);
	m_wndPictureCtrl.SetZoom(FALSE);
	UpdateUIState();
	m_wndPictureCtrl.Invalidate(FALSE);
}

void CDlgFloorPictureReference::MoveDown()
{
	const ARCVector2& pt = m_wndPictureCtrl.GetPoint();
	double nOffset = m_wndPictureCtrl.GetImageHeight()/10;
	m_wndPictureCtrl.SetPoint(ARCVector2(pt.x,pt.y+nOffset));
	m_wndPictureCtrl.SetMove(TRUE);
	m_wndPictureCtrl.SetPan(FALSE);
	m_wndPictureCtrl.SetZoom(FALSE);
	UpdateUIState();
	m_wndPictureCtrl.Invalidate(FALSE);
}

void CDlgFloorPictureReference::MoveLeft()
{
	const ARCVector2& pt = m_wndPictureCtrl.GetPoint();
	double nOffset = m_wndPictureCtrl.GetImageWidth()/10;
	m_wndPictureCtrl.SetPoint(ARCVector2(pt.x+nOffset,pt.y));
	m_wndPictureCtrl.SetMove(TRUE);
	m_wndPictureCtrl.SetPan(FALSE);
	m_wndPictureCtrl.SetZoom(FALSE);
	UpdateUIState();
	m_wndPictureCtrl.Invalidate(FALSE);
}

void CDlgFloorPictureReference::MoveRight()
{
	const ARCVector2& pt = m_wndPictureCtrl.GetPoint();
	double nOffset = m_wndPictureCtrl.GetImageWidth()/10;
	m_wndPictureCtrl.SetPoint(ARCVector2(pt.x-nOffset,pt.y));
	m_wndPictureCtrl.SetMove(TRUE);
	m_wndPictureCtrl.SetPan(FALSE);
	m_wndPictureCtrl.SetZoom(FALSE);
	UpdateUIState();
	m_wndPictureCtrl.Invalidate(FALSE);
}

void CDlgFloorPictureReference::PickOnMap()
{
	m_pFloor->m_picInfo.refLine.clear();
	m_dLength = m_dRotation = m_dOffsetX = m_dOffsetY = 0.0;

	m_wndPictureCtrl.SetReferenceLine(TRUE);
	m_wndPictureCtrl.SetPan(FALSE);
	m_wndPictureCtrl.SetZoom(FALSE);

	UpdateData(FALSE);
	UpdateUIState();
}

void CDlgFloorPictureReference::EidtPicture()
{
	CString strFilters = _T("Floor Picture File (*.bmp;*.jpg;*.png;*.tif;*tiff)|*.bmp;*.jpg;*.png;*.tif;*.tiff|");
	CFileDialog fileDlg (TRUE, NULL, NULL, OFN_FILEMUSTEXIST, strFilters, this);

	if (fileDlg.DoModal() != IDOK)
		return;

	CString strFilePath = fileDlg.GetPathName();
	if (!strFilePath.CompareNoCase(m_strFilePath))
	{
		return;
	}
	m_wndPictureCtrl.SetFilePath(strFilePath);
	m_strFilePath = strFilePath;
	m_wndPictureCtrl.Invalidate(FALSE);
	CRect rect;
	GetDlgItem(IDC_STATIC_FILEPATH)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	m_wndPictureCtrl.SetPictureLoad(FALSE);
	m_wndPictureCtrl.SetTextureValid(FALSE);
	
	CPictureInfo& overlay = m_pFloor->m_picInfo;
	overlay.bShow = TRUE;
	overlay.dRotation = 0;
	overlay.dScale = 1;
	overlay.vOffset = ARCVector2(0,0);
	overlay.vSize = ARCVector2(0,0);
	Point ptList[2];
	ptList[0].setX(0.0);ptList[0].setY(0.0);ptList[0].setZ(0.0);
	ptList[1].setX(0.0);ptList[1].setY(0.0);ptList[1].setZ(0.0);

	overlay.refLine.init(2, ptList);
	m_wndPictureCtrl.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),NULL);
	m_wndPictureCtrl.SetPath(overlay.refLine);
	ASSERT(m_p3DView != NULL);
	m_p3DView->UpdateFloorOverlay(m_nLevelID,((CAirsideGround *)m_pFloor)->m_picInfo);
	UpdateUIState();
}

LRESULT CDlgFloorPictureReference::PickOnMapFromPictureEvent(WPARAM wParam, LPARAM lParam)
{
	Path& path = m_wndPictureCtrl.GetPath();

	if (path.getCount() == 2)
	{
		double dImageWidth = m_wndPictureCtrl.GetImageWidth();
		double dWidth = m_wndPictureCtrl.GetWidth();
		double dRate = dImageWidth/dWidth;

		Point head, tail;
		head.setPoint(path.getPoint(0).getX()*dRate,path.getPoint(0).getY()*dRate,0.0);
		tail.setPoint(path.getPoint(1).getX()*dRate,path.getPoint(1).getY()*dRate,0.0);
		Point ptList[2];
		ptList[0] = head;
		ptList[1] = tail;

		CPictureInfo& overlay = m_pFloor->m_picInfo;
		overlay.refLine.init( 2, ptList);
		if(m_dLength != 0.0 )
		{
			ASSERT(m_p3DView != NULL);
			m_p3DView->UpdateFloorOverlay(m_nLevelID,((CAirsideGround *)m_pFloor)->m_picInfo);
		}
		UpdateUIState();
	}
	return true;
}

BEGIN_EVENTSINK_MAP(CDlgFloorPictureReference, CDialog)
	ON_EVENT(CDlgFloorPictureReference, IDC_IMAGEVIEWER1, 1, FireGetReferenceLinePositionOnMapEvent, VTS_NONE)
END_EVENTSINK_MAP()

void CDlgFloorPictureReference::FireGetReferenceLinePositionOnMapEvent()
{
	double a=.0,b=.0,c=.0,d=.0;

//	m_spFlrPicDefine->GetReferenceLinePositionOnMap(&a,&b,&c,&d);

	CPictureInfo& overlay = m_pFloor->m_picInfo;

	Point head, tail;
	double z=.0;
	head.setPoint( a, b, z);
	tail.setPoint( c, d, z);

	Point ptList[2];
	ptList[0] = head;
	ptList[1] = tail;
	overlay.refLine.init( 2, ptList);

	UpdateUIState();

	if(m_dLength != 0.0 )
	{
		ASSERT(m_p3DView != NULL);
		m_p3DView->UpdateFloorOverlay(m_nLevelID,((CAirsideGround *)m_pFloor)->m_picInfo);
	}
}

void CDlgFloorPictureReference::OnBnClickedOk()
{
	UpdateData(TRUE);

	if(m_dLength <=0.0)
	{
		AfxMessageBox("Please set reference line's length first");
		return;
	}

	CPictureInfo& overlay = m_pFloor->m_picInfo;
	overlay.sFileName = m_strFilePath;

	CAirsideGround* asground = dynamic_cast<CAirsideGround*>(m_pFloor);
	if(asground){
		asground->SaveOverlayInfo( m_nLevelID );	
	}
	
	OnOK();
}

void CDlgFloorPictureReference::OnBnClickedCancel()
{
	((CAirsideGround *)m_pFloor)->ReadOverlayInfo( m_nLevelID );
	OnCancel();
}

void CDlgFloorPictureReference::OnEnChangeEdit()
{
	if(!m_bInit)
		return;

	UpdateData(TRUE);

	CPictureInfo& overlay = m_pFloor->m_picInfo;
	overlay.sFileName = m_strFilePath;
	overlay.sPathName = m_strFilePath;
	overlay.dScale = m_dLength * 100.0;

	overlay.dRotation = m_dRotation;
	overlay.vOffset[0] = m_dOffsetX;
	overlay.vOffset[1] = m_dOffsetY;

	UpdateUIState();
	
	ASSERT(m_p3DView != NULL);
	m_p3DView->UpdateFloorOverlay(m_nLevelID,m_pFloor->m_picInfo);
	
}

BOOL CDlgFloorPictureReference::OnToolTipText(UINT,NMHDR* pNMHDR,LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	// if there is a top level routing frame then let it handle the message
	/*if (GetRoutingFrame() != NULL) return FALSE;*/

	// to be thorough we will need to handle UNICODE versions of the message also !!
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	TCHAR szFullText[512];
	CString strTipText;
	UINT nID = pNMHDR->idFrom;

	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{
		// idFrom is actually the HWND of the tool 
		nID = ::GetDlgCtrlID((HWND)nID);
	}

	if (nID!= 0) // will be zero on a separator
	{

		AfxLoadString(nID, szFullText);
		strTipText=szFullText;

#ifndef _UNICODE
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
		}
		else
		{
			_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
		}
#else
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			_wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
		}
		else
		{
			lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
		}
#endif

		*pResult = 0;

		// bring the tooltip window above other popup windows
		::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
			SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);

		return TRUE;
	}

	return FALSE;
}
