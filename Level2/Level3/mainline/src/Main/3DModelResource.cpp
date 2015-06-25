#include "StdAfx.h"
#include ".\3dmodelresource.h"
#include "./3DSFileLoader.h"
#include "./TermPlan.h"


const char* C3DModelResource::sm_szFileNames[] = {
	"Trafficlight.3ds",
	"StopSign.3ds",
	"Tollgate.3DS",
	"Yieldsign.3ds",
	"Trafficlight2D.3ds",
	"StopSign2D.3ds",
	"TollGate2D.3ds",
	"Yieldsign2D.3ds"
};

C3DModelResource::C3DModelResource(void)
{
	Init();
}

C3DModelResource::~C3DModelResource(void)
{
	for(size_t i=0;i<m_models.size();i++)
	{
		if( m_models[i] ) delete m_models[i];
	}
}

void C3DModelResource::Init()
{
	for(int i = 0;i<sm_nTextureCount;i++)
		m_models.push_back(NULL);
}

C3dModel * C3DModelResource::getModel(ModelType type)
{
	if( m_models[type] == NULL)
	{
		C3dModel *newModel = new C3dModel;
		C3DSFileLoader fileloader;
		if(! fileloader.LoadModel(getModelFileName(type),*newModel) )
		{
			m_models[type] =NULL;
			delete newModel;
			return &m_default;
		}
		else 
			m_models[type] = newModel;
	}
	return m_models[type];
}

CString C3DModelResource::getModelFileName(ModelType type)const
{
	CString fullPath;
	fullPath.Format("%s\\%s",((CTermPlanApp*)AfxGetApp())->GetTexturePath(),sm_szFileNames[type]);
	return fullPath;
}