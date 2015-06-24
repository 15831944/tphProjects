#pragma once
#include "onboardobjectbasedlg.h"
#include "AirsideGUI/UnitPiece.h"
#include <common/path.h>
class ARCPath3;
class COnBoardEscalator;
class CAircaftLayOut;


class COnBoardEscalatorDlg :
	public COnBoardObjectBaseDlg,public CUnitPiece
{
public:
	DECLARE_DYNAMIC(COnBoardEscalatorDlg)
public:
	COnBoardEscalatorDlg(COnBoardEscalator *pEscaltor,CAircaftLayOut *pLayout, int nProjID, CWnd* pParent = NULL);
	~COnBoardEscalatorDlg(void);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void LoadTree();
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);


	virtual FallbackReason GetFallBackReason();
	virtual void SetObjectPath(const ARCPath3& path);
	virtual bool UpdateOtherData();

	DECLARE_MESSAGE_MAP()


	void InsertPathToTree( const Path& path, HTREEITEM hParentItem );

	void InsertPointToTree( const Point& point, HTREEITEM hParentItem,const CString& strDeckName );

public:
	virtual BOOL OnInitDialog();

protected:
	HTREEITEM m_hItemServiceLocation;
	HTREEITEM m_hItemWidth;
protected:
	int m_nProjID;
	CAircaftLayOut *m_pLayout;
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);

	void DefineZPos();


};
