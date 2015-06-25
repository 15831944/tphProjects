#pragma once

class PreciseTimer 
{
public:	

	PreciseTimer(){ bRunning = false; }
	virtual void Start() =0 ;
	virtual void Stop() = 0 ;
	virtual void Reset() = 0;

	virtual double getElapsedSeconds()const =0 ;	
	bool bRunning;

};


class Win32PreciseTimer : public PreciseTimer
{
public:
	Win32PreciseTimer(){ Reset(); }
	void Start();
	void Stop();

	void Reset();

	virtual double getElapsedSeconds()const;

	

protected:
	LARGE_INTEGER m_tStartTime;
	LARGE_INTEGER m_tElasped;


	static const LARGE_INTEGER dfFrequence;
};

#include <fstream>
#include <iostream>


class TextFileLog
{
public:
	TextFileLog(const char* nameFile):strFile(nameFile){}	

	void Reset(){ m_time.Reset(); }
	void FuncBegin(const CString& funcName);
	void FuncEnd();

public:
	CString strFile;
	Win32PreciseTimer m_time;
	std::ofstream outfile;
};