// FlowBelt.h: interface for the CFlowBelt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLOWBELT_H__0B32BE03_3F78_46BD_8F87_3D4BF7954D1C__INCLUDED_)
#define AFX_FLOWBELT_H__0B32BE03_3F78_46BD_8F87_3D4BF7954D1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Loader.h"

class CFlowBelt : public CLoader  
{
public:
	CFlowBelt();
	virtual ~CFlowBelt();
	
public:
	// check if the conveyor is vacant
	virtual bool isVacant( Person* pPerson  = NULL) const;

};

#endif // !defined(AFX_FLOWBELT_H__0B32BE03_3F78_46BD_8F87_3D4BF7954D1C__INCLUDED_)
