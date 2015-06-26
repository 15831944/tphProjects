
#pragma once
//#include "../MFCExControl/resource.h"
#include "CoolTree.h"
#include "../MFCExControl/CARCTipEdit.h"

class MFCEXCONTROL_API CCoolTreeEditSpin : public CDialog
{
// Construction
public:
	void SetSpinRange(int nMin,int nMax);
	CWnd* m_pParent;
	CCoolTreeEditSpin(CWnd* pParent = NULL);   // standard constructor
	CWnd* SetParent( CWnd* pWndNewParent );

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//enum { IDD = IDD_COOLTREE_EDITSPIN };
	CSpinButtonCtrl	m_spinValue;
	CEdit	m_editValue;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKillfocusEdit1();

private:
	BOOL m_bESC;
};
