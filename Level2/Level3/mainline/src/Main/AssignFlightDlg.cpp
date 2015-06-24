// AssignFlightDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "AssignFlightDlg.h"

#include "TermPlanDoc.h"
#include "PassengerTypeDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAssignFlightDlg dialog

#define DEPITEM 1
#define ARRITEM 2

CAssignFlightDlg::CAssignFlightDlg(InputTerminal* _terminal,CWnd* pParent,CString& _strProcGroupName)
	: m_cout_pro(0),CDialog(CAssignFlightDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAssignFlightDlg)
	//}}AFX_DATA_INIT
	p_InTerm = _terminal;
	strProcGroupName = _strProcGroupName;
    initProcessorName(_strProcGroupName) ;
	m_bDragging = false;
	m_pDragImage = NULL;
}

void CAssignFlightDlg::initProcessorName(CString& groupName)
{
	ProcessorID id;
	id.SetStrDict( p_InTerm->inStrDict );
	id.setID( groupName );
	GroupIndex index = p_InTerm->GetProcessorList()->getGroupIndex(id) ;
	TCHAR name[1024] = {0};
	CString Cname;
	ProcessorID pro_id ;
	pro_id.SetStrDict(p_InTerm->inStrDict);
	if(index.start < 0 || index.end < 0)
		return ;
	for (int i = index.start ; i <= index.end ;i++)
	{
		p_InTerm->GetProcessorList()->getNameAt(i,name) ;
		Cname.Format(name);
		Cname.Replace(',','-');
		pro_id.setID(Cname) ;
		m_proNamesList.push_back(pro_id.GetIDString()) ;
		m_cout_pro++ ;
	}
}
void CAssignFlightDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAssignFlightDlg)
	DDX_Control(pDX, IDC_LIST_ALL, m_listAll);
	DDX_Control(pDX, IDOK, m_butApply);
	DDX_Control(pDX, IDC_SAVE, m_butSave);
	DDX_Control(pDX, IDC_GTCHARTCTRL, m_gtCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAssignFlightDlg, CDialog)
	//{{AFX_MSG_MAP(CAssignFlightDlg)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_ALL, OnBegindragListAll)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAssignFlightDlg message handlers
BOOL CAssignFlightDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitListCtrl();
	SetListData();

	InitGtCtrl();
	
	//CString strTitle;
	//strTitle.Format("Assign to %s ",strProcName );
	//SetWindowText(strTitle );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CString CAssignFlightDlg::GetProjectPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}


void CAssignFlightDlg::InitGtCtrl()
{
	m_gtCtrl.InitControl((long)AfxGetThread() );
	m_gtCtrl.SetMoveItemSnapLineable( false );

	long lXPos,lYPos;
	m_gtCtrl.GetScrollRange( &lXPos,&lYPos );
	m_gtCtrl.SetScrollPos(lXPos/2-lXPos*3/24,lYPos);
	m_gtCtrl.ShowBtn(0,false);
	m_gtCtrl.ShowBtn(1,false);
	//m_gtCtrl.ShowBtn(2,false);
	//m_gtCtrl.ShowBtn(5,false);
	//m_gtCtrl.ShowBtn(6,false);
	m_gtCtrl.ShowBtn(9,false);
	m_gtCtrl.ShowBtn(10,false);
	m_gtCtrl.ShowBtn(11,false);
	m_gtCtrl.ShowBtn(12,false);
	m_gtCtrl.ShowBtn(15,false);
	m_gtCtrl.ShowBtn(16,false);
	m_gtCtrl.ShowBtn(17,false);
	int _lineIndex  = 0 ;
	for (int i = 0 ; i < (int)m_proNamesList.size() ; i++)
	{
		_lineIndex = m_gtCtrl.AddLine(m_proNamesList[i],NULL) ;
		m_gtCtrl.SetLineData(_lineIndex,i) ;
	}
}

void CAssignFlightDlg::InitListCtrl()
{
	DWORD dwStyle = m_listAll.GetExtendedStyle();
	dwStyle |= LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listAll.SetExtendedStyle( dwStyle );
	char* columnLabel[] = {"Mobile Element", "Open Interval for Departures(min)","Open Interval for Arrrivals(min)" ,"Number of Processors","IsDaily"};
	int DefaultColumnWidth[] = { 270, 170,170 ,170,50};
	int nFormat[] = { LVCFMT_CENTER, LVCFMT_CENTER,LVCFMT_CENTER,LVCFMT_CENTER,LVCFMT_CENTER};

	for( int i=0; i<5; i++ )
	{
		m_listAll.InsertColumn(i,columnLabel[i], nFormat[i], DefaultColumnWidth[i]);
	}

	m_imageList.Create(16, 16, ILC_MASK, 0, 4);
	m_imageList.Add( AfxGetApp()->LoadIcon( IDI_MOBIL_ELEMENT_TYPE ) );
	m_listAll.SetImageList( &m_imageList, LVSIL_SMALL );
}


void CAssignFlightDlg::SetListData()
{
	CString sbuf;
	CString strDep,strArr;
	m_listAll.DeleteAllItems();

	int iCount = p_InTerm->m_pProSwitchTimeDB->getCount();
	for( int i=0; i<iCount; i++ )
	{
		ProSwitchTime pst = p_InTerm->m_pProSwitchTimeDB->getItem( i );
         int _proName = pst.getProcessNum() ;
		 
		// _itoa(_proName,)
		pst.getMobType()->screenPrint( sbuf,0,256 );
		int idx = m_listAll.InsertItem(i,sbuf.GetBuffer(0));
		strDep.Format("%-4d  to  %-4d",pst.getDepOpenTime(),pst.getDepCloseTime() );
		strArr.Format("%-4d  to  %-4d",pst.getArrOpenTime(),pst.getArrCloseTime() );

		if( pst.getStyle() ==0 )
		{
			m_listAll.SetItemText( idx,1,"NONE" );
			m_listAll.SetItemText( idx,2,"NONE" );
		}
		else if( pst.getStyle() == 1)
		{
			m_listAll.SetItemText( idx,1,strDep );
			m_listAll.SetItemText( idx,2,"NONE" );
		}
		else if( pst.getStyle() == 2 )
		{
			m_listAll.SetItemText( idx,1,"NONE" );
			m_listAll.SetItemText( idx,2,strArr );
		}
		else
		{
			m_listAll.SetItemText( idx,1,strDep );
			m_listAll.SetItemText( idx,2,strArr );
		}
	//	m_listAll.SetItemText(idx,3,)
		m_listAll.SetItemData(idx,i);
	}
	return;
}

void CAssignFlightDlg::OnSave() 
{
	// TODO: Add your control notification handler code here
	int i,j;
	for( i=0; i< static_cast<int>(m_switchlist.size()); i++ )
	{
		for( j=0; j<p_InTerm->m_pProSwitchTimeDB->getCount(); j++ )
		{
			ProSwitchTime pst = p_InTerm->m_pProSwitchTimeDB->getItem( j );
			if( m_switchlist[i].isEqual( pst ) )
				break;
		}
		if( j >= p_InTerm->m_pProSwitchTimeDB->getCount() )
			p_InTerm->m_pProSwitchTimeDB->addItem( m_switchlist[i] );
	}

	p_InTerm->m_pProSwitchTimeDB->saveDataSet( GetProjectPath(), FALSE );
	return;
}

void CAssignFlightDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
}


// drag and drop
void CAssignFlightDlg::OnBegindragListAll(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_nDragIndex = pNMListView->iItem;
	if(m_pDragImage)
		delete m_pDragImage;

	POINT pt;
	pt.x = 8;
	pt.y = 8;
	m_pDragImage = m_listAll.CreateDragImage (m_nDragIndex, &pt);
	ASSERT (m_pDragImage);
	// changes the cursor to the drag image (DragMove() is still required in 
	// OnMouseMove())
	VERIFY (m_pDragImage->BeginDrag (0, CPoint (8, 8)));
	VERIFY (m_pDragImage->DragEnter (GetDesktopWindow (), pNMListView->ptAction));
	
	m_bDragging = true;
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	SetCapture ();
	*pResult = 0;
}

void CAssignFlightDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_bDragging)
	{
		CPoint pt (point);
		ClientToScreen (&pt);

		// move the drag image
		VERIFY (m_pDragImage->DragMove (pt));
		// unlock window updates
		m_pDragImage->DragShowNolock (FALSE);

		BOOL bSuccess = m_gtCtrl.IsUnderCursor();
		if( bSuccess )
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		}
		else
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		}

		m_pDragImage->DragShowNolock (TRUE);
	}
	
	CDialog::OnMouseMove(nFlags, point);
}

void CAssignFlightDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_bDragging)
	{
		CPoint pt (point);
		ClientToScreen (&pt);
		
		if( m_gtCtrl.IsUnderCursor() )
		{
			AddItemFromLib();
		}

		m_bDragging = false;
		ReleaseCapture();
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		// end dragging
		m_pDragImage->DragLeave (GetDesktopWindow ());
		m_pDragImage->EndDrag ();
	}
	
	CDialog::OnLButtonUp(nFlags, point);
}

bool CAssignFlightDlg::AddItemFromLib()
{
	if( IsExistInGT() )
	{
		CString strPaxType = m_listAll.GetItemText( m_nDragIndex,0 );
		CString strMsg;
		strMsg.Format("The %s already exist in GT chart!\r\nyou can not add the pax type to GT chart!",strPaxType);
		MessageBox( strMsg, NULL, MB_OK|MB_ICONINFORMATION );
		return false;
	}
	else
	{
		int iItemData = m_listAll.GetItemData( m_nDragIndex );
		ProSwitchTime pst = p_InTerm->m_pProSwitchTimeDB->getItem( iItemData);
		m_switchlist.push_back( pst );

		AddLineToGT( pst,m_switchlist.size()-1 );
		return true;
	}
	m_gtCtrl.ClearItemSeled( false );
}


void CAssignFlightDlg::AddLineToGT(ProSwitchTime &pst, int lineData )
{
	CString szType;
	pst.getMobType()->screenPrint(szType,0,256);
	long lLineIdx = m_gtCtrl.AddLine( szType.GetBuffer(0),NULL );
	m_gtCtrl.SetLineData( lLineIdx, lineData );

	long lItemIdx;
	if( pst.getStyle()==1 || pst.getStyle()==3)
	{
		m_gtCtrl.AddItem( &lItemIdx, lLineIdx,"DEPATURE",NULL,
						  pst.getDepOpenTime(),pst.getDepCloseTime()-pst.getDepOpenTime(),true);
		m_gtCtrl.SetItemData( lItemIdx,DEPITEM);
	}
	if( pst.getStyle()==2 || pst.getStyle()==3)
	{
		m_gtCtrl.AddItem( &lItemIdx, lLineIdx,"ARRIVAL",NULL,
						  pst.getArrOpenTime(),pst.getArrCloseTime()-pst.getArrOpenTime(),true);
		m_gtCtrl.SetItemData( lItemIdx,ARRITEM);
	}
	m_butApply.EnableWindow(true);
}


void CAssignFlightDlg::UpdateGTLine(long _lLineIdx)
{
	long lLineData;
	m_gtCtrl.GetLineData( _lLineIdx, &lLineData );
	
	m_gtCtrl.ClearAllItemOfLine( _lLineIdx );
	ProSwitchTime& pst = m_switchlist[lLineData];
	long lItemIdx;
	if( pst.getStyle()==1 || pst.getStyle()==3)
	{
		m_gtCtrl.AddItem( &lItemIdx, _lLineIdx,"DEPATURE",NULL,
						  pst.getDepOpenTime(),pst.getDepCloseTime()-pst.getDepOpenTime(),true);
		m_gtCtrl.SetItemData( lItemIdx,DEPITEM);
	}
	if( pst.getStyle()==2 || pst.getStyle()==3)
	{
		m_gtCtrl.AddItem( &lItemIdx, _lLineIdx,"ARRIVAL",NULL,
						  pst.getArrOpenTime(),pst.getArrCloseTime()-pst.getArrOpenTime(),true);
		m_gtCtrl.SetItemData( lItemIdx,ARRITEM);
	}	
	m_gtCtrl.ClearItemSeled( false );
	m_butApply.EnableWindow(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////




bool CAssignFlightDlg::IsExistInGT()
{
	int iItemData = m_listAll.GetItemData( m_nDragIndex );
	ProSwitchTime pst = p_InTerm->m_pProSwitchTimeDB->getItem( iItemData );
	return IsExistInGT( pst );
}


bool CAssignFlightDlg::IsExistInGT( ProSwitchTime& pst)
{
	for( int i=0; i< static_cast<int>(m_switchlist.size()); i++)
	{
		if( pst == m_switchlist[i] )
		{
			return true;
		}
	}
	return false;
}

void CAssignFlightDlg::RebuildGt()
{
	m_gtCtrl.Clear( false );
	for(int i=0; i<static_cast<int>(m_switchlist.size()); i++ )
	{
		ProSwitchTime pst = m_switchlist[i];
		AddLineToGT(pst, i);
	}
	m_gtCtrl.ClearItemSeled( false );
}

BEGIN_EVENTSINK_MAP(CAssignFlightDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CAssignFlightDlg)
	ON_EVENT(CAssignFlightDlg, IDC_GTCHARTCTRL, 12 /* ClickBtnAddLine */, OnClickBtnAddLineGt, VTS_NONE)
	ON_EVENT(CAssignFlightDlg, IDC_GTCHARTCTRL, 13 /* ClickBtnDelLine */, OnClickBtnDelLineGt, VTS_BOOL VTS_I4 VTS_BSTR VTS_BSTR VTS_I4)
	ON_EVENT(CAssignFlightDlg, IDC_GTCHARTCTRL, 10 /* ItemMoved */, OnItemMovedGtchartctrl, VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_I4)
	ON_EVENT(CAssignFlightDlg, IDC_GTCHARTCTRL, 9 /* ItemResized */, OnItemResizedGtchartctrl, VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_I4)
	ON_EVENT(CAssignFlightDlg, IDC_GTCHARTCTRL, 7 /* DblClickItem */, OnDblClickItemGtchartctrl, VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CAssignFlightDlg, IDC_GTCHARTCTRL, 6 /* ClickBtnDelItem */, OnClickBtnDelItemGtchartctrl, VTS_BOOL VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CAssignFlightDlg, IDC_GTCHARTCTRL, 4 /* ClickBtnAddItem */, OnClickBtnAddItemGtchartctrl, VTS_BOOL VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CAssignFlightDlg::OnClickBtnAddLineGt() 
{
	// TODO: Add your control notification handler code here
	CString szType;
	ProSwitchTime pst;

	CPassengerTypeDialog dlg( m_pParentWnd );
	if( dlg.DoModal() == IDCANCEL)
		return;
	CMobileElemConstraint mMobileElem = dlg.GetMobileSelection();
	pst.setMobType( mMobileElem );
	pst.SetDefaultValue();
	if( IsExistInGT(pst) )
	{
		CString strMsg;
		pst.getMobType()->screenPrint( szType, 0, 256 );
		strMsg.Format("The %s already exist in GT chart!\r\nyou can not add the pax type to GT chart!",szType.GetBuffer(0));
		MessageBox( strMsg, NULL, MB_OK|MB_ICONINFORMATION );
		return ;		
	}	

	m_switchlist.push_back( pst );
	AddLineToGT( pst,m_switchlist.size()-1 );
}

void CAssignFlightDlg::OnClickBtnDelLineGt(BOOL bSuccess, long nLineIndex, LPCTSTR strCaption, LPCTSTR strAddition, long nLineData) 
{
	// TODO: Add your control notification handler code here
	if( !bSuccess )
	{
		MessageBox(" If you want to delete a line, you must highlighted the line first!",NULL,MB_OK|MB_ICONINFORMATION );
		return;
	}
	m_switchlist.erase( m_switchlist.begin() + nLineData );

	RebuildGt();	
	m_butApply.EnableWindow(true);
}


void CAssignFlightDlg::OnItemMovedGtchartctrl(long nLineIndexFrom, long nLineIndexTo, long nItemIndex, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration) 
{
	// TODO: Add your control notification handler code here
	long lLineData;
	long lItemData;
	m_gtCtrl.GetLineData( nLineIndexFrom, &lLineData );
	m_gtCtrl.GetItemData( nItemIndex, &lItemData );

	if( lItemData == DEPITEM )	// depature
	{
		m_switchlist[lLineData].setDepOpenTime(timeBegin);
		m_switchlist[lLineData].setDepCloseTime(timeBegin + timeDuration );
	}
	else if( lItemData == ARRITEM )	// arrival
	{
		m_switchlist[lLineData].setArrOpenTime(timeBegin);
		m_switchlist[lLineData].setArrCloseTime(timeBegin + timeDuration );
	}
	m_butApply.EnableWindow(true);
}

void CAssignFlightDlg::OnItemResizedGtchartctrl(long nIndexLine, long nIndexItem, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration) 
{
	// TODO: Add your control notification handler code here
	long lLineData;
	long lItemData;
	m_gtCtrl.GetLineData( nIndexLine, &lLineData );
	m_gtCtrl.GetItemData( nIndexItem, &lItemData );

	if( lItemData == DEPITEM )	// depature
	{
		m_switchlist[lLineData].setDepOpenTime(timeBegin);
		m_switchlist[lLineData].setDepCloseTime(timeBegin + timeDuration );
	}
	else if( lItemData == ARRITEM )	// arrival
	{
		m_switchlist[lLineData].setArrOpenTime(timeBegin);
		m_switchlist[lLineData].setArrCloseTime(timeBegin + timeDuration );
	}	
	m_butApply.EnableWindow(true);
}

void CAssignFlightDlg::OnDblClickItemGtchartctrl(long nIndexLine, long nIndexItem, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration, long itemData) 
{
	// TODO: Add your control notification handler code here
	long lLineData;
	m_gtCtrl.GetLineData( nIndexLine,&lLineData );

	if( m_switchlist[lLineData].getStyle() == 1 || m_switchlist[lLineData].getStyle() == 2)
		m_switchlist[lLineData].setStyle( 0 );

	else if( m_switchlist[lLineData].getStyle() == 3) 
	{
		if( itemData == DEPITEM )
			m_switchlist[lLineData].setStyle(2);
		else if( itemData == ARRITEM )
			m_switchlist[lLineData].setStyle(1);
		else 
			m_switchlist[lLineData].setStyle(0);
	}
	else
		m_switchlist[lLineData].setStyle(0);

	m_butApply.EnableWindow(true);
}

void CAssignFlightDlg::OnClickBtnDelItemGtchartctrl(BOOL bSuccess, long nIndexLine, long nIndexItem, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration, long itemData) 
{
	// TODO: Add your control notification handler code here
	if( !bSuccess )
	{
		MessageBox(" If you want to delete a item, you must highlighted the item first!",NULL,MB_OK|MB_ICONINFORMATION );
		return;
	}
	OnDblClickItemGtchartctrl( nIndexLine, nIndexItem,strCaption,strAddition,timeBegin,timeDuration,itemData);
}

void CAssignFlightDlg::OnClickBtnAddItemGtchartctrl(BOOL bSuccess, long nLineIndex) 
{
	// TODO: Add your control notification handler code here
	if( !bSuccess )
	{
		MessageBox(" If you want to add a item to a line, you must highlighted the line first!",NULL,MB_OK|MB_ICONINFORMATION );
		return;
	}

	long lLineData;
	//char szType[256];
	CString szType;
	m_gtCtrl.GetLineData( nLineIndex, &lLineData );
	ProSwitchTime& pst = m_switchlist[lLineData];
	int iStyle = pst.getStyle();

	if( iStyle == 0)
	{
		int iMB = MessageBox(" Now, you can add ARRIVAL item and DEPATURE item.\r\n If you want to add ARRIVAL item,please click YES.\r\n If you want to add DEPATURE item, please click NO.\r\n If you don't want to add any item ,please click CANCEL.",NULL,
							  MB_YESNOCANCEL |MB_ICONINFORMATION);
		switch( iMB )
		{
		case IDYES:	// arrival
			pst.SetArrDefValue();
			pst.setStyle(2);
			break;
		case IDNO:	// DEP
			pst.SetDepDefValue();
			pst.setStyle(1);
			break;
		case IDCANCEL: //
			return;
		}
	}
	else if( iStyle == 1)	// already exist dep, need add arr
	{
		pst.SetArrDefValue();
		pst.setStyle(3);
	}
	else if( iStyle == 2)	// already exist arr, need add dep
	{
		pst.SetDepDefValue();
		pst.setStyle(3);
	}
	else if( iStyle == 3)	// arr and dep all exists ,can not add a item
	{
		pst.getMobType()->screenPrint(szType,0,256);
		CString strMsg;
		strMsg.Format("%s: \r\n ARRIVAL item and DEPATURE item all already exist,you can not add aitem to the line.",szType.GetBuffer(0) );
		MessageBox(strMsg);
	}

	//
	UpdateGTLine( nLineIndex );
}
