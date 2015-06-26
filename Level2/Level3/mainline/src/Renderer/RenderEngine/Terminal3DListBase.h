#pragma  once


enum Terminal3DUpdateCode
{
	Terminal3DUpdateAll = -1,
};

class CRender3DScene;
class CRenderAirside3D;

class RENDERENGINE_API CRender3DListBase
{
public:
	CRender3DListBase();

	void Set3DScene(CRender3DScene* p3DScene);

protected:
	CRender3DScene* m_p3DScene;
};

class RENDERENGINE_API CAirportObject3DListBase : public CRender3DListBase
{
public:
	CAirportObject3DListBase();

	void SetRenderAirport3D(CRenderAirside3D *pAirport3D);

protected:
	CRenderAirside3D *m_pAirport3D;
};
