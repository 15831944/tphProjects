#pragma once
#include "3DSceneNode.h"

#include <InputAirside/Runway.h>
struct TextureClip
{
	std::string texture_name;
	std::vector<ARCVector2> vertex_coords;
	std::vector<ARCVector2> texture_coords;
};
class TextureClips : public std::vector<TextureClip> 
{

};

class CRenderRunway3D : public C3DSceneNode	
{
public:
	SCENE_NODE(CRenderRunway3D,C3DSceneNode)

	virtual void Update3D(ALTObject* pBaseObj);
	virtual void Update3D(ALTObjectDisplayProp* pDispObj);

protected:
	void GenTextureClips(Runway* pRunway);
	void Build(Runway* pRunway);

private:
	TextureClips m_vClips;
};

