// MoveALTObjectCommand.h: interface for the MoveALTObjectCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOVEALTOBJECTCOMMAND_H__2A8D1C83_35F8_41BD_BC9A_E86EFFC0CF26__INCLUDED_)
#define AFX_MOVEALTOBJECTCOMMAND_H__2A8D1C83_35F8_41BD_BC9A_E86EFFC0CF26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common/Command.h"
#include <vector>

class ALTObject;
class CTermPlanDoc;
class MoveALTObjectCommand : public Command
{
public:
	MoveALTObjectCommand(std::vector< ref_ptr<ALTObject3D> >& altobjectlist, CTermPlanDoc* pDoc, double dx, double dy);
	virtual ~MoveALTObjectCommand();

	virtual void OnDo();
	virtual void OnUndo();

	virtual CString GetCommandString();

private:
	double m_dx;							//x offset
	double m_dy;							//y offset
	std::vector<UINT> m_altobjectidlist;	//altobject to move (store unique id)
	CTermPlanDoc* m_pDoc;
};

#endif // !defined(AFX_MOVEALTOBJECTCOMMAND_H__2A8D1C83_35F8_41BD_BC9A_E86EFFC0CF26__INCLUDED_)
