// MoveProcessorsCommand.h: interface for the MoveProcessorsCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOVEPROCESSORSCOMMAND_H__2A8D1C83_35F8_41BD_BC9A_E86EFFC0CF26__INCLUDED_)
#define AFX_MOVEPROCESSORSCOMMAND_H__2A8D1C83_35F8_41BD_BC9A_E86EFFC0CF26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common/Command.h"
#include <vector>


class MoveProcessorsCommand : public Command
{
public:
	MoveProcessorsCommand(CObjectDisplayList& proclist, CTermPlanDoc* pDoc, double dx, double dy);
	virtual ~MoveProcessorsCommand();

	virtual void OnDo();
	virtual void OnUndo();

	virtual CString GetCommandString();

private:
	double m_dx;							//x offset
	double m_dy;							//y offset
	std::vector<UINT> m_procidlist;			//processors to move (store unique id)
	CTermPlanDoc* m_pDoc;
};

#endif // !defined(AFX_MOVEPROCESSORSCOMMAND_H__2A8D1C83_35F8_41BD_BC9A_E86EFFC0CF26__INCLUDED_)
