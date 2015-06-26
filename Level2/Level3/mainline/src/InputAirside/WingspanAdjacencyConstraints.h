#pragma once
#include "IntersectionNode.h"
#include "Taxiway.h"
class CAirportDatabase;
class ARCFlight;

class INPUTAIRSIDE_API  CWingspanAdjacencyConstraints
{
public:
	class INPUTAIRSIDE_API TaxiwayNodeItem
	{
	public:
		TaxiwayNodeItem()
		{
			m_nID = -1;
			m_dMinValue = 5;
			m_dMaxValue = 100;
			m_pAirportDB = NULL;

		}
		~TaxiwayNodeItem()
		{

		}

	public:
		int m_nID;
		Taxiway m_pTaxiway;
		IntersectionNode m_startNode;
		IntersectionNode m_endNode;
		double m_dMinValue;
		double m_dMaxValue;
		CAirportDatabase* m_pAirportDB;

		CString getTaxiwayName() const;

		CString getStartNodeName() const;

		CString getEndNodeName() const;

		bool FlightFit(const ARCFlight* pFlight)const;

		//database operation
		void ReadData(int nodeID);
		void RemoveData();
		void UpdateData();
		void SaveData();
	protected:
	private:
	};


public:
	CWingspanAdjacencyConstraints(void);
	~CWingspanAdjacencyConstraints(void);

	void InitFromDBRecordset(CADORecordset& recordset);
	void DeleteData();
	void UpdateData();
	void SaveData(int nParentID);
	void SetAirportDB(CAirportDatabase* pAirportDB);
public:
	int m_nID;
	TaxiwayNodeItem m_firstaxiway;
	TaxiwayNodeItem m_secondTaxiway;
};

class INPUTAIRSIDE_API CWingspanAdjacencyConstraintsList
{
public:
	CWingspanAdjacencyConstraintsList(CAirportDatabase* pAirportDB);
	~CWingspanAdjacencyConstraintsList();
public:
	void AddItem(CWingspanAdjacencyConstraints *pItem);
	void DelItem(CWingspanAdjacencyConstraints *pItem);
	int GetCount()const;
	CWingspanAdjacencyConstraints* GetConstraint(int idx)const;

	void LoadData(int nProjID);
	void SaveData(int nProjID);

protected:
	void ClearMemory();

protected:
	CAirportDatabase* m_pAirportDB;
	std::vector<CWingspanAdjacencyConstraints *> m_vConstraints;
	std::vector<CWingspanAdjacencyConstraints*> m_vDelConstraints;
};