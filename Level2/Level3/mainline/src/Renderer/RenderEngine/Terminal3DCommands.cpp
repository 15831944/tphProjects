#include "StdAfx.h"
#include "Terminal3DCommands.h"
#include <Common/BaseObject.h>


Terminal3DCommand::Terminal3DCommand( CTerminal3DObject::SharedPtr p3DObj, OperationType eOpType, LPCTSTR lpCommandString )
	: m_p3DObj(p3DObj)
	, m_eOpType(eOpType)
	, m_lpCommandString(lpCommandString)
{

}

CBaseObject* Terminal3DCommand::GetBaseObject() const
{
	return m_p3DObj ? m_p3DObj->GetBaseObject() : NULL;
}

Terminal3DMoveCommand::Terminal3DMoveCommand( CTerminal3DObject::SharedPtr p3DObj )
	: Terminal3DCommand(p3DObj, OP_move, _T("Move Terminal 3D Object"))
{

}

void Terminal3DMoveCommand::OnDo()
{
	CBaseObject* pObj = GetBaseObject();
	if (pObj)
	{
		ARCVector3 offset = m_endPos - pObj->GetLocation();
		pObj->MoveObject(offset.x, offset.y);
		m_p3DObj->Update3D(pObj);
	} 
	else
		ASSERT(FALSE);
}

void Terminal3DMoveCommand::OnUndo()
{
	CBaseObject* pObj = GetBaseObject();
	if (pObj)
	{
		ARCVector3 offset = m_startPos - pObj->GetLocation();
		pObj->MoveObject(offset.x, offset.y);
		m_p3DObj->Update3D(pObj);
	}
	else
		ASSERT(FALSE);
}

void Terminal3DMoveCommand::OnBeginOp()
{
	CBaseObject* pObj = GetBaseObject();
	if (pObj)
	{
		m_startPos = pObj->GetLocation();
	}
	else
		ASSERT(FALSE);
}

void Terminal3DMoveCommand::OnEndOp()
{
	CBaseObject* pObj = GetBaseObject();
	if (pObj)
	{
		m_endPos = pObj->GetLocation();
	}
	else
		ASSERT(FALSE);
}

bool Terminal3DMoveCommand::ToBeRecorded() const
{
	return m_startPos.DistanceTo(m_endPos) > ARCMath::EPSILON;
}

Terminal3DRotateCommand::Terminal3DRotateCommand( CTerminal3DObject::SharedPtr p3DObj, double dr, const ARCVector3& point )
	: Terminal3DCommand(p3DObj, OP_move, _T("Rotate Terminal 3D Object"))
	, m_dr(dr)
	, m_point(point)
{

}

void Terminal3DRotateCommand::OnDo()
{
	CBaseObject* pObj = GetBaseObject();
	if (pObj)
	{
		pObj->RotateObject(m_dr, m_point);
		m_p3DObj->Update3D(pObj);
	}
	else
		ASSERT(FALSE);
}

void Terminal3DRotateCommand::OnUndo()
{
	CBaseObject* pObj = GetBaseObject();
	if (pObj)
	{
		pObj->RotateObject(-m_dr, m_point);
		m_p3DObj->Update3D(pObj);
	}
	else
		ASSERT(FALSE);
}

bool Terminal3DRotateCommand::ToBeRecorded() const
{
	return abs(m_dr) > ARCMath::EPSILON;
}