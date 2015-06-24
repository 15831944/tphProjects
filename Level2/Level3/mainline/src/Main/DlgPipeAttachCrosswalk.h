#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "../MFCExControl/XTResizeDialog.h"

class CrossWalkAttachPipe;
class CPipeAttachCrosswalkListCtrl: public CListCtrl
{
public:
	CPipeAttachCrosswalkListCtrl();
	virtual ~CPipeAttachCrosswalkListCtrl();

	void InsertItem(int _nIdx,CrossWalkAttachPipe* pCrossPipe);
	void SetEndIndex(int nEndIndex);
protected:
	virtual void DrawItem( LPDRAWITEMSTRUCT lpdis);
	BOOL GetItemCheck(int nItem);
	void SetItemCheck(int nItem,BOOL bCheck);
	afx_msg LRESULT OnCheckStateChanged( WPARAM wParam, LPARAM lParam );
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
protected:
	HBITMAP m_hBitmapCheck;
	CSize m_sizeCheck;
	int m_nIdx;
};

// CDlgPipeAttachCrosswalk dialog
class CPipe;
class InputTerminal;
class CDlgPipeAttachCrosswalk : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgPipeAttachCrosswalk)

public:
	CDlgPipeAttachCrosswalk(CPipe* pPipe,int nEndIndex,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPipeAttachCrosswalk();

// Dialog Data
	enum { IDD = IDD_DIALOG_PIPEATTACHCROSSWALK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();
	void OnOK();
	void OnCancel();

	DECLARE_MESSAGE_MAP()

public:
	void InitListCtrlHead();
	void SetListCtrlContent();

private:
	InputTerminal* GetInputTerminal();
	CString GetProjPath();
private:
	CPipeAttachCrosswalkListCtrl m_wndListCtrl;
	CPipe* m_pPipe;
	int m_nIdx;
};
