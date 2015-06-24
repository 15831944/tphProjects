// RotateProcessorsCommand.h: interface for the RotateProcessorsCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROTATEPROCESSORSCOMMAND_H__702B14DF_271D_4953_8321_12CD94170ECB__INCLUDED_)
#define AFX_ROTATEPROCESSORSCOMMAND_H__702B14DF_271D_4953_8321_12CD94170ECB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common/Command.h"
#include <vector>

class RotateProcessorsCommand : public Command 
{
public:
	RotateProcessorsCommand(CObjectDisplayList& proclist, CTermPlanDoc* pDoc, double dr);
	virtual ~RotateProcessorsCommand();

	virtual void OnDo();
	virtual void OnUndo();

	virtual CString GetCommandString();

private:
	double m_dr;							//delta rotation
	std::vector<UINT> m_procidlist;			//processors to move (store unique ids)
	CTermPlanDoc* m_pDoc;

};

#endif // !defined(AFX_ROTATEPROCESSORSCOMMAND_H__702B14DF_271D_4953_8321_12CD94170ECB__INCLUDED_)
