#pragma once
#include "../InputAirside/ApproachSeparationType.h"

// DlgACTypeSelection dialog

class AircraftClassifications;

class AIRSIDEGUI_API DlgACTypeSelection : public CDialog
{
	DECLARE_DYNAMIC(DlgACTypeSelection)

public:
	DlgACTypeSelection(int nProjID, CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgACTypeSelection();

// Dialog Data
	//enum { IDD = IDD_DIALOG_ACTYPESELECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	int m_nProjID;
	CTreeCtrl m_TreeACType;
	CString m_strACType;
	int m_nSelACTypeID;
	FlightClassificationBasisType m_enumACCategory;
	std::vector<AircraftClassifications*> m_vAircraftClassifications;

	void LoadTree();
	int GetCurACTypePos(HTREEITEM hTreeItem);

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTvnSelchangedTreeACType(NMHDR *pNMHDR, LRESULT *pResult);
	int GetACTypeID();
	CString GetACTypeName();
	FlightClassificationBasisType GetACCategory();
};
