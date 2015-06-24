#pragma once
#include <map>
#include <set>
#include "..\Common\Referenced.h"
#include "..\Common\ref_ptr.h"

#include "../Common/ARCVector.h"

class C3DView;
class ALTObject3D;

class Selectable :public Referenced{
public:

	Selectable();

	enum SelectType{
		ALT_OBJECT,
		ALT_PATH_CONTROLPOINT,
		ALT_OBJECT_PATH_CONTROLPOINT,
		ALT_OBJECT_POINT_CONTROLPOINT,
		ALT_OBJECT_MARK,
		FILLET_TAXIWAY,
		FILLET_POINT,
		INTERSECTED_STRETCH,
		INTERSECTED_POINT,
		HOLD_LINE,
	};

	virtual void Selected(bool b = true){ m_bSelected = b; }
	virtual bool IsSelected()const{  return m_bSelected; } 
	
	virtual void DrawSelect(C3DView * pView){}

	virtual void EnableSelect(bool b = true) { m_bEnableSelect = b; }
	virtual bool IsEnableSelect()const{ return m_bEnableSelect; }
	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);
	virtual void DeleteEditPoint(ALTObject3D* pObj3D){}
	virtual void FlushChange(){}
	virtual void AfterMove();

	virtual SelectType GetSelectType()const = 0 ;


	virtual ARCPoint3 GetLocation()const=0;
	bool IsOnMove()const{ return m_bOnMove; }

private:
	bool m_bSelected;
	bool m_bEnableSelect;
	bool m_bOnMove;
};

//typedef std::set< ref_ptr<Selectable> > SelectableList;
class ALTObject3DList;
class SelectableList : public std::vector< ref_ptr<Selectable> >
{
public:
	void insert(Selectable * pSel);
	void GetSelectObjects(ALTObject3DList& list);

};

// map of object id --- gl select id

class CSelectionMap 
{
public:
	CSelectionMap(void);
	virtual ~CSelectionMap(void);

	GLuint NewSelectable(Selectable * psel);	
	
	GLint GetSelectID(Selectable * psel);

	Selectable *  GetSelectable(GLint selID)const;

	void Clear();


	typedef std::map<Selectable * ,GLuint>  SelectionMapType;
private:

	SelectionMapType  m_map;
 };
