// LinkageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "LinkageDlg.h"

//#include "inputs\SubFlowList.h"
//#include "inputs\SubFlow.h"
//#include "inputs\in_term.h"
#include "engine\proclist.h"
#include "TermPlanDoc.h"
#include "MainFrm.h"
#include "3DView.h"
#include <CommonData/Fallback.h>
#include "common\WinMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLinkageDlg dialog


CLinkageDlg::CLinkageDlg(InputTerminal* _pInTerm,CString& strProjectPath,CWnd* pParent )
	: CDialog(CLinkageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLinkageDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pInputTerm=_pInTerm;
	m_pDragImage= NULL;
	m_bDragging=FALSE;
	szProjectPath = strProjectPath;
}


void CLinkageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinkageDlg)
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_STATIC_SELECTED_GATE, m_staticSeledGate);
	DDX_Control(pDX, IDC_STATIC_PROCESSOR, m_staticProcessor);
	DDX_Control(pDX, IDC_STATIC_GATE, m_staticGate);
	DDX_Control(pDX, IDC_TREE_GATE, m_TreeGate);
	DDX_Control(pDX, IDC_TREE_PROCESSOR, m_TreeProc);
	DDX_Control(pDX, IDC_TREE_SELECTED_GATE, m_TreeSeledGate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLinkageDlg, CDialog)
	//{{AFX_MSG_MAP(CLinkageDlg)
	ON_WM_CREATE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROCESSOR, OnSelchangedTreeProcessor)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_GATE, OnSelchangedTreeGate)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SELECTED_GATE, OnSelchangedTreeSelectedGate)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE_PROCESSOR, OnBegindragTreeProcessor)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE_GATE, OnBegindragTreeGate)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_BTN_ADD_GATE,OnAddGate)
	ON_COMMAND(ID_BTN_ADD_PROC,OnAddProc)
	ON_COMMAND(ID_BTN_DEL,OnDel)
	ON_COMMAND(ID_BTN_PICK_GATE,OnPickGate)
	ON_COMMAND(ID_BTN_PICK_PROC,OnPickProc)
	ON_COMMAND(ID_PAXFLOW_ONETOONE,OnOneToOne)
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)

	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkageDlg message handlers

int CLinkageDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_LINKAGE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	return 0;
}

BOOL CLinkageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ImageList.Create ( IDB_PROCESS,16,1,RGB(255,0,255) );
	CBitmap bm;
	bm.LoadBitmap(IDB_GATE);
	m_ImageList.Add(&bm,RGB(255,0,255));
	InitProcTree(-1,m_TreeProc,0,m_vectID);
	InitProcTree(GateProc,m_TreeGate,1,m_vectIDGate);
	m_TreeSeledGate.SetImageList(&m_ImageList,TVSIL_NORMAL);

	InitToolBar();

	InitGateTree();
	CRect rectWnd;
	GetClientRect(rectWnd);
	DoSize(rectWnd.Width(),rectWnd.Height());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLinkageDlg::InitToolBar()
{
	CRect rc;
	m_TreeSeledGate.GetWindowRect(rc);
	ScreenToClient(rc);
	rc.bottom=rc.top+26;
	rc.OffsetRect(0,-26);
	
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	EnableBtnOfTB(ID_BTN_PICK_GATE,TRUE);
}

BOOL CLinkageDlg::InitProcTree(int nProcType,CTreeCtrl& treeCtrl,UINT nIDImage,std::vector<ProcessorID>& vectID)
{
	treeCtrl.SetImageList(&m_ImageList,TVSIL_NORMAL );
	
	AfxGetApp()->BeginWaitCursor();
	ProcessorList *procList = m_pInputTerm->procList;
	if (procList == NULL)
		return TRUE;

	// Set data to processor tree
	ProcessorID id, *pid = NULL;
	id.SetStrDict( m_pInputTerm->inStrDict );
	int includeBarriers = 0;
	CString strTemp, strTemp1, strTemp2, strTemp3, str;

	TVITEM ti;
	TVINSERTSTRUCT tis;
	HTREEITEM hItem = NULL, hItem1 = NULL, hItem2 = NULL, hItem3 = NULL;	
	int i = 0, j = 0;
	
	StringList strDict;
	procList->getAllGroupNames (strDict, nProcType);
	if (includeBarriers && !strDict.getCount())
		procList->getMemberNames (id, strDict, BarrierProc);

/*
	if( m_bDisplayBaggageDeviceSign)
	{
		const Processor* pProcBaggageDevice =procList->getProcessor( BAGGAGE_DEVICE_PROCEOR_INDEX );
		vectID.push_back( *( pProcBaggageDevice->getID() ) );	
		CString sDisplay = " ";
		sDisplay += pProcBaggageDevice->getID()->GetIDString();
		treeCtrl.SetItemData(treeCtrl.InsertItem( sDisplay ), 0 );
	}
*/
	
	for (j = 0; j < strDict.getCount(); j++)   // Level 1
	{
		strTemp = strDict.getString (j);
		strTemp.TrimLeft(); strTemp.TrimRight();
		if (strTemp.IsEmpty())
			continue;
		
		ti.mask       = TCIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		ti.cChildren  = 1;
		ti.pszText    = strTemp.GetBuffer(0);
		ti.cchTextMax = 256;
		ti.iSelectedImage=ti.iImage=nIDImage;
		
		tis.hParent = TVI_ROOT;
		tis.hInsertAfter = TVI_SORT;
		tis.item = ti;
			
		hItem = treeCtrl.InsertItem(&tis);
		if (hItem == NULL)
			continue;
		
		str = strTemp;
		id.setID((LPCSTR) str);

		//Added by Xie Bo for debug
//		char buf[256];
//		id.printID(buf,"-");
//		AfxMessageBox(buf);


		vectID.push_back( id );
		treeCtrl.SetItemData(hItem, (DWORD) ( vectID.size()-1 ) );
		
		StringList strDict1;
		procList->getMemberNames (id, strDict1, nProcType);
		for (int m = 0; m < strDict1.getCount(); m++)   // Level 2
		{
			strTemp1 = strDict1.getString (m);
			strTemp1.TrimLeft(); strTemp1.TrimRight();
			if (strTemp1.IsEmpty())
				continue;
			
			ti.pszText  = strTemp1.GetBuffer(0);
			tis.hParent = hItem;
			tis.item = ti;
			
			hItem1 = treeCtrl.InsertItem(&tis);
			if (hItem1 == NULL)
				continue;
			
			str = strTemp;
			str += "-";
			str += strTemp1;

			id.setID((LPCSTR) str);

			vectID.push_back( id );
			treeCtrl.SetItemData(hItem1, (DWORD) ( vectID.size()-1 ) );

			StringList strDict2;
			procList->getMemberNames (id, strDict2, nProcType);
			for (int n = 0; n < strDict2.getCount(); n++)   // Level 3
			{
				strTemp2 = strDict2.getString (n);
				strTemp2.TrimLeft(); strTemp2.TrimRight();
				if (strTemp2.IsEmpty())
					continue;
				
				ti.pszText  = strTemp2.GetBuffer(0);
				tis.hParent = hItem1;
				tis.item    = ti;
				
				hItem2 = treeCtrl.InsertItem(&tis);
				if (hItem2 == NULL)
					continue;

				str = strTemp;   str += "-";
				str += strTemp1; str += "-";
				str += strTemp2;

				id.setID((LPCSTR) str);

				vectID.push_back( id );
				treeCtrl.SetItemData(hItem2, (DWORD) ( vectID.size()-1 ) );

				StringList strDict3;
				procList->getMemberNames (id, strDict3, nProcType);
				for (int x = 0; x < strDict3.getCount(); x++)   // Level 4
				{
					strTemp3 = strDict3.getString (x);
					strTemp3.TrimLeft(); strTemp3.TrimRight();
					if (strTemp3.IsEmpty())
						continue;
					
					ti.pszText  = strTemp3.GetBuffer(0);
					tis.hParent = hItem2;
					tis.item    = ti;
					
					hItem3 = treeCtrl.InsertItem(&tis);

					str = strTemp;   str += "-";
					str += strTemp1; str += "-";
					str += strTemp2; str += "-";
					str += strTemp3;
					
					id.setID((LPCSTR) str);

					vectID.push_back( id );
					treeCtrl.SetItemData(hItem3, (DWORD) ( vectID.size()-1 ) );
				}
			}
		}
	}
	return TRUE;
}

void CLinkageDlg::OnSelchangedTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	m_hItemDragSrc = pNMTreeView->itemNew.hItem;
	IsEnableBtnAddProc();
	*pResult = 0;
	m_bByProc=TRUE;
	
}

void CLinkageDlg::InitGateTree()
{
	//////////////////////////////////////////////////////////////////////////
	for( int i=0; i<PROC_TYPES; i++ )
	{
		MiscProcessorData* pMiscProcessorData = m_pInputTerm->miscData->getDatabase( i );
		for( int j = 0; j<pMiscProcessorData->getCount(); j++ )
		{
			MiscDataElement* pElement = (MiscDataElement*)pMiscProcessorData->getItem( j );

			MiscData* pMiscData = pElement->getData();
			MiscProcessorIDList* pProcessorIDList = (MiscProcessorIDList*)pMiscData->getGateList();
			for( int k =0; k<pProcessorIDList->getCount(); k++ )
			{
				CString strProc = pElement->getID()->GetIDString();
				const MiscProcessorIDWithOne2OneFlag* pProcID = (MiscProcessorIDWithOne2OneFlag*)pProcessorIDList->getID(k);
				CString strGate = pProcID->GetIDString();
				
				HTREEITEM hRootItem = IsRootOfGate(&m_TreeSeledGate, strGate);
				if( hRootItem== NULL )
					hRootItem = m_TreeSeledGate.InsertItem( strGate, 1,1,NULL);

				if( pProcID->getOne2OneFlag() )
					strProc += CString("{1:1}");
				
				HTREEITEM hProc = m_TreeSeledGate.InsertItem( strProc, 0,0,hRootItem );
			}
		}
	}
	
	// sort tree by Name
	sortGateTree(); 
}

void CLinkageDlg::OnSelchangedTreeGate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_hItemDragSrc = pNMTreeView->itemNew.hItem;
	EnableBtnOfTB(ID_BTN_ADD_GATE,TRUE);
	*pResult = 0;
	m_bByProc=FALSE;
	
}

void CLinkageDlg::EnableBtnOfTB(UINT nID, BOOL bEnable)
{
	m_ToolBar.GetToolBarCtrl().EnableButton(nID,bEnable);
}

void CLinkageDlg::OnAddGate()
{
	m_hItemDragSrc=m_TreeGate.GetSelectedItem();
	AddGate();
}

BOOL CLinkageDlg::GetGateID(ProcessorID &id)
{
	HTREEITEM hItem=m_hItemDragSrc;
	int nCurSel=m_TreeGate.GetItemData(hItem);
	id = m_vectIDGate[nCurSel];   
	return TRUE;
}

BOOL CLinkageDlg::SelectGate(ProcessorID _id)
{
/*
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return FALSE;
	

	ProcessorIDList* pProcIDList = pMiscData->getGateList();
	HTREEITEM hItem = m_TreeGate.GetChildItem( m_hGateItem );
	while( hItem )
	{
		int nDBIdx = m_TreeGate.GetItemData( hItem );
		
		const ProcessorID* pProcID = pProcIDList->getID( nDBIdx );
		if( *pProcID == _id )
		{
			m_TreeGate.SelectItem( hItem );
			return TRUE;
		}
		hItem = m_TreeGate.GetNextItem( hItem, TVGN_NEXT );
	}	*/

	return FALSE;
}

void CLinkageDlg::OnSelchangedTreeSelectedGate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	IsEnableBtnAddProc();
	HTREEITEM hItem=m_TreeSeledGate.GetSelectedItem();
	EnableBtnOfTB(ID_BTN_DEL,(BOOL)hItem);
	if(!m_TreeSeledGate.GetParentItem(hItem))
	{
		EnableBtnOfTB(ID_BTN_PICK_PROC,TRUE);
		EnableBtnOfTB(ID_PAXFLOW_ONETOONE,FALSE);
	}
	else
	{
		EnableBtnOfTB(ID_BTN_PICK_PROC,FALSE);
		EnableBtnOfTB(ID_PAXFLOW_ONETOONE,TRUE);
	}	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CLinkageDlg::OnAddProc()
{
	m_hItemDragSrc=m_TreeProc.GetSelectedItem();
	AddProc();

}

BOOL CLinkageDlg::GetProcID(ProcessorID &id)
{
	HTREEITEM hItem=m_hItemDragSrc;
	int nCurSel=m_TreeProc.GetItemData(hItem);
	id = m_vectID[nCurSel];   
	return TRUE;
}

void CLinkageDlg::IsEnableBtnAddProc()
{
	HTREEITEM hItemGate=m_TreeSeledGate.GetSelectedItem();
	if(hItemGate==NULL)
	{
		EnableBtnOfTB(ID_BTN_ADD_PROC,FALSE);
		EnableBtnOfTB(ID_BTN_PICK_PROC,FALSE);
		
		return;
	}

	HTREEITEM hItem=m_TreeProc.GetSelectedItem();
	if(hItem==NULL)
	{
		EnableBtnOfTB(ID_BTN_ADD_PROC,FALSE);
		return;
	}

	if(!m_TreeSeledGate.GetParentItem(hItemGate))
	{
		EnableBtnOfTB(ID_BTN_ADD_PROC,TRUE);
		EnableBtnOfTB(ID_BTN_PICK_PROC,TRUE);
		
	}
	else
	{
		EnableBtnOfTB(ID_BTN_ADD_PROC,FALSE);
		EnableBtnOfTB(ID_BTN_PICK_PROC,FALSE);
	}
}

void CLinkageDlg::OnBegindragTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	//////////////////////////////////////////////////////////////////
	m_hItemDragSrc = pNMTreeView->itemNew.hItem;
	
	if(m_pDragImage)
		delete m_pDragImage;
	
	m_pDragImage = m_TreeProc.CreateDragImage( m_hItemDragSrc);
	if( !m_pDragImage )
		return;
	m_bDragging = TRUE;
	m_pDragImage->BeginDrag ( 0,CPoint(8,8) );
	CPoint  pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter ( GetDesktopWindow (),pt );  
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	SetCapture();
	m_bByProc=TRUE;
	
}

CLinkageDlg::~CLinkageDlg()
{
	if(m_pDragImage)
		delete m_pDragImage;
}

void CLinkageDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	HTREEITEM  hItem;
	UINT       flags;
	
	if( m_bDragging )
	{
		CPoint pt (point);
		ClientToScreen (&pt);
		
		// move the drag image
		VERIFY (m_pDragImage->DragMove (pt));
		// unlock window updates
		m_pDragImage->DragShowNolock (FALSE);
		//////////////////////////////////////////////////////
		CRect rectProcessTree;
		m_TreeSeledGate.GetWindowRect( &rectProcessTree );
		//ClientToScreen( &rectProcessTree );
		if( rectProcessTree.PtInRect( pt) )	
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			CPoint ptTest( point );
			ClientToScreen(&ptTest);
			m_TreeSeledGate.ScreenToClient(&ptTest);
			
			// TRACE("x = %d, y = %d \r\n", ptTest.x,ptTest.y );
			hItem = m_TreeSeledGate.HitTest( ptTest,&flags);
			if(m_bByProc)
			{
				if( hItem != NULL )
				{
					// TRACE( "ENTERY" );
					if(!m_TreeSeledGate.GetParentItem(hItem))
						m_TreeSeledGate.SelectItem(hItem);
				}
				else
				{
					m_TreeSeledGate.SelectItem(NULL);
					SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
				}
			}
		}
		else
		{
			m_TreeSeledGate.SelectItem(NULL);
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		}
		m_pDragImage->DragShowNolock (TRUE);
		m_TreeProc.Invalidate(FALSE);
		m_TreeGate.Invalidate(FALSE);
	}	
	CDialog::OnMouseMove(nFlags, point);
}

void CLinkageDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( m_bDragging )
	{
		CPoint pt (point);
		ClientToScreen (&pt);
		if(!m_bByProc)
		{
			CRect rectWnd;
			m_TreeSeledGate.GetWindowRect(rectWnd);
			ScreenToClient(rectWnd);
			if(rectWnd.PtInRect(point))
				AddGate();
			
		}
		else
		{
			AddProc();
		}
		
		m_bDragging = false;
		ReleaseCapture();
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		// end dragging
		m_pDragImage->DragLeave (GetDesktopWindow ());
		m_pDragImage->EndDrag ();
		delete m_pDragImage;
		m_pDragImage = NULL;
		m_TreeSeledGate.Invalidate();
	}	
	
	CDialog::OnLButtonUp(nFlags, point);
}

void CLinkageDlg::AddProc()
{
	HTREEITEM hItemGate=m_TreeSeledGate.GetSelectedItem();
	if(hItemGate==NULL) return;
	HTREEITEM hItemProc=m_hItemDragSrc;
	ProcessorID id;
	GetProcID(id);
	char buf[256];
	id.printID(buf);
	CString strProc=buf;
	if( IsChildItem( &m_TreeSeledGate, strProc, hItemGate) )
	{
		CString strErr;
		strErr.Format("%s already exist under the tree item",strProc );
		MessageBox( strErr ,NULL, MB_OK|MB_ICONINFORMATION);
		return;
	}

	CString strGate = m_TreeSeledGate.GetItemText( hItemGate );
	ProcessorID gateID;
	gateID.SetStrDict( m_pInputTerm->inStrDict );
	gateID.setID( strGate.GetBuffer(strGate.GetLength()) );
	if( AddElementToDB( id, gateID ) )
	{
		HTREEITEM hItemInserted=m_TreeSeledGate.InsertItem(strProc,0,0,hItemGate);
		m_TreeSeledGate.SetItemData(hItemInserted,m_TreeProc.GetItemData(hItemProc));
		m_TreeSeledGate.Expand(hItemGate,TVE_EXPAND);
		if(!m_TreeSeledGate.EnsureVisible(hItemInserted))
		{
			m_TreeSeledGate.SelectSetFirstVisible(hItemInserted);
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	sortGateTree();
}

void CLinkageDlg::OnBegindragTreeGate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	//////////////////////////////////////////////////////////////////
	m_hItemDragSrc = pNMTreeView->itemNew.hItem;
	
	if(m_pDragImage)
		delete m_pDragImage;
	
	m_pDragImage = m_TreeGate.CreateDragImage( m_hItemDragSrc);
	if( !m_pDragImage )
		return;
	m_bDragging = TRUE;
	m_pDragImage->BeginDrag ( 0,CPoint(8,8) );
	CPoint  pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter ( GetDesktopWindow (),pt );  
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	SetCapture();
	m_bByProc=FALSE;
}

void CLinkageDlg::AddGate()
{
	HTREEITEM hItemProc=m_hItemDragSrc;
	ProcessorID id;
	GetGateID(id);
	char buf[256];
	id.printID(buf);
	CString strProc=buf;
	if( IsRootOfGate( &m_TreeSeledGate, strProc ) )
	{
		CString strErr;
		strErr.Format("%s already exist in the tree",strProc );
		MessageBox( strErr ,NULL, MB_OK|MB_ICONINFORMATION);
		return;
	}
	
	HTREEITEM hItemInserted=m_TreeSeledGate.InsertItem(strProc,1,1);
	m_TreeSeledGate.SetItemData(hItemInserted,m_TreeGate.GetItemData(hItemProc));
	if(!m_TreeSeledGate.EnsureVisible(hItemInserted))
	{
		m_TreeSeledGate.SelectSetFirstVisible(hItemInserted);
	}
	
	//////////////////////////////////////////////////////////////////////////
	sortGateTree();
}

void CLinkageDlg::OnDel()
{
	HTREEITEM hItem=m_TreeSeledGate.GetSelectedItem();
	CString strItem = m_TreeSeledGate.GetItemText( hItem );
	if( strItem.Find("{1:1}") != -1 )
		strItem = strItem.Left( strItem.GetLength() - 5 );

	ProcessorID procID;
	procID.SetStrDict( m_pInputTerm->inStrDict );
	procID.setID( strItem);

	//////////////////////////////////////////////////////////////////////////
	HTREEITEM hGate = m_TreeSeledGate.GetParentItem( hItem); 
	if( hGate)	//proc
	{		
		CString strGate = m_TreeSeledGate.GetItemText( hGate );
		ProcessorID gateID;
		gateID.SetStrDict( m_pInputTerm->inStrDict );
		gateID.setID( strGate.GetBuffer( strGate.GetLength() ));

		DeleteElementFromDB( procID, gateID );
	}
	else
	{
		HTREEITEM hChild = m_TreeSeledGate.GetChildItem( hItem );
		while( hChild )
		{
			CString strChild = m_TreeSeledGate.GetItemText( hChild );
			if( strChild.Find("{1:1}") != -1 )
				strChild = strChild.Left( strChild.GetLength() - 5 );
			
			ProcessorID childID;
			childID.SetStrDict( m_pInputTerm->inStrDict );
			childID.setID( strChild.GetBuffer( strChild.GetLength() ) );

			DeleteElementFromDB( childID, procID );

			hChild = m_TreeSeledGate.GetNextSiblingItem( hChild );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	
	if(hItem)
	{
		m_TreeSeledGate.DeleteItem(hItem);
		if(!m_TreeSeledGate.GetSelectedItem())
			EnableBtnOfTB(ID_BTN_DEL,FALSE);
		IsEnableBtnAddProc();
	}
}

void CLinkageDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	DoSize(cx,cy);
	
}

void CLinkageDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	HWND hwndT=::GetWindow(m_hWnd, GW_CHILD);
	CRect rectCW;
	CRgn rgnCW;
	rgnCW.CreateRectRgn(0,0,0,0);
	while (hwndT != NULL)
	{
		CWnd* pWnd=CWnd::FromHandle(hwndT)  ;
		if(
			pWnd->IsKindOf(RUNTIME_CLASS(CListBox))||
			pWnd->IsKindOf(RUNTIME_CLASS(CTreeCtrl)))
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
}

BOOL CLinkageDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;
}

void CLinkageDlg::OnPickGate()
{
		
	CMDIChildWnd* pChildWnd=(CMDIChildWnd*)((CMDIFrameWnd*)AfxGetMainWnd())->GetActiveFrame();
	CView* pView=pChildWnd->GetActiveView();
	
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)(pView)->GetDocument();
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
	
	enumReason = PICK_GATE;
	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error",NULL,MB_OK|MB_ICONERROR );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}
}

void CLinkageDlg::OnPickProc()
{
	CMDIChildWnd* pChildWnd=(CMDIChildWnd*)((CMDIFrameWnd*)AfxGetMainWnd())->GetActiveFrame();
	CView* pView=pChildWnd->GetActiveView();
	
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)(pView)->GetDocument();
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
	
	enumReason = PICK_PROC;
	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" ,NULL, MB_ICONERROR|MB_OK);
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}
}

void CLinkageDlg::HideDialog(CWnd *parentWnd)
{
	GetParent()->ShowWindow(SW_HIDE);
	ShowWindow(SW_HIDE);

	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
	
}

void CLinkageDlg::ShowDialog(CWnd *parentWnd)
{
	while (!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		parentWnd->ShowWindow(SW_SHOW);
		parentWnd->EnableWindow(FALSE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(FALSE);
	GetParent()->ShowWindow(SW_SHOW);
	ShowWindow(SW_SHOW);	
	
	EnableWindow();
}

LRESULT CLinkageDlg::OnTempFallbackFinished(WPARAM wParam,LPARAM lParam)
{

	ShowWindow(SW_SHOW);	
	GetParent()->ShowWindow(SW_SHOW);
	EnableWindow();
	CMDIChildWnd* pChildWnd=(CMDIChildWnd*)((CMDIFrameWnd*)AfxGetMainWnd())->GetActiveFrame();
	CView* pView=pChildWnd->GetActiveView();
	
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)(pView)->GetDocument();
	
	int iProcCout = pDoc->GetSelectProcessorsCount();
	if( iProcCout > 0)
	{
		C3DView* p3DView=(C3DView*)pView;
		for( int i=0; i<iProcCout; i++)
		{
			CObjectDisplay *pObjectDisplay = pDoc->GetSelectedObjectDisplay(i);

			if (pObjectDisplay == NULL)
				continue;

			if (pObjectDisplay->GetType() != ObjectDisplayType_Processor2)
				continue;


			CProcessor2* pProc2=(CProcessor2 *)pObjectDisplay;
			if( pProc2->GetProcessor() == NULL )
				continue;

			if(p3DView->m_eMouseState==NS3DViewCommon::_pickgate)
			{
				//////////////////////////////////////////////////////////////////////////
				// add gate
				if( pProc2->GetProcessor()->getProcessorType() != GateProc )	// can not add the processor
					continue;
				
				CString strProc = pProc2->GetProcessor()->getID()->GetIDString();
				if( IsRootOfGate( &m_TreeSeledGate, strProc ) )					// already exist in the tree
					continue;
				
				HTREEITEM hItemInserted=m_TreeSeledGate.InsertItem(strProc,1,1);
				if(!m_TreeSeledGate.EnsureVisible(hItemInserted))
				{
					m_TreeSeledGate.SelectSetFirstVisible(hItemInserted);
				}
			}
			else if(p3DView->m_eMouseState==NS3DViewCommon::_pickproc)
			{
				//////////////////////////////////////////////////////////////////////////
				HTREEITEM hGateItem = m_TreeSeledGate.GetSelectedItem();
				if( hGateItem == NULL)
					return FALSE;
				
				CString strGate = m_TreeSeledGate.GetItemText( hGateItem );
				ProcessorID gate;
				gate.SetStrDict( m_pInputTerm->inStrDict );
				gate.setID( strGate.GetBuffer( strGate.GetLength() ));

				ProcessorID procID = *(pProc2->GetProcessor()->getID());
				CString strProc = procID.GetIDString();
				
				if( IsChildItem( &m_TreeSeledGate, strProc, hGateItem) )			// already exist under the item
					continue;
				
				if( AddElementToDB( procID,gate ) )
				{
					HTREEITEM hItemInserted=m_TreeSeledGate.InsertItem(strProc,0,0,hGateItem);
					m_TreeSeledGate.Expand(hGateItem,TVE_EXPAND);
					if(!m_TreeSeledGate.EnsureVisible(hItemInserted))
					{
						m_TreeSeledGate.SelectSetFirstVisible(hItemInserted);
					}
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		sortGateTree();
	}
	return TRUE;	
}

BOOL CLinkageDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
    ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);
	
    // if there is a top level routing frame then let it handle the message
    if (GetRoutingFrame() != NULL) return FALSE;
	
    // to be thorough we will need to handle UNICODE versions of the message also !!
    TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
    TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
    TCHAR szFullText[512];
    CString strTipText;
    UINT nID = pNMHDR->idFrom;
	
    if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
        pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
    {
        // idFrom is actually the HWND of the tool 
        nID = ::GetDlgCtrlID((HWND)nID);
    }
	
    if (nID != 0) // will be zero on a separator
    {
        AfxLoadString(nID, szFullText);
        strTipText=szFullText;
		
#ifndef _UNICODE
        if (pNMHDR->code == TTN_NEEDTEXTA)
        {
            lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
        }
        else
        {
            _mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
        }
#else
        if (pNMHDR->code == TTN_NEEDTEXTA)
        {
            _wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
        }
        else
        {
            lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
        }
#endif
		
        *pResult = 0;
		
        // bring the tooltip window above other popup windows
        ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
            SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);
        
        return TRUE;
    }
	
    return FALSE;
}



HTREEITEM CLinkageDlg::IsRootOfGate( CTreeCtrl* p_tree, CString& strLable )
{
	assert( p_tree );

	HTREEITEM hRootItem = p_tree->GetRootItem();
	while( hRootItem )
	{
		CString strItem = p_tree->GetItemText( hRootItem );
		if( strItem == strLable )
			break;
		hRootItem = p_tree->GetNextSiblingItem( hRootItem );
	}

	return hRootItem;
}

HTREEITEM CLinkageDlg::IsChildItem( CTreeCtrl* p_tree, CString& strLable, HTREEITEM hParent )
{
	assert( p_tree );

	HTREEITEM hChildItem = p_tree->GetChildItem( hParent );
	while( hChildItem )
	{
		CString strItem = p_tree->GetItemText( hChildItem );
		if( strItem.Find("{1:1}") != -1)
			strItem = strItem.Left( strItem.GetLength() - 5 );
		
		if( strItem == strLable )
			break;
		hChildItem = p_tree->GetNextSiblingItem( hChildItem );
	}
	
	return hChildItem;
}

bool CLinkageDlg::AddElementToDB( ProcessorID& _procID, ProcessorID& _gateID )
{
	int m_nProcDataType = GetProcssorType( _procID );
	if( m_nProcDataType == -1 )
		return false;

	MiscData* pMiscData = NULL;
	MiscDataElement* pDataElement = SelectEntryProc(_procID,m_nProcDataType);
	if( pDataElement== NULL )
	{
		MiscProcessorData* pMiscDB = m_pInputTerm->miscData->getDatabase( m_nProcDataType );
		if( pMiscDB == NULL )
			return false;

		pDataElement = new MiscDataElement( _procID );
		switch( m_nProcDataType )
		{
		case PointProc:
			pMiscData = new MiscData;
			break;
		case DepSourceProc:
			pMiscData = new MiscDepSourceData;
			break;
		case DepSinkProc:
			pMiscData = new MiscDepSinkData;
			break;
		case LineProc:
			pMiscData = new MiscLineData;
			break;
		case BaggageProc:
			pMiscData = new MiscBaggageData;
			break;
		case HoldAreaProc:
			pMiscData = new MiscHoldAreaData;
			break;
		case GateProc:
			pMiscData = new MiscGateData;
			break;
		case FloorChangeProc:
			pMiscData = new MiscFloorChangeData;
			break;
		case BarrierProc:
            break;			
		case MovingSideWalkProc:
			pMiscData = new MiscMovingSideWalkData;
			break;
		case Elevator:
			pMiscData = new MiscElevatorData;
			break;
		case ConveyorProc:
			pMiscData = new MiscConveyorData;
			break;
		case StairProc:
			pMiscData = new MiscStairData;
			break;
		case EscalatorProc:
			pMiscData = new MiscEscalatorData;
			break;
		case IntegratedStationProc:
			pMiscData = new MiscIntegratedStation;
			break;	
			
		default:
			return false;
		}
	
		pDataElement->setData( pMiscData );
		pMiscDB->addItem( pDataElement );
	}
	else
	{
		pMiscData = pDataElement->getData();
	}

	ProcessorIDList* pProcIDList = pMiscData->getGateList();
	MiscProcessorIDWithOne2OneFlag* pID = new MiscProcessorIDWithOne2OneFlag( _gateID );
	pProcIDList->addItem( pID );	
	return true;
}

void CLinkageDlg::DeleteElementFromDB( ProcessorID& _procID,ProcessorID& _gateID )
{
	int m_nProcDataType = GetProcssorType( _procID );
	if( m_nProcDataType == -1 )
		return;
	
	if( !SelectEntryProc(_procID,m_nProcDataType) )	// check if exist.
		return;
		
	MiscProcessorData* pMiscData = m_pInputTerm->miscData->getDatabase( m_nProcDataType );
	if( pMiscData == NULL )
		return;
	
	for( int i=0; i<pMiscData->getCount(); i++ )
	{
		const ProcessorID* pProcID = pMiscData->getProcName( i);
		if( *pProcID == _procID )
		{
			MiscDataElement* pElement = (MiscDataElement*)pMiscData->getItem( i );
			MiscData* pData = (MiscData*)pElement->getData();
			ProcessorIDList* pList = pData->getGateList();
			for( int j=0; j<pList->getCount(); j++ )
			{
				if( *pList->getItem(j) == _gateID )
				{
					pList->removeItem( j );
					if( pList->getCount()== 0 )
					{
						pMiscData->removeItem( i );
					}
					return;
				}

			}
		}
	}
}


void CLinkageDlg::SetOne2OneFlag( ProcessorID& _procID, ProcessorID& _gateID )
{
	int m_nProcDataType = GetProcssorType( _procID );
	if( m_nProcDataType == -1 )
		return;
	
	if( !SelectEntryProc(_procID,m_nProcDataType) )	// check if exist.
		return;
	
	MiscProcessorData* pMiscData = m_pInputTerm->miscData->getDatabase( m_nProcDataType );
	if( pMiscData == NULL )
		return;
	
	for( int i=0; i<pMiscData->getCount(); i++ )
	{
		const ProcessorID* pProcID = pMiscData->getProcName( i);
		if( *pProcID == _procID )
		{
			MiscDataElement* pElement = (MiscDataElement*)pMiscData->getItem( i );
			MiscData* pData = (MiscData*)pElement->getData();
			MiscProcessorIDList* pList = (MiscProcessorIDList*)pData->getGateList();
			for( int j=0; j<pList->getCount(); j++ )
			{
				MiscProcessorIDWithOne2OneFlag* pMiscID = (MiscProcessorIDWithOne2OneFlag*)pList->getItem(j);
				if( *pMiscID == _gateID )
				{
					pMiscID->setOne2OneFlag( !pMiscID->getOne2OneFlag() );
					return;
				}
				
			}
		}
	}
	
}


MiscDataElement* CLinkageDlg::SelectEntryProc(ProcessorID _id, int m_nProcDataType)
{
	MiscProcessorData* pMiscDB = m_pInputTerm->miscData->getDatabase( m_nProcDataType );
	if( pMiscDB == NULL )
		return NULL;
	
	int nProcCount = pMiscDB->getCount();
	for( int i=0; i<nProcCount; i++ )
	{
		const ProcessorID* pProcID = pMiscDB->getProcName( i);
		if( *pProcID == _id )
			return (MiscDataElement*)pMiscDB->getItem(i);
	}

	return NULL;
}

int CLinkageDlg::GetProcssorType( ProcessorID _id)
{
	GroupIndex group = m_pInputTerm->procList->getGroupIndex( _id );
	if( group.start ==-1 && group.end == -1)
		return -1;
	Processor *_proc = m_pInputTerm->procList->getProcessor( group.start );
	return _proc->getProcessorType();
}


void CLinkageDlg::OnOneToOne()
{
	HTREEITEM hProc = m_TreeSeledGate.GetSelectedItem();
	if( hProc == NULL )
		return;
	HTREEITEM hGate = m_TreeSeledGate.GetParentItem( hProc );
	
	CString strProc = m_TreeSeledGate.GetItemText( hProc );
	CString strGate = m_TreeSeledGate.GetItemText( hGate );
	CString strProc_ = strProc;

	if( strProc.Find("{1:1}")!=-1)
	{
		strProc_ = strProc.Left( strProc.GetLength()-5 );
	}

	ProcessorID proc, gate;
	proc.SetStrDict( m_pInputTerm->inStrDict );
	proc.setID( strProc_.GetBuffer( strProc_.GetLength() ));
	
	gate.SetStrDict( m_pInputTerm->inStrDict );
	gate.setID( strGate.GetBuffer( strGate.GetLength() ));
	
	SetOne2OneFlag( proc, gate);
	
	if( strProc.Find("{1:1}")!=-1)
	{
		strProc= strProc.Left( strProc.GetLength()-5 );
	}
	else
	{
		strProc += CString("{1:1}");
	}
	m_TreeSeledGate.SetItemText( hProc, strProc);
	
}


void CLinkageDlg::OnOK()
{
	m_pInputTerm->miscData->saveDataSet( szProjectPath, true );
	CDialog::OnOK();
}

void CLinkageDlg::OnCancel()
{
	m_pInputTerm->miscData->loadDataSet( szProjectPath );
	CDialog::OnCancel();
}



void CLinkageDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.x =600;
	lpMMI->ptMinTrackSize.y=400; 
	
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CLinkageDlg::DoSize(int cx, int cy)
{
	if(!m_TreeProc.m_hWnd) return;
	CRect rectWnd;
	int nOffset=7;
	int nOffsetHeight=42;
	int nWidthWnd=cx*3/10;
	int nHeightWnd,nLeftWnd,nTopWnd;
	
	m_TreeProc.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	
	int nWidthEdge=rectWnd.left;
	int nHeightEdge=rectWnd.top;
	
	nHeightWnd=cy-nOffsetHeight-rectWnd.top;
	
	m_TreeProc.SetWindowPos(NULL,0,0,nWidthWnd,nHeightWnd,
		SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
	m_TreeProc.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	int nSplitter1Left=rectWnd.right;
	int nSplitter1Height=rectWnd.Height();
	nLeftWnd=rectWnd.right+nWidthEdge;
	
	m_staticGate.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	nTopWnd=rectWnd.top;
	nWidthWnd=cx-nWidthEdge-nLeftWnd;
	nHeightWnd=rectWnd.Height();
	m_staticGate.SetWindowPos(NULL,nLeftWnd,nTopWnd,nWidthWnd,
		nHeightWnd,SWP_NOZORDER|SWP_NOACTIVATE);
	m_staticGate.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	
	nLeftWnd=rectWnd.left;
	nTopWnd=nHeightEdge;
	nHeightWnd=cy*3/10;
	nWidthWnd=rectWnd.Width();
	m_TreeGate.SetWindowPos(NULL,nLeftWnd,nTopWnd,nWidthWnd,
		nHeightWnd,SWP_NOZORDER|SWP_NOACTIVATE);
	m_TreeGate.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	int nSplitter2Top=rectWnd.bottom;
	int nSplitter2Left=rectWnd.left;
	//Splitter1------------------------------------------------------------------------------
	int nSplitter1Top=rectWnd.top;
	int nSplitter1Width=rectWnd.left-nSplitter1Left;
	if(!m_wndSplitter1.m_hWnd)//Create Splitter1
	{
		m_wndSplitter1.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_WND_SPLITTER1);
		m_wndSplitter1.SetStyle(SPS_VERTICAL);
	}
	m_wndSplitter1.SetRange(nOffset*3,cx-nOffset*3);
	m_wndSplitter1.SetWindowPos(NULL,nSplitter1Left,nSplitter1Top,nSplitter1Width,nSplitter1Height,SWP_NOACTIVATE|SWP_NOZORDER);
	//-----------------------------------------------------------------------------------------------------------	
	nTopWnd=rectWnd.bottom+nWidthEdge;
	nLeftWnd=rectWnd.left;
	nWidthWnd=rectWnd.Width();
	m_staticSeledGate.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	nHeightWnd=rectWnd.Height();
	m_staticSeledGate.SetWindowPos(NULL,nLeftWnd,nTopWnd,nWidthWnd,
		nHeightWnd,SWP_NOZORDER|SWP_NOACTIVATE);
	m_staticSeledGate.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	
	int nSplitter2Height=rectWnd.top-nSplitter2Top;
	
	nTopWnd=rectWnd.bottom+2+26;
	nLeftWnd=rectWnd.left;
	nWidthWnd=rectWnd.Width();
	nHeightWnd=cy-nOffsetHeight-nTopWnd;
	m_TreeSeledGate.SetWindowPos(NULL,nLeftWnd,nTopWnd,
		nWidthWnd,nHeightWnd,SWP_NOZORDER|SWP_NOACTIVATE);
	m_TreeSeledGate.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	//Splitter2------------------------------------------------------------------------------
	
	int nSplitter2Width=rectWnd.Width();
	if(!m_wndSplitter2.m_hWnd)//Create Splitter2
	{
		m_wndSplitter2.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_WND_SPLITTER2);
		m_wndSplitter2.SetStyle(SPS_HORIZONTAL);
	}
	m_wndSplitter2.SetRange(nOffset*6,cy-26*3-nOffset*3);
	m_wndSplitter2.SetWindowPos(NULL,nSplitter2Left,nSplitter2Top,nSplitter2Width,nSplitter2Height,SWP_NOACTIVATE|SWP_NOZORDER);
	//-----------------------------------------------------------------------------------------------------------	
	
	nLeftWnd=rectWnd.right;
	nTopWnd=rectWnd.bottom+nWidthEdge;
	m_btnCancel.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	nWidthWnd=rectWnd.Width();
	nHeightWnd=rectWnd.Height();
	nLeftWnd-=nWidthWnd;
	m_btnCancel.SetWindowPos(NULL,nLeftWnd,nTopWnd,
		nWidthWnd,nHeightWnd,SWP_NOZORDER|SWP_NOACTIVATE);
	m_btnCancel.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	
	nLeftWnd-=(4+rectWnd.Width());
	m_btnOK.SetWindowPos(NULL,nLeftWnd,nTopWnd,
		nWidthWnd,nHeightWnd,SWP_NOZORDER|SWP_NOACTIVATE);
	m_btnOK.GetWindowRect(rectWnd);
	
	InitToolBar();
	Invalidate();
	// TODO: Add your message handler code here
}

LRESULT CLinkageDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{

	if (message == WM_NOTIFY)
	{
		switch(wParam)
		{
		case ID_WND_SPLITTER1:
		case ID_WND_SPLITTER2:
			{
				SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
				DoResize(pHdr->delta,wParam);
			}
			break;
		}
	}	
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CLinkageDlg::DoResize(int delta,UINT nIDSplitter)
{
	switch(nIDSplitter)
	{
	case ID_WND_SPLITTER1:
		{
			CSplitterControl::ChangeWidth(&m_TreeProc, delta);
			CSplitterControl::ChangeWidth(&m_ToolBar,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_staticSeledGate,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_staticGate,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_wndSplitter2,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_TreeGate, -delta, CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_TreeSeledGate,-delta,CW_RIGHTALIGN);
		}
		break;
	case ID_WND_SPLITTER2:
		{
			CSplitterControl::ChangeHeight(&m_TreeGate, delta);
			CSplitterControl::ChangeHeight(&m_TreeSeledGate,-delta,CW_BOTTOMALIGN);
			CRect rectWnd;
			m_staticSeledGate.GetWindowRect(rectWnd);
			ScreenToClient(rectWnd);
			m_staticSeledGate.SetWindowPos(NULL,rectWnd.left,rectWnd.top+delta,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
			
			m_ToolBar.GetWindowRect(rectWnd);
			ScreenToClient(rectWnd);
			m_ToolBar.SetWindowPos(NULL,rectWnd.left,rectWnd.top+delta,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
			

		}
		break;
	}
	
	Invalidate();
}


void CLinkageDlg::sortGateTree()
{
	// first sort all root node
	m_TreeSeledGate.SortChildren( NULL );
	// then sort each node
	HTREEITEM hItem = m_TreeSeledGate.GetRootItem();
	while( hItem )
	{
		m_TreeSeledGate.SortChildren( hItem );
		
		hItem = m_TreeSeledGate.GetNextSiblingItem( hItem );
	}	
}
