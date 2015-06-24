// PollutantSourcePage.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "PollutantSourcePage.h"
#include ".\pollutantsourcepage.h"


// CPollutantSourcePage dialog

IMPLEMENT_DYNAMIC(CPollutantSourcePage, CResizablePage)

CPollutantSourcePage::CPollutantSourcePage(UINT nIDCaption,FuelProperties *pFuelProperties,CAirPollutantList *pAirPollutantList,CWnd* pParentWnd)
: CResizablePage(CPollutantSourcePage::IDD,nIDCaption)
,m_pFuelProperties(pFuelProperties)
,m_pAirPollutantList(pAirPollutantList)
,m_pParentWnd(pParentWnd)
{
}
/*
CPollutantSourcePage::CPollutantSourcePage(FuelProperties *pFuelProperties,CAirPollutantList *pAirPollutantList)
:CResizablePage(CPollutantSourcePage::IDD)
,m_pFuelProperties(pFuelProperties)
,m_pAirPollutantList(pAirPollutantList)
{

}
*/
CPollutantSourcePage::CPollutantSourcePage()
:CResizablePage(CPollutantSourcePage::IDD)
{
}

CPollutantSourcePage::~CPollutantSourcePage()
{
}

void CPollutantSourcePage::DoDataExchange(CDataExchange* pDX)
{
	CResizablePage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TREE_POLLUTANTSOURCE, m_tree);
}


BEGIN_MESSAGE_MAP(CPollutantSourcePage, CResizablePage)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_TOOLBARBUTTONADD,OnButtonAdd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL,OnButtonDel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_POLLUTANTSOURCE, OnTvnSelchangedTreePollutantsource)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// CPollutantSourcePage message handlers

int CPollutantSourcePage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CResizablePage::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_toolBar.CreateEx(this)||!m_toolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		MessageBox("Can't creat toolbar.");
		return -1;
	}
	return 0;
}

BOOL CPollutantSourcePage::OnInitDialog() 
{
	CResizablePage::OnInitDialog();
	// TODO: Add extra initialization here
	
	InitToolBar();
	ReloadTree();
	return true;
}
void CPollutantSourcePage::InitToolBar()
{
	CRect rc;
	m_tree.GetWindowRect(rc);
	ScreenToClient(rc);

	rc.bottom=rc.top+26;
	rc.OffsetRect(0,-26);

	m_toolBar.MoveWindow(rc);
	m_toolBar.ShowWindow(SW_SHOW);

	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,true);

}
// void CPollutantSourcePage::SetPollutantMap(PollutantIDSymbolMap &IDSymbolMap)
// {
// 	m_mapPollutantIDSymbol.clear();
// 	m_mapPollutantIDSymbol.insert(IDSymbolMap.begin(),IDSymbolMap.end());
// }
void CPollutantSourcePage::OnTvnSelchangedTreePollutantsource(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
void CPollutantSourcePage::OnSize(UINT nType, int cx, int cy) 
{	
	if (m_tree.GetSafeHwnd()==NULL)
	{
		return;
	}
	CRect rect;
	GetWindowRect(rect);
	ScreenToClient(rect);
	rect.left+=11;
	rect.top+=39;
	rect.right-=11;
	rect.bottom-=14;
	m_tree.MoveWindow(rect);

	CResizablePage::OnSize(nType, cx, cy);
	Invalidate();

}

void CPollutantSourcePage::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
}
