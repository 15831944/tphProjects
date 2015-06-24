// DlgRecordOptions.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "HyperCam.h"
#include "DlgRecordOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRecordOptions dialog


CDlgRecordOptions::CDlgRecordOptions(IHyperCam* pHyperCam, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRecordOptions::IDD, pParent)
{
	m_pHyperCam = pHyperCam;

	//{{AFX_DATA_INIT(CDlgRecordOptions)
	m_nCompressorIdx = -1;
	m_nFpsPB = 0;
	m_nQuality = 0;
	m_nFpsRec = 0;
	m_bRecMouse = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgRecordOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRecordOptions)
	DDX_Control(pDX, IDC_COMBO_COMPRESSOR, m_cmbCompressor);
	DDX_Control(pDX, IDC_SPIN_QUALITY, m_spinQuality);
	DDX_CBIndex(pDX, IDC_COMBO_COMPRESSOR, m_nCompressorIdx);
	DDX_Text(pDX, IDC_EDIT_PLAYBACKRATE, m_nFpsPB);
	DDV_MinMaxInt(pDX, m_nFpsPB, 1, 100);
	DDX_Text(pDX, IDC_EDIT_QUALITY, m_nQuality);
	DDV_MinMaxInt(pDX, m_nQuality, 0, 100);
	DDX_Text(pDX, IDC_EDIT_RECORDRATE, m_nFpsRec);
	DDV_MinMaxInt(pDX, m_nFpsRec, 1, 100);
	DDX_Check(pDX, IDC_CHECK_RECMOUSE, m_bRecMouse);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRecordOptions, CDialog)
	//{{AFX_MSG_MAP(CDlgRecordOptions)
	ON_BN_CLICKED(IDC_BUTTON_COMPRESSORCONFIG, OnButtonCompressorConfig)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_QUALITY, OnDeltaposSpinQuality)
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_EDIT_PLAYBACKRATE, OnKillfocusEditPlaybackrate)
	ON_EN_KILLFOCUS(IDC_EDIT_RECORDRATE, OnKillfocusEditRecordrate)
	ON_EN_KILLFOCUS(IDC_EDIT_QUALITY, OnKillfocusEditQuality)
	ON_BN_CLICKED(IDC_CHECK_RECMOUSE, OnCheckRecmouse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRecordOptions message handlers

void CDlgRecordOptions::OnButtonCompressorConfig() 
{
	UpdateData(TRUE);
	char* p = NULL;
	int idx = m_cmbCompressor.GetCurSel();
	p = (char*) m_cmbCompressor.GetItemData(idx);
	//AfxMessageBox(p);
	if(m_pHyperCam) {
		m_pHyperCam->SetCompressor(p);
		m_pHyperCam->ConfigureCompressor((long) this->GetSafeHwnd());
	}
	
}

void CDlgRecordOptions::OnDeltaposSpinQuality(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMUPDOWN* pNMUpDown = (NMUPDOWN*)pNMHDR;
	
	int iFinal = m_nQuality - pNMUpDown->iDelta;
	if(iFinal < 0 || iFinal > 100)
		*pResult = 1;
	else {
		m_nQuality = iFinal;
		*pResult = 0;
	}
	UpdateData(FALSE);
}

BOOL CDlgRecordOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_nQuality = m_pHyperCam->GetCompQuality();
	m_spinQuality.SetPos(m_pHyperCam->GetCompQuality());

	m_nFpsRec = (int) m_pHyperCam->GetFrameRate();
	m_nFpsPB = (int) m_pHyperCam->GetPlaybackRate();

	m_bRecMouse = m_pHyperCam->GetRecordCursor();

	CString sCurrentCompressor = m_pHyperCam->GetCompressor();
	int nCurrentCIdx = -1;

	CString sCompName;
	int nCompIdx = 0;
	for(sCompName = m_pHyperCam->ListCompressor(nCompIdx++);
		!sCompName.IsEmpty();
		sCompName = m_pHyperCam->ListCompressor(nCompIdx++)) {
			m_cmbCompressor.AddString(sCompName.Right(sCompName.GetLength()-5));
			char* p = new char[5];
			strcpy(p,sCompName.Left(4));
			m_cmbCompressor.SetItemData(nCompIdx-1,(DWORD) p);
			if(sCurrentCompressor.Compare(p) == 0)
				nCurrentCIdx = nCompIdx-1;
	}
	UpdateData(FALSE);
	if(nCurrentCIdx != -1)
		m_cmbCompressor.SetCurSel(nCurrentCIdx);
	else
		m_cmbCompressor.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRecordOptions::OnDestroy() 
{	
	for(int i=0; i<m_cmbCompressor.GetCount(); i++) {
		char* p = (char*) m_cmbCompressor.GetItemData(i);
		delete [] p;
	}

	CDialog::OnDestroy();
}

void CDlgRecordOptions::OnKillfocusEditPlaybackrate() 
{
	UpdateData(TRUE);
	if(m_pHyperCam)
		m_pHyperCam->SetPlaybackRate((double)m_nFpsPB);
	
}

void CDlgRecordOptions::OnKillfocusEditRecordrate() 
{
	UpdateData(TRUE);
	if(m_pHyperCam)
		m_pHyperCam->SetFrameRate((double)m_nFpsRec);
	
}

void CDlgRecordOptions::OnKillfocusEditQuality() 
{
	UpdateData(TRUE);
	if(m_pHyperCam)
		m_pHyperCam->SetCompQuality(m_nQuality);
	
}

void CDlgRecordOptions::OnCheckRecmouse() 
{
	UpdateData(TRUE);
	if(m_pHyperCam)
		m_pHyperCam->SetRecordCursor(m_bRecMouse);
	
}
