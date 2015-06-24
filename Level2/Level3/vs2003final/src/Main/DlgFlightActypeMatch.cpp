// DlgFlightActypeMatch.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgFlightActypeMatch.h"
#include ".\dlgflightactypematch.h"
#include "../Common/Aircraft.h"
#include "../Common/ACTypesManager.h"
// CDlgFlightActypeMatch dialog
#include "../Inputs/flight.h"
//IMPLEMENT_DYNAMIC(CDlgFlightActypeMatch, CXTResizeDialog)
//CDlgFlightActypeMatch::CDlgFlightActypeMatch(std::map<CString,TY_DATA>* HandleCodeData,CACTypesManager* _ActypeManager , CWnd* pParent /*=NULL*/)
//	:m_HandleCode(HandleCodeData),m_ActypeManager(_ActypeManager), CXTResizeDialog(CDlgFlightActypeMatch::IDD, pParent)
//{
//}
//
//CDlgFlightActypeMatch::~CDlgFlightActypeMatch()
//{
//}
//
//void CDlgFlightActypeMatch::DoDataExchange(CDataExchange* pDX)
//{
//	CXTResizeDialog::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_TREE_ACTYPE, m_TreeCtrl);
//}
//
//
//BEGIN_MESSAGE_MAP(CDlgFlightActypeMatch, CXTResizeDialog)
//	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ACTYPE, OnTvnSelchangedTreeActype)
//	ON_WM_SIZE() 
//END_MESSAGE_MAP()
//
//void CDlgFlightActypeMatch::OnSize(UINT nType, int cx, int cy) 
//{
//	CXTResizeDialog::OnSize(nType,cx,cy) ;
//}
//BOOL CDlgFlightActypeMatch::OnInitDialog()
//{
//	CXTResizeDialog::OnInitDialog() ;
//	InitTreeCtrl() ;
//
//	SetResize(IDC_TREE_ACTYPE,SZ_TOP_LEFT,SZ_MIDDLE_RIGHT) ;
//	SetResize(IDC_STATIC_SPICAL,SZ_MIDDLE_LEFT,SZ_BOTTOM_RIGHT);
//	SetResize(IDC_STATIC_SPECIAL,SZ_MIDDLE_LEFT,SZ_MIDDLE_LEFT);
//	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
//	return TRUE ;
//}
//void CDlgFlightActypeMatch::InitTreeCtrl()
//{
//
//	std::map<CString,TY_DATA>::iterator iter = m_HandleCode->begin() ;
//	COOLTREE_NODE_INFO info ;
//	CCoolTree::InitNodeInfo(this,info) ;
//	for( ; iter != m_HandleCode->end() ;iter++)
//	{
//		HTREEITEM PItem = m_TreeCtrl.InsertItem((*iter).first,info,FALSE);
//		InsertTreeSubItem((*iter).first ,PItem) ;
//		m_TreeCtrl.Expand(PItem,TVE_EXPAND) ;
//	}
//}
//void CDlgFlightActypeMatch::InsertTreeSubItem(CString _code,HTREEITEM _paritem)
//{
//	std::vector<CACType*> m_Result ;
//	if(!m_ActypeManager->FindActypeByIATACodeOrICAOCode(_code,m_Result))
//		return ;
//	CString ItemName ;
//	CACType* PActypeItem = NULL ;
//
//	
//	COOLTREE_NODE_INFO info ;
//	CCoolTree::InitNodeInfo(this,info) ;
//	info.nt = NT_CHECKBOX ;
//	info.net = NET_NORMAL ;
//	for (int i = 0 ; i < (int)m_Result.size() ;i++)
//	{
//		PActypeItem = m_Result[i] ;
//
//		ItemName.Format(_T("%s (IATACode: %s,ICAOCode: %s)"),PActypeItem->GetIATACode(),PActypeItem->GetIATACode(),PActypeItem->GetICAOCode()) ;
//
//		HTREEITEM item = m_TreeCtrl.InsertItem(ItemName,info,FALSE,FALSE,_paritem) ;
//
//		m_TreeCtrl.SetItemData(item,(DWORD_PTR)PActypeItem) ;
//	}
//}
//BOOL CDlgFlightActypeMatch::GetSelectActype(CString& _node ,HTREEITEM _rootItem)
//{
//	if(_rootItem == NULL)
//		return FALSE;
//	HTREEITEM pChildItem = m_TreeCtrl.GetChildItem(_rootItem) ;
//	CACType* pActype = NULL ;
//	TCHAR Tcactype[1024] = {0} ;
//	while(pChildItem)
//	{
//		if(m_TreeCtrl.IsCheckItem(pChildItem) == 1)
//		{
//			pActype = (CACType*)m_TreeCtrl.GetItemData(pChildItem) ;
//			strcpy(Tcactype,pActype->GetIATACode()) ;
//			for (int i = 0 ; i < (int)(*m_HandleCode)[_node].size();i++ )
//			{
//				Flight* pFlight = (*m_HandleCode)[_node][i] ;
//					
//				pFlight->setACType(Tcactype) ;
//			}
//			return TRUE ;
//		}
//		pChildItem = m_TreeCtrl.GetNextSiblingItem(pChildItem) ;
//	}
//	return FALSE ;
//}
//void CDlgFlightActypeMatch::OnOK()
//{
//	HTREEITEM rooItem = m_TreeCtrl.GetRootItem() ;
//	CString codename ;
//	while(rooItem)
//	{
//		codename = m_TreeCtrl.GetItemText(rooItem) ;
//		if(!GetSelectActype(codename,rooItem))
//		{
//			CString Error ;
//			Error.Format(_T("%s has not been matched by Actype"),codename) ;
//			MessageBox(Error,_T("Warning"),MB_OK) ;
//			return ;
//		}
//		rooItem = m_TreeCtrl.GetNextSiblingItem(rooItem);
//	}
//	CXTResizeDialog::OnOK() ;
//}
//
//// CDlgFlightActypeMatch message handlers
//
//void CDlgFlightActypeMatch::OnTvnSelchangedTreeActype(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	HTREEITEM CurselItem = m_TreeCtrl.GetSelectedItem() ;
//	if(CurselItem == NULL)
//		return ;
//	if(!m_TreeCtrl.GetCheck(CurselItem))
//		return ;
//	HTREEITEM nexitem = m_TreeCtrl.GetNextSiblingItem(CurselItem) ;
//	while(nexitem)
//	{
//		if(m_TreeCtrl.GetCheck(nexitem))
//			m_TreeCtrl.SetCheck(nexitem,FALSE) ;
//		nexitem = m_TreeCtrl.GetNextSiblingItem(nexitem) ;
//	}
//	*pResult = 0;
//}
//LRESULT CDlgFlightActypeMatch::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
//{
//	switch(message)
//	{
//	case UM_CEW_STATUS_CHANGE:
//		{
//			HTREEITEM hItem=(HTREEITEM)wParam;
//			if(m_TreeCtrl.IsCheckItem(hItem) != 1)
//				break;
//			HTREEITEM nexitem = m_TreeCtrl.GetChildItem(m_TreeCtrl.GetParentItem(hItem)) ;
//			while(nexitem)
//			{
//				if(m_TreeCtrl.IsCheckItem(nexitem) == 1 && nexitem != hItem)
//					m_TreeCtrl.SetCheckStatus(nexitem,FALSE) ;
//				nexitem = m_TreeCtrl.GetNextSiblingItem(nexitem) ;
//			}
//		}
//	default:
//		break;
//	}
//	return CXTResizeDialog::DefWindowProc(message,wParam,lParam) ;
//}
