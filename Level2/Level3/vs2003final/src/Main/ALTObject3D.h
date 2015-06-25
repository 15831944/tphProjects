#pragma once

#include <vector>
#include <gl/glew.h>


#include ".\SelectionMap.h"
#include "..\Common\ARCVector.h"
#include "..\Common\SyncDateType.h"
#include "../InputAirside/ALTObject.h"
#include "../InputAirside/ALTObjectDisplayProp.h"

class C3DView;

class ALTObject3D :public Selectable, public SyncDateType
{
public:
	typedef ref_ptr<ALTObject3D> RefPtr;
	ALTObject3D(int _Id );
	
	virtual ~ALTObject3D(void);

	ALTObject* GetObject()const;	

	ALTObjectDisplayProp *GetDisplayProp();
	const ALTObjectDisplayProp *GetDisplayProp()const;
	void SetInEdit(bool b = true){ m_bInEdit = b ; }
	inline bool IsInEdit()const { return m_bInEdit; }


	virtual void DrawOGL(C3DView * m_pView){}
	virtual void AddEditPoint(double x, double y, double z){}
		
	int GetID()const
	{ 
		return m_nID; 
	}
	void SetID(int nID){ m_nID = nID; }

	virtual SelectType GetSelectType()const ;
	ALTOBJECT_TYPE GetObjectType()const{ if(m_pObj.get()) return m_pObj->GetType(); return ALT_UNKNOWN; }

	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);
	virtual void OnRotate(DistanceUnit dx){}
	virtual void AfterMove();

	virtual ARCPoint3 GetLocation()const  ;

	virtual ARCPoint3 GetSubLocation(int index)const ;

    //virtual void AddThickness(const Path& path,Path& newPath);

	// write data to database
	virtual void FlushChange();

	// update data from database
	virtual void Update();

	// sync geo date for drawing
	virtual void DoSync();

	virtual CString GetDisplayName()const;

	static ALTObject3D * NewALTObject3D(ALTObject * pObj);
protected:
	int m_nID;
	ALTObject::RefPtr m_pObj;
	
	bool m_bUpdated;
	bool m_bInEdit;
	//double m_dThick;
};


class ALTObject3DList : public std::vector< ALTObject3D::RefPtr > 
{
public:
	ALTObject3DList(){}

	ALTObject3D * GetALTObject3D(int objID);

	ALTObjectList GetObjectList()const;

	bool HasType(ALTOBJECT_TYPE objType)const;
};

struct ALTObject3DIDIs{
	ALTObject3DIDIs(int nID){ m_nID = nID; }
	bool operator()(const ref_ptr<ALTObject3D>& pObj )const{
		ASSERT(pObj.get());		
		return pObj->GetID() == m_nID;
	}
	int m_nID;
};
