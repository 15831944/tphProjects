// DlgAircraftFurnishingDB.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgAircraftFurnishingDB.h"
#include "../InputOnBoard/AircraftFurnishingSection.h"
#include "DlgAircraftFurnishingNameSpec.h"
#include "../Database/ADODatabase.h"
#include "../Renderer/RenderEngine/RenderEngine.h"
#include "../Common/BitmapSaver.h"
#include "../Common/WinMsg.h"
// CDlgAircraftFurnishingDB dialog

IMPLEMENT_DYNAMIC(CDlgAircraftFurnishingDB, CDialog)
CDlgAircraftFurnishingDB::CDlgAircraftFurnishingDB(CAircraftFurnishingSectionManager* _FurnishMan ,CWnd* pParent /*=NULL*/)
	: m_pFurnishishManger(_FurnishMan),CDialog(CDlgAircraftFurnishingDB::IDD, pParent)
	, m_pFurnishEditModel(NULL)
{
	ClearBmps(false);}

void CDlgAircraftFurnishingDB::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FURNISH, m_wndListCtrl);
	DDX_Control(pDX, IDC_MODEL_VIEW_Z, m_wndPicture);
}


BEGIN_MESSAGE_MAP(CDlgAircraftFurnishingDB, CDialog)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_MODEL_NEW,OnAddFurnishingSection)
	ON_COMMAND(ID_MODEL_DELETE,OnRemoveFurnishingSection)
	ON_COMMAND(ID_MODEL_EDIT,OnEditFurnishingSection)
	ON_COMMAND(ID_EDIT_COMPONENT,OnEditFurnishingSection)
	ON_NOTIFY(LVN_ITEMCHANGED,IDC_LIST_FURNISH,OnSelChangeFurnishingSection)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_MESSAGE(WM_NOEDIT_DBCLICK,OnDBClick)
	ON_NOTIFY(NM_RCLICK,IDC_LIST_FURNISH, OnRClick)
END_MESSAGE_MAP()


// CDlgAircraftFurnishingDB message handlers

BOOL CDlgAircraftFurnishingDB::OnInitDialog()
{
	CDialog::OnInitDialog();

	OnInitToolbar();
	OnInitListCtrl();
	UpdateToolbarState();

	InitListData();
	DisplayAllBmpInCtrl();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgAircraftFurnishingDB::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndACTypeModel.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS ) ||
		!m_wndACTypeModel.LoadToolBar(IDR_DATABASE_MODEL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

void CDlgAircraftFurnishingDB::OnInitToolbar()
{
	CRect rect;
	m_wndListCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.bottom = rect.top - 6;
	rect.top = rect.bottom - 22;
	m_wndACTypeModel.MoveWindow(&rect,true);
	m_wndACTypeModel.ShowWindow(SW_SHOW);

	m_wndACTypeModel.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,TRUE);
	m_wndACTypeModel.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,TRUE);
	m_wndACTypeModel.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,TRUE);
}

void CDlgAircraftFurnishingDB::OnInitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	char* columnLabel[] = {	"Index","Type", "Level1Name", "Level2Name","Level3Name","Level4Name","Create Time","Modified Time","Creator"};
	int DefaultColumnWidth[] = {45,50,95,95,95,95,90,90,50};

	int nColFormat[] = 
	{	
		LVCFMT_NOEDIT,
			LVCFMT_NOEDIT,
			LVCFMT_NOEDIT,
			LVCFMT_NOEDIT,
			LVCFMT_NOEDIT,
			LVCFMT_NOEDIT,
			LVCFMT_NOEDIT,
			LVCFMT_NOEDIT,
			LVCFMT_NOEDIT
	};

	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	lvc.csList = &strList;

	for (int i = 0; i < 9; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		m_wndListCtrl.InsertColumn(i, &lvc);;
	}
}

void CDlgAircraftFurnishingDB::OnPaint()
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
			|| pWnd == GetDlgItem(IDC_MODEL_VIEW_Z)
			|| pWnd == GetDlgItem(IDC_MODEL_VIEW_Y)
			|| pWnd == GetDlgItem(IDC_STATIC_ISOMETRIC_FURNISH)
			|| pWnd == GetDlgItem(IDC_STATIC_FRONT_X))
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

	DisplayAllBmpInCtrl();
	CDialog::OnPaint();
}

void CDlgAircraftFurnishingDB::OnRemoveFurnishingSection()
{
	int nSel = GetCurSelIndex();
	if (nSel != LB_ERR)
	{
		CAircraftFurnishingModel* pFurnish = NULL;
		pFurnish = (CAircraftFurnishingModel*)m_wndListCtrl.GetItemData(nSel);
		m_pFurnishishManger->DelModel(pFurnish) ;
		m_wndListCtrl.DeleteItem(nSel) ;
	}
	InitListData() ;
	DisplayAllBmpInCtrl() ;
}
int CDlgAircraftFurnishingDB::GetCurSelIndex()
{
	POSITION position =  m_wndListCtrl.GetFirstSelectedItemPosition() ;
	return m_wndListCtrl.GetNextSelectedItem(position);
}
void CDlgAircraftFurnishingDB::OnAddFurnishingSection()
{
	CAircraftFurnishingModel* section = new CAircraftFurnishingModel(m_pFurnishishManger) ;
	CDlgAircraftFurnishingNameSpec dlg(section,GetParent()) ;
	dlg.SetNew(true);
	if(dlg.DoModal() != IDOK)
	{
		delete section ;
		return;
	}
	m_pFurnishishManger->AddModel(section);
	int item = InsertItemToList(section,m_wndListCtrl.GetItemCount()) ;
	m_wndListCtrl.SetItemState(item, LVIS_SELECTED, LVIS_SELECTED);
	m_wndListCtrl.SetFocus();
	return;
}

void CDlgAircraftFurnishingDB::OnEditFurnishingSection()
{
	//open Aircraft Model Database View
	int nSel = GetCurSelIndex();
	if (nSel != LB_ERR)
	{
		CAircraftFurnishingModel* pFurnish = NULL;
		pFurnish = (CAircraftFurnishingModel*)m_wndListCtrl.GetItemData(nSel);
		if(pFurnish != NULL)
		{
			m_pFurnishEditModel = pFurnish;
			OnOK();
		}
	}
}

LRESULT CDlgAircraftFurnishingDB::OnDBClick(WPARAM wParam, LPARAM lParam)
{
	int nSel = GetCurSelIndex();
	if (nSel != LB_ERR)
	{
		CAircraftFurnishingModel* pFurnish = NULL;
		pFurnish = (CAircraftFurnishingModel*)m_wndListCtrl.GetItemData(nSel);
		if(pFurnish != NULL)
		{
			CDlgAircraftFurnishingNameSpecEdit dlgEdit(pFurnish,GetParent()) ;
			if(dlgEdit.DoModal() == IDOK)
			{
				EditItemToList(pFurnish,nSel) ;
				DisplayAllBmpInCtrl() ;
			}
		}
	}

	return 0;
}

void CDlgAircraftFurnishingDB::UpdateToolbarState()
{
	BOOL bEditEnable = FALSE;
	BOOL bDelEnable = FALSE;
	int nSeleIndex = GetCurSelIndex();
	if (nSeleIndex != LB_ERR)
	{
// 		CAircraftFurnishingModel* pFurnish = (CAircraftFurnishingModel*)m_wndListCtrl.GetItemData(nSeleIndex);
// 		if(pFurnish)
// 		{
			bEditEnable = TRUE/*!pFurnish->mbExternalMesh*/;
			bDelEnable = TRUE;
// 		}
	}
	m_wndACTypeModel.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,bEditEnable);
	m_wndACTypeModel.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,bDelEnable);
}

void CDlgAircraftFurnishingDB::OnSelChangeFurnishingSection(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	UpdateToolbarState();
	if((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
	{
		GetDlgItem(IDC_MODEL_VIEW_Z)->Invalidate(FALSE);
		GetDlgItem(IDC_MODEL_VIEW_Y)->Invalidate(FALSE);
		GetDlgItem(IDC_STATIC_FRONT_X)->Invalidate(FALSE);
		GetDlgItem(IDC_STATIC_ISOMETRIC_FURNISH)->Invalidate(FALSE);
		return;
	}

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
	{
		GetDlgItem(IDC_MODEL_VIEW_Z)->Invalidate(FALSE);
		GetDlgItem(IDC_MODEL_VIEW_Y)->Invalidate(FALSE);
		GetDlgItem(IDC_STATIC_FRONT_X)->Invalidate(FALSE);
		GetDlgItem(IDC_STATIC_ISOMETRIC_FURNISH)->Invalidate(FALSE);
		return;
	}

	if(pnmv->uOldState & LVIS_SELECTED) 
	{
		if(!(pnmv->uNewState & LVIS_SELECTED)) 
		{
			DisplayBmpInCtrl(NULL,IDC_MODEL_VIEW_Z);
			DisplayBmpInCtrl(NULL,IDC_MODEL_VIEW_Y);
			DisplayBmpInCtrl(NULL,IDC_STATIC_FRONT_X);
			DisplayBmpInCtrl(NULL,IDC_STATIC_ISOMETRIC_FURNISH);

			m_wndACTypeModel.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,FALSE);
			m_wndACTypeModel.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,FALSE);
		}	
	}
	else if(pnmv->uNewState & LVIS_SELECTED)
	{
		int nSel = m_wndListCtrl.GetCurSel();
		CAircraftFurnishingModel* pFurnish = (CAircraftFurnishingModel*)m_wndListCtrl.GetItemData(nSel);
		LoadBmp(pFurnish);
		DisplayAllBmpInCtrl();
	}
// 	GetDlgItem(IDC_MODEL_VIEW_Z)->Invalidate(FALSE);
// 	GetDlgItem(IDC_MODEL_VIEW_Y)->Invalidate(FALSE);
// 	GetDlgItem(IDC_STATIC_FRONT_X)->Invalidate(FALSE);
// 	GetDlgItem(IDC_STATIC_ISOMETRIC_FURNISH)->Invalidate(FALSE);
	Invalidate(FALSE);
}

void CDlgAircraftFurnishingDB::InitListData()
{
	m_wndListCtrl.DeleteAllItems();
 	for(int i = 0 ; i < m_pFurnishishManger->GetModelCount();i++)
 	{
 		CAircraftFurnishingModel* pFurnishing = m_pFurnishishManger->GetModel(i);
        InsertItemToList(pFurnishing,i) ;
 	}
}

int CDlgAircraftFurnishingDB::InsertItemToList(CAircraftFurnishingModel* _AirsideFuenishing,int index) 
{
	int item = 0 ;
	CString str ;
	str.Format(_T("%d"),index+1) ;
	item = m_wndListCtrl.InsertItem(index,str);

	ALTObjectID altObjID;
	altObjID.FromString(_AirsideFuenishing->m_sModelName);
	m_wndListCtrl.SetItemText(item, 1,_AirsideFuenishing->m_Type);
	m_wndListCtrl.SetItemText(item, 2, altObjID.at(0).c_str());
	m_wndListCtrl.SetItemText(item, 3, altObjID.at(1).c_str());
	m_wndListCtrl.SetItemText(item, 4, altObjID.at(2).c_str());
	m_wndListCtrl.SetItemText(item, 5, altObjID.at(3).c_str());
	CString strTime;
	CTime time = _AirsideFuenishing->GetCreateTime();
	strTime.Format(_T("%02d:%02d:%02d"),time.GetHour(),time.GetMinute(),time.GetSecond());
	m_wndListCtrl.SetItemText(item, 6,strTime);
	time = _AirsideFuenishing->GetLastModifiedTime();
	strTime.Format(_T("%02d:%02d:%02d"),time.GetHour(),time.GetMinute(),time.GetSecond());
	m_wndListCtrl.SetItemText(item, 7,strTime);
	m_wndListCtrl.SetItemText(item, 8,_AirsideFuenishing->GetCreator());
	m_wndListCtrl.SetItemData(item,(DWORD_PTR)_AirsideFuenishing);
	return item ;
}
void CDlgAircraftFurnishingDB::EditItemToList(CAircraftFurnishingModel* _AirsideFuenishing,int item) 
{
	ALTObjectID altObjID;
	altObjID.FromString(_AirsideFuenishing->m_sModelName);
	m_wndListCtrl.SetItemText(item, 1,_AirsideFuenishing->m_Type);
	m_wndListCtrl.SetItemText(item, 2, altObjID.at(0).c_str());
	m_wndListCtrl.SetItemText(item, 3, altObjID.at(1).c_str());
	m_wndListCtrl.SetItemText(item, 4, altObjID.at(2).c_str());
	m_wndListCtrl.SetItemText(item, 5, altObjID.at(3).c_str());
	CString strTime;
	CTime time = _AirsideFuenishing->GetCreateTime();
	strTime.Format(_T("%02d:%02d:%02d"),time.GetHour(),time.GetMinute(),time.GetSecond());
	m_wndListCtrl.SetItemText(item, 6,strTime);
	time = _AirsideFuenishing->GetLastModifiedTime();
	strTime.Format(_T("%02d:%02d:%02d"),time.GetHour(),time.GetMinute(),time.GetSecond());
	m_wndListCtrl.SetItemText(item, 7,strTime);
	m_wndListCtrl.SetItemText(item, 8,_AirsideFuenishing->GetCreator());
	m_wndListCtrl.SetItemData(item,(DWORD_PTR)_AirsideFuenishing);
}
void CDlgAircraftFurnishingDB::OnBnClickedOk()
{
	CWaitCursor wc;
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pFurnishishManger->SaveData();
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	CDialog::OnOK();
}

void CDlgAircraftFurnishingDB::OnCancel()
{
	CWaitCursor wc;
	m_pFurnishishManger->loadDatabase(m_pFurnishishManger->m_pAirportDB);
	CDialog::OnCancel();
}

void CDlgAircraftFurnishingDB::DisplayBmpInCtrl(HBITMAP hBmp,UINT nID)
{
	if (hBmp != NULL)
	{
		CBitmap bmpObj;
		bmpObj.Attach(hBmp);
		CWnd *pWnd = GetDlgItem(nID);
		CDC *pDC = pWnd->GetDC();

		CDC DCCompatible;
		DCCompatible.CreateCompatibleDC(pDC);
		CWnd::DefWindowProc(WM_PAINT, (WPARAM)DCCompatible.m_hDC , 0);

		CRect rect;
		pWnd->GetClientRect(rect);
		CBitmap *pOldBmp = DCCompatible.SelectObject(&bmpObj);
		BITMAP bmpInfo;
		bmpObj.GetObject(sizeof(bmpInfo),&bmpInfo);

		pDC->SetStretchBltMode(STRETCH_HALFTONE);
		pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), &DCCompatible, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY );
		DCCompatible.SelectObject(pOldBmp);
		bmpObj.Detach();
		return;
	}
	DisplayEmptyBmp(nID);
}

void CDlgAircraftFurnishingDB::DisplayAllBmpInCtrl()
{
	int nSel = GetCurSelIndex();
	if (nSel != LB_ERR)
	{
		CAircraftFurnishingModel* pFurnish = (CAircraftFurnishingModel*)m_wndListCtrl.GetItemData(nSel);
		if (pFurnish)
		{
			DisplayBmpInCtrl(m_hBmp[0],IDC_MODEL_VIEW_Z);
			DisplayBmpInCtrl(m_hBmp[1],IDC_MODEL_VIEW_Y);
			DisplayBmpInCtrl(m_hBmp[2],IDC_STATIC_FRONT_X);
			DisplayBmpInCtrl(m_hBmp[3],IDC_STATIC_ISOMETRIC_FURNISH);

			GetDlgItem(IDC_MODEL_VIEW_Z)->Invalidate(FALSE);
			GetDlgItem(IDC_MODEL_VIEW_Y)->Invalidate(FALSE);
			GetDlgItem(IDC_STATIC_FRONT_X)->Invalidate(FALSE);
			GetDlgItem(IDC_STATIC_ISOMETRIC_FURNISH)->Invalidate(FALSE);	
			return;
		}
	}

	DisplayBmpInCtrl(NULL,IDC_MODEL_VIEW_Z);
	DisplayBmpInCtrl(NULL,IDC_MODEL_VIEW_Y);
	DisplayBmpInCtrl(NULL,IDC_STATIC_FRONT_X);
	DisplayBmpInCtrl(NULL,IDC_STATIC_ISOMETRIC_FURNISH);

	GetDlgItem(IDC_MODEL_VIEW_Z)->Invalidate(FALSE);
	GetDlgItem(IDC_MODEL_VIEW_Y)->Invalidate(FALSE);
	GetDlgItem(IDC_STATIC_FRONT_X)->Invalidate(FALSE);
	GetDlgItem(IDC_STATIC_ISOMETRIC_FURNISH)->Invalidate(FALSE);

}

void CDlgAircraftFurnishingDB::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType,cx,cy);
    
	if (m_wndListCtrl.m_hWnd != NULL)
	{
		
		CRect rect;
		CRect rc;
		GetDlgItem(IDC_STATIC_COMPONENTS)->SetWindowPos(NULL,10,11,cx-2*10,16,NULL);
		GetDlgItem(IDC_STATIC_COMPONENTS)->GetWindowRect(&rect);
		ScreenToClient(rect);

		m_wndACTypeModel.SetWindowPos(NULL,rect.left+1,rect.bottom,cx-22,26,NULL);
		m_wndACTypeModel.GetWindowRect(&rect);
		ScreenToClient(rect);

		m_wndListCtrl.SetWindowPos(NULL,rect.left+1,rect.bottom + 2,cx-2*rect.left-4,cy/2-rect.bottom - 20,NULL);
		m_wndListCtrl.GetWindowRect(&rc);
		ScreenToClient(&rc);

		GetDlgItem(IDC_STATIC_FURNISH)->SetWindowPos(NULL,rc.left+1,rc.bottom + 10,cx - 2*rect.left-4,cy- rc.bottom -55,NULL);
		GetDlgItem(IDC_STATIC_FURNISH)->GetWindowRect(&rc);
		ScreenToClient(rc);

		GetDlgItem(IDC_STATIC_Z)->SetWindowPos(NULL,rc.left+2,rc.top + 20,rc.Width()/4 -2,16,NULL);
		GetDlgItem(IDC_STATIC_Y)->SetWindowPos(NULL,rc.left+rc.Width()/4,rc.top+20,rc.Width()/4,16,NULL);
		GetDlgItem(IDC_STATIC_X)->SetWindowPos(NULL,rc.left+rc.Width()/2,rc.top+20,rc.Width()/4,16,NULL);
		GetDlgItem(IDC_STATIC_ISO)->SetWindowPos(NULL,rc.right-rc.Width()/4,rc.top+20,rc.Width()/4,16,NULL);

		CRect rcView;
		GetDlgItem(IDC_MODEL_VIEW_Z)->SetWindowPos(NULL,rc.left,rc.top+38,rc.Width()/4,rc.Height()-38,NULL);
		GetDlgItem(IDC_MODEL_VIEW_Z)->GetWindowRect(&rcView);
		ScreenToClient(&rcView);
		GetDlgItem(IDC_MODEL_VIEW_Y)->SetWindowPos(NULL,rcView.right,rc.top+38,rc.Width()/4,rc.Height()-38,NULL);
		GetDlgItem(IDC_MODEL_VIEW_Y)->GetWindowRect(&rcView);
		ScreenToClient(&rcView);
		GetDlgItem(IDC_STATIC_FRONT_X)->SetWindowPos(NULL,rcView.right,rc.top+38,rc.Width()/4,rc.Height()-38,NULL);
		GetDlgItem(IDC_STATIC_FRONT_X)->GetWindowRect(&rcView);
		ScreenToClient(&rcView);
		GetDlgItem(IDC_STATIC_ISOMETRIC_FURNISH)->SetWindowPos(NULL,rcView.right,rc.top+38,rc.right-rcView.right,rc.Height()-38,NULL);

		CRect rcBtn;
		GetDlgItem(IDCANCEL)->GetWindowRect(&rcBtn);
		GetDlgItem(IDCANCEL)->SetWindowPos(NULL,rc.right - rcBtn.Width() - 2,rc.bottom + 8,rcBtn.Width(),rcBtn.Height(),NULL);
		GetDlgItem(IDCANCEL)->GetWindowRect(&rcBtn);
		ScreenToClient(rcBtn);
		GetDlgItem(IDOK)->SetWindowPos(NULL,rcBtn.left - rcBtn.Width() - 16,rc.bottom + 8,rcBtn.Width(),rcBtn.Height(), NULL);
		GetDlgItem(IDOK)->GetWindowRect(&rcBtn);
		ScreenToClient(rcBtn);
	}
}

BOOL CDlgAircraftFurnishingDB::OnEraseBkgnd(CDC*  pDC)     
{   
	return TRUE;   
} 

void CDlgAircraftFurnishingDB::LoadBmp( CModel* pModel )
{
	ClearBmps(true);
	if(!pModel->MakeSureFileOpen())
		return;

	CString sBmpFile = pModel->msTmpWorkDir + STR_THUMBFILE_X;			
	m_hBmp[0] = BitmapSaver::FromFile(sBmpFile);	
	sBmpFile = pModel->msTmpWorkDir + STR_THUMBFILE_Y;
	m_hBmp[1] = BitmapSaver::FromFile(sBmpFile);	

	sBmpFile = pModel->msTmpWorkDir + STR_THUMBFILE_Z;
	m_hBmp[2] = BitmapSaver::FromFile(sBmpFile);	

	sBmpFile = pModel->msTmpWorkDir + STR_THUMBFILE_P;
	m_hBmp[3] = BitmapSaver::FromFile(sBmpFile);
}

void CDlgAircraftFurnishingDB::ClearBmps( bool bDelete )
{
	if(bDelete)
	{
		for(int i=0;i<4;i++)
			::DeleteObject(m_hBmp[i]);
	}
	for(int i=0;i<4;i++)
		m_hBmp[i] = NULL;
}

CDlgAircraftFurnishingDB::~CDlgAircraftFurnishingDB()
{
	ClearBmps(true);
}

void CDlgAircraftFurnishingDB::DisplayEmptyBmp( UINT nID )
{
	// bkcolor
	CRect rcClient;
	CWnd *pWnd = GetDlgItem(nID);
	CDC *dc = pWnd->GetDC();
	pWnd->GetClientRect(&rcClient);

	CBrush pOrgBrush(RGB(255,255,255));
	dc->FillRect(rcClient,&pOrgBrush);
}
// CDlgAircraftFurnishingDB message handlers
void CDlgAircraftFurnishingDB::OnRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint cliPt;
	GetCursorPos(&cliPt);
	CPoint point = cliPt;
	m_wndListCtrl.ScreenToClient(&cliPt);
	int index;
	if((index = m_wndListCtrl.HitTestEx(cliPt, NULL)) != -1)
	{
		CMenu menu;
		menu.LoadMenu(IDR_MENU_COMPONENT);
		menu.GetSubMenu(0)->ModifyMenu(ID_EDIT_COMPONENT, MF_BYCOMMAND | MF_STRING, ID_EDIT_COMPONENT, _T("Edit aircraft furnishing"));
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON,point.x,point.y,this);
	}
}

