#pragma once
#include <afxwin.h>

class CInPlaceList : public CComboBox
{
public:
    CInPlaceList(int iItem, int iSubItem, CStringList *plstItems, int nSel);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual ~CInPlaceList();

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnNcDestroy();
    afx_msg void OnCloseup();
    DECLARE_MESSAGE_MAP()

private:
    int  m_iItem;
    int  m_iSubItem;
    CStringList* m_lstItems;
    int  m_nSel;
    BOOL m_bESC;    // To indicate whether ESC key was pressed
};