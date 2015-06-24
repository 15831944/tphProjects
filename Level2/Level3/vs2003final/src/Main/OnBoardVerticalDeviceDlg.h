#pragma once
#include "onboardobjectbasedlg.h"
#include "AirsideGUI/UnitPiece.h"

class COnBoardBaseVerticalDevice;
class CAircaftLayOut;
class CPoint2008;
class COnBoardVerticalDeviceDlg :	public COnBoardObjectBaseDlg, public CUnitPiece
{
	DECLARE_DYNAMIC(COnBoardVerticalDeviceDlg)
public:
	COnBoardVerticalDeviceDlg(COnBoardBaseVerticalDevice *pVerticalDevice,CAircaftLayOut *pLayout, int nProjID, CWnd* pParent);
	~COnBoardVerticalDeviceDlg(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


	void LoadTree();
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);


	virtual FallbackReason GetFallBackReason();
	virtual void SetObjectPath(CPath2008& path);
	virtual bool UpdateOtherData();

	void InsertPathToTree(const CPath2008& point, HTREEITEM hParentItem,const CString& strDeckName);
	void InsertServicePathToTree(HTREEITEM hParentItem,COnBoardBaseVerticalDevice *pVerticalDevice);
	void InsertPointToTree(const CPoint2008& point, HTREEITEM hParentItem,const CString& strDeckName);

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();


protected:
	HTREEITEM m_hItemServiceLocation;
	HTREEITEM m_hItemQueue;
	HTREEITEM m_hItemInCons;
	HTREEITEM m_hItemOutCons;

protected:
	int m_nProjID;
	CAircaftLayOut *m_pLayout;
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);

	virtual void OnDeletePath();
	void ToggleQueueFixed();
	void DefineZPos();
	


};
