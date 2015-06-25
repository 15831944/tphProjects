#pragma once
#include "afxcmn.h"


// CDlgOccupiedSelectStand dialog

class CDlgOccupiedSelectStand : public CDialog
{
	DECLARE_DYNAMIC(CDlgOccupiedSelectStand)
public:
	typedef std::vector<std::pair<CString,int> > AltObjectVector;
	typedef std::vector<std::pair<CString,int> >::iterator AltObjectVectorIter;

	typedef std::map<CString, AltObjectVector> AltObjectVectorMap;
	typedef std::map<CString, AltObjectVector>::iterator AltObjectVectorMapIter;

public:
	CDlgOccupiedSelectStand(AltObjectVectorMap* pStandVectorMap, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgOccupiedSelectStand();
	int GetStandID(){return m_nStandID;}
	CString GetStandName(){return m_strStandName;}

// Dialog Data
	enum { IDD = IDD_DIALOG_OCCUPIEDSELECTSTAND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CTreeCtrl m_wndSelectStandTree;
	AltObjectVectorMap* m_pStandVectorMap;
	int m_nStandID;
	CString m_strStandName;
	void LoadTree();
public:
	afx_msg void OnTvnSelchangedTreeConflictselectStand(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
