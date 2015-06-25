// StartDate.h: interface for the CStartDate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STARTDATE_H__7F1DAA6E_0C64_4FAC_BD1B_7DCD22D978F9__INCLUDED_)
#define AFX_STARTDATE_H__7F1DAA6E_0C64_4FAC_BD1B_7DCD22D978F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxdisp.h>
#include "elaptime.h"

class CStartDate  
{
public:

	CStartDate();
	virtual ~CStartDate();

	BOOL IsAbsoluteDate() const { return m_bAbsDate; }
	void SetAbsoluteDate( bool _bAbsDate ){ m_bAbsDate = _bAbsDate;	}

	void SetDate(int _year, int _month, int _day);
	void SetDate( const COleDateTime& _date ) { m_dtDate = _date; }

	const COleDateTime& GetDate() const { return m_dtDate; }


	// convert Elapsed time to the real date / time.
	void GetDateTime(const ElapsedTime& _t,		// input:
							 bool& _bAbsDate,			// output: tell if _date valid
							 COleDateTime& _date,		// output: give absolute date
							 int& _nDtIdx,				// output: date index
							 COleDateTime& _time ) const;		// output: time part.
	
	// read start date from string
	void ReadStartDate( CString& _str );

	// write start date to the str
	CString WriteStartDate() const;


	void GetRelativeTime( const COleDateTime& _inDate, const ElapsedTime& _inTime, ElapsedTime& _outTime) const;

	void GetElapsedTime(const bool _bAbsDate,			// input: tell if _date valid
							const COleDateTime& _date,		// input: give absolute date
							const int _nDtIdx,				// input: date index
							const COleDateTime& _time,		// input: time part.
							ElapsedTime& _t					// output:
						) const;

	CStartDate& operator=(const CStartDate& _rhs);
/*
	COleDateTime GetDateTime();
	int GetDay() const;
	int GetMonth() const;
	int GetYear() const;
//	BOOL GetRegularDateTime(const ElapsedTime& _inTime, CStartDate& _outDate, ElapsedTime& _outTime);
	const COleDateTime& GetDate() const;
	CStartDate(const COleDateTime& _date);


	CString PrintDate();
*/
protected:
	COleDateTime	m_dtDate;
	bool			m_bAbsDate;		// if true, use abs date, m_dtDate is valid
									// if false, no date infomation.
	void GetRelativeTime(const COleDateTime& _dt, ElapsedTime& _outTime) const;
};

#endif // !defined(AFX_STARTDATE_H__7F1DAA6E_0C64_4FAC_BD1B_7DCD22D978F9__INCLUDED_)
