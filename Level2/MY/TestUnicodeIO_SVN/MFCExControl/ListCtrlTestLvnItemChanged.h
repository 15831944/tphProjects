#pragma once

class CListCtrlTestLvnItemChanged : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlTestLvnItemChanged)

public:
	CListCtrlTestLvnItemChanged();
	virtual ~CListCtrlTestLvnItemChanged();
    afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	DECLARE_MESSAGE_MAP()
};


