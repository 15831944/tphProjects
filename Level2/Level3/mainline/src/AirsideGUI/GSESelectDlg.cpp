// GSESelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "GSESelectDlg.h"
#include ".\gseselectdlg.h"

#include "InputAirside/VehicleSpecificationItem.h"

// CGSESelectDlg dialog

IMPLEMENT_DYNAMIC(CGSESelectDlg, CDialog)
CGSESelectDlg::CGSESelectDlg(int nProjID, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_SELECT_GSE, pParent)
	, m_nSelGSEID(-1)
{
	m_vehicleSpecifications.ReadData(nProjID);
}

CGSESelectDlg::~CGSESelectDlg()
{
}

void CGSESelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GSE, m_listGSE);
}


BEGIN_MESSAGE_MAP(CGSESelectDlg, CDialog)
	ON_LBN_DBLCLK(IDC_LIST_GSE, OnLbnDblclkListGSE)
END_MESSAGE_MAP()


// CGSESelectDlg message handlers

BOOL CGSESelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	size_t nVehicleCount = m_vehicleSpecifications.GetElementCount();

	CString strValue;
	for (size_t i = 0; i < nVehicleCount; i++)
	{
		CVehicleSpecificationItem* pItem = m_vehicleSpecifications.GetItem(i);
		int nIndex = m_listGSE.AddString(pItem->getName());
		m_listGSE.SetItemData(nIndex, pItem->GetID());
	}
	m_listGSE.SetCurSel(0);

	//------------------------------------------------------------
	// 
	CPoint pt;
	::GetCursorPos(&pt);
	CRect rc;
	GetWindowRect(rc);

	rc.MoveToXY(pt.x - rc.Width()/2, pt.y - rc.Height()/2);
	// 	ScreenToClient(rc);

	MoveWindow(rc);
	//------------------------------------------------------------

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGSESelectDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	int nSelIndex = m_listGSE.GetCurSel();
	if (LB_ERR == nSelIndex)
	{
		MessageBox(_T("Please select one vehicle!"));
		return;
	}

	m_nSelGSEID = m_listGSE.GetItemData(nSelIndex);
	CDialog::OnOK();
}

void CGSESelectDlg::OnLbnDblclkListGSE()
{
	OnOK();
}