#include "StdAfx.h"
#include "resource.h"
#include ".\dlgonboardelementprop.h"
#include "../Common/WinMsg.h"
#include "TermPlanDoc.h"
#include "MainFrm.h"

IMPLEMENT_DYNAMIC(CDlgOnboardElementProp, CXTResizeDialog)
CDlgOnboardElementProp::CDlgOnboardElementProp(CAircraftElement* pElement,CWnd* pParent)
: CXTResizeDialog(CDlgOnboardElementProp::IDD, pParent)
{
	m_hRClickItem = NULL;
	m_bPropModified = false;
	m_bNameModified = false;
	m_bPathModified = false;
	m_bTypeModify = false;
}

CDlgOnboardElementProp::~CDlgOnboardElementProp( void )
{

}

void CDlgOnboardElementProp::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LEVEL1, m_cmbName1);
	DDX_Control(pDX, IDC_COMBO_LEVEL2, m_cmbName2);
	DDX_Control(pDX, IDC_COMBO_LEVEL3, m_cmbName3);
	DDX_Control(pDX, IDC_COMBO_LEVEL4, m_cmbName4);
	DDX_Control(pDX, IDC_TREE_PROPERTY, m_treeProp);
	DDX_Control(pDX, IDC_OBJECT_IMAGE, m_ImageStatic);
}


BEGIN_MESSAGE_MAP(CDlgOnboardElementProp, CXTResizeDialog)
	ON_CBN_KILLFOCUS(IDC_COMBO_LEVEL1, OnCbnKillfocusComboNameLevel1)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL1, OnCbnDropdownComboNameLevel1)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL2, OnCbnDropdownComboNameLevel2)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL3, OnCbnDropdownComboNameLevel3)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL4, OnCbnDropdownComboNameLevel4)
	ON_MESSAGE(WM_OBJECTTREE_DOUBLECLICK, OnPropTreeDoubleClick)
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_COMMAND(ID_PROCPROP_PICKFROMMAP, OnProcpropPickfrommap)
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
	ON_COMMAND(ID_PROCPROP_DELETE, OnProcpropDelete)
END_MESSAGE_MAP()


void CDlgOnboardElementProp::OnCbnDropdownComboNameLevel1()
{

}

void CDlgOnboardElementProp::OnCbnDropdownComboNameLevel2()
{	
	m_cmbName2.ResetContent();
	SortedStringList levelName;	

	CString csLevel1;
	m_cmbName1.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;

	m_lstExistObjectName.GetLevel2StringList(csLevel1,levelName);

	SortedStringList::iterator iter = levelName.begin() ;
	for (; iter != levelName.end(); ++iter)
	{
		m_cmbName2.AddString(*iter);
	}
}

void CDlgOnboardElementProp::OnCbnDropdownComboNameLevel3()
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
	m_lstExistObjectName.GetLevel3StringList(csLevel1,csLevel2,levelName);

	SortedStringList::iterator iter = levelName.begin() ;
	for (; iter != levelName.end(); ++iter)
	{
		m_cmbName3.AddString(*iter);
	}
}

void CDlgOnboardElementProp::OnCbnDropdownComboNameLevel4()
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
	m_lstExistObjectName.GetLevel4StringList(csLevel1,csLevel2,csLevel3,levelName);

	SortedStringList::iterator iter = levelName.begin() ;
	for (; iter != levelName.end(); ++iter)
	{
		m_cmbName4.AddString(*iter);
	}
}

BOOL CDlgOnboardElementProp::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	
	m_cmbName1.ResetContent();
	m_cmbName2.ResetContent();
	m_cmbName3.ResetContent();
	m_cmbName4.ResetContent();
	GetDlgItem(IDC_STATIC_PROP)->ShowWindow(SW_HIDE);

	/*if (m_pObject != NULL)
	{
		ALTObjectID objName;
		m_pObject->getObjName(objName);
		m_cmbName1.SetWindowText(objName.at(0).c_str());
		m_cmbName2.SetWindowText(objName.at(1).c_str());
		m_cmbName3.SetWindowText(objName.at(2).c_str());
		m_cmbName4.SetWindowText(objName.at(3).c_str());
	}	*/
	InitComboBox();

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
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	CRect rect;
	GetClientRect(rect);

	AdjustCombox(rect.right - rect.left);	

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOnboardElementProp::InitComboBox()
{
	//level 1
	SortedStringList levelName;	
	m_lstExistObjectName.GetLevel1StringList(levelName);

	SortedStringList::iterator iter = levelName.begin() ;
	for (; iter != levelName.end(); ++iter)
	{
		m_cmbName1.AddString(*iter);
	}
	levelName.clear();
}

LRESULT CDlgOnboardElementProp::OnPropTreeDoubleClick(WPARAM wParam, LPARAM lParam)
{
	HTREEITEM hTreeItem = (HTREEITEM)lParam;
	OnDoubleClickPropTree(hTreeItem);
	return 0;
}

void CDlgOnboardElementProp::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{

}

void CDlgOnboardElementProp::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	m_hRClickItem = NULL;

	CRect rectTree;
	m_treeProp.GetWindowRect( &rectTree );
	if( !rectTree.PtInRect(point) ) 
		return;

	m_treeProp.SetFocus();
	CPoint pt = point;
	m_treeProp.ScreenToClient( &pt );
	UINT iRet;
	m_hRClickItem = m_treeProp.HitTest( pt, &iRet );
	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
	{
		m_hRClickItem = NULL;
	}

	if (m_hRClickItem==NULL)
		return;

	CMenu menuPopup;
	menuPopup.LoadMenu( IDR_MENU_POPUP );
	CMenu* pMenu=NULL;

	OnContextMenu(menuPopup,pMenu);
	if (pMenu)
		pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

}
void CDlgOnboardElementProp::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{


}
LRESULT CDlgOnboardElementProp::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgOnboardElementProp::OnProcpropPickfrommap() 
{


	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();


	/*C3DView* p3DView = pDoc->Get3DView();
	if( p3DView == NULL )
	{
		if( EnvMode_AirSide == pDoc->m_systemMode )
			pDoc->GetMainFrame()->CreateOrActivateFrame(  theApp.m_p3DAirSideTemplate, RUNTIME_CLASS(C3DView) );
		else
			pDoc->GetMainFrame()->CreateOrActivateFrame(  theApp.m_p3DTemplate, RUNTIME_CLASS(C3DView) );
		p3DView = pDoc->Get3DView();
		}
	assert( p3DView );*/

	HideDialog( pDoc->GetMainFrame() );
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();

	enum FallbackReason enumReason = GetFallBackReason();


	//	enumReason= PICK_ONEPOINT;

	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}
}

void CDlgOnboardElementProp::HideDialog(CWnd* parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CDlgOnboardElementProp::ShowDialog(CWnd* parentWnd)
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
LRESULT CDlgOnboardElementProp::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_SHOW);	
	EnableWindow();
	DoTempFallBackFinished(wParam,lParam);
	UpdateTempObjectInfo();
	return true;
}
void CDlgOnboardElementProp::OnProcpropDelete() 
{

}

void CDlgOnboardElementProp::OnOK()
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

	DoneEditTempObject();
	CXTResizeDialog::OnOK();
}



CTermPlanDoc * CDlgOnboardElementProp::GetDocument()
{
	return (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
}

void CDlgOnboardElementProp::DoneEditTempObject()
{
	GetDocument()->SetTempObject(NULL);
}

void CDlgOnboardElementProp::OnCancel()
{
	DoneEditTempObject();
	CXTResizeDialog::OnCancel();
}

void CDlgOnboardElementProp::AdjustCombox(int nWidth)
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

void CDlgOnboardElementProp::OnSize(UINT nType, int cx, int cy) 
{
	CXTResizeDialog::OnSize(nType, cx, cy);
	AdjustCombox(cx);	
}

bool CDlgOnboardElementProp::DoTempFallBackFinished( WPARAM wParam, LPARAM lParam )
{
	std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);
	size_t nSize = pData->size();

	CPath2008 path;
	CPoint2008 *pointList = new CPoint2008[nSize];
	for (size_t i =0; i < nSize; ++i)
	{
		ARCVector3 v3 = pData->at(i);
		pointList[i].setPoint(v3[VX] ,v3[VY],v3[VZ]);
	}
	path.init(nSize,pointList);
	//SetObjectPath(path);	
	delete []pointList;

	return true;
}

void CDlgOnboardElementProp::OnCbnKillfocusComboNameLevel1()
{

}

void CDlgOnboardElementProp::UpdateTempObjectInfo()
{

}
