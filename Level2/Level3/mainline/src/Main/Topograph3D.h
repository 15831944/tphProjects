#pragma once

#include ".\ALTObject3D.h"
class SelectableSettings;

class CTopograph3D 
{
public:
	CTopograph3D(int id);
	virtual ~CTopograph3D(void);

	void Init();

	ALTObject3D * GetObject3D(int id);
	
	ALTObject3D* AddObject(ALTObject* pObj);
	void UpdateAddorRemoveObjects();
	bool UpdateChangeOfObject( ALTObject* pObj );

	virtual void DrawOGL(C3DView * pView);
	virtual void DrawSelectOGL(C3DView  *pView, SelectableSettings& selSetting);

	void RenderToDepth(C3DView * pView);

	void RenderContours(C3DView * pView);


	void GetObject3DList(ALTObject3DList& objList)const;



private :
	int m_nID;

	ALTObject3DList m_vSurface;
	ALTObject3DList m_vStructure;
	ALTObject3DList m_vWallShape;
	ALTObject3DList m_vContour;
	ALTObject3DList m_vReportingArea;

};
