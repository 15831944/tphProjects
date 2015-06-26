#include "StdAfx.h"
#include "Terminal3DObject.h"

using namespace Ogre;

#include <InputAirside/ALTObject.h>

void C3DSceneNodeObject::UpdateSelected( bool b )
{
	m_bSelected = b;
	SetShowBound(b);
}

void CTerminal3DObject::Update3D( CBaseObject* pBaseObj, int nUpdateCode /*= Terminal3DUpdateAll*/ )
{
	SetShowBound(m_bSelected);
}

void CTerminal3DObject::Update3D( CObjectDisplay* pDispObj, int nUpdateCode /*= Terminal3DUpdateAll*/ )
{
	SetShowBound(m_bSelected);
}

CBaseObject* CTerminal3DObject::GetBaseObject() const
{
	return NULL;
}

CObjectDisplay* CTerminal3DObject::GetDisplayObject() const
{
	return NULL;
}













//////////////////////////////////////////////////////////////////////////
//
void CAirside3DObject::Update3D( ALTObject* pBaseObj, int nUpdateCode /*= Terminal3DUpdateAll*/ )
{

}

void CAirside3DObject::Update3D( ALTObjectDisplayProp* pDispObj, int nUpdateCode /*= Terminal3DUpdateAll*/ )
{

}

ALTObject* CAirside3DObject::GetBaseObject() const
{
	return NULL;
}

ALTObjectDisplayProp* CAirside3DObject::GetDisplayProp() const
{
	ALTObject* altObj = GetBaseObject();
	return altObj ? altObj->getDisplayProp() : NULL;
}