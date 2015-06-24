#pragma once
#include "airsideobjectbasedlg.h"
#include "../AirsideGUI/UnitPiece.h"

class CUnitPiece;
class Surface;
class CAirsideSurfaceDlg :
	public CAirsideObjectBaseDlg , public CUnitPiece
{
	DECLARE_DYNAMIC(CAirsideSurfaceDlg)
public:
	CAirsideSurfaceDlg(int nSurfaceID ,int nAirportID,int nProjID,CWnd *pParent = NULL);
	//CAirsideSurfaceDlg(Surface* pSurface, int nProjID,CWnd * pParent = NULL);
	~CAirsideSurfaceDlg(void);

protected:
	HTREEITEM m_hServiceLocation;
	
	void LoadTree();
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);
	CListCtrl m_wndTextureList;
	CImageList m_TextureImgList;
	ULONG_PTR	m_gdiplusToken;
	//bool m_bPathModified;

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);

	virtual FallbackReason GetFallBackReason();
	virtual void SetObjectPath(CPath2008& path);
	virtual bool UpdateOtherData();
	DECLARE_MESSAGE_MAP()

	void InitTextureList();

public:
	virtual BOOL OnInitDialog();
	virtual void OnLvnItemchangedListProp(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	afx_msg void OnSize( UINT, int, int );
};
