#pragma once
#include "onboardobjectbasedlg.h"

#include "AirsideGUI/UnitPiece.h"

class CCabinDivider;
class CAircaftLayOut;

class COnBoardCabinDividerDlg :
	public COnBoardObjectBaseDlg ,public CUnitPiece
{
	DECLARE_DYNAMIC(COnBoardCabinDividerDlg)
public:
	COnBoardCabinDividerDlg(CCabinDivider *pDivider,CAircaftLayOut *pLayout, int nProjID, CWnd* pParent = NULL);
	~COnBoardCabinDividerDlg(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void LoadTree();
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);


	virtual FallbackReason GetFallBackReason();
	virtual void SetObjectPath(CPath2008& path);
	virtual bool UpdateOtherData();

	DECLARE_MESSAGE_MAP()


public:
	virtual BOOL OnInitDialog();

protected:
	HTREEITEM m_hItemThickness;
	HTREEITEM m_hItemPosition;
protected:
	int m_nProjID;
	CAircaftLayOut *m_pLayout;
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);





};
