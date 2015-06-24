#pragma once

class InputLandside;

// CDlgLandsideSelectVehicleType dialog

class CDlgLandsideSelectVehicleType : public CDialog
{
	DECLARE_DYNAMIC(CDlgLandsideSelectVehicleType)

public:
	CDlgLandsideSelectVehicleType(InputLandside *pLandside, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLandsideSelectVehicleType();

// Dialog Data
	//enum { IDD = IDD_DIALOG_LANDSIDE_SELECTVEHICLETYPE };

public:
	//return 0, all type
	//return -1, no select
	//return vehicle range
	int GetSelectType();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CTreeCtrl m_treeType;
	
	InputLandside *m_pLandside;


public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();

	int m_nTypeSelected;
};
