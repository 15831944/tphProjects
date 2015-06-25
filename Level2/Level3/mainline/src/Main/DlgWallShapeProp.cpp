// DlgWallShapeProp.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgWallShapeProp.h"

#include "TermPlanDoc.h"
#include "MainFrm.h"
#include <CommonData/Fallback.h>
#include "common\WinMsg.h"
#include "Floor2.h"
// 
#include <Inputs/WallShape.h>
#include <Inputs/WallShapeList.h>

// DlgWallShapeProp dialog

IMPLEMENT_DYNAMIC(DlgWallShapeProp, CXTResizeDialog)
DlgWallShapeProp::DlgWallShapeProp(WallShape * _pWall,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(DlgWallShapeProp::IDD, pParent)
{
	m_csLevel1 = _T("");
	m_csLevel2 = _T("");
	m_csLevel3 = _T("");
	m_csLevel4 = _T("");
	
	m_pWallShape = NULL;
	if (_pWall)
	{
		m_pWallShape = _pWall ;
	}
	m_dWidth=100;
	m_dheight=300;
	m_hWallPath = 0;
	m_hWidth = 0;
	m_hHeight = 0;
}

DlgWallShapeProp::~DlgWallShapeProp()
{
}

void DlgWallShapeProp::DoDataExchange(CDataExchange* pDX)
{
	
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_NAME_1, m_wndName1Combo);
	DDX_Control(pDX, IDC_COMBO_NAME_2, m_wndName2Combo);
	DDX_Control(pDX, IDC_COMBO_NAME_3, m_wndName3Combo);
	DDX_Control(pDX, IDC_COMBO_NAME_4, m_wndName4Combo);
	DDX_Control(pDX, IDC_TREE_PROPERTIES, m_wndPropTree);
	

	DDX_CBString(pDX, IDC_COMBO_NAME_1, m_csLevel1);
	DDX_CBString(pDX, IDC_COMBO_NAME_2, m_csLevel2);
	DDX_CBString(pDX, IDC_COMBO_NAME_3, m_csLevel3);
	DDX_CBString(pDX, IDC_COMBO_NAME_4, m_csLevel4);
}
BOOL DlgWallShapeProp::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	ASSERT(pDoc);

	SetResize(ID_WALLSHAPE_NAMEINCLUDE, SZ_TOP_LEFT, SZ_TOP_RIGHT);
	SetResize(IDC_COMBO_NAME_1, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_COMBO_NAME_2, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_COMBO_NAME_3, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_COMBO_NAME_4, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(ID_WALLSHAPE_PROINCLUDE, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_TREE_PROPERTIES, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	/////
	//initialize unit manager
	CUnitPiece::InitializeUnitPiece(pDoc->GetProjectID(),UM_ID_18,this);
	CRect rectItem;
	GetDlgItem(IDOK)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int yPos = rectItem.top;
	GetDlgItem(ID_WALLSHAPE_PROINCLUDE)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int xPos = rectItem.left;
	CUnitPiece::MoveUnitButton(xPos,yPos);
	SetResize(XIAOHUABUTTON_ID, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	CUnitPiece::SetUnitInUse(ARCUnit_Length_InUse);
	/////

	if(m_pWallShape)
	{
		m_wndName1Combo.ResetContent();
		m_wndName2Combo.ResetContent();
		m_wndName3Combo.ResetContent();
		m_wndName4Combo.ResetContent();

		//// level1 
		//StringList groups;
		//GetInputTerminal()->GetProcessorList()->getAllGroupNames( groups, -1 );
		//for( int i = 0; i < groups.getCount(); i++ )
		//	m_comboLevel1.AddString( groups.getString( i ) );


		m_wndName1Combo.EnableWindow();
		m_wndName2Combo.EnableWindow();
		m_wndName3Combo.EnableWindow();
		m_wndName4Combo.EnableWindow();

		const CStructureID pID = m_pWallShape->getID();
		if (!pID.isBlank())
		{	
			char szName[128];

			pID.getNameAtLevel( szName, 0 );
			m_wndName1Combo.SetWindowText( szName );
			szName[0] = 0;
			pID.getNameAtLevel( szName, 1 );
			m_wndName2Combo.SetWindowText( szName );
			szName[0] = 0;
			pID.getNameAtLevel( szName, 2 );
			m_wndName3Combo.SetWindowText( szName );
			szName[0] = 0;
			pID.getNameAtLevel( szName, 3 );
			m_wndName4Combo.SetWindowText( szName );
		}
		else
		{
			//New Item ,Load Old Catelog Infomation now
			StringList groups;
			pDoc->GetCurWallShapeList().getAllGroupNames( groups);
					
			for( int i = 0; i < groups.getCount(); i++ )
				m_wndName1Combo.AddString( groups.getString( i ) );
		}

/*		CString wallShapeName = m_pWallShape->getID();
		CString strToken;
		int curpos=0;
		CString strlevel[4];
		int nlevel=0;
		do
		{
			strToken=wallShapeName.Tokenize("-",curpos);
			strlevel[nlevel]=strToken;
			nlevel++;
		}
		while(strToken!=""&&nlevel<4);

		m_csLevel1=strlevel[0];m_csLevel2=strlevel[1];m_csLevel3=strlevel[2];m_csLevel4=strlevel[3];
*/		
		// service location 
		for(size_t i=0;i<m_pWallShape->GetPointCount();i++)
		{
			Point & p=m_pWallShape->GetPointAt(i);
			m_Wallpath.push_back(p);
		}
//		UpdateData(FALSE);

	


		LoadTree();		
	}

	return TRUE;
}

void DlgWallShapeProp::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	m_hRClickItem=NULL;
	CRect rectTree;
	m_wndPropTree.GetWindowRect( &rectTree );
	if( !rectTree.PtInRect(point) ) 
		return;

	m_wndPropTree.SetFocus();
	CPoint pt = point;
	m_wndPropTree.ScreenToClient( &pt );
	UINT iRet;
	m_hRClickItem = m_wndPropTree.HitTest( pt, &iRet );
	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
	{
		m_hRClickItem = NULL;
	}

	if (m_hRClickItem==NULL)
		return;
	
	CMenu menuPopup;
	menuPopup.LoadMenu( IDR_MENU_POPUP );
	CMenu* pMenu=NULL;
	
	if( m_hRClickItem == m_hWidth )
	{
		pMenu = menuPopup.GetSubMenu( 51 );
	}
	else if (m_hRClickItem == m_hHeight){
		pMenu = menuPopup.GetSubMenu( 50 );
	}else{
		pMenu = menuPopup.GetSubMenu( 5 );
	}
	

	pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

}

int DlgWallShapeProp:: DoFakeModal(){
	if( CXTResizeDialog::Create(DlgWallShapeProp::IDD, m_pParentWnd) ) 
	{
		CenterWindow();
		ShowWindow(SW_SHOW);
		GetParent()->EnableWindow(FALSE);
		EnableWindow();
		int nReturn = RunModalLoop();
		DestroyWindow();
		return nReturn;
	}
	else
		return IDCANCEL;
}
void DlgWallShapeProp::OnProcpropPickfrommap() 
{

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	C3DView* p3DView = pDoc->Get3DView();
	if( p3DView == NULL )
	{
		pDoc->GetMainFrame()->CreateOrActive3DView();
		p3DView = pDoc->Get3DView();
	}
	if (!p3DView)
	{
		ASSERT(FALSE);
		return;
	}

	HideDialog( pDoc->GetMainFrame() );
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();
	enum FallbackReason enumReason;
	enumReason = PICK_MANYPOINTS;

	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}

}
void DlgWallShapeProp::OnNMRclickTreeProperties(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hItem = m_wndPropTree.GetSelectedItem();
	if (m_wndPropTree.GetParentItem(hItem) == NULL 
		&& m_wndPropTree.GetPrevSiblingItem(hItem) == NULL)
	{

	}

	*pResult = 0;
}

void DlgWallShapeProp::HideDialog(CWnd* parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void DlgWallShapeProp::ShowDialog(CWnd* parentWnd)
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
LRESULT DlgWallShapeProp::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_SHOW);	
	EnableWindow();
	std::vector<Point>* pData = reinterpret_cast< std::vector<Point>* >(wParam);

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	

	m_Wallpath = *pData;			
	//pDoc->m_tempProcInfo.SetTempServicLocation(m_vServiceLocation);
	
	LoadTree();

	return TRUE;
}

void DlgWallShapeProp::LoadTree()
{
	m_wndPropTree.SetRedraw(FALSE);

	//delete all items
	if(m_wndPropTree.ItemHasChildren(m_hWallPath))
	{
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = m_wndPropTree.GetChildItem(m_hWallPath);

		while (hChildItem != NULL)
		{
			hNextItem = m_wndPropTree.GetNextItem(hChildItem, TVGN_NEXT);
			m_wndPropTree.DeleteItem(hChildItem);
			hChildItem = hNextItem;
		}
	}
	if(m_hWallPath)
	{
		m_wndPropTree.DeleteItem(m_hWallPath);
		m_hWallPath = 0;
	}
	if(m_hWidth)
	{
		m_wndPropTree.DeleteItem(m_hWidth);
		m_hWidth = 0;
	}
	if(m_hHeight)
	{
		m_wndPropTree.DeleteItem(m_hHeight);
		m_hHeight = 0;
	}

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();

	m_hWallPath = m_wndPropTree.InsertItem(_T("Wall Shape Path("+CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit())+")"));
	
	if( m_Wallpath.size() > 0 )
	{
		for( int i=0; i<static_cast<int>(m_Wallpath.size()); i++ )
		{
			Point v3D = m_Wallpath[i];
			CString csPoint;

			csPoint.Format( "x = %.2f; y = %.2f", CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),v3D.getX()),\
				 CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),v3D.getY()) );

			CFloor2* pFloor = NULL;
			pFloor = GetPointFloor( pDoc->m_nActiveFloor );
			if (pFloor)
				csPoint += _T("; Floor:") + pFloor->FloorName();

			aoidDataEx.lSize = sizeof(aoidDataEx);
			aoidDataEx.dwptrItemData = 0;
			aoidDataEx.vrItemStringSectionColorShow.clear();
			isscStringColor.colorSection = RGB(0,0,255);
			strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),v3D.getX()));		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

			strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),v3D.getY()));		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

			strTemp.Format(_T("%s"),pFloor->FloorName());		
			isscStringColor.strSection = strTemp;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

			hItemTemp = m_wndPropTree.InsertItem( csPoint, m_hWallPath  );
			m_wndPropTree.SetItemDataEx(hItemTemp,aoidDataEx);
		}
		m_wndPropTree.Expand( m_hWallPath, TVE_EXPAND );
	}
	//width
	if(m_pWallShape )
	{					
		m_dWidth  = m_pWallShape->GetWidth();
		m_dheight = m_pWallShape->GetHeight();
		m_dWidth = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),m_dWidth );
		m_dheight = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),m_dheight ) ;
	}

	CString csLabel = " Wall Shape  Width ("  + 	CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + "):" ;
	CString sValue ;
	sValue.Format(" (%.2f)",m_dWidth );
	csLabel += sValue;
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%.2f"),m_dWidth);
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hWidth  = m_wndPropTree.InsertItem( csLabel);	
	m_wndPropTree.SetItemDataEx(m_hWidth,aoidDataEx);
	//height
	csLabel = "Wall Shape Height ("  + 	CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + "):" ;
	
	sValue.Format(" (%.2f)",m_dheight);
	csLabel += sValue;
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%.2f"),m_dheight );
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hHeight  = m_wndPropTree.InsertItem( csLabel);	
	m_wndPropTree.SetItemDataEx(m_hHeight,aoidDataEx);		

	m_wndPropTree.SetRedraw(TRUE);
}

void DlgWallShapeProp::OnBnClickedOk()
{
	OnOK();
}

void DlgWallShapeProp::OnOK() 
{
	UpdateData();

	if (static_cast<int>(m_Wallpath.size()) <2)
	{
		AfxMessageBox("The number of the wall path must larger than 2 !");
		return;
	}
	PreProcessName();

	CString csName = m_csLevel1;
	if( !m_csLevel2.IsEmpty() )
		csName += "-" + m_csLevel2;
	if( !m_csLevel3.IsEmpty() )
		csName += "-" + m_csLevel3;
	if( !m_csLevel4.IsEmpty() )
		csName += "-" + m_csLevel4;

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	CStructureID tempID;
	tempID.SetStrDict(pDoc->GetTerminal().inStrDict);
	tempID.setID(csName);
	if (tempID.isBlank())
	{
		AfxMessageBox("Please input the name  !");
		return;	
	}
	if(m_pWallShape && m_pWallShape->GetPointCount() < 2)
	{
		AfxMessageBox("Define 2 Points at least!");
		return;	
	}

	if (tempID.compare(m_pWallShape->getID()) != 0 )
	{
		if(IsNameValid(csName) == false)
		{
			AfxMessageBox("The name inputted is not valid !");
			return;
		}
		m_pWallShape->setID(csName);
	}


	//	if( !CheckProcessorName() )
	//		return;
	
	CXTResizeDialog::OnOK();
}
CFloor2* DlgWallShapeProp::GetPointFloor(int nLevel)
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	std::vector<CFloor2*> vFloors = pDoc->GetCurModeFloor().m_vFloors;
	std::vector<CFloor2*>::iterator iter;

	for (iter = vFloors.begin(); iter != vFloors.end(); iter++)
	{
		if ((*iter)->Level() == nLevel)
			return (*iter);
	}

	return NULL;
}
void DlgWallShapeProp::PreProcessName()
{
	m_csLevel1.TrimLeft();
	m_csLevel1.TrimRight();
	m_csLevel1.Remove( _T(',') );
	m_csLevel1.Replace(_T(' '), _T('_'));
	m_csLevel1.MakeUpper();

	m_csLevel2.TrimLeft();
	m_csLevel2.TrimRight();
	m_csLevel2.Remove( _T(',') );
	m_csLevel2.Replace(_T(' '), _T('_'));
	m_csLevel2.MakeUpper();

	m_csLevel3.TrimLeft();
	m_csLevel3.TrimRight();
	m_csLevel3.Remove( _T(',') );
	m_csLevel3.Replace(_T(' '), _T('_'));
	m_csLevel3.MakeUpper();

	m_csLevel4.TrimLeft();
	m_csLevel4.TrimRight();
	m_csLevel4.Remove( _T(',') );
	m_csLevel4.Replace(_T(' '), _T('_'));
	m_csLevel4.MakeUpper();
}

bool DlgWallShapeProp::IsNameValid(CString& strName)
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	return pDoc->GetCurWallShapeList().IsNameValid(strName);

}
BEGIN_MESSAGE_MAP(DlgWallShapeProp, CXTResizeDialog)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_PROPERTIES, OnNMRclickTreeProperties)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROCPROP_PICKFROMMAP, OnProcpropPickfrommap)
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_COMMAND(ID_WALLHEIGHT_DEFINEHEIGHT, OnWallheightDefineheight)
	ON_COMMAND(ID_WALLWIDTH_DEFINEWIDTH, OnWallwidthDefinewidth)

	ON_COMMAND(ID_PROCPROP_DELETE, OnProcpropDelete)

	ON_CBN_DROPDOWN(IDC_COMBO_NAME_2, OnDropdownComboLevel2)
	ON_CBN_DROPDOWN(IDC_COMBO_NAME_3, OnDropdownComboLevel3)
	ON_CBN_DROPDOWN(IDC_COMBO_NAME_4, OnDropdownComboLevel4)
	ON_CBN_KILLFOCUS(IDC_COMBO_NAME_1, OnKillfocusComboLevel1)

END_MESSAGE_MAP()


// DlgWallShapeProp message handlers
LRESULT DlgWallShapeProp::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_INPLACE_SPIN ){
		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		if (m_hRClickItem == m_hHeight)
		{
			if (m_pWallShape)
			{
				m_pWallShape->SetHeight(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent));
			}
			else
				m_dheight = CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent);

			LoadTree();
			return TRUE;
		}
		if (m_hRClickItem == m_hWidth)
		{
			if (m_pWallShape)
			{
				m_pWallShape->SetWidth(CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent));
			}
			else
				m_dWidth = CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent);

			LoadTree();
			return TRUE;
		}
	}else{
		return CDialog::DefWindowProc(message, wParam, lParam);
	}
	
	
	
	
	return TRUE;
}
void DlgWallShapeProp::OnWallheightDefineheight()
{
	// TODO: Add your command handler code here
	m_wndPropTree.SetDisplayType( 2 );
	m_wndPropTree.SetDisplayNum(  static_cast<int>(m_dheight) );
	m_wndPropTree.SetSpinRange( 0,200);
	m_wndPropTree.SpinEditLabel( m_hRClickItem );

}

void DlgWallShapeProp::OnWallwidthDefinewidth()
{
	// TODO: Add your command handler code here
	m_wndPropTree.SetDisplayType( 2 );
	m_wndPropTree.SetDisplayNum(  static_cast<int>(m_dWidth) );
	m_wndPropTree.SetSpinRange( 0,200);
	m_wndPropTree.SpinEditLabel( m_hRClickItem );
}

void DlgWallShapeProp::OnKillfocusComboLevel1() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	PreProcessName();

	if( 1/*m_bTypeCanChange*/ )
	{
		SetTypeByLevel1();	
	}
}

// base on the string in the combo level1 
void DlgWallShapeProp::SetTypeByLevel1()
{
	/*
	if (!m_csLevel1.IsEmpty())
	{
		CString strTemp = m_csLevel1;
		m_wndName1Combo.ResetContent();

		CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
		ASSERT(pDoc);
		StringList groups;
		pDoc->GetCurWallShapeList().getAllGroupNames( groups);

		int nCurSel = -1;
		for( int i = 0; i < groups.getCount(); i++ )
		{
			CString strAdd = groups.getString(i);
			if (!strAdd.CompareNoCase(strTemp))
				nCurSel = i;
			m_wndName1Combo.AddString(strAdd);
		}
		if(nCurSel != -1)
			m_wndName1Combo.SetCurSel(nCurSel);
	}

	*/

/*
	if( !m_csLevel1.IsEmpty() )
	{
		CStructureID id;
		id.SetStrDict(GetInputTerminal()->inStrDict);
		id.setID(m_csLevel1);

		StringList strList;
		CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
		ASSERT(pDoc);
		GroupIndex groupIdx = pDoc->GetCurWallShapeList().getGroupIndexOriginal(id);

		if( groupIdx.start >= 0 )
		{
			WallShape * pWallShape = NULL;
			pWallShape = pDoc->GetCurWallShapeList().getWallShape(id);
			ASSERT(pWallShape);
			//int nTypeIdx  ;
			for (int i=0; i< pComboBox->GetCount(); i++)
			{
			//	LPTSTR szTemp;
			//	pComboBox->GetLBText(i, szTemp);
			//	if( szTemp == m_csLevel1 )
			//	{
			//		pComboBox->SetCurSel( i );
			//		break;
			//	}
			}
			pComboBox->EnableWindow( FALSE );
			LoadTree();

			return;
		}

	}


	if (m_nForceProcType == -1)
		if(pComboBox)
			pComboBox->EnableWindow();
	*/
	LoadTree();
	
}

void DlgWallShapeProp::OnDropdownComboLevel2() 
{
	// TODO: Add your control notification handler code here
m_wndName2Combo.ResetContent();
	
	CString csLevel1;
	m_wndName1Combo.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;

	CStructureID id;
	id.SetStrDict(GetInputTerminal()->inStrDict);
	id.setID(m_csLevel1);

	StringList strList;
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	ASSERT(pDoc);
	pDoc->GetCurWallShapeList().getMemberNames(id, strList);
	for( int i=0; i<strList.getCount(); i++ )
		m_wndName2Combo.AddString( strList.getString( i ) );

}

// base on what level 1-2 selection
void DlgWallShapeProp::OnDropdownComboLevel3() 
{
	// TODO: Add your control notification handler code here
	m_wndName3Combo.ResetContent();

	CString csLevel1;
	m_wndName1Combo.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;
	CString csLevel2;
	m_wndName2Combo.GetWindowText( csLevel2 );
	if( csLevel2.IsEmpty() )
		return;

	CString csLevel12 = csLevel1 + "-" + csLevel2;

	CStructureID id;
	id.SetStrDict(GetInputTerminal()->inStrDict);
	id.setID(csLevel12);

	StringList strList;
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	ASSERT(pDoc);
	pDoc->GetCurWallShapeList().getMemberNames(id, strList);
	for( int i=0; i<strList.getCount(); i++ )
		m_wndName3Combo.AddString( strList.getString( i ) );	

}

// base on what level 1-2-3 selection
void DlgWallShapeProp::OnDropdownComboLevel4() 
{
	// TODO: Add your control notification handler code here
	m_wndName4Combo.ResetContent();

	CString csLevel1;
	m_wndName1Combo.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;
	CString csLevel2;
	m_wndName2Combo.GetWindowText( csLevel2 );
	if( csLevel2.IsEmpty() )
		return;
	CString csLevel3;
	m_wndName3Combo.GetWindowText( csLevel3 );
	if( csLevel3.IsEmpty() )
		return;

	CString csLevel123 = csLevel1 + "-" + csLevel2 + "-" + csLevel3;

	CStructureID id;
	id.SetStrDict(GetInputTerminal()->inStrDict);
	id.setID(csLevel123);

	StringList strList;
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	ASSERT(pDoc);
	pDoc->GetCurWallShapeList().getMemberNames(id, strList);

	for( int i=0; i<strList.getCount(); i++ )
		m_wndName4Combo.AddString( strList.getString( i ) );	


}

InputTerminal* DlgWallShapeProp::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	return (InputTerminal*)&pDoc->GetTerminal();
}

void DlgWallShapeProp::OnProcpropDelete()
{
	m_Wallpath.clear();
	LoadTree();
}

bool DlgWallShapeProp::ConvertUnitFromDBtoGUI(void)
{
	return (false);
}

bool DlgWallShapeProp::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool DlgWallShapeProp::ConvertUnitFromGUItoDB(void)
{
	return (false);
}




