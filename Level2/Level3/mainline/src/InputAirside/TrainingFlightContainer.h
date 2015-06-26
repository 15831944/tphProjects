#pragma once
#include "../Common/ALTObjectID.h"
#include "../Common/ProDistrubutionData.h"
#include "../Common/TimeRange.h"
#include "../Database/DBElement.h"
#include "../Database/DBElementCollection_Impl.h"
#include "InputAirsideAPI.h"

class CADORecordset;

class INPUTAIRSIDE_API CTrainingFlightData : public DBElement
{
public:
	CTrainingFlightData();
	virtual ~CTrainingFlightData();

	struct Interntion 
	{
		Interntion()
			:m_nNum(0)
			,m_dValue(0.0)
		{

		}
		int		m_nNum;
		double	m_dValue;

		CString getString()const
		{
			CString strValue;
			strValue.Format(_T("%d,%f"),m_nNum,m_dValue);
			return strValue;
		}
		void ParseString(const CString& strFlight)
		{
			int nPos = strFlight.Find(',');
			if (nPos != -1)
			{
				CString strLeft;
				CString strRight;
				strLeft = strFlight.Left(nPos);
				m_nNum = atoi(strLeft);
				int nLengh = strFlight.GetLength();
				strRight = strFlight.Right(nLengh - nPos - 1);
				m_dValue = atof(strRight);
			}
		}
	};

public:
	void SetName(const CString& sName);
	const CString& GetName()const;

	void SetCarrierCode(const CString& sCode);
	const CString& GetCarrierCode()const;

	void SetAcType(const CString& sAcType);
	const CString& GetAcType()const;

	void SetNumFlight(int nNumFlight);
	int GetNumFlight()const;

	void SetStartPoint(const ALTObjectID& objID);
	const ALTObjectID& GetStartPoint()const;

	void SetDistribution(CProbDistEntry* pPDEntry);
	const CProDistrubution& GetDistribution()const;

	void SetTimeRange(const ElapsedTime& startTime, const ElapsedTime& endTime);
	const TimeRange& GetTimeRange()const;

	CString GetTimeRaneString() const;

	void SetEndPoint(const ALTObjectID& endPoint);
	const ALTObjectID& GetEndPoint()const;

	void SetLowFlight(int nNumFlight);
	int GetLowFlight()const;

	void SetLowValue(double dValue);
	double GetLowValue()const;

	void SetTouchFlight(int nNumFlight);
	int GetTouchFlight()const;

	void SetTouchValue(double dValue);
	double GetTouchValue()const;

	void SetStopFlight(int nNumFlight);
	int GetStopFlight()const;

	void SetStopValue(CProbDistEntry* pPDEntry);
	const CProDistrubution& GetStopValue()const;

	void SetStartStand(bool bStand);
	bool GetStartType()const;

	void SetEndStand(bool bStand);
	bool GetEndType()const;

	void SetForceGo(int nNum);
	int GetForceGo()const;


	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void SaveData(int nParentID);
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const;

private:
	CString				m_sName;
	CString				m_sCarrierCode;
	CString				m_sAcType;
	int					m_nNumFlight;
	bool				m_bStartStand;
	ALTObjectID			m_startPointID;
	CProDistrubution	m_distribution;
	Interntion			m_lowInterntion;
	Interntion			m_touchInterntion;
	int					m_nStopNum;
	CProDistrubution	m_stopDist;
	bool				m_bEndStand;
	ALTObjectID			m_endPointID;
	TimeRange			m_timeRange;
	int m_nForceNum;
};
class INPUTAIRSIDE_API CTrainingFlightContainer : public DBElementCollection<CTrainingFlightData>
{
public:
	CTrainingFlightContainer(void);
	virtual ~CTrainingFlightContainer(void);

	virtual void GetSelectElementSQL(CString& strSQL) const;
};
