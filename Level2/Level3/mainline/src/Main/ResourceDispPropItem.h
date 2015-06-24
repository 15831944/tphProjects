// ResourceDispPropItem.h: interface for the CResourceDispPropItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCEDISPPROPITEM_H__551BD190_2433_4D65_9C95_A8E4F65E5647__INCLUDED_)
#define AFX_RESOURCEDISPPROPITEM_H__551BD190_2433_4D65_9C95_A8E4F65E5647__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <utility>

//associates a shape and color to a resource pool

class CResourceDispPropItem  
{
public:
	CResourceDispPropItem();
	CResourceDispPropItem(int _nResPoolIdx);
	CResourceDispPropItem( const CResourceDispPropItem& _rhs );
	virtual ~CResourceDispPropItem();

	int GetResourcePoolIndex() const { return m_nResourcePoolIdx; }
	BOOL IsVisible() const { return m_isVisible; }
	const std::pair<int, int>& GetShape() const { return m_shape; }
	COLORREF GetColor() const { return m_colorRef; }

	void SetResourcePoolIndex(int n) { m_nResourcePoolIdx=n; }
	void IsVisible(BOOL b) { m_isVisible=b; }
	void SetShape(const std::pair<int, int>& s) { m_shape=s; }
	void SetColor(COLORREF c) { m_colorRef=c; }
		
protected:
	int m_nResourcePoolIdx;
	BOOL m_isVisible;
	std::pair<int, int> m_shape;
	COLORREF m_colorRef;
};

#endif // !defined(AFX_RESOURCEDISPPROPITEM_H__551BD190_2433_4D65_9C95_A8E4F65E5647__INCLUDED_)
