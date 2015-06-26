#pragma once
#include <common\command.h>
#include <Common\ARCVector.h>
#include "3DNodeObject.h"

enum E3DObjectEditOperation
{
	OP_null3DObj,

	OP_move3DObj,
	OP_rotate3DObj,
	OP_scale3DObj,
	OP_mirror3DObj,

	OP_add3DObj,
	OP_del3DObj,
};

//operation start will create more like a operation 
class RENDERENGINE_API  C3DObjectCommand : public CEditOperationCommand
{
public:
	enum EditCoordination
	{
		COORD_NONE = 0,

		COORD_X,
		COORD_Y,
		COORD_Z,

		COORD_XY,
		COORD_YZ,
		COORD_ZX,

		COORD_XYZ,
	};

	C3DObjectCommand( C3DNodeObject nodeObj, EditCoordination eCoord, E3DObjectEditOperation eOpType, LPCTSTR lpCommandString );

	E3DObjectEditOperation getEditOp() const { return m_eOpType; }
	EditCoordination GetCoord() const { return m_eCoord; }
	virtual CString GetCommandString() { return m_lpCommandString; }
	C3DNodeObject GetNodeObj() const { return m_nodeObj; }


	virtual bool ToBeRecorded() const;

protected:

	EditCoordination m_eCoord; // meaningless to some operations, but reserve as member of base class

	E3DObjectEditOperation m_eOpType;
	LPCTSTR m_lpCommandString;

	C3DNodeObject m_nodeObj;
};


class C3DObjectMoveCommand : public C3DObjectCommand
{
public:
	C3DObjectMoveCommand(C3DNodeObject nodeObj, EditCoordination eCoord);
	virtual void OnDo() ;
	virtual void OnUndo() ;
	virtual void OnBeginOp();
	virtual void OnEndOp();
	virtual bool ToBeRecorded() const;

	ARCVector3 GetStartPos() const { return m_startPos; }

protected:

	ARCVector3 m_startPos;
	ARCVector3 m_endPos;
};

class C3DObjectRotateCommand : public C3DObjectCommand
{
public:
	C3DObjectRotateCommand(C3DNodeObject nodeObj, EditCoordination eCoord);
	virtual void OnDo() ;
	virtual void OnUndo() ;
	virtual void OnBeginOp();
	virtual void OnEndOp();

	virtual bool ToBeRecorded() const;

	ARCVector3 GetStartRotation() const { return m_startRotation; }

protected:

	ARCVector3 m_startRotation;
	ARCVector3 m_endRotation;
};

class C3DObjectMirrorCommand : public C3DObjectCommand
{
public:
	C3DObjectMirrorCommand(C3DNodeObject nodeObj);
	virtual void OnDo() ;
	virtual void OnUndo() ;
	virtual void OnBeginOp();
	virtual void OnEndOp();


protected:
	ARCVector3 m_startScale;
};

class C3DObjectScaleCommand : public C3DObjectCommand
{
public:
	C3DObjectScaleCommand(C3DNodeObject nodeObj, EditCoordination eCoord);
	virtual void OnDo() ;
	virtual void OnUndo() ;
	virtual void OnBeginOp();
	virtual void OnEndOp();

	virtual bool ToBeRecorded() const;

	ARCVector3 GetStartScale() const { return m_startScale; }

protected:

	ARCVector3 m_startScale;
	ARCVector3 m_endScale;
};

class C3DObjectAddCommand : public C3DObjectCommand
{
public:
	C3DObjectAddCommand(C3DNodeObject nodeObj, C3DNodeObject rootNodeObj);
	virtual void OnDo() ;
	virtual void OnUndo() ;
	virtual void OnBeginOp();
	virtual void OnEndOp();

protected:

	C3DNodeObject m_nodeRoot;
};

class C3DObjectDelCommand : public C3DObjectCommand
{
public:
	C3DObjectDelCommand(C3DNodeObject nodeObj, C3DNodeObject rootNodeObj);
	virtual void OnDo() ;
	virtual void OnUndo() ;
	virtual void OnBeginOp();
	virtual void OnEndOp();

protected:

	C3DNodeObject m_nodeRoot;
};
