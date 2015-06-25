// CommandHistory.cpp: implementation of the CommandHistory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Command.h"
#include "CommandHistory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CommandHistory::CommandHistory(int N)
{
	m_nMaxCommands = N;
	ASSERT(N>0);
	
	m_itNextRedo=end();
	m_itNextUndo = end();
}

CommandHistory::~CommandHistory()
{
	DeleteCommands(begin(), end());
}

bool CommandHistory::Undo()
{
	if(m_itNextUndo==end())
		return false;
	(*m_itNextUndo)->Undo();
	m_itNextRedo = m_itNextUndo;
	if(m_itNextUndo==begin())
		m_itNextUndo = end();
	else
		m_itNextUndo--;


	//
	// TRACE("***COMMAND HISTORY***\n");
	// TRACE("%d commands:\n", size());
//	for(iterator it=begin(); it!=end(); it++) {
		// TRACE((*it)->GetCommandString());
//		if(it==m_itNextUndo)
			// TRACE("<-- NextUndo");
//		else if(it==m_itNextRedo)
			// TRACE("<-- NextRedo");
		// TRACE("\n");
//	}
	// TRACE("*********************\n");
	//


	return true;
}

bool CommandHistory::Redo()
{
	if(m_itNextRedo==end())
		return false;
	(*m_itNextRedo)->Do();
	m_itNextUndo = m_itNextRedo;
	if(++m_itNextRedo == end())
		m_itNextRedo = end();

#ifdef _DEBUG
	//
	// TRACE("***COMMAND HISTORY***\n");
	// TRACE("%d commands:\n", size());
//	for(iterator it=begin(); it!=end(); it++) {
		// TRACE((*it)->GetCommandString());
//		if(it==m_itNextUndo)
			// TRACE("<-- NextUndo");
//		else if(it==m_itNextRedo)
			// TRACE("<-- NextRedo");
		// TRACE("\n");
//	}
	// TRACE("*********************\n");
	//
#endif

	return true;
}

bool CommandHistory::CanUndo()
{
	if(m_itNextUndo==end())
		return false;
	return true;
}

bool CommandHistory::CanRedo()
{
	if(m_itNextRedo==end())
		return false;
	return true;
}


void CommandHistory::Add(Command* c)
{
	if(m_itNextRedo!=end()) {
		DeleteCommands(m_itNextRedo, end());
		m_itNextRedo = end();
	}

	m_itNextUndo = insert(end(), c);

	if(static_cast<int>(size()) > m_nMaxCommands)
		pop_front();


#ifdef _DEBUG
	//
	// TRACE("***COMMAND HISTORY***\n");
	// TRACE("%d commands:\n", size());
//	for(iterator it=begin(); it!=end(); it++) {
		// TRACE((*it)->GetCommandString());
//		if(it==m_itNextUndo)
			// TRACE("<-- NextUndo");
//		else if(it==m_itNextRedo)
			// TRACE("<-- NextRedo");
		// TRACE("\n");
//	}
	// TRACE("*********************\n");
	//
#endif
}

void CommandHistory::DeleteCommands(iterator first, iterator last)
{
	for(iterator it=first; it!=last; it++) {
		delete (*it);
	}
	erase(first, last);
}

void CommandHistory::DeleteCommands(iterator it)
{
	delete *it;
	erase(it);
}

void CommandHistory::Clear()
{
	DeleteCommands(begin(), end());
}