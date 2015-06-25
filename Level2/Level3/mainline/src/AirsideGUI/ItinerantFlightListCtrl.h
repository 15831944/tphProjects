#ifndef ITIFLIGHT_H
#define ITIFLIGHT_H
#include "../MFCExControl/ListCtrlEx.h"
#include "../InputAirside/ItinerantFlight.h"
#include "resource.h"

class CItinerantFlightListCtrl: public CListCtrlEx
{
public:
	CItinerantFlightListCtrl();
	virtual ~CItinerantFlightListCtrl();

	void InsertItinerantFlightItem( int _nIdx, ItinerantFlight* _pItem );
	int InsertColumn(int nCol, const LV_COLUMNEX* pColumn);
	void SetEntryValue(CString strValue){m_strEntryValue = strValue;}
	void SetExitValue(CString strValue){m_strExitValue = strValue;}
protected:
	virtual void DrawItem( LPDRAWITEMSTRUCT lpdis);
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct) ;

protected:
	DECLARE_MESSAGE_MAP()
protected:
	CImageList m_shapesImageList;
	int m_nShapeSelItem;
	int m_nShapeSelSubItem;

	HBITMAP m_hBitmapCheck;
	CSize m_sizeCheck;
	CString m_strEntryValue;
	CString m_strExitValue;
};
#endif