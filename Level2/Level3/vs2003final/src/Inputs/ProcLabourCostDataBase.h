// PorcLabourCostDateBase.h: interface for the CPorcLabourCostDateBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PORCLABOURCOSTDATEBASE_H__D810F5C5_46C2_4FED_AE6E_207F894C5785__INCLUDED_)
#define AFX_PORCLABOURCOSTDATEBASE_H__D810F5C5_46C2_4FED_AE6E_207F894C5785__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "ProcLabourCost.h"
#include "common\dataset.h"

class CProcLabourCostDataBase
{
public:
	CProcLabourCostDataBase();
	virtual ~CProcLabourCostDataBase();

public:
	// access
	void AddItem( CProcLabourCost procLabourCost );
	void EraseItem( int nIdx );
	int  GetSize();
	void Clear();
	CProcLabourCost GetItem( int nIdx );
	void ModifyItem( CProcLabourCost procLabourCost, int nIdx );

protected:
	std::vector<CProcLabourCost> m_vProcLabour;
};

#endif // !defined(AFX_PORCLABOURCOSTDATEBASE_H__D810F5C5_46C2_4FED_AE6E_207F894C5785__INCLUDED_)
