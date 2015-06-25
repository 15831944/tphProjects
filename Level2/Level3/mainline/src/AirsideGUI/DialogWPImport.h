#pragma once
#include "afxcmn.h"
#include"EditListCtrl.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/AirWayPoint.h"
#include "MFCExControl/XTResizeDialog.h"

// CDialogWPImport dialog
class CLatitude;
class CLongitude;
class CDialogWPImport : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDialogWPImport)
public:
	CDialogWPImport(int nProjectID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogWPImport();
public:
	CToolBar	m_wndFltToolbar;
	CEditListCtrl m_list;	

	//BOOL m_dataIsWright;
	//BOOL m_isFileImport;

	//int m_nObjID;
	//int m_nWayPointID;	
	//int m_nAirPortID;

	// Dialog Data
	enum { IDD = IDD_DIALOG_WAYPOINT_IMPORT };
	//ref_ptr<ALTObject> m_pObject;
	//inline ALTObject * GetObject(){ return m_pObject.get(); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNewImport();
	afx_msg void OnDelRecord();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnOK();
	afx_msg void OnSave(); 
	void UpdateUIState();

private:
	int m_nProjectId;
	ALTAirport m_Airport;
	std::vector<AirWayPoint> m_vWaypoints;
};
