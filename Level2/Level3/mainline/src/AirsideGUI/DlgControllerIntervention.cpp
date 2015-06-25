// ControllerIntervention.cpp : implementation file
//


#include "stdafx.h"
#include "Resource.h"
//#include "AirsideGUI.h"
#include "DlgControllerIntervention.h"
#include "DlgSectorSelect.h"
#include "ControllerInterventionData.h"

// CDlgControllerSocter dialog

IMPLEMENT_DYNCREATE(CControllerIntervention, CDialog)
CControllerIntervention::CControllerIntervention(int projId,CWnd* pParent)
: CDialog(CControllerIntervention::IDD, pParent),projID(projId)
{
	ControllerData.setProjID(projId) ;
}
CControllerIntervention::CControllerIntervention(CWnd* pParent)
: CDialog(CControllerIntervention::IDD, pParent)
{

}
CControllerIntervention::~CControllerIntervention()
{
}

void CControllerIntervention::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_Flight, flightCheckBox);
	DDX_Control(pDX, IDC_CHECK_CONFLICT, conflictCheckbox);
	DDX_Control(pDX, IDC_CHECK_DISTANCE, DistanceCheckbox);
	DDX_Control(pDX, IDC_CHECK_MAINTENACE, maintenaceCheckbox);
	DDX_Control(pDX, IDC_CHECK_DEPARTURE, departurecheckbox);
	DDX_Control(pDX, IDC_TREE_SECTOR, sectortree);

    DDX_Control(pDX, IDC_STATIC_NAME_VAL, sector_name);
	DDX_Control(pDX, IDC_EDIT_RADIUS, sector_radius);
	DDX_Control(pDX, IDC_COMBO_HOLDPRIORITY, Holdpri_list);
	DDX_Control(pDX, IDC_COMBO_PRIRITY1, pri1_list);
	DDX_Control(pDX, IDC_COMBO_PRIRITY2, pri2_list);
	DDX_Control(pDX, IDC_COMBO_PRIRITY3, pri3_list);
	DDX_Control(pDX, IDC_COMBO_PRIRITY4, pri4_list);
	DDX_Control(pDX, IDC_COMBO_FIX, fix_list) ;
	DDX_Control(pDX, IDC_STATIC_FIX, STATIC_FIX);
}

BOOL CControllerIntervention::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitHoldPriList();
	InitFIxList() ;
	InitPriorityList();
	InitTree();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CControllerIntervention, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SECTOR, OnTvnSelchangedTreeSector)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, OnBnClickedButtonCreate)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonApply)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIRITY1, OnCbnSelchangeComboPririty)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIRITY2, OnCbnSelchangeComboPririty2)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIRITY3, OnCbnSelchangeComboPririty3)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIRITY4, OnCbnSelchangeComboPririty4)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_SECTOR, OnTvnSelchangingTreeSector)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_CBN_SELCHANGE(IDC_COMBO_HOLDPRIORITY, OnCbnSelchangeComboHoldpriority)

	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()





// CDlgControllerSocter message handlers

BOOL     CControllerIntervention::OnSave()
{
	return  ControllerData.saveDataToDB();
}
BOOL CControllerIntervention::SaveSector(CControllerInterventionData::P_SECTOR_TYPE psector)
{
	if(psector == NULL)
		return FALSE ;
	CString val ;
	sector_name.GetWindowText(val) ;
	psector->setSectorName(val) ;
	int code = 0 ;
	if(flightCheckBox.GetCheck() == BST_CHECKED)
		psector->setInterventionRequirementSelect(FLIGHT_CHECKBOX) ;
	else
		psector->setInterventionRequirementNoSelect(FLIGHT_CHECKBOX) ;

	if(conflictCheckbox.GetCheck() == BST_CHECKED)
		psector->setInterventionRequirementSelect(CONFLICT_CHECKBOX) ;
	else
		psector->setInterventionRequirementNoSelect(CONFLICT_CHECKBOX) ;

	if(DistanceCheckbox.GetCheck() == BST_CHECKED)
		psector->setInterventionRequirementSelect(DISTANCE_CHECKBOX) ;
	else
		psector->setInterventionRequirementNoSelect(DISTANCE_CHECKBOX) ;

	if(maintenaceCheckbox.GetCheck() == BST_CHECKED)
		psector->setInterventionRequirementSelect(MAINTENACE_CHECKBOX) ;
	else
		psector->setInterventionRequirementNoSelect(MAINTENACE_CHECKBOX) ;

	if(departurecheckbox.GetCheck() == BST_CHECKED)
		psector->setInterventionRequirementSelect(DEPARTURE_CHECKBOX) ;
	else
		psector->setInterventionRequirementNoSelect(DEPARTURE_CHECKBOX) ;	  

	val.Empty();
	sector_radius.GetWindowText(val);
	psector->setAdiusOfconcern(_tstof(val)) ;

	psector->setHoldPriority(Holdpri_list.GetCurSel()) ;

  
	psector->setFixID(fix_list.GetItemData(fix_list.GetCurSel()) );
    
	psector->setInterventionPriority(PRIORITY1_INDEX,pri1_list.GetCurSel());
	psector->setInterventionPriority(PRIORITY2_INDEX,pri2_list.GetCurSel());
	psector->setInterventionPriority(PRIORITY3_INDEX,pri3_list.GetCurSel());
	psector->setInterventionPriority(PRIORITY4_INDEX,pri4_list.GetCurSel());
	return TRUE ;
}


void CControllerIntervention::OnTvnSelchangedTreeSector(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM selectedItem = sectortree.GetSelectedItem() ;
	if(selectedItem == sectortree.GetRootItem())
      setAllDlgItemsStart(FALSE) ;
	else
		setAllDlgItemsStart(TRUE) ;
	updateMessage( selectedItem) ;
	*pResult = 0;
}
void CControllerIntervention::setAllDlgItemsStart(BOOL bEnable)
{
	 flightCheckBox.EnableWindow(bEnable) ;
	 conflictCheckbox.EnableWindow(bEnable);
	 DistanceCheckbox.EnableWindow(bEnable);
	 maintenaceCheckbox.EnableWindow(bEnable);
	 departurecheckbox.EnableWindow(bEnable);
	 sector_radius.EnableWindow(bEnable);
	 Holdpri_list.EnableWindow(bEnable);
	 pri1_list.EnableWindow(bEnable);
	 pri2_list.EnableWindow(bEnable);
	 pri3_list.EnableWindow(bEnable);
	 pri4_list.EnableWindow(bEnable);
	 fix_list.EnableWindow(bEnable) ;
	 GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
	 GetDlgItem(IDOK)->EnableWindow(bEnable) ;
	 GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(bEnable) ;
	 GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(bEnable) ;
}
void CControllerIntervention::updateMessage(HTREEITEM tree_Item)
{
	if(tree_Item == NULL)
		return ;
	CSectorData* sectordata = ( CSectorData*)sectortree.GetItemData(tree_Item);
	if (sectordata != NULL)
	{
		sector_name.SetWindowText(sectordata->getName()) ;
		setCheckBoxGroup(sectordata);
		setRadiusOfConcern(sectordata);
		setFixID(sectordata) ;
		setEditHoldPriotity(sectordata);
		setPriority(sectordata) ;
	}
}
void CControllerIntervention::setCheckBoxGroup(CSectorData* data)
{
	if(data != NULL)
	{
		if(data->getInterventionRequirementSelect().isSelected(FLIGHT_CHECKBOX) == TRUE)
			flightCheckBox.SetCheck(BST_CHECKED) ;
		else
            flightCheckBox.SetCheck(BST_UNCHECKED) ;
		if(data->getInterventionRequirementSelect().isSelected(CONFLICT_CHECKBOX) == TRUE)
			conflictCheckbox.SetCheck(BST_CHECKED) ;
		else
            conflictCheckbox.SetCheck(BST_UNCHECKED) ;
		if(data->getInterventionRequirementSelect().isSelected(DISTANCE_CHECKBOX) == TRUE)
			DistanceCheckbox.SetCheck(BST_CHECKED) ;
		else
            DistanceCheckbox.SetCheck(BST_UNCHECKED) ;
		if(data->getInterventionRequirementSelect().isSelected(MAINTENACE_CHECKBOX) == TRUE)
			maintenaceCheckbox.SetCheck(BST_CHECKED) ;
		else
			maintenaceCheckbox.SetCheck(BST_UNCHECKED) ;
		if(data->getInterventionRequirementSelect().isSelected(DEPARTURE_CHECKBOX) == TRUE)
			departurecheckbox.SetCheck(BST_CHECKED) ;
		else
			departurecheckbox.SetCheck(BST_UNCHECKED) ;
	}
} 
void CControllerIntervention::setRadiusOfConcern(CSectorData* data)
{
	if (data != NULL)
	{
		CString str ;
		double val = data->getAdiusOfconcern() ;
		str.Format(_T("%.2f"),val) ;
		sector_radius.SetWindowText(str);
	}
}
void  CControllerIntervention::setFixID(CSectorData* data)
{
	if (data != NULL)
	{
		int cout = fix_list.GetCount() ;
		int index = 0 ;
		for (; index < cout ; ++index)
		{
			if(data->getFixID() == fix_list.GetItemData(index) )
			{
				fix_list.SetCurSel(index) ;
				break ;
			}
		}
		if(index == cout)
			fix_list.SetCurSel(fix_list.FindString(0,_T("Unknown"))) ;
	}
}
void CControllerIntervention::setEditHoldPriotity(CSectorData* data)
{
	if(data != NULL)
	{
		Holdpri_list.SetCurSel(data->getHoldPriority().getSelected()) ;
		if (data->getHoldPriority().getSelected() == 1)
		{
			STATIC_FIX.ShowWindow(SW_SHOW) ;
			fix_list.ShowWindow(SW_SHOW) ;
		}else
		{
			STATIC_FIX.ShowWindow(SW_HIDE) ;
			fix_list.ShowWindow(SW_HIDE) ;
		}
	}
}
void CControllerIntervention::setPriority(CSectorData* data)
{
	if (data == NULL)
		return ;
	pri1_list.SetCurSel(data->getInterventionPriority().getSelectedIndex(PRIORITY1_INDEX)) ;
	pri2_list.SetCurSel(data->getInterventionPriority().getSelectedIndex(PRIORITY2_INDEX)) ;
	pri3_list.SetCurSel(data->getInterventionPriority().getSelectedIndex(PRIORITY3_INDEX)) ;
	pri4_list.SetCurSel(data->getInterventionPriority().getSelectedIndex(PRIORITY4_INDEX)) ;

}
BOOL CControllerIntervention::InitTree()
{
	//sectortree.DeleteAllItems();
	const CControllerInterventionData::SECTOR_VECTOR vet  = ControllerData.getSectors();
	CControllerInterventionData::ITERATOR_SECTOR_CONST iter = vet.begin();
	HTREEITEM root =  sectortree.InsertItem(_T("Controller Converention"),TVI_ROOT);
	for(int i = 0;iter != vet.end();++iter,++i)
	{
		HTREEITEM item =   sectortree.InsertItem(_T((*iter)->getName()),root);
		sectortree.SetItemData(item,(DWORD)(*iter)) ;
		if(i == 0)
	      sectortree.SelectItem(item) ;
	}
	sectortree.Expand(root,TVE_EXPAND) ;
	return TRUE ;
}
HTREEITEM CControllerIntervention::AddTreeItem(CControllerInterventionData::P_SECTOR_TYPE psector)
{
	if(psector == NULL)
		return FALSE ;
	HTREEITEM item =   sectortree.InsertItem(_T(psector->getName()),sectortree.GetRootItem());
	sectortree.SetItemData(item,(DWORD)psector) ;
	sectortree.Expand(item, TVE_EXPAND);
	return  item ;
}
HTREEITEM CControllerIntervention::FindItemFromTree(const TCHAR* item_name)
{
	HTREEITEM root = sectortree.GetRootItem() ;
    if(root == NULL)
		return NULL ;
	HTREEITEM child = sectortree.GetChildItem(root) ;
	while (child != NULL)
	{
		if(_tcscmp(item_name,sectortree.GetItemText(child)) == 0 )
		{
			return child ;
		}
		child = sectortree.GetNextSiblingItem(child) ;
	}
   return NULL ;
}
BOOL CControllerIntervention::DelTreeItem()
{
	HTREEITEM selectedItem = sectortree.GetSelectedItem() ;
	if(selectedItem == NULL)
	{
		MessageBox(_T("Must select a node"),_T("Error message"),MB_OK) ;
		return FALSE ;
	}
	sectortree.DeleteItem(selectedItem) ;
	HTREEITEM child = sectortree.GetChildItem(sectortree.GetRootItem()) ;
	if(child != NULL)
	   sectortree.SelectItem(child) ;
	return TRUE ;
}
BOOL CControllerIntervention::InitHoldPriList() 
{
	for(int i = 0 ; i < CControllerInterventionData::getHoldPrioritysize() ;++i)
	{
		Holdpri_list.AddString(*(CControllerInterventionData::HOlD_Priority+ i)) ;
	}
	Holdpri_list.SetCurSel(0) ;
	return TRUE ;
}
BOOL CControllerIntervention::InitPriorityList()
{
	for(int i = 0 ; i <  CControllerInterventionData::getPriorityitemSize() ;++i)
	{
		const _TCHAR*  p = *(CControllerInterventionData::Priority_item+i);
		if(p != NULL)
		{
			pri1_list.AddString(p);
			pri2_list.AddString(p);
			pri3_list.AddString(p) ;
			pri4_list.AddString(p) ;
		}
	}
	setPriorityListsToDefault();
	return TRUE ;
}
BOOL CControllerIntervention::InitFIxList() 
{
		int index= 0 ;
	CControllerInterventionData::FIX_VECTOR fixSet = ControllerData.getFixName();
	CControllerInterventionData::FIX_VECTOR_ITERATOR iter = fixSet.begin() ;
	for ( ; iter != fixSet.end() ;++iter)
	{
	  index = fix_list.AddString((*iter).name) ;
	  if(index != CB_ERR)
	   fix_list.SetItemData(index ,(*iter).id) ;
	}
	index = fix_list.AddString(_T("Unknown"));
	if(index != CB_ERR)
	  fix_list.SetItemData(index,-1) ;
	return TRUE ;
}
void CControllerIntervention::OnBnClickedButtonCreate()
{
	// TODO: Add your control notification handler code here
	DlgSectorSelect   selectDig(projID) ;

    if(selectDig.DoModal() != IDOK) 
		  return ;
     
	
	   if(FindItemFromTree(selectDig.getName()) == NULL)
	   {
		   CControllerInterventionData::P_SECTOR_TYPE psector =  ControllerData.createNewSector();
		   psector->setSectorName(selectDig.getName()) ;
		   psector->setNameID(selectDig.getNameID()) ;
		   HTREEITEM item =  AddTreeItem(psector)  ;
		if(item != NULL)
		     sectortree.SelectItem(item) ;
		sector_name.SetWindowText(psector->getName()) ;
		setCheckboxesTodefault() ;
		setRadiusEditToDefault();
		setPriorityListsToDefault();
	   }else
	   {
		   CString str ;
		   str.Format(_T("This node \'%s\' already exists !"),selectDig.getName());
		   MessageBox(str) ;
	   }
	
}

void CControllerIntervention::OnBnClickedButtonApply()
{
	// TODO: Add your control notification handler code here	
	if(!CheckPoliy())
		return ;
	CControllerInterventionData::P_SECTOR_TYPE sector = getCurSelectedItemData();
	if(sector != NULL && sectortree.GetSelectedItem() != sectortree.GetRootItem())
	{
	 SaveSector(sector) ;
	 sector->modified() ;
	}
} 

BOOL CControllerIntervention::CheckPoliy()
{
	CString val ;
	sector_name.GetWindowText(val) ;
	BOOL res = TRUE ;
	if(val.IsEmpty())
	{
		MessageBox(_T("You should input Sector's name!")) ;
		res = FALSE ;
	}
	val = "" ;
	sector_radius.GetWindowText(val) ;
	if(val.IsEmpty())
	{
		MessageBox(_T("You should input Radius of concern! ")) ;
		res = FALSE ;
	}
	return res ;
}
//////////////////////////////////////////////////////////////////////////
void CControllerIntervention::SetSectorNameEditTodefault()
{
	sector_name.SetWindowText("");
}
void CControllerIntervention::setCheckboxesTodefault()
{
	flightCheckBox.SetCheck(BST_UNCHECKED);
	conflictCheckbox.SetCheck(BST_UNCHECKED) ;
	DistanceCheckbox.SetCheck(BST_UNCHECKED) ;
	maintenaceCheckbox.SetCheck(BST_UNCHECKED) ;
	departurecheckbox.SetCheck(BST_UNCHECKED) ;
}
void CControllerIntervention::setRadiusEditToDefault()
{
	sector_radius.SetWindowText("");
}
void CControllerIntervention::setPriorityListsToDefault()
{
	pri1_list.SetCurSel(0);
	pri2_list.SetCurSel(1);
	pri3_list.SetCurSel(2);
	pri4_list.SetCurSel(3);
}
CControllerInterventionData::P_SECTOR_TYPE CControllerIntervention::getCurSelectedItemData()
{
	HTREEITEM selectedItem = sectortree.GetSelectedItem() ;
	if(selectedItem == NULL)
	{
		return NULL ;
	}
	return  (CControllerInterventionData::P_SECTOR_TYPE )sectortree.GetItemData(selectedItem) ;
}
void CControllerIntervention::OnCbnSelchangeComboPririty()
{
	// TODO: Add your control notification handler code here	

	int sel = pri1_list.GetCurSel() ;
	int pri2 = pri2_list.GetCurSel() ;
	int pri3 = pri3_list.GetCurSel() ;
	int pri4 = pri4_list.GetCurSel() ;
	if(sel == pri2)
		SetComboToNoselect(pri2_list);
	if(sel == pri3)
		SetComboToNoselect(pri3_list);
	if(sel == pri4)
		SetComboToNoselect(pri4_list) ;
	CControllerInterventionData::P_SECTOR_TYPE p = getCurSelectedItemData() ;
	if(p != NULL)
		p->modified() ;
}
void  CControllerIntervention::SetComboToNoselect(CComboBox& box)
{
	int i = 0;
	int pri1 = pri1_list.GetCurSel() ;
	int pri2 = pri2_list.GetCurSel() ;
	int pri3 = pri3_list.GetCurSel() ;
	int pri4 = pri4_list.GetCurSel() ;
	for ( ; i < 4 ;++ i)
	{
		if(pri1 != i && pri2 != i && pri3 != i && pri4 != i)
			box.SetCurSel(i);
	}
}
void CControllerIntervention::OnCbnSelchangeComboPririty2()
{
	// TODO: Add your control notification handler code here
	int sel = pri2_list.GetCurSel() ;
	int pri1 = pri1_list.GetCurSel() ;
	int pri3 = pri3_list.GetCurSel();
	int pri4 = pri4_list.GetCurSel() ;
	if(sel == pri1)
		SetComboToNoselect(pri1_list);
	if(sel == pri3)
		SetComboToNoselect(pri3_list);
	if(sel == pri4)
		SetComboToNoselect(pri4_list) ;
	CControllerInterventionData::P_SECTOR_TYPE p = getCurSelectedItemData() ;
	if(p != NULL)
		p->modified() ;
}

void CControllerIntervention::OnCbnSelchangeComboPririty3()
{
	// TODO: Add your control notification handler code here
	int sel = pri3_list.GetCurSel() ;
	int pri1 = pri1_list.GetCurSel() ;
	int pri2 = pri2_list.GetCurSel();
	int pri4 = pri4_list.GetCurSel() ;
	if(sel == pri1)
		SetComboToNoselect(pri1_list);
	if(sel == pri2)
		SetComboToNoselect(pri2_list);
	if(sel == pri4)
		SetComboToNoselect(pri4_list) ;
	CControllerInterventionData::P_SECTOR_TYPE p = getCurSelectedItemData() ;
	if(p != NULL)
		p->modified() ;
}

void CControllerIntervention::OnCbnSelchangeComboPririty4()
{
	// TODO: Add your control notification handler code here
	int sel = pri4_list.GetCurSel() ;
	int pri1 = pri1_list.GetCurSel() ;
	int pri2 = pri2_list.GetCurSel();
	int pri3 = pri3_list.GetCurSel() ;
	if(sel == pri1)
		SetComboToNoselect(pri1_list);
	if(sel == pri3)
		SetComboToNoselect(pri3_list);
	if(sel == pri2)
		SetComboToNoselect(pri2_list) ;	
	CControllerInterventionData::P_SECTOR_TYPE p = getCurSelectedItemData() ;
	if(p != NULL)
		p->modified() ;

}

void CControllerIntervention::OnTvnSelchangingTreeSector(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	//HTREEITEM selectedItem = sectortree.GetSelectedItem() ;
	//    CControllerInterventionData::P_SECTOR_TYPE sector = (CControllerInterventionData::P_SECTOR_TYPE )sectortree.GetItemData(selectedItem) ;
	//SaveSector( sector) ;
	*pResult = 0;
}

void CControllerIntervention::OnBnClickedButtonDel()
{
	HTREEITEM item = sectortree.GetSelectedItem() ;
	if(item == NULL)
	{
		MessageBox(_T("Please Select a Node !"),_T("Error"),MB_OK) ;
		return ;
	}
	if(item != sectortree.GetRootItem())
	{

		ControllerData.DelSector( getCurSelectedItemData() ) ;
		DelTreeItem() ;
	}
}

void CControllerIntervention::OnCbnSelchangeComboHoldpriority()
{
	// TODO: Add your control notification handler code here
	if(Holdpri_list.GetCurSel() == 1)
	{
		STATIC_FIX.ShowWindow(SW_SHOW);
		fix_list.ShowWindow(SW_SHOW);
	}else
	{
		STATIC_FIX.ShowWindow(SW_HIDE);
		fix_list.ShowWindow(SW_HIDE);
	}
}

void CControllerIntervention::OnBnClickedOk()
{
	try
	{	
		CADODatabase::BeginTransaction() ;
	OnBnClickedButtonApply();
	ControllerData.DelSectorToDB() ;
	ControllerData.saveDataToDB();
	CADODatabase::CommitTransaction() ;
	}
	catch (CADODatabase e)
	{
		CADODatabase::RollBackTransation() ;
	}

	OnOK();
}

void CControllerIntervention::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}



// CControllerIntervention message handlers
