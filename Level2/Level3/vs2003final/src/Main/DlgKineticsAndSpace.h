#pragma once
#include "../AirsideGUI/NodeViewDbClickHandler.h"
#include "../MFCExControl/ListCtrlEx.h"

class InputTerminal;
class KineticsAndSpaceData;
class KineticsAndSpaceDataList;
enum KineticsAndSpaceDataType;

class DlgKineticsAndSpace: public CXTResizeDialog
{
	DECLARE_DYNAMIC(DlgKineticsAndSpace)
public:
	DlgKineticsAndSpace(int nPrjID, KineticsAndSpaceDataType eDataType, CWnd* pParent = NULL);
	~DlgKineticsAndSpace(void);
	// Dialog Data
	enum { IDD = IDD_DIALOG_MANTENANCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNewItem();
	afx_msg void OnDelItem();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnSelComboBox(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnDbClickListItem( WPARAM wparam, LPARAM lparam);

	void SetListContent();

protected:
	CString getDlgTitleAndUnit(KineticsAndSpaceDataType eDataType);
	void InitListCtrl();
	InputTerminal* GetInputTerminal();

protected:
	CToolBar m_wndFltToolbar;
	CListCtrlEx	m_wndListCtrl;

	int	m_nProjID;
	CString	m_strCaption;
	CString m_strUnit;

	int m_nRowSel;
	int m_nColumnSel;

	KineticsAndSpaceDataList* m_pKineticsAndSpaceDataList;
	CWnd* m_pParentWnd;

};
