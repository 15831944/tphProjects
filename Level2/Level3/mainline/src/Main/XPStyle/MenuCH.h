#include "afxtempl.h"
#if !defined(CMenuCH_h)
#define CMenuCH_h

#define MIT_ICON	 1
#define MIT_COLOR	 2
#define MIT_XP		 3

class CMenuItemEx
{
public:
	CMenuItemEx(LPCSTR strText,CBitmap* pBitmap=NULL,HICON hIcon=NULL,int nBmpIndex=-1)
	{
		lstrcpy(m_szText,strText);
		m_pBitmap = pBitmap;
		m_hIcon   = hIcon;
		m_nBmpIndex=nBmpIndex;
	}
public:
	char	  m_szText[128];		// message text 
	char*     m_pszHotkey;			// hotkey text
	CBitmap*  m_pBitmap;	
	int m_nBmpIndex;
	HICON	  m_hIcon;
	int		  m_nWidth,			
		      m_nHeight;
};
class CMenuCH:public CMenu
{
public:
	CMenuCH();
	~CMenuCH();
public:
	void SetMenuWidth(DWORD width);
	void SetMenuHeight(DWORD height);
	void SetMenuType(UINT nType);
	BOOL AppendMenu(UINT nFlags,UINT nIDNewItem,LPCSTR lpszNewItem,
					UINT nIDBitmap=NULL,HICON hIcon=NULL);
protected:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	void DrawXPMenu(LPDRAWITEMSTRUCT lpDIS);
	void DrawColorMenu(LPDRAWITEMSTRUCT lpDIS);
	void DrawIconMenu(LPDRAWITEMSTRUCT lpDIS);
	void DrawCheckMark(CDC* pDC,int x,int y,COLORREF color);
protected:
	UINT        m_nType;
	DWORD 		m_Width,
				m_Height;
	CTypedPtrList<CPtrList,CMenuItemEx *> m_MenuList;
public:
	int m_nMaxWidthStr;
	void SetBmpSize(int cx,int cy);
	int m_nBmpWidth;
	int m_nBmpHeight;
	CImageList m_imageList;
	BOOL AppendMenu(UINT nFlags,UINT nIDNewItem,LPCSTR lpszNewItem,HBITMAP hBitmap);
	COLORREF	m_SelColor;
	int			m_curSel;
};
#endif