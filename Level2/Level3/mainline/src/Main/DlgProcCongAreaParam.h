#pragma once


// CDlgProcCongAreaParam dialog

class CDlgProcCongAreaParam : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcCongAreaParam)

public:
	CDlgProcCongAreaParam(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgProcCongAreaParam();

	void SetLambdaValue(double dLambda){ m_dLambda = dLambda; }
	double GetLambdaValue() const { return m_dLambda; }

	void SetTriggerValue(double dTrigger){ m_dTrigger = dTrigger; }
	double GetTriggerValue() const { return m_dTrigger; }
	// Dialog Data
	enum { IDD = IDD_DIALOG_PROCCONGAREAPARAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_dLambda;
	double m_dTrigger;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
