#pragma once

#include <common/ARCVector.h>

class OgreConvert
{
public:
	static inline Ogre::ColourValue GetColor(DWORD c,float a = 1.0f)
	{
		return Ogre::ColourValue(GetRValue(c)/255.0f,GetGValue(c)/255.0f,GetBValue(c)/255.0f,a);
	}
	static inline Ogre::ColourValue GetColor(ARCColor3 color, float a = 1.0f)
	{
		return Ogre::ColourValue(color[RED]/255.0f, color[GREEN]/255.0f, color[BLUE]/255.0f, a);
	}
	static inline Ogre::ColourValue GetColor(ARCColor4 color)
	{
		return Ogre::ColourValue(color[RED]/255.0f, color[GREEN]/255.0f, color[BLUE]/255.0f, color[ALPHA]/255.0f);
	}
	static inline DWORD GetColor(const Ogre::ColourValue&c ){ return c.getAsABGR(); }

	//static inline Ogre::Vector3 GetVector(const ARCVector3& v);
};

