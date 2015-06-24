#pragma once
#include <vector>
#include "../Common/Path2008.h"
#include "afxcmn.h"
#include "PtLineChartEx.h"
using namespace std;

// CAirsideStretchVerticalProfileEditor dialog

class CAirsideStretchVerticalProfileEditor : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CAirsideStretchVerticalProfileEditor)

public:
	CAirsideStretchVerticalProfileEditor(vector<double>&vXPos, vector<double>&vZPos, const CPath2008& _path, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAirsideStretchVerticalProfileEditor();

// Dialog Data
	enum { IDD = IDD_DIALOG_AIRSIDEVERTICAL_PROFILE_EDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg LRESULT UpdateSliderCtrlPos( WPARAM wParam, LPARAM lParam );
	//wParam:distance, lParam: Height
	afx_msg LRESULT UpdatePointDistanceAndHeight( WPARAM wParam, LPARAM lParam );
public:
	CSliderCtrl m_SliderCtrl;
	CPtLineChartEx	m_wndPtLineChart;

	vector<double> m_vXPos;
	vector<double> m_vZPos;
	CPath2008      m_Path;
	CString        m_strHeight;
	CString        m_strDistance;
};
