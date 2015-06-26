#pragma once
#include "ALTObject.h"

class Runway;
class Taxiway;
class Stand;
class DeicePad;
class Stretch;
class CPath2008;
class CircleStretch;

//////////////////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API IntersectItem
{
public:
	enum TYPE{ 
			   RUNWAY_INTESECT=0 , 
		       TAXIWAY_INTERSECT,
			   STAND_LEADINLINE_INTERSECT,
			   STAND_LEADOUT_LINEINTERSECT, 
			   STRETCH_INTERSECT, 
			   CIRCLESTRETCH_INTERSECT,
			   DEICE_INCONSTRAIN,
			   DEICE_OUTCONSTRAIN,
			 };

	IntersectItem();
	virtual TYPE GetType()const=0;
	virtual int GetObjectID()const;

	virtual int GetStandLeadInOutLineID()const;
	int GetUID()const;
	void SetUID(int nID){ m_nUID = nID; }
	DistanceUnit GetDistInItem()const;
	void SetDistInItem(const DistanceUnit& dist){ m_dDistInItem = dist; }

	void SetObjectID(int nObjID){ m_nObjectID = nObjID; }
	ALTObject* GetObject();
	void SetObject(ALTObject* pObj);

	void SaveData(int nParentID);
	void DeleteData();

	virtual CString GetNameString();
	virtual CPath2008 GetPath() = 0;

	static std::vector<IntersectItem*> ReadData(int nParentID);
	static IntersectItem* ReadData(CADORecordset& recordset);

	bool IsIdent(IntersectItem* pItem);
	static bool CompareLess(IntersectItem* pItem1, IntersectItem*pItem2);

	virtual IntersectItem* Clone()const = 0;

	class IDComparor
	{
	public:
		bool operator()(const IntersectItem* lhs, const IntersectItem* rhs)
		{
			return lhs->GetObjectID() == rhs->GetObjectID();
		}
	};
	
protected:
	int m_nUID; // unique id
	DistanceUnit m_dDistInItem;
	int m_nObjectID;
	ALTObject::RefPtr m_pObject;

	void GetInsertSQL(int nParentID,CString& strSQL) ;
	void GetUpdateSQL(CString& strSQL) ;
	void GetDeleteSQL(CString& strSQL) ;
	static void GetSelectSQL(int nParentID,CString& strSQL);

};

//////////////////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API RunwayIntersectItem : public IntersectItem
{

public:
	virtual TYPE GetType()const{ return RUNWAY_INTESECT; }

	void SetRunway(Runway*pRunway);
	Runway* GetRunway();

	bool operator < ( const RunwayIntersectItem& otherItem)const;
	
	virtual IntersectItem* Clone()const;
	virtual CPath2008 GetPath();
protected:

};

//////////////////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API TaxiwayIntersectItem : public IntersectItem
{
public:
	virtual TYPE GetType()const{ return TAXIWAY_INTERSECT; }

	void SetTaxiway(Taxiway* pTaxiway);
	Taxiway * GetTaxiway();

	bool operator < ( const TaxiwayIntersectItem& otherItem)const;

	virtual CString GetNameString();
	virtual CPath2008 GetPath();
	virtual IntersectItem* Clone()const;
protected:

};

//////////////////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API StretchIntersectItem : public IntersectItem
{
public:
	virtual TYPE GetType()const {return STRETCH_INTERSECT;}
	
	void SetStretch(Stretch* pStretch); 

	Stretch * GetStretch();

	bool operator < (const StretchIntersectItem& otherItem)const;

	virtual CPath2008 GetPath();
	virtual IntersectItem* Clone()const;

protected:

};

//////////////////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API CircleStretchIntersectItem :public IntersectItem
{
public:
	virtual TYPE GetType()const {return CIRCLESTRETCH_INTERSECT;}
	void SetCircleStretch(CircleStretch* pCirStretch);
	CircleStretch * GetCircleStretch();
	bool operator < (const CircleStretchIntersectItem& otherItem)const;
	virtual CPath2008 GetPath();
	virtual IntersectItem* Clone()const;
};

//////////////////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API StandLeadInLineIntersectItem : public IntersectItem
{
public:
	virtual TYPE GetType()const{ return STAND_LEADINLINE_INTERSECT ; }

	virtual int GetStandLeadInOutLineID()const{ return m_nLeadLineID; }

	void SetStand(Stand* pStand);
	void SetLeadInLineID(int id){ m_nLeadLineID = id; }
	Stand * GetStand();

	virtual CString GetNameString();
	virtual CPath2008 GetPath();

	bool operator < ( const StandLeadInLineIntersectItem& otherItem)const;

	virtual IntersectItem* Clone()const;

protected:
	int m_nLeadLineID;
};

//////////////////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API StandLeadOutLineIntersectItem : public IntersectItem
{
public:
	virtual TYPE GetType()const{ return STAND_LEADOUT_LINEINTERSECT; }

	virtual int GetStandLeadInOutLineID()const{ return m_nLeadLineID; }
	void SetStand(Stand* pStand);
	void SetLeadOutLineID(int id){ m_nLeadLineID = id; }
	Stand * GetStand();

	virtual CString GetNameString();
	virtual CPath2008 GetPath();

	bool operator < ( const StandLeadInLineIntersectItem& otherItem);
	
	virtual IntersectItem* Clone()const;
protected:
	int m_nLeadLineID;
};


class DeicePad;
//////////////////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API DeicePadInContrainItem : public IntersectItem
{
public:
	virtual TYPE GetType()const{ return DEICE_INCONSTRAIN; }

	void SetDeicePad(DeicePad* pdeicepad);
	DeicePad* GetDeicePad(){ return (DeicePad*) GetObject(); }

	virtual CString GetNameString();
	virtual CPath2008 GetPath();

	bool operator < (const DeicePadInContrainItem&otherItem )const;
	virtual IntersectItem* Clone()const;
};

//////////////////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API DeicePadOutConstrainItem : public IntersectItem
{
public:
	virtual TYPE GetType()const{ return  DEICE_OUTCONSTRAIN; }

	
	void SetDeicePad(DeicePad* pdeicepad);
	DeicePad* GetDeicePad(){ return (DeicePad*) GetObject(); }

	bool operator < (const DeicePadOutConstrainItem& otherItem)const;

	virtual CString GetNameString();
	virtual CPath2008 GetPath();
	virtual IntersectItem* Clone()const;
};