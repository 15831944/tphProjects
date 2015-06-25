// DlgAirsideIntersectionSelect.cpp : implementation file
//

#include "stdafx.h"
/*#include "AirsideGUI.h"*/
#include "resource.h"
#include "DlgAirsideIntersectionSelect.h"
#include "InputAirside/IntersectionNode.h"
#include ".\dlgairsideintersectionselect.h"

// CDlgAirsideIntersectionSelect dialog

IMPLEMENT_DYNAMIC(CDlgAirsideIntersectionSelect, CXTResizeDialog)
CDlgAirsideIntersectionSelect::CDlgAirsideIntersectionSelect(int nAirPortID,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgAirsideIntersectionSelect::IDD, pParent)
	,m_nSelectIntersectionID(-1)
	,m_nAirPortID(nAirPortID)
{
}

CDlgAirsideIntersectionSelect::~CDlgAirsideIntersectionSelect()
{
}

void CDlgAirsideIntersectionSelect::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_listBoxIntersection);
}
BOOL CDlgAirsideIntersectionSelect::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	SetResize(IDC_LIST2,SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC,SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetListContent();
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgAirsideIntersectionSelect, CXTResizeDialog)
	ON_LBN_SELCHANGE(IDC_LIST2, OnLbnSelchangeList2)
END_MESSAGE_MAP()

void CDlgAirsideIntersectionSelect::SetListContent()
{
	std::vector<IntersectionNode> vIntersectionNodes;
	IntersectionNode::ReadNodeList(m_nAirPortID, vIntersectionNodes);
	for (int i=0;i<(int)vIntersectionNodes.size();i++)
	{
		int j=m_listBoxIntersection.AddString(vIntersectionNodes.at(i).GetName());
// 		m_listBoxIntersection.InsertString(i,vIntersectionNodes.at(i).GetName());
		m_listBoxIntersection.SetItemData(j,(DWORD)vIntersectionNodes.at(i).GetID());
	}

}

// CDlgAirsideIntersectionSelect message handlers

void CDlgAirsideIntersectionSelect::OnLbnSelchangeList2()
{
	// TODO: Add your control notification handler code here
	m_nSelectIntersectionID=(int)m_listBoxIntersection.GetItemData(m_listBoxIntersection.GetCurSel());
}
