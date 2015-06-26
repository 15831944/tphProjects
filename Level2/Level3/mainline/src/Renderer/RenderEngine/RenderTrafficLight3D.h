#pragma once
#include "3DSceneNode.h"

#include <InputAirside/TrafficLight.h>

class CRenderTrafficLight3D :
	public C3DSceneNode
{
public:
	SCENE_NODE(CRenderTrafficLight3D,C3DSceneNode)
	virtual void Update3D(ALTObject* pBaseObj);
	virtual void Update3D(ALTObjectDisplayProp* pDispObj);

protected:
	void CalculateRotation(TrafficLight* pTrafficLight);
	void Build(TrafficLight* pTrafficLight, Ogre::ManualObject* pObj);
	C3DSceneNode AddOneTrafficLightModel(int idx);
private:
	std::vector<double> m_vDegree;
	//std::vector<C3DNodeObject> m_shapes;

	//class AttachDetachFunctor
	//{
	//public:
	//	AttachDetachFunctor(C3DNodeObject& parentObj, size_t nThresholdValue)
	//		: m_parentObj(parentObj)
	//		, m_nThresholdValue(nThresholdValue)
	//		, m_nCurCount(0)
	//	{

	//	}
	//	void operator()(C3DNodeObject& nodeObj)
	//	{
	//		if (m_nCurCount<m_nThresholdValue)
	//		{
	//			nodeObj.AttachTo(m_parentObj);
	//		}
	//		else
	//		{
	//			nodeObj.Detach();
	//		}
	//		m_nCurCount++;
	//	}

	//private:
	//	C3DNodeObject& m_parentObj;
	//	size_t m_nThresholdValue;
	//	size_t m_nCurCount;
	//};
};

//#include "Terminal3DList.h"
//class TrafficLight;
//class CRenderTrafficLight3DList : 
//	public CAirportObject3DList<CRenderTrafficLight3DList,TrafficLight , CRenderTrafficLight3D>
//{
//
//};

