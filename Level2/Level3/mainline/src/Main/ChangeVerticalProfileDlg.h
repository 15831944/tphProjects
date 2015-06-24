#if !defined(AFX_CHANGEVERTICALPROFILEDLG_H__D8A8AC42_999A_4F0B_9262_F26665BE0AE9__INCLUDED_)
#define AFX_CHANGEVERTICALPROFILEDLG_H__D8A8AC42_999A_4F0B_9262_F26665BE0AE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangeVerticalProfileDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChangeVerticalProfileDlg dialog
#include "PtLineChart.h"
#include "common\path.h"
#include <vector>
#include "../inputs/IN_TERM.H"
class CChangeVerticalProfileDlg : public CDialog
{
// Construction
public:
	CChangeVerticalProfileDlg(std::vector<double>&vXPos,std::vector<double>&vZPos,std::vector<double>&vHeights, const Path& _path,CStringArray* pStrArrFloorName,EnvironmentMode envMode = EnvMode_Terminal ,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChangeVerticalProfileDlg)
	enum { IDD = IDD_DIALOG_VERTICAL_PROFILE_EDITOR };
	CStatic	m_staticAngle;
	CStatic	m_staticLen;
	CEdit	m_editLength;
	CEdit	m_editAngle;
	CButton	m_btnCancel;
	CButton	m_btnOK;
	CPtLineChart	m_wndPtLineChart;

	CSpinButtonCtrl m_spinStartSlope; 
	CSpinButtonCtrl m_spinEndSlope;

	int m_nStartSlope;
	int m_nEndSlope;

	int m_nMaxRange;
	//}}AFX_DATA
	void Resize(int cx,int cy);
	std::vector<double>m_vXPos;
	std::vector<double>m_vZPos;
	std::vector<double>m_vHeights;
	Path m_Path;
	CStringArray* m_pStrArrFloorName;
	EnvironmentMode m_envMode;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeVerticalProfileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool OnOkOnBoard();

	// Generated message map functions
	//{{AFX_MSG(CChangeVerticalProfileDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	virtual void OnOK();
	//}}AFX_MSG
	afx_msg LRESULT UpdateLengthAngle( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedStaticLen2();
	afx_msg void OnChangeEditStartslope();
	afx_msg void OnChangeEditEndslope();
	afx_msg void OnPlay();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEVERTICALPROFILEDLG_H__D8A8AC42_999A_4F0B_9262_F26665BE0AE9__INCLUDED_)
