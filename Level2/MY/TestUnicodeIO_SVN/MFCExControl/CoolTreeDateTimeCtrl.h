#pragma once

class MFCEXCONTROL_API CCoolTreeDateTimeCtrl :public CDateTimeCtrl
{
public:
	CCoolTreeDateTimeCtrl(HTREEITEM hItem);
	~CCoolTreeDateTimeCtrl(void);

public:
	void SetParentItem(HTREEITEM hItem)     {     m_hItem = hItem;    }

protected:
	//{{AFX_MSG(CTreeComboBox)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	HTREEITEM m_hItem;
};
