// DlgOccupiedAssignedStandDelaytime.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "Resource.h"
#include "DlgOccupiedAssignedStandDelaytime.h"
#include ".\dlgoccupiedassignedstanddelaytime.h"


// CDlgOccupiedAssignedStandDelaytime dialog

IMPLEMENT_DYNAMIC(CDlgOccupiedAssignedStandDelaytime, CDialog)
CDlgOccupiedAssignedStandDelaytime::CDlgOccupiedAssignedStandDelaytime(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOccupiedAssignedStandDelaytime::IDD, pParent)
	, m_nDelaytime(10)
{
}

CDlgOccupiedAssignedStandDelaytime::~CDlgOccupiedAssignedStandDelaytime()
{
}

void CDlgOccupiedAssignedStandDelaytime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DELAYTIME, m_nDelaytime);
}


BEGIN_MESSAGE_MAP(CDlgOccupiedAssignedStandDelaytime, CDialog)
	ON_EN_CHANGE(IDC_EDIT_DELAYTIME, OnEnChangeEditDelaytime)
END_MESSAGE_MAP()


// CDlgOccupiedAssignedStandDelaytime message handlers

BOOL CDlgOccupiedAssignedStandDelaytime::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOccupiedAssignedStandDelaytime::OnEnChangeEditDelaytime()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
}
