#include "StdAfx.h"
#include ".\scenenodequerydata.h"
std::ostream& operator << ( std::ostream& os, SceneNodeQueryData& qd )
{
	os << qd.eNodeType << qd.nodeData;
	return os;
}

SceneNodeQueryData::SceneNodeQueryData( SceneNodeType type /*= SceneNode_None*/, const Ogre::Any& data /*= Ogre::Any()*/ ) : eNodeType(type)
, nodeData(data)
,nSubType(0)
{
	mpNodeDynamic = NULL;
}

//////////////////////////////////////////////////////////////////////////
std::ostream& operator << ( std::ostream& os, GUIDQueryData& qd )
{
	os << qd.objGUID << qd.clsGUID;
	return os;
}
