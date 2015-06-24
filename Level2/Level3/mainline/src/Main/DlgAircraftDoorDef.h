#pragma once


// CDlgAircraftDoorDef dialog
class ACTypeDoor ;
class CDlgAircraftDoorDef : public CDialog
{
	DECLARE_DYNAMIC(CDlgAircraftDoorDef)

public:
	CDlgAircraftDoorDef(ACTypeDoor* _pActypeDoor , CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAircraftDoorDef();

// Dialog Data
	enum { IDD = IDD_DIALOG_AIRCRAFTDOOR_DEF };
protected:
	ACTypeDoor* m_pActypeDoor ;
	CEdit m_DoorName ;
	CEdit m_DoorHeight ;
	CEdit m_DoorWidth ;
	CEdit m_DoorSill ;
	CEdit m_Center ;
	CComboBox m_HandDoor ;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	BOOL OnInitDialog() ;
	void OnOK() ;
protected:
	void InitCtrlData() ;
	void GetDataFromCtrlData() ;
};
