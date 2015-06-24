// BoardingCallDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "BoardingCallDlg.h"
#include "termplandoc.h"
#include "flightdialog.h"
#include "..\inputs\FlightConWithProcIDDatabase.h"
#include "..\inputs\fltdata.h"
#include "..\common\probdistmanager.h"
#include "..\Inputs\procdata.h"
#include "..\inputs\probab.h"
//#include "ProbDistDlg.h"
#include "DlgProbDIst.h"
#include "ProcesserDialog.h"
#include "SelectALTObjectDialog.h"
#include <Common/ProbabilityDistribution.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBoardingCallDlg dialog


CBoardingCallDlg::CBoardingCallDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBoardingCallDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBoardingCallDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBoardingCallDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBoardingCallDlg)
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_TREE1, m_tree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBoardingCallDlg, CDialog)
	//{{AFX_MSG_MAP(CBoardingCallDlg)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(ID_ADD_STAGE, OnToolbarButtonAdd)
	ON_COMMAND(ID_DELETE, OnToolbarButtonDel)
	ON_COMMAND(ID_EDIT,OnToolbarButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelchangedTree1)
	ON_COMMAND(ID_ADD_FLIGHT, OnToolbarButtonAdd)
	ON_COMMAND(ID_CHANGE_PROCESSOR, OnChangeProcessor)
	ON_COMMAND(ID_DEFAULT_PROCESSOR, OnDefaultProcessor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBoardingCallDlg message handlers

int CBoardingCallDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_BOARDING_CALL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	
	return 0;
}

BOOL CBoardingCallDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CRect rect;
	m_tree.GetWindowRect(rect);
	ScreenToClient(rect);
	m_wndToolBar.SetWindowPos(NULL,rect.left,rect.top-26,rect.Width(),26,NULL);
	m_wndToolBar.GetToolBarCtrl().EnableButton( ID_ADD_STAGE );
	m_wndToolBar.GetToolBarCtrl().EnableButton( ID_DELETE,FALSE );
	m_wndToolBar.GetToolBarCtrl().EnableButton( ID_EDIT,FALSE );
	
	LoadTreeItems();
	m_tree.Expand(m_tree.GetRootItem(),TVE_EXPAND);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

InputTerminal* CBoardingCallDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
	
}

void CBoardingCallDlg::OnToolbarButtonDel()
{
	if( DeleteStageChild())
		m_btnSave.EnableWindow();
}

void CBoardingCallDlg::OnToolbarButtonAdd()
{
	HTREEITEM hItemSeled=m_tree.GetSelectedItem();
	HTREEITEM hItemParent=m_tree.GetParentItem(hItemSeled);
	if(hItemParent==NULL)//Add a stage
	{
		GetInputTerminal()->flightData->AddStage();
		int nDBCount=GetConstraintDBCount();
		FlightConWithProcIDDatabase* pConDB = GetConstraintDatabase(nDBCount);
		HTREEITEM hItemLast=AddStage(pConDB,nDBCount);
		m_btnSave.EnableWindow();	
		m_tree.SelectItem(hItemLast);
		m_tree.Expand(hItemLast,TVE_EXPAND);
		return;
	}
	//Add a item to selected stage	
	BOOL bDirty = FALSE;
	int nItemData=m_tree.GetItemData(hItemSeled);
	FlightConWithProcIDDatabase* pConDB = (FlightConWithProcIDDatabase*)nItemData;
	assert( pConDB );
	CFlightDialog dlg( m_pParentWnd );
	if( dlg.DoModal() == IDOK )
	{
		FlightConstraint newFltCon = dlg.GetFlightSelection();
		bDirty = NewStageChild(m_tree.GetSelectedItem(),&newFltCon ,nItemData);
	}
	if( bDirty )
		m_btnSave.EnableWindow();	
}

FlightConWithProcIDDatabase* CBoardingCallDlg::GetConstraintDatabase(int nStageIndex)
{
	FlightConWithProcIDDatabase* pConDB = NULL;
	pConDB = GetInputTerminal()->flightData->getCalls(nStageIndex);
	return pConDB;
}

void CBoardingCallDlg::OnButtonSave()
{
	CWaitCursor wc;
	GetInputTerminal()->flightData->resortBoardingCallDB();
	GetInputTerminal()->flightData->saveDataSet( GetProjPath(), true );
	m_btnSave.EnableWindow( FALSE );
}

CString CBoardingCallDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
	
}

void CBoardingCallDlg::OnCancel() 
{
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnCancel();
		return;
	}
	AfxGetApp()->BeginWaitCursor();
	
	try
	{
		GetInputTerminal()->flightData->loadDataSet( GetProjPath() );
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
		delete _pError;			
	}
	AfxGetApp()->EndWaitCursor();
	
	CDialog::OnCancel();
}

void CBoardingCallDlg::OnOK() 
{
	// TODO: Add extra validation here
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnOK();
		return;
	}
	
	AfxGetApp()->BeginWaitCursor();
	OnButtonSave();
	AfxGetApp()->EndWaitCursor();

	CDialog::OnOK();
}

HTREEITEM CBoardingCallDlg::LoadTreeItems(Constraint* _pSelCon)
{
	m_tree.DeleteAllItems();
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	m_hBoardingCalls=m_tree.InsertItem("Boarding Calls",cni,FALSE);
	int nDBCount=GetConstraintDBCount();
	CString strStageName;
	HTREEITEM hItemSeled=NULL,hItemLast;
	for(int i=0;i<nDBCount;i++)
	{
		FlightConWithProcIDDatabase* pConDB = GetConstraintDatabase(1+i);
		strStageName.Format("Stage %d",i+1);
		HTREEITEM hStage1=m_tree.InsertItem(strStageName,cni,FALSE,FALSE,m_hBoardingCalls);
		hItemLast=hStage1;
		m_tree.SetItemData( hStage1,(DWORD)pConDB);
		int nCount = pConDB->getCount();
		for( int k=0; k<nCount; k++ )
		{
			ConstraintWithProcIDEntry* pEntry = (ConstraintWithProcIDEntry*)(pConDB->getItem( k ));
			HTREEITEM hItem=AddStageChild2Tree(hStage1,pEntry);
			
			const Constraint* pCon = pEntry->getConstraint();
			if( pCon == _pSelCon )
			{
				hItemSeled=hItem;
			}	
		}		
	}

	if(hItemSeled)
	{
		m_tree.SelectItem(hItemSeled);
		m_tree.Expand(hItemSeled,TVE_EXPAND);

	}

	return hItemLast;
}

BOOL CBoardingCallDlg::NewStageChild(HTREEITEM hItem,Constraint* _pNewCon,DWORD pStage)
{		
	FlightConWithProcIDDatabase* pConDB =(FlightConWithProcIDDatabase*)pStage;
	HTREEITEM hItemChild=m_tree.GetChildItem(hItem);
	int i=0;
	// can not check if exist, because ...
	/*********
	// check if exist 	
	while(hItemChild)
	{
		int nIdx=m_tree.GetItemData(hItemChild);
		const FlightConstraint* pFltCon = ((FlightConstraintDatabase*)pConDB)->getConstraint((ConstraintWithProcIDEntry*)nIdx);
		if( _pNewCon->isEqual(pFltCon) )
		{
			// select the item
			m_tree.SelectItem(hItemChild);
			m_tree.Expand(hItemChild,TVE_EXPAND);
			return FALSE;
		}
		hItemChild=m_tree.GetNextSiblingItem(hItemChild);
		i++;
	}
	**********/
	
	FlightConstraint* pFltCon = new FlightConstraint;
	*pFltCon = *(FlightConstraint*)_pNewCon;
	ConstraintWithProcIDEntry* pNewEntry = new ConstraintWithProcIDEntry();
	//ConstantDistribution* pDist = new ConstantDistribution( 0 );
	// default distribution
	double vals[3] = {-1200, -1000, -600};
	double probs[3] = {20, 90, 100};
    ProbabilityDistribution *defaultDist = new HistogramDistribution (3, vals, probs);

	ProcessorID id;
	id.SetStrDict( GetInputTerminal()->inStrDict );

	pNewEntry->initialize( pFltCon, defaultDist, id);
	pConDB->addEntry( pNewEntry );
	AddStageChild2Tree(hItem,pNewEntry);
	return TRUE;
}

HTREEITEM CBoardingCallDlg::AddStageChild2Tree(HTREEITEM hItemParent, ConstraintWithProcIDEntry* pEntry)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	const Constraint* pCon = pEntry->getConstraint();
	CString szName;
	pCon->screenPrint(szName, 0, 256 );
	CString strName="Flight Type: ";
	strName+=szName;
	const ProbabilityDistribution* pProbDist = pEntry->getValue();
	char szDist[1024];
	pProbDist->screenPrint( szDist );
	// add flight item
	HTREEITEM hItem= m_tree.InsertItem(strName,cni,FALSE,FALSE,hItemParent);
	m_tree.SetItemData( hItem,(DWORD)pEntry);
	// add procserrID
	CString strProcItem;
	if(pEntry->getProcID().isBlank())
		strProcItem="Stand: All";
	else
		strProcItem.Format("Stand: %s", pEntry->getProcID().GetIDString() );
	hItem=m_tree.InsertItem(strProcItem,cni,FALSE,FALSE,hItem);
	// add distribution
	cni.net=NET_COMBOBOX;
	CString str_szDist = szDist;
	int n = str_szDist.Find(':');
	
	ASSERT(n);

	CString str_insert = "(Seconds)";
	str_szDist.Insert(n,str_insert);
	m_tree.InsertItem(str_szDist,cni,FALSE,FALSE,hItem);
	return hItem;
}

void CBoardingCallDlg::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	HTREEITEM hItemSeled=pNMTreeView->itemNew.hItem;
	if(hItemSeled)
	{
		HTREEITEM hItemChild=m_tree.GetChildItem(hItemSeled);

		m_wndToolBar.GetToolBarCtrl().EnableButton( ID_DELETE,hItemChild!=NULL);
		m_wndToolBar.GetToolBarCtrl().EnableButton( ID_EDIT,hItemChild==NULL);

		HTREEITEM hItemParent=m_tree.GetParentItem(hItemSeled);
		if(hItemParent==NULL)
		{
			m_wndToolBar.GetToolBarCtrl().EnableButton( ID_ADD_STAGE,TRUE);
			m_wndToolBar.GetToolBarCtrl().EnableButton( ID_ADD_FLIGHT,FALSE);
			m_wndToolBar.GetToolBarCtrl().EnableButton( ID_EDIT,FALSE);
		}
		else 
		{
			hItemParent=m_tree.GetParentItem(hItemParent);
			if(hItemParent==NULL)
			{
				m_wndToolBar.GetToolBarCtrl().EnableButton( ID_ADD_FLIGHT,TRUE);
				m_wndToolBar.GetToolBarCtrl().EnableButton( ID_ADD_STAGE,FALSE);
			}
			else
			{
				m_wndToolBar.GetToolBarCtrl().EnableButton( ID_ADD_STAGE,FALSE);
				m_wndToolBar.GetToolBarCtrl().EnableButton( ID_ADD_FLIGHT,FALSE);
				if(m_tree.GetParentItem(hItemParent)==NULL)//hItemSeled is default level node
				{
					hItemParent=m_tree.GetParentItem(hItemSeled);
					int nItemData=m_tree.GetItemData(hItemParent);
					FlightConWithProcIDDatabase* pConDB = (FlightConWithProcIDDatabase*)nItemData;//GetConstraintDatabase(nItemData);
					m_wndToolBar.GetToolBarCtrl().EnableButton( ID_DELETE,pConDB->getCount()!=1);
				}
				else
				{
					m_wndToolBar.GetToolBarCtrl().EnableButton( ID_DELETE,FALSE);

				}
			}
		}


	}
	else
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton( ID_ADD_STAGE,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton( ID_ADD_FLIGHT,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton( ID_DELETE,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton( ID_EDIT,FALSE);
	}
	
	*pResult = 0;
}

BOOL CBoardingCallDlg::DeleteStageChild()
{
	HTREEITEM hItemSeled= m_tree.GetSelectedItem();
	if( hItemSeled==NULL)
		return FALSE;
	
	HTREEITEM hItemParent=m_tree.GetParentItem(hItemSeled);
	if(hItemParent==NULL)//Boarding Calls Node
	{	//Delete All Items
		int nDBCount=GetConstraintDBCount();
		for(int i=0;i<nDBCount;i++)
		{
			GetInputTerminal()->flightData->DeleteOneStage(0);
		}
		LoadTreeItems();
	}
	else
	{
		hItemParent=m_tree.GetParentItem(hItemParent);
		if(hItemParent==NULL)//Stage Node
		{
			int nItemData=m_tree.GetItemData(hItemSeled);
			HTREEITEM hItemPrevSibling=m_tree.GetPrevSiblingItem(hItemSeled);
			FlightConWithProcIDDatabase* pConDB=(FlightConWithProcIDDatabase*)nItemData;
			MiscProcessorData* pMiscDB = GetInputTerminal()->miscData->getDatabase( (int)HoldAreaProc );
			for (int nProcIdx = 0; nProcIdx < pMiscDB->getCount(); nProcIdx++)
			{
				MiscData* pMiscData = ((MiscDataElement*)pMiscDB->getItem( nProcIdx ))->getData();
				long lCapacity= ((MiscHoldAreaData*)pMiscData)->GetStageNumber();
				if (lCapacity > GetInputTerminal()->flightData->getBoardingCallsStageSize() - 1)
				{
					CString strTemp = _T("");
					strTemp.Format(_T("Boarding Calls Stage can't less than %d"),lCapacity);
					MessageBox(strTemp,NULL,MB_OK);
					return FALSE;
				}
			}
			GetInputTerminal()->flightData->DeleteOneStage(pConDB);
			//Change Text Of Following Stage Node
			HTREEITEM hItemNextSibling=m_tree.GetNextSiblingItem(hItemSeled);
			CString strPrev=m_tree.GetItemText(hItemSeled);
			while(hItemNextSibling)
			{
				CString strNext=m_tree.GetItemText(hItemNextSibling);
				m_tree.SetItemText(hItemNextSibling,strPrev);
				hItemNextSibling=m_tree.GetNextSiblingItem(hItemNextSibling);
				strPrev=strNext;
			}
			m_tree.DeleteItem(hItemSeled);
		
		}
		else	//Default Node
		{
			int nStageIndex=m_tree.GetItemData(m_tree.GetParentItem(hItemSeled));
			FlightConWithProcIDDatabase* pConDB = (FlightConWithProcIDDatabase*)nStageIndex;	
			if( !pConDB )
				return FALSE;
			
			int nDBIdx = m_tree.GetItemData( hItemSeled);
			pConDB->removeItem((ConstraintWithProcIDEntry*)nDBIdx );	
			m_tree.DeleteItem(hItemSeled);
		}
	}
	return TRUE;
}

void CBoardingCallDlg::OnToolbarButtonEdit()
{
	HTREEITEM hItem=m_tree.GetSelectedItem();
	m_tree.DoEdit(hItem);
}

LRESULT CBoardingCallDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(message==UM_CEW_COMBOBOX_BEGIN)	
	{
		CWnd* pWnd=m_tree.GetEditWnd((HTREEITEM)wParam);
		CComboBox* pCB=(CComboBox*)pWnd;
		if(pCB->GetCount()==0)
		{
			//---------------------------------------------------------------------------------------
			CStringList strList;
			CString s;
			s.LoadString( IDS_STRING_NEWDIST );
			pCB->AddString(s);
			int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
			for( int m=0; m<nCount; m++ )
			{
				CProbDistEntry* pPBEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( m );			
				pCB->AddString(pPBEntry->m_csName);
			}
			//---------------------------------------------------------------------------------------

		}			
	}
	else if(message==UM_CEW_COMBOBOX_SELCHANGE)
	{
		HTREEITEM hItemSeled=(HTREEITEM)wParam;
		int nIndexSeled=m_tree.GetCmbBoxCurSel(hItemSeled);
		ChangeProbDist(hItemSeled,nIndexSeled);
		m_btnSave.EnableWindow(TRUE);
	}
	else if(message==UM_CEW_SHOWMENU_BEGIN)
	{
		COOLTREE_NODE_INFO* pCNI=(COOLTREE_NODE_INFO*)lParam;
		HTREEITEM hItem=(HTREEITEM)wParam;
		if(pCNI)
		{
			pCNI->unMenuID=IDR_BOARDING_CALL;
		}
	}
	else if(message==UM_CEW_SHOWMENU_READY)
	{
		CMenu* pMenu=(CMenu*)lParam;
		if(pMenu)
		{
			DisableMenuItem(pMenu);
			HTREEITEM hItemSeled=(HTREEITEM)wParam;
			if(hItemSeled)
			{
				HTREEITEM hItemChild=m_tree.GetChildItem(hItemSeled);
				
				pMenu->EnableMenuItem( ID_DELETE,(hItemChild!=NULL?MF_ENABLED:MF_GRAYED)|MF_BYCOMMAND);
				pMenu->EnableMenuItem( ID_EDIT,(hItemChild==NULL?MF_ENABLED:MF_GRAYED)|MF_BYCOMMAND);
				
				HTREEITEM hItemParent=m_tree.GetParentItem(hItemSeled);
				if(hItemParent==NULL)
				{
					pMenu->EnableMenuItem( ID_ADD_STAGE,MF_ENABLED|MF_BYCOMMAND);
					pMenu->EnableMenuItem( ID_ADD_FLIGHT,MF_GRAYED|MF_BYCOMMAND);
				}
				else 
				{
					hItemParent=m_tree.GetParentItem(hItemParent);
					if(hItemParent==NULL)
					{
						pMenu->EnableMenuItem( ID_ADD_FLIGHT,MF_ENABLED|MF_BYCOMMAND);
						pMenu->EnableMenuItem( ID_ADD_STAGE,MF_GRAYED|MF_BYCOMMAND);
					}
					else
					{
						pMenu->EnableMenuItem( ID_ADD_STAGE,MF_GRAYED|MF_BYCOMMAND);
						pMenu->EnableMenuItem( ID_ADD_FLIGHT,MF_GRAYED|MF_BYCOMMAND);
						if(m_tree.GetParentItem(hItemParent)==NULL)//hItemSeled is default level node
						{
							hItemParent=m_tree.GetParentItem(hItemSeled);
							int nItemData=m_tree.GetItemData(hItemParent);
							FlightConWithProcIDDatabase* pConDB =(FlightConWithProcIDDatabase*)nItemData;
							pMenu->EnableMenuItem( ID_DELETE,(pConDB->getCount()!=1?MF_ENABLED:MF_GRAYED)|MF_BYCOMMAND);
						}
						else
						{
							pMenu->EnableMenuItem(ID_CHANGE_PROCESSOR,MF_ENABLED|MF_BYCOMMAND);
							pMenu->EnableMenuItem(ID_DEFAULT_PROCESSOR,MF_ENABLED|MF_BYCOMMAND);
							pMenu->EnableMenuItem( ID_DELETE,MF_GRAYED|MF_BYCOMMAND);
						}
					}
				}
				
				
			}
			else
			{
				pMenu->EnableMenuItem( ID_ADD_STAGE,FALSE);
				pMenu->EnableMenuItem( ID_ADD_FLIGHT,FALSE);
				pMenu->EnableMenuItem( ID_DELETE,FALSE);
				pMenu->EnableMenuItem( ID_EDIT,FALSE);
			}			
		}
	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}

int CBoardingCallDlg::GetConstraintDBCount()
{
	return GetInputTerminal()->flightData->getBoardingCallsStageSize();
}

void CBoardingCallDlg::ChangeProbDist(HTREEITEM hItem, int nIndexSeled)
{
		int nStageIndex=m_tree.GetItemData(m_tree.GetParentItem(m_tree.GetParentItem(m_tree.GetParentItem(hItem))));
		FlightConWithProcIDDatabase* pConDB =(FlightConWithProcIDDatabase*)nStageIndex;	
	
		CString strItem=m_tree.GetItemText(hItem);
		// change data.
		ProbabilityDistribution* pProbDist = NULL;
		CString s;
		s.LoadString( IDS_STRING_NEWDIST );
		int nItemData=m_tree.GetItemData( m_tree.GetParentItem(m_tree.GetParentItem(hItem)));
		if( strcmp( strItem, s ) == 0 )
		{
			CDlgProbDist dlg(GetInputTerminal()->m_pAirportDB, true );
			if(dlg.DoModal()==IDOK) {
				int idxPD = dlg.GetSelectedPDIdx();
				ASSERT(idxPD!=-1);
				CProbDistEntry* pde = _g_GetActiveProbMan( GetInputTerminal() )->getItem(idxPD);
				m_tree.SetItemText( hItem, pde->m_csName );
				pProbDist = pde->m_pProbDist;
			}
			else {
				//cancel
				const ProbabilityDistribution* pProbDist = ((ConstraintWithProcIDEntry*)nItemData)->getValue();
				char szDist[1024];
				//CString szDist;
				pProbDist->screenPrint( szDist );
				m_tree.SetItemText(hItem, szDist);
				return;
			}
		}
		else
		{
			CProbDistEntry* pPDEntry = NULL;
			int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
			for( int i=0; i<nCount; i++ )
			{
				pPDEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( i );
				if( strcmp( pPDEntry->m_csName, strItem ) == 0 )
					break;
			}
			assert( i < nCount );
			pProbDist = pPDEntry->m_pProbDist;
		}
		assert( pProbDist );

		ConstraintWithProcIDEntry* pConEntry = (ConstraintWithProcIDEntry*)nItemData;

		ProbabilityDistribution* pDist = ProbabilityDistribution::CopyProbDistribution(pProbDist);
		
		pConEntry->setValue( pDist );


}

void CBoardingCallDlg::DisableMenuItem(CMenu *pMenu)
{
	int nCount=pMenu->GetMenuItemCount();
	for(int i=0;i<nCount;i++)
	{
		pMenu->EnableMenuItem(i,MF_GRAYED|MF_BYPOSITION);
	}
}

HTREEITEM CBoardingCallDlg::AddStage(FlightConWithProcIDDatabase *pConDB,int iIndex)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	
	CString strStageName;
	strStageName.Format("Stage %d",iIndex);

	HTREEITEM hStage1=m_tree.InsertItem(strStageName,cni,FALSE,FALSE,m_hBoardingCalls);
	m_tree.SetItemData( hStage1,(DWORD)pConDB);

	int nCount = pConDB->getCount();
	for( int k=0; k<nCount; k++ )
	{
		ConstraintWithProcIDEntry* pEntry = (ConstraintWithProcIDEntry*)(pConDB->getItem( k ));
		HTREEITEM hItem=AddStageChild2Tree(hStage1,pEntry);
	}		
	return hStage1;
}

void CBoardingCallDlg::OnChangeProcessor() 
{
	HTREEITEM hCurSelItem = m_tree.GetSelectedItem();
	HTREEITEM hFlightItem = m_tree.GetParentItem( hCurSelItem );

	DWORD dwItemData = m_tree.GetItemData( hFlightItem );
	ConstraintWithProcIDEntry* pEntry = (ConstraintWithProcIDEntry*) dwItemData;
	if( pEntry == NULL )
		return;


	//ASSERT(FALSE);

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	

	CSelectALTObjectDialog objDlg(0,pDoc->GetCurrentAirport());
	objDlg.SetType(ALT_STAND);
	std::vector<CString> strList;
	if(objDlg.DoModal()==IDOK) {
		int nIdCount = objDlg.GetObjectIDStringList(strList);
		for(int i =0;i<nIdCount;i++){
			CString strTreeItem;
			if(strList.at(i).IsEmpty()){
				strTreeItem  = "Stand:";
			}else {
				strTreeItem.Format("Stand: %s", strList.at(i));
			}
			ProcessorID procID;
			procID.SetStrDict( GetInputTerminal()->inStrDict );
			procID.setID(strList.at(i));
			pEntry->setProcID( procID );

			m_tree.SetItemText(hCurSelItem,strTreeItem);
		}
		m_btnSave.EnableWindow(TRUE);
	}

/*
	CProcesserDialog dlg( GetInputTerminal() );
	dlg.SetType( GateProc );
//	dlg.SetType2(StandProcessor);
	dlg.SetOnlyShowACStandGate(false);
	if( dlg.DoModal() == IDOK )
	{
		ProcessorID procID;
		ProcessorIDList idList;
		if( dlg.GetProcessorIDList( idList ) )
		{
			int nIdCount = idList.getCount();
			for( int i = 0; i < nIdCount; i++)
			{
				procID = *idList[i];				
				// modify data base
				procID.SetStrDict( GetInputTerminal()->inStrDict );
				pEntry->setProcID( procID );
				// modify tree item 
				CString strTreeItem;
				if(procID.isBlank())
					strTreeItem="Processor:";
				else
					strTreeItem.Format("Processor: %s", procID.GetIDString() );
				
				m_tree.SetItemText( hCurSelItem, strTreeItem );
			}
		}
		m_btnSave.EnableWindow( TRUE );
	}
*/
}

void CBoardingCallDlg::OnDefaultProcessor() 
{
	HTREEITEM hCurSelItem = m_tree.GetSelectedItem();
	HTREEITEM hFlightItem = m_tree.GetParentItem( hCurSelItem );
	
	DWORD dwItemData = m_tree.GetItemData( hFlightItem );
	ConstraintWithProcIDEntry* pEntry = (ConstraintWithProcIDEntry*) dwItemData;
	if( pEntry == NULL )
		return;
	
	ProcessorID procID;
	procID.SetStrDict( GetInputTerminal()->inStrDict );
	// modify data base	
	pEntry->setProcID( procID );
	// modify tree item 
	CString strTreeItem;
	if(procID.isBlank())
		strTreeItem="Processor:";
	else
		strTreeItem.Format("Processor: %s", procID.GetIDString() );
	m_tree.SetItemText( hCurSelItem, strTreeItem );

	m_btnSave.EnableWindow( TRUE );
}

void CBoardingCallDlg::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);
	if( m_btnOk.m_hWnd == NULL )
		return;

	// TODO: Add your message handler code here
	CRect Barrc,Treerc,rc;
	m_btnCancel.GetWindowRect(&rc);
	m_btnCancel.MoveWindow(cx-rc.Width()-10,cy-15-rc.Height(),rc.Width(),rc.Height());
	m_btnOk.MoveWindow(cx-2*rc.Width()-30,cy-15-rc.Height(),rc.Width(),rc.Height());
	m_btnSave.MoveWindow(cx-3*rc.Width()-50,cy-15-rc.Height(),rc.Width(),rc.Height());	
	m_wndToolBar.GetWindowRect( &Barrc );
	m_wndToolBar.MoveWindow(12,10,cx-20,Barrc.Height());
	m_tree.MoveWindow(10,10+Barrc.Height(),cx-20,cy-20-rc.Height()-50);
	InvalidateRect(NULL);
}

void CBoardingCallDlg::OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI )
{
	// TODO: Add your message handler code here and/or call default

	lpMMI->ptMinTrackSize.x = 552;
	lpMMI->ptMinTrackSize.y = 348;

	CDialog::OnGetMinMaxInfo(lpMMI);
}