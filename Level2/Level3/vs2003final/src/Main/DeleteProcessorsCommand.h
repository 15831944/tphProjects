// DeleteProcessorsCommand.h: interface for the DeleteProcessorsCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DELETEPROCESSORSCOMMAND_H__672A7044_E690_4DA3_98E8_59572BE5D480__INCLUDED_)
#define AFX_DELETEPROCESSORSCOMMAND_H__672A7044_E690_4DA3_98E8_59572BE5D480__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Command.h"
#include <vector>

class CTermPlanDoc;
class CProcessor2;

class DeleteProcessorsCommand : public Command
{
public:
	DeleteProcessorsCommand(std::vector<CProcessor2*>& proclist, CTermPlanDoc* pDoc);
	virtual ~DeleteProcessorsCommand();

	virtual void Do();
	virtual void Undo();

	virtual CString GetCommandString();

private:
	CTermPlanDoc* m_pDoc;
	std::vector<ProcessorID> m_procidsToDelete;		//processors to delete when Do() is executed

};

#endif // !defined(AFX_DELETEPROCESSORSCOMMAND_H__672A7044_E690_4DA3_98E8_59572BE5D480__INCLUDED_)
