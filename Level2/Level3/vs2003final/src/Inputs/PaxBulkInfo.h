// PaxBulkInfo.h: interface for the CPaxBulkInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAXBULKINFO_H__C692A542_55DB_4067_B88B_C6E8877FB588__INCLUDED_)
#define AFX_PAXBULKINFO_H__C692A542_55DB_4067_B88B_C6E8877FB588__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\template.h"
#include "common\dataset.h"
#include "inputsdll.h"
#include "inputs\MobileElemConstraint.h"
class CPaxBulk
{
public:
	CMobileElemConstraint m_mobileElemConst;	
	ElapsedTime   m_TimeFrequency;
	ElapsedTime   m_TimeRangeBegin;
	ElapsedTime   m_TimeRangeEnd;
	ElapsedTime   m_TimeStart;
	ElapsedTime   m_TimeEnd;
	
	int m_RangeTimeSign;
	int   m_nCapacity;
	std::vector<int> m_vBulkSeats;// use to flag how many  bulk seats can be used.
	int m_nBulkIndex; // car index
	std::vector<int> m_vBulkPercent;
public:
	bool fits(const CMobileElemConstraint& _const);

	//either all shuttle buses have departed or passenger can't take later bus, return false;
	//or return true;
	bool takeCurrentBulk(ElapsedTime& time ,long& nIndex, int _nPaxGroup,const ElapsedTime& flyTime);
	bool GetRangeStartIndex(long& nIndex,const ElapsedTime& flyTime);
	bool GetStartEndRange(long& iStart, long& iEnd, long lIndex,const ElapsedTime& flyTime);

	bool TimeOverLap(const ElapsedTime& flyTime)const;

	CPaxBulk()
	{
		m_nCapacity = 50;
		m_TimeFrequency.SetTime("1:0:0");
		m_TimeStart.SetTime("5:0:0");
		m_TimeEnd.SetTime("23:0:0");
		m_TimeRangeBegin.SetTime("1:0:0");
		m_TimeRangeEnd.SetTime("16:0:0");
		m_nBulkIndex = 0;
		m_RangeTimeSign =0;
	}

	const CMobileElemConstraint& GetMobileElementConstraint()const { return m_mobileElemConst ;} ;
};


class CPaxBulkInfo :public DataSet
{
public:
//	virtual void saveDataSet (const CString& _pProjPath, bool _bUndo ) const;
//	virtual void loadDataSet (const CString& _pProjPath);
	CPaxBulkInfo();
	virtual ~CPaxBulkInfo();
	std::vector<CPaxBulk> m_vectBulkInfo;	

	// clear data structure before load data from the file.
	virtual void clear(){ m_vectBulkInfo.clear(); };
	virtual void readData( ArctermFile& p_file );
	virtual void readObsoleteData(ArctermFile&p_file);
	virtual void writeData( ArctermFile& p_file )const ;
    virtual void initDefaultValues(){};
	
	virtual const char *getTitle () const {return " PaxBulk  " ;};
	virtual const char *getHeaders () const {return " PassengerType,Frequence,StartTime,EndTime,Capacity,TimeRange ";};	
	void initBulk();

	//find a proper bulk and return its index in m_vectorBulkInfo or return -1;
	int findFitBulk( const CMobileElemConstraint& _const);
	int findValuedBulk(ElapsedTime&time,long& nfreqIndex, int group,const CMobileElemConstraint& _const,const ElapsedTime&flyTime);
	void SortData();
};

#endif // !defined(AFX_PAXBULKINFO_H__C692A542_55DB_4067_B88B_C6E8877FB588__INCLUDED_)
