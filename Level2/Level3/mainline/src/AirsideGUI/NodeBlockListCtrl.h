#pragma once
#include "MFCExControl/XListCtrl.h"

class IntersectionNodeBlock;
class NodeBlockListCtrl: public CListCtrl
{
public:
	NodeBlockListCtrl(void);
	~NodeBlockListCtrl(void);

	virtual void DrawItem( LPDRAWITEMSTRUCT lpdis);
	void InsertNodeBlockItem( int _nIdx, IntersectionNodeBlock* _pItem );

protected:
	HBITMAP m_hBitmapCheck;
	CSize m_sizeCheck;

	void GetBlockRect(CRect& r);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};
