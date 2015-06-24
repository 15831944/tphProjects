#pragma once

#include "AirsideObjectTreeCtrl.h"
#include <AirsideGUI/UnitPiece.h>
#include <Common/point.h>

class WallShape;
class InputTerminal;
class CFloor2;
// DlgWallShapeProp dialog
class DlgWallShapeProp : public CXTResizeDialog,public CUnitPiece
{
	DECLARE_DYNAMIC(DlgWallShapeProp)

public:
	
	DlgWallShapeProp(WallShape * _pWall=NULL ,CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgWallShapeProp();
	virtual BOOL OnInitDialog();


// Dialog Data
	enum { IDD = IDD_DIALOG_WALLSHAPE };
	
	int DoFakeModal();
	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	bool IsNameValid(CString& strName);

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);
	
	afx_msg void OnDropdownComboLevel2();
	afx_msg void OnDropdownComboLevel3();
	afx_msg void OnDropdownComboLevel4();
	afx_msg void OnKillfocusComboLevel1();

	DECLARE_MESSAGE_MAP()
	CComboBox m_wndName1Combo;
	CComboBox m_wndName2Combo;
	CComboBox m_wndName3Combo;
	CComboBox m_wndName4Combo;
	CAirsideObjectTreeCtrl m_wndPropTree;

	afx_msg void OnProcpropPickfrommap();
	afx_msg void OnNMRclickTreeProperties(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);
	afx_msg void OnProcpropDelete();

	void LoadTree();
	CFloor2* GetPointFloor(int nLevel);

	WallShape * m_pWallShape ;
	void PreProcessName();
	HTREEITEM m_hWallPath;
	HTREEITEM m_hWidth;
	HTREEITEM m_hHeight;
	HTREEITEM m_hRClickItem;

public:

	CString	m_csLevel1;
	CString	m_csLevel2;
	CString	m_csLevel3;
	CString	m_csLevel4;
	double m_dWidth;
	double m_dheight;
	std::vector<Point> m_Wallpath;
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnBnClickedOk();
	virtual void OnOK();
	afx_msg void OnWallheightDefineheight();
	afx_msg void OnWallwidthDefinewidth();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	void SetTypeByLevel1();
	InputTerminal* GetInputTerminal();
	int m_nForceProcType;
};

