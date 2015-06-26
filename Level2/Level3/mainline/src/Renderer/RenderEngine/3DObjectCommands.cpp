#include "StdAfx.h"
#include ".\3dobjectcommands.h"



C3DObjectCommand::C3DObjectCommand( C3DNodeObject nodeObj, EditCoordination eCoord, E3DObjectEditOperation eOpType, LPCTSTR lpCommandString )
	: m_nodeObj(nodeObj)
	, m_eCoord(eCoord)
	, m_eOpType(eOpType)
	, m_lpCommandString(lpCommandString)
{
}

bool C3DObjectCommand::ToBeRecorded() const
{
	return true;
}
C3DObjectMoveCommand::C3DObjectMoveCommand( C3DNodeObject nodeObj, EditCoordination eCoord )
	: C3DObjectCommand(nodeObj, eCoord, OP_move3DObj, _T("Move 3D Node Object"))
{
	ASSERT(COORD_X  == eCoord
		|| COORD_Y  == eCoord
		|| COORD_Z  == eCoord
		|| COORD_XY == eCoord
		|| COORD_YZ == eCoord
		|| COORD_ZX == eCoord
		);
}

void C3DObjectMoveCommand::OnDo()
{
	m_nodeObj.SetPosition(m_endPos);
}

void C3DObjectMoveCommand::OnUndo()
{
	m_nodeObj.SetPosition(m_startPos);
}

void C3DObjectMoveCommand::OnEndOp()
{
	m_endPos = m_nodeObj.GetPosition();
}

void C3DObjectMoveCommand::OnBeginOp()
{
	m_startPos = m_nodeObj.GetPosition();
}

bool C3DObjectMoveCommand::ToBeRecorded() const
{
	return m_startPos.DistanceTo(m_nodeObj.GetPosition()) > ARCMath::EPSILON;
}
C3DObjectRotateCommand::C3DObjectRotateCommand( C3DNodeObject nodeObj, EditCoordination eCoord )
	: C3DObjectCommand(nodeObj, eCoord, OP_rotate3DObj, _T("Rotate 3D Node Object"))
{
	ASSERT(COORD_X == eCoord
		|| COORD_Y == eCoord
		|| COORD_Z == eCoord
		);
}

void C3DObjectRotateCommand::OnDo()
{
	m_nodeObj.SetRotation(m_endRotation);
}

void C3DObjectRotateCommand::OnUndo()
{
	m_nodeObj.SetRotation(m_startRotation);
}

void C3DObjectRotateCommand::OnEndOp()
{
	m_endRotation = m_nodeObj.GetRotation();
}

void C3DObjectRotateCommand::OnBeginOp()
{
	m_startRotation = m_nodeObj.GetRotation();
}

bool C3DObjectRotateCommand::ToBeRecorded() const
{
	return m_startRotation.DistanceTo(m_nodeObj.GetRotation()) > ARCMath::EPSILON;
}

C3DObjectMirrorCommand::C3DObjectMirrorCommand( C3DNodeObject nodeObj)
	: C3DObjectCommand(nodeObj, COORD_NONE, OP_rotate3DObj, _T("Rotate 3D Node Object"))
{

}

void C3DObjectMirrorCommand::OnDo()
{
	m_nodeObj.SetScale(-m_startScale);
}

void C3DObjectMirrorCommand::OnUndo()
{
	m_nodeObj.SetScale(m_startScale);
}

void C3DObjectMirrorCommand::OnEndOp()
{

}

void C3DObjectMirrorCommand::OnBeginOp()
{
	m_startScale = m_nodeObj.GetScale();
}

C3DObjectScaleCommand::C3DObjectScaleCommand( C3DNodeObject nodeObj, EditCoordination eCoord )
	: C3DObjectCommand(nodeObj, eCoord, OP_scale3DObj, _T("Scale 3D Node Object"))
{
	ASSERT(COORD_X   == eCoord
		|| COORD_Y   == eCoord
		|| COORD_Z   == eCoord
		|| COORD_XYZ == eCoord
		);
}

void C3DObjectScaleCommand::OnDo()
{
	m_nodeObj.SetScale(m_endScale);
}

void C3DObjectScaleCommand::OnUndo()
{
	m_nodeObj.SetScale(m_startScale);
}

void C3DObjectScaleCommand::OnEndOp()
{
	m_endScale = m_nodeObj.GetScale();
}

void C3DObjectScaleCommand::OnBeginOp()
{
	m_startScale = m_nodeObj.GetScale();
}

bool C3DObjectScaleCommand::ToBeRecorded() const
{
	return m_startScale.DistanceTo(m_nodeObj.GetScale()) > ARCMath::EPSILON;
}
C3DObjectAddCommand::C3DObjectAddCommand( C3DNodeObject nodeObj, C3DNodeObject rootNodeObj )
	: C3DObjectCommand(nodeObj, COORD_NONE, OP_add3DObj, _T("Add 3D Node Object"))
	, m_nodeRoot(rootNodeObj)
{

}

void C3DObjectAddCommand::OnDo()
{
	m_nodeObj.AttachTo(m_nodeRoot);
}

void C3DObjectAddCommand::OnUndo()
{
	m_nodeObj.Detach();
}

void C3DObjectAddCommand::OnEndOp()
{

}

void C3DObjectAddCommand::OnBeginOp()
{

}

C3DObjectDelCommand::C3DObjectDelCommand( C3DNodeObject nodeObj, C3DNodeObject rootNodeObj )
	: C3DObjectCommand(nodeObj, COORD_NONE, OP_del3DObj, _T("Delete 3D Node Object"))
	, m_nodeRoot(rootNodeObj)
{

}

void C3DObjectDelCommand::OnDo()
{
	m_nodeObj.Detach();
}

void C3DObjectDelCommand::OnUndo()
{
	m_nodeObj.AttachTo(m_nodeRoot);
}

void C3DObjectDelCommand::OnEndOp()
{

}

void C3DObjectDelCommand::OnBeginOp()
{

}
