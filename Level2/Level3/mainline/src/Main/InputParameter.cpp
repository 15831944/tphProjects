// InputParameter.cpp: implementation of the CInputParameter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InputParameter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInputParameter::CInputParameter()
{

}

CInputParameter::~CInputParameter()
{

}

BOOL CInputParameter::LoadData( const CString& strPath )
{
	if(strPath == "")
		return FALSE;

	return TRUE;
}

BOOL CInputParameter::Save( const CString& strPath )
{
	if(strPath == "")
		return FALSE;

	return TRUE;
}
