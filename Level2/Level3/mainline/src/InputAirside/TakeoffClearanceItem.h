#pragma once

#include <vector>

#include "ApproachSeparationType.h"
#include "AircraftClassifications.h"
#include "InputAirsideAPI.h"

class CADORecordset;

enum TakeoffClearanceType
{
	TakeoffBehindTakeoffSameRW = 0,
	TakeoffBehindTakeoffIntersectRW,
	TakeoffBehindLandingSameRW,
	TakeoffBehindLandingIntersectRW,
};


class INPUTAIRSIDE_API TakeoffClearanceClassfiBasisItem
{
public:

	TakeoffClearanceClassfiBasisItem()
		:m_nID(-1)
		,m_nProjID(-1)
		,m_nTakeoffClearanceItemID(-1)
		,m_nMinSeparation(0)
		,m_emClassifiBasis(WingspanBased)
		{
			m_bImport = false;
		}
	TakeoffClearanceClassfiBasisItem(int nProjID, int nTakeoffClearanceItemID, FlightTypeStyle emTrail, 
		FlightTypeStyle emLead, FlightClassificationBasisType emClassifiBasis, int nMinSeparation)
		:m_nID(-1)
		,m_nTakeoffClearanceItemID(nTakeoffClearanceItemID)
		,m_nProjID(nProjID)
		,m_nMinSeparation(nMinSeparation)
		,m_emClassifiBasis(emClassifiBasis)
	{}

	~TakeoffClearanceClassfiBasisItem(){}

public:
	bool m_bImport;//Peter,2007.07.17,for import and save data,using id in right way.
	int m_nID;
	int m_nProjID;
	int m_nTakeoffClearanceItemID;
	
	int m_nLeadID;
	int m_nTrailID;
	FlightClassificationBasisType    m_emClassifiBasis;
	int	m_nMinSeparation;

public:
	void ReadData(CADORecordset& adoRecordset);
	void SaveData(int nTakeOffClearanceID);
	void UpdateData();
	void DeleteData();
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
};



class INPUTAIRSIDE_API TakeoffClearanceItem
{

public:
	typedef std::vector<TakeoffClearanceClassfiBasisItem*> TakeoffClearanceBasisVector;
	typedef std::vector<TakeoffClearanceClassfiBasisItem*>::iterator TakeoffClearanceBasisIter;

public:
	TakeoffClearanceItem(int nProjID, TakeoffClearanceType emType);
	virtual ~TakeoffClearanceItem(void);

	virtual void InsertDefaultValue(){};
	void InsertDefaultBasisItem();
	virtual void ReadData(CADORecordset& adoRecordset);
	virtual void SaveData(){};
	void DeleteData();
	virtual void ImportData(CAirsideImportFile& importFile);
	virtual void ExportData(CAirsideExportFile& exportFile);

	void ReadClassficBasisData();
	void SaveClassFicBasisData();

	TakeoffClearanceType GetType(){	return m_emType; }

	void GetItemsByType(TakeoffClearanceBasisVector& vect, FlightClassificationBasisType emType);

	int	 GetDistTakeoffApproach()			{ return m_nDistTakeoffApproach; }
	void SetDistTakeoffApproach(int nValue)	{	m_nDistTakeoffApproach = nValue; }
	int	 GetTimeTakeoffAproach()			{ return m_nTimeTakeoffAproach; }
	void SetTimeTakeoffAproach(int nValue)	{	m_nTimeTakeoffAproach = nValue; }
	int	 GetTimeTakeoffSlot()				{ return m_nTimeTakeoffSlot; }
	void SetTimeTakeoffSlot(int nValue)		{	m_nTimeTakeoffSlot = nValue; }
	
	void SetCurrentFlightClassificationBasisType(FlightClassificationBasisType emType)
	{
		m_emClassficType = emType;
	}
	FlightClassificationBasisType GetCurrentFltClassificationType()
	{
		return m_emClassficType;
	}

	void SetDistTakeoffApproachFlag(BOOL bValue){	m_bDistTakeoffApproach = bValue; }
	BOOL IsDistTakeoffApproach()			{ return m_bDistTakeoffApproach; }

protected:

	bool m_bImport;//Peter,2007.07.17,for import and save data,using id in right way.
	int m_nID;
	int m_nProjID;
	TakeoffClearanceType m_emType;

	BOOL m_bDistTakeoffApproach;

	int m_nDistTakeoffApproach;
	int m_nTimeTakeoffAproach;
	int m_nTimeTakeoffSlot;

	FlightClassificationBasisType m_emClassficType;//Current Type
	TakeoffClearanceBasisVector m_vectBasis;
};


class INPUTAIRSIDE_API TakeoffBehindTakeoffSameRunwayOrCloseParalled : 
	public TakeoffClearanceItem
{
public:
	TakeoffBehindTakeoffSameRunwayOrCloseParalled(int nProjID, TakeoffClearanceType emType);
	~TakeoffBehindTakeoffSameRunwayOrCloseParalled(void);

	virtual void ReadData(CADORecordset& adoRecordset);
	virtual void SaveData();
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
	virtual void InsertDefaultValue();
	void UpdateData();

	int	 GetFtLanderDownRunway()			{ return m_nFtLanderDownRunway; }
	void SetFtLanderDownRunway(int nValue)	{	m_nFtLanderDownRunway = nValue; }
	int	 GetTimeAfter()						{ return m_nTimeAfter; }
	void SetTimeAfter(int nValue)			{	m_nTimeAfter = nValue; }
	BOOL IsDistAtLeast()					{	return m_bDistAtLeast; }
	void SetDistAtLeastFlag(BOOL bValue)	{	m_bDistAtLeast = bValue; }
private:

	BOOL m_bDistAtLeast;
	
	int m_nFtLanderDownRunway;
	int m_nTimeAfter;


};

class INPUTAIRSIDE_API TakeoffBehindTakeoffIntersectingConvergngRunway : 
	public TakeoffClearanceItem
{
public:
	TakeoffBehindTakeoffIntersectingConvergngRunway(int nProjID, TakeoffClearanceType emType);
	~TakeoffBehindTakeoffIntersectingConvergngRunway(void);
	virtual void InsertDefaultValue();
	virtual void ReadData(CADORecordset& adoRecordset);
	virtual void SaveData();
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
	void UpdateData();
};


class INPUTAIRSIDE_API TakeoffBehindLandingSameRunway : 
	public TakeoffClearanceItem
{
public:
	TakeoffBehindLandingSameRunway(int nProjID, TakeoffClearanceType emType);
	~TakeoffBehindLandingSameRunway(void);

	virtual void InsertDefaultValue();
	virtual void ReadData(CADORecordset& adoRecordset);
	virtual void SaveData();
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
	void UpdateData();

	int	 GetFtLanderDownRunway()			{ return m_nFtLanderDownRunway; }
	void SetFtLanderDownRunway(int nValue)	{	m_nFtLanderDownRunway = nValue; }
	
private:
	int m_nFtLanderDownRunway;
};


class INPUTAIRSIDE_API TakeoffBehindLandingIntersectingConvergingRunway : 
	public TakeoffClearanceItem
{
public:
	TakeoffBehindLandingIntersectingConvergingRunway(int nProjID, TakeoffClearanceType emType);
	~TakeoffBehindLandingIntersectingConvergingRunway(void);
	virtual void InsertDefaultValue();

	virtual void ReadData(CADORecordset& adoRecordset);
	virtual void SaveData();
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
	void UpdateData();
};
