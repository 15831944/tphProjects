#pragma once
#include "ALTObjectID.h"
#include "Guid.h"
#include "Referenced.h"
#include "ref_ptr.h"
#include "ALT_TYPE.h"
#include "BaseObject.h"
///////////////////////////layout object interface///////////////////////////////////////////////
class ILayoutObjectDisplay;
class ILayoutObject : public virtual CGUIDObject, public Referenced
{
public:
	typedef ref_ptr<ILayoutObject> RefPtr;
	ILayoutObject();
	virtual ~ILayoutObject(){}
	virtual ALTOBJECT_TYPE GetType() const=0;
	bool GetLocked() const{ return m_bLocked; }
	void SetLocked(bool bLocked = true){ m_bLocked = bLocked; }
protected:
	bool m_bLocked; // identifies whether the processor is locked individually

};

class IFacilityObject 
{
public:
	virtual ~IFacilityObject(){}

	ALTObjectID& getName(){ return m_str4Name; }
	const ALTObjectID& getName()const{ return m_str4Name; }
protected:
	ALTObjectID m_str4Name;
};

class ILayoutObjectList
{
public:
	virtual int getCount()const=0;
	virtual ILayoutObject * getILayoutObject(int idx)=0; 
};


//////////////////////////layout object display prop interface////////////////////////////////////////////////
#include <common/ARCColor.h>
#include <map>
class ILayoutObjectDisplay 
{
public:
	class DspProp
	{
	public:
		DspProp();
		DspProp(const ARCColor4& c, bool b = true);
		bool bOn;
		ARCColor4 cColor;
	};
	enum {
		_Shape = -2,
		_Name = -1,
	};		
	ILayoutObjectDisplay();
	typedef std::map<int, DspProp> DspPropMap;

	virtual CString GetPropTypeString(int nPropType);
	virtual void InitDefault();
	DspProp& GetProp(int nPropType){ return m_map[nPropType]; }	
	virtual int GetPropTypeEnd()const{ return 0; }
protected:
	DspPropMap m_map;
};

