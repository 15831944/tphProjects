// DlgAircraftDoors.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgAircraftDoors.h"
#include ".\dlgaircraftdoors.h"
#include "../Common/ACTypesManager.h"
#include "DlgAircraftDoorDef.h"
#include "../Common/AirportDatabase.h"
// CDlgAircraftDoors dialog

IMPLEMENT_DYNAMIC(CDlgAircraftDoors, CPropertyPage)
CDlgAircraftDoors::CDlgAircraftDoors(CWnd* pParent )
	: CPropertyPage(CDlgAircraftDoors::IDD)
{
}

CDlgAircraftDoors::~CDlgAircraftDoors()
{
}

void CDlgAircraftDoors::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DOOR, m_ListDoor);
	DDX_Control(pDX, IDC_STATIC_TOOL, m_ToolStatic);
}


BEGIN_MESSAGE_MAP(CDlgAircraftDoors, CPropertyPage)
	ON_NOTIFY(LVN_ITEMCHANGED,IDC_LIST_DOOR,OnLvnItemchangedList)
	ON_WM_SIZE() 
	ON_WM_CREATE() 
	ON_COMMAND(ID_AIRCRAFTDOOR_ADD,OnAddDoor)
	ON_COMMAND(ID_AIRCRAFTDOOR_DEL,OnDeleteDoor)
	ON_COMMAND(ID_AIRCRAFTDOOR_EDIT,OnEditDoor)
	ON_COMMAND(ID_SAVE,OnSave)
END_MESSAGE_MAP()

void CDlgAircraftDoors::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION posi = m_ListDoor.GetFirstSelectedItemPosition() ;
	if(posi != NULL)
	{
		m_DoorTool.GetToolBarCtrl().EnableButton(ID_AIRCRAFTDOOR_DEL,TRUE) ;
		m_DoorTool.GetToolBarCtrl().EnableButton(ID_AIRCRAFTDOOR_EDIT,TRUE) ;
	}else
	{
		m_DoorTool.GetToolBarCtrl().EnableButton(ID_AIRCRAFTDOOR_DEL,FALSE) ;
		m_DoorTool.GetToolBarCtrl().EnableButton(ID_AIRCRAFTDOOR_EDIT,FALSE) ;
	}

}
// CDlgAircraftDoors message handlers

BOOL CDlgAircraftDoors::OnInitDialog()
{
	CPropertyPage::OnInitDialog() ;
	OnInitDoorListHead() ;
	ASSERT(m_SelActype);
	OnInitDoorList(m_SelActype);
	m_ToolStatic.ShowWindow(FALSE) ;

	CRect rect ;
	m_ToolStatic.GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;

	m_DoorTool.MoveWindow(&rect) ;

	m_DoorTool.GetToolBarCtrl().EnableButton(ID_AIRCRAFTDOOR_ADD,TRUE);
	m_DoorTool.GetToolBarCtrl().EnableButton(ID_AIRCRAFTDOOR_DEL,FALSE);
	m_DoorTool.GetToolBarCtrl().EnableButton(ID_AIRCRAFTDOOR_EDIT,FALSE);
	
	
	m_DoorTool.ShowWindow(TRUE) ;

	return TRUE ;
}

void CDlgAircraftDoors::setACType(CACType* pACType)
{
	m_SelActype = pACType;
}

void CDlgAircraftDoors::OnInitDoorListHead()
{
	DWORD dwStyle = m_ListDoor.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_ListDoor.SetExtendedStyle( dwStyle );

	char* columnLabel[] = {	"Door Name","Nose to door center", "Door height", "Door width","Sill height","Left/Right hand door"};
	int DefaultColumnWidth[] = {90,150,95,95,95,150};

	for (int i = 0; i < 6; i++)
	{
		m_ListDoor.InsertColumn(i,columnLabel[i],LVCFMT_CENTER,DefaultColumnWidth[i]) ;
	}
	m_ListDoor.InitColumnWidthPercent() ;
}

void CDlgAircraftDoors::OnInitDoorList( CACType* _actype )
{
	m_ListDoor.DeleteAllItems() ;
	if(_actype == NULL)
		return ;
	int size = (int)_actype->m_vACDoorList.size() ;
	for (int ndx = 0 ; ndx < size ;ndx++)
	{
		AddDoorListItem(_actype->m_vACDoorList.at(ndx) ) ;
	}

}
void CDlgAircraftDoors::AddDoorListItem(ACTypeDoor* _acdoor)
{
	if(_acdoor == NULL)
		return ;
	CString val ;
	int ndx = m_ListDoor.InsertItem(0,_acdoor->m_strName) ;
	val.Format(_T("%0.2f"),_acdoor->m_dNoseDist) ;
	m_ListDoor.SetItemText(ndx,1,val) ;
	val.Format(_T("%0.2f"),_acdoor->m_dHeight) ;
	m_ListDoor.SetItemText(ndx,2,val) ;

	val.Format(_T("%0.2f"),_acdoor->m_dWidth) ;
	m_ListDoor.SetItemText(ndx,3,val) ;

	val.Format(_T("%0.2f"),_acdoor->m_dSillHeight) ;
	m_ListDoor.SetItemText(ndx,4,val) ;

	m_ListDoor.SetItemText(ndx,5,ACTypeDoor::GetStringByHand(_acdoor->m_enumDoorDir)) ;

	m_ListDoor.SetItemData(ndx,(DWORD_PTR)_acdoor) ;
}
void CDlgAircraftDoors::EditDoorListItem(ACTypeDoor* _acdoor)
{
	if(_acdoor == NULL)
		return ;
	POSITION position = m_ListDoor.GetFirstSelectedItemPosition() ;
	if(position == NULL)
		return ;
	int ndx = m_ListDoor.GetNextSelectedItem(position) ;
	CString val ;
	m_ListDoor.SetItemText(ndx,0,_acdoor->m_strName) ;
	val.Format(_T("%0.2f"),_acdoor->m_dNoseDist) ;
	m_ListDoor.SetItemText(ndx,1,val) ;
	val.Format(_T("%0.2f"),_acdoor->m_dHeight) ;
	m_ListDoor.SetItemText(ndx,2,val) ;

	val.Format(_T("%0.2f"),_acdoor->m_dWidth) ;
	m_ListDoor.SetItemText(ndx,3,val) ;

	val.Format(_T("%0.2f"),_acdoor->m_dSillHeight) ;
	m_ListDoor.SetItemText(ndx,4,val) ;

	m_ListDoor.SetItemText(ndx,5,ACTypeDoor::GetStringByHand(_acdoor->m_enumDoorDir)) ;

	m_ListDoor.SetItemData(ndx,(DWORD_PTR)_acdoor) ;
}

void CDlgAircraftDoors::OnSize( UINT nType, int cx, int cy )
{
	CPropertyPage::OnSize(nType,cx,cy) ;
	if(m_ListDoor.GetSafeHwnd())
		m_ListDoor.ResizeColumnWidth() ;
}

int CDlgAircraftDoors::OnCreate( LPCREATESTRUCT lpCreateStruct )
{

	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_DoorTool.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)  ||
		!m_DoorTool.LoadToolBar(IDR_AIRCRAFTDOOR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

void CDlgAircraftDoors::OnAddDoor()
{
	ACTypeDoor* pactype = new ACTypeDoor ;
		CDlgAircraftDoorDef dlg(pactype,this) ;
		if(dlg.DoModal() == IDOK)
		{
			AddDoorListItem(pactype) ;
			pactype->SetACID(m_SelActype->GetID()) ;
			m_SelActype->m_vACDoorList.push_back(pactype);
		}else
			delete pactype ;
}

void CDlgAircraftDoors::OnDeleteDoor()
{
		int ndx = 0 ; 
	ACTypeDoor* Pdoor = GetCurrentSelectDoor(ndx) ;
	if(Pdoor == NULL)
		return ;
	m_SelActype->RemoveDoor(Pdoor) ;
	m_ListDoor.DeleteItem(ndx) ;
}
ACTypeDoor* CDlgAircraftDoors::GetCurrentSelectDoor(int& _selNdx)
{
	POSITION position = m_ListDoor.GetFirstSelectedItemPosition() ;
	if(position == NULL)
		return NULL;
	_selNdx  = m_ListDoor.GetNextSelectedItem(position) ;
	ACTypeDoor* Pdoor = (ACTypeDoor*)m_ListDoor.GetItemData(_selNdx) ;
	return Pdoor ;
}
void CDlgAircraftDoors::OnEditDoor()
{
	int ndx = 0 ;
	CDlgAircraftDoorDef dlg(GetCurrentSelectDoor(ndx),this) ;
	dlg.SetDlgTitle("Edit aircraft door");
	if(dlg.DoModal() == IDOK)
	{
		EditDoorListItem(GetCurrentSelectDoor(ndx)) ;
	}
}
void CDlgAircraftDoors::OnOK()
{
	CPropertyPage::OnOK() ;
}

void CDlgAircraftDoors::OnSave()
{
}

void CDlgAircraftDoors::OnCancel()
{
	CPropertyPage::OnCancel() ;
}