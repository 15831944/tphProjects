#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CDlgACScaleSize dialog

class CDlgACScaleSize : public CDialog
{
	DECLARE_DYNAMIC(CDlgACScaleSize)

public:
	CDlgACScaleSize(CWnd* pParent/* = NULL*/,int fScaleRange);   // standard constructor
	virtual ~CDlgACScaleSize();
	
	void SetScale(float _scale){ m_fScaleSize = _scale; }
	float GetScale(){ return m_fScaleSize; }
// Dialog Data
	enum { IDD = IDD_DIALOG_ACSCALESIZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	int m_nScaleRange;
public:
	CEdit m_editScale;
	float m_fScaleSize;
	CSpinButtonCtrl m_spinScale;
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaPosSpinDensity(NMHDR* pNMHDR, LRESULT* pResult);

};
