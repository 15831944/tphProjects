// PaxFlowSelectPipes.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "PaxFlowSelectPipes.h"
#include <algorithm>
#include "inputs\in_term.h"
#include "inputs\PipeDataSet.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaxFlowSelectPipes dialog


//##ModelId=3E30700E0222
CPaxFlowSelectPipes::CPaxFlowSelectPipes(InputTerminal* _pTerm,const PIPEIDX& _pipes,int _iTypeOfUsingPipes,CWnd* pParent /*=NULL*/)
	: CDialog(CPaxFlowSelectPipes::IDD, pParent)
{
	m_vPipeIdx = _pipes;
	m_pTerm = _pTerm;
	m_bSelectPipe = true;
	m_iTypeOfUsingPipes = _iTypeOfUsingPipes;
	//{{AFX_DATA_INIT(CPaxFlowSelectPipes)
	//}}AFX_DATA_INIT
}


//##ModelId=3E30700E02AE
void CPaxFlowSelectPipes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaxFlowSelectPipes)
	DDX_Control(pDX, IDC_TREE_RESULT, m_treeResult);
	DDX_Control(pDX, IDC_BUTTON_RESET, m_btnReset);
	DDX_Control(pDX, IDC_BUTTON_SELECT, m_btnSelect);
	DDX_Control(pDX, IDC_LIST_PIPES, m_listPipes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPaxFlowSelectPipes, CDialog)
	//{{AFX_MSG_MAP(CPaxFlowSelectPipes)
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, OnButtonSelect)
	ON_LBN_DBLCLK(IDC_LIST_PIPES, OnDblclkListPipes)
	ON_BN_CLICKED(ID_OK, OnOk)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(IDOK,OnUpdateTreeResult)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaxFlowSelectPipes message handlers

//##ModelId=3E30700E02CD
BOOL CPaxFlowSelectPipes::OnInitDialog() 
{
	CDialog::OnInitDialog();
//	if( m_iTypeOfUsingPipes == 1 )
//	{
//		m_bSelectPipe = false;
//		m_listPipes.EnableWindow( FALSE );
//		m_treeResult.EnableWindow(	FALSE );
//		//m_editPaht.EnableWindow( FALSE );
//		m_btnSelect.EnableWindow( FALSE );
//		m_btnReset.EnableWindow( FALSE);
//
//	}
//	else

	{
		m_bSelectPipe = true;
		ReloadList();
		DisplayPath();
	}
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




//##ModelId=3E30700E02C2
void CPaxFlowSelectPipes::ReloadList()
{
	m_listPipes.ResetContent();
	int iPipeCount = m_pTerm->m_pPipeDataSet->GetPipeCount();
	
	for( int i=0; i<iPipeCount; ++i )
	{
		std::vector<int>::iterator iterFind	= std::find( m_vPipeIdx.begin(),m_vPipeIdx.end(), i );
		if( iterFind == m_vPipeIdx.end() )
		{
			int iIdx = m_listPipes.AddString( m_pTerm->m_pPipeDataSet->GetPipeAt( i )->GetPipeName() );
			m_listPipes.SetItemData( iIdx, i );	
		}		
	}
}
//##ModelId=3E30700E02CC
void CPaxFlowSelectPipes::DisplayPath()
{
	//m_pathValue = "";
	m_treeResult.DeleteAllItems();
	int iCount = m_vPipeIdx.size();
	if ( iCount <=0 )
		return ;
	HTREEITEM hItem = m_treeResult.InsertItem( "Selected Pipes " );
	HTREEITEM hExpand;
	for( int i=0; i < iCount; ++i )
	{
		hExpand = hItem;		
		hItem = m_treeResult.InsertItem( m_pTerm->m_pPipeDataSet->GetPipeAt( m_vPipeIdx[ i ] )->GetPipeName(),hItem );
		m_treeResult.Expand( hExpand , TVE_EXPAND );
		//m_pathValue += m_pTerm->m_pPipeDataSet->GetPipeAt( m_vPipeIdx[ i ] )->GetPipeName();
		//m_pathValue += " --> ";
	}
	//m_pathValue += m_pTerm->m_pPipeDataSet->GetPipeAt( m_vPipeIdx[ iCount-1 ] )->GetPipeName();
	//this->UpdateData( false );
}

//##ModelId=3E30700E02D7
void CPaxFlowSelectPipes::OnButtonReset() 
{
	m_vPipeIdx.clear();
	ReloadList();
	//m_pathValue ="";
	m_treeResult.DeleteAllItems();	
	//UpdateData( false );
}

//##ModelId=3E30700E0309
void CPaxFlowSelectPipes::OnButtonSelect() 
{
	int iSelected = m_listPipes.GetCurSel();
	int iPipeIdx = (int)m_listPipes.GetItemData( iSelected );
	if ( ( iPipeIdx >= 0 && iPipeIdx < m_pTerm->m_pPipeDataSet->GetPipeCount() ) )
	{
		if( m_vPipeIdx.size() > 0 )
		{
			CPipe* pSelectPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( iPipeIdx );
			ASSERT( pSelectPipe );
			
			CPipe* pOtherSelectPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( m_vPipeIdx[ m_vPipeIdx.size() -1 ] );
			ASSERT( pOtherSelectPipe );

			if( ! pSelectPipe->IfIntersectionWithOtherPipe( pOtherSelectPipe ) )
			{
				CString sMessage( " Pipe: " );
				sMessage += pSelectPipe->GetPipeName();
				sMessage += " and Pipe : ";
				sMessage += pOtherSelectPipe->GetPipeName();
				sMessage += " don not intersect with each other ! ";
				AfxMessageBox( sMessage );
				return;
			}
		}

		m_vPipeIdx.push_back( iPipeIdx );
		ReloadList();
		DisplayPath();	
	}
}

//##ModelId=3E30700E0313
void CPaxFlowSelectPipes::OnDblclkListPipes() 
{
	int iSelected = m_listPipes.GetCurSel();
	int iPipeIdx = (int)m_listPipes.GetItemData( iSelected );
	ASSERT( iPipeIdx >= 0 && iPipeIdx < m_pTerm->m_pPipeDataSet->GetPipeCount() );

	if( m_vPipeIdx.size() > 0 )
	{
		CPipe* pSelectPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( iPipeIdx );
		ASSERT( pSelectPipe );
		
		CPipe* pOtherSelectPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( m_vPipeIdx[ m_vPipeIdx.size() -1 ] );
		ASSERT( pOtherSelectPipe );

		if( ! pSelectPipe->IfIntersectionWithOtherPipe( pOtherSelectPipe ) )
		{
			CString sMessage( " Pipe: " );
			sMessage += pSelectPipe->GetPipeName();
			sMessage += " and Pipe : ";
			sMessage += pOtherSelectPipe->GetPipeName();
			sMessage += " don not intersect with each other ! ";
			AfxMessageBox( sMessage );
			return;
		}
	}

	
	m_vPipeIdx.push_back( iPipeIdx );
	ReloadList();
	DisplayPath();	
	
}

BOOL CPaxFlowSelectPipes::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateDialogControls(this,TRUE);
	return CDialog::PreTranslateMessage(pMsg);
}

void CPaxFlowSelectPipes::OnUpdateTreeResult(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_treeResult.GetCount()>0);
}



void CPaxFlowSelectPipes::OnOk() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}
