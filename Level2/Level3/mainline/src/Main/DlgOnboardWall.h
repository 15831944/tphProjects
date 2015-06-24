#pragma once
#include "onboardobjectbasedlg.h"
#include "AirsideGUI/UnitPiece.h"

class COnBoardWall;
class CAircaftLayOut;

class CDlgOnboardWall :
	public COnBoardObjectBaseDlg,public CUnitPiece
{
public:
	DECLARE_DYNAMIC(CDlgOnboardWall)

public:
	CDlgOnboardWall(COnBoardWall *pWall,CAircaftLayOut *pLayout, int nProjID, CWnd* pParent = NULL);
	~CDlgOnboardWall(void);

public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);


protected:
	void LoadTree();

protected:
	int m_nProjID;
	CAircaftLayOut *m_pLayout;


protected:
	HTREEITEM m_hItemPosition;
	HTREEITEM m_hItemWidth;
	HTREEITEM m_hItemHeight;

//	CPath2008 m_Wallpath;
//	double m_dWidth;
//	double m_dheight;
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
