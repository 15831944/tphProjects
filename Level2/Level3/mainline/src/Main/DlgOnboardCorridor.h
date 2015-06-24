#pragma once
#include "onboardobjectbasedlg.h"
#include "AirsideGUI/UnitPiece.h"


class COnboardCorridor;
class CAircaftLayOut;

class CDlgOnboardCorridor :
	public COnBoardObjectBaseDlg,public CUnitPiece
{
public:
	DECLARE_DYNAMIC(CDlgOnboardCorridor)

public:
	CDlgOnboardCorridor(COnboardCorridor *pSureface,CAircaftLayOut *pLayout, int nProjID, CWnd* pParent = NULL);
	~CDlgOnboardCorridor(void);

	virtual BOOL OnInitDialog();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);
protected:
	void LoadTree();
	void InitTextureList();

protected:
	int m_nProjID;
	CAircaftLayOut *m_pLayout;
	
	
	HTREEITEM m_hItemPosition;
	CListCtrl m_wndTextureList;
	CImageList m_TextureImgList;
protected:
	virtual FallbackReason GetFallBackReason();
	virtual void SetObjectPath(CPath2008& path);
	virtual bool UpdateOtherData();

	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);
	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);
	
	virtual void OnOK();

	virtual afx_msg void OnLvnItemchangedListProp(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

};
