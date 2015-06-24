// MacroCommand.h: interface for the MacroCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MACROCOMMAND_H__EEF3EDEE_C7F5_4D88_9C95_E00E19749FEE__INCLUDED_)
#define AFX_MACROCOMMAND_H__EEF3EDEE_C7F5_4D88_9C95_E00E19749FEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\Command.h"
#include <list>

class MacroCommand  
{
public:
	MacroCommand();
	virtual ~MacroCommand();

	virtual void Add(Command* c);

	virtual void Do();
	virtual void Undo();

private:
	std::list<Command*> m_cmdlist;

};

#endif // !defined(AFX_MACROCOMMAND_H__EEF3EDEE_C7F5_4D88_9C95_E00E19749FEE__INCLUDED_)
