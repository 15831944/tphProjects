#pragma once


// DlgAnimationSpeed dialog

class DlgAnimationSpeed : public CDialog
{
	DECLARE_DYNAMIC(DlgAnimationSpeed)

public:
	DlgAnimationSpeed(int iAnimationSpeed, CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgAnimationSpeed();


	int getAnimationSpeed() const;
protected:
	int m_nAnimationSpeed;
	// Dialog Data
	enum { IDD = IDD_DIALOG_ANIMATIONSPEED };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEdit1();
};
