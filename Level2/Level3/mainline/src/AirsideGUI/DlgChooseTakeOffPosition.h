#pragma once
#include "../InputAirside/TakeoffSequencing.h"
#include "afxcmn.h"

// CDlgChooseTakeOffPosition dialog

class CDlgChooseTakeOffPosition : public CDialog
{
	DECLARE_DYNAMIC(CDlgChooseTakeOffPosition)

public:
	CDlgChooseTakeOffPosition(TakeoffSequencing * pTakeOffSeq,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChooseTakeOffPosition(void); 
	// Dialog Data
	enum { IDD = IDD_SELECTTAKEOFFPOSITION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	std::vector<TakeOffPositionInfo *> m_vrSelItemInfo;
protected:
	TakeoffSequencing * m_pTakeOffSeq;
	CTreeCtrl m_wndTakeoffPosiTree;
protected:
	void SetTreeContents(void);
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
