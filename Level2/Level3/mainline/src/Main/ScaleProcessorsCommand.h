// ScaleProcessorsCommand.h: interface for the ScaleProcessorsCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCALEPROCESSORSCOMMAND_H__60E68825_BB09_4CDC_B54A_E95B0B7B9693__INCLUDED_)
#define AFX_SCALEPROCESSORSCOMMAND_H__60E68825_BB09_4CDC_B54A_E95B0B7B9693__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common/Command.h"
#include <vector>


class ScaleProcessorsCommand : public Command
{
public:
	ScaleProcessorsCommand(CObjectDisplayList& proclist, CTermPlanDoc* pDoc, double dx, double dy, double dz);
	virtual ~ScaleProcessorsCommand();

	virtual void OnDo();
	virtual void OnUndo();

	virtual CString GetCommandString();

private:
	double m_dx;							//delta x scale
	double m_dy;							//delta y scale
	double m_dz;							//delta z scale
	std::vector<UINT> m_procidlist;			//processors to move (store unique ids)
	CTermPlanDoc* m_pDoc;

};

#endif // !defined(AFX_SCALEPROCESSORSCOMMAND_H__60E68825_BB09_4CDC_B54A_E95B0B7B9693__INCLUDED_)
