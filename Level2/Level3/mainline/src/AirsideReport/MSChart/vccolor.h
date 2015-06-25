#if !defined(AFX_VCCOLOR_H__737390D0_2794_457D_AA6A_1AFAB9D226D9__INCLUDED_)
#define AFX_VCCOLOR_H__737390D0_2794_457D_AA6A_1AFAB9D226D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.

/////////////////////////////////////////////////////////////////////////////
// CVcColor wrapper class

class CVcColor : public COleDispatchDriver
{
public:
	CVcColor() {}		// Calls COleDispatchDriver default constructor
	CVcColor(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CVcColor(const CVcColor& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	short GetRed();
	void SetRed(short nNewValue);
	short GetGreen();
	void SetGreen(short nNewValue);
	short GetBlue();
	void SetBlue(short nNewValue);
	BOOL GetAutomatic();
	void SetAutomatic(BOOL bNewValue);
	void Set(short Red, short Green, short Blue);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCCOLOR_H__737390D0_2794_457D_AA6A_1AFAB9D226D9__INCLUDED_)
