#pragma once
#include "Parameters.h"
#include "../Common/termfile.h"
#include "../Common/ALTObjectID.h"
class AIRSIDEREPORT_API CFlightStandOperationParameter : public CParameters
{
public:
	CFlightStandOperationParameter();
	virtual ~CFlightStandOperationParameter();

public:
	virtual void LoadParameter();
	virtual void UpdateParameter();
	virtual CString GetReportParamName();

	virtual BOOL ExportFile(ArctermFile& _file);
	virtual BOOL ImportFile(ArctermFile& _file);

	void setSubType(int nSubType){m_nSubType = nSubType ;}
	int getSubType(){ return m_nSubType;}

	int getCount()const;
	void addItem(const ALTObjectID& objID);
	const ALTObjectID& getItem(int nIdx);
	void removeItem(const ALTObjectID& objID);
	void removeItem(int nIdx);
	void clear();
	bool fits(const ALTObjectID& objID)const;

protected:
	virtual void WriteParameter(ArctermFile& _file);
	virtual void ReadParameter(ArctermFile& _file);

protected:
	int   m_nSubType;
	std::vector<ALTObjectID> m_vStandIDList;
};