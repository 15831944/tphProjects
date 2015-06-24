// DlgLabourCosts.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "EconomicaDataTypeDlg.h"
#include "TermPlanDoc.h"
#include "termplan.h"
#include "economic\ProcEconomicData.h"
#include "economic\ProcEconDatabase.h"
#include "economic\ProcEconomicDataElement.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEconomicaDataTypeDlg dialog


CEconomicaDataTypeDlg::CEconomicaDataTypeDlg(enum ECONOMICDATATTYPE _enumType, CWnd* pParent)
: CDialog(CEconomicaDataTypeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEconomicaDataTypeDlg)
	m_enumType = _enumType;
	m_strValue1 = _T("");
	m_strValue2 = _T("");
	m_strValue3 = _T("");
	m_strValue4 = _T("");
	m_strValue5 = _T("");
	//}}AFX_DATA_INIT
	m_strValue1 = "";
	m_strValue2 = "";
	m_strValue3 = "";
	m_strValue4 = "";
	m_strValue5 = "";

	m_hSelItem=NULL;
	
	m_treeProc.setInputTerminal( GetInputTerminal() );
}


void CEconomicaDataTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEconomicaDataTypeDlg)
	DDX_Control(pDX, IDC_BUT_NEW, m_btnNew);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_btnApply);
	DDX_Control(pDX, IDC_STATIC_LABEL_5, m_staticLabel5);
	DDX_Control(pDX, IDC_EDIT_VALUE_5, m_editValue5);
	DDX_Control(pDX, IDC_STATIC_TREETITLE, m_staticTreeTitle);
	DDX_Control(pDX, IDC_STATIC_LABEL_4, m_staticLabel4);
	DDX_Control(pDX, IDC_STATIC_LABEL_3, m_staticLabel3);
	DDX_Control(pDX, IDC_STATIC_LABEL_2, m_staticLabel2);
	DDX_Control(pDX, IDC_STATIC_LABEL_1, m_staticLabel1);
	DDX_Control(pDX, IDC_EDIT_VALUE_4, m_editValue4);
	DDX_Control(pDX, IDC_EDIT_VALUE_3, m_editValue3);
	DDX_Control(pDX, IDC_EDIT_VALUE_2, m_editValue2);
	DDX_Control(pDX, IDC_EDIT_VALUE_1, m_editValue1);
	DDX_Control(pDX, IDC_BUTTON_ERASE, m_btnErase);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_TREE_PROC, m_treeProc);
	DDX_Text(pDX, IDC_EDIT_VALUE_1, m_strValue1);
	DDX_Text(pDX, IDC_EDIT_VALUE_2, m_strValue2);
	DDX_Text(pDX, IDC_EDIT_VALUE_3, m_strValue3);
	DDX_Text(pDX, IDC_EDIT_VALUE_4, m_strValue4);
	DDX_Text(pDX, IDC_EDIT_VALUE_5, m_strValue5);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEconomicaDataTypeDlg, CDialog)
//{{AFX_MSG_MAP(CEconomicaDataTypeDlg)
ON_BN_CLICKED(IDC_BUTTON_ERASE, OnButtonErase)
ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROC, OnSelchangedTreeProc)
ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_EN_SETFOCUS(IDC_EDIT_VALUE_1, OnSetfocusEditValue1)
	ON_EN_KILLFOCUS(IDC_EDIT_VALUE_1, OnKillfocusEditValue1)
	ON_EN_CHANGE(IDC_EDIT_VALUE_1, OnChangeEditValue1)
	ON_EN_CHANGE(IDC_EDIT_VALUE_2, OnChangeEditValue2)
	ON_EN_CHANGE(IDC_EDIT_VALUE_3, OnChangeEditValue3)
	ON_EN_CHANGE(IDC_EDIT_VALUE_4, OnChangeEditValue4)
	ON_EN_CHANGE(IDC_EDIT_VALUE_5, OnChangeEditValue5)
	ON_BN_CLICKED(IDC_BUT_NEW, OnButNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEconomicaDataTypeDlg message handlers
InputTerminal* CEconomicaDataTypeDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}


CString CEconomicaDataTypeDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}


void CEconomicaDataTypeDlg::OnButtonErase() 
{
	// Modified by Xie Bo 2002.5.12
//	int nDBIdx;
//	BOOL bOwn;
//	
//    GetDBIdx( nDBIdx,bOwn );
//	
//	if( nDBIdx == -1 || !bOwn )
//		return;
	HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
	if( hSelItem == NULL )
		return;

	int nDBIdx = m_treeProc.GetItemData( hSelItem );

	if( nDBIdx == -1 )
		return;
	m_treeProc.m_strSelectedID=m_treeProc.GetItemText(hSelItem);
	// End Modify
		
	m_pDB->deleteItem( nDBIdx );
	
	ReloadDatabase();
	//m_treeProc.ResetTreeData( NULL );
	ShowDefaultData();
	

	// Added by Xie Bo 2002.5.12
	// Ensure visible
	if(m_treeProc.hSelItem==NULL)
		return;
	HTREEITEM hParent = m_treeProc.GetParentItem(m_treeProc.hSelItem);
	HTREEITEM hArray[100];
	int nCount=0;
	while (hParent != NULL)
	{
		//m_treeProc.Expand(hParent, TVE_EXPAND);
		hArray[nCount]=hParent;
		nCount++;
		hParent = m_treeProc.GetParentItem(hParent);
	}
	for(int i=nCount-1;i>=0;i--)
	{
		m_treeProc.Expand(hArray[i], TVE_EXPAND);
	}
	m_treeProc.EnsureVisible(m_treeProc.hSelItem);
	m_treeProc.SelectItem(m_treeProc.hSelItem);
	
	// Added by Xie Bo 2002.5.12
	// This is just a patch.
	m_treeProc.SetItemColor(m_treeProc.hSelItem,RGB(0,0,0));
	m_btnSave.EnableWindow();
}

void CEconomicaDataTypeDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	// Added by Xie Bo 2002.5.12
	CWaitCursor wc;
	m_pDB->saveDataSet( GetProjPath(), true );
	m_btnSave.EnableWindow( FALSE );
}

void CEconomicaDataTypeDlg::ReloadDatabase()
{
	int nProcType = -1;
	if( m_enumType == ECONOMIC_GATEUSAGE )
	{
		nProcType = StandProcessor; // load stand processor
		m_treeProc.LoadData( GetInputTerminal(), m_pDB, nProcType );
		nProcType = GateProc;
	}
	m_treeProc.LoadData( GetInputTerminal(), m_pDB, nProcType );
}

// 
void CEconomicaDataTypeDlg::OnSelchangedTreeProc(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	ShowItemData();
	*pResult = 0;
}

BOOL CEconomicaDataTypeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	switch( m_enumType )
	{
	case ECONOMIC_LABOUR:
		m_staticTreeTitle.SetWindowText( "Processor" );
		m_staticLabel4.SetWindowText( "OT / hr ( $ )" );
		m_staticLabel3.SetWindowText( "Reg / hr ( $ )" );
		m_staticLabel2.SetWindowText( "Base / yr ( $ )" );
		m_staticLabel1.SetWindowText( "Shift period  ( hrs )" );
		SetWindowText( "Labour Fixed and Variable Costs" );
		m_staticLabel4.ShowWindow( SW_SHOW );
		m_editValue4.ShowWindow( SW_SHOW );
		break;
		
	case ECONOMIC_PROCESSOR:
		SetWindowText( "Processor Fixed and Variable Costs" );
		m_staticTreeTitle.SetWindowText( "Processor" );
		m_staticLabel4.SetWindowText( "Op cost/hr ( $ )" );
		m_staticLabel3.SetWindowText( "Annual Dep ( $ )" );
		m_staticLabel2.SetWindowText( "Depr period ( yrs )" );
		m_staticLabel1.SetWindowText( "Installation Cost ( $ )" );
		m_staticLabel4.ShowWindow( SW_SHOW );
		m_editValue4.ShowWindow( SW_SHOW );
		break;
		
	case ECONOMIC_PROCESSORUSAGE:
		SetWindowText( "Processor Usage Fees" );
		m_staticTreeTitle.SetWindowText( "Processor" );
		m_staticLabel5.SetWindowText( "$/bag" );
		m_staticLabel4.SetWindowText( "$/pax" );
		m_staticLabel3.SetWindowText( "$/flight" );
		m_staticLabel2.SetWindowText( "$/hr" );
		m_staticLabel1.SetWindowText( "$/day" );
		m_staticLabel4.ShowWindow( SW_SHOW );
		m_editValue4.ShowWindow( SW_SHOW );
		m_staticLabel5.ShowWindow( SW_SHOW );
		m_editValue5.ShowWindow( SW_SHOW );
		break;
		
	case ECONOMIC_GATEUSAGE:
		SetWindowText( "Gate Usage Fees" ); 
		m_staticTreeTitle.SetWindowText( "Gates" );
		m_staticLabel3.SetWindowText( "$ / 1000 lb" );
		m_staticLabel2.SetWindowText( "$ / hr" );
		m_staticLabel1.SetWindowText( "$ / flight" );
		break;
	}

	GetDatabase();
	
	ReloadDatabase();

	ShowItemData();
	
	m_btnSave.EnableWindow( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


// show default value without enable the control
void CEconomicaDataTypeDlg::ShowDefaultData()
{
	switch( m_enumType )
	{
	case ECONOMIC_LABOUR:
		m_strValue1 = "8";
		m_strValue2 = "0";
		m_strValue3 = "0";
		m_strValue4 = "0";
		break;
		
	case ECONOMIC_PROCESSOR:
		m_strValue1 = "0";
		m_strValue2 = "5";
		m_strValue3 = "0";
		m_strValue4 = "0.0";
		break;
		
	case ECONOMIC_GATEUSAGE:
		m_strValue1 = "0";
		m_strValue2 = "0";
		m_strValue3 = "0";
		break;
		
	case ECONOMIC_PROCESSORUSAGE:
		m_strValue1 = "0";
		m_strValue2 = "0";
		m_strValue3 = "0";
		m_strValue4 = "0";
		m_strValue5 = "0";
		break;
	}
	
	UpdateData( FALSE );
}


// update the data
void CEconomicaDataTypeDlg::SetData()
{
    int nDBIdx;
	BOOL bOwn;
    GetDBIdx( nDBIdx,bOwn );
	assert( nDBIdx >= 0 && bOwn );
    
	char* csStop;
	double dValue;
	switch( m_enumType )
	{
	case ECONOMIC_LABOUR:
		{	
			CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
			CProcLabrFixVarCostData* pData = (CProcLabrFixVarCostData*)pEntry->getData();
			dValue = strtod( m_strValue2,&csStop );
			if( dValue >= 0 && *csStop != m_strValue2[0] )
				pData->SetBaseSalary( dValue );
			dValue = strtod( m_strValue4,&csStop );
			if( dValue >= 0 && *csStop != m_strValue4[0] )
				pData->SetOTPay( dValue );
			dValue = strtod( m_strValue1,&csStop );
			if( dValue >= 0 && *csStop != m_strValue1[0] )
				pData->SetShiftPeriod( static_cast<int>(dValue) );
			dValue = strtod( m_strValue3,&csStop );
			if( dValue >= 0 && *csStop != m_strValue3[0] )
				pData->SetHourlyPay( dValue );				
			break;
		}
		
	case ECONOMIC_PROCESSOR:
		{	
			
			CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
			CProcFixVarCostData* pData = (CProcFixVarCostData*)pEntry->getData();
			dValue = strtod( m_strValue1,&csStop );
			if( dValue >= 0 && *csStop != m_strValue1[0] )
				pData->SetInstallCost( dValue );
			dValue = strtod( m_strValue2,&csStop );
			if( dValue > 0 && *csStop != m_strValue2[0] )
				pData->SetDeprPeriod( static_cast<int>(dValue) );
			dValue = strtod( m_strValue4,&csStop );
			if( dValue >= 0 && *csStop != m_strValue4[0] )
				pData->SetOpCostPerHour( dValue );
				
			break;
		}
		
	case ECONOMIC_GATEUSAGE:
		{	
			CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
			CGateUsageFeeData* pData = (CGateUsageFeeData*)pEntry->getData();
			dValue = strtod( m_strValue1,&csStop );
			if( dValue >= 0 && *csStop != m_strValue1[0] )
				pData->SetFeePerFlt( dValue );
			dValue = strtod( m_strValue2,&csStop );
			if( dValue >= 0 && *csStop != m_strValue2[0] )
				pData->SetFeePerHour( dValue );
			dValue = strtod( m_strValue3,&csStop );
			if( dValue >= 0 && *csStop != m_strValue3[0] )
				pData->SetFeePer100lb( dValue );				
			break;
		}
	case ECONOMIC_PROCESSORUSAGE:
		{	
			CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
			CProcUsageFeeData* pData = (CProcUsageFeeData*)pEntry->getData();
			dValue = strtod( m_strValue1,&csStop );
			if( dValue >= 0 && *csStop != m_strValue1[0] )
				pData->SetFeePerDay( dValue );
			dValue = strtod( m_strValue2,&csStop );
			if( dValue >= 0 && *csStop != m_strValue2[0] )
				pData->SetFeePerHour( dValue );
			dValue = strtod( m_strValue3,&csStop );
			if( dValue >= 0 && *csStop != m_strValue3[0] )
				pData->SetFeePerFlt( dValue );
			dValue = strtod( m_strValue4,&csStop );
			if( dValue >= 0 && *csStop != m_strValue4[0] )
				pData->SetFeePerPax( dValue );		
			dValue = strtod( m_strValue5,&csStop );
			if( dValue >= 0 && *csStop != m_strValue5[0] )
				pData->SetFeePerBag( dValue );				
				
			break;
		}
	}
}


BOOL CEconomicaDataTypeDlg::CheckInput( int _nID,CString& _str )
{
	UpdateData( TRUE );
	GetDlgItem( _nID )->GetWindowText( _str );
	
	// Added by Xie Bo 2002.5.9
	
	if(_str.IsEmpty())
	{
		CString str,strMessage;
		switch(_nID)
		{
		case IDC_EDIT_VALUE_1:
			m_staticLabel1.GetWindowText(str);
			strMessage.Format("%s is empty!",str);
			AfxMessageBox(strMessage);
			break;
		case IDC_EDIT_VALUE_2:
			m_staticLabel2.GetWindowText(str);
			strMessage.Format("%s is empty!",str);
			AfxMessageBox(strMessage);
			break;
		case IDC_EDIT_VALUE_3:
			m_staticLabel3.GetWindowText(str);
			strMessage.Format("%s is empty!",str);
			AfxMessageBox(strMessage);
			break;
		case IDC_EDIT_VALUE_4:
			m_staticLabel4.GetWindowText(str);
			strMessage.Format("%s is empty!",str);
			AfxMessageBox(strMessage);
			break;
		case IDC_EDIT_VALUE_5:
			m_staticLabel5.GetWindowText(str);
			strMessage.Format("%s is empty!",str);
			AfxMessageBox(strMessage);
			break;
		}
		return FALSE;
	}

	char* csStop;
	double dValue = strtod( _str,&csStop );
	if( dValue <0 && *csStop == _str[0] )
	{
		MessageBox( "Input data must be larger than zero!");
		return FALSE;
	}	
	return TRUE;
}

void CEconomicaDataTypeDlg::OnOK() 
{
	// TODO: Add extra validation here
	if( m_btnSave.IsWindowEnabled() )
		OnButtonSave();
	
	CDialog::OnOK();
}

void CEconomicaDataTypeDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if( m_btnSave.IsWindowEnabled() )
	{
		try
		{
			m_pDB->loadDataSet( GetProjPath() );
		}
		catch( FileVersionTooNewError* _pError )
		{
			char szBuf[128];
			_pError->getMessage( szBuf );
			MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
			delete _pError;			
		}
	}
	CDialog::OnCancel();
}


void CEconomicaDataTypeDlg::OnButtonApply() 
{
	// TODO: Add your control notification handler code here
	switch( m_enumType )
	{
	case ECONOMIC_LABOUR:
	case ECONOMIC_PROCESSOR:
		if( CheckInput( IDC_EDIT_VALUE_1,m_strValue1 ) &&
			CheckInput( IDC_EDIT_VALUE_2,m_strValue2 ) &&
			CheckInput( IDC_EDIT_VALUE_3,m_strValue3 ) &&
			CheckInput( IDC_EDIT_VALUE_4,m_strValue4 )  )
			SetData();
		break;
		
		
	case ECONOMIC_PROCESSORUSAGE:
		if( CheckInput( IDC_EDIT_VALUE_1,m_strValue1 ) &&
			CheckInput( IDC_EDIT_VALUE_2,m_strValue2 ) &&
			CheckInput( IDC_EDIT_VALUE_3,m_strValue3 ) &&
			CheckInput( IDC_EDIT_VALUE_4,m_strValue4 ) &&
			CheckInput( IDC_EDIT_VALUE_5,m_strValue5 ) )
			SetData();
		break;
		
	case ECONOMIC_GATEUSAGE:
		if( CheckInput( IDC_EDIT_VALUE_1,m_strValue1 ) &&
			CheckInput( IDC_EDIT_VALUE_2,m_strValue2 ) &&
			CheckInput( IDC_EDIT_VALUE_3,m_strValue3 )  )
			SetData();
		break;
	}

	ShowItemData();
	m_btnSave.EnableWindow();
}

void CEconomicaDataTypeDlg::OnSetfocusEditValue1() 
{
	// TODO: Add your control notification handler code here
	m_hSelItem=m_treeProc.GetSelectedItem();
	//m_btnApply.EnableWindow();
}

void CEconomicaDataTypeDlg::OnKillfocusEditValue1() 
{
	// TODO: Add your control notification handler code here
	//OnButtonApply();
}

void CEconomicaDataTypeDlg::OnChangeEditValue1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_btnApply.EnableWindow(TRUE);
}

void CEconomicaDataTypeDlg::OnChangeEditValue2() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_btnApply.EnableWindow(TRUE);
	
}

void CEconomicaDataTypeDlg::OnChangeEditValue3() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_btnApply.EnableWindow(TRUE);
	
}

void CEconomicaDataTypeDlg::OnChangeEditValue4() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_btnApply.EnableWindow(TRUE);
	
}

void CEconomicaDataTypeDlg::OnChangeEditValue5() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_btnApply.EnableWindow(TRUE);
	
}


CEconomicManager* CEconomicaDataTypeDlg::GetEconomicManager()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (CEconomicManager*)&pDoc->GetEconomicManager();
}


void CEconomicaDataTypeDlg::GetDatabase()
{
	m_pDB = NULL;

	switch( m_enumType )
	{
	case ECONOMIC_LABOUR:
		m_pDB = GetEconomicManager()->m_pLaborCost;
		break;
	case ECONOMIC_PROCESSOR:
		m_pDB = GetEconomicManager()->m_pProcessorCost;
		break;
	case ECONOMIC_GATEUSAGE:
		m_pDB = GetEconomicManager()->m_pGateUsageRevenue;
		break;
	case ECONOMIC_PROCESSORUSAGE:
		m_pDB = GetEconomicManager()->m_pProcUsageRevenue;
		break;
	}
}

void CEconomicaDataTypeDlg::OnButNew() 
{
	// add new item into database
	AddNewDefaultData();

	// show the value in the GUI	
	m_treeProc.ResetTreeData( NULL );

	ShowItemData();

	m_btnSave.EnableWindow();
}



// when click new, add default value into database.
void CEconomicaDataTypeDlg::AddNewDefaultData()
{
	HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
	assert( hSelItem );
    
	ProcessorID id;
	CString str = m_treeProc.GetItemText( hSelItem );
	id.SetStrDict( GetInputTerminal()->inStrDict );
	id.setID( str );
			

	switch( m_enumType )
	{
	case ECONOMIC_LABOUR:
		{
			CProcLabrFixVarCostData* pData = new CProcLabrFixVarCostData();
			CProcEconomicDataElement* pEntry = new CProcEconomicDataElement( id );
			
			pData->SetBaseSalary( 0.0 );
			pData->SetOTPay( 0.0 );
			pData->SetShiftPeriod( 8 );
			pData->SetHourlyPay( 0.0 );				
			pEntry->setData( pData );
			m_pDB->addItem( pEntry );			
			break;
		}
		
	case ECONOMIC_PROCESSOR:
		{				
			CProcFixVarCostData* pData = new CProcFixVarCostData();
			CProcEconomicDataElement* pEntry = new CProcEconomicDataElement( id );			
			pData->SetInstallCost( 0.0 );
			pData->SetDeprPeriod( 5 );
			pData->SetOpCostPerHour( 0.0 );			
			pEntry->setData( pData );
			m_pDB->addItem( pEntry );
			break;
		}
		
	case ECONOMIC_GATEUSAGE:
		{	
			CGateUsageFeeData* pData = new CGateUsageFeeData();
			CProcEconomicDataElement* pEntry = new CProcEconomicDataElement( id );
			
			pData->SetFeePerFlt( 0.0 );
			pData->SetFeePerHour( 0.0 );
			pData->SetFeePer100lb( 0.0 );							
			pEntry->setData( pData );
			m_pDB->addItem( pEntry );			
			break;
		}
	case ECONOMIC_PROCESSORUSAGE:
		{	
			CProcUsageFeeData* pData = new CProcUsageFeeData();
			CProcEconomicDataElement* pEntry = new CProcEconomicDataElement( id );
			
			pData->SetFeePerDay( 0.0 );
			pData->SetFeePerHour( 0.0 );
			pData->SetFeePerFlt( 0.0 );
			pData->SetFeePerPax( 0.0 );		
			pData->SetFeePerBag( 0.0 );				
			pEntry->setData( pData );
			m_pDB->addItem( pEntry );			
			break;
		}
	}
}


// base on the tree selection
// return the database index of selected item or its parents.
// bOwn tells if its own database.
void CEconomicaDataTypeDlg::GetDBIdx( int& nDBIdx, BOOL& bOwn )
{
	HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
	if( hSelItem == NULL )
	{
		nDBIdx = -1;
		bOwn = FALSE;
		return;
	}
	nDBIdx = m_treeProc.GetItemData( hSelItem );
	bOwn = TRUE;
	if( nDBIdx != -1 )
		return;
	bOwn = FALSE;
	nDBIdx = -1;
	hSelItem = m_treeProc.GetParentItem( hSelItem );
	while( nDBIdx == -1 && hSelItem != NULL )
	{
		nDBIdx = m_treeProc.GetItemData( hSelItem );
		hSelItem = m_treeProc.GetParentItem( hSelItem );
	}
	
}

// base on the processor tree selection, load the data in the edits and enable the edit
void CEconomicaDataTypeDlg::ShowItemData()
{
	int nDBIdx;
	BOOL bOwn;
    GetDBIdx( nDBIdx,bOwn );

	// enable the control
	if( nDBIdx >= 0 && bOwn )
	{
		m_btnErase.EnableWindow(TRUE);
		m_btnNew.ShowWindow( SW_HIDE );
		m_btnApply.ShowWindow( SW_SHOW );
		m_btnApply.EnableWindow(FALSE);

		m_editValue1.SetReadOnly(FALSE);
		m_editValue2.SetReadOnly(FALSE);
		m_editValue3.SetReadOnly(FALSE);
		m_editValue4.SetReadOnly(FALSE);
		m_editValue5.SetReadOnly(FALSE);
	}
	else
	{
		m_btnErase.EnableWindow(FALSE);
		m_btnApply.ShowWindow( SW_HIDE );
		m_btnNew.ShowWindow( SW_SHOW );		
		m_editValue1.SetReadOnly();
		m_editValue2.SetReadOnly();
		m_editValue3.SetReadOnly();
		m_editValue4.SetReadOnly();
		m_editValue5.SetReadOnly();
	}
	
	if( nDBIdx == -1 )
	{
		ShowDefaultData();
		return;
	}
	
	switch( m_enumType )
	{
	case ECONOMIC_LABOUR:
		{		
			
			CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
			CProcLabrFixVarCostData* pData = (CProcLabrFixVarCostData*)pEntry->getData();
			
			m_strValue1.Format( "%d",pData->GetShiftPeriod() );
			m_strValue2.Format( "%.2f",pData->GetBaseSalary() );
			m_strValue3.Format( "%.2f",pData->GetHourlyPay() );
			m_strValue4.Format( "%.2f",pData->GetOTPay() );
			
			break;
		}
		
	case ECONOMIC_PROCESSOR:
		{
			
			CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
			CProcFixVarCostData* pData = (CProcFixVarCostData*)pEntry->getData();
			
			m_strValue1.Format( "%.2f",pData->GetInstallCost() );
			m_strValue2.Format( "%d",pData->GetDeprPeriod() );
			m_strValue3.Format( "%.2f",pData->GetInstallCost()/pData->GetDeprPeriod() );
			m_strValue4.Format( "%.2f",pData->GetOpCostPerHour() );
			
			break;
		}
		
	case ECONOMIC_GATEUSAGE:
		{
			
			CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
			CGateUsageFeeData* pData = (CGateUsageFeeData*)pEntry->getData();
			
			m_strValue1.Format( "%.2f",pData->GetFeePerFlt() );
			m_strValue2.Format( "%.2f",pData->GetFeePerHour() );
			m_strValue3.Format( "%.2f",pData->GetFeePer100lb() );
			
			break;
		}
		
	case ECONOMIC_PROCESSORUSAGE:
		{
			
			CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
			CProcUsageFeeData* pData = (CProcUsageFeeData*)pEntry->getData();
			
			m_strValue1.Format( "%.2f",pData->GetFeePerDay() );
			m_strValue2.Format( "%.2f",pData->GetFeePerHour() );
			m_strValue3.Format( "%.2f",pData->GetFeePerFlt() );
			m_strValue4.Format( "%.2f",pData->GetFeePerPax() );	
			m_strValue5.Format( "%.2f",pData->GetFeePerBag() );
			
			break;
		}
	}
	
	UpdateData( FALSE );	
}

