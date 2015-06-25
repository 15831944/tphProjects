#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "../MFCExControl/CoolTree.h"
#include "../MFCExControl/XTResizeDialog.h"


// CDlgWeatherScript dialog
class CWeatherScripts ;
class CPrecipitationTypeEventItem ;
class CDlgWeatherScript : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgWeatherScript)

public:
	CDlgWeatherScript(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgWeatherScript();

// Dialog Data
	enum { IDD = IDD_DIALOG_WEATHER_SCRIPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	BOOL  OnInitDialog() ;
	void OnSize(UINT nType, int cx, int cy) ;
	int OnCreate(LPCREATESTRUCT lpCreateStruct) ;
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;

	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult); 
protected:
	void InitTreeView() ;
	void InitToolBar() ;
	void AddPrecipitationItem(CPrecipitationTypeEventItem* _data) ;
protected:
	void OnSave() ;
	void OnOK() ;
	void OnCancel() ;

	void OnLoad() ;
	void OnSaveAs() ;
protected:
	void OnNewButton() ;
	void OnDelButton() ;
protected:
	CCoolTree m_TreeView;
	CStatic m_StaticTool;
	CButton m_ButtonLoad;
	CButton m_ButtonSaveAs;
	CButton m_ButtonSave;
    CToolBar m_ToolBar ;
	CWeatherScripts* m_WeatherScripts ;
protected:
	CString FormatPrecipitationItemName(CString& _name) ;
	CString FormatStartTime(CString& _time) ;
	CString FormatEndTime(CString& _time) ;

	void InitComboString(CComboBox* pCB) ;
	void HandleComboxChange(CComboBox* pCB,HTREEITEM _item) ;
};
