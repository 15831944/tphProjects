#pragma once
#include "afxwin.h"
#include "../Common/AirsideFlightType.h"

// DlgNewFlightType dialog

class DlgNewFlightType : public CDialog
{
	DECLARE_DYNAMIC(DlgNewFlightType)

public:
	DlgNewFlightType(const AirsideFlightType& fltType, CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgNewFlightType();

// Dialog Data
	enum { IDD = IDD_DLGNEWFLIGHTTYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckArrtodep();
	afx_msg void OnBnClickedButtonCnst1();
	afx_msg void OnBnClickedButtonCnst2();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	const AirsideFlightType& GetFltType(); 

private:
	CButton m_btnArrToDep;
	AirsideFlightType m_fltType;
};


#define DECLARE_NEWFLIGHTTYPE_SELECT_CALLBACK()									\
	static BOOL SelectNewFlightType(CWnd* pParent,AirsideFlightType& fltType)	\
{																				\
	DlgNewFlightType flightDlg(fltType,pParent);								\
	if(IDOK == flightDlg.DoModal())												\
	{																			\
		fltType = flightDlg.GetFltType();										\
		return TRUE;															\
	}																			\
	return FALSE;																\
}			
