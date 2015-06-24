// DlgLandsideSelectVehicleType.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgLandsideSelectVehicleType.h"
#include ".\dlglandsideselectvehicletype.h"
#include "..\Landside\VehicleLandSideDefine.h"
#include "..\LandsideReport\LandsideBaseParam.h"
#include "../Landside/InputLandside.h"

// CDlgLandsideSelectVehicleType dialog

IMPLEMENT_DYNAMIC(CDlgLandsideSelectVehicleType, CDialog)
CDlgLandsideSelectVehicleType::CDlgLandsideSelectVehicleType(InputLandside *pLandside,CWnd* pParent /*=NULL*/)
	: CDialog(/*CDlgLandsideSelectVehicleType::IDD*/IDD_DIALOG_LANDSIDE_SELECTVEHICLETYPE, pParent)
{
	m_nTypeSelected = -1;
	m_pLandside = pLandside;
}


CDlgLandsideSelectVehicleType::~CDlgLandsideSelectVehicleType()
{
}

void CDlgLandsideSelectVehicleType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_VEHIClETYPE, m_treeType);
}


BEGIN_MESSAGE_MAP(CDlgLandsideSelectVehicleType, CDialog)
END_MESSAGE_MAP()


// CDlgLandsideSelectVehicleType message handlers

BOOL CDlgLandsideSelectVehicleType::OnInitDialog()
{
	CDialog::OnInitDialog();

//add root
	CString strAllType = LandsideBaseParam::GetVehicleTypeName(m_pLandside,LANDSIDE_ALLVEHICLETYPES);
	HTREEITEM hRoot =  m_treeType.InsertItem(strAllType);
	m_treeType.SetItemData(hRoot,LANDSIDE_ALLVEHICLETYPES);
	//vehicle types

	//enum enumVehicleLandSideType
	//{
	//	VehiclType_Private=100,
	//	VehicleType_Public,
	//	VehicleType_Taxi,
	//	VehicleType_Iterate,
	//	VehicleType_LandSideCount
	//};

	//for (int vtype = VehiclType_Private; vtype < VehicleType_LandSideCount; ++ vtype)
	//{
	//	HTREEITEM hItem =  m_treeType.InsertItem(LandSideVehicleTypeName[vtype-VehiclType_Private],NULL,NULL, hRoot);
	//	m_treeType.SetItemData(hItem, vtype);
	//}

	/*size_t nVehicleSpeCount = m_pLandside->getOperationVehicleList().GetElementCount();
	COMBOBOXEXITEM cbItem;
	cbItem.mask = CBEIF_TEXT;
	for(size_t i = 0; i < nVehicleSpeCount; i++) 
	{
		CString strName = m_pLandside->getOperationVehicleList().GetItem(i)->getName();
		HTREEITEM hItem =  m_treeType.InsertItem(strName,NULL,NULL, hRoot);

		int nTypeID = m_pLandside->getOperationVehicleList().GetItem(i)->GetID();
		m_treeType.SetItemData(hItem, nTypeID);
	}*/

	m_treeType.Expand(hRoot,TVE_EXPAND);
	


	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgLandsideSelectVehicleType::GetSelectType()
{
	return m_nTypeSelected;
}

void CDlgLandsideSelectVehicleType::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	HTREEITEM hItem = m_treeType.GetSelectedItem();
	if(hItem != NULL)
		m_nTypeSelected = m_treeType.GetItemData(hItem);


	CDialog::OnOK();
}
