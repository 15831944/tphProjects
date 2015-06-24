// InputValueDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TermPlan.h"
#include "InputValueDlg.h"


// CInputValueDlg �Ի���

IMPLEMENT_DYNAMIC(CInputValueDlg, CDialog)
CInputValueDlg::CInputValueDlg(CString strPrompt,long nValue,long nMin,long nMax,BOOL bNotDragWnd,CString strTitle,CWnd* pParent /*=NULL*/)
	: CDialog(CInputValueDlg::IDD, pParent)
	, m_strTitle(_T(""))
	, m_strPrompt(_T(""))
	, m_nValue(0)
	, m_nMin(0)
	, m_nMax(0)
	, m_bNotDragWnd(bNotDragWnd)
{
	ASSERT(nValue>=nMin&&nValue<=nMax);
	if(!strTitle.IsEmpty())
		m_strTitle=strTitle;
	m_strPrompt=strPrompt;
	m_nValue=nValue;
	m_nMin=nMin;
	m_nMax=nMax;
}

CInputValueDlg::~CInputValueDlg()
{
}

void CInputValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_PROMPT, m_strPrompt);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_nValue);
	DDX_Control(pDX, IDC_EDIT_VALUE, m_editValue);
	DDX_Control(pDX, IDC_SPIN_VALUE, m_spinValue);
}


BEGIN_MESSAGE_MAP(CInputValueDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


// CInputValueDlg ��Ϣ�������

BOOL CInputValueDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_spinValue.SetRange32(m_nMin,m_nMax);
	CString strTitle=m_strTitle;
	if(m_bNotDragWnd)
		strTitle+=_T("(Don't Drag)");
	SetWindowText(strTitle);
	m_editValue.SetFocus();
	m_editValue.SetSel(0,-1);
	return FALSE;  // return TRUE unless you set the focus to a control
	// �쳣��OCX ����ҳӦ���� FALSE
}

void CInputValueDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	if(m_nValue<m_nMin||m_nValue>m_nMax)
	{
		CString strMsg;
		strMsg.Format(_T("Range of the value is from %d to %d ."),m_nMin,m_nMax);
		AfxMessageBox(strMsg);
		return;
	}
	OnOK();
}

UINT CInputValueDlg::OnNcHitTest(CPoint point)
{
	UINT unResult=CDialog::OnNcHitTest(point); 	
	if(m_bNotDragWnd&&unResult==HTCAPTION)
	{
		return HTCLIENT;
	}
	else
		return unResult;
}
