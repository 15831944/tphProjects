#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "../Common/ACTypeDoor.h"
#include "../MFCExControl/ListCtrlReSizeColum.h"
// CDlgAircraftDoors dialog
class CACTypesManager ;
class CACType ;
class CAirportDatabase;
class CDlgAircraftDoors : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgAircraftDoors)
public:
	CDlgAircraftDoors(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAircraftDoors();

// Dialog Data
	enum { IDD = IDD_DIALOG_AIRCRAFTDOOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	BOOL OnInitDialog() ;
//	void OnInitListBox() ;

	void OnInitDoorListHead() ;
	void OnInitDoorList(CACType* _actype) ;
protected:
	void OnAddDoor() ;
	void OnDeleteDoor();
	void OnEditDoor();

	void AddDoorListItem(ACTypeDoor* _acdoor);



	afx_msg void OnSize(UINT nType, int cx, int cy);
	int OnCreate(LPCREATESTRUCT lpCreateStruct) ;
public:
	//afx_msg void OnLbnSelchangeList1();
	void EditDoorListItem(ACTypeDoor* _acdoor);
	ACTypeDoor* GetCurrentSelectDoor(int& _selNdx);
	void setACType(CACType* pACType);
protected:
	CListBox m_ListBox;
	CSortListCtrlEx m_ListDoor;
	CStatic m_ToolStatic;
	//CACTypesManager* m_ActypeManager ;
	CToolBar m_DoorTool ;
	CACType* m_SelActype ;
	//CAirportDatabase* m_AirportDB ;
protected:
	void OnOK() ;
	void OnSave() ;
	void OnCancel() ;
	void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
};
