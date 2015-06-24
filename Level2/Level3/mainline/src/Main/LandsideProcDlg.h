#pragma once
// 
// #include "afxcmn.h"
// #include "TreeCtrlEx.h"
// #include "afxwin.h"
#include "../Common/ARCPath.h"
#include "../Common/ARCVector.h"
#include "ARCLayoutObjectDlg.h"
#include "AirsideObjectTreeCtrl.h"
#include "LayoutObjectPropDlgImpl.h"
// CLandsideProcDlg dialog
class LandsideFacilityLayoutObject;
class ControlPath;

class CLandsideProcDlg : public CARCLayoutObjectDlg
{
	DECLARE_DYNAMIC(CLandsideProcDlg)

public:
	CLandsideProcDlg(LandsideFacilityLayoutObject * pProc, CWnd* pParent = NULL);   // standard constructor
	virtual ~CLandsideProcDlg();	

	virtual void SetObjectLock(bool b);
	virtual bool IsObjectLocked()const;
	virtual void InitData();
	virtual int GetProjectID();
	virtual FallbackReason GetFallBackReason();	

	virtual void LoadTree();
	void LoadTreeSubItemCtrlPoints(HTREEITEM preItem,const ControlPath& path );
	void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);
	virtual void DonePickPoints(const CPath2008& pickpoints );
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()	
public:
	// Generated message map functions
	//{{AFX_MSG(CLandsideProcDlg)

	//}}AFX_MSG
	//void InitComboBox();	
};

