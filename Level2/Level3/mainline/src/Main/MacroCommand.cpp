// MacroCommand.cpp: implementation of the MacroCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "MacroCommand.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MacroCommand::MacroCommand()
{
}

MacroCommand::~MacroCommand()
{
	for(std::list<Command*>::iterator it=m_cmdlist.begin(); it!=m_cmdlist.end(); ++it) {
		delete (*it);
		(*it) = NULL;
	}
	m_cmdlist.clear();
}

void MacroCommand::Add(Command* c)
{
	m_cmdlist.push_back(c);
}

void MacroCommand::Do()
{
	for(std::list<Command*>::iterator it=m_cmdlist.begin(); it!=m_cmdlist.end(); ++it) {
		(*it)->Do();
	}
}

void MacroCommand::Undo()
{
	for(std::list<Command*>::reverse_iterator rit=m_cmdlist.rbegin(); rit!=m_cmdlist.rend(); ++rit) {
		(*rit)->Undo();
	}
}
