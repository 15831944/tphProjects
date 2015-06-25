// DlgSelectExitOfRunway.cpp : implementation file
//

#include "stdafx.h"
#include "DlgSelectExitOfRunway.h"
#include ".\dlgselectexitofrunway.h"

#include "InputAirside/Runway.h"
#include "InputAirside/RunwayExit.h"
// CDlgSelectExitOfRunway dialog

IMPLEMENT_DYNAMIC(CDlgSelectExitOfRunway, CDialog)
CDlgSelectExitOfRunway::CDlgSelectExitOfRunway(int nRunwayID,
											   int nRunwayMarkIndex,
											   IExitOfRunwayFilter* pExitFilter/* = NULL*/,
											   CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectExitOfRunway::IDD, pParent)
	, m_pExitFilter(pExitFilter)
	, m_nRunwayID(nRunwayID)
	, m_nRunwayMarkIndex(nRunwayMarkIndex)
	, m_nSelExitID(-1)
{
}

CDlgSelectExitOfRunway::~CDlgSelectExitOfRunway()
{
}

void CDlgSelectExitOfRunway::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EXITS, m_listExits);
}


BEGIN_MESSAGE_MAP(CDlgSelectExitOfRunway, CDialog)
END_MESSAGE_MAP()


// CDlgSelectExitOfRunway message handlers
BOOL CDlgSelectExitOfRunway::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_listExits.ResetContent();

	ResetListBoxContent();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectExitOfRunway::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	int nCurSel = m_listExits.GetCurSel();
	if (LB_ERR == nCurSel)
	{
		MessageBox(_T("Please select one exit!"));
		return;
	}
	m_listExits.GetText(nCurSel,m_SelExitName) ;
	m_nSelExitID = m_listExits.GetItemData(nCurSel);
	CString strError;
	if (m_pExitFilter && FALSE == m_pExitFilter->IsAllowed(m_nSelExitID, strError))
	{
		MessageBox(strError);
		return;
	}
	
	CDialog::OnOK();
}

void CDlgSelectExitOfRunway::ResetListBoxContent()
{
	Runway altRunway;
	altRunway.ReadObject(m_nRunwayID);
	RunwayExitList exitlist;
	int n = altRunway.GetExitList(RUNWAY_MARK(m_nRunwayMarkIndex),exitlist);
	for(int i=0;i<n;i++)
	{
		m_listExits.AddString(exitlist[i].GetName());
		m_listExits.SetItemData(i, exitlist[i].GetID());
	}
}
