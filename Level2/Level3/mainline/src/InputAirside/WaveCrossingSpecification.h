#pragma once

#include "../Database/DBElement.h"
#include "../Database/DBElementCollection_Impl.h"
#include "InputAirsideAPI.h"

class CADORecordset;
class HoldPosition;
//cross runway, when next flight is take off
class INPUTAIRSIDE_API CWaveCrossingRunwayTakeOff
{
public:
	CWaveCrossingRunwayTakeOff();
	~CWaveCrossingRunwayTakeOff();

public:
	void ReadData(CADORecordset& adoRecordset);


public:
	bool GetMinimum() const;
	void SetMinimum(bool val);

	int GetAllowWaves() const;
	void SetAllowWaves(int val);

protected:
	// the mode , true or false
	bool m_bMinimum;

	// if not Minimum Waves, need to define n Waves
	int m_nAllowWaves;

private:
};

class INPUTAIRSIDE_API CWaveCrossingRunwayLanding
{
public:
	CWaveCrossingRunwayLanding();
	~CWaveCrossingRunwayLanding();

public:
	void ReadData(CADORecordset& adoRecordset);

	//
public:
	bool GetMinimum() const;
	void SetMinimum(bool val);


	int GetMinMiles() const;
	void SetMinMiles(int val);


	int GetMinSecons() const;
	void SetMinSecons(int val);

	int GetAllowWaves() const;
	void SetAllowWaves(int val);	
	
	int GetAllowSeconds() const;
	void SetAllowSeconds(int val);
protected:
	bool m_bMinimum;

	//if m_bminimum true
	int m_nMinMiles; // unit: nm
	int m_nMinSecons;//unit: second


	//if m_bminimum false
	int m_nAllowWaves;
	int m_nAllowSeconds;//unit: second


private:
};


class INPUTAIRSIDE_API CWaveCrossingSpecificationItem :public DBElement
{
public:
	CWaveCrossingSpecificationItem();
	~CWaveCrossingSpecificationItem();

public:
	CWaveCrossingRunwayLanding& GetLandingSettings();
	CWaveCrossingRunwayTakeOff& GetTakeoffSettings();

	CString GetName() const;
	void SetName(CString val);


	void AddHoldPosition(HoldPosition* holdNode);
	void DeleteHoldPosition(HoldPosition* holdNode);

	int GetHoldCount();

	HoldPosition *GetHoldPosition(int nIndex);
	//data access
public:
	virtual void InitFromDBRecordset(CADORecordset& recordset);

	void SaveData(int nProjectID);
	void DeleteData();
	
protected:
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const;
	
protected:
	//void UpdateData();
	
	//read hold position
	void ReadHoldPosition();
	void SaveHoldposition();
	void DeleteHoldPostion();




protected:
	CString m_strName;// the item name

	std::vector<HoldPosition *> m_vNodes;
	std::vector<HoldPosition *> m_vAddNodes;
	std::vector<HoldPosition *> m_vDelNodes;
	CWaveCrossingRunwayLanding m_landingSettings;


	CWaveCrossingRunwayTakeOff m_takeoffSettings;

	
private:
};


class INPUTAIRSIDE_API CWaveCrossingSpecification: public DBElementCollection<CWaveCrossingSpecificationItem>
{
public:
	CWaveCrossingSpecification(void);
	~CWaveCrossingSpecification(void);

public:
	CWaveCrossingSpecificationItem* AddNewItem(CWaveCrossingSpecificationItem* dbElement);
protected:
	//
	void GetSelectElementSQL(CString& strSQL) const;
};
























