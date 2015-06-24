// DlgGateTypedefine.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "..\Engine\proclist.h"
#include "..\inputs\in_term.h"
#include "../InputAirside/ALTObject.h"
#include "TermPlanDoc.h"
#include "../Engine/gate.h"
#include "DlgGateTypedefine.h"
#include ".\dlggatetypedefine.h"


// CDlgGateTypedefine dialog
static LPCTSTR GateArray[] = {_T("ArrGate"),_T("DepGate")};

IMPLEMENT_DYNAMIC(CDlgGateTypedefine, CDialog)
CDlgGateTypedefine::CDlgGateTypedefine(StringDictionary* _pStrDict,ProcessorList* proList,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGateTypedefine::IDD, pParent)
	,m_pCurProList(proList)
	,m_pStrDict(_pStrDict)
{
}

CDlgGateTypedefine::~CDlgGateTypedefine()
{
}

void CDlgGateTypedefine::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GATE, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgGateTypedefine, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_GATE, OnLvnEndlabeleditListGate)
END_MESSAGE_MAP()


// CDlgGateTypedefine message handlers

BOOL CDlgGateTypedefine::OnInitDialog()
{
	CDialog::OnInitDialog();

//	InitGateType();
	OnInitListCtrl();
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgGateTypedefine::OnInitListCtrl()
{
	if(!m_pCurProList)return ;

	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	StringList strDict;
	m_pCurProList->getAllGroupNames (strDict, GateProc);
	
	CStringList strList;
	strList.RemoveAll();
	strList.AddTail(GateArray[0]);
	strList.AddTail(GateArray[1]);

	CString strCaption;
	LV_COLUMNEX lvc;

	CRect rc;
	m_wndListCtrl.GetClientRect(&rc);

	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	strCaption  = "Gate Name";
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption; 
	lvc.cx = rc.Width()/2;
	lvc.csList = &strList;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(0, &lvc);

	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	strCaption = "Gate Type";
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = rc.Width()/2;
	lvc.csList = &strList;
	lvc.fmt = LVCFMT_DROPDOWN;
	m_wndListCtrl.InsertColumn(1, &lvc);

	for (int i  = 0; i  < strDict.getCount(); i++)   
	{
		m_wndListCtrl.InsertItem(i,strDict.getString (i));
	//	m_wndListCtrl.SetItemText(i,1,strList.GetHead());
	}
	int nCount = m_wndListCtrl.GetHeaderCtrl()->GetItemCount();
}

void CDlgGateTypedefine::InitGateType()
{
	ProcessorID id;
	Processor* pid = NULL;
	id.SetStrDict( m_pStrDict );
	StringList strDict;
	m_pCurProList->getAllGroupNames (strDict, GateProc);
	for (int i = 0; i < strDict.getCount(); i++)
	{
		id.setID(strDict.getString(i));
		GroupIndex gIndex = m_pCurProList->getGroupIndexOriginal(id);
		for (int j = gIndex.start; j <= gIndex.end; j++)
		{
			pid = m_pCurProList->getProcessor(j);
			((GateProcessor*)pid)->setGateType(ArrGate);
		}
	}
}

void CDlgGateTypedefine::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CDlgGateTypedefine::OnLvnEndlabeleditListGate(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	CString strTemp = _T("");
	for (int nIndex = 0; nIndex < m_wndListCtrl.GetItemCount(); nIndex++)
	{
		strTemp = m_wndListCtrl.GetItemText(nIndex,1);
		for (int k = 0; k < 2; k++)
		{
			if (strTemp.CompareNoCase(GateArray[k]) == 0)
			{
				ProcessorID id;
				Processor* pid = NULL;
				id.SetStrDict( m_pStrDict);
				StringList strDict;
				m_pCurProList->getAllGroupNames (strDict, GateProc);
				
				id.setID(strDict.getString(nIndex));
				GroupIndex gIndex = m_pCurProList->getGroupIndexOriginal(id);
				for (int j = gIndex.start; j <= gIndex.end; j++)
				{
					pid = m_pCurProList->getProcessor(j);
					((GateProcessor*)pid)->setGateType((GateType)(k+1));
				}
			}
		}
	}
	*pResult = 0;
}

void CDlgGateTypedefine::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	CDialog::PostNcDestroy();
}
