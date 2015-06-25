#pragma once
#include "../MFCExControl/CoolTree.h"
#include "../Common/FLT_CNST.H"
#include "../Common/AirportDatabase.h"
#include "NodeViewDbClickHandler.h"
#include "../MFCExControl/XTResizeDialog.h"



// CDlgAircraftSurfaceInitialCondition dialog
class CAircraftSurfaceSetting ;
class CAirportDatabase ;
class CSurfacesInitalEvent ;
class CConditionItem ;
class CDlgAircraftSurfaceInitialCondition : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgAircraftSurfaceInitialCondition)

public:
	CDlgAircraftSurfaceInitialCondition(CAirportDatabase* _database ,PSelectFlightType	_pSelectFlightType,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAircraftSurfaceInitialCondition();

// Dialog Data
	enum { IDD = IDD_DIALOG_SURFACEINITAL };
protected:
	CStatic   m_staticTool ;
	CToolBar  m_ToolBar ;
	CCoolTree m_Tree ;
	CAirportDatabase* m_AirportDataBase ;
	CAircraftSurfaceSetting* m_Setting ;
	PSelectFlightType	m_pSelectFlightType ;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	enum { TY_TIME,TY_FLIGHT_TY};
	DECLARE_MESSAGE_MAP()
protected:
	BOOL OnInitDialog() ;
	void OnSize(UINT nType, int cx, int cy) ;
	int OnCreate(LPCREATESTRUCT lpCreateStruct) ;
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;
		afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult); 
protected:
	void InitToolBar() ;
	void InitTreeView() ;
    void AddEventItemData(CSurfacesInitalEvent* _event) ;
	void AddFlightType(HTREEITEM _item , CConditionItem* _condition) ;
protected:
	void OnOK() ;
	void OnCancel() ;
	void OnSave() ;
	void OnSaveAs() ;
	void OnLoad() ;
protected:
	void OnNewButton() ;
	void OnDelButton() ;

protected:
	void NewEvent() ;
	void DelEvent() ;

	void NewFlightType() ;
	void DelFlightType() ;
protected:
	CString FormatEventItemName(CString& _name) ;
	CString FormatTimeItemName(CString& _start ,CString& _end) ;
	CString FormatFlightTypeName(CString& str) ;
	CString FormatConditionItemName(CString& str) ;
	BOOL CheckFlightType(CSurfacesInitalEvent* _event,FlightConstraint& _constraint) ;

	void InitComboString(CComboBox* pCB) ;
	void HandleComboxChange(CComboBox* pCB,HTREEITEM _item) ;
	void ShowDialogHandle(HTREEITEM _item) ;
};
