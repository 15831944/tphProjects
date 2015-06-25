#pragma once
#include "parameters.h"
#include "../common/elaptime.h"
#include "AirsideReportNode.h"
#include "SummaryParaFilter.h"

class AIRSIDEREPORT_API AirsideFlightConflictPara:
	public CParameters
{
public:
	AirsideFlightConflictPara(void);
	virtual ~AirsideFlightConflictPara(void);

public:
	virtual void LoadParameter();
	virtual void UpdateParameter();

	virtual BOOL ExportFile(ArctermFile& _file);
	virtual BOOL ImportFile(ArctermFile& _file);

	//area
	void ClearAreas();
	void AddArea(const ALTObjectID& altName);
	bool IsAreaFit(const ALTObjectID& altName);
	const std::vector<ALTObjectID>& GetAreaList(){ return m_vAreaPara; }

	virtual CString GetReportParamName();
public:
	std::vector<ALTObjectID> m_vAreaPara;

	void setSubType(int nSubType){m_nSubType = nSubType ;}
	int getSubType(){ return m_nSubType;}

	void setSummaryFilterPara(int nConType, int nAreaIdx, int nLocType, int OpType, int ActType);
	SummaryDataFilter* getSummaryDataFilter() const;

	virtual void WriteParameter(ArctermFile& _file);
	virtual void ReadParameter(ArctermFile& _file);

protected:
	int   m_nSubType;
	SummaryDataFilter* m_pSummaryFilter;





};
