#pragma once
#include <common/Guid.h>
#include "SceneNodeType.h"
#include <commondata/querydata.h>

class CRenderScene3DNode;


class SceneNodeQueryData : public QueryData
{
public:
	SceneNodeQueryData(SceneNodeType type = SceneNode_None, const Ogre::Any& data = Ogre::Any());

	friend std::ostream& operator << ( std::ostream& os, SceneNodeQueryData& qd );

	SceneNodeType eNodeType;///deprecated use mpNodeDynamic instead
	int nSubType; //deprecated use data instead
	Ogre::Any nodeData; //deprecated use data instead
public:	
	CRenderScene3DNode* mpNodeDynamic; // the prototype of the node, ref to a static object /used to call dynamic functions
};


struct GUIDQueryData 
{
public:
	GUIDQueryData(const GUID& _objGUID, const GUID& _clsGUID)
		: objGUID(_objGUID)
		, clsGUID(_clsGUID)
	{

	}

	friend std::ostream& operator << ( std::ostream& os, GUIDQueryData& qd );

	CGuid objGUID;
	CGuid clsGUID;
};