// DlgIntersectionNodeBlock.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgIntersectionNodeBlock.h"
#include ".\dlgintersectionnodeblock.h"
#include "../InputAirside/IntersectionNodeBlockList.h"

// CDlgIntersectionNodeBlock dialog

IMPLEMENT_DYNAMIC(CDlgIntersectionNodeBlock, CXTResizeDialog)
CDlgIntersectionNodeBlock::CDlgIntersectionNodeBlock(int nAirportID,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgIntersectionNodeBlock::IDD, pParent)
{
	m_pNodeOptionList = new IntersectionNodeBlockList;
	m_pNodeOptionList->InitNodeBlockData(nAirportID);
}

CDlgIntersectionNodeBlock::~CDlgIntersectionNodeBlock()
{
	if (m_pNodeOptionList)
	{
		delete m_pNodeOptionList;
		m_pNodeOptionList = NULL;
	}

}

void CDlgIntersectionNodeBlock::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_NODEBLOCK, m_lstNodeOption);
}


BEGIN_MESSAGE_MAP(CDlgIntersectionNodeBlock, CXTResizeDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_CREATE()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_NODEBLOCK, OnColumnclickListCtrl)
END_MESSAGE_MAP()


// CDlgIntersectionNodeBlock message handlers

void CDlgIntersectionNodeBlock::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_pNodeOptionList->SaveData();
	OnOK();
}

BOOL CDlgIntersectionNodeBlock::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	DWORD dwStyle = m_lstNodeOption.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_lstNodeOption.SetExtendedStyle( dwStyle );
	m_wndSortableHeaderCtrl.SubclassWindow(m_lstNodeOption.GetHeaderCtrl()->m_hWnd );

	// Create column
	m_lstNodeOption.InsertColumn( 0, "Node ID", LVCFMT_CENTER, 70 );
	m_wndSortableHeaderCtrl.SetDataType( 0, dtINT );

	m_lstNodeOption.InsertColumn( 1, "Node Name", LVCFMT_CENTER, 240 );
	m_wndSortableHeaderCtrl.SetDataType( 1, dtSTRING );

	m_lstNodeOption.InsertColumn( 2, "Block Disallowed", LVCFMT_CENTER, 80 );
	m_wndSortableHeaderCtrl.SetDataType( 2, dtINT );

	SetResize(IDC_LIST_NODEBLOCK, SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	InitNodeOption();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  
}

void CDlgIntersectionNodeBlock::InitNodeOption()
{
	int nCount = m_pNodeOptionList->GetIntersectionNodeBlockCount();
	for (int i = 0; i < nCount; i++)
	{
		IntersectionNodeBlock* pData = m_pNodeOptionList->GetNodeBlockByIdx(i);
		m_lstNodeOption.InsertNodeBlockItem(i,pData);
	}
}

int CDlgIntersectionNodeBlock::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rtEmpty;
	rtEmpty.SetRectEmpty();
	if( m_lstNodeOption.GetSafeHwnd())
	{
		m_wndSortableHeaderCtrl.SubclassWindow(m_lstNodeOption.GetHeaderCtrl()->m_hWnd );
	}

	return 0;
}

void CDlgIntersectionNodeBlock::OnColumnclickListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nTestIndex = pNMListView->iSubItem;
	if( nTestIndex >= 0 )
	{
		CWaitCursor	wCursor;
		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
		{
			m_wndSortableHeaderCtrl.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
		}
		else
		{
			m_wndSortableHeaderCtrl.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
		}
		m_wndSortableHeaderCtrl.SaveSortList();
	}	
	*pResult = 0;
}