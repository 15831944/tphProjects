#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "../MFCExControl/XTResizeDialog.h"


class InputTerminal;
class InputOnboard;
class CStorageDataSet;
class CCarryonStoragePriorities;
class CFlightCarryonStoragePriorities;

class DlgCarryonStoragePriorities: public CXTResizeDialog
{
	DECLARE_DYNAMIC(DlgCarryonStoragePriorities)
public:
	DlgCarryonStoragePriorities(InputTerminal * pInterm,InputOnboard* pInOnboard,CWnd* pParent = NULL);
	~DlgCarryonStoragePriorities(void);

	enum { IDD = IDD_DIALOG_STORAGEPRIORITIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

private:
	void InitListCtrl();
	bool InitFlightList();
	void InitPaxList();
	void SetListCtrl();
	CString getStorageName(int nStorageID);
	int getStorageID(CString strStorageName);

public:
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnItemchangedListData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeFlight();
	afx_msg void OnSelchangePaxType();

	afx_msg void OnToolbarPaxTypeAdd();
	afx_msg void OnToolbarPaxTypeDel();
	afx_msg void OnToolbarPaxTypeEdit();

	afx_msg void OnDataAdd();
	afx_msg void OnDataDel();

protected:
	CListBox m_lisPaxType;
	CListBox m_listFlight;
	CListCtrlEx m_wndListCtrl;
	CToolBar m_wndPaxToolBar;
	CToolBar m_wndDataToolBar;
	InputTerminal *m_pInterm;
	InputOnboard* m_pInOnboard;
	CCarryonStoragePriorities* m_pCarryonStoragePriorities;
	CFlightCarryonStoragePriorities* m_pSelFlightData;
	CStorageDataSet* m_pStorages;
};
