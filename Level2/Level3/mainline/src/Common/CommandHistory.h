// CommandHistory.h: interface for the CommandHistory class.
//
//////////////////////////////////////////////////////////////////////

#pragma once 
#include <list>

class Command;

class CommandHistory : private std::list<Command*>
{
public:
	CommandHistory(int N);
	virtual ~CommandHistory();

	bool Undo();
	bool Redo();

	void Add(Command* c);

	bool CanUndo();
	bool CanRedo();

	void Clear();

	iterator GetNextUndo(){ return m_itNextUndo; }
	iterator GetNextRedo(){ return m_itNextRedo; }

	Command* GetNextUndoCommand(){ return CanUndo() ? *m_itNextUndo : NULL; }
	Command* GetNextRedoCommand(){ return CanRedo() ? *m_itNextRedo : NULL; }
private:
	void DeleteCommands(iterator first, iterator last);
	void DeleteCommands(iterator it);

	int m_nMaxCommands;
	iterator m_itNextUndo;
	iterator m_itNextRedo;

};
