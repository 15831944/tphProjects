#pragma once
#include "onboardobjectbasedlg.h"
#include "AirsideGUI/UnitPiece.h"

class COnboardPortal;
class CAircaftLayOut;
class CDlgOnboardPortal :
	public COnBoardObjectBaseDlg,public CUnitPiece
{
public:
	DECLARE_DYNAMIC(CDlgOnboardPortal)

public:
	CDlgOnboardPortal(COnboardPortal *pPortal,CAircaftLayOut *pLayout, int nProjID, CWnd* pParent = NULL);
	~CDlgOnboardPortal(void);

	virtual BOOL OnInitDialog();
protected:
	void LoadTree();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);




protected:
	int m_nProjID;
	CAircaftLayOut *m_pLayout;

	HTREEITEM m_hItemPosition;

protected:
	virtual FallbackReason GetFallBackReason();
	virtual void SetObjectPath(CPath2008& path);
	virtual bool UpdateOtherData();

	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);
	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);
	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
};
