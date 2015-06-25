#pragma once
#include "..\MFCExControl\listctrlex.h"

#include "..\AirsideGUI\DialogResize.h"
#include "..\InputAirside\BaggageCartParkingLinkage.h"

// DlgBaggageCartParking dialog

class InputTerminal; 

class CDlgBaggageCartParking : public CDialogResize
{
	DECLARE_DYNAMIC(CDlgBaggageCartParking)

public:
	CDlgBaggageCartParking(int nPrjID, InputTerminal* pInterm, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgBaggageCartParking();

// Dialog Data
	enum { IDD = IDD_DIALOG_BAGGAGE_PARKING_SPEC };


	virtual BOOL OnInitDialog();
	void InitToolBar();
	void InitListCtrl();
	void UpdateToolBar();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNewItem();
	afx_msg void OnDeleteItem();
	afx_msg void OnEditItem();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedSave();
	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);
	afx_msg void OnProcpropPickfrommap(); 


	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);
	void UpdateClickButton();
	void InsertParkingPlaceItem(int idx,BaggageParkingPlace* pItem);

	void LoadListContent();
	DECLARE_MESSAGE_MAP()

	DECLARE_DLGRESIZE_MAP


	CListCtrlEx m_wndListCtrl;
	CToolBar m_wndToolBar;

protected:
	InputTerminal  * m_pInterm;
	int m_nPrjID;
	int m_iSelectItem;

//	Path m_copyPath;
	bool m_bDataChanged;

	std::vector<int> m_vTypeList;
	BaggageCartParkingLinkage m_BaggageCartSpec;
public:
	afx_msg void OnNMDblclkListctrlBagparkpos(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListctrlBagparkpos(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickListctrlBagparkpos(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCopy();
	afx_msg void OnBnClickedPaste();
	afx_msg LRESULT OnMsgComboChange(WPARAM wParam, LPARAM lParam);
};
