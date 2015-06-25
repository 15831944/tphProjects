#ifndef ELAPSED_TIME_H
#define ELAPSED_TIME_H


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afx.h>
#include <cstdio>
#include <fstream>
#include <afxdisp.h>
#include "commondll.h"
#include "replace.h"

#define TimePrecision   100L         // time in 1/100th of a second
#define SCALE_TO_TICKS  0.182        // clock ticks per second
#define WholeDay (86400L)			 //=24*3600L

class COMMON_TRANSFER ElapsedTime
{
protected:
	long m_time;

public:

    // Constructors
    ElapsedTime () { m_time = 0l; }
    ElapsedTime (long seconds) { m_time = seconds*TimePrecision; }
    ElapsedTime (double seconds) { m_time = (long)(seconds*TimePrecision); }
    ElapsedTime (const ElapsedTime& t) { m_time = t.m_time; }

    // initializers
	ElapsedTime& operator = (float seconds){ m_time = (long)(seconds*TimePrecision); return *this; }
    ElapsedTime& operator = (long seconds)
        { m_time = seconds*TimePrecision; return *this; }
    ElapsedTime& operator = (const ElapsedTime& t)
        { m_time = t; return *this; }

    void set (long seconds) { m_time = seconds*TimePrecision; }
    void set (int hour, int minute, int second);
	void set (const COleDateTime& oleDateTime);
    ElapsedTime& setPrecisely (long hundSeconds) { m_time = hundSeconds; return *this; }
	long getPrecisely () const { return m_time; }

	COleDateTime ToOleDateTime();
    // increment operator
    void operator++ () { m_time += TimePrecision; }

	// arithmetic integer operators
    ElapsedTime operator + (long seconds) const;
    ElapsedTime operator - (long seconds) const;
    ElapsedTime operator * (long seconds) const;
    ElapsedTime operator / (long seconds) const;
    void operator += (long seconds) { m_time += (seconds*TimePrecision); }
    void operator += (float t) { m_time += (long)(t*TimePrecision); }
    void operator -= (long seconds) { m_time -= (seconds*TimePrecision); }
    void operator -= (float t) { m_time -= (long)(t*TimePrecision); }
    void operator *= (unsigned i) { m_time *= i; }
    void operator *= (float i) { m_time *= (long)i; }
	void operator /= (unsigned i) { m_time /= i; }
	

    // arithmetic ElapsedTime operators
    ElapsedTime operator + (const ElapsedTime& t) const;
    ElapsedTime operator - (const ElapsedTime& t) const;
    float operator / (const ElapsedTime& t) const
        { return (float)((double)m_time / (double)t.m_time); }
	
	double operator * (const ElapsedTime& t) const
        { return m_time/(double)TimePrecision * t.m_time/(double)TimePrecision ; }
    void operator += (const ElapsedTime& t) { m_time += t.m_time; }
    void operator -= (const ElapsedTime& t) { m_time -= t.m_time; }

	ElapsedTime& operator*=(double d){ m_time = static_cast<long>(m_time*d); return *this; };
	ElapsedTime operator*(double d)const{ return ElapsedTime().setPrecisely( static_cast<long>(m_time*d) ); }
   
	// relational ElapsedTime operators
    bool operator == (const ElapsedTime& t) const { return (m_time == t.m_time); }
	bool operator != (const ElapsedTime& t) const { return m_time != t.m_time; }
    bool operator < (const ElapsedTime& t) const { return (m_time < t.m_time); }    
    bool operator > (const ElapsedTime& t) const{ return (m_time > t.m_time); }
    bool operator >= (const ElapsedTime& t) const{ return m_time >= t.m_time; }
    bool operator <= (const ElapsedTime& t) const{ return m_time <= t.m_time; }

    // relational integer operators
    bool operator == (long sec) const { return (m_time == sec); }
    bool operator < (long seconds) const
        { return (m_time < (seconds*TimePrecision));};
    bool operator > (long seconds) const
        { return (m_time > (seconds*TimePrecision));};
    bool operator <= (long seconds) const
        { return (m_time <= (seconds*TimePrecision));};
	bool operator >= (long seconds) const
        { return (m_time >= (seconds*TimePrecision));};


    void printTime (char *str, int printSeconds = 1) const;
	CString printTime( int printSeconds = 1 ) const;
	CString PrintDateTime() const;
    bool SetTime (const char *str);
    operator long () const { return m_time; }
	operator double () const { return  ((double)m_time/TimePrecision); }
    int asSeconds (void) const
        { return m_time/TimePrecision; }
    int asMinutes (void) const
        { return m_time/TimePrecision/60l; }
    int asHours (void) const
        { return m_time/TimePrecision/3600l; }
    //void setHours (int hours) { m_time = hours * 3600l * TimePrecision; }
	 void setHours (float hours) { m_time = (long)(hours * 3600.0 * TimePrecision); }

    // returns m_time in 1/18.2 of a second
    long getTicks (void) const { return (long)(m_time * SCALE_TO_TICKS+0.5); }
    void setTicks (long ticks) { m_time = (long)(ticks / SCALE_TO_TICKS+0.5); }

    void getSystemTime (void);
    //void setSystemTime (void) const;

	int GetDay(void) const;
	void SetDay(int nDay);
	int GetHour(void) const;
	void SetHour(int nHour);
	int GetMinute(void) const;
	void SetMinute(int nMinute);
	int GetSecond(void) const;
	void SetSecond(int nSecond);

	static ElapsedTime Max(); 

	friend COMMON_TRANSFER stdostream&  operator << (stdostream& s, const ElapsedTime& t);
	friend COMMON_TRANSFER stdistream& operator >> (stdistream& s, ElapsedTime& t);
};

#endif
