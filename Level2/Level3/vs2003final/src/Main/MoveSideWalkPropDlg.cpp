// MoveSideWalkPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "MoveSideWalkPropDlg.h"
#include "TermPlanDoc.h"
#include "common\ProbDistManager.h"
#include "inputs\SideMoveWalkProcData.h"
#include "inputs\SideMoveWalkDataSet.h"
#include "Common\WinMsg.h"
#include "../common/UnitsManager.h"
#include "..\inputs\probab.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMoveSideWalkPropDlg property page

IMPLEMENT_DYNCREATE(CMoveSideWalkPropDlg, CPropertyPage)

CMoveSideWalkPropDlg::CMoveSideWalkPropDlg( CWnd* pView ) : CPropertyPage(CMoveSideWalkPropDlg::IDD)
{
	m_pView = pView ;
	m_iDbIndex = -1;
	m_iCollumnIdx = -1;

	m_treeProc.setInputTerminal( GetInputTerminal() );
}

CMoveSideWalkPropDlg::~CMoveSideWalkPropDlg()
{
}

void CMoveSideWalkPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMoveSideWalkPropDlg)
	DDX_Control(pDX, IDC_BUTTON_NEW, m_btnNew);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_btnDel);
	DDX_Control(pDX, IDC_LIST_PROP, m_listProperty);
	DDX_Control(pDX, IDC_TREE_PROCESSOR, m_treeProc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMoveSideWalkPropDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CMoveSideWalkPropDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROCESSOR, OnSelchangedTreeProcessor)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)	
	ON_MESSAGE(WM_COLLUM_INDEX, OnCollumnIndex)		
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMoveSideWalkPropDlg message handlers

BOOL CMoveSideWalkPropDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_treeProc.m_bDisplayAll=TRUE;
	ReloadDatabase();
	SetListCtrl();


	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
	{
		m_btnNew.EnableWindow(FALSE);
		m_btnDel.EnableWindow(FALSE);
		return TRUE;
	}
	int nDBIdx = m_treeProc.GetItemData( hItem );
	if(nDBIdx == -1)
	{
		m_btnNew.EnableWindow(TRUE);
		m_btnDel.EnableWindow(FALSE);
	}
	else
	{
		m_btnNew.EnableWindow(FALSE);
		m_btnDel.EnableWindow(TRUE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CMoveSideWalkPropDlg::ReloadDatabase()
{
	m_treeProc.LoadData( GetInputTerminal(), (ProcessorDatabase*)GetInputTerminal()->m_pMovingSideWalk , MovingSideWalkProc );
}
	
InputTerminal* CMoveSideWalkPropDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pView)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}

CString CMoveSideWalkPropDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pView)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}

void CMoveSideWalkPropDlg::SetListCtrl()
{
	DWORD dwStyle = m_listProperty.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listProperty.SetExtendedStyle( dwStyle );

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char columnLabel[2][128];
	sprintf( columnLabel[0], "Property " );
	CString csLabel;
	strcpy( columnLabel[1], "Value" );		
	int DefaultColumnWidth[] = { 193, 230 };
	int nFormat[] = {	LVCFMT_CENTER, LVCFMT_CENTER };
	for( int i=0; i<2; i++ )
	{
		CStringList strList;
		CString s;
		s.LoadString(IDS_STRING_NEWDIST);
		strList.InsertAfter( strList.GetTailPosition(), s );
		int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
		for( int m=0; m<nCount; m++ )
		{
			CProbDistEntry* pPBEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( m );			
			strList.InsertAfter( strList.GetTailPosition(), pPBEntry->m_csName );
		}
		lvc.csList = &strList;
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		if( i == 0 )
			lvc.fmt = LVCFMT_NOEDIT;
		else
			lvc.fmt = LVCFMT_NUMBER;
		m_listProperty.InsertColumn( i, &lvc );
	}
	//m_listProperty.SetInputTerminal( GetInputTerminal() );
}

void CMoveSideWalkPropDlg::OnSelchangedTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	//ReloadListData( NULL );
	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
		return ;
	m_iDbIndex = m_treeProc.GetItemData( hItem );
	ReloadListData();
	
	
	/*
	if(m_iDbIndex == -1)
	{
		m_btnNew.EnableWindow(TRUE);
		m_btnDel.EnableWindow(FALSE);
		m_listProperty.DeleteAllItems();
		
	}
	else
	{
		DistanceUnit dSpeed= ((CSideMoveWalkProcData *)(GetInputTerminal()->m_pMovingSideWalk->getItem( m_iDbIndex )))->GetMoveSpeed();
		DistanceUnit dWidth= ((CSideMoveWalkProcData *)(GetInputTerminal()->m_pMovingSideWalk->getItem( m_iDbIndex )))->GetWidth();
		m_listProperty.DeleteAllItems();
		m_listProperty.SetSpinDisplayType( 1 );
		CString sSpeed= "Moving Spdeed ( " ;
		sSpeed += UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits());
		sSpeed += "/s )";
			
		int iItemIndex = m_listProperty.InsertItem(0,sSpeed );
		m_listProperty.SetItemData( iItemIndex, m_iDbIndex );
		
		CString sDisplaySpeed;
		sDisplaySpeed.Format("%.2f", UNITSMANAGER->ConvertLength(dSpeed ) );
		
		m_listProperty.SetItemText( iItemIndex, 1, sDisplaySpeed );



		CString sWidth= "Width ( " ;
		sWidth += UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits());
		sWidth += " )";
			
			
		iItemIndex = m_listProperty.InsertItem(1,sWidth );
		m_listProperty.SetItemData( iItemIndex, m_iDbIndex );

		CString sDisplayWidth;
		sDisplayWidth.Format("%.2f", UNITSMANAGER->ConvertLength(dWidth ) );
		
		m_listProperty.SetItemText( iItemIndex, 1, sDisplayWidth );

		m_btnDel.EnableWindow();
		m_btnNew.EnableWindow(FALSE);
		
		
	}
*/

	*pResult = 0;



	
}

LRESULT CMoveSideWalkPropDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_INPLACE_EDIT)
	{
		CString* pString = (CString*)lParam;
		//LPSPINTEXT pst = (LPSPINTEXT) lParam;

		/*
		if( pst->iPercent > 0 )
			((CSideMoveWalkProcData *)(GetInputTerminal()->m_pMovingSideWalk->getItem( m_iDbIndex )))->SetMoveSpeed( pst->iPercent );
		else
		{
			GetInputTerminal()->m_pMovingSideWalk->deleteItem( m_iDbIndex );
			ReloadDatabase();	
		}
		*/
		if ( m_iDbIndex < 0 )
			return FALSE;
		if ( m_iCollumnIdx == 2 )
		{
			int iCapacity = atoi( *pString );
			((CSideMoveWalkProcData *)(GetInputTerminal()->m_pMovingSideWalk->getItem( m_iDbIndex )))->SetCapacity( iCapacity );
		}
		else
		{
			DistanceUnit dResultSpeed = UNITSMANAGER->UnConvertLength( atof( *pString ) );
			if( m_iCollumnIdx == 0 )
					((CSideMoveWalkProcData *)(GetInputTerminal()->m_pMovingSideWalk->getItem( m_iDbIndex )))->SetMoveSpeed( dResultSpeed );
			else if ( m_iCollumnIdx == 1 )
					((CSideMoveWalkProcData *)(GetInputTerminal()->m_pMovingSideWalk->getItem( m_iDbIndex )))->SetWidth( dResultSpeed );
		}
		
		

		
		SetModified();
		return TRUE;
	}
	
	return CPropertyPage::DefWindowProc(message, wParam, lParam);
}

void CMoveSideWalkPropDlg::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	GetInputTerminal()->m_pMovingSideWalk->saveDataSet( GetProjPath(), false );
	CPropertyPage::OnOK();
}

void CMoveSideWalkPropDlg::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	try
	{
		GetInputTerminal()->m_pMovingSideWalk->loadDataSet( GetProjPath() );
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
		delete _pError;			
	}
	CPropertyPage::OnCancel();
}

void CMoveSideWalkPropDlg::OnButtonDelete() 
{


	if( m_iDbIndex == -1 )
		return;

	GetInputTerminal()->m_pMovingSideWalk->deleteItem( m_iDbIndex );
	ReloadDatabase();
	SetModified();
	m_btnDel.EnableWindow(FALSE);
	
	if(m_treeProc.hSelItem==NULL)
		return;
	HTREEITEM hParent = m_treeProc.GetParentItem(m_treeProc.hSelItem);
	if (hParent != NULL)
		m_treeProc.Expand(hParent, TVE_EXPAND);
	m_treeProc.EnsureVisible(m_treeProc.hSelItem);
	//m_treeProc.SelectItem(m_treeProc.hSelItem);
	
	
	m_listProperty.DeleteAllItems();

	
}

void CMoveSideWalkPropDlg::OnButtonNew() 
{
	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
		return ;

	if(m_iDbIndex == -1)
	{
		ProcessorID id;
		CString str = m_treeProc.GetItemText( hItem );
		id.SetStrDict( GetInputTerminal()->inStrDict );
		id.setID( str );
	
		CSideMoveWalkProcData tempData( id );	
		tempData.SetMoveSpeed( 100 );
		tempData.SetWidth( 100 );
		tempData.SetCapacity( 0 );
		m_iDbIndex = GetInputTerminal()->m_pMovingSideWalk->insert( id, &tempData);
		m_treeProc.ResetTreeData( NULL );

		double iSpeed= 100.0;
		double dWidth = 100.0;
		int iCapacity = 0;
		m_listProperty.DeleteAllItems();
		m_listProperty.SetSpinDisplayType( 1 );
		CString sSpeed= "Moving Spdeed ( " ;
		sSpeed += UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits());
		sSpeed += "/s )";
			
		int iItemIndex = m_listProperty.InsertItem(0,sSpeed );
		m_listProperty.SetItemData( iItemIndex, m_iDbIndex );
		
		CString sDisplaySpeed;
		sDisplaySpeed.Format("%.2f", UNITSMANAGER->ConvertLength(iSpeed ) );
		
		m_listProperty.SetItemText( iItemIndex, 1, sDisplaySpeed );


		CString sWidth= "Width ( " ;
		sWidth += UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits());
		sWidth += " )";
			
		iItemIndex = m_listProperty.InsertItem(1,sWidth );
		m_listProperty.SetItemData( iItemIndex, m_iDbIndex );
		
		CString sDisplayWidth;
		sDisplayWidth.Format("%.2f", UNITSMANAGER->ConvertLength(dWidth ) );
		
		m_listProperty.SetItemText( iItemIndex, 1, sDisplayWidth );



		CString sCapacity = "Capacity ( 0 unlimited )";
		iItemIndex = m_listProperty.InsertItem(2,sCapacity );
		m_listProperty.SetItemData( iItemIndex, m_iDbIndex );
		CString sDisplayCapacity;
		sDisplayCapacity.Format("%d",iCapacity );
		m_listProperty.SetItemText( iItemIndex, 1, sDisplayCapacity );
		
		
	}


	// Refresh the state
	hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
		return ;
	m_iDbIndex = m_treeProc.GetItemData( hItem );
	if(m_iDbIndex == -1)
	{
		m_btnNew.EnableWindow(TRUE);
		m_btnDel.EnableWindow(FALSE);

	}
	else
	{
		m_btnNew.EnableWindow(FALSE);
		m_btnDel.EnableWindow(TRUE);
	}

	SetModified();
}


LRESULT CMoveSideWalkPropDlg::OnCollumnIndex(   WPARAM wParam,  LPARAM lParam   ) 
{
	// TODO: Add your message handler code here and/or call default
	 int iIndex = (int)wParam;
	 if( iIndex >= 0 )
		 m_iCollumnIdx = iIndex ;
	 else
		 m_iCollumnIdx = -1;

	 return TRUE;
	
}

CSideMoveWalkProcData * CMoveSideWalkPropDlg::GetConstraintDatabase( bool& _bIsOwn )
{
	HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
	if( hSelItem == NULL )
		return NULL;
	int nDBIdx = m_treeProc.GetItemData( hSelItem );
	if( nDBIdx != -1 )
		_bIsOwn = true;
	else
		_bIsOwn = false;
	while( nDBIdx == -1 && hSelItem != NULL ) 
	{
		hSelItem = m_treeProc.GetParentItem( hSelItem );
		if( hSelItem == NULL )
			break;
		nDBIdx = m_treeProc.GetItemData( hSelItem );
	}
	if(nDBIdx == -1)
	{
		return NULL;
	}
	return (CSideMoveWalkProcData *)GetInputTerminal()->m_pMovingSideWalk->getItem(nDBIdx);
	
}

void CMoveSideWalkPropDlg::ReloadListData()
{
	bool bIsOwn;
	m_listProperty.DeleteAllItems();
	CSideMoveWalkProcData* pConDB = GetConstraintDatabase( bIsOwn );
	double dSpeed;
	double dWidth;
	int iCapacity;
	if(pConDB==NULL)
	{
		dSpeed = 100.0;
		dWidth = 100.0;
		iCapacity = 0;

	}
	else
	{
		dSpeed = pConDB->GetMoveSpeed();
		dWidth= pConDB->GetWidth();
		iCapacity = pConDB->GetCapacity();

	}

	if( bIsOwn )
	{
		m_btnDel.EnableWindow();
		m_btnNew.EnableWindow(FALSE);
	}
	else
	{
		m_btnDel.EnableWindow(FALSE);
		m_btnNew.EnableWindow();
	}

	m_listProperty.SetSpinDisplayType( 1 );
	CString sSpeed= "Moving Spdeed ( " ;
	sSpeed += UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits());
	sSpeed += "/s )";
			
	int iItemIndex = m_listProperty.InsertItem(0,sSpeed );
	m_listProperty.SetItemData( iItemIndex, m_iDbIndex );
		
	CString sDisplaySpeed;
	sDisplaySpeed.Format("%.2f", UNITSMANAGER->ConvertLength(dSpeed ) );
		
	m_listProperty.SetItemText( iItemIndex, 1, sDisplaySpeed );


	CString sWidth= "Width ( " ;
	sWidth += UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits());
	sWidth += " )";
		
	iItemIndex = m_listProperty.InsertItem(1,sWidth );
	m_listProperty.SetItemData( iItemIndex, m_iDbIndex );
		
	CString sDisplayWidth;
	sDisplayWidth.Format("%.2f", UNITSMANAGER->ConvertLength(dWidth ) );
	
	m_listProperty.SetItemText( iItemIndex, 1, sDisplayWidth );

	CString sCapacity = "Capacity ( 0 unlimited )";
	iItemIndex = m_listProperty.InsertItem(2,sCapacity );
	m_listProperty.SetItemData( iItemIndex, m_iDbIndex );
	CString sDisplayCapacity;
	sDisplayCapacity.Format("%d",iCapacity );
	m_listProperty.SetItemText( iItemIndex, 1, sDisplayCapacity );

	
	
}

