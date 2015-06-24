// MathProcessDefine.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "MathProcessDefine.h"
#include "DlgProbDist.h"
#include "../common/ProbDistManager.h"
#include "../inputs/probab.h"
#include "engine\terminal.h"
#include <Common/ProbabilityDistribution.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMathProcessDefine dialog


CMathProcessDefine::CMathProcessDefine(CWnd* pParent /*=NULL*/)
	: CDialog(CMathProcessDefine::IDD, pParent)
{
	m_pInTerm = NULL;
//	m_pProbDist = NULL;
	//{{AFX_DATA_INIT(CMathProcessDefine)
	m_strName = _T("");
	m_nProcCount = 0;
	m_nCapacity = 0;
	m_strProcDist = _T("");
	m_bCapacity = FALSE;
	//}}AFX_DATA_INIT
	m_pProcessVector = NULL;
}


void CMathProcessDefine::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMathProcessDefine)
	DDX_Control(pDX, IDC_CHECK_CAPACITY, m_chkCapacity);
	DDX_Control(pDX, IDC_PROC_DIST, m_cmbProcDist);
	DDX_Control(pDX, IDC_SPIN_CAPACITY, m_spnCapacity);
	DDX_Control(pDX, IDC_SPIN_PROCCOUNT, m_spnProcCount);
	DDX_Control(pDX, IDC_PROC_QUEUE_CONF, m_cmbQConfig);
	DDX_Control(pDX, IDC_PROC_QUEUE_DISC, m_cmbQDiscipline);
	DDX_Text(pDX, IDC_PROC_NAME, m_strName);
	DDX_Text(pDX, IDC_PROC_COUNT, m_nProcCount);
//	DDV_MinMaxInt(pDX, m_nProcCount, 1, 170);
	DDX_Text(pDX, IDC_PROC_CAPACITY, m_nCapacity);
	DDX_Text(pDX, IDC_PROC_DIST, m_strProcDist);
	DDX_Check(pDX, IDC_CHECK_CAPACITY, m_bCapacity);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMathProcessDefine, CDialog)
	//{{AFX_MSG_MAP(CMathProcessDefine)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PROCCOUNT, OnDeltaposSpinProcCount)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CAPACITY, OnDeltaposSpinCapacity)
	ON_BN_CLICKED(IDC_CHECK_CAPACITY, OnCheckCapacity)
	ON_CBN_SELCHANGE(IDC_PROC_DIST, OnSelchangeProcDist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMathProcessDefine message handlers

void CMathProcessDefine::InitComboBox()
{
	int i = 0;
	for (i = 0; i < sizeof(s_pszQDiscipline) / sizeof(s_pszQDiscipline[0]); i++)
	{
		m_cmbQDiscipline.AddString(s_pszQDiscipline[i]);
	}
	if (m_cmbQDiscipline.GetCount())
		m_cmbQDiscipline.SetCurSel(0);


	for (i = 0; i < sizeof(s_pszQConfig) / sizeof(s_pszQConfig[0]); i++)
	{
		m_cmbQConfig.AddString(s_pszQConfig[i]);
	}
	if (m_cmbQConfig.GetCount())
		m_cmbQConfig.SetCurSel(0);
}

BOOL CMathProcessDefine::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitComboBox();
	InitProbDistComboBox();
	
	m_spnProcCount.SetRange(1, 170);
	m_spnCapacity.SetRange(0, 1000);

	m_strName =	m_mathProcess.GetName();

	m_nProcCount = m_mathProcess.GetProcessCount();
	m_cmbProcDist.SelectString(0, m_mathProcess.GetProbDistName());
	m_cmbQDiscipline.SelectString(0, s_pszQDiscipline[m_mathProcess.GetQDiscipline()]);
	m_cmbQConfig.SelectString(0, s_pszQConfig[m_mathProcess.GetQConfig()]);

	m_strProcDist =	m_mathProcess.GetProbDistName();
	if (!m_strProcDist.IsEmpty())
		GetDlgItem(IDC_PROC_NAME)->EnableWindow(FALSE);
	
	UpdateData(FALSE);
	
	m_nCapacity	= m_mathProcess.GetCapacity();
	m_bCapacity = (m_nCapacity < 0) ? FALSE : TRUE;
	if(m_bCapacity == FALSE)
		SetDlgItemText(IDC_PROC_CAPACITY, "Unlimited");
	else
		SetDlgItemInt(IDC_PROC_CAPACITY, m_nCapacity);
	
	GetDlgItem(IDC_SPIN_CAPACITY)->EnableWindow(m_bCapacity);
	GetDlgItem(IDC_PROC_CAPACITY)->EnableWindow(m_bCapacity);
	m_chkCapacity.SetCheck((UINT)m_bCapacity);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMathProcessDefine::OnOK() 
{
	CString strText = "";
	GetDlgItemText(IDC_PROC_CAPACITY, strText);
	if(strText == "Unlimited")
		SetDlgItemText(IDC_PROC_CAPACITY, "-1");
	UpdateData();

	if (!CheckData())
		return;

	if (!CheckProcDistData())
		return;

	m_mathProcess.SetName(m_strName);
	m_mathProcess.SetProcessCount(m_nProcCount);
	m_mathProcess.SetQDiscipline((QueueDiscipline)m_cmbQDiscipline.GetCurSel());
	m_mathProcess.SetQConfig((QueueConfig)m_cmbQConfig.GetCurSel());
	if (!m_bCapacity)
	{
		m_nCapacity = -1;
	}
	m_mathProcess.SetCapacity(m_nCapacity);
	m_mathProcess.SetProbDistName(m_strProcDist);
	
	CDialog::OnOK();
}

BOOL CMathProcessDefine::CheckData()
{
	if (m_strName.IsEmpty())
	{
		AfxMessageBox(_T("Please fill the process name."));
		return FALSE;
	}

	if (m_mathProcess.GetName().IsEmpty())
	{
		for(int i = 0; i < static_cast<int>(m_pProcessVector->size()); i++)
		{
			if(m_strName == m_pProcessVector->at(i).GetName())
			{
				AfxMessageBox(_T("A process with the specified name already exists!"));
				return FALSE;
			}
		}
	}

	if (m_nProcCount < 1 || m_nProcCount > 170)
	{
		AfxMessageBox(_T("Processor count should be between 1 to 170"));
		return FALSE;
	}

	int nDistIndex = m_cmbProcDist.GetCurSel();
	if (nDistIndex == 0 || nDistIndex == LB_ERR)
	{
		AfxMessageBox(_T("You must select a service time distribution"));
		return FALSE;
	}

	return TRUE;
}


void CMathProcessDefine::OnDeltaposSpinProcCount(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	m_nProcCount = GetDlgItemInt(IDC_PROC_COUNT);
	m_nProcCount += pNMUpDown->iDelta;
	SetDlgItemInt(IDC_PROC_COUNT, m_nProcCount);

	*pResult = 0;
}

void CMathProcessDefine::OnDeltaposSpinCapacity(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	m_nCapacity = GetDlgItemInt(IDC_PROC_CAPACITY);
	m_nCapacity += pNMUpDown->iDelta;
	SetDlgItemInt(IDC_PROC_CAPACITY, m_nCapacity);

	*pResult = 0;
}

void CMathProcessDefine::OnSelchangeProcDist() 
{
	int nSel = m_cmbProcDist.GetCurSel();
	if (nSel == CB_ERR)
		return;

	if (nSel  == 0)
	{
		CDlgProbDist dlg(m_pInTerm->m_pAirportDB, true);
		if (dlg.DoModal() == IDOK)
		{
			int idxPD = dlg.GetSelectedPDIdx();
			ASSERT(idxPD!=-1);
			CProbDistEntry* pde = _g_GetActiveProbMan( m_pInTerm )->getItem(idxPD);
			m_strProcDist = pde->m_csName;
			InitProbDistComboBox();
			m_cmbProcDist.SetCurSel(idxPD + 1);
		}
		else
		{
			m_cmbProcDist.SelectString(0, m_strProcDist);
		}
	}
	else
	{
		m_cmbProcDist.GetLBText(nSel, m_strProcDist);
	}

	if (!CheckProcDistData())
		m_strProcDist = _T("");
}

void CMathProcessDefine::InitProbDistComboBox()
{
	m_cmbProcDist.ResetContent();

	CStringList strList;
	CString s;
	s.LoadString( IDS_STRING_NEWDIST );
	m_cmbProcDist.AddString(s);
	
	int nCount = _g_GetActiveProbMan( m_pInTerm )->getCount();
	for( int m=0; m<nCount; m++ )
	{
		CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInTerm )->getItem( m );			
		m_cmbProcDist.AddString(pPBEntry->m_csName);
	}
}

void CMathProcessDefine::SetMathProcess(const CMathProcess &proc)
{
	m_mathProcess = proc;
}

void CMathProcessDefine::OnCheckCapacity() 
{
	if( (m_bCapacity = !m_bCapacity) == FALSE )
		SetDlgItemText(IDC_PROC_CAPACITY, "Unlimited");
	else
	{
		if(m_nCapacity < 0)
			m_nCapacity = 0;
		SetDlgItemInt(IDC_PROC_CAPACITY, m_nCapacity);
	}

	m_chkCapacity.SetCheck((int)m_bCapacity);
	GetDlgItem(IDC_SPIN_CAPACITY)->EnableWindow(m_bCapacity);
	GetDlgItem(IDC_PROC_CAPACITY)->EnableWindow(m_bCapacity);
}

bool CMathProcessDefine::CheckProcDistData()
{
	ProbabilityDistribution* pProbDist = NULL;
	int nCount = _g_GetActiveProbMan(m_pInTerm)->getCount();
	for (int i = 0; i < nCount; i++)
	{
		CProbDistEntry* pde = _g_GetActiveProbMan(m_pInTerm)->getItem(i);
		if (pde->m_csName.CompareNoCase(m_strProcDist) == 0)
			pProbDist = pde->m_pProbDist;
	}
	if (pProbDist == NULL)
		return false;
	
	ProbTypes probType = (ProbTypes)(pProbDist->getProbabilityType());
	if (probType != EXPONENTIAL && probType != NORMAL && probType != HISTOGRAM && probType != UNIFORM)
	{
		AfxMessageBox(_T("The service time distribution does not support at present!"));
		m_cmbProcDist.SelectString(0, m_strProcDist);
		return false;
	}
	return true;
}
