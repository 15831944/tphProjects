#pragma once
#include "ConDBExListCtrl.h"
#include "DocDisplayTimeListCtrl.h"
// CDlgDocDisplayTime dialog
class CDocDisplayTimeDB ;
class InputTerminal;
class CDocDisplayTimeEntry;
class CPassengerType;
class CDlgDocDisplayTime : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgDocDisplayTime)

public:
	CDlgDocDisplayTime(InputTerminal* _InputTerminal ,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDocDisplayTime();

// Dialog Data
	enum { IDD = IDD_DIALOG_DOCDISPLAYTIME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
protected:
	CDocDisplayTimeListCtrl	m_listctrlData;
   CToolBar m_wndToolbar ;
   CStatic m_sta_tool ;
   InputTerminal* m_terminal ;
   CDocDisplayTimeDB* m_TimeDB ;
public:
   afx_msg void OnSize(UINT nType, int cx, int cy) ;
    BOOL OnInitDialog() ;

   virtual void OnCancel();
   virtual void OnOK();
   int OnCreate(LPCREATESTRUCT lpCreateStruct) ;
protected:
	void InitList() ;
	void InitToobar() ;
protected:
	void InitListView() ;
	void InitListData() ;
	void InsertItemToList(CDocDisplayTimeEntry* _entry,CPassengerType* _paxTY) ;
public:
	void OnDelDocDisplayTime() ;
	void OnNewDocDisplayTime() ;
	DECLARE_MESSAGE_MAP()
};
