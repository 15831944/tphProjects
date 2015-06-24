// ProcLabourCost.h: interface for the CProcLabourCost class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCLABOURCOST_H__7FAF742B_B296_4FFA_9EEB_558CC56F4179__INCLUDED_)
#define AFX_PROCLABOURCOST_H__7FAF742B_B296_4FFA_9EEB_558CC56F4179__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "inputsdll.h"

class INPUTS_TRANSFER CProcLabourCost  
{
public:
	CProcLabourCost();
	virtual ~CProcLabourCost();

public:
	// access
    int GetProcessorID(){ return m_nProcessorID; }
	int GetShiftPeriod(){ return m_nShiftPeriod; }
	double GetBasePerhr(){ return m_dBaseperhr; }
	double GetRegperhr(){ return m_dRegperhr; }
	double GetOTperhr(){ return m_dOTperhr; }

	void SetProcessorID( int _nProcessorID ){ m_nProcessorID = _nProcessorID; }
	void SetShiftPeriod( int _nShiftPeriod ){ m_nShiftPeriod = _nShiftPeriod; }
	void SetBasePerhr( double _dBaseperhr ){ m_dBaseperhr = _dBaseperhr; }
	void SetRegperhr( double _dRegperhr ){ m_dRegperhr = _dRegperhr; }
	void SetOTperhr( double _dOTperhr ){ m_dOTperhr = _dOTperhr; }

protected:
	int m_nProcessorID;
	int m_nShiftPeriod;
	double m_dBaseperhr;
	double m_dRegperhr;
	double m_dOTperhr;
};	

#endif // !defined(AFX_PROCLABOURCOST_H__7FAF742B_B296_4FFA_9EEB_558CC56F4179__INCLUDED_)
