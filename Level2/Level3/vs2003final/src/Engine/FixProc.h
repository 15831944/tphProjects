// FixProc.h: interface for the FixProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIXPROC_H__406DD4F3_D100_4EC0_9DD5_0157903A70E9__INCLUDED_)
#define AFX_FIXPROC_H__406DD4F3_D100_4EC0_9DD5_0157903A70E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AirfieldProcessor.h"
#include "../Common/LatLong.h"

class FixProc : public AirfieldProcessor  
{
public:
	enum OperationType{ Cruise=0, Descend, Approach, ClimbOut,/*add new type here */ TypeCount}; 
	const static CString OperationTypeString[TypeCount];

	FixProc();
	virtual ~FixProc();
	//processor type
	virtual int getProcessorType (void) const { return FixProcessor; }
	//processor name
	virtual const char *getProcessorName (void) const { return "FIX"; }
	//processor i/o
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;

	//lower limit
	void SetLowerLimit(const DistanceUnit& _dLower){ m_dLowerLimit = _dLower; };
	DistanceUnit GetLowerLimit() const { return m_dLowerLimit; };
	//upper limit
	void SetUpperLimit(const DistanceUnit& _dUpper){ m_dUpperLimit =  _dUpper; }
	DistanceUnit GetUpperLimit() const { return m_dUpperLimit; }
	//number
	void SetNumber(int _nNumber) { m_nNumber = _nNumber; }
	int GetNumber() const { return m_nNumber; }

	virtual Processor* CreateNewProc();
	virtual bool CopyOtherData(Processor* _pDestProc);

	OperationType getOperationType(){return m_operateType; }
	void setOperationType(const OperationType & _opType){ m_operateType  = _opType; }

private:
	DistanceUnit m_dLowerLimit;
	DistanceUnit m_dUpperLimit;
	int m_nNumber;
	OperationType m_operateType;
	
public:
	CLatitude *m_pLatitude;
	CLongitude *m_pLongitude;

};

#endif // !defined(AFX_FIXPROC_H__406DD4F3_D100_4EC0_9DD5_0157903A70E9__INCLUDED_)
