#pragma once
#include "afxwin.h"
#include "InputAirside/InputAirside.h"

// CDlgVehicleTypeSelect dialog

class CDlgVehicleTypeSelect : public CDialog
{
	DECLARE_DYNAMIC(CDlgVehicleTypeSelect)

public:
	CDlgVehicleTypeSelect(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgVehicleTypeSelect();

// Dialog Data
	enum { IDD = IDD_DIALOG_VEHICLE_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString& GetSelectedTy() ;
protected:
	CListBox m_VehicleListBox;
protected:
	CString m_SelectedType ;
protected:
	void InitVehicleListBox() ;
    virtual BOOL OnInitDialog() ;
protected:
	afx_msg virtual void OnOK() ;
};
class CDlgVehiclePoolSelect : public CDlgVehicleTypeSelect
{
public:
	CDlgVehiclePoolSelect(int _projID ,CWnd* pParent = NULL):m_nProjID(_projID),CDlgVehicleTypeSelect(pParent) { m_PoolID = 0 ;};
	~CDlgVehiclePoolSelect() {};
	BOOL OnInitDialog() ;
public:
	int GetPoolID() ;
protected:
	int m_nProjID ;
	int m_PoolID ;

	void OnOK() ;
};
