#include "StdAfx.h"
#include "Resource.h"
#include "TermPlanDoc.h"
#include ".\arclayoutobjectdlg.h"
#include "common/WinMsg.h"
#include "./MainFrm.h"
#include "ARCportLayoutEditor.h"
IMPLEMENT_DYNAMIC(CARCLayoutObjectDlg, CXTResizeDialog)
CARCLayoutObjectDlg::CARCLayoutObjectDlg(LandsideFacilityLayoutObject* pObj,CDlgCallBack* pCallback,CWnd* pParent /*=NULL*/,QueryData* pData )
: CXTResizeDialog(CARCLayoutObjectDlg::IDD, pParent)
{		
	m_pImpl = ILayoutObjectPropDlgImpl::Create(pObj,this,pData);
	m_pCallBack = pCallback;
}

CARCLayoutObjectDlg::~CARCLayoutObjectDlg()
{
	delete m_pImpl;
}

void CARCLayoutObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LEVEL1, m_cmbName1);
	DDX_Control(pDX, IDC_COMBO_LEVEL2, m_cmbName2);
	DDX_Control(pDX, IDC_COMBO_LEVEL3, m_cmbName3);
	DDX_Control(pDX, IDC_COMBO_LEVEL4, m_cmbName4);	
	DDX_Control(pDX, IDC_OBJECT_IMAGE, m_ImageStatic);
	DDX_Control(pDX, IDC_CHECK_LOCKED, m_btnCheckLocked);
	DDX_Control(pDX, IDC_TREE_PROPERTY, m_treeProp);
	DDX_Control(pDX, IDC_CHECK_CELLPHONE, m_btnCheckCellPhone);
}


BEGIN_MESSAGE_MAP(CARCLayoutObjectDlg, CXTResizeDialog)
	ON_CBN_KILLFOCUS(IDC_COMBO_LEVEL1, OnCbnKillfocusComboNameLevel1)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL1, OnCbnDropdownComboNameLevel1)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL2, OnCbnDropdownComboNameLevel2)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL3, OnCbnDropdownComboNameLevel3)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL4, OnCbnDropdownComboNameLevel4)

	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_PROCPROP_PICKFROMMAP, OnProcpropPickfrommap)
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
	ON_COMMAND(ID_PROCPROP_DELETE, OnProcpropDelete)
	ON_COMMAND(ID_STRETCHCONTROLPOINTS_REVERSE, OnControlPathRevese)
	ON_COMMAND(ID_MENU_CHPROFILE,HandleChangeVerticalProfile)
	ON_BN_CLICKED(IDC_CHECK_LOCKED, OnBnClickedCheckLocked)
	ON_MESSAGE(WM_OBJECTTREE_DOUBLECLICK, OnPropTreeDoubleClick)	
	//ON_MESSAGE(WM_OBJECTTREE_LBUTTONDOWN, OnPropTreeLButtonDown)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROPERTY, OnSelChanged)
END_MESSAGE_MAP()






void CARCLayoutObjectDlg::OnCbnKillfocusComboNameLevel1()
{
	// TODO: Add your control notification handler code here
}

void CARCLayoutObjectDlg::OnCbnDropdownComboNameLevel1()
{

}

BOOL CARCLayoutObjectDlg::PreTranslateMessage(MSG* pMsg)     
{   
	return CXTResizeDialog::PreTranslateMessage(pMsg);   
}   

void CARCLayoutObjectDlg::OnCbnDropdownComboNameLevel2()
{	
	m_cmbName2.ResetContent();
	SortedStringList levelName;	

	CString csLevel1;
	m_cmbName1.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;

	m_pImpl->getListObjectName().GetLevel2StringList(csLevel1,levelName);

	SortedStringList::iterator iter = levelName.begin() ;
	for (; iter != levelName.end(); ++iter)
	{
		m_cmbName2.AddString(*iter);
	}
}

void CARCLayoutObjectDlg::OnCbnDropdownComboNameLevel3()
{
	m_cmbName3.ResetContent();

	CString csLevel1;
	m_cmbName1.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;
	CString csLevel2;
	m_cmbName2.GetWindowText( csLevel2 );
	if( csLevel2.IsEmpty() )
		return;	
	SortedStringList levelName;	
	m_pImpl->getListObjectName().GetLevel3StringList(csLevel1,csLevel2,levelName);

	SortedStringList::iterator iter = levelName.begin() ;
	for (; iter != levelName.end(); ++iter)
	{
		m_cmbName3.AddString(*iter);
	}
}

void CARCLayoutObjectDlg::OnCbnDropdownComboNameLevel4()
{
	m_cmbName4.ResetContent();

	CString csLevel1;
	m_cmbName1.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;
	CString csLevel2;
	m_cmbName2.GetWindowText( csLevel2 );
	if( csLevel2.IsEmpty() )
		return;
	CString csLevel3;
	m_cmbName3.GetWindowText( csLevel3 );
	if( csLevel3.IsEmpty() )
		return;
	SortedStringList levelName;	
	m_pImpl->getListObjectName().GetLevel4StringList(csLevel1,csLevel2,csLevel3,levelName);

	SortedStringList::iterator iter = levelName.begin() ;
	for (; iter != levelName.end(); ++iter)
	{
		m_cmbName4.AddString(*iter);
	}
}

BOOL CARCLayoutObjectDlg::OnInitDialog()
{
	if(!m_pImpl)
		return FALSE;	

	CXTResizeDialog::OnInitDialog();

	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(m_pImpl->getBitmapResourceID()));
	
	ASSERT(bitmap);
	m_ImageStatic.SetBitmap(bitmap);

	CXTResizeDialog::SetWindowText( m_pImpl->getTitle() );
 
	InitComboBox();

	CRect rect;
	GetClientRect(rect);
	AdjustCombox(rect.right - rect.left);	

	GetDlgItem(IDC_STATIC_PROP)->ShowWindow(SW_HIDE);

	InitUnitPiece();

	m_pImpl->LoadTree();
	
	m_btnCheckLocked.SetCheck(m_pImpl->IsObjectLocked());


	SetResize(IDC_STATIC, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_COMBO_LEVEL1, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_LINE1, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_COMBO_LEVEL2, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_LINE2, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_COMBO_LEVEL3, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_LINE3, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_COMBO_LEVEL4, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_PROPER, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_PROP, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LIST_PROP, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_TREE_PROPERTY, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_OBJECT_IMAGE, SZ_TOP_RIGHT, SZ_TOP_RIGHT);
	SetResize(IDC_CHECK_LOCKED, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_CHECK_CELLPHONE, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);


	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	if (m_pImpl && m_pImpl->ChangeFocus())
	{
		return FALSE;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CARCLayoutObjectDlg::InitComboBox()
{
	m_cmbName1.ResetContent();
	m_cmbName2.ResetContent();
	m_cmbName3.ResetContent();
	m_cmbName4.ResetContent();
	//level 1
	SortedStringList levelName;	
	m_pImpl->getListObjectName().GetLevel1StringList(levelName);

	SortedStringList::iterator iter = levelName.begin() ;
	for (; iter != levelName.end(); ++iter)
	{
		m_cmbName1.AddString(*iter);
	}
	levelName.clear();

	ALTObjectID& objName = m_pImpl->GetObjectName();
	m_cmbName1.SetWindowText(objName.at(0).c_str());
	m_cmbName2.SetWindowText(objName.at(1).c_str());
	m_cmbName3.SetWindowText(objName.at(2).c_str());
	m_cmbName4.SetWindowText(objName.at(3).c_str());
}

LRESULT CARCLayoutObjectDlg::OnPropTreeDoubleClick(WPARAM wParam, LPARAM lParam)
{
	HTREEITEM hTreeItem = (HTREEITEM)lParam;
	m_pImpl->OnDoubleClickPropTree(hTreeItem);
	return 0;
}

//LRESULT CARCLayoutObjectDlg::OnPropTreeLButtonDown(WPARAM wParam, LPARAM lParam)
//{
//	HTREEITEM hTreeItem = (HTREEITEM)lParam;
//	m_pImpl->OnLButtonDownPropTree(hTreeItem);
//	return 0;
//}

void CARCLayoutObjectDlg::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hTreeItem = NULL;
	hTreeItem = pNMTreeView->itemNew.hItem;
	if (hTreeItem == NULL) return;

	m_pImpl->OnSelChangedPropTree(hTreeItem);
	return;
}

void CARCLayoutObjectDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	
	CRect rectTree;
	GetTreeCtrl().GetWindowRect( &rectTree );
	if( !rectTree.PtInRect(point) ) 
		return;
	
	m_pImpl->SetRClickItem(NULL);
	GetTreeCtrl().SetFocus();
	CPoint pt = point;
	GetTreeCtrl().ScreenToClient( &pt );
	UINT iRet;
	
	HTREEITEM m_hRClickItem = GetTreeCtrl().HitTest( pt, &iRet );
	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
	{
		m_hRClickItem = NULL;
	}
	m_pImpl->SetRClickItem(m_hRClickItem);

	if (m_hRClickItem==NULL)
		return;

	CMenu menuPopup;
	menuPopup.LoadMenu( IDR_MENU_POPUP );
	CMenu* pMenu=NULL;

	m_pImpl->OnContextMenu(menuPopup,pMenu);
	m_pImpl->CreateChangeVerticalProfileMenu(pMenu);

	if (pMenu)
		pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

}


LRESULT CARCLayoutObjectDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_pImpl)
		m_pImpl->OnDefWindowProc(message,wParam,lParam);

	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CARCLayoutObjectDlg::OnProcpropPickfrommap() 
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();

	CMDIChildWnd* pWnd = pDoc->GetMainFrame()->CreateOrActiveRender3DView();
	if(!pWnd)
		return;

	HideDialog( pDoc->GetMainFrame() );
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();

	enum FallbackReason enumReason = m_pImpl->GetFallBackReason();	
	m_pImpl->GetFallBackReasonData(data);

	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}
}

void CARCLayoutObjectDlg::HideDialog(CWnd* parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CARCLayoutObjectDlg::ShowDialog(CWnd* parentWnd)
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
LRESULT CARCLayoutObjectDlg::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_SHOW);	
	EnableWindow();
	m_pImpl->DoFallBackFinished(wParam,lParam);
	//UpdateTempObjectInfo();
	return true;
}
void CARCLayoutObjectDlg::OnProcpropDelete() 
{
	m_pImpl->OnPropDelete();
}

void CARCLayoutObjectDlg::OnOK()
{
	//	ALTOBJECT_NAME objName;
	CString strName;
	m_cmbName1.GetWindowText(strName);
	strName.Trim();
	if (strName.IsEmpty())
	{
		MessageBox("please input object name.");
		return ;
	}
	ALTObjectID m_objName;
	m_objName.m_val[0] = strName.MakeUpper();

	m_cmbName2.GetWindowText(strName);
	strName.Trim();
	m_objName.m_val[1] = strName.MakeUpper();

	m_cmbName3.GetWindowText(strName);
	strName.Trim();
	m_objName.m_val[2] = strName.MakeUpper();

	m_cmbName4.GetWindowText(strName);
	strName.Trim();
	m_objName.m_val[3] = strName.MakeUpper();
	
	if(!m_pImpl->SetObjectName(m_objName))
	{		
		MessageBox("The ObjName is not valid,please re-input.");
		return ;		
	}
	CString sError = _T("Application Error,contact Arcterm for support.");
	if(!m_pImpl->OnOK(sError))
	{
		MessageBox(sError);
		return;
	}

	CDialog::OnOK(); 
	if(m_pCallBack)
	{ 
		m_pCallBack->OnOk(); 
		delete m_pCallBack;
		m_pCallBack = NULL;
	}

}




CTermPlanDoc * CARCLayoutObjectDlg::GetDocument()
{
	return (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
}



void CARCLayoutObjectDlg::OnCancel()
{	
	m_pImpl->OnCancel();
	CDialog::OnCancel();
	if(m_pCallBack)
	{
		m_pCallBack->OnCancel();
		delete m_pCallBack;
		m_pCallBack = NULL;
	}
}

void CARCLayoutObjectDlg::AdjustCombox(int nWidth)
{
	const int nIconWidth = 80;

	int iLevelWidth = (nWidth- 2*15 - 3*9 - nIconWidth)/4;

	if (NULL != GetDlgItem(IDC_COMBO_LEVEL1))
	{
		CRect rect;
		m_cmbName1.GetWindowRect(&rect);
		ScreenToClient( &rect );
		m_cmbName1.MoveWindow(rect.left,25,iLevelWidth,12);
	}	

	if (NULL != GetDlgItem(IDC_STATIC_LINE1))
	{
		CRect rect;
		m_cmbName1.GetWindowRect(&rect);
		ScreenToClient( &rect );
		GetDlgItem(IDC_STATIC_LINE1)->MoveWindow( rect.left+iLevelWidth+2,32,5,3 );
	}

	if (NULL != GetDlgItem(IDC_COMBO_LEVEL2))
	{
		CRect rect;
		m_cmbName1.GetWindowRect(&rect);
		ScreenToClient( &rect );
		m_cmbName2.MoveWindow(rect.left+iLevelWidth+9,25,iLevelWidth,12 );
	}	

	if (NULL != GetDlgItem(IDC_STATIC_LINE2))
	{
		CRect rect;
		m_cmbName1.GetWindowRect(&rect);
		ScreenToClient( &rect );
		GetDlgItem(IDC_STATIC_LINE2)->MoveWindow( rect.left+2*iLevelWidth+11,32,5,3 );
	}

	if (NULL != GetDlgItem(IDC_COMBO_LEVEL3))
	{
		CRect rect;
		m_cmbName1.GetWindowRect(&rect);
		ScreenToClient( &rect );
		m_cmbName3.MoveWindow(rect.left+2*iLevelWidth+18, 25, iLevelWidth,12 );
	}	

	if (NULL != GetDlgItem(IDC_STATIC_LINE3))
	{
		CRect rect;
		m_cmbName1.GetWindowRect(&rect);
		ScreenToClient( &rect );
		GetDlgItem(IDC_STATIC_LINE3)->MoveWindow( rect.left+3*iLevelWidth+20,32,5,3 );
	}	

	if (NULL != GetDlgItem(IDC_COMBO_LEVEL4))
	{
		CRect rect;
		m_cmbName1.GetWindowRect(&rect);
		ScreenToClient( &rect );
		m_cmbName4.MoveWindow(rect.left+3*iLevelWidth+27, 25, iLevelWidth,12 );
	}
}

void CARCLayoutObjectDlg::OnSize(UINT nType, int cx, int cy) 
{
	CXTResizeDialog::OnSize(nType, cx, cy);
	AdjustCombox(cx);	
}




void CARCLayoutObjectDlg::OnBnClickedCheckLocked()
{
	// TODO: Add your control notification handler code here
	m_pImpl->SetObjectLock(BST_CHECKED == m_btnCheckLocked.GetCheck());	
}

void CARCLayoutObjectDlg::InitUnitPiece()
{
	CUnitPiece::InitializeUnitPiece(GetProjectID(),UM_ID_1,this);
	CRect rectItem;
	GetDlgItem(IDOK)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int yPos = rectItem.top;
	GetTreeCtrl().GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int xPos = rectItem.left;
	CUnitPiece::MoveUnitButton(xPos,yPos);
	SetResize(XIAOHUABUTTON_ID, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);

	m_pImpl->InitUnitPiece(this);
	
	//CUnitPiece::SetUnitInUse(ARCUnit_Length_InUse);
}

bool CARCLayoutObjectDlg::ConvertUnitFromDBtoGUI( void )
{
	//it do nothing , so return (false);
	return (false);
}

bool CARCLayoutObjectDlg::RefreshGUI( void )
{
	m_pImpl->LoadTree();
	return (true);
}

bool CARCLayoutObjectDlg::ConvertUnitFromGUItoDB( void )
{
	//it do nothing , so return (false);
	return (false);
}

int CARCLayoutObjectDlg::GetProjectID()
{
	return GetDocument()->GetProjectID();
}

InputLandside* CARCLayoutObjectDlg::getInputLandside()
{
	return GetDocument()->getARCport()->m_pInputLandside;
}

void CARCLayoutObjectDlg::OnControlPathRevese()
{
	if(m_pImpl)
		m_pImpl->OnControlPathReverse();
}

void CARCLayoutObjectDlg::HandleChangeVerticalProfile()
{
	if(m_pImpl)
		m_pImpl->HandleChangeVerticalProfile();
}

CButton * CARCLayoutObjectDlg::GetCheckBoxCellPhone()
{
	return 	&m_btnCheckCellPhone;
}

void CARCLayoutObjectDlg::OnDestroy()
{
	CXTResizeDialog::OnDestroy();
	if(m_pCallBack)
	{
		m_pCallBack->OnCancel();
		delete m_pCallBack;
		m_pCallBack = NULL;
	}
}
