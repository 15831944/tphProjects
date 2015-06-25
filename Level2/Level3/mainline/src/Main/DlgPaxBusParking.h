#pragma once
#include "afxcmn.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "..\Common\path.h"
#include "../MFCExControl/XTResizeDialog.h"


// CDlgPaxBusParking dialog
class InputTerminal;
class PaxBusParkingPlaceList;
class PaxBusParkingPlace;
class CTermPlanDoc;
class CDlgPaxBusParking : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgPaxBusParking)

public:
	CDlgPaxBusParking(int nProjID, InputTerminal* _pInTerm, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPaxBusParking();

// Dialog Data
	enum { IDD = IDD_DIALOG_PAXBUSPARKING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNewItem();
	afx_msg void OnDelItem();
public:
	void InitToolBar();
	void InitListCtrl();
	void UpdateToolBar();
	int GetListCtrlSelectedItem();
	void SetListContent();
	void InsertParkingPlaceItem(int idx,PaxBusParkingPlace* pItem);
private:
	CListCtrlEx m_wndListCtrl;
	CToolBar m_wndToolBar;
    int n_selectedItem;
	int m_nProjID;
	InputTerminal* m_pInTerm;
	PaxBusParkingPlaceList* m_pPaxBusParkingList;
	Path m_copypath;
	int m_nFlag;
public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnMsgComboChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNoEditDBClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);
	afx_msg void OnProcpropPickfrommap();
protected:
	virtual void OnOK();
	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);
	bool DoTempFallBackFinished(WPARAM wParam, LPARAM lParam);
	void SetObjectPath(Path& path);
public:
	afx_msg void OnBnClickedSave();
	afx_msg void OnLvnItemchangedListPaxbusparking(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnNMRclickListPaxbusparking(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonCopy();
	afx_msg void OnBnClickedButtonPaste();
};
