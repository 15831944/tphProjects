// UtilitiesArray.h: interface for the CUtilitiesArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTILITIESARRAY_H__C97C6036_0E57_4A37_8CED_B296D42AE5D0__INCLUDED_)
#define AFX_UTILITIESARRAY_H__C97C6036_0E57_4A37_8CED_B296D42AE5D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

#include "UtilitiesItem.h"

class CUtilitiesArray  
{
public:
	CUtilitiesArray();
	virtual ~CUtilitiesArray();

	BOOL ClearAt(int nIndex);
	
// Data
//protected:
public:
	CArray<CUtilitiesItem, CUtilitiesItem&> m_Array;
};

#endif // !defined(AFX_UTILITIESARRAY_H__C97C6036_0E57_4A37_8CED_B296D42AE5D0__INCLUDED_)
