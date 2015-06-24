// ProcesserDialog.cpp ;: implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ProcesserDialog.h"

#include "inputs\SubFlowList.h"
#include "inputs\SubFlow.h"
#include "inputs\in_term.h"
#include "engine\proclist.h"
#include "engine\gate.h"
#include "Engine\conveyor.h"
#include "Inputs\miscproc.h"
#include "Inputs\procdata.h"
#include "TermPlanDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProcesserDialog dialog


CProcesserDialog::CProcesserDialog( InputTerminal* _pInTerm, CWnd* pParent /*=NULL*/ )
	: CDialog(CProcesserDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProcesserDialog)
	m_strProcID = _T("");
	//}}AFX_DATA_INIT

	m_nCurSel = -1;
	m_pInTerm = _pInTerm;
	m_nProcType = -1;
	m_nProcType2 = -1;
	
	m_bDisplayBaggageDeviceSign = false; 
	m_bDisplayProcess = false;

	m_bSelectProcessor = true;
	m_iFilterFlag = FilterByProcType;
	m_bOnlyShowACStandGate = false;
	m_bOnlyShowPusherConveyor = false;
}


void CProcesserDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcesserDialog)
	DDX_Control(pDX, IDCANCEL, m_btCancle);
	DDX_Control(pDX, ID_BTN_SELECT, m_btOk);
	DDX_Control(pDX, IDC_EDIT_PROCESSORID, m_editResult);
	DDX_Control(pDX, IDC_PROCESS_LIST, m_listProcess);
	DDX_Control(pDX, IDC_TREE_PROCESSOR, m_ProcTree);
	DDX_Text(pDX, IDC_EDIT_PROCESSORID, m_strProcID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProcesserDialog, CDialog)
	//{{AFX_MSG_MAP(CProcesserDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_PROCESSOR, OnDblclkTreeProcessor)
	ON_BN_CLICKED(ID_BTN_SELECT, OnBtnSelect)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROCESSOR, OnSelchangedTreeProcessor)
	ON_LBN_SELCHANGE(IDC_PROCESS_LIST, OnSelchangeProcessList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcesserDialog message handlers

BOOL CProcesserDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
    
	AfxGetApp()->BeginWaitCursor();
	if(m_pInTerm == NULL)
		return TRUE;
	ProcessorList *procList = m_pInTerm->GetTerminalProcessorList();
	if (procList == NULL)
		return TRUE;

	// Set data to processor tree
	ProcessorID id, *pid = NULL;
	id.SetStrDict( m_pInTerm->inStrDict );
	int includeBarriers = 0;
	CString strTemp, strTemp1, strTemp2, strTemp3, str;

	TVITEM ti;
	TVINSERTSTRUCT tis;
	HTREEITEM hItem = NULL, hItem1 = NULL, hItem2 = NULL, hItem3 = NULL;	
	int i = 0, j = 0;
	
	StringList strDict;
	//modify by bird
	if( m_iFilterFlag == FilterByProcType )
	{
		procList->getAllGroupNames (strDict, m_nProcType);
	}
	else			   //FilterByProcTypeList
	{
		for( int ii =0; ii<static_cast<int>(m_vProcType.size()); ii++ )
		{
			procList->getAllGroupNames(strDict, m_vProcType[ii] );
		}
	}

	if (includeBarriers && !strDict.getCount())
		procList->getMemberNames (id, strDict, BarrierProc);

	/*///////////////////////////////////////////////////////////////////////////
	const Processor* pProcStart =procList->getProcessor( START_PROCESSOR_INDEX );
	const Processor* pProcEnd = procList->getProcessor( END_PROCESSOR_INDEX );

	m_vectID.push_back( *( pProcStart->getID() ) );
	m_vectID.push_back( *(pProcEnd->getID() ));
	CString sDisplay = " ";
	sDisplay += pProcStart->getID()->GetIDString();
	m_ProcTree.SetItemData(m_ProcTree.InsertItem( sDisplay ), 0 );

	sDisplay = " ";
	sDisplay += pProcEnd->getID()->GetIDString();
	m_ProcTree.SetItemData(m_ProcTree.InsertItem( sDisplay ), 1 );
	*/////////////////////////////////////////////////////////////////////////////
	if( m_bDisplayBaggageDeviceSign)
	{
		const Processor* pProcBaggageDevice =procList->getProcessor( BAGGAGE_DEVICE_PROCEOR_INDEX );
		m_vectID.push_back( *( pProcBaggageDevice->getID() ) );	
		CString sDisplay = " ";
		sDisplay += pProcBaggageDevice->getID()->GetIDString();
		m_ProcTree.SetItemData(m_ProcTree.InsertItem( sDisplay ), 0 );
	}
	
	for (j = 0; j < strDict.getCount(); j++)   // Level 1
	{
		strTemp = strDict.getString (j);
		strTemp.TrimLeft(); strTemp.TrimRight();
		if (strTemp.IsEmpty())
			continue;
		
		//check AC Stand Gate Flag
		if( !CheckACStandFlag( strTemp) )
			continue;
		if( !CheckPusherConveyor( strTemp) )
			continue;

		
		ti.mask       = TCIF_TEXT;
		ti.cChildren  = 1;
		ti.pszText    = strTemp.GetBuffer(0);
		ti.cchTextMax = 256;
		
		tis.hParent = TVI_ROOT;
		tis.hInsertAfter = TVI_SORT;
		tis.item = ti;
			
		hItem = m_ProcTree.InsertItem(&tis);
		if (hItem == NULL)
			continue;
		
		str = strTemp;
		id.setID((LPCSTR) str);

		//Added by Xie Bo for debug
//		char buf[256];
//		id.printID(buf,"-");
//		AfxMessageBox(buf);


		m_vectID.push_back( id );
		m_ProcTree.SetItemData(hItem, (DWORD) ( m_vectID.size()-1 ) );
		
		StringList strDict1;
		//modify by bird 2003/7/15
		if( m_iFilterFlag == FilterByProcType )
		{
			procList->getMemberNames (id, strDict1, m_nProcType);
		}
		else			  //FilterByProcTypeList
		{
			for( int ii=0; ii<static_cast<int>(m_vProcType.size()); ii++ )
				procList->getMemberNames( id, strDict1, m_vProcType[ii] );
		}

		for (int m = 0; m < strDict1.getCount(); m++)   // Level 2
		{
			strTemp1 = strDict1.getString (m);
			strTemp1.TrimLeft(); strTemp1.TrimRight();
			if (strTemp1.IsEmpty())
				continue;
			
			//check AC Stand Gate Flag
			if( !CheckACStandFlag( strTemp+"-"+strTemp1) )
				continue;
			if( !CheckPusherConveyor( strTemp+"-"+strTemp1) )
				continue;

			ti.pszText  = strTemp1.GetBuffer(0);
			tis.hParent = hItem;
			tis.item = ti;
			
			hItem1 = m_ProcTree.InsertItem(&tis);
			if (hItem1 == NULL)
				continue;
			
			str = strTemp;
			str += "-";
			str += strTemp1;

			id.setID((LPCSTR) str);

			m_vectID.push_back( id );
			m_ProcTree.SetItemData(hItem1, (DWORD) ( m_vectID.size()-1 ) );

			StringList strDict2;
			// modify by bird 2003/7/15
			if( m_iFilterFlag == FilterByProcType )
			{
				procList->getMemberNames (id, strDict2, m_nProcType);
			}
			else			   //FilterByProcTypeList
			{
				for( int ii=0; ii<static_cast<int>(m_vProcType.size()); ii++ )
					procList->getMemberNames( id,strDict2, m_vProcType[ii] );
			}

			for (int n = 0; n < strDict2.getCount(); n++)   // Level 3
			{
				strTemp2 = strDict2.getString (n);
				strTemp2.TrimLeft(); strTemp2.TrimRight();
				if (strTemp2.IsEmpty())
					continue;
				
				//check AC Stand Gate Flag
				if( !CheckACStandFlag( strTemp+"-"+strTemp1+"-"+strTemp2) )
					continue;
				if( !CheckPusherConveyor( strTemp+"-"+strTemp1+"-"+strTemp2) )
					continue;

				ti.pszText  = strTemp2.GetBuffer(0);
				tis.hParent = hItem1;
				tis.item    = ti;
				
				hItem2 = m_ProcTree.InsertItem(&tis);
				if (hItem2 == NULL)
					continue;

				str = strTemp;   str += "-";
				str += strTemp1; str += "-";
				str += strTemp2;

				id.setID((LPCSTR) str);

				m_vectID.push_back( id );
				m_ProcTree.SetItemData(hItem2, (DWORD) ( m_vectID.size()-1 ) );

				StringList strDict3;
				//modify by bird 2003/7/15
				if( m_iFilterFlag == FilterByProcType )
				{
					procList->getMemberNames (id, strDict3, m_nProcType);
				}
				else			  //FilterByProcTypeList
				{
					for( int ii=0; ii<static_cast<int>(m_vProcType.size()); ii++ )
						procList->getMemberNames( id, strDict3, m_vProcType[ii] );
				}
				for (int x = 0; x < strDict3.getCount(); x++)   // Level 4
				{
					strTemp3 = strDict3.getString (x);
					strTemp3.TrimLeft(); strTemp3.TrimRight();
					if (strTemp3.IsEmpty())
						continue;

					//check AC Stand Gate Flag
					if( !CheckACStandFlag( strTemp+"-"+strTemp1+"-"+strTemp2+"-"+strTemp3) )
						continue;
					if( !CheckPusherConveyor( strTemp+"-"+strTemp1+"-"+strTemp2+"-"+strTemp3) )
						continue;

					ti.pszText  = strTemp3.GetBuffer(0);
					tis.hParent = hItem2;
					tis.item    = ti;
					
					hItem3 = m_ProcTree.InsertItem(&tis);

					str = strTemp;   str += "-";
					str += strTemp1; str += "-";
					str += strTemp2; str += "-";
					str += strTemp3;
					
					id.setID((LPCSTR) str);

					m_vectID.push_back( id );
					m_ProcTree.SetItemData(hItem3, (DWORD) ( m_vectID.size()-1 ) );
				}
			}
		}
	}

	if (m_nProcType2 != -1)
	{

		LoadProcType2Proc();
	}

	if( !m_bDisplayProcess )
	{
		m_listProcess.ShowWindow( SW_HIDE );
		CRect reWindow;
		this->GetWindowRect( reWindow );
		this->SetWindowPos( &wndTop, reWindow.TopLeft().x, reWindow.TopLeft().y, 350,410, SWP_SHOWWINDOW );
		this->CenterWindow();
	}
	else
	{
		CRect reWindow;
		this->GetWindowRect( reWindow );
		CRect editWin;
		m_editResult.GetWindowRect( editWin );
		m_editResult.ScreenToClient( editWin );
		m_editResult.MoveWindow( editWin.TopLeft().x+10 ,editWin.TopLeft().y+10, reWindow.Width() - 35,editWin.Height() );

		this->ScreenToClient( reWindow );
		m_btOk.GetWindowRect( editWin );
		//m_btOk.MoveWindow(  editWin.left - reWindow.left + 358, reWindow.bottom -20 ,editWin.Width(), editWin.Height() );
		m_btOk.MoveWindow(  reWindow.right -editWin.Width()*2 -40 , reWindow.bottom -35 ,editWin.Width(), editWin.Height() );

		m_btCancle.GetWindowRect( editWin );
		//m_btCancle.MoveWindow(  editWin.left - reWindow.left + 358, reWindow.bottom -20 ,  editWin.Width(), editWin.Height() );
		m_btCancle.MoveWindow(  reWindow.right -editWin.Width() -20, reWindow.bottom -35 ,  editWin.Width(), editWin.Height() );

		int iProcessCount = m_pInTerm->m_pSubFlowList->GetProcessUnitCount();
		for( int i=0; i<iProcessCount; ++i )
		{
			CSubFlow* pFlow =  m_pInTerm->m_pSubFlowList->GetProcessUnitAt( i );
			ASSERT( pFlow  );
			m_listProcess.AddString(  pFlow->GetProcessUnitName() );			
		}
	}
	AfxGetApp()->EndWaitCursor();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProcesserDialog::OnDblclkTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	*pResult = 0;
}

void CProcesserDialog::OnBtnSelect() 
{
	if( m_bSelectProcessor )
	{
		HTREEITEM hItem = m_ProcTree.GetSelectedItem();
		if (hItem == NULL) return;
		
		m_nCurSel = m_ProcTree.GetItemData(hItem);

		m_vectSel.clear();
		HTREEITEM hSel = m_ProcTree.GetFirstSelectedItem();
		while ( hSel != NULL )
		{
			int nCurSel = m_ProcTree.GetItemData(hSel);
			if ( nCurSel >= 0)
			{
				m_vectSel.push_back(nCurSel); 
			}
			
			hSel = m_ProcTree.GetNextSelectedItem(hSel);
		}
		

	}
	else
	{
		if( m_sSelectedProcess.IsEmpty() )
			return;
	}
	
	CDialog::OnOK();
}

void CProcesserDialog::OnSelchangedTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hItem = NULL;
	hItem = pNMTreeView->itemNew.hItem;
	if (hItem == NULL) return;
	
	m_nCurSel = m_ProcTree.GetItemData(hItem);

	m_vectSel.clear();
	HTREEITEM hSel = m_ProcTree.GetFirstSelectedItem();
	while ( hSel != NULL )
	{
		int nCurSel = m_ProcTree.GetItemData(hSel);
		if ( nCurSel >= 0)
		{
			m_vectSel.push_back(nCurSel); 
		}
		
		hSel = m_ProcTree.GetNextSelectedItem(hSel);
	}	

	char szBuf[256];
	m_vectID[m_nCurSel].printID(szBuf);
	
	GetDlgItem(IDC_EDIT_PROCESSORID)->SetWindowText(szBuf);

	m_bSelectProcessor = true;
	
	*pResult = 0;
}


bool CProcesserDialog::GetProcessorID( ProcessorID& _ID )
{ 
	if( m_bSelectProcessor )
	{
		if( m_nCurSel >= 0 )
		{
			_ID = m_vectID[m_nCurSel];   
			return true;
		}
	}
	else
	{
		
		_ID.SetStrDict( m_pInTerm->inStrDict );
		_ID.setID( m_sSelectedProcess );
		return true;
	}
	
	return false;
}

bool CProcesserDialog::GetProcessorIDList(ProcessorIDList &_IDlist)
{

	int nSize = m_vectSel.size();
	for(int i = 0; i < nSize; i++)
	{
		int nCurSel = 0;
		nCurSel = m_vectSel[i];
		if ( nCurSel >= 0)
		{
			_IDlist.addItem(&m_vectID[nCurSel]);
		}		
	}
	if ( _IDlist.getCount() > 0 )
	{
		return true;		
	}
	
	return false;	
}

void CProcesserDialog::SetType( int _nProcType )
{
	m_iFilterFlag = FilterByProcType;
	m_nProcType = _nProcType;
}
void CProcesserDialog::SetType2( int _nProcType )
{
	m_iFilterFlag = FilterByProcType;
	m_nProcType2 = _nProcType;
}

void CProcesserDialog::SetTypeList( const std::vector<int>& _nProcTypeList )
{
	m_iFilterFlag = FilterByProcTypeList;
	m_vProcType = _nProcTypeList;
}
void CProcesserDialog::OnSelchangeProcessList() 
{
	m_bSelectProcessor = false;

	int iSelectedProcess = m_listProcess.GetCurSel( );
	if( iSelectedProcess == LB_ERR  )
		return;
	m_listProcess.GetText( iSelectedProcess, m_sSelectedProcess );

	GetDlgItem(IDC_EDIT_PROCESSORID)->SetWindowText(m_sSelectedProcess);
}


// if return false, need not to add the string to tree
// else need
bool CProcesserDialog::CheckACStandFlag( const CString& procIDString)
{
	// if need to check ac stand flag
	if( m_bOnlyShowACStandGate )
	{
		// get all processor index
		ProcessorID procID;
		procID.SetStrDict( m_pInTerm->inStrDict );
		procID.setID( procIDString );
		GroupIndex group = m_pInTerm->GetProcessorList()->getGroupIndex( procID );
		for( int i= group.start; i<= group.end; i++ )
		{
			// for each processor
			// if is not gate_proc, return true( need to add the string to tree
			Processor* proc = m_pInTerm->procList->getProcessor( i );
			assert( proc );
			if( proc->getProcessorType() != GateProc )
				return true;
			// if the AcStandGateFlag of one processor is true.
			// return true
			GateProcessor* pGate = (GateProcessor*)proc;
			if( pGate->getACStandGateFlag() )
				return true;
		}
		// if the AcStandGateFlag of all processor all is false.
		// return false
		return false;
	}
	
	return true;
}
void CProcesserDialog::LoadProcType2Proc()
{
	ProcessorList *procList = m_pInTerm->GetAirsideProcessorList();
	ProcessorID id, *pid = NULL;
	id.SetStrDict( m_pInTerm->inStrDict );
	int includeBarriers = 0;
	CString strTemp, strTemp1, strTemp2, strTemp3, str;

	TVITEM ti;
	TVINSERTSTRUCT tis;
	HTREEITEM hItem = NULL, hItem1 = NULL, hItem2 = NULL, hItem3 = NULL;	
	int i = 0, j = 0;

	StringList strDict;
	//modify by bird
	if( m_iFilterFlag == FilterByProcType )
	{
		procList->getAllGroupNames (strDict, m_nProcType2);
	}
	else			   //FilterByProcTypeList
	{
		for( int ii =0; ii<static_cast<int>(m_vProcType.size()); ii++ )
		{
			procList->getAllGroupNames(strDict, m_vProcType[ii] );
		}
	}

	if (includeBarriers && !strDict.getCount())
		procList->getMemberNames (id, strDict, BarrierProc);

	/*///////////////////////////////////////////////////////////////////////////
	const Processor* pProcStart =procList->getProcessor( START_PROCESSOR_INDEX );
	const Processor* pProcEnd = procList->getProcessor( END_PROCESSOR_INDEX );

	m_vectID.push_back( *( pProcStart->getID() ) );
	m_vectID.push_back( *(pProcEnd->getID() ));
	CString sDisplay = " ";
	sDisplay += pProcStart->getID()->GetIDString();
	m_ProcTree.SetItemData(m_ProcTree.InsertItem( sDisplay ), 0 );

	sDisplay = " ";
	sDisplay += pProcEnd->getID()->GetIDString();
	m_ProcTree.SetItemData(m_ProcTree.InsertItem( sDisplay ), 1 );
	*/////////////////////////////////////////////////////////////////////////////
	if( m_bDisplayBaggageDeviceSign)
	{
		const Processor* pProcBaggageDevice =procList->getProcessor( BAGGAGE_DEVICE_PROCEOR_INDEX );
		m_vectID.push_back( *( pProcBaggageDevice->getID() ) );	
		CString sDisplay = " ";
		sDisplay += pProcBaggageDevice->getID()->GetIDString();
		m_ProcTree.SetItemData(m_ProcTree.InsertItem( sDisplay ), 0 );
	}

	for (j = 0; j < strDict.getCount(); j++)   // Level 1
	{
		strTemp = strDict.getString (j);
		strTemp.TrimLeft(); strTemp.TrimRight();
		if (strTemp.IsEmpty())
			continue;

		//check AC Stand Gate Flag
//		if( !CheckACStandFlag( strTemp) )
//			continue;

		ti.mask       = TCIF_TEXT;
		ti.cChildren  = 1;
		ti.pszText    = strTemp.GetBuffer(0);
		ti.cchTextMax = 256;

		tis.hParent = TVI_ROOT;
		tis.hInsertAfter = TVI_SORT;
		tis.item = ti;

		hItem = m_ProcTree.InsertItem(&tis);
		if (hItem == NULL)
			continue;

		str = strTemp;
		id.setID((LPCSTR) str);

		//Added by Xie Bo for debug
		//		char buf[256];
		//		id.printID(buf,"-");
		//		AfxMessageBox(buf);


		m_vectID.push_back( id );
		m_ProcTree.SetItemData(hItem, (DWORD) ( m_vectID.size()-1 ) );

		StringList strDict1;
		//modify by bird 2003/7/15
		if( m_iFilterFlag == FilterByProcType )
		{
			procList->getMemberNames (id, strDict1, m_nProcType2);
		}
		else			  //FilterByProcTypeList
		{
			for( int ii=0; ii<static_cast<int>(m_vProcType.size()); ii++ )
				procList->getMemberNames( id, strDict1, m_vProcType[ii] );
		}

		for (int m = 0; m < strDict1.getCount(); m++)   // Level 2
		{
			strTemp1 = strDict1.getString (m);
			strTemp1.TrimLeft(); strTemp1.TrimRight();
			if (strTemp1.IsEmpty())
				continue;

			//check AC Stand Gate Flag
//			if( !CheckACStandFlag( strTemp+"-"+strTemp1) )
//				continue;

			ti.pszText  = strTemp1.GetBuffer(0);
			tis.hParent = hItem;
			tis.item = ti;

			hItem1 = m_ProcTree.InsertItem(&tis);
			if (hItem1 == NULL)
				continue;

			str = strTemp;
			str += "-";
			str += strTemp1;

			id.setID((LPCSTR) str);

			m_vectID.push_back( id );
			m_ProcTree.SetItemData(hItem1, (DWORD) ( m_vectID.size()-1 ) );

			StringList strDict2;
			// modify by bird 2003/7/15
			if( m_iFilterFlag == FilterByProcType )
			{
				procList->getMemberNames (id, strDict2, m_nProcType2);
			}
			else			   //FilterByProcTypeList
			{
				for( int ii=0; ii<static_cast<int>(m_vProcType.size()); ii++ )
					procList->getMemberNames( id,strDict2, m_vProcType[ii] );
			}

			for (int n = 0; n < strDict2.getCount(); n++)   // Level 3
			{
				strTemp2 = strDict2.getString (n);
				strTemp2.TrimLeft(); strTemp2.TrimRight();
				if (strTemp2.IsEmpty())
					continue;

				//check AC Stand Gate Flag
//				if( !CheckACStandFlag( strTemp+"-"+strTemp1+"-"+strTemp2) )
//					continue;

				ti.pszText  = strTemp2.GetBuffer(0);
				tis.hParent = hItem1;
				tis.item    = ti;

				hItem2 = m_ProcTree.InsertItem(&tis);
				if (hItem2 == NULL)
					continue;

				str = strTemp;   str += "-";
				str += strTemp1; str += "-";
				str += strTemp2;

				id.setID((LPCSTR) str);

				m_vectID.push_back( id );
				m_ProcTree.SetItemData(hItem2, (DWORD) ( m_vectID.size()-1 ) );

				StringList strDict3;
				//modify by bird 2003/7/15
				if( m_iFilterFlag == FilterByProcType )
				{
					procList->getMemberNames (id, strDict3, m_nProcType2);
				}
				else			  //FilterByProcTypeList
				{
					for( int ii=0; ii<static_cast<int>(m_vProcType.size()); ii++ )
						procList->getMemberNames( id, strDict3, m_vProcType[ii] );
				}
				for (int x = 0; x < strDict3.getCount(); x++)   // Level 4
				{
					strTemp3 = strDict3.getString (x);
					strTemp3.TrimLeft(); strTemp3.TrimRight();
					if (strTemp3.IsEmpty())
						continue;

					//check AC Stand Gate Flag
//					if( !CheckACStandFlag( strTemp+"-"+strTemp1+"-"+strTemp2+"-"+strTemp3) )
//						continue;

					ti.pszText  = strTemp3.GetBuffer(0);
					tis.hParent = hItem2;
					tis.item    = ti;

					hItem3 = m_ProcTree.InsertItem(&tis);

					str = strTemp;   str += "-";
					str += strTemp1; str += "-";
					str += strTemp2; str += "-";
					str += strTemp3;

					id.setID((LPCSTR) str);

					m_vectID.push_back( id );
					m_ProcTree.SetItemData(hItem3, (DWORD) ( m_vectID.size()-1 ) );
				}
			}
		}
	}	

}

bool CProcesserDialog::CheckPusherConveyor( const CString& procIDString )
{
	// if need to check ac stand flag
	if( m_bOnlyShowPusherConveyor )
	{
		// get all processor index
		ProcessorID procID;
		procID.SetStrDict( m_pInTerm->inStrDict );
		procID.setID( procIDString );
		GroupIndex group = m_pInTerm->GetProcessorList()->getGroupIndex( procID );
		for( int i= group.start; i<= group.end; i++ )
		{
			// for each processor
			// if is not gate_proc, return true( need to add the string to tree
			Processor* proc = m_pInTerm->procList->getProcessor( i );
			assert( proc );
			if( proc->getProcessorType() != ConveyorProc )
				return true;
			// if the AcStandGateFlag of one processor is true.
			// return true
			//check
			Conveyor * pConveyor = (Conveyor*)proc;
			MiscProcessorData* pMiscDB = m_pInTerm->miscData->getDatabase( (int)ConveyorProc );
			for(int j=0;j<pMiscDB->getCount();j++)
			{				
				MiscDataElement* pDatElement = (MiscDataElement*)pMiscDB->getItem(j);
				MiscConveyorData* pConveyData =  (MiscConveyorData*) pDatElement->getData();
				if( pConveyData->GetSubType() == PUSHER )
				{
					if(pDatElement->getID()->idFits(*proc->getID()))
						return true;
				}
			}
		}
		// if the AcStandGateFlag of all processor all is false.
		// return false
		return false;
	}

	return true;
}
