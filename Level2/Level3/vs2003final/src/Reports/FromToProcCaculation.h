// FromToProcCaculation.h: interface for the CFromToProcCaculation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FROMTOPROCCACULATION_H__4C4340AE_CF08_4084_8A29_F2179CD76AA0__INCLUDED_)
#define AFX_FROMTOPROCCACULATION_H__4C4340AE_CF08_4084_8A29_F2179CD76AA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>


class CFromToProcCaculation  
{	
public:
	enum StateFlag { START_COMPUTE,END_COMPUTE,INVALID } ;
	class FROM_TO_PROC 
	{
	public:
		int m_iFromProc;
		int FromProcState;// the state of pax at the processor -- EntityEvents
		int m_iToProc;
		int ToProcState;// the state of pax at the processor 
		double m_dValue;
		StateFlag m_enFlag;
	};

protected:
	std::vector<FROM_TO_PROC>m_vValues;
public:
	CFromToProcCaculation();
	virtual ~CFromToProcCaculation();	
public:
	void InitData( int _iFromProc, int _iToProc );
	void AddValue( int _iProc , double _dValue, int enumState );
	std::vector<FROM_TO_PROC>& GetValues() { return m_vValues;	}
	void ClearAll(){ m_vValues.clear();	}
	void ResetAll();
	double GetTotalValue()const;

};

#endif // !defined(AFX_FROMTOPROCCACULATION_H__4C4340AE_CF08_4084_8A29_F2179CD76AA0__INCLUDED_)
