// DlgIntersectionSelect.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgIntersectionSelect.h"
#include "Landside/InputLandside.h"
#include ".\dlgintersectionselect.h"

// CDlgIntersectionSelect dialog

IMPLEMENT_DYNAMIC(CDlgIntersectionSelect, CXTResizeDialog)
CDlgIntersectionSelect::CDlgIntersectionSelect(CTermPlanDoc *tmpDoc,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgIntersectionSelect::IDD, pParent)
	, m_pDoc(tmpDoc)
{
	m_strIntersection=_T("");
}

CDlgIntersectionSelect::~CDlgIntersectionSelect()
{
}

void CDlgIntersectionSelect::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_INTERSECTION, m_treeIntersection);
	DDX_Control(pDX, IDOK, m_btnOK);
}
BOOL CDlgIntersectionSelect::OnInitDialog()
{
	if(!CXTResizeDialog::OnInitDialog())
		return FALSE;
	SetTreeContent();
	m_btnOK.EnableWindow(FALSE);
	return TRUE;
}

bool CDlgIntersectionSelect::AddObject( LandsideIntersectionNode * pObj )
{
	const ALTObjectID& objID = pObj->getName();	
	int idLength = objID.idLength();

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_FREE;
    HTREEITEM hInsertItem=TVI_ROOT;
// 	CString strName=_T("");
	for (int nLevel =0; nLevel< idLength; ++nLevel)
	{		
		CString strLevel = objID.m_val[nLevel].c_str();	
		
		strLevel.MakeUpper();
		if(strLevel.IsEmpty())
			break;

// 		if (nLevel==0)
// 		{
// 			strName=strLevel;
// 		}else
// 		{
// 			strName=strName+_T("-")+strLevel;
// 		}

		if (!StrExistUnderItem(hInsertItem,strLevel))
		{
			hInsertItem=m_treeIntersection.InsertItem(strLevel,cni,FALSE,FALSE,hInsertItem);
		}
	}
	return false;
}
bool CDlgIntersectionSelect::StrExistUnderItem(HTREEITEM &hItem, const CString& str/*,bool& bInsert*/ )
{
	//find if str exist under hItem.if exist,set hItem point to it,
	//and return true.Else return false.
	if (hItem==NULL)
	{
		hItem=TVI_ROOT;
	}
	HTREEITEM hChildItem=m_treeIntersection.GetChildItem(hItem);
	while(hChildItem!=NULL)
	{
		if (m_treeIntersection.GetItemText(hChildItem) == str)
		{
			hItem=hChildItem;
			return true;
		}
		hChildItem=m_treeIntersection.GetNextItem(hChildItem,TVGN_NEXT);
	}
	return false;
}
void CDlgIntersectionSelect::SetTreeContent()
{
	m_treeIntersection.DeleteAllItems();
	LandsideFacilityLayoutObjectList& objlist = m_pDoc->getARCport()->m_pInputLandside->getObjectList();
	SortedStringList levelName;
	for(int i=0;i<objlist.getCount();i++)
	{
		LandsideIntersectionNode* pObj = (LandsideIntersectionNode *)objlist.getObject(i);
		if(pObj->GetType() == ALT_LINTERSECTION)
		{
			AddObject(pObj);
		}
	}
}


BEGIN_MESSAGE_MAP(CDlgIntersectionSelect, CXTResizeDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_INTERSECTION, OnTvnSelchangedTreeIntersection)
END_MESSAGE_MAP()


// CDlgIntersectionSelect message handlers

void CDlgIntersectionSelect::OnTvnSelchangedTreeIntersection(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hSelItem=m_treeIntersection.GetSelectedItem();
	if (hSelItem == NULL)
	{
		m_btnOK.EnableWindow(FALSE);
		return;
	}	
	m_strIntersection=m_treeIntersection.GetItemText(hSelItem);
	HTREEITEM hParentItem=m_treeIntersection.GetParentItem(hSelItem);
	while(hParentItem!=NULL)
	{
		m_strIntersection=m_treeIntersection.GetItemText(hParentItem)+_T("-")+m_strIntersection;
		hParentItem=m_treeIntersection.GetParentItem(hParentItem);
	}
	
	m_btnOK.EnableWindow(TRUE);

	*pResult = 0;
}
