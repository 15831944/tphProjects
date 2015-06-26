#include "StdAfx.h"

#include "IntersectionsDefine.h"



void InsecObjectPart::SetObject( ALTObject * pObj )
{
	nALTObjectID = pObj->getID();
	m_pObj = pObj;
}

int InsecObjectPart::GetObjectID() const
{
	if(m_pObj.get())
	{
		return m_pObj->getID();
	}
	return nALTObjectID;
}

ALTObject * InsecObjectPart::GetObject()
{
	if(!m_pObj.get())
	{
		m_pObj = ALTObject::ReadObjectByID(nALTObjectID);
	}
	return m_pObj.get();
}

InsecObjectPart::InsecObjectPart()
{
	nALTObjectID = -1;
	part = -1;
	pos = 0;
}

InsecObjectPart::InsecObjectPart( const InsecObjectPart& otherPart)
{
	nALTObjectID = otherPart.nALTObjectID;//object id 
	part = otherPart.part;                    //part of the stretch
	pos= otherPart.pos;                  //relative position of the stretch path(0-1)
	m_pObj = otherPart.m_pObj;
}
InsecObjectPart::~InsecObjectPart( void )
{

}