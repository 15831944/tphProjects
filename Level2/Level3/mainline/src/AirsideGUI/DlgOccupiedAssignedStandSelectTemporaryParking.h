#pragma once
#include "afxcmn.h"


// CDlgOccupiedSelectTemporaryParking dialog

class CDlgOccupiedSelectTemporaryParking : public CDialog
{
	DECLARE_DYNAMIC(CDlgOccupiedSelectTemporaryParking)
public:
	typedef std::vector<std::pair<CString,int> > AltObjectVector;
	typedef std::vector<std::pair<CString,int> >::iterator AltObjectVectorIter;

	typedef std::map<CString, AltObjectVector> AltObjectVectorMap;
	typedef std::map<CString, AltObjectVector>::iterator AltObjectVectorMapIter;

public:
	CDlgOccupiedSelectTemporaryParking(AltObjectVectorMap* pTemporaryParkingVectorMap, CWnd* pParent = NULL);   // TemporaryParking constructor
	virtual ~CDlgOccupiedSelectTemporaryParking();
	int GetTemporaryParkingID(){return m_nTemporaryParkingID;}
	CString GetTemporaryParkingName(){return m_strTemporaryParkingName;}

// Dialog Data
	enum { IDD = IDD_DIALOG_OCCUPIEDSELECTTEMPORARYPARKING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CTreeCtrl m_wndSelectTemporaryParkingTree;
	AltObjectVectorMap* m_pTemporaryParkingVectorMap;
	int m_nTemporaryParkingID;
	CString m_strTemporaryParkingName;
	void LoadTree();
public:
	afx_msg void OnTvnSelchangedTreeConflictselectTemporaryParking(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
