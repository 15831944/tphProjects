#include "StdAfx.h"
#include ".\projecteditcontext.h"

CProjectEditContext::CProjectEditContext()
:m_editHistory(100)
{
	mpParentDoc = NULL;
	//m_bLockView =false;
}

bool CProjectEditContext::AddCommand( CEditOperationCommand*pCmd )
{
	if(pCmd && pCmd->IsRecord())
	{
		if(!pCmd->IsDo())
		{
			pCmd->EndOp();
		}	
		if(pCmd->IsDo())
			m_editHistory.Add(pCmd);
		return true;
	}		
	return false;
}


CEditOperationCommand* CProjectEditContext::Undo()
{
	if(m_editHistory.CanUndo())
	{
		CEditOperationCommand* pUndoOP= (CEditOperationCommand*) (*m_editHistory.GetNextUndo());
		if(pUndoOP){		
			if(m_editHistory.Undo())
				return pUndoOP;		
		}
	}
	return NULL;
}

CEditOperationCommand* CProjectEditContext::Redo()
{
	if(m_editHistory.CanRedo())
	{
		CEditOperationCommand* pRedoOP= (CEditOperationCommand*) (*m_editHistory.GetNextRedo());
		if(pRedoOP){		
			if(m_editHistory.Redo())
				return pRedoOP;		
		}
	}
	return NULL;
}

CProjectEditContext::~CProjectEditContext()
{

}

bool CProjectEditContext::CanRedo()
{
	return m_editHistory.CanRedo();
}

bool CProjectEditContext::CanUndo()
{
	return m_editHistory.CanUndo();
}
