#pragma once
#include "Parameters.h"

#include "../Common/ALTObjectID.h"
class AirsideRunwayCrossigsLog;

class AIRSIDEREPORT_API AirsideRunwayCrossingsParameter : public CParameters
{
public:
	class AIRSIDEREPORT_API RunwayCrossingsItem
	{
	public:
		RunwayCrossingsItem();
		~RunwayCrossingsItem();
	
		int getCount()const;
		void addItem(const ALTObjectID& objID);
		void deleteItem(int nIdx);
		ALTObjectID& getItem(int nIdx);

		void setRunway(const ALTObjectID& strObjName);
		ALTObjectID& getRunway();

		void WriteParameter(ArctermFile& _file);
		void ReadParameter(ArctermFile& _file);

		bool fit(AirsideRunwayCrossigsLog* pLog);
	private:
		ALTObjectID m_RunwayItem;
		std::vector<ALTObjectID> m_vTaxiway;
	};
public:
	AirsideRunwayCrossingsParameter(void);
	~AirsideRunwayCrossingsParameter(void);

public:
	virtual void LoadParameter();
	virtual void UpdateParameter();


public:
	virtual CString GetReportParamName();
public:
	void setSubType(int nSubType){m_nSubType = nSubType ;}
	int getSubType(){ return m_nSubType;}

	int getCount()const;
	AirsideRunwayCrossingsParameter::RunwayCrossingsItem& getItem(int nIdx);
	void addItem(const RunwayCrossingsItem& item);
	void deleteItem(int nIdx);
	void clear();

	bool fit(AirsideRunwayCrossigsLog* pLog);

protected:
	int   m_nSubType;
	std::vector<RunwayCrossingsItem> m_vRunwayCrossings;

public:
	virtual BOOL ExportFile(ArctermFile& _file);
	virtual BOOL ImportFile(ArctermFile& _file);

protected:
	virtual void WriteParameter(ArctermFile& _file);
	virtual void ReadParameter(ArctermFile& _file);
};