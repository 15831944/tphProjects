#ifndef __LandsideTrafficLight3D_H__
#define __LandsideTrafficLight3D_H__

#include "RenderScene3DNode.h"
#include "CommonData/3DViewCommon.h"
#include "SceneState.h"
// #include "LandsideStretch3D.h"
// #include "../../Landside/LandsideTrafficLight.h"

// template<class DataType>
class LandsideTrafficLight3D : public CRenderScene3DNode
{
protected:
	//DataType*	m_trafficLight;

public:
	SCENENODE_CONVERSION_DEF(LandsideTrafficLight3D, CRenderScene3DNode)
//	LandsideTrafficLight3D(DataType* light) { m_trafficLight = light; }
	
	void		Update3D();
	void		BuildPole(ManualObject* pObj,double poleLeftLength,double poleRightLength);
// 	void		SetTrafficLightData(DataType* light) { m_trafficLight = light; }

	void		SetRed(Ogre::Entity* light, bool bEnable);
	void		SetGreen(Ogre::Entity* light, bool bEnable);
	void		SetYellow(Ogre::Entity* light, bool bEnable);

	void		SetCrossRed(Ogre::Entity* light);
	void		SetCrossGreen(Ogre::Entity* light);
	void		SetCrossYellow(Ogre::Entity* light);

	void		SetRed(bool isCrossOver=false);
	void		SetGreen(bool isCrossOver=false);
	void		SetYellow(bool isCrossOver=false);
	void		SetLocationData(CPoint2008 pos,ARCVector3 dir,int nStretchID,/*int nLaneIndex,*/int nGroupSize,double nLaneWidth,CString sName,CString sMesh);
};


#endif