#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "Resource.h"

// CDlgSelectExitOfRunway dialog

class IExitOfRunwayFilter
{
public:
	virtual BOOL IsAllowed(int nExitID, CString& strError) = 0;
};

class CDlgSelectExitOfRunway : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectExitOfRunway)

public:
	CDlgSelectExitOfRunway(int nRunwayID, int nRunwayMarkIndex, IExitOfRunwayFilter* pExitFilter = NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectExitOfRunway();

	int GetSelExitID() const { return m_nSelExitID; }
	CString GetSelExitName() { return m_SelExitName ;} ;
// Dialog Data
	enum { IDD = IDD_DIALOG_EXITS_SEL };

protected:
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	void ResetListBoxContent();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CListBox m_listExits;

	IExitOfRunwayFilter* m_pExitFilter;

	int m_nRunwayID;
	int m_nRunwayMarkIndex;
	int m_nSelExitID;
	CString m_SelExitName ;
};
