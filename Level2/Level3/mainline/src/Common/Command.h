// Command.h: interface for the Command class.
//
// Based on the Command Design Pattern
//////////////////////////////////////////////////////////////////////

#pragma once

class   Command
{
public:
	enum LastCommandOp
	{
		_null,
		_doing,
		_canceled,
		_do,
		_undo,
	};
	virtual ~Command() {};

	void Do(){ OnDo(); mLastOp = Command::_do; } 
	void Undo(){ OnUndo(); mLastOp = Command::_undo; }

	virtual void OnDo()=0;
	virtual void OnUndo()=0;

	virtual CString GetCommandString()=0;
	LastCommandOp mLastOp;

	bool IsDo()const{ return mLastOp==Command::_do; }
	virtual bool IsRecord()const{ return true; }
protected:
	Command(){ mLastOp = _null; };

};

#define UNRECODE_COMMAND 	virtual bool IsRecord()const{ return false; }\
							virtual void OnDo(){} \
							virtual void OnUndo(){} 


class CEditOperationCommand :	public Command
{
public:
	void BeginOp(){ OnBeginOp(); mLastOp = Command::_doing; }
	void EndOp(){  OnEndOp(); mLastOp = Command::_do; };
	void CancelOp(){ OnCancelOp(); mLastOp = Command::_canceled; }

	virtual void OnBeginOp(){}
	virtual void OnEndOp(){}
	virtual void OnCancelOp(){}
protected:
};

