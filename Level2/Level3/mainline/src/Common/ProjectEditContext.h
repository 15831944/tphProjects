#pragma once
#include <Common/Command.h>
#include <Common/CommandHistory.h>

class CDocument;

class CProjectEditContext
{
public:
	CProjectEditContext();
	virtual ~CProjectEditContext();
	void SetDocument(CDocument* pDoc){ mpParentDoc = pDoc; }
	CDocument* GetDocument()const{ return mpParentDoc; }

	CommandHistory& getHistory(){ return m_editHistory; }

	bool AddCommand( CEditOperationCommand*pCmd );
	CEditOperationCommand* Undo();
	CEditOperationCommand* Redo();

	bool CanRedo();

	bool CanUndo();


	void flipLockView(){ m_bLockView =!m_bLockView; } 
	bool IsViewLocked()const{ return m_bLockView; }

protected:
	bool m_bLockView;
	CommandHistory m_editHistory;
	CDocument* mpParentDoc;
};