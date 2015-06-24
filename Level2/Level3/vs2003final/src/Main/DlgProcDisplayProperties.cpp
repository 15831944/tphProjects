// DlgProcDisplayProperties.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "Processor2.h"
#include "DlgProcDisplayProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define _NORMALPROC		0
#define _STATIONPROC	1
#define _ELEVATORPROC	2

static const UINT CHKIDS[] = { IDC_CHK_SHAPE, IDC_CHK_SERVLOC, IDC_CHK_QUEUE, IDC_CHK_INCONSTRAINT, IDC_CHK_OUTCONSTRAINT, IDC_CHK_PROCNAME };

static const CString PROCSTRINGS[3][6] = { 
	"Display Object Shape", "Display Service Location", "Display Queue", 
	"Display In Constraint", "Display Out Constraint", "Display Processor Name",

	"Display Car Area", "Display Bre-Boarding Holding Area", "Display Entry Doors",
	"Display In Constraint", "Display Exit Doors", "Display Processor Name",

	"Display Object Shape", "Display Lift Area", "Display Wait Area",
	"Display Doors", "Display Elevator Shaft", "Display Processor Name",
};


/////////////////////////////////////////////////////////////////////////////
// CDlgProcDisplayProperties dialog


CDlgProcDisplayProperties::CDlgProcDisplayProperties(CPROCESSOR2LIST* pProc2List, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcDisplayProperties::IDD, pParent)
{
	ASSERT(pProc2List != NULL);
	ASSERT(pProc2List->size() > 0);
	m_pProc2List = pProc2List;

	for(int i=PDP_MIN; i<=PDP_MAX; i++) {
		m_bMultiColor[i] = FALSE;
		m_nDisplay[i] = 0;
		m_bApplyColor[i] = FALSE;
		m_bApplyDisplay[i] = FALSE;
	}
	
	//{{AFX_DATA_INIT(CDlgProcDisplayProperties)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgProcDisplayProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProcDisplayProperties)
	DDX_Check(pDX, IDC_CHK_SHAPE, m_nDisplay[PDP_DISP_SHAPE]);
	DDX_Check(pDX, IDC_CHK_SERVLOC, m_nDisplay[PDP_DISP_SERVLOC]);
	DDX_Check(pDX, IDC_CHK_QUEUE, m_nDisplay[PDP_DISP_QUEUE]);
	DDX_Check(pDX, IDC_CHK_PROCNAME, m_nDisplay[PDP_DISP_PROCNAME]);
	DDX_Check(pDX, IDC_CHK_OUTCONSTRAINT, m_nDisplay[PDP_DISP_OUTC]);
	DDX_Check(pDX, IDC_CHK_INCONSTRAINT, m_nDisplay[PDP_DISP_INC]);
	DDX_Control(pDX, IDC_CHK_SHAPE, m_chkDisplay[PDP_DISP_SHAPE]);
	DDX_Control(pDX, IDC_CHK_SERVLOC, m_chkDisplay[PDP_DISP_SERVLOC]);
	DDX_Control(pDX, IDC_CHK_QUEUE, m_chkDisplay[PDP_DISP_QUEUE]);
	DDX_Control(pDX, IDC_CHK_OUTCONSTRAINT, m_chkDisplay[PDP_DISP_OUTC]);
	DDX_Control(pDX, IDC_CHK_PROCNAME, m_chkDisplay[PDP_DISP_PROCNAME]);
	DDX_Control(pDX, IDC_CHK_INCONSTRAINT, m_chkDisplay[PDP_DISP_INC]);
	
	DDX_Control(pDX, IDC_CHANGESHAPECOLOR, m_btnColor[PDP_DISP_SHAPE]);
	DDX_Control(pDX, IDC_CHANGESERVLOCCOLOR, m_btnColor[PDP_DISP_SERVLOC]);
	DDX_Control(pDX, IDC_CHANGEQUEUECOLOR, m_btnColor[PDP_DISP_QUEUE]);
	DDX_Control(pDX, IDC_CHANGEINCCOLOR, m_btnColor[PDP_DISP_INC]);
	DDX_Control(pDX, IDC_CHANGEOUTCCOLOR, m_btnColor[PDP_DISP_OUTC]);
	DDX_Control(pDX, IDC_CHANGENAMECOLOR, m_btnColor[PDP_DISP_PROCNAME]);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProcDisplayProperties, CDialog)
	//{{AFX_MSG_MAP(CDlgProcDisplayProperties)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHANGESHAPECOLOR, IDC_CHANGENAMECOLOR, OnChangeColor)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHK_SHAPE, IDC_CHK_PROCNAME, OnChkDisplay)
	//}}AFX_MSG_MAP
	ON_MESSAGE(CPN_SELENDOK,     OnSelEndOK)
	ON_MESSAGE(CPN_SELENDCANCEL, OnSelEndCancel)
	ON_MESSAGE(CPN_SELCHANGE,    OnSelChange)
	ON_MESSAGE(CPN_CLOSEUP,         OnCloseUp)
	ON_MESSAGE(CPN_DROPDOWN,     OnDropDown)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgProcDisplayProperties message handlers

BOOL CDlgProcDisplayProperties::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int i,j;
	int t;
	if(m_pProc2List->at(0)->GetProcessor() != NULL && m_pProc2List->at(0)->GetProcessor()->getProcessorType() == IntegratedStationProc) {
		t=_STATIONPROC;
	}
	else if(m_pProc2List->at(0)->GetProcessor() != NULL && m_pProc2List->at(0)->GetProcessor()->getProcessorType() == Elevator) {
		t=_ELEVATORPROC;
	}
	else {
		t=_NORMALPROC;
	}

	for(i=PDP_MIN; i<=PDP_MAX; i++) {
		this->GetDlgItem(CHKIDS[i])->SetWindowText(PROCSTRINGS[t][i]);
	}

	CProcessor2::CProcDispProperties dispProp;
	dispProp = m_pProc2List->at(0)->m_dispProperties;
	for(i=PDP_MIN; i<=PDP_MAX; i++) {
		m_nDisplay[i] = dispProp.bDisplay[i];
		m_cColor[i] = dispProp.color[i];
	}

	for(j=1; j<static_cast<int>(m_pProc2List->size()); j++) {
		dispProp = m_pProc2List->at(j)->m_dispProperties;
		for(int i=PDP_MIN; i<=PDP_MAX; i++) {
			if(m_nDisplay[i] != dispProp.bDisplay[i])
				m_nDisplay[i] = 2; //intermediate state
			if(m_cColor[i] != dispProp.color[i])
				m_bMultiColor[i] = TRUE; //multi colored
		}
	}

	for(i=PDP_MIN; i<=PDP_MAX; i++) {
		if(m_nDisplay[i] == 2)//intermediate state
			MakeChkTriState(CHKIDS[i], TRUE);
		m_btnColor[i].SetColor(m_cColor[i]);
	}

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgProcDisplayProperties::OnChangeColor(UINT nID) 
{
	// TODO: Add your control notification handler code here
	int idx = nID - IDC_CHANGESHAPECOLOR;
	CColorDialog dlg(m_cColor[idx], CC_ANYCOLOR | CC_FULLOPEN, this);
	if(dlg.DoModal() == IDOK)
	{
		m_cColor[idx] = dlg.GetColor();
		m_bApplyColor[idx] = TRUE;
	}
}

void CDlgProcDisplayProperties::OnChkDisplay(UINT nID)
{
	int idx = nID - IDC_CHK_SHAPE;
	UpdateData(TRUE);
	if(m_nDisplay[idx] != 2) {
		m_bApplyDisplay[idx] = TRUE;
	}
	else
		m_bApplyDisplay[idx] = FALSE;
}

BOOL CDlgProcDisplayProperties::MakeChkTriState(UINT nID, BOOL bTriState)
{
	BOOL bRet = FALSE;
	CButton* pChk = (CButton*)GetDlgItem(nID);
	if(pChk->GetCheck() == 2)
		bRet = TRUE;
	UINT nStyle = pChk->GetButtonStyle();
	if(bTriState)
		pChk->SetButtonStyle(BS_AUTO3STATE);
	else
		pChk->SetButtonStyle(BS_AUTOCHECKBOX);

	return bRet;
}

void CDlgProcDisplayProperties::OnOK()
{
	for(int i=PDP_MIN; i<=PDP_MAX; i++) {
		if(m_bApplyColor[i]) {
			for(int j=0; j<static_cast<int>(m_pProc2List->size()); j++) {
				m_pProc2List->at(j)->m_dispProperties.color[i] = m_cColor[i];
			}
		}
		if(m_bApplyDisplay[i]) {
			for(int j=0; j<static_cast<int>(m_pProc2List->size()); j++) {
				m_pProc2List->at(j)->m_dispProperties.bDisplay[i] = m_nDisplay[i];
			}
		}
	}

	CDialog::OnOK();
}

LONG CDlgProcDisplayProperties::OnSelEndOK(UINT lParam, LONG wParam)
{
   m_cColor[wParam-IDC_CHANGESHAPECOLOR] = lParam;
   m_bApplyColor[wParam-IDC_CHANGESHAPECOLOR] = TRUE;
    return TRUE;
}

LONG CDlgProcDisplayProperties::OnSelEndCancel(UINT /*lParam*/, LONG /*wParam*/)
{
    TRACE0("Selection cancelled\n");
    return TRUE;
}

LONG CDlgProcDisplayProperties::OnSelChange(UINT /*lParam*/, LONG /*wParam*/)
{
    TRACE0("Selection changed\n");
    return TRUE;
}

LONG CDlgProcDisplayProperties::OnCloseUp(UINT /*lParam*/, LONG /*wParam*/)
{
    TRACE0("Colour picker close up\n");
    return TRUE;
}

LONG CDlgProcDisplayProperties::OnDropDown(UINT /*lParam*/, LONG /*wParam*/)
{
    TRACE0("Colour picker drop down\n");
    return TRUE;
}
