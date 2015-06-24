#pragma once
#include "afxwin.h"
#include <common/Path2008.h>

// CDlgSeatArraySpec dialog
class CSeatGroup;
class CAircraftPlacements;

class CDlgSeatArraySpec : public CDialog
{
	DECLARE_DYNAMIC(CDlgSeatArraySpec)

public:
	CDlgSeatArraySpec(CSeatGroup* pseatGroup ,CAircraftPlacements* placements, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSeatArraySpec();

// Dialog Data
	enum { IDD = IDD_DIALOG_SEAT_ARRAYSPEC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonDrawline();
	afx_msg void OnBnClickedCancel();

	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);
	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);
	CEdit m_editName;
	CEdit m_editSeatPitch;
	CEdit m_editrowNum;
	
	CButton m_radioNumRow;

	CPath2008 m_points;
	CButton m_radioPerToLongAxis;

	bool UpdateDataModify();
	CSeatGroup* m_pSeatGroup;
	CAircraftPlacements* m_pPlacements;
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioNumrow();
	afx_msg void OnBnClickedRadioFitrow();
};
