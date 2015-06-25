#if !defined(AFX_VCDATAPOINTLABEL_H__DF5534F6_6765_4B17_B20A_966374A4DA37__INCLUDED_)
#define AFX_VCDATAPOINTLABEL_H__DF5534F6_6765_4B17_B20A_966374A4DA37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


// Dispatch interfaces referenced by this interface
class CVcBackdrop;
class CVcFont;
class CVcLCoor;
class CVcTextLayout;
class COleFont;

/////////////////////////////////////////////////////////////////////////////
// CVcDataPointLabel wrapper class

class CVcDataPointLabel : public COleDispatchDriver
{
public:
	CVcDataPointLabel() {}		// Calls COleDispatchDriver default constructor
	CVcDataPointLabel(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CVcDataPointLabel(const CVcDataPointLabel& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetText();
	void SetText(LPCTSTR lpszNewValue);
	CVcBackdrop GetBackdrop();
	CVcFont GetVtFont();
	CVcLCoor GetOffset();
	CVcTextLayout GetTextLayout();
	long GetLocationType();
	void SetLocationType(long nNewValue);
	BOOL GetCustom();
	void SetCustom(BOOL bNewValue);
	short GetComponent();
	void SetComponent(short nNewValue);
	long GetLineStyle();
	void SetLineStyle(long nNewValue);
	CString GetValueFormat();
	void SetValueFormat(LPCTSTR lpszNewValue);
	CString GetPercentFormat();
	void SetPercentFormat(LPCTSTR lpszNewValue);
	short GetTextLength();
	void SetRefFont(LPDISPATCH newValue);
	COleFont GetFont();
	void ResetCustomLabel();
	void Select();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCDATAPOINTLABEL_H__DF5534F6_6765_4B17_B20A_966374A4DA37__INCLUDED_)
