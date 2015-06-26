#pragma once
#include "..\Common\TimeRange.h"
#include "RunwayExit.h"
#include "..\Database\DBElementCollection.h"



class INPUTAIRSIDE_API AirsideEnrouteQCapacityItem : public DBElement
{
public:
	AirsideEnrouteQCapacityItem();
	~AirsideEnrouteQCapacityItem();

public:
	void setTakeOffPosition(int nExitID);
	RunwayExit *getTakeOffPosition();

	//meter -> cm
	void setMaxLength(double dMaxLength);
	//cm ->m
	double getMaxLength() const;

public:
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const;
	void InitFromDBRecordset(CADORecordset& recordset);
protected:
	RunwayExit m_takeOffPosition;
	//the unit is cm in database
	double		m_dMaxLength;

};

class INPUTAIRSIDE_API AirsideEnrouteQCapacityItemList : public DBElementCollection< AirsideEnrouteQCapacityItem >
{
public:
	AirsideEnrouteQCapacityItemList();
	~AirsideEnrouteQCapacityItemList();

public:
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;

	AirsideEnrouteQCapacityItem* AddNewItem(AirsideEnrouteQCapacityItem* pEnrouteQItem);
};



class INPUTAIRSIDE_API  AirsideEnrouteQCapacityTimeRange :public DBElement 
{
public:
	AirsideEnrouteQCapacityTimeRange() {}
	~AirsideEnrouteQCapacityTimeRange(){}

public:
	void SetTimeRange(ElapsedTime eStart, ElapsedTime eTimeEnd);

	TimeRange* getTimeRange() {return &m_timeRange;}

	AirsideEnrouteQCapacityItemList* getEnrouteQCapacity();
public:
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const;
	void InitFromDBRecordset(CADORecordset& recordset);

	virtual void SaveData(int nParentID);


protected:
	TimeRange m_timeRange;
	AirsideEnrouteQCapacityItemList m_enRouteData;
};


class INPUTAIRSIDE_API CAirsideEnrouteQueueCapacity : public DBElementCollection< AirsideEnrouteQCapacityTimeRange >
{
public:
	CAirsideEnrouteQueueCapacity(void);
	~CAirsideEnrouteQueueCapacity(void);

public:
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;

	AirsideEnrouteQCapacityTimeRange* AddNewItem(AirsideEnrouteQCapacityTimeRange* pTimeRangeData);
};
