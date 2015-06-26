#pragma once
#include "HoldPosition.h"
#include "InputAirsideAPI.h"

#include "./IntersectionNode.h"
class RunwayExitDescription;

class INPUTAIRSIDE_API RunwayExit
{
public:
	enum ExitSide { EXIT_LEFT, EXIT_RIGHT,};
	
	RunwayExit();
	RunwayExit(const RunwayExit&);
	RunwayExit& operator =(const RunwayExit&);
	~RunwayExit();

public:

	void SaveData(int nRunwayID);
	void UpdateData();
	void DeleteData();

	void ReadData(CADORecordset& adoRecordset);
	void ReadData(int nRunwayExitID);

	void ExportRunwayExit(CAirsideExportFile& exportFile,int nRunwayID);
	void ImportRunwayExit(CAirsideImportFile& importFile);

	static void ReadExitsList(int rwId, RUNWAY_MARK rwMark, std::vector<RunwayExit>& reslt ); 
	static void ReadExitsList(int rwId, std::vector<RunwayExit>& reslt );
	static void ExportRuwayExits(CAirsideExportFile& exportFile);
	static void ImportRunwayExits(CAirsideImportFile& importFile);
	
	void SetID(int nID);
	int GetID()const;

public:
	CString GetName()const;
	int GetRunwayID()const;
	IntersectionNode& GetIntesectionNode();
	IntersectionNode GetIntesectionNode()const;

	int GetTaxiwayID()const;
	ExitSide GetSideType()const;
	double GetAngle()const;
	RUNWAY_MARK GetRunwayMark()const;
	int GetIntersectNodeID()const;

	void SetName(CString strName);
	void SetExitSide(ExitSide side);
	void SetRunwayMark(RUNWAY_MARK rwMark);
	void SetAngle(double dAngle);
	void SetRunwayID(int nID);
	//intersection taxiway or runway
	void SetIntersectRunwayOrTaxiwayID(int nID);
	void SetIntersectNode(const IntersectionNode& node);
	
	//check is the identical exit 
	bool IsIdent(const RunwayExit& otherExit)const;
	bool EqualToRunwayExitDescription(const RunwayExitDescription& runwayDes)const;
protected:
	int m_nID;

	//data
	ExitSide m_side;	//key
	RUNWAY_MARK m_rwMark;  //key
	CString m_strName;   
	double m_dAngle;   // degree
	int m_nRunwayID;   //key
	int m_nRunwayOrTaxiwayID;   //key
	IntersectionNode m_intersectNode; //key
};


typedef std::vector<RunwayExit> RunwayExitList;

