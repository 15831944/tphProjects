#pragma once
#include "afxdtctl.h"

#include "Common/elaptime.h"
#include "AirsideGUIAPI.h"

// CDlgTimePicker dialog

class AIRSIDEGUI_API CDlgTimePicker : public CDialog
{
	DECLARE_DYNAMIC(CDlgTimePicker)

public:
	CDlgTimePicker(ElapsedTime eTime,CString strCaption = _T(""), CWnd* pParent = NULL,bool bSecFormat = false);   // standard constructor
	virtual ~CDlgTimePicker();

	ElapsedTime GetTime() const { return m_time; }


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();


	DECLARE_MESSAGE_MAP()

private:
	ElapsedTime m_time;
	bool m_bSecFormat;
	CString m_strCaption;
	CDateTimeCtrl m_timePicker;
};
