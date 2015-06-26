#pragma once 
#include "altobject.h"
#include "..\Database\DBElementCollection_Impl.h"
#include "../Common/Point2008.h"

#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CTaxiInterruptLine : public DBElement
{
	friend class CDlgHoldShortLines;
	friend class CTaxiInterruptLine3D;

public:
	//Guid
	const CGuid& getGuid() const;
	static const GUID& getTypeGUID();

	CTaxiInterruptLine(void);
	virtual ~CTaxiInterruptLine(void);
protected:
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const;

public:
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	void UpdateData();

	int GetFrontIntersectionNodeID();
	int GetBackIntersectionNodeID();
	double GetDistToFrontIntersectionNode();
	void SetDistToFrontIntersectionNode(double dDistToFrontInterNode);
	CPoint2008 GetPosition();
	CString GetName();

	int GetTaxiwayID();

private:
	CGuid m_guid;

	int m_nFrontInterNodeID;		//frontal intersection node id
	int m_nBackInterNodeID;			//back intersection node id
	double m_dDistToFrontInterNode;	//relative distance to frontal intersection node.
	CString m_strName;				//name
	CString m_strDescription;		//pick shown
	CPoint2008 m_Position;			//coordinates
	int m_nTaxiwayID;
};


class INPUTAIRSIDE_API TaxiInterruptLineList : public DBElementCollection<CTaxiInterruptLine>
{
public:
	TaxiInterruptLineList(void);
	virtual ~TaxiInterruptLineList(void);

	CTaxiInterruptLine* GetTaxiInterruptLine(const CGuid& guid) const;
	CTaxiInterruptLine* GetTaxiInterruptLIneByID(int nID);
protected:
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;

private:
	void DoNotCall();
};