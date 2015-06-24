#pragma once
#include "../MFCExControl/ListCtrlReSizeColum.h"
#include "../Common/IDGather.h"
// CDlgAircraftConfiguration dialog
class CAircaftLayOut ;
class CAircarftLayoutManager ;
class InputTerminal ;
class CTermPlanDoc;
class CDlgAircraftConfiguration : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgAircraftConfiguration)

public:
	CDlgAircraftConfiguration(const CString& _actype ,int Flightid ,InputTerminal* _input ,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAircraftConfiguration();

// Dialog Data
	enum { IDD = IDD_DIALOG_AIRCRAFT_CONFIGURATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void OnLvnItemchangedListRules(NMHDR *pNMHDR, LRESULT *pResult) ;
protected:
	CSortListCtrlEx m_list ;
	CToolBar m_toolBar ;
	CEdit m_Static_ToolBar ;
	CString m_ACType ;
	CAircarftLayoutManager* m_LayOutManager ;
	int m_flightId ; 
	InputTerminal* m_terminal ;
	CTermPlanDoc *  pActiveDoc  ;
protected:
	BOOL OnInitDialog() ;
	afx_msg void OnSize(UINT nType, int cx, int cy) ;
protected:
	void OnInitList() ;
	void OnInitListView() ;
	void OnInitListData() ;
    void InsertListItem(CAircaftLayOut* _layout,int _index) ;
	void EditListItem(CAircaftLayOut* _layout,int col) ;
	void DelListItem() ;
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct) ;
	void InitToolBar() ;
	DECLARE_MESSAGE_MAP()
protected:
	afx_msg void OnNewAircraftConfig() ;
	afx_msg void OnDelAircraftConfig() ;
	afx_msg void OnEditAircraftConfig() ;

	void AfterSelectItem() ;
protected:
	std::vector<CString> m_ColumName ;
protected:
	void OnOK() ;
	void OnSelect() ;
};
