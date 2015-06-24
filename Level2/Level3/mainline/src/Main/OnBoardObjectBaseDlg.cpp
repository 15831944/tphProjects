// OnBoardObjectBaseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OnBoardObjectBaseDlg.h"
#include "termplan.h"
#include "../Common/WinMsg.h"
#include "./ChildFrm.h"
#include "ViewMsg.h"
#include "TermPlanDoc.h"
//#include "TermPlan.h"
#include "InputOnBoard/AircraftElmentShapeDisplay.h"
#include "MainFrm.h"
//#include "3DView.h"
#include "OnBoard/OnboardViewMsg.h"
#include "onboard/AircraftLayOutFrame.h"
#include "onboard/AircraftLayout3DView.h"
#include <inputonboard/Seat.h>
#include <inputonboard/Door.h>
#include <inputonboard/Storage.h>
#include <inputonboard/OnBoardBaseVerticalDevice.h>
#include "InputOnBoard/OnBoardWall.h"
#include "InputOnBoard/OnboardSurface.h"
#include "InputOnBoard/OnboardPortal.h"
#include <inputonboard/OnboardGalley.h>
#include <InputOnBoard/EmergencyExit.h>
#include "InputOnBoard/OnboardCorridor.h"

// COnBoardObjectBaseDlg dialog

IMPLEMENT_DYNAMIC(COnBoardObjectBaseDlg, CXTResizeDialog)
COnBoardObjectBaseDlg::COnBoardObjectBaseDlg(CAircraftElement *pAircraftElement,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(COnBoardObjectBaseDlg::IDD, pParent)
{
	m_pObject = pAircraftElement;
	m_hRClickItem = NULL;

	m_bPropModified = false;
	m_bNameModified = false;
	m_bPathModified = false;
	m_bTypeModify = false;
//	m_bNewOrUpdateObject = true;
}

COnBoardObjectBaseDlg::COnBoardObjectBaseDlg()
{
	m_pObject = NULL;
}
COnBoardObjectBaseDlg::~COnBoardObjectBaseDlg()
{
}

void COnBoardObjectBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LEVEL1, m_cmbName1);
	DDX_Control(pDX, IDC_COMBO_LEVEL2, m_cmbName2);
	DDX_Control(pDX, IDC_COMBO_LEVEL3, m_cmbName3);
	DDX_Control(pDX, IDC_COMBO_LEVEL4, m_cmbName4);
	DDX_Control(pDX, IDC_TREE_PROPERTY, m_treeProp);
	DDX_Control(pDX, IDC_OBJECT_IMAGE, m_ImageStatic);
}


BEGIN_MESSAGE_MAP(COnBoardObjectBaseDlg, CXTResizeDialog)
	ON_CBN_KILLFOCUS(IDC_COMBO_LEVEL1, OnCbnKillfocusComboNameLevel1)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL1, OnCbnDropdownComboNameLevel1)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL2, OnCbnDropdownComboNameLevel2)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL3, OnCbnDropdownComboNameLevel3)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL4, OnCbnDropdownComboNameLevel4)
	ON_MESSAGE(WM_OBJECTTREE_DOUBLECLICK, OnPropTreeDoubleClick)
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_COMMAND(ID_PROCPROP_PICKFROMMAP, OnProcpropPickfrommap)
	ON_MESSAGE(TP_ONBOARD_DATABACK, OnTempFallbackFinished)
	ON_COMMAND(ID_PROCPROP_DELETE, OnProcpropDelete)
	ON_COMMAND(ID_PROCPROP_TOGGLEDQUEUEFIXED, OnToggleQueueFixed)
	ON_COMMAND(ID_PROCPROP_DEFINE_Z_POS, OnDefineZPos)
END_MESSAGE_MAP()


// COnBoardObjectBaseDlg message handlers

void COnBoardObjectBaseDlg::OnCbnKillfocusComboNameLevel1()
{
	// TODO: Add your control notification handler code here
}

void COnBoardObjectBaseDlg::OnCbnDropdownComboNameLevel1()
{

}

void COnBoardObjectBaseDlg::OnCbnDropdownComboNameLevel2()
{	
	//m_cmbName2.ResetContent();
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

void COnBoardObjectBaseDlg::OnCbnDropdownComboNameLevel3()
{
	//m_cmbName3.ResetContent();

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

void COnBoardObjectBaseDlg::OnCbnDropdownComboNameLevel4()
{
	//m_cmbName4.ResetContent();

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

BOOL COnBoardObjectBaseDlg::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	try
	{		
		CAircraftElement::GetObjectNameList(m_lstExistObjectName);
	}
	catch (CADOException &e)
	{
		e.ErrorMessage();
		MessageBox(_T("Cann't read the Object."));
	}

	GetDlgItem(IDC_STATIC_PROP)->ShowWindow(SW_HIDE);

	InitComboBox();
	InitTypeComboBox();
	

	SetResize(IDC_STATIC_NAME, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_COMBO_LEVEL1, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_LINE1, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_COMBO_LEVEL2, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_LINE2, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_COMBO_LEVEL3, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_LINE3, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_COMBO_LEVEL4, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_PROPER, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_PROP, SZ_TOP_CENTER, SZ_TOP_CENTER);
	SetResize(IDC_LIST_PROP, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_TREE_PROPERTY, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_OBJECT_IMAGE, SZ_TOP_RIGHT, SZ_TOP_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_ROW, SZ_TOP_LEFT, SZ_TOP_RIGHT);
	SetResize(IDC_STATIC_COLUMN, SZ_TOP_LEFT, SZ_TOP_RIGHT);
	SetResize(IDC_STATIC_TYPE, SZ_TOP_LEFT, SZ_TOP_RIGHT);
	SetResize(IDC_COMBO_TYPE, SZ_TOP_LEFT, SZ_TOP_RIGHT);
	CRect rect;
	GetClientRect(rect);

	AdjustCombox(rect.right - rect.left);	

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void COnBoardObjectBaseDlg::InitComboBox()
{
	m_cmbName1.ResetContent();
	m_cmbName2.ResetContent();
	m_cmbName3.ResetContent();
	m_cmbName4.ResetContent();

	if (m_pObject != NULL)
	{
		ALTObjectID objName = m_pObject->GetIDName();
		m_cmbName1.SetWindowText(objName.at(0).c_str());
		m_cmbName2.SetWindowText(objName.at(1).c_str());
		m_cmbName3.SetWindowText(objName.at(2).c_str());
		m_cmbName4.SetWindowText(objName.at(3).c_str());
	}	
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

LRESULT COnBoardObjectBaseDlg::OnPropTreeDoubleClick(WPARAM wParam, LPARAM lParam)
{
	HTREEITEM hTreeItem = (HTREEITEM)lParam;
	OnDoubleClickPropTree(hTreeItem);
	return 0;
}

void COnBoardObjectBaseDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{

}
void COnBoardObjectBaseDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
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

void COnBoardObjectBaseDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{


}
LRESULT COnBoardObjectBaseDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void COnBoardObjectBaseDlg::OnProcpropPickfrommap() 
{
	CWaitCursor wc;
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	CAircraftLayOutFrame* layoutFrame = NULL ;
	layoutFrame = (CAircraftLayOutFrame*)pDoc->GetMainFrame()->CreateOrActivateFrame(theApp.m_pAircraftLayoutTermplate, RUNTIME_CLASS(CAircraftLayout3DView));
	wc.Restore();

	ASSERT(layoutFrame->m_LayoutView);
	HideDialog(  pDoc->GetMainFrame() );
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();
	FallbackReason enumReason = GetFallBackReason();

	if( !layoutFrame->m_LayoutView->SendMessage( TP_ONBOARDLAYOUT_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}
}

void COnBoardObjectBaseDlg::HideDialog(CWnd* parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void COnBoardObjectBaseDlg::ShowDialog(CWnd* parentWnd)
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
LRESULT COnBoardObjectBaseDlg::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_SHOW);	
	EnableWindow();
	DoTempFallBackFinished(wParam,lParam);
	UpdateTempObjectInfo();
	return true;
}
void COnBoardObjectBaseDlg::OnProcpropDelete() 
{
	OnDeletePath();
}

void COnBoardObjectBaseDlg::OnOK()
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

//CXTResizeDialog * COnBoardObjectBaseDlg::NewObjectDlg(int nObjID,ALTOBJECT_TYPE objType, int nParentID,int nAirportID,int nProjID, CWnd *pParent /* = NULL */)
//{
//	CXTResizeDialog *pDlg = NULL;
//
//	return pDlg;
//}


void COnBoardObjectBaseDlg::UpdateTempObjectInfo()
{
	//CAirside3D * pAirside3D = NULL;

	//if(GetDocument() && GetDocument()->Get3DViewParent() )
	//{
	//	pAirside3D = GetDocument()->Get3DViewParent()->GetAirside3D();
	//}
	//if(pAirside3D)
	//{
	//	ALTObject3D * pObj3D = pAirside3D->GetObject3D(m_pObject->getID());
	//	if(pObj3D)
	//	{
	//		pObj3D->DoSync();
	//	}
	//	else
	//	{
	//		pObj3D = pAirside3D->AddObject(GetObject());
	//		pObj3D->DoSync();
	//	}
	//	//if(pObj3D)
	//	//	pObj3D->SetInEdit(false);
	//}

}

CTermPlanDoc * COnBoardObjectBaseDlg::GetDocument()
{
	return (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
}

void COnBoardObjectBaseDlg::DoneEditTempObject()
{
	//GetDocument()->SetTempObject(NULL);
}

void COnBoardObjectBaseDlg::OnCancel()
{
	DoneEditTempObject();
	CXTResizeDialog::OnCancel();
}

void COnBoardObjectBaseDlg::AdjustCombox(int nWidth)
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
	if (NULL != GetDlgItem(IDC_STATIC_ROW))
	{
		CRect rect;
		m_cmbName1.GetWindowRect(&rect);
		ScreenToClient( &rect );
		GetDlgItem(IDC_STATIC_ROW)->MoveWindow(rect.left+2*iLevelWidth+18+iLevelWidth/4, 10, iLevelWidth,12 );

	}	

	if (NULL != GetDlgItem(IDC_STATIC_COLUMN))
	{
		CRect rect;
		m_cmbName1.GetWindowRect(&rect);
		ScreenToClient( &rect );
		//m_cmbName4.MoveWindow(rect.left+3*iLevelWidth+27+iLevelWidth/2, 25, iLevelWidth,12 );

		GetDlgItem(IDC_STATIC_COLUMN)->MoveWindow(rect.left+3*iLevelWidth+27+iLevelWidth/4, 10, iLevelWidth,12 );

	}


}

void COnBoardObjectBaseDlg::OnSize(UINT nType, int cx, int cy) 
{
	CXTResizeDialog::OnSize(nType, cx, cy);
	AdjustCombox(cx);	
}

bool COnBoardObjectBaseDlg::DoTempFallBackFinished( WPARAM wParam, LPARAM lParam )
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
	SetObjectPath(path);	
	delete []pointList;

	SetObjectPath(ARCPath3(*pData));
	return true;
}

void COnBoardObjectBaseDlg::UpdateObjectViews()
{
	GetDocument()->UpdateAllViews(NULL,VM_ONBOARD_UPDATE_AIRCRAFTELEMENT, (CObject*)m_pObject);
}

void COnBoardObjectBaseDlg::UpdateAddandRemoveObject()
{
	GetDocument()->UpdateAllViews(NULL,VM_ONBOARD_ADD_AIRCRAFTELEMENT, (CObject*)m_pObject);
}

void COnBoardObjectBaseDlg::UpdateNewViews()
{
	GetDocument()->UpdateAllViews(NULL,NULL,NULL);
}

bool COnBoardObjectBaseDlg::SetObjectName()
{
	CString strName;
	m_cmbName1.GetWindowText(strName);
	strName.Trim();
	if (strName.IsEmpty())
	{
		MessageBox("please input object name.");
		return  false;
	}
	m_bNameModified = true;
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
	if(m_pObject->GetIDName().m_val == m_objName.m_val)
	{
		m_bNameModified = false;
	}


	m_pObject->SetIDName(m_objName);
	return true;
}

void COnBoardObjectBaseDlg::OnDeletePath()
{
	ASSERT(0);
}

void COnBoardObjectBaseDlg::ToggleQueueFixed()
{
	ASSERT(0);
}

void COnBoardObjectBaseDlg::DefineZPos()
{
	ASSERT(0);
}

void COnBoardObjectBaseDlg::OnToggleQueueFixed()
{
	ToggleQueueFixed();
}

void COnBoardObjectBaseDlg::OnDefineZPos()
{
	DefineZPos();
}


void COnBoardObjectBaseDlg::InitTypeComboBox()
{
	//init type combo box
	CComboBox *pCombo = (CComboBox *)GetDlgItem(IDC_COMBO_TYPE);
	CString objType = _T("");
	if(m_pObject){
		objType = m_pObject->GetType();
	}
	int nSel = 0;


	if(pCombo)
	{
		CString typeString; int nIdx; 

#define ADDTYPESTRING(T)  typeString = T::TypeString; \
						  nIdx = pCombo->AddString(typeString);\
						  if(objType == typeString){ nSel = nIdx; }
		
		ADDTYPESTRING(CSeat)
		ADDTYPESTRING(CStorage)	
		ADDTYPESTRING(CDoor)
		ADDTYPESTRING(COnboardGalley)
		ADDTYPESTRING(CEmergencyExit)
		ADDTYPESTRING(COnBoardStair)	
		ADDTYPESTRING(COnBoardEscalator)
		ADDTYPESTRING(COnBoardElevator)
		ADDTYPESTRING(COnBoardWall)
		ADDTYPESTRING(COnboardSurface)
		ADDTYPESTRING(COnboardPortal)		
		ADDTYPESTRING(COnboardCorridor)	
		pCombo->SetCurSel(nSel);
	}
}
