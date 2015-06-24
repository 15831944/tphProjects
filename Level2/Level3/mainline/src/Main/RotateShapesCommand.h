// RotateShapesCommand.h: interface for the RotateShapesCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROTATESHAPESCOMMAND_H__48B6EDFC_C91C_4841_B256_CD2E06E1F271__INCLUDED_)
#define AFX_ROTATESHAPESCOMMAND_H__48B6EDFC_C91C_4841_B256_CD2E06E1F271__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common/Command.h"
#include <vector>

class RotateShapesCommand : public Command
{
public:
	RotateShapesCommand(CObjectDisplayList& proclist, CTermPlanDoc* pDoc, double dr);
	virtual ~RotateShapesCommand();

	virtual void OnDo();
	virtual void OnUndo();

	virtual CString GetCommandString();

private:
	double m_dr;							//delta rotation
	std::vector<UINT> m_procidlist;			//processors whose shapes we rotate (store unique ids)
	CTermPlanDoc* m_pDoc;
};

#endif // !defined(AFX_ROTATESHAPESCOMMAND_H__48B6EDFC_C91C_4841_B256_CD2E06E1F271__INCLUDED_)
