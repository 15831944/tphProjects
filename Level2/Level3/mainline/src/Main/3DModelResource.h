#ifndef C3DMODEL_RESOURCE_DEF
#define C3DMODEL_RESOURCE_DEF

#include "./3dModel.h"

#pragma once

class C3DModelResource
{

public:
	enum ModelType 
	{
		TrafficlightModel=0, 
		StopSignModel,
		TollGateModel,
		YieldSignModel,
		Trafficlight2DModel,
		StopSign2DModel,
		TollGate2DModel,
		YieldSign2DModel,
	};

	static const int sm_nTextureCount = 8;
	static const char* sm_szFileNames[];

	//get specified model
	C3dModel* getModel(ModelType type);

	C3DModelResource(void);
	virtual ~C3DModelResource(void);

	void Init();

	//return the model's fullpath name;
	CString getModelFileName(ModelType type)const;
private:


	std::vector <C3dModel*> m_models;
	C3dModel m_default;

};

#endif
