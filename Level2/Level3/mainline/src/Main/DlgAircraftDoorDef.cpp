// DlgAircraftDoorDef.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgAircraftDoorDef.h"
#include "../Common/ACTypeDoor.h"

// CDlgAircraftDoorDef dialog

IMPLEMENT_DYNAMIC(CDlgAircraftDoorDef, CDialog)
CDlgAircraftDoorDef::CDlgAircraftDoorDef(ACTypeDoor* _pActypeDoor ,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAircraftDoorDef::IDD, pParent),m_pActypeDoor(_pActypeDoor)
{
	m_strDlgTitle.Empty();
}

CDlgAircraftDoorDef::~CDlgAircraftDoorDef()
{
}

void CDlgAircraftDoorDef::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_EDIT_NAME,m_DoorName) ;
	DDX_Control(pDX,IDC_EDIT_HEIGHT,m_DoorHeight) ;
	DDX_Control(pDX,IDC_EDIT_WIDTH,m_DoorWidth) ;
	DDX_Control(pDX,IDC_EDIT_SILL,m_DoorSill) ;
	DDX_Control(pDX,IDC_EDIT_CENTER,m_Center) ;
	DDX_Control(pDX,IDC_COMBO_HANDDOOR,m_HandDoor) ;
}


BEGIN_MESSAGE_MAP(CDlgAircraftDoorDef, CDialog)
END_MESSAGE_MAP()

BOOL CDlgAircraftDoorDef::OnInitDialog()
{
	CDialog::OnInitDialog() ;
	if(!m_strDlgTitle.IsEmpty())
	{
		SetWindowText(m_strDlgTitle);
	}
	InitCtrlData() ;
	return TRUE;
}

void CDlgAircraftDoorDef::InitCtrlData()
{
	if(m_pActypeDoor == NULL)
		return ;
	CString str;
	m_DoorName.SetWindowText(m_pActypeDoor->m_strName) ;

	str.Format(_T("%0.2f"),m_pActypeDoor->m_dHeight) ;
	m_DoorHeight.SetWindowText(str) ;

	str.Format(_T("%0.2f"),m_pActypeDoor->m_dWidth) ;
	m_DoorWidth.SetWindowText(str) ;

	str.Format(_T("%0.2f"),m_pActypeDoor->m_dSillHeight) ;
	m_DoorSill.SetWindowText(str) ;

	str.Format(_T("%0.2f"),m_pActypeDoor->m_dNoseDist) ;
	m_Center.SetWindowText(str) ;

	int ndx = m_HandDoor.AddString(ACTypeDoor::GetStringByHand(ACTypeDoor::LeftHand)) ;
	m_HandDoor.SetItemData(ndx ,ACTypeDoor::LeftHand) ;

	ndx = m_HandDoor.AddString(ACTypeDoor::GetStringByHand(ACTypeDoor::RightHand)) ;
	m_HandDoor.SetItemData(ndx ,ACTypeDoor::RightHand) ;

	ndx = m_HandDoor.AddString(ACTypeDoor::GetStringByHand(ACTypeDoor::BothSide)) ;
	m_HandDoor.SetItemData(ndx,ACTypeDoor::BothSide) ;

	for (int i = 0 ; i < m_HandDoor.GetCount() ;i++)
	{
		if(m_HandDoor.GetItemData(i) == m_pActypeDoor->m_enumDoorDir)
		{
			m_HandDoor.SetCurSel(i) ;
			return ;
		}
	}
	m_HandDoor.SetCurSel(0) ;
}

void CDlgAircraftDoorDef::GetDataFromCtrlData()
{
	  m_DoorName.GetWindowText( m_pActypeDoor->m_strName ) ;
	  CString val ;
	  m_DoorHeight.GetWindowText(val) ;
	  m_pActypeDoor->m_dHeight = atof(val) ;

	m_DoorWidth.GetWindowText(val) ;
	m_pActypeDoor->m_dWidth = atof(val) ;

	m_DoorSill.GetWindowText(val) ;
	m_pActypeDoor->m_dSillHeight = atof(val) ;

	m_Center.GetWindowText(val) ;
	m_pActypeDoor->m_dNoseDist = atof(val) ;

	int ndx = m_HandDoor.GetCurSel() ;

	m_pActypeDoor->m_enumDoorDir = (ACTypeDoor::DoorDir)m_HandDoor.GetItemData(ndx) ;
}

void CDlgAircraftDoorDef::OnOK()
{
	GetDataFromCtrlData() ;
	CDialog::OnOK() ;
}
// CDlgAircraftDoorDef message handlers
