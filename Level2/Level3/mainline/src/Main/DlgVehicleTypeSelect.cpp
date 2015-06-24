// DlgVehicleTypeSelect.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgVehicleTypeSelect.h"
#include "../InputAirside/VehicleSpecifications.h"
#include "../InputAirside/VehicleSpecificationItem.h"
#include "../InputAirside/ALTAirport.h"
// CDlgVehicleTypeSelect dialog

IMPLEMENT_DYNAMIC(CDlgVehicleTypeSelect, CDialog)
CDlgVehicleTypeSelect::CDlgVehicleTypeSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgVehicleTypeSelect::IDD, pParent)
{
}

CDlgVehicleTypeSelect::~CDlgVehicleTypeSelect()
{
}

void CDlgVehicleTypeSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_VEHICLETYPE, m_VehicleListBox);
}


BEGIN_MESSAGE_MAP(CDlgVehicleTypeSelect, CDialog)
END_MESSAGE_MAP()
BOOL CDlgVehicleTypeSelect::OnInitDialog() 
{
	CDialog::OnInitDialog() ;
	InitVehicleListBox() ;
	return TRUE ;
}
void CDlgVehicleTypeSelect::InitVehicleListBox()
{

   for ( int i = 0 ; i < VehicleSize ;i++)
   {
	   m_VehicleListBox.AddString( CVehicleSpecificationItem::GetVehicleTypeByID(i) ) ;
   }
}
void CDlgVehicleTypeSelect::OnOK()
{
	int ndx = m_VehicleListBox.GetCurSel() ;
	if(ndx != LB_ERR)
	{
		m_VehicleListBox.GetText(ndx,m_SelectedType) ;
	}
	CDialog::OnOK() ;
}
CString& CDlgVehicleTypeSelect::GetSelectedTy()
{
	return m_SelectedType ;
}
// CDlgVehicleTypeSelect message handlers
void CDlgVehiclePoolSelect::OnOK()
{
	int ndx = m_VehicleListBox.GetCurSel() ;
	if(ndx != LB_ERR)
	{
		m_VehicleListBox.GetText(ndx,m_SelectedType) ;
		m_PoolID = m_VehicleListBox.GetItemData(ndx) ;
	}
	CDialog::OnOK() ;
}
int CDlgVehiclePoolSelect::GetPoolID()
{
	return m_PoolID ;
}
BOOL CDlgVehiclePoolSelect::OnInitDialog()
{
	CDialog::OnInitDialog() ;
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);

		std::vector<ALTObject> vParkingLot;	
		ALTObject::GetObjectList(ALT_VEHICLEPOOLPARKING,*iterAirportID,vParkingLot);
		std::vector<ALTObject>::iterator iter = vParkingLot.begin();
		int ndx = -1 ;
		for (; iter != vParkingLot.end(); ++iter)
		{
			ndx = m_VehicleListBox.AddString((*iter).GetObjNameString()) ;
			m_VehicleListBox.SetItemData(ndx,(*iter).getID()) ;
		}
	}
	SetWindowText(_T("Vehicle Pool Selected")) ;
	return TRUE ;
}