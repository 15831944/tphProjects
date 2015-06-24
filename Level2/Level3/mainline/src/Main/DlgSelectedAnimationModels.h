#pragma once


// CDlgSelectedAnimationModels dialog
class AnimationData;
class CDlgSelectedAnimationModels : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectedAnimationModels)

public:
	CDlgSelectedAnimationModels(AnimationData &amData,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectedAnimationModels();

// Dialog Data
	enum { IDD = IDD_DIALOG_SELECTEDANIMAMODE };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:	
	AnimationData *m_pAnimData;
	BOOL m_bTerminalMode;
	BOOL m_bAirsideMode;
	BOOL m_bLandsideMode;
	BOOL m_bOnBoardMode;


	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();


};
