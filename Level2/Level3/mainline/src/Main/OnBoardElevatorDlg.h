#pragma once

#pragma once
#include "onboardobjectbasedlg.h"
#include "AirsideGUI/UnitPiece.h"
#include <common/path.h>
class ARCPath3;
class COnBoardEscalator;
class CAircaftLayOut;

class COnBoardElevator;
class CAircaftLayOut;
class COnBoardElevatorDlg :
	public COnBoardObjectBaseDlg,public CUnitPiece
{
public:
	DECLARE_DYNAMIC(COnBoardElevatorDlg)
public:
	COnBoardElevatorDlg(COnBoardElevator *pElevator,CAircaftLayOut *pLayout, int nProjID, CWnd* pParent = NULL);
	~COnBoardElevatorDlg(void);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void LoadTree();
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);


	virtual FallbackReason GetFallBackReason();
	virtual bool UpdateOtherData();

	DECLARE_MESSAGE_MAP()


public:
	virtual BOOL OnInitDialog();
	HTREEITEM m_hLiftWidth;
	HTREEITEM m_hLiftLength;
	HTREEITEM m_hLiftNOL;
	HTREEITEM m_hWaitAreaWidth;
	HTREEITEM m_hWaitAreaLength;
	HTREEITEM m_hWaitAreaPos;
	HTREEITEM m_hMaxFloor;
	HTREEITEM m_hMinFloor;
	HTREEITEM m_hLift;
	HTREEITEM m_hWaitArea;

	HTREEITEM m_hItemServiceLocation;
protected:
	
protected:
	int m_nProjID;
	CAircaftLayOut *m_pLayout;
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);

	void SetObjectPath(const ARCPath3& path);
	afx_msg void OnDefineToggled();


	void InsertPathToTree( const Path& path, HTREEITEM hParentItem );

	void InsertPointToTree( const Point& point, HTREEITEM hParentItem,const CString& strDeckName );
};
