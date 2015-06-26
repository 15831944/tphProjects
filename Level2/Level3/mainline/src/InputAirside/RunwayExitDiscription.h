#pragma once
#include "RunwayExit.h"
#include "IntersectionNode.h"
class RunwayExit;
class INPUTAIRSIDE_API RunwayExitDescription
{
public:
	RunwayExitDescription();
	~RunwayExitDescription();

public:
	void SaveData();
	void UpdateData();
	void DeleteData();

	void ReadData(int nID);

public:
	void InitData(const RunwayExit& rwyExit);
	int GetIndex()const;
	void SetID(int nID);
	int GetID()const;

	CString GetName()const;
	int GetRunwayID()const;
	const CString& GetRunwayName()const;
	int GetTaxiwayID()const;
	const CString& GetTaxiwayName()const;
	RunwayExit::ExitSide GetSideType()const;
	double GetAngle()const;
	RUNWAY_MARK GetRunwayMark()const;

	void SetName(CString strName);
	void SetExitSide(RunwayExit::ExitSide side);
	void SetRunwayMark(RUNWAY_MARK rwMark);
	void SetAngle(double dAngle);
	void SetRunwayID(int nID);
	void SetRunwayName(const CString& sRunway);
	//intersection taxiway or runway
	void SetIntersectRunwayOrTaxiwayID(int nID);
	void SetTaxiwayName(const CString& sTaxiway);

	bool EqualToRunwayExit(const RunwayExit& runwayExit)const;

protected:
	int m_nID;

	//data
	RunwayExit::ExitSide m_side;	//key
	RUNWAY_MARK m_rwMark;  //key
	CString m_strName;   
	double m_dAngle;   // degree
	int m_nRunwayID;   //key
	CString m_sRunway;
	int m_nRunwayOrTaxiwayID;   //key
	CString m_sTaxiway;
	int m_nIndex; //for intersection node
	std::vector<IntersectionNode>m_vResult;
};
