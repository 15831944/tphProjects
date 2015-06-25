#pragma once
#include "afxcmn.h"


// CDlgConflictSelectTaxiway dialog

class CDlgConflictSelectTaxiway : public CDialog
{
	DECLARE_DYNAMIC(CDlgConflictSelectTaxiway)
public:
	typedef std::vector<std::pair<CString,int> > TaxiwayVector;
	typedef std::vector<std::pair<CString,int> >::iterator TaxiwayVectorIter;

	typedef std::map<CString, TaxiwayVector> TaxiwayVectorMap;
	typedef std::map<CString, TaxiwayVector>::iterator TaxiwayVectorMapIter;

public:
	CDlgConflictSelectTaxiway(TaxiwayVectorMap* pTaxiwayVectorMap, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgConflictSelectTaxiway();
	int GetTaxiwayID(){return m_nTaxiwayID;}
	CString GetTaxiwayName(){return m_strTaxiwayName;}

// Dialog Data
	enum { IDD = IDD_DIALOG_CONFLICTSELECTTAXIWAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CTreeCtrl m_wndSelectTaxiwayTree;
	TaxiwayVectorMap* m_pTaxiwayVectorMap;
	int m_nTaxiwayID;
	CString m_strTaxiwayName;
	void LoadTree();
public:
	afx_msg void OnTvnSelchangedTreeConflictselecttaxiway(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
};
