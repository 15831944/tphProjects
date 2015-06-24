#pragma once
#include "treectrlex.h"
#include "afxwin.h"
#include "../Common/NewPassengerType.h"
#include "../MFCExControl/XTResizeDialog.h"
class Terminal ;
// CDlgNewPassengerType dialog

class CDlgNewPassengerType : public CXTResizeDialog
{
	DECLARE_DYNCREATE(CDlgNewPassengerType)

public:
	CDlgNewPassengerType(CWnd* pParent = NULL);
	CDlgNewPassengerType(CPassengerType* _paxType ,InputTerminal* _terminal ,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNewPassengerType();
// Overrides
    virtual void OnOK() ;
	virtual void OnButtonCancel() ;

// Dialog Data
	enum { IDD = IDD_DIALOG_NEW_PAXTYPE };
private:
	CPassengerType* m_paxType ;
	InputTerminal* m_terminal ;
	CString m_strOrigin;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	CCoolTree m_PaxTyTree;
	CStatic m_Static_paxTY;
public:
	void SetTreeNodeText(HTREEITEM Item) ;
	void InitLevelTree() ;
	HTREEITEM InsertTreeNode(CString _itemText,int level ) ;
	void InitComboString(CComboBox* _combox , int _level) ;
	//void SetLevelData(HTREEITEM _item,int _level) ;
	void InitPaxTypeText() ;
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;
	//afx_msg void OnTvnSelchangedTreePaxtypeLevel(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnNMRclickTreePaxtypeLevel(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnNMClickTreePaxtypeLevel(NMHDR *pNMHDR, LRESULT *pResult);
	void OnSize(UINT nType, int cx, int cy) ;
};
