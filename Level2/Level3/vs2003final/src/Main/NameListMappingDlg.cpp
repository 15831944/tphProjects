// NameListMappingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "NameListMappingDlg.h"
#include "inputs\in_term.h"
#include "TermPlanDoc.h"
#include "inputs\UserName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNameListMappingDlg dialog


CNameListMappingDlg::CNameListMappingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNameListMappingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNameListMappingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bInit=FALSE;
	m_bDragging				= FALSE;
	m_pDragImage			= NULL;
	m_hItemDragSrc			= NULL;
	m_hItemDragDes			= NULL;
	m_pDragWnd=NULL;
	for(int i=0;i<9;i++)
		m_bColumnDropped[i]=FALSE;
	
	m_bHaveAssignTitle		= false;
}


void CNameListMappingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNameListMappingDlg)
	DDX_Control(pDX, IDC_BUTTON_FILE, m_butFile);
	DDX_Control(pDX, IDOK, m_butOK);
	DDX_Control(pDX, IDCANCEL, m_butCancel);
	DDX_Control(pDX, IDC_TREE_PROC, m_treeProc);
	DDX_Control(pDX, IDC_STATIC_TEXT, m_staticText);
	DDX_Control(pDX, IDC_STATIC_PROC, m_staticProc);
	DDX_Control(pDX, IDC_STATIC_MAPPING, m_staticMapping);
	DDX_Control(pDX, IDC_LIST_TEXT, m_listText);
	DDX_Control(pDX, IDC_LIST_MAPPING, m_listMapping);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_butSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNameListMappingDlg, CDialog)
	//{{AFX_MSG_MAP(CNameListMappingDlg)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnButtonFile)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE_PROC, OnBegindragTreeProc)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_TEXT, OnClickListText)
	ON_NOTIFY(HDN_ITEMCHANGED, IDC_LIST_MAPPING, OnItemchangedListMapping)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPeoplemoverDelete)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_WM_GETMINMAXINFO()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TEXT, OnDblclkListText)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TEXT, OnItemchangedListText)
	ON_COMMAND(ID_NAMELIST_TITLE, OnNamelistTitle)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROC, OnSelchangedTreeProc)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MAPPING, OnItemchangedListMapping)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPeoplemoverNew)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_BEGIN_DRAG,OnBegindrag)
	ON_COMMAND(ID_DELETE_COLUMN,OnDeleteColumn)
	ON_COMMAND(ID_DELETE_ROW,OnDeleteRow)
	ON_MESSAGE(UM_HEADERCTRL_CLICK_COLUMN,OnClickColumn)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	ON_CBN_SELENDOK(ID_COMBOBOX, OnComboxSelectChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNameListMappingDlg message handlers

int CNameListMappingDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBarLeft.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBarLeft.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	// TODO: Add your specialized creation code here
	if (!m_wndToolBarRight.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBarRight.LoadToolBar(IDR_NAMELIST_TEXT))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	// TODO: Add your specialized creation code here
	TBBUTTON tbbtn;
	tbbtn.idCommand=0;
	tbbtn.fsStyle=TBSTYLE_SEP;
	m_wndToolBarRight.GetToolBarCtrl().AddButtons(1,&tbbtn);
	m_wndToolBarRight.SetButtonInfo(3,ID_COMBOBOX,TBBS_SEPARATOR,160);
	CRect rect;
	m_wndToolBarRight.GetItemRect(3,&rect);
	rect.bottom=rect.top+200;
	BOOL bCreated=m_cmbScale.Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST,rect,&m_wndToolBarRight,ID_COMBOBOX);
	m_cmbScale.AddString("Semicolon");
	m_cmbScale.AddString("Comma");
	m_cmbScale.AddString("Space");
	m_cmbScale.AddString("Tab");
	m_cmbScale.SetCurSel(1);
	m_bInit=TRUE;

	//////////////////////////////////////////////////////////////////////////
	// control the mapping tool bar
	m_wndToolBarLeft.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE );
	m_wndToolBarLeft.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW, FALSE );
	m_wndToolBarLeft.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE, FALSE );
	m_wndToolBarRight.GetToolBarCtrl().EnableButton( ID_NAMELIST_TITLE, FALSE );
	return 0;
}

BOOL CNameListMappingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_hcArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_hcNo = AfxGetApp()->LoadStandardCursor(IDC_NO);
	m_listText.SubClassHeaderCtrl();
	// TODO: Add extra initialization here
	m_imageList.Create ( IDB_PROCESS,16,1,RGB(255,0,255) );
	m_treeProc.SetImageList ( &m_imageList,TVSIL_NORMAL );
	initProcTree();
	initListCtrl();
	loadUserNameList();
	m_wndToolBarRight.GetToolBarCtrl().EnableButton(ID_DELETE_COLUMN,FALSE);
	m_wndToolBarRight.GetToolBarCtrl().EnableButton(ID_DELETE_ROW,FALSE);
	m_wndSplitterLeft.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, IDC_STATIC_SPLITTER);
	m_wndSplitterRight.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, IDC_STATIC_SPLITTER+1);
	m_wndSplitterLeft.SetStyle(SPS_VERTICAL);
	m_wndSplitterRight.SetStyle(SPS_VERTICAL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

InputTerminal* CNameListMappingDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}

CString CNameListMappingDlg::GetProjectPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}

void CNameListMappingDlg::initProcTree()
{
	ProcessorList *procList = GetInputTerminal()->procList;
	if (procList == NULL)
		return ;
	
	m_treeProc.DeleteAllItems();	// clear proc tree
	// Set data to processor tree
	ProcessorID id, *pid = NULL;
	id.SetStrDict( GetInputTerminal()->inStrDict );
	//int includeBarriers = 0;
	CString strTemp, strTemp1, strTemp2, strTemp3, str;

	TVITEM ti;
	TVINSERTSTRUCT tis;
	HTREEITEM hItem = NULL, hItem1 = NULL, hItem2 = NULL, hItem3 = NULL;	
	int i = 0, j = 0;
	
	StringList strDict;
	procList->getAllGroupNames (strDict, -1);
	
	for (j = 0; j < strDict.getCount(); j++)   // Level 1
	{
		strTemp = strDict.getString (j);
		strTemp.TrimLeft(); strTemp.TrimRight();
		if (strTemp.IsEmpty())
			continue;
		
		ti.mask       = TCIF_TEXT|TCIF_IMAGE;
		ti.cChildren  = 1;
		ti.pszText    = strTemp.GetBuffer(0);
		ti.cchTextMax = 256;
		ti.iImage=0;

		tis.hParent = TVI_ROOT;
		tis.hInsertAfter = TVI_SORT;
		tis.item = ti;
		
		hItem = m_treeProc.InsertItem(&tis);
		if (hItem == NULL)
			continue;
		
		str = strTemp;
		id.setID((LPCSTR) str);
	
		StringList strDict1;
		procList->getMemberNames (id, strDict1, -1);
		for (int m = 0; m < strDict1.getCount(); m++)   // Level 2
		{
			strTemp1 = strDict1.getString (m);
			strTemp1.TrimLeft(); strTemp1.TrimRight();
			if (strTemp1.IsEmpty())
				continue;
			
			ti.pszText  = strTemp1.GetBuffer(0);
			tis.hParent = hItem;
			tis.item = ti;
			
			hItem1 = m_treeProc.InsertItem(&tis);
			if (hItem1 == NULL)
				continue;
			
			str = strTemp;
			str += "-";
			str += strTemp1;

			id.setID((LPCSTR) str);

			StringList strDict2;
			procList->getMemberNames (id, strDict2, -1);
			for (int n = 0; n < strDict2.getCount(); n++)   // Level 3
			{
				strTemp2 = strDict2.getString (n);
				strTemp2.TrimLeft(); strTemp2.TrimRight();
				if (strTemp2.IsEmpty())
					continue;
				
				ti.pszText  = strTemp2.GetBuffer(0);
				tis.hParent = hItem1;
				tis.item    = ti;
				
				hItem2 = m_treeProc.InsertItem(&tis);
				if (hItem2 == NULL)
					continue;

				str = strTemp;   str += "-";
				str += strTemp1; str += "-";
				str += strTemp2;

				id.setID((LPCSTR) str);

				StringList strDict3;
				procList->getMemberNames (id, strDict3, -1);
				for (int x = 0; x < strDict3.getCount(); x++)   // Level 4
				{
					strTemp3 = strDict3.getString (x);
					strTemp3.TrimLeft(); strTemp3.TrimRight();
					if (strTemp3.IsEmpty())
						continue;
					
					ti.pszText  = strTemp3.GetBuffer(0);
					tis.hParent = hItem2;
					tis.item    = ti;
					
					hItem3 = m_treeProc.InsertItem(&tis);

					str = strTemp;   str += "-";
					str += strTemp1; str += "-";
					str += strTemp2; str += "-";
					str += strTemp3;
					
					id.setID((LPCSTR) str);
				}
			}
		}
	}
}

void CNameListMappingDlg::initListCtrl()
{
	// init mapping list_ctrl
	DWORD dwStyle = m_listMapping.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_listMapping.SetExtendedStyle( dwStyle );

	char* columnLabel[] = 
	{
		"Model name",
		"Start Seq",
		"End Seq",
		"No",
		"User name",
		"Start Seq",
		"End Seq",
		"No",
		"File name"
	};
	int defaultColumnWidth[] = { 60,30,30,30,60,30,30,30,80};
	for( int i=0; i<sizeof(defaultColumnWidth)/sizeof(int); i++ )
	{
		m_listMapping.InsertColumn( i, columnLabel[i], LVCFMT_CENTER, defaultColumnWidth[i] );
	}

	// init text list_ctrl
	dwStyle = m_listText.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_listText.SetExtendedStyle( dwStyle );
	CRect rect;
	m_listText.GetClientRect(rect);
	m_listText.InsertColumn(0,"No.",LVCFMT_CENTER, rect.Width());
}

void CNameListMappingDlg::loadUserNameList()
{
	// first clear the mapping ctrl
	m_listMapping.DeleteAllItems();

	const std::map< CString, CUserName>& tempmap = GetInputTerminal()->m_pUserNameListDB->getModelNameToUserNameMap();
	std::map< CString, CUserName>::const_iterator iter;
	for( iter = tempmap.begin(); iter!= tempmap.end(); ++iter )
	{
		std::vector<CString> vItemStr;
		if( GetInputTerminal()->m_pUserNameListDB->printUserName( iter->first, vItemStr) )
		{
			assert(vItemStr.size() ==9 );
			int iItemIdx = m_listMapping.InsertItem( m_listMapping.GetItemCount(), vItemStr[0] );
			for( int i=1; i<9; i++ )
			{
				m_listMapping.SetItemText( iItemIdx, i, vItemStr[i] );
			}
		}
		vItemStr.clear();
	}
}

void CNameListMappingDlg::OnSize(UINT nType, int cx, int cy) 
{
	if(m_bInit&&m_treeProc.m_hWnd)	
	{
		CRect rectWnd,rectWnd2;
		int nBottomOffset=66;
		int nMargin=10;
		m_treeProc.SetWindowPos(NULL,0,0,cx/5,cy-nBottomOffset,SWP_NOMOVE);
		m_treeProc.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);
		int nWidth=rectWnd.Width();
		m_wndSplitterLeft.SetWindowPos(NULL,rectWnd.right ,rectWnd.top,nMargin,rectWnd.Height(),NULL);
		
		int nBottom=rectWnd.bottom;
		
		m_staticProc.GetWindowRect(rectWnd2);
		ScreenToClient(rectWnd2);
		m_staticProc.SetWindowPos(NULL,0,0,nWidth,rectWnd2.Height(),SWP_NOMOVE);
		m_staticMapping.SetWindowPos(NULL,rectWnd.right+nMargin,rectWnd2.top,cx*2/5,rectWnd2.Height(),NULL);
		m_staticMapping.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);
		int nTop=rectWnd.top;
		int nHeight=rectWnd.Height();
		m_wndToolBarLeft.SetWindowPos(NULL,rectWnd.left,rectWnd.bottom+1,rectWnd.Width(),25,NULL);
		m_wndToolBarLeft.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);
		
		m_listMapping.SetWindowPos(NULL,rectWnd.left,rectWnd.bottom,rectWnd.Width(),cy-nBottomOffset-25,NULL);
		m_wndSplitterLeft.SetRange(nMargin+10,rectWnd.left+rectWnd.Width()-10);
		m_wndSplitterRight.SetWindowPos(NULL,rectWnd.left+rectWnd.Width() ,rectWnd.bottom,nMargin,cy-nBottomOffset-25,NULL);
		m_wndSplitterRight.SetRange(rectWnd.left+10,cx-nMargin-10);
		int nLeft=rectWnd.left+rectWnd.Width()+nMargin;
		m_listText.SetWindowPos(NULL,nLeft,rectWnd.bottom,cx-nMargin-nLeft,cy-nBottomOffset-25,NULL);
		m_listText.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);
		int nRight=rectWnd.right;
		m_wndToolBarRight.SetWindowPos(NULL,rectWnd.left,rectWnd.top-25,rectWnd.Width(),25,NULL);
		m_cmbScale.GetClientRect(rectWnd2);
		m_cmbScale.SetWindowPos(NULL,0,0,rectWnd.Width()-69,rectWnd2.Height(),SWP_NOMOVE);
		m_staticText.SetWindowPos(NULL,rectWnd.left,nTop,rectWnd.Width(),nHeight,NULL);

		m_butCancel.GetClientRect(rectWnd2);
		
		m_butCancel.SetWindowPos(NULL,nRight-rectWnd2.Width(),nBottom+nMargin,0,0,SWP_NOSIZE);
		m_butOK.SetWindowPos(NULL,nRight-rectWnd2.Width()*2-5,nBottom+nMargin,0,0,SWP_NOSIZE);
		m_butSave.SetWindowPos(NULL,nRight-rectWnd2.Width()*3-10,nBottom+nMargin,0,0,SWP_NOSIZE);
		m_butFile.SetWindowPos(NULL,nRight-rectWnd2.Width()*4-15,nBottom+nMargin,0,0,SWP_NOSIZE);

		InvalidateRect(NULL);
	}
	CDialog::OnSize(nType, cx, cy);
}


//////////////////////////////////////////////////////////////////////////
void CNameListMappingDlg::DeleteAllColumn()
{
	int nColNum = m_listText.GetHeaderCtrl()->GetItemCount();
	while(nColNum!=0)
	{
		m_listText.DeleteColumn(0);
		nColNum=m_listText.GetHeaderCtrl()->GetItemCount();
	}
}

int CNameListMappingDlg::GetColNumFromStr(CString strText,CString strSep,CStringArray& strArray)
{
	int nColNum=1;
	strArray.RemoveAll();

	int nPos=strText.Find(strSep,0);
	while(nPos!=-1)
	{
		nColNum++;
		strArray.Add(strText.Left(nPos));
		strText=strText.Right(strText.GetLength()-nPos-1);
		nPos=strText.Find(strSep,0);
	}
	strArray.Add(strText);
	return nColNum;
}

void CNameListMappingDlg::AddColumn(int nColNum)
{
	for(int j=0;j<nColNum;j++)
	{
		CString strColumn;
		strColumn.Format("Col %d",j+1);
		m_listText.InsertColumn(m_listText.GetHeaderCtrl()->GetItemCount(),strColumn,LVCFMT_CENTER,50);
	}
}

void CNameListMappingDlg::SetItemText(CStringArray& strArray)
{
	int nLineNum=m_listText.GetItemCount();
	CString strLineNum;
	strLineNum.Format("%d",m_listText.GetItemCount()+1 );
	int nLineIndex=m_listText.InsertItem(m_listText.GetItemCount(),strLineNum );
	for(int k=0;k<strArray.GetSize();k++)
	{
		m_listText.SetItemText(nLineIndex,k+1,strArray[k]);
	}
}
//////////////////////////////////////////////////////////////////////////

void CNameListMappingDlg::OnButtonFile() 
{
	// TODO: Add your control notification handler code here
	CString strFilter = "CSV Files (*.csv)|*.csv|Text Files (*.txt)|*.txt|All Types (*.*)|*.*||";
	CFileDialog filedlg( TRUE ,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter, this );
	if( filedlg.DoModal() == IDOK )
	{
		if( readTextFromFile(filedlg.GetPathName()) )
		{
			int iCol,iRow;
			char chDelimiter = GetDelimiter();
			addTextToList( chDelimiter, iCol, iRow );
			m_strFileName = filedlg.GetPathName();
			m_bHaveAssignTitle = false;
		}
	}
}

bool CNameListMappingDlg::readTextFromFile( const CString& _strFile)
{
	CString strMsg, strLine;
	CStdioFile file;
	if(!file.Open( _strFile,CFile::modeRead ))
	{
		strMsg.Format("Can not open file:\r\n%s", _strFile );
		MessageBox( strMsg,"Error",MB_OK|MB_ICONERROR );
		return false;
	}
	
	m_strLineArray.RemoveAll();	// clear the array
	while( file.ReadString( strLine) )
	{
		m_strLineArray.Add( strLine );
	}
	
	file.Close();
	return true;
}

void  CNameListMappingDlg::addTextToList(char chDelimiter, int& _col,int& _row )
{
	CWaitCursor wc;
	// clear text list
	m_listText.DeleteAllItems();
	DeleteAllColumn();
	
	if( (_row = m_strLineArray.GetSize()) == 0)
		return;
	
	CString strLine = m_strLineArray.GetAt(0);
	if(strLine.IsEmpty())
		return;
	m_listText.InsertColumn(0,"No.",LVCFMT_CENTER, 40);
	
	CStringArray strArray;
	_col = GetColNumFromStr( strLine, chDelimiter, strArray );
	
	AddColumn( _col );
	SetItemText(strArray);
	for( int i=1; i<_row; i++)
	{
		strLine = m_strLineArray.GetAt(i);
		int nColNum = GetColNumFromStr( m_strLineArray.GetAt(i), chDelimiter, strArray );
		if( _col<nColNum )
		{
			AddColumn( nColNum-_col);
			_col = nColNum;
		}
		SetItemText( strArray );
	}
	m_listText.m_nItemHeight = m_listText.GetItemHeight();
	
	
}
	

void CNameListMappingDlg::OnBegindragTreeProc(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	//////////////////////////////////////////////////////////////////
	m_hItemDragSrc = pNMTreeView->itemNew.hItem;
	m_hItemDragDes = NULL;
	
	if(m_pDragImage)
		delete m_pDragImage;
	
	m_pDragImage = m_treeProc.CreateDragImage( m_hItemDragSrc);
	if( !m_pDragImage)
		return;
	
	m_bDragging = true;
	m_pDragImage->BeginDrag ( 0,CPoint(8,8) );
	CPoint  pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter ( GetDesktopWindow (),pt );  
	
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	SetCapture();
}

void CNameListMappingDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( m_bDragging )
	{
		CPoint pt (point);
		ClientToScreen (&pt);
		
		CRect rectProcessTree;
		m_listMapping.GetWindowRect( &rectProcessTree );
		if( rectProcessTree.PtInRect( pt) && m_listMapping.IsWindowEnabled() )	
		{
			//PopProcessDefineMenu( point );
			dragProcToListCtrl();
		}
		
		m_bDragging = false;
		ReleaseCapture();
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		// end dragging
		m_pDragImage->DragLeave (GetDesktopWindow ());
		m_pDragImage->EndDrag ();
		delete m_pDragImage;
		m_pDragImage = NULL;
	}
	if (m_pDragImage && m_pDragWnd) // In Drag&Drop mode ?
	{
		CWaitCursor wc;
		::ReleaseCapture();
		m_pDragImage->DragLeave(GetDesktopWindow());
		m_pDragImage->EndDrag();
		
		CPoint pt(point); 
		ClientToScreen(&pt);
		CWnd* m_pDropWnd = WindowFromPoint(pt);
		
		m_pDragImage->DeleteImageList();
		delete m_pDragImage;
		m_pDragImage = NULL;
		if(m_pDragWnd!=m_pDropWnd&&m_pDropWnd->GetParent()!=m_pDragWnd)
		{
			//Move Dragged Column Data To Target Column
			if((m_pDropWnd==&m_listMapping||m_pDropWnd==m_listMapping.GetHeaderCtrl()))
			{
				CPoint pt( point );
				ClientToScreen( &pt );
				m_listMapping.ScreenToClient( &pt );
				int iItemIndex = m_listMapping.HitTest( pt );
				if( iItemIndex==-1 || iItemIndex> m_listMapping.GetItemCount() )
				{
					MessageBox("Can not drag the column to this row.\r\n","Warning", MB_OK|MB_ICONINFORMATION );
				}
				else
				{
					CString strProcName = m_listMapping.GetItemText( iItemIndex, 0 );
					std::vector<CString> vItemString;
					if( strProcName.IsEmpty() ||
						!getColumnText( vItemString, m_listText.m_nColIndexDragged ) )
					{
						//MessageBox("Can not drag the column to mapping list.\r\n","Error", MB_OK|MB_ICONERROR );
					}
					else
					{
						// init name
						CUserName name;
						std::vector<int> vSeqNum;
						int imin,imax;
						imin = imax = -1;
						for( int k=1; k<static_cast<int>(vItemString.size()); k++ )
						{
							int num = atoi( vItemString[k] );
							imin = imin==-1 ? num : min(imin,num) ;
							imax = max( imax, num);
							vSeqNum.push_back( num );
						}
						name.setName( vItemString[0] );
						name.setSeqStart( imin );
						name.setSeqEnd( imax );
						name.setNum( vSeqNum.size() );
						name.setSeqNumList( vSeqNum );
						name.setFileName( m_strFileName );

						ProcessorID procID;
						procID.SetStrDict( GetInputTerminal()->inStrDict );
						procID.setID( strProcName );

						if( !GetInputTerminal()->m_pUserNameListDB->modifyUseName( procID, name ) )
						{
							//MessageBox("Can not drag the column to the processor.\r\n","Error", MB_OK|MB_ICONERROR );
						}
						else
						{
							vItemString.clear();
							if( GetInputTerminal()->m_pUserNameListDB->printUserName( strProcName, vItemString ) )
							{
								assert(vItemString.size() ==9 );
								for( int i=0; i<9; i++ )
								{
									m_listMapping.SetItemText( iItemIndex, i, vItemString[i] );
								}
							}

							// delete column of text list 
							//m_listText.DeleteColumn(m_listText.m_nColIndexDragged);
						}
					}

				}
				/*	
				int nColNum=m_listMapping.GetHeaderCtrl()->GetItemCount();
				int nWidthColTotal=0;
				CPoint ptCol=pt;
				m_listMapping.GetHeaderCtrl()->ScreenToClient(&ptCol);
				int nColIndexDrop=m_listMapping.GetColIndexUnderPoint(ptCol,nColNum);
				
				if(m_bColumnDropped[nColIndexDrop]==FALSE&&nColIndexDrop<m_listMapping.GetHeaderCtrl()->GetItemCount())
				{
					for(int j=0;j<m_listText.GetItemCount();j++)
					{
						if(m_listMapping.GetItemCount()!=m_listText.GetItemCount())
							m_listMapping.InsertItem(j,"");
						m_listMapping.SetItemText(j,nColIndexDrop,m_listText.GetItemText(j,m_listText.m_nColIndexDragged));
					}
					m_listText.DeleteColumn(m_listText.m_nColIndexDragged);
					m_bColumnDropped[nColIndexDrop]=TRUE;
				}
				*/
			}
			else if((m_pDropWnd==&m_listText||m_pDropWnd==m_listText.GetHeaderCtrl()))
			{		
				int nColNumSourceList=m_listText.GetHeaderCtrl()->GetItemCount();
				CString strColumn;
				LVCOLUMN lvcol;
				lvcol.mask=LVCF_TEXT ;
				char strBuf[256];
				lvcol.pszText=strBuf;
				lvcol.cchTextMax=sizeof(strBuf);
				m_listMapping.GetColumn(m_listMapping.m_nColIndexDragged,&lvcol);
				strColumn=lvcol.pszText;
				m_listText.InsertColumn(nColNumSourceList,strColumn,LVCFMT_CENTER,50);
				for(int j=0;j<m_listMapping.GetItemCount();j++)
				{
					CString strItem=m_listMapping.GetItemText(j,m_listMapping.m_nColIndexDragged);
					m_listText.SetItemText(j,nColNumSourceList,strItem);
					m_listMapping.SetItemText(j,m_listMapping.m_nColIndexDragged,"");
				}
				m_bColumnDropped[m_listMapping.m_nColIndexDragged]=FALSE;				
			}		
			
		}
		
		m_listText.EndDrag();
		m_listMapping.EndDrag();
		m_pDragWnd = NULL;
		

	}
}

void CNameListMappingDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	//HTREEITEM  hItem;
	//UINT       flags;
	
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
		m_listMapping.GetWindowRect( &rectProcessTree );
		//ClientToScreen( &rectProcessTree );
		if( rectProcessTree.PtInRect( pt) )	
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

		}
		else
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		}
		m_pDragImage->DragShowNolock (TRUE);
		m_treeProc.Invalidate(FALSE);
	}
	if (m_pDragImage && m_pDragWnd) // In Drag&Drop mode ?
	{
		CPoint ptDropPoint(point);
		ClientToScreen(&ptDropPoint);
		m_pDragImage->DragMove(ptDropPoint);
		CWnd* pDropWnd = CWnd::WindowFromPoint(ptDropPoint);
		HCURSOR hCursor;
		if(pDropWnd == m_listMapping.GetHeaderCtrl()||pDropWnd == &m_listMapping)
		{
			int nColNum=m_listMapping.GetHeaderCtrl()->GetItemCount();
			m_listMapping.GetHeaderCtrl()->ScreenToClient(&ptDropPoint);
			int nColIndexDrop=m_listMapping.GetColIndexUnderPoint(ptDropPoint,nColNum);
			if(nColIndexDrop<m_listMapping.GetHeaderCtrl()->GetItemCount()&&m_bColumnDropped[nColIndexDrop]==FALSE)
				hCursor=m_hcArrow;
			else
				hCursor=m_hcNo;
		}
		else
			hCursor=m_hcNo;
		SetCursor(hCursor);
	}
	CDialog::OnMouseMove(nFlags,point);
}

CNameListMappingDlg::~CNameListMappingDlg()
{
	// clear drag image
	if( m_pDragImage )
		delete m_pDragImage;
	
}

LRESULT CNameListMappingDlg::OnBegindrag(WPARAM wParam,LPARAM lParam)
{
	int nColIndexDragged=wParam;
	CListCtrl* pList=(CListCtrl*)lParam;
	if(pList==&m_listText&&nColIndexDragged==0)
	{
		m_listText.EndDrag();
		return TRUE;
	}
	

	POINT pt;
	m_pDragImage = CreateDragImageEx(nColIndexDragged,pList, &pt);
	if (m_pDragImage == NULL)
		return TRUE;
	m_pDragWnd=pList;
	m_pDragImage->BeginDrag(0, CPoint(0,0));
	CPoint ptPoint;
	GetCursorPos(&ptPoint);
	m_pDragImage->DragEnter(GetDesktopWindow(), ptPoint);
	SetCapture();
	return TRUE;
}

CImageList* CNameListMappingDlg::CreateDragImageEx(int nColIndexDragged,CListCtrl *pList, LPPOINT lpPoint)
{
	
	CRect rectComplete(0,0,0,0);
	
	// Determine List Control Client width size
	int nWidthCol=pList->GetColumnWidth(nColIndexDragged);
	rectComplete.right=nWidthCol;
	CListCtrlDrag* pLCD=(CListCtrlDrag*)pList;
	int nItemHeight=pLCD->m_nItemHeight;
	int nCPP=pList->GetCountPerPage();
	CRect rcHeaderCtrl;
	pList->GetHeaderCtrl()->GetClientRect(rcHeaderCtrl);
	rectComplete.bottom=nCPP*nItemHeight+rcHeaderCtrl.Height();
	
	
	// Determine the size of the drag image (limite for rows visibled and Client width)
	
	CClientDC dcClient(this);
	CDC dcMem;
	CBitmap Bitmap;
	
	if (!dcMem.CreateCompatibleDC(&dcClient))
		return NULL;
	
	if (!Bitmap.CreateCompatibleBitmap(&dcClient, rectComplete.Width(), rectComplete.Height()))
		return NULL;
	
	CBitmap *pOldMemDCBitmap = dcMem.SelectObject(&Bitmap);
	// Use green as mask color
	dcMem.FillSolidRect(0, 0, rectComplete.Width(), rectComplete.Height(), RGB(0,255,0));
	
	// Paint each DragImage in the DC
	int nCountRow=pList->GetItemCount();
	
	int nVisibleRowNum=0;
	CFont* pOleFont=dcMem.SelectObject(pList->GetFont());
	CImageList* pColIL=pList->GetHeaderCtrl()->CreateDragImage(nColIndexDragged);
	pColIL->Draw(&dcMem,0,CPoint(0,0),ILD_NORMAL);
	
	CRect rcItem;
	for(int i=pList->GetTopIndex();i<nCountRow;i++)
	{
		rcItem=CRect(CPoint(0,rcHeaderCtrl.Height()+nVisibleRowNum*nItemHeight),CSize(nWidthCol,nItemHeight));
		dcMem.DrawText(pList->GetItemText(i,nColIndexDragged),rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		nVisibleRowNum++;
		if(nVisibleRowNum>nCPP)
			break;
	}
	dcMem.SelectObject(pOleFont);
	dcMem.SelectObject(pOldMemDCBitmap);
	CImageList* pCompleteImageList = new CImageList;
	pCompleteImageList->Create(rectComplete.Width(), rectComplete.Height(), ILC_COLOR | ILC_MASK, 0, 1);
	pCompleteImageList->Add(&Bitmap, RGB(0, 255, 0)); // Green is used as mask color
	Bitmap.DeleteObject();
	
	if (lpPoint)
	{
		lpPoint->x = rectComplete.left;
		lpPoint->y = rectComplete.top;
	}
	
	return pCompleteImageList;
}

void CNameListMappingDlg::OnDeleteColumn()
{
	m_listText.DeleteColumn(m_nColIndexClicked);
	m_nColIndexClicked=-1;
	m_wndToolBarRight.GetToolBarCtrl().EnableButton(ID_DELETE_COLUMN,FALSE);
	
}

void CNameListMappingDlg::OnDeleteRow()
{
	POSITION pos = m_listText.GetFirstSelectedItemPosition();
	int nCountSeled=m_listText.GetSelectedCount();
	if(!pos) return;
	int sel = -1;
	while (pos)
	{
		sel = m_listText.GetNextSelectedItem(pos);
		break;
	}
	if(sel==-1) return;
	for(int i=0;i<nCountSeled;i++)
		m_listText.DeleteItem(sel);
	m_nSeledRowIndex=-1;
	m_wndToolBarRight.GetToolBarCtrl().EnableButton(ID_DELETE_ROW,FALSE);
}

void CNameListMappingDlg::OnClickListText(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_nSeledRowIndex=pNMListView->iItem;
	if(m_nSeledRowIndex!=-1)
		m_wndToolBarRight.GetToolBarCtrl().EnableButton(ID_DELETE_ROW,TRUE);
	else
		m_wndToolBarRight.GetToolBarCtrl().EnableButton(ID_DELETE_ROW,FALSE);
	
	*pResult = 0;
}

void CNameListMappingDlg::OnDblclkListText(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_listText.GetFirstSelectedItemPosition();
	if( pos )
	{
		int iCurSelItem = m_listText.GetNextSelectedItem( pos );
		//////////////////////////////////////////////////////////////////////////
		//if the item is selected, make it unselected
		if( m_listText.GetItemState( iCurSelItem, LVIS_SELECTED ) == LVIS_SELECTED )
			m_listText.SetItemState( iCurSelItem, ~LVIS_SELECTED, LVIS_SELECTED );
	}
	
	*pResult = 0;
}

LRESULT CNameListMappingDlg::OnClickColumn(WPARAM wParam,LPARAM lParam)
{
	m_nColIndexClicked=wParam;
	if(wParam!=-1 && wParam!=0 )	// can not delete the first column
		m_wndToolBarRight.GetToolBarCtrl().EnableButton(ID_DELETE_COLUMN,TRUE);
	else
		m_wndToolBarRight.GetToolBarCtrl().EnableButton(ID_DELETE_COLUMN,FALSE);
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CNameListMappingDlg::dragProcToListCtrl()
{
	assert( m_hItemDragSrc );

	CString strProcName = m_treeProc.GetItemText( m_hItemDragSrc );
	HTREEITEM hParent = m_treeProc.GetParentItem( m_hItemDragSrc );
	while( hParent )
	{
		strProcName = m_treeProc.GetItemText( hParent ) + "-" + strProcName;
		hParent = m_treeProc.GetParentItem( hParent );
	}
	
	ProcessorID prccID;
	prccID.SetStrDict( GetInputTerminal()->inStrDict );
	prccID.setID( strProcName );

	if( !GetInputTerminal()->m_pUserNameListDB->addUserName( prccID ) )
	{
		//MessageBox("Can not add the processor to mapping.","Error",MB_OK|MB_ICONERROR );
		return;
	}

	std::vector<CString> vStrItem;
	if(GetInputTerminal()->m_pUserNameListDB->printUserName( strProcName, vStrItem ))
	{		
		assert(vStrItem.size() ==9 );
		int iItemIdx = m_listMapping.InsertItem( m_listMapping.GetItemCount(), vStrItem[0] );
		for( int i=1; i<9; i++ )
		{
			m_listMapping.SetItemText( iItemIdx, i, vStrItem[i] );
		}

		m_listMapping.SetCheck( iItemIdx, TRUE );
	}

}

LRESULT CNameListMappingDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_NOTIFY)
	{
		if (wParam == IDC_STATIC_SPLITTER )
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			DoResizeLeft(pHdr->delta);
		}
		else if (wParam == IDC_STATIC_SPLITTER+1 )
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			DoResizeRight(pHdr->delta);
		}
	}	
	return CDialog::DefWindowProc(message, wParam, lParam);
}



void CNameListMappingDlg::DoResizeLeft(int delta)
{
	CSplitterControl::ChangeWidth(&m_staticProc,delta);
	CSplitterControl::ChangeWidth(&m_treeProc,delta);
	CSplitterControl::ChangeWidth(&m_staticMapping,-delta,CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_wndToolBarLeft,-delta,CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_listMapping,-delta,CW_RIGHTALIGN);
	int nMin,nMax;
	m_wndSplitterRight.GetRange(nMin,nMax);
	m_wndSplitterRight.SetRange(nMin+delta,nMax);

	InvalidateRect(NULL);
}

void CNameListMappingDlg::DoResizeRight(int delta)
{
	CSplitterControl::ChangeWidth(&m_staticMapping,delta);
	CSplitterControl::ChangeWidth(&m_wndToolBarLeft,delta);
	CSplitterControl::ChangeWidth(&m_listMapping,delta);
	CSplitterControl::ChangeWidth(&m_staticText,-delta,CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_wndToolBarRight,-delta,CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_listText,-delta,CW_RIGHTALIGN);
	int nMin,nMax;
	m_wndSplitterLeft.GetRange(nMin,nMax);
	m_wndSplitterLeft.SetRange(nMin,nMax+delta);

	InvalidateRect(NULL);
}

char CNameListMappingDlg::GetDelimiter()
{
	CString strCmobText;
	m_cmbScale.GetWindowText( strCmobText );
	strCmobText.TrimLeft();
	strCmobText.TrimRight();

	if( strCmobText == "Comma" )
		return ',';
	if( strCmobText == "Semicolon" )
		return ';';
	if( strCmobText == "Space" )
		return ' ';
	if( strCmobText == "Tab" )
		return '\t';
	return ',';
}


bool CNameListMappingDlg::getColumnText( std::vector<CString>& _vItemString, int _iColumn )
{
	POSITION pos = m_listText.GetFirstSelectedItemPosition();
	if( pos!= NULL )		// one item is selected
	{
		int iCurSelItem = m_listText.GetNextSelectedItem( pos );
		CString strItem = m_listText.GetItemText( iCurSelItem, _iColumn );

		if( isNumString( strItem ) )	//???
		{
			// if the select string is num string, shall drag the all column
			return getEntireColumText( _vItemString, _iColumn )	;
		}
		else
		{
			return getSomeColumnText( strItem, _vItemString, _iColumn, iCurSelItem );
		}
	}
	else					// no item is selected
	{
		// if no item is selected, we shall drag the all column
		return getEntireColumText( _vItemString, _iColumn );
	}
}

//  in: a string just like "DOOR-1"
// out: strName "DOOR" and strNum "1"
// if can parse the string, return true( "DOOR-1" )
// else return false( "DOOR" or "1" )
bool CNameListMappingDlg::getUserNameAndNum( const CString& strItem, CString& strName, CString& strNum )
{
	int pos,prepos;
	pos = prepos = strItem.Find('-');
	if( pos== -1)
		return false;

	while( pos!= -1 )
	{
		prepos = pos;
		pos = strItem.Find( '-', prepos+1 );
	}
	strName = strItem.Left( prepos );
	strNum = strItem.Mid( prepos+1 );
	
	return isNumString( strNum );
}


bool CNameListMappingDlg::getSomeColumnText(  const CString& _strItem, std::vector<CString>& _vItemString, int _iColumn,int _iCurSel )
{
	CString strMsg;
	CString userName,strNum;
	CString strItem = _strItem;
	int iItemCount = m_listText.GetItemCount();

	if( !getUserNameAndNum(strItem, userName, strNum ))
	{
		strMsg.Format("the string: %s is invalid", strItem );
		MessageBox( strMsg,"Warning",MB_OK|MB_ICONINFORMATION );
		return false;
	}
		
	_vItemString.push_back( userName );
	_vItemString.push_back( strNum );
	for( int i=0; i<iItemCount; i++ )
	{	
		if( i == _iCurSel )
			continue;
		
		strItem = m_listText.GetItemText( i, _iColumn );
		strItem.TrimLeft();
		strItem.TrimRight();
		if( strItem.IsEmpty() )
			continue;
		if( getUserNameAndNum( strItem, userName, strNum ))
		{
			if( userName == _vItemString[0] )
			{
				// only add unique string
				for( int k=0; k<static_cast<int>(_vItemString.size()); k++ )
				{
					if( _vItemString[k] == strNum )
						break;
				}
				if( k== _vItemString.size() )
					_vItemString.push_back( strNum );
			}
		}
	}
	return true;
}

bool CNameListMappingDlg::getEntireColumText( std::vector<CString>& _vItemString, int _iColumn )
{
	CString strMsg;
	// get column title
	HDITEM hd;
	char szBuf[256];
	hd.mask		   = HDI_TEXT;
	hd.cchTextMax  = 256;
	hd.pszText	   = szBuf;
	m_listText.GetHeaderCtrl()->GetItem( _iColumn, &hd );
	CString strTitle = CString(szBuf );
	strTitle.TrimLeft();
	strTitle.TrimRight();
	strTitle.MakeUpper();

	if( strTitle.IsEmpty() )
	{
		strMsg.Format( "The title of the column is empty,so can not drag the column" );
		MessageBox( strMsg,"Warning",MB_OK|MB_ICONINFORMATION );
		return false;
	}

	int iItemCount = m_listText.GetItemCount();
	_vItemString.push_back( strTitle );
	for( int i=0; i<iItemCount; i++ )
	{
		CString strItem = m_listText.GetItemText( i, _iColumn );
		strItem.Remove(' ');
		if( strItem.IsEmpty() )
			continue;
		
		for( int j=0; j<strItem.GetLength(); j++ )
		{
			if( !isNumeric( strItem[j] ))
			{
				strMsg.Format(" The string: %s is not numeral", strItem );
				MessageBox( strMsg,"Warning",MB_OK|MB_ICONINFORMATION );
				return false;
			}
			// only add unique string
			for( int k=0; k<static_cast<int>(_vItemString.size()); k++ )
			{
				if( _vItemString[k] == strItem )
					break;
			}
			if( k == _vItemString.size() )
				_vItemString.push_back( strItem );
		}
	}

	return true;
}


void CNameListMappingDlg::OnItemchangedListMapping(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	POSITION pos = m_listMapping.GetFirstSelectedItemPosition();
	if( pos )
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE, TRUE );
	else
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE, FALSE );
	
	*pResult = 0;
}

void CNameListMappingDlg::OnPeoplemoverDelete() 
{
	// TODO: Add your command handler code here]
	POSITION pos = m_listMapping.GetFirstSelectedItemPosition();
	if( pos )
	{
		int iItemIdx = m_listMapping.GetNextSelectedItem(pos);

		CString strProcName = m_listMapping.GetItemText( iItemIdx,0 );
		ProcessorID procID;
		procID.SetStrDict( GetInputTerminal()->inStrDict );
		procID.setID( strProcName );
		
		GetInputTerminal()->m_pUserNameListDB->deleteUserName( procID );
		m_listMapping.DeleteItem( iItemIdx );
	}
}

void CNameListMappingDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	if( GetInputTerminal()->m_pUserNameListDB->canSave() )
	{	
		GetInputTerminal()->m_pUserNameListDB->saveDataSet( GetProjectPath(), false );
		return;
	}

	MessageBox("There is a processor do not define user name!\r\n","ERROR",MB_OK|MB_ICONERROR );
}

void CNameListMappingDlg::OnOK() 
{
	if( GetInputTerminal()->m_pUserNameListDB->canSave() )
	{	
		GetInputTerminal()->m_pUserNameListDB->saveDataSet( GetProjectPath(), false );
		CDialog::OnOK();
		return;
	}
	
	MessageBox("There is a processor do not define user name!\r\n","ERROR",MB_OK|MB_ICONERROR );
}

void CNameListMappingDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	GetInputTerminal()->m_pUserNameListDB->loadDataSet( GetProjectPath() );
	CDialog::OnCancel();
}

void CNameListMappingDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.x = 600;
	lpMMI->ptMinTrackSize.y = 400;
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CNameListMappingDlg::OnPaint() 
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
			pWnd->IsKindOf(RUNTIME_CLASS(CListCtrl))||
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

BOOL CNameListMappingDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;
}

BOOL CNameListMappingDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
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

void CNameListMappingDlg::OnComboxSelectChanged()
{
	if( m_strFileName.IsEmpty() )
		return;
	
	if( readTextFromFile(m_strFileName) )
	{
		int iCol,iRow;
		char chDelimiter = GetDelimiter();
		addTextToList( chDelimiter, iCol, iRow );
	}	
}

bool CNameListMappingDlg::isNumString( const CString& _strNum )
{
	for( int i=0; i<_strNum.GetLength(); i++ )
		if( !isNumeric(_strNum[i]) )
			return false;
		
		return true;
}



void CNameListMappingDlg::OnItemchangedListText(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	POSITION pos = m_listText.GetFirstSelectedItemPosition();
	if( pos!= NULL && !m_bHaveAssignTitle)
	{
		m_wndToolBarRight.GetToolBarCtrl().EnableButton( ID_NAMELIST_TITLE, TRUE );
	}
	else
	{
		m_wndToolBarRight.GetToolBarCtrl().EnableButton( ID_NAMELIST_TITLE, FALSE );
	}
	
	*pResult = 0;
}

void CNameListMappingDlg::OnNamelistTitle() 
{
	POSITION pos = m_listText.GetFirstSelectedItemPosition();
	if( pos!= NULL )
	{
		int iItemIdx = m_listText.GetNextSelectedItem(pos);
		// get item text and remove the row
		std::vector<CString> vItemText;
		for( int i=0; i<m_listText.GetHeaderCtrl()->GetItemCount(); i++ )
		{
			CString itemText = m_listText.GetItemText( iItemIdx, i );
			vItemText.push_back( itemText );
		}
		
		m_listText.DeleteItem( iItemIdx );
		
		// set the colum title
		HDITEM hd;
		for( i=0; i<m_listText.GetHeaderCtrl()->GetItemCount(); i++ )
		{
			m_listText.GetHeaderCtrl()->GetItem( i,&hd);
			hd.mask |= HDI_TEXT;
			hd.pszText = vItemText[i].GetBuffer(0);
			hd.cchTextMax = 128;
			m_listText.GetHeaderCtrl()->SetItem( i, &hd );
		}
		
		m_wndToolBarRight.GetToolBarCtrl().EnableButton( ID_NAMELIST_TITLE, FALSE );
		m_bHaveAssignTitle = true;
	}
}

void CNameListMappingDlg::OnSelchangedTreeProc(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	//////////////////////////////////////////////////////////////////////////
	HTREEITEM hCurSelItem  = m_treeProc.GetSelectedItem();
	if( hCurSelItem )
	{
		CString strProcName = m_treeProc.GetItemText( hCurSelItem );
		HTREEITEM hParent = m_treeProc.GetParentItem( hCurSelItem );
		while( hParent )
		{
			strProcName = m_treeProc.GetItemText( hParent ) + "-" + strProcName;
			hParent = m_treeProc.GetParentItem( hParent );
		}
		
		ProcessorID procID;
		procID.SetStrDict( GetInputTerminal()->inStrDict );
		procID.setID( strProcName );
		//////////////////////////////////////////////////////////////////////////
		if( GetInputTerminal()->m_pUserNameListDB->isProcIDValid( procID ) )
			m_wndToolBarLeft.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW, TRUE );
		else
			m_wndToolBarLeft.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW, FALSE );
	}
	else
	{
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW, FALSE );	
	}

	*pResult = 0;
}


void CNameListMappingDlg::OnPeoplemoverNew() 
{
	HTREEITEM hCurSelItem  = m_treeProc.GetSelectedItem();
	if( hCurSelItem )
	{
		CString strProcName = m_treeProc.GetItemText( hCurSelItem );
		HTREEITEM hParent = m_treeProc.GetParentItem( hCurSelItem );
		while( hParent )
		{
			strProcName = m_treeProc.GetItemText( hParent ) + "-" + strProcName;
			hParent = m_treeProc.GetParentItem( hParent );
		}
		
		ProcessorID prccID;
		prccID.SetStrDict( GetInputTerminal()->inStrDict );
		prccID.setID( strProcName );
		//////////////////////////////////////////////////////////////////////////
		
		if( GetInputTerminal()->m_pUserNameListDB->addUserName( prccID ) )
		{
			std::vector<CString> vStrItem;
			if(GetInputTerminal()->m_pUserNameListDB->printUserName( strProcName, vStrItem ))
			{		
				assert(vStrItem.size() ==9 );
				int iItemIdx = m_listMapping.InsertItem( m_listMapping.GetItemCount(), vStrItem[0] );
				for( int i=1; i<9; i++ )
				{
					m_listMapping.SetItemText( iItemIdx, i, vStrItem[i] );
				}
				
				m_listMapping.SetCheck( iItemIdx, TRUE );
			}
		}
	
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW, FALSE );
	}
	else
	{
		MessageBox(" No item is selected in the left tree","Warning", MB_OK|MB_ICONINFORMATION );
		m_wndToolBarLeft.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW, FALSE );	
	}
}
