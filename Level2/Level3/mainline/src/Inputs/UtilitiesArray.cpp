// UtilitiesArray.cpp: implementation of the CUtilitiesArray class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UtilitiesArray.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUtilitiesArray::CUtilitiesArray()
{
	m_Array.RemoveAll();
}

CUtilitiesArray::~CUtilitiesArray()
{

}

BOOL CUtilitiesArray::ClearAt(int nIndex)
{
	ASSERT(nIndex >= 0);
	if(nIndex < 0 || nIndex >= m_Array.GetSize())
		return FALSE;
	
	CUtilitiesItem ui;
	m_Array.SetAt(nIndex, ui);
	
	return TRUE;
}
