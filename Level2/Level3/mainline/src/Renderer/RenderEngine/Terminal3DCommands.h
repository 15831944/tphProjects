#pragma once
#include <common\command.h>
#include <Common\ARCVector.h>
#include "Terminal3DObject.h"

class RENDERENGINE_API  Terminal3DCommand : public CEditOperationCommand
{
public:
	enum OperationType
	{
		OP_null,

		OP_move,
		OP_rotate,
	};

	Terminal3DCommand( CTerminal3DObject::SharedPtr p3DObj, OperationType eOpType, LPCTSTR lpCommandString );

	OperationType getEditOp() const { return m_eOpType; }
	virtual CString GetCommandString() { return m_lpCommandString; }
	CTerminal3DObject::SharedPtr GetNodeObj() const { return m_p3DObj; }
	CBaseObject* GetBaseObject() const;

	virtual bool ToBeRecorded() const = 0;

protected:
	OperationType m_eOpType;
	LPCTSTR m_lpCommandString;

	CTerminal3DObject::SharedPtr m_p3DObj;
};


class Terminal3DMoveCommand : public Terminal3DCommand
{
public:
	Terminal3DMoveCommand(CTerminal3DObject::SharedPtr p3DObj);
	virtual void OnDo();
	virtual void OnUndo() ;
	virtual void OnBeginOp();
	virtual void OnEndOp();
	virtual bool ToBeRecorded() const;

	ARCVector3 GetStartPos() const { return m_startPos; }

protected:
	ARCVector3 m_startPos;
	ARCVector3 m_endPos;
};

class Terminal3DRotateCommand : public Terminal3DCommand
{
public:
	Terminal3DRotateCommand(CTerminal3DObject::SharedPtr p3DObj, double dr, const ARCVector3& point);
	virtual void OnDo() ;
	virtual void OnUndo() ;
	virtual bool ToBeRecorded() const;

protected:
	const double m_dr;
	const ARCVector3 m_point;
};
