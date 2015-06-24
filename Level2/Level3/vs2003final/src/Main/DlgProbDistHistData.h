#if !defined(AFX_DLGPROBDISTHISTDATA_H__FB3875A1_F3E1_498F_A13D_6838576A3FE7__INCLUDED_)
#define AFX_DLGPROBDISTHISTDATA_H__FB3875A1_F3E1_498F_A13D_6838576A3FE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgProbDistHistData.h : header file
//
#include "ListCtrlEx2.h"
#include <vector>
#include "XPStyle\NewMenu.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgProbDistHistData dialog
typedef std::pair<double, double> pair_range;

class CDlgProbDistHistData : public CDialog
{
// Construction
public:
	CDlgProbDistHistData(CWnd* pParent = NULL);   // standard constructor

	std::vector<double>* GetValueVectorPtr() { return m_pvdbValue; }
	void SetValueVectorPtr(std::vector<double>* pvdbValue) { m_pvdbValue = pvdbValue; }

	std::vector<pair_range>* GetRangeVectorPtr() { return m_pvPairRange; }
	void SetRangeVectorPtr(std::vector<pair_range>* pvPairRange) { m_pvPairRange = pvPairRange;	}

	void SetType(UINT nType) { m_nType = nType;	}
	UINT GetType() const { return m_nType; }
// Dialog Data
	//{{AFX_DATA(CDlgProbDistHistData)
	enum { IDD = IDD_DIALOG_PROBDIST3 };
	CListCtrlEx2	m_lstHistFileData;
	UINT	m_nRecordCount;
	CString	m_strFileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProbDistHistData)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	std::vector<double>* m_pvdbValue;
	std::vector<pair_range>* m_pvPairRange;
	UINT m_nType;
	CNewMenu m_nMenu, *m_pSubMenu;
	int m_nSelected;

	int m_nSortColumn;

	CWnd* EditSubItem(int nItem, int nSubItem);

	void SortByColumn(int nSortCol);
	static int CALLBACK CompareFuncAscending(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK CompareFuncDescending(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	void LoadProbDistHistList();

	void AddDataToListCtrl();
	
	void InitListColumns();

	int AddListItems();

	int ValueInRange(int nValue);
	// Generated message map functions
	//{{AFX_MSG(CDlgProbDistHistData)
	afx_msg void OnButtonBrowse();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnCtxHistogramAdd();
	afx_msg void OnCtxHistogramModify();
	afx_msg void OnCtxHistogramDelete();
	afx_msg void OnCtxHistogramDeleteall();
	afx_msg void OnRclickListHistfiledata(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnItemchangingListHistfiledata(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditListHistfiledata(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROBDISTHISTDATA_H__FB3875A1_F3E1_498F_A13D_6838576A3FE7__INCLUDED_)
