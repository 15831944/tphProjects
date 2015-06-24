// CopyProcessorsCommand.h: interface for the CopyProcessorsCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COPYPROCESSORSCOMMAND_H__A2D656EA_4EF0_434E_8128_F557D6A7517C__INCLUDED_)
#define AFX_COPYPROCESSORSCOMMAND_H__A2D656EA_4EF0_434E_8128_F557D6A7517C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Command.h"
#include <vector>


class CTermPlanDoc;
class CProcessor2;

class CopyProcessorsCommand : public Command
{
public:
	CopyProcessorsCommand(std::vector<CProcessor2*>& proclist, CTermPlanDoc* pDoc);
	virtual ~CopyProcessorsCommand();

	virtual void Do();
	virtual void Undo();

	virtual CString GetCommandString();

private:
	CTermPlanDoc* m_pDoc;
	std::vector<ProcessorID> m_procidsToCopy;		//processors to copy when Do() is executed
	std::vector<ProcessorID> m_procidsToDelete;		//processors to delete when Undo() is executed
};

#endif // !defined(AFX_COPYPROCESSORSCOMMAND_H__A2D656EA_4EF0_434E_8128_F557D6A7517C__INCLUDED_)
