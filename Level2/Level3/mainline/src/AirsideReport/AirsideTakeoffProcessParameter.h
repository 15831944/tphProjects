#pragma once
#include "parameters.h"

class AIRSIDEREPORT_API CAirsideTakeoffProcessParameter :
	public CParameters
{
public:
	CAirsideTakeoffProcessParameter(void);
	virtual ~CAirsideTakeoffProcessParameter(void);

public:
	void setSubType(int nSubType){m_nSubType = nSubType ;}
	int getSubType(){ return m_nSubType;}

	virtual void LoadParameter();
	virtual void UpdateParameter();

	virtual BOOL ExportFile(ArctermFile& _file);
	virtual BOOL ImportFile(ArctermFile& _file);

	int GetPositionCount()const;
	void AddPosition(const CString& sPosition);
	void ClearPosition();
	void DeletePosition(int nIdx);
	CString GetPosition(int nIdx)const;

	bool fit(const CString& sTakeoff,const ElapsedTime& time);
	CString GetReportParamName();
protected:
	virtual void WriteParameter(ArctermFile& _file);
	virtual void ReadParameter(ArctermFile& _file);

private:
	std::vector<CString> m_vTakeoffPosition;

};
