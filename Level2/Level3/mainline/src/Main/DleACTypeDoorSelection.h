#pragma once
#include "afxcmn.h"
#include "../Common/ACTypeDoor.h"

// DlgACTypeDoorSelection dialog

class DlgACTypeDoorSelection : public CDialog
{
	DECLARE_DYNAMIC(DlgACTypeDoorSelection)

public:
	DlgACTypeDoorSelection(ACTYPEDOORLIST* pAcDoors,const CString& strACName, CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgACTypeDoorSelection();

// Dialog Data
	enum { IDD = IDD_DLEACTYPEDOORSELECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchangedTreeAcdoorsel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();

	virtual BOOL OnInitDialog();
	void OnInitDoors();

	ACTypeDoor* GetSelectedAcTypeDoor() {return m_pDoor;}
private:
	CTreeCtrl m_wndTreeCtrl;
	ACTYPEDOORLIST* m_pAcDoors;
	CString m_strACType;
	ACTypeDoor* m_pDoor;
};
