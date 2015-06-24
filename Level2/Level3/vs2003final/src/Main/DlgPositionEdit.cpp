// DlgPositionEdit.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgPositionEdit.h"
#include "../AirsideGUI/UnitPiece.h"



// CDlgPositionEdit dialog

IMPLEMENT_DYNAMIC(CDlgPositionEdit, CDialog)
CDlgPositionEdit::CDlgPositionEdit(ARCUnit_Length emType,const CPoint2008& pt,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPositionEdit::IDD, pParent)
	,m_pt(pt)
{
	m_emType = emType;
	m_dPtX = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,emType,m_pt.getX());
	m_dPtY = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,emType,m_pt.getY());
	m_dPtZ = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,emType,m_pt.getZ());
}

CDlgPositionEdit::~CDlgPositionEdit()
{
}

void CDlgPositionEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT_X,m_dPtX);
	DDX_Text(pDX,IDC_EDIT_Y,m_dPtY);
	DDX_Text(pDX,IDC_EDIT_Z,m_dPtZ);
}


BEGIN_MESSAGE_MAP(CDlgPositionEdit, CDialog)
END_MESSAGE_MAP()


// CDlgPositionEdit message handlers
BOOL CDlgPositionEdit::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strXpt(_T(""));
	strXpt.Format(_T("%.2f"),m_dPtX);

	CString strYpt(_T(""));
	strYpt.Format(_T("%.2f"),m_dPtY);

	CString strZpt(_T(""));
	strZpt.Format(_T("%.2f"),m_dPtZ);
	
	GetDlgItem(IDC_EDIT_X)->SetWindowText(strXpt);
	GetDlgItem(IDC_EDIT_Y)->SetWindowText(strYpt);
	GetDlgItem(IDC_EDIT_Z)->SetWindowText(strZpt);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

CPoint2008 CDlgPositionEdit::GetPoint()
{
	m_pt.setX(CARCLengthUnit::ConvertLength(m_emType,DEFAULT_DATABASE_LENGTH_UNIT,m_pt.getX()));
	m_pt.setY(CARCLengthUnit::ConvertLength(m_emType,DEFAULT_DATABASE_LENGTH_UNIT,m_pt.getY()));
	m_pt.setZ(CARCLengthUnit::ConvertLength(m_emType,DEFAULT_DATABASE_LENGTH_UNIT,m_pt.getZ()));
	return m_pt;
}

void CDlgPositionEdit::OnOK()
{
	UpdateData();
	m_pt.setX(m_dPtX);
	m_pt.setY(m_dPtY);
	m_pt.setZ(m_dPtZ);
	CDialog::OnOK();
}