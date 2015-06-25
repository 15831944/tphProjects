#include "stdafx.h"
#include "TermPlan.h"
#include "DlgGateAdjacency.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(DlgGateAdjacency, CDialogEx)

DlgGateAdjacency::DlgGateAdjacency(std::vector<CGateAdjacency>* pAdjacency, CWnd* pParent)
    : CXTResizeDialog(DlgGateAdjacency::IDD, pParent), m_pAdjacency(pAdjacency)
{
}

DlgGateAdjacency::~DlgGateAdjacency()
{
}

void DlgGateAdjacency::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgGateAdjacency, CDialogEx)
END_MESSAGE_MAP()

