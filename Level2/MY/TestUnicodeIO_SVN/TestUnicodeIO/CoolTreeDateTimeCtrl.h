#pragma once

class CCoolTreeDateTimeCtrl : public CDateTimeCtrl
{
public:
    CCoolTreeDateTimeCtrl(HTREEITEM hItem);
    ~CCoolTreeDateTimeCtrl(void);

public:
    void SetParentItem(HTREEITEM hItem){ m_hItem = hItem; }

protected:
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    DECLARE_MESSAGE_MAP()

private:
    HTREEITEM m_hItem;
};
