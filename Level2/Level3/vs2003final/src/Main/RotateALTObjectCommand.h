// RotateALTObjectCommand.h: interface for the RotateALTObjectCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROTATEALTOBJECTCOMMAND_H__702B14DF_271D_4953_8321_12CD94170ECB__INCLUDED_)
#define AFX_ROTATEALTOBJECTCOMMAND_H__702B14DF_271D_4953_8321_12CD94170ECB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common/Command.h"
#include <vector>

class ALTObject;
class CTermPlanDoc;
class RotateALTObjectCommand : public Command 
{
public:
	RotateALTObjectCommand(std::vector< ref_ptr<ALTObject3D> >& altobjectlist, CTermPlanDoc* pDoc, double dr);
	virtual ~RotateALTObjectCommand();

	virtual void OnDo();
	virtual void OnUndo();

	virtual CString GetCommandString();

private:
	double m_dr;							//delta rotation
	std::vector<UINT> m_altobjectidlist;	//altobject to Rotate (store unique ids)
	CTermPlanDoc* m_pDoc;

};

#endif // !defined(AFX_ROTATEALTOBJECTCOMMAND_H__702B14DF_271D_4953_8321_12CD94170ECB__INCLUDED_)
