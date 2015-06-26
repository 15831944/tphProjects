#pragma once
#include "3DSceneNode.h"

class IARCportLayoutEditContext;




class C3DAirsideNode : public C3DSceneNode
{
	SCENE_NODE(C3DAirsideNode,C3DSceneNode)


public:
	void UpdateAll(IARCportLayoutEditContext* airside);
	//void RemoveFloor(const CString& floorId);

	template <class DataType,class _3DType>
	void SetupObjectList(IARCportLayoutEditContext* pEditContext );

};


class ALTAirportLayout;
class C3DAirportNode : public C3DSceneNode
{
	SCENE_NODE(C3DAirportNode,C3DSceneNode)
public:
	void Setup3D(ALTAirportLayout* pairport);
	template <class DataType,class _3DType>
	void UpdateObjectList(ALTAirportLayout* airportID, IARCportLayoutEditContext* pEditContext );

	int GetAirportID()const;
	void SetAirportID(int id);
};